#!/bin/env python

# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

""" Encodes a bit string composed mostly by zeroes"""

_B64_alphabet = ("ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                 "abcdefghijklmnopqrstuvwxyz"
                 "0123456789+/")


def _toB64(n):
    s = ""
    while n > 63:
        s = _B64_alphabet[n % 64] + s
        n = n//64
    s = _B64_alphabet[n % 64] + s
    return s


def _fromB64(s):
    n = 0
    for i in range(0, len(s)):
        n *= 64
        n += _B64_alphabet.index(s[i])
    return n


def compressB64(s):
    """Encodes a string of zeroes and ones using a RLE method.

    s is the string to encode.

    Consecutive zeroes are encoded by its repetition count in Base64.
    Ones are copied to output substituted by exclamation marks.

    Example:
           input:   0001100001001101010000
           output:  D!!E!C!!B!B!E

    Effective compression is only achieved when input is composed mostly
    by zeroes.

    The encoded string is returned.
    """

    count = 0
    res = ""
    for k in range(0, len(s)):
        c = s[k]
        if c == '0':
            count += 1
        else:
            if count > 0:
                res += _toB64(count)
            res += "!"
            count = 0

    if count > 0:
        res += _toB64(count)

    return res


def decompressB64(s):
    """Decodes a string coded by compressB().

    s is the string to decode.

    The decoded string is returned.
    """

    res = ""
    count = ""
    for k in range(0, len(s)):
        c = s[k]
        if c == "!" or c == " ":
            if len(count) > 0:
                res += "0" * _fromB64(count)
                count = ""
            res += c.replace("!", "1").replace(" ", "0")
        else:
            count += c
    if len(count) > 0:
        res += "0" * _fromB64(count)

    return res


if __name__ == "__main__":

    s = "000010000000000010000001000000000001000000000010000001"
    c = compressB64(s)
    d = decompressB64(c)
    x = int(s, 2)
    sx = "{:0X}".format(x)

    print("")
    print("Original:     >>"+s+"<<")
    print("Compressed:   >>"+c+"<<")
    print("Hex repr:     >>"+sx+"<<")
    print("Decompressed: >>"+d+"<<")

    import random
    for i in range(0, 100):
        # generate random bit string
        s = ""
        for j in range(0, 1000):
            if random.random() > 0.02:
                s += '0'
            else:
                s += '1'

        # test compression and decompression
        c = compressB64(s)
        d = decompressB64(c)
        x = int(s, 2)
        sx = "{:0X}".format(x)

        if i == 0:
            print("")
            print("Original:     >>"+s+"<<")
            print("Compressed:   >>"+c+"<<")
            print("Hex repr:     >>"+sx+"<<")
            print("Decompressed: >>"+d+"<<")

        if s != d:
            print("")
            print("Decompression error")
            print("Original:     >>"+s+"<<")
            print("Compressed:   >>"+c+"<<")
            print("Hex repr:     >>"+sx+"<<")
            print("Decompressed: >>"+d+"<<")
