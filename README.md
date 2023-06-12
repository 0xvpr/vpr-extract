# vpr-extract
This project aims to provide a tool for the extraction of the  
.text section of a COFF object file.

## Build Instructions (Using CMake)
```bash
git clone https://github.com/0xvpr/vpr-extract && cd vpr-extract
cmake -B build
cmake --build build
# sudo cmake --build build --target install # uncomment for local install
```

## Example Usage
```bash
vpr-extract path/to/file-1.obj path/to/file-2.obj
#
# Target: path/to/file-1.obj -> file-1.bin
# Size of path/to/file-1.obj:      420
# Written to 'file-1.bin'.
# 
# Target: path/to/file-2.obj -> file-2.bin
# Size of path/to/file-2.obj:      69
# Written to 'file-2.bin'.
#
```
