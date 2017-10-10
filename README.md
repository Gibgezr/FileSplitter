# FileSplitter
simple and fast file splitter/rejoiner

A bare-bones file splitter and joiner, useful for splitting giant files into chunks that fat32 filesystems can handle. 
I needed this when copying a 6GB file onto a USB drive to move it via sneakernet to another machine.
The utilities I found for doing this were incredibly slow and didn't report errors properly, so I wrote one quickly.
Command line only. No double buffering or threads (yet!), but now that I'm not necessarily reading/writing on same disk those features might get added if I get bored.
Cross-platform, use a modern C++ compiler like VS 2017 and the fstream stuff will be VERY fast.
I thought about using C file IO, but on a newer C++ compiler fstream can outperform it; certianly this app is fast enough for me.
I use a 512KB read/write chunk size; this works well for me.
