# LzssPack

LzssPack is a simple command line packer primarily meant to be used for retro computing projects. It uses a simple compression scheme based on Lempel–Ziv–Storer–Szymanski algorithm. The stream format was originally devised by Pavel "Zilog" Cimbal who used it in his Sinclair ZX Spectrum compression utility called Thrasher.

## Stream Format

The stream uses byte-aligned markers in order to allow for an efficient implementation of the decoder on 8-bit architectures. There are two types of blocks, literals and offset-length pairs (phrases). Literals can contain up to 128 bytes. Phrases are up to 10 bytes long with offsets ranging from 1 to 4095 bytes.

The compressed stream is interpreted as follows (in binary):

<code>CCCCCCC1</code> - Copy the next CCCCCCC + 1 bytes to the output.
<code>CCCHHHH0 LLLLLLLL</code> - Copy CCC + 3 bytes from the offset -HHHHLLLLLLLL relative to the current output stream position.
<code>00000000 00000000</code> - End of stream marker.

## Sample Decoder

The following code is a Zilog Z80 implementation of the decoder (for reversed stream), created by Pavel "Zilog" Cimbal, Milos "Baze" Bazelides, Juraj "Hvge" Durech and Marian "Beetle" Bobrik.

<code>
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
</code>
