# LzssPack

LzssPack is a simple command line packer primarily meant to be used for retro computing projects.
It uses a simple compression scheme based on Lempel–Ziv–Storer–Szymanski algorithm. The stream
format was originally devised by Pavel "Zilog" Cimbal who used it in his Sinclair ZX Spectrum
compression utility called Thrasher.

## Stream Format

The stream uses byte-aligned markers in order to enable efficient implementation of the decoder
on 8-bit architectures. There are two types of blocks, literal and offset-length pair (phrase).
Literals can contain up to 128 bytes. Phrases are up to 10 bytes long with offsets ranging from
1 to 4095 bytes.

The compressed stream is interpreted as follows (in binary):

CCCCCCC1

Copy the next CCCCCCC + 1 bytes to the output.

CCCHHHH0 LLLLLLLL

Copy CCC + 3 bytes from the offset -HHHHLLLLLLLL relative to the current output stream position.

00000000 00000000

End of stream marker.
 