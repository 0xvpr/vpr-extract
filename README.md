# vpr-extract
This project aims to provide a tool for the extraction of the  
.text section of a COFF object file.

## Build instructions
```bash
git clone https://github.com/0xvpr/vpr-extract
cd vpr-extract
make
# sudo make install # uncomment if you want it available globally (only recommended for LINUX/WSL/MSYS environments)
```

## Example Usage
```bash
vpr-extract path/to/file-1 path/to/file-2
```

## Testing
```bash
make tests
```
