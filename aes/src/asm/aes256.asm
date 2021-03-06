; Copyright (c) 2015 Egor Tensin <Egor.Tensin@gmail.com>
; This file is part of the "AES tools" project.
; For details, see https://github.com/egor-tensin/aes-tools.
; Distributed under the MIT License.

.586
.xmm
.model flat

.code

@aes_AES256_encrypt_block_@20 proc
    pxor xmm0, [ecx]
    aesenc xmm0, [ecx + 10h]
    aesenc xmm0, [ecx + 20h]
    aesenc xmm0, [ecx + 30h]
    aesenc xmm0, [ecx + 40h]
    aesenc xmm0, [ecx + 50h]
    aesenc xmm0, [ecx + 60h]
    aesenc xmm0, [ecx + 70h]
    aesenc xmm0, [ecx + 80h]
    aesenc xmm0, [ecx + 90h]
    aesenc xmm0, [ecx + 0A0h]
    aesenc xmm0, [ecx + 0B0h]
    aesenc xmm0, [ecx + 0C0h]
    aesenc xmm0, [ecx + 0D0h]
    aesenclast xmm0, [ecx + 0E0h]
    ret
@aes_AES256_encrypt_block_@20 endp

@aes_AES256_decrypt_block_@20 proc
    pxor xmm0, [ecx]
    aesdec xmm0, [ecx + 10h]
    aesdec xmm0, [ecx + 20h]
    aesdec xmm0, [ecx + 30h]
    aesdec xmm0, [ecx + 40h]
    aesdec xmm0, [ecx + 50h]
    aesdec xmm0, [ecx + 60h]
    aesdec xmm0, [ecx + 70h]
    aesdec xmm0, [ecx + 80h]
    aesdec xmm0, [ecx + 90h]
    aesdec xmm0, [ecx + 0A0h]
    aesdec xmm0, [ecx + 0B0h]
    aesdec xmm0, [ecx + 0C0h]
    aesdec xmm0, [ecx + 0D0h]
    aesdeclast xmm0, [ecx + 0E0h]
    ret
@aes_AES256_decrypt_block_@20 endp

