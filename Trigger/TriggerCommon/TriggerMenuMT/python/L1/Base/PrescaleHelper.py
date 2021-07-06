# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

"""
The prescaling uses a pseudo-random binary sequence generator of 24 bit width (31 bit internal)

- Prescales are defined by a cutoff (C).
- Each time a random number (R) is generated.
- C, R are between 0 and 2**24-1

- The trigger item is accepted if R>=C. The prescale (PS) corresponding to a cutoff C is PS = (2**24-1)/(2**24-C)


In this scheme only number that are powers of 2 can be represented
exactly. Effective prescales are typically non-integer.  Integer
numbers can~t be represented exactly but have an error which grows with the
prescale)
"""

maxPrescaleCut = 0xFFFFFF  # 2**24 - 1

def getCutFromPrescale(prescale):
    """
    C = 2**24-(2**24-1)/PS
    
    PS =       1 --> C = 1
    PS =       2 --> C = 8388609
    PS =      10 --> C = 15099495
    PS =    1000 --> C = 16760439
    PS =   10000 --> C = 16775539
    PS = 2**24-1 --> C = 2**24-1
    """

    if prescale==0:
        raise RuntimeError("L1 Prescale value of 0 is not allowed")
    
    sign = -1 if prescale<0 else 1
    prescale = abs(prescale)
    cut=sign * ( 0x1000000 - (0xFFFFFF/prescale) )
    cut = round( cut )
    if  prescale > 0xFFFFFF:
        cut=sign * (0x1000000-1)

    return int(cut)

def getPrescaleFromCut(cut):
    """
    PS = (2**24-1)/(2**24-C)

    C =       1 --> PS =     1
    C = 2**24-1 --> PS = 2**24-1
    """
    
    if cut==0:
        raise RuntimeError("L1 Prescale cut of 0 is not allowed, use cut=1 for a prescale value of 1")

    sign = -1 if cut<0 else 1
    cut = abs(cut)
    return (sign * 0xFFFFFF) / ( 0x1000000 - cut )
