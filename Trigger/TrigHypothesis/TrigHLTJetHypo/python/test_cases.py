# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

# condition names are names of leaf node, or combining nodes
# existing condition names 6/1/2021:
# simple
# dijet
# agg
# all

test_strings = [
    'simple([(38et, 0eta320)])',
    'simple([(38et, 0eta320)(40et, 0eta320)])',
    'simple([(38et, 0eta320, 011jvt)])',
    'simple([(70et,0eta240)(70et,0eta240)(70et,0eta240)(70et,0eta240)(70et,0eta240)])',
    'simple([(10et, neta0)(20et, peta)])', # missing low value for eta - take default
    'simple([(100momwidth200)])', # jet moment condition
    
    'qjet([(34qjmass)])',
 
    # forest: forward backward jets + dijet sharing
    """simple
    (
        [(50et, 500neta, leg000)(50et, peta500, leg000)]
    )
    dijet
    (
        [(34mass, 26djdphi, leg000)]
        simple
        (
            [(10et, 0eta320, leg000)(20et, 0eta320, leg000)]
        )
    )""",

    # single tree forward backward jets + dijet no sharing
    """ all([]
        simple
        (
           [(50et, 500neta, leg000)(50et, peta500, leg000)]
        )
        dijet
        (
            [(34mass, 26djdphi, leg000)]
            simple
            (
                [(10et, 0eta320, leg000)(20et, 0eta320, leg000)]
            )
        )
    )""",

    
    'simple([(38et, 0eta320, 100fltr:neta50, 90fltr:cphi100)])',
]


if __name__ == '__main__':
    for i, tc in enumerate(test_strings):
        print (i, tc) # noqa: ATL901

    print('There are %d test cases' % len(test_strings)) # noqa: ATL901