@aes_AES256_expand_key_@36 proc
    ; A "word" (in terms of the FIPS 187 standard) is a 32-bit block.
    ; Words are denoted by `w[N]`.
    ;
    ; A key schedule is composed of 14 "regular" keys and a dumb key for
    ; the "whitening" step.
    ;
    ; A key schedule is thus composed of 60 "words".
    ; The FIPS standard includes an algorithm to calculate these words via
    ; a simple loop:
    ;
    ; i = 8
    ; while i < 60:
    ;     temp = w[i - 1]
    ;     if i % 8 == 0:
    ;         temp = SubWord(RotWord(temp))^Rcon
    ;     elif i % 8 == 4:
    ;         temp = SubWord(temp)
    ;     w[i] = w[i - 8]^temp
    ;     i = i + 1
    ;
    ; The loop above may be unrolled like this:
    ;
    ; w[8] = SubWord(RotWord(w[7]))^Rcon^w[0]
    ; w[9] = w[8]^w[1]
    ;      = SubWord(RotWord(w[7]))^Rcon^w[1]^w[0]
    ; w[10] = w[9]^w[2]
    ;       = SubWord(RotWord(w[7]))^Rcon^w[2]^w[1]^w[0]
    ; w[11] = w[10]^w[3]
    ;       = SubWord(RotWord(w[7]))^Rcon^w[3]^w[2]^w[1]^w[0]
    ; w[12] = SubWord(w[11])^w[4]
    ; w[13] = w[12]^w[5]
    ;       = SubWord(w[11])^w[5]^w[4]
    ; w[14] = w[13]^w[6]
    ;       = SubWord(w[11])^w[6]^w[5]^w[4]
    ; w[15] = w[14]^w[7]
    ;       = SubWord(w[11])^w[7]^w[6]^w[5]^w[4]
    ; w[16] = SubWord(RotWord(w[15]))^Rcon^w[8]
    ; w[17] = w[16]^w[9]
    ;       = SubWord(RotWord(w[15]))^Rcon^w[9]^w[8]
    ; w[18] = w[17]^w[10]
    ;       = SubWord(RotWord(w[15]))^Rcon^w[10]^w[9]^w[8]
    ; w[19] = w[18]^w[11]
    ;       = SubWord(RotWord(w[15]))^Rcon^w[11]^w[10]^w[9]^w[8]
    ; w[20] = SubWord(w[19])^w[12]
    ; w[21] = w[20]^w[13]
    ;       = SubWord(w[19])^w[13]^w[12]
    ; w[22] = w[21]^w[14]
    ;       = SubWord(w[19])^w[14]^w[13]^w[12]
    ; w[23] = w[22]^w[15]
    ;       = SubWord(w[19])^w[15]^w[14]^w[13]^w[12]
    ;
    ; ... and so on.
    ;
    ; The Intel AES-NI instruction set facilitates calculating SubWord
    ; and RotWord using `aeskeygenassist`, which is used in this routine.
    ;
    ; Preconditions:
    ; * xmm1[127:96] == w[7],
    ; * xmm1[95:64]  == w[6],
    ; * xmm1[63:32]  == w[5],
    ; * xmm1[31:0]   == w[4],
    ; * xmm0[127:96] == w[3],
    ; * xmm0[95:64]  == w[2],
    ; * xmm0[63:32]  == w[1],
    ; * xmm0[31:0]   == w[0].

    movdqa [ecx], xmm0          ; sets w[0], w[1], w[2], w[3]
    movdqa [ecx + 10h], xmm1    ; sets w[4], w[5], w[6], w[7]
    lea ecx, [ecx + 20h]        ; ecx = &w[8]

    aeskeygenassist xmm5, xmm1, 1h     ; xmm5[127:96] = RotWord(SubWord(w[7]))^Rcon
    pshufd xmm5, xmm5, 0FFh            ; xmm5[95:64] = xmm5[63:32] = xmm5[31:0] = xmm5[127:96]
    call aes256_keygen_assist          ; sets w[8], w[9], w[10], w[11]

    aeskeygenassist xmm5, xmm1, 0      ; xmm5[95:64] = SubWord(w[11])
    pshufd xmm5, xmm5, 0AAh            ; xmm5[127:96] = xmm5[63:32] = xmm5[31:0] = xmm5[95:64]
    call aes256_keygen_assist          ; sets w[12], w[13], w[14], w[15]

    aeskeygenassist xmm5, xmm1, 2h     ; xmm5[127:96] = RotWord(SubWord(w[15]))^Rcon
    pshufd xmm5, xmm5, 0FFh            ; xmm5[95:64] = xmm5[63:32] = xmm5[31:0] = xmm5[127:96]
    call aes256_keygen_assist          ; sets w[16], w[17], w[18], w[19]

    aeskeygenassist xmm5, xmm1, 0      ; xmm5[95:64] = SubWord(w[19])
    pshufd xmm5, xmm5, 0AAh            ; xmm5[127:96] = xmm5[63:32] = xmm5[31:0] = xmm5[95:64]
    call aes256_keygen_assist          ; sets w[20], w[21], w[22], w[23]

    aeskeygenassist xmm5, xmm1, 4h     ; xmm5[127:96] = RotWord(SubWord(w[23]))^Rcon
    pshufd xmm5, xmm5, 0FFh            ; xmm5[95:64] = xmm5[63:32] = xmm5[31:0] = xmm5[127:96]
    call aes256_keygen_assist          ; sets w[24], w[25], w[26], w[27]

    aeskeygenassist xmm5, xmm1, 0      ; xmm5[95:64] = SubWord(w[27])
    pshufd xmm5, xmm5, 0AAh            ; xmm5[127:96] = xmm5[63:32] = xmm5[31:0] = xmm5[95:64]
    call aes256_keygen_assist          ; sets w[28], w[29], w[30], w[31]

    aeskeygenassist xmm5, xmm1, 8h     ; xmm5[127:96] = RotWord(SubWord(w[31]))^Rcon
    pshufd xmm5, xmm5, 0FFh            ; xmm5[95:64] = xmm5[63:32] = xmm5[31:0] = xmm5[127:96]
    call aes256_keygen_assist          ; sets w[32], w[33], w[34], w[35]

    aeskeygenassist xmm5, xmm1, 0      ; xmm5[95:64] = SubWord(w[35])
    pshufd xmm5, xmm5, 0AAh            ; xmm5[127:96] = xmm5[63:32] = xmm5[31:0] = xmm5[95:64]
    call aes256_keygen_assist          ; sets w[36], w[37], w[38], w[39]

    aeskeygenassist xmm5, xmm1, 10h    ; xmm5[127:96] = RotWord(SubWord(w[39]))^Rcon
    pshufd xmm5, xmm5, 0FFh            ; xmm5[95:64] = xmm5[63:32] = xmm5[31:0] = xmm5[127:96]
    call aes256_keygen_assist          ; sets w[40], w[41], w[42], w[43]

    aeskeygenassist xmm5, xmm1, 0      ; xmm5[95:64] = SubWord(w[43])
    pshufd xmm5, xmm5, 0AAh            ; xmm5[127:96] = xmm5[63:32] = xmm5[31:0] = xmm5[95:64]
    call aes256_keygen_assist          ; sets w[44], w[45], w[46], w[47]

    aeskeygenassist xmm5, xmm1, 20h    ; xmm5[127:96] = RotWord(SubWord(w[47]))^Rcon
    pshufd xmm5, xmm5, 0FFh            ; xmm5[95:64] = xmm5[63:32] = xmm5[31:0] = xmm5[127:96]
    call aes256_keygen_assist          ; sets w[48], w[49], w[50], w[51]

    aeskeygenassist xmm5, xmm1, 0      ; xmm5[95:64] = SubWord(w[51])
    pshufd xmm5, xmm5, 0AAh            ; xmm5[127:96] = xmm5[63:32] = xmm5[31:0] = xmm5[95:64]
    call aes256_keygen_assist          ; sets w[52], w[53], w[54], w[55]

    aeskeygenassist xmm5, xmm1, 40h    ; xmm5[127:96] = RotWord(SubWord(w[55]))^Rcon
    pshufd xmm5, xmm5, 0FFh            ; xmm5[95:64] = xmm5[63:32] = xmm5[31:0] = xmm5[127:96]
    call aes256_keygen_assist          ; sets w[56], w[57], w[58], w[59]

    ret

