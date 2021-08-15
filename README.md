## LzssPack

LzssPack is a simple command line utility primarily meant as a cross-platform packer for retro computing platforms. It uses a simple compression scheme based on Lempel–Ziv–Storer–Szymanski algorithm. The format was originally devised by Pavel "Zilog" Cimbal who released it as part of Thrasher utility for Sinclair ZX Spectrum. Although there are more efficient compression schemes in use even on vintage platforms, the method strikes a good balance between compression ratio, decoder simplicity and speed. It is therefore suitable for on-the-fly decompression e.g. during loading.

### Stream Format

The stream uses byte-aligned markers in order to allow for efficient decoder implementation on 8-bit architectures. There are two types of blocks, literals and offset-length pairs (phrases). Literals can contain up to 128 bytes. Phrases are 3 to 10 bytes long with offsets ranging from 1 to 4095 bytes.

The compressed stream is interpreted as follows (in binary):

`CCCCCCC1` - Copy the next `CCCCCCC + 1` bytes to the output.

`CCCHHHH0 LLLLLLLL` - Copy `CCC + 3` bytes from the offset `HHHHLLLLLLLL` relative to the current output stream position.

`00000000 00000000` - End of stream marker.

### Sample Decoder

The following code is a Zilog Z80 implementation of the decoder (for reversed stream), created by Pavel "Zilog" Cimbal, Milos "Baze" Bazelides, Juraj "Hvge" Durech and Marian "Beetle" Bobrik.

```
Read    xor     a
        rr      (hl)
        ld      c,(hl)
        rrd
        ld      b,(hl)
        dec     hl
        jr      c,Copy
        ld      c,a
        or      (hl)
        ret     z
        push    hl
        ld      l,(hl)
        ld      h,c
        add     hl,de
        ld      c,b
        inc     c
        inc     c
Copy    inc     c
        ld      b,0
        lddr
        jr      c,Read
        pop     hl
        dec     hl
        jr      Read
```
