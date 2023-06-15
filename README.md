<h1 align="center">vpr-extract</h1>
<p align="center">
  <img src="https://img.shields.io/badge/Windows--x86__64-supported-green">
  <img src="https://img.shields.io/badge/Windows--x86-supported-green">
  <img src="https://img.shields.io/badge/Linux--x86__64-supported-green">
  <img src="https://img.shields.io/badge/Linux--x86-unsupported-red">
  <a href="https://mit-license.org/">
    <img src="https://img.shields.io/github/license/0xvpr/vpr-extract?style=flat-square">
  </a>
  <br>
  <h3 align="center">A command line tool for the extraction of the .text section of 32/64-bit COFF objects, and ELF64 Relocatable objects.</h3>
  <br>
</p>

## Build Instructions
### Using Docker CMake (Recommended)
```bash
git clone https://github.com/0xvpr/vpr-extract && cd vpr-extract
make docker-container
make docker-build
```
### Using Local CMake (Unix)
```bash
git clone https://github.com/0xvpr/vpr-extract && cd vpr-extract
make local-build
```
### Using Local CMake (Windows)
```powershell
git clone https://github.com/0xvpr/vpr-extract && cd vpr-extract
cmake.exe -B build
cmake.exe --build build
```

### Example Usage
```bash
vpr-extract path/to/file-1.obj path/to/file-2.obj

Target: path/to/file-1.obj -> file-1.bin
Size of path/to/file-1.obj:      420
Written to 'file-1.bin'.

Target: path/to/file-2.obj -> file-2.bin
Size of path/to/file-2.obj:      69
Written to 'file-2.bin'.
```
