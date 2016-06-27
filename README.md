# mk-bin

A stupid program to create a binary file specific platforms.
I needed this for testing uboot on some boards.

### Usage

```bash
mk-bin [OPTION]
Create a binary!
    -e, --endian   endianess[Either 'little' or 'big']
    -s, --size     size of the binary
    -n, --noop     fill the binary with noops
    -f, --file     name of the output binary file
    -h, --help     print this help
```

You should be able to do something like this:

```bash
./mk-bin -s 100 -f foo.bin -e big
```

or

```bash
./mk-bin -s 100 -f foo.bin -e little
```