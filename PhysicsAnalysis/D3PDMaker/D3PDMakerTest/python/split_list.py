# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

def split_list1 (l, delim):
    """Split the string L at the character DELIM.
But don't split within groups nested in ([{}]) or in strings.

>>> split_list1 ('12345', ',')
['12345']
>>> split_list1 ('123, [45, 67], (89,[10,11],12), 13', ',')
['123', '[45, 67]', '(89,[10,11],12)', '13']
>>> split_list1 ('12, "34,56", 78', ',')
['12', '"34,56"', '78']
>>> dq='"'
>>> sq="'"
>>> bs='\\\\'
>>> len(bs)
1
>>> split_list1 ('12, "34,56", '+sq+'11,12'+sq+', 78', ',')
['12', '"34,56"', "'11,12'", '78']
>>> split_list1 ('12, "34,56'+bs+dq+',99", '+sq+'11,12'+sq+', 78', ',')
['12', '"34,56\\\\",99"', "'11,12'", '78']
>>> split_list1 ('12, "34,56'+bs+bs+dq+',99, '+sq+'11,12'+sq+', 78', ',')
['12', '"34,56\\\\\\\\"', '99', "'11,12'", '78']
"""
    i = 0
    out = []
    sz = len(l)
    while i < sz:
        nest = 0
        j = i
        while j < sz:
            c = l[j]
            if c == delim and nest == 0:
                break
            elif c in '[{(':
                nest += 1
            elif c in '])}':
                if nest == 0: break
                nest -= 1
            elif c in '\'"':
                j += 1
                esc = False
                while j < sz and not (l[j] == c and not esc):
                    if esc:
                        esc = False
                    elif l[j] == '\\':
                        esc = True
                    j += 1
            j += 1
        out.append (l[i:j])
        i = j+1
        while i < sz and l[i] == ' ':
            i += 1
    return out


def split_list (l):
    """L is a string representation of a list.  Split it at commas.
But don't split within groups nested in ([{}]) or in strings.

>>> split_list ('[12345]')
['12345']
>>> split_list ('[123,[45,67],(89,[10,11],12),13]')
['123', '[45,67]', '(89,[10,11],12)', '13']
"""
    return split_list1 (l[1:-1], ',')


if __name__ == '__main__':
    from PyUtils import coverage                          #pragma: NO COVER
    c = coverage.Coverage ('D3PDMakerTest.split_list')    #pragma: NO COVER
    c.doctest_cover ()                                    #pragma: NO COVER
