## LzssPack

LzssPack is a simple command line packer with focus on vintage computing platforms. It uses a simple byte-aligned compression scheme based on Lempel–Ziv–Storer–Szymanski algorithm. The format was devised by Pavel "Zilog" Cimbal in 1998 during development of his program Trasher for Sinclair ZX Spectrum. The compression ratio was decent for the time, though it doesn't really match the performace of contemporary utilities like LZSA, ZX2, LzxPack etc. Nevertheless, the format's simplicity makes it occasionally usable for quick on-the-fly decompression.

### Stream Format

The stream uses byte-aligned markers in order to allow for efficient decoder implementation on 8-bit architectures. There are two types of blocks, literals and offset-length pairs (phrases). Literals can contain up to 128 bytes. Phrases are 3 to 10 bytes long with offsets ranging from 1 to 4095 bytes. Zero offset indicates end of stream.

The compressed stream is interpreted as follows (in binary):

`CCCCCCC1` - Copy the next `CCCCCCC + 1` bytes to the output.

`CCCHHHH0 LLLLLLLL` - Copy `CCC + 3` bytes from the offset `HHHHLLLLLLLL` relative to the current output position.

`00000000 00000000` - End of stream.

### Sample Decoder

The following code is a Zilog Z80 implementation of the decoder (assuming reversed stream), created around the year 2000 by Pavel "Zilog" Cimbal with some help from demogroup 3SC (Milos "Baze" Bazelides, Juraj "Hvge" Durech and Marian "Beetle" Bobrik).

```
;       HL = source address
;       DE = destination address

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