aes256_keygen_assist:
    ; Preconditions:
    ; * xmm1[127:96] == w[i+7],
    ; * xmm1[95:64]  == w[i+6],
    ; * xmm1[63:32]  == w[i+5],
    ; * xmm1[31:0]   == w[i+4],
    ; * xmm0[127:96] == w[i+3],
    ; * xmm0[95:64]  == w[i+2],
    ; * xmm0[63:32]  == w[i+1],
    ; * xmm0[31:0]   == w[i],
    ; * xmm5[127:96] == xmm5[95:64] == xmm5[63:32] == xmm5[31:0] == HWGEN,
    ;   where HWGEN is either RotWord(SubWord(w[i+7]))^Rcon or SubWord(w[i+7]),
    ;   depending on the number of the round being processed,
    ; * ecx == &w[i+8].
    ;
    ; Postconditions:
    ; * xmm1[127:96] == w[i+11] == HWGEN^w[i+3]^w[i+2]^w[i+1]^w[i],
    ; * xmm1[95:64]  == w[i+10] == HWGEN^w[i+2]^w[i+1]^w[i],
    ; * xmm1[63:32]  == w[i+9]  == HWGEN^w[i+1]^w[i],
    ; * xmm1[31:0]   == w[i+8]  == HWGEN^w[i],
    ; * xmm0[127:96] == w[i+7],
    ; * xmm0[95:64]  == w[i+6],
    ; * xmm0[63:32]  == w[i+5],
    ; * xmm0[31:0]   == w[i+4],
    ; * ecx == &w[i+12],
    ; * the value in xmm4 is also modified.

    ; Calculate
    ;     w[i+3]^w[i+2]^w[i+1]^w[i],
    ;     w[i+2]^w[i+1]^w[i],
    ;     w[i+1]^w[i] and
    ;     w[i].
    movdqa xmm4, xmm0    ; xmm4 = xmm0
    pslldq xmm4, 4       ; xmm4 <<= 32
    pxor xmm0, xmm4      ; xmm0 ^= xmm4
    pslldq xmm4, 4       ; xmm4 <<= 32
    pxor xmm0, xmm4      ; xmm0 ^= xmm4
    pslldq xmm4, 4       ; xmm4 <<= 32
    pxor xmm0, xmm4      ; xmm0 ^= xmm4
                         ; xmm0[127:96] == w[i+3]^w[i+2]^w[i+1]^w[i]
                         ; xmm0[95:64]  == w[i+2]^w[i+1]^w[i]
                         ; xmm0[63:32]  == w[i+1]^w[i]
                         ; xmm0[31:0]   == w[i]

    ; Calculate
    ;     HWGEN^w[i+3]^w[i+2]^w[i+1]^w[i],
    ;     HWGEN^w[i+2]^w[i+1]^w[i],
    ;     HWGEN^w[i+1]^w[i] and
    ;     HWGEN^w[i].
    pxor xmm0, xmm5    ; xmm0 ^= xmm5
                       ; xmm0[127:96] == w[i+11] == HWGEN^w[i+3]^w[i+2]^w[i+1]^w[i]
                       ; xmm0[95:64]  == w[i+10] == HWGEN^w[i+2]^w[i+1]^w[i]
                       ; xmm0[63:32]  == w[i+9]  == HWGEN^w[i+1]^w[i]
                       ; xmm0[31:0]   == w[i+8]  == HWGEN^w[i]

    ; Set w[i+8], w[i+9], w[i+10] and w[i+11].
    movdqa [ecx], xmm0    ; w[i+8]  = HWGEN^w[i]
                          ; w[i+9]  = HWGEN^w[i+1]^w[i]
                          ; w[i+10] = HWGEN^w[i+2]^w[i+1]^w[i]
                          ; w[i+11] = HWGEN^w[i+3]^w[i+2]^w[i+1]^w[i]
    add ecx, 10h          ; ecx = &w[i+12]

    ; Swap the values in xmm0 and xmm1.
    pxor xmm0, xmm1
    pxor xmm1, xmm0
    pxor xmm0, xmm1

    ret
