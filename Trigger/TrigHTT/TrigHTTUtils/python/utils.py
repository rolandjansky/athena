# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

'''
@file utils.py
@author Riley Xu - riley.xu@cern.ch
@date September 18, 2020
@brief Small utility functions
'''


def human_readable(n):
    '''Returns n as a human-readable string'''
    nz = 0 # number of trailing 0s
    while n % (10**(nz+1)) == 0:
        nz += 1

    nd = 1 # number of digits
    while n / 10**nd > 0:
        nd += 1

    # Display max 3 digits
    if nd - nz > 3:
        nz = nd - 3

    if nd > 9:
        base = 9
        tail = 'B'
    elif nd > 6:
        base = 6
        tail = 'M'
    elif nd > 3:
        base = 3
        tail = 'K'
    else:
        base = 0
        tail = ''

    if nz >= base:
        disp = str(n / 10**base)
    else:
        disp = str(round(float(n) / 10**base, base - nz))
    return disp + tail
