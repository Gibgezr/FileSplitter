#include<iostream>
#include<fstream>
#include<string>
#include<vector>

void Usage(char *argv0)
{
	std::cout << "USAGE:\n";
	std::cout << argv0 << " <switches> <filename>\n";
	std::cout << "Where switches is one of:\n";
	std::cout << "-s (split the file into 3.5 GB chunks)\n";
	std::cout << "-sX (split the file into X MB chunks)\n";
	std::cout << "-j (join the file)\n";
	std::cout << "And filename is either the file to be split,\n";
	std::cout << "Or the .rip file to be joined togethor.\n\n";

	exit(-1);
}

void JoinFiles(std::string firstFile)
{
	//make a buffer to read chunks into
	char data[512 * 1024];

	//load the .rip file as text and parse it
	std::vector<std::string> fileNames;
	auto ripFile = std::fstream(firstFile, std::ios::in);
	if (!ripFile.is_open())
	{
		std::cout << "Cannot open .rip file " << firstFile << " for processing.\n";
		exit(-2);
	}

	while (!ripFile.eof())
	{
		std::string f;
		std::getline(ripFile, f);
		fileNames.push_back(f);
	}
	
	ripFile.close();

	auto outFile = std::fstream(fileNames[0], std::ios::out | std::ios::binary | std::ios::trunc);
	for (int i = 1; i < fileNames.size(); ++i)
	{
		auto inFile = std::fstream(fileNames[i], std::ios::in | std::ios::binary);

		if (!inFile.is_open())
		{
			std::cout << "Cannot open input file " << fileNames[i] << " for processing.\n";
			outFile.close(); //delete outfile???
			exit(-2);
		}

		//read chunks from infile, write out to outfile
		while (true)
		{
			//read a chunk
			inFile.read((char*)&data[0], 512 * 1024);
			size_t extracted = inFile.gcount();
			if (extracted < 1) break;

			outFile.write((char*)&data[0], extracted);
			
			if (outFile.fail())
			{
				//Probably ran out of space
				std::cout << "Error writing to file " << fileNames[0] << ".\nMaybe disk is full?\n";
				outFile.close();
				inFile.close();
				exit(-10);
			}
		}

		inFile.close();		
	}

	outFile.close();
}

void SplitFile(std::string fileName, size_t fileSize = 3500)
{
	fileSize *= 1024 * 1024;
	//make a buffer to read chunks into
	char data[512 * 1024];

	//parse the input filename
	std::string ripFileName;
	for (int i = 0; i < fileName.size(); ++i)
	{
		if (fileName[i] == '.') break;
		ripFileName += fileName[i];
	}

	auto ripFile = std::fstream(ripFileName + ".rip", std::ios::out);

	ripFile << fileName << "\n"; //first line is original file name

	auto inFile = std::fstream(fileName, std::ios::in | std::ios::binary );

	if (!inFile.is_open())
	{
		std::cout << "Cannot open input file " << fileName << " for processing.\n";
		ripFile.close(); //delete ripFile???
		exit(-2);
	}

	int counter = 0;

	bool doneAll = false;
	while (!doneAll)
	{
		//create a file to write out to
		std::string outFileName = ripFileName + '.';
		if (counter < 100) outFileName += '0';
		if (counter < 10) outFileName += '0';
		outFileName += std::to_string(counter);

		auto outFile = std::fstream(outFileName, std::ios::out | std::ios::binary | std::ios::trunc);
		counter++;

		ripFile << outFileName << "\n"; //add this output file to the rip file listing

		//loop while reading data from inFile into buffer and writing back out to outFile
		size_t totalWritten = 0;

		while (true)
		{
			//read a chunk
			inFile.read((char*)&data[0], 512 * 1024);
			size_t extracted = inFile.gcount();
			if (extracted < 1)
			{
				doneAll = true; //finished processing!
				break;
			}

			outFile.write((char*)&data[0], extracted);

			if (outFile.fail())
			{
				//Probably ran out of space
				std::cout << "Error writing to file " << outFileName << ".\nMaybe disk is full?\n";
				outFile.close();
				inFile.close();
				ripFile.close();
				exit(-10);
			}

			totalWritten += extracted;
			if (totalWritten >= fileSize) break;
		}

		outFile.close();

	}

	inFile.close();
	ripFile.close();
}

int main(int argc, char **argv)
{
	if (argc != 3) Usage(argv[0]);
	if(argv[1][0] != '-') Usage(argv[0]);
	switch (argv[1][1])
	{
	case 'j':
		std::cout << "Joining files from " << argv[2] << ", wait one moment please..." << std::endl;
		JoinFiles(argv[2]);
		std::cout << "Files joined.\n";
		break;
	case 's':
	{
		std::cout << "Splitting " << argv[2] << " into multiple smaller files, wait one moment please..." << std::endl;
		std::string split = argv[1];
		if (split.size() == 2) SplitFile(argv[2]);
		else
		{
			split.erase(0, 2);
			size_t mb = std::stoi(split);
			SplitFile(argv[2], mb);
		}

		std::cout << "Files split.\n";
	}
		break;
	default:
		Usage(argv[0]);
		break;
	}

	return 0;
}