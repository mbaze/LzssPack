# LzssPack

LzssPack is a simple command line packer primarily meant to be used for retro computing projects. It uses a simple compression scheme based on Lempel–Ziv–Storer–Szymanski algorithm. The stream format was originally devised by Pavel "Zilog" Cimbal who used it in his Sinclair ZX Spectrum compression utility called Thrasher.

## Stream Format

The stream uses byte-aligned markers in order to allow for an efficient implementation of the decoder on 8-bit architectures. There are two types of blocks, literals and offset-length pairs (phrases). Literals can contain up to 128 bytes. Phrases are up to 10 bytes long with offsets ranging from 1 to 4095 bytes.

The compressed stream is interpreted as follows (in binary):

<code>CCCCCCC1</code> - Copy the next <code>CCCCCCC + 1</code> bytes to the output.<br>
<code>CCCHHHH0 LLLLLLLL</code> - Copy <code>CCC + 3</code> bytes from the offset <code>-HHHHLLLLLLLL</code> relative to the current output stream position.<br>
<code>00000000 00000000</code> - End of stream marker.

## Sample Decoder

The following code is a Zilog Z80 implementation of the decoder (for reversed stream), created by Pavel "Zilog" Cimbal, Milos "Baze" Bazelides, Juraj "Hvge" Durech and Marian "Beetle" Bobrik.

<code>
Read    xor     a<br>
        rr      (hl)<br>
        ld      c,(hl)<br>
        rrd<br>
        ld      b,(hl)<br>
        dec     hl<br>
        jr      c,Copy<br>
        ld      c,a<br>
        or      (hl)<br>
        ret     z<br>
        push    hl<br>
        ld      l,(hl)<br>
        ld      h,c<br>
        add     hl,de<br>
        ld      c,b<br>
        inc     c<br>
        inc     c<br>
Copy    inc     c<br>
        ld      b,0<br>
        lddr<br>
        jr      c,Read<br>
        pop     hl<br>
        dec     hl<br>
        jr      Read<br>
</code>
