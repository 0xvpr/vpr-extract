# vpr-extract
This project aims to provide a tool for the extraction of the  
.text section of a COFF object file.

## Build Instructions
```bash
git clone https://github.com/0xvpr/vpr-extract
cd vpr-extract
make
# sudo make install # uncomment if you want it available globally (only recommended for LINUX/WSL/MSYS environments)
```

## Example Usage
```bash
vpr-extract path/to/file-1 path/to/file-2
#
# Target: path/to/file-1 -> out-0.bin
# Size of path/to/file-1:      420
# Written to 'out-0.bin'.
# 
# Target: path/to/file-2 -> out-1.bin
# Size of path/to/file-2:      69
# Written to 'out-1.bin'.
#
```

## Testing
```bash
make tests
```