@aes_AES256_expand_key_@36 endp

@aes_AES256_derive_decryption_keys_@8 proc
    movdqa xmm5, [ecx]
    movdqa xmm4, [ecx + 0E0h]
    movdqa [edx], xmm4
    movdqa [edx + 0E0h], xmm5

    aesimc xmm5, [ecx + 10h]
    aesimc xmm4, [ecx + 0D0h]
    movdqa [edx + 10h], xmm4
    movdqa [edx + 0D0h], xmm5

    aesimc xmm5, [ecx + 20h]
    aesimc xmm4, [ecx + 0C0h]
    movdqa [edx + 20h], xmm4
    movdqa [edx + 0C0h], xmm5

    aesimc xmm5, [ecx + 30h]
    aesimc xmm4, [ecx + 0B0h]
    movdqa [edx + 30h], xmm4
    movdqa [edx + 0B0h], xmm5

    aesimc xmm5, [ecx + 40h]
    aesimc xmm4, [ecx + 0A0h]
    movdqa [edx + 40h], xmm4
    movdqa [edx + 0A0h], xmm5

    aesimc xmm5, [ecx + 50h]
    aesimc xmm4, [ecx + 90h]
    movdqa [edx + 50h], xmm4
    movdqa [edx + 90h], xmm5

    aesimc xmm5, [ecx + 60h]
    aesimc xmm4, [ecx + 80h]
    movdqa [edx + 60h], xmm4
    movdqa [edx + 80h], xmm5

    aesimc xmm5, [ecx + 70h]
    movdqa [edx + 70h], xmm5

    ret
@aes_AES256_derive_decryption_keys_@8 endp

end
