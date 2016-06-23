# mk-noop-bin

A stupid program to create a binary file with a bunch of NOOP's for specific platforms.
I needed this for testing uboot on some boards.

### Usage

```bash
mk-noop-bin [OPTION]
Create a binary of some size filled with NOOPS!
    -e, --endian   endianess[Either 'little'(default) or 'big']
    -s, --size     size of the binary
    -f, --file     name of the output binary file
    -h, --help     print this help
    -v, --version  increase verbosity of output
```

You should be able to do something like this:

```bash
./mk-noop-bin -s 100 -f foo.bin -e big
```

or

```bash
./mk-noop-bin -s 100 -f foo.bin -e little
```