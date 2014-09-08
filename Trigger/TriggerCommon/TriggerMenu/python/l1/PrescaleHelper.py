# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
The prescaling uses a pseudo-random binary sequence generator of 24 bit width (31 bit internal)

- Prescales are defined by a cutoff (C).
- Each time a random number (R) is generated.
- C, R are between 0 and 2**24-1

- The trigger item is accepted if R<=C. The prescale (PS) corresponding to a cutoff C is PS = 2*24/(C+1.)

In this scheme only number that are powers of 2 can be represented
exactly. Effective prescales are typically non-integer.  Integer
numbers can~t be represented exactly but have an error which grows with the
prescale)
"""

maxPrescaleCut = 0xFFFFFF  # 2**24 - 1

def getCutFromPrescale(prescale):
    """
    C = 2*24/PS - 1
    
    PS =     1 --> C = 2*24-1
    PS =     2 --> C = 2*23-1
    PS =    10 --> C = 1677720
    PS =  1024 --> C = 2*14-1
    PS =    50 --> C = 335543
    PS =   500 --> C = 33553
    PS =  5000 --> C = 3354
    PS = 50000 --> C = 334
    """
    sign = -1 if prescale<0 else 1
    prescale = abs(prescale)
    return sign * (0x1000000 / prescale - 1)


def getPrescaleFromCut(cut):
    """
    PS = 2*24/(C+1.)

    C = 2*24-1  --> PS =     1
    C = 2*23-1  --> PS =     2
    C = 1677720 --> PS =    10.000002980233305
    C = 2*14-1  --> PS =  1024
    C = 335543  --> PS =    50.0000447035
    C = 33553   --> PS =   500.006407582 
    C = 3354    --> PS =  5000.66020864  
    C = 334     --> PS = 50081.238806    
    """
    sign = -1 if cut<0 else 1
    cut = abs(cut)
    return sign * 0x1000000 / ( cut + 1. )
