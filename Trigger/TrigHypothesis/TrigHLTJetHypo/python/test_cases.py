# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

test_strings = [
    'simple([(38et, 0eta320)])',
    'simple([(38et, 0eta320)(40et, 0eta320)])',
    'simple([(38et, 0eta320, 011jvt)])',
    'or([] simple([(10et)]) simple([(20et)(40et)]))',
    'and([] simple([(10et)]) simple([(20et)]))',
    'not([] simple([(10et)]))',
    'not([] and([] simple([(40et, 0eta320)]) simple([(100et, 0eta320)])))',
    'or([] not([] simple([(40et, 0eta320)])) not([] simple([(100et, 0eta320)])))',
    'or([] and([] simple([(40et, 0eta320)]) simple([(40et, 0eta320)])) not([] simple([(100et, 0eta320)])))',

    'and([] simple([(50et)(70et)]) dijet([(900djmass, 26djdphi)]))',
    'and([]simple([(50et)(70et)])combgen([]dijet([(900djmass,26djdphi)]) simple([(10et)(20et)])))',
    'and([]simple([(81et)(81et)])combgen([(50et, eta100)]dijet([(26djdphi)])))',
    'simple([(70et,0eta240)(70et,0eta240)(70et,0eta240)(70et,0eta240)(70et,0eta240)])',
    'partgen([(20et,0eta320)]simple([(40et,0eta320)(50et,0eta320)])simple([(35et,0eta240)(55et,0eta240)]))',
    'simple([(10et, neta0)(20et, peta)])', # missing low value for eta - take default
    'simple([(100momwidth200)])', # jet moment condition
    
    # from HLT_j0_vbenfSEP30etSEP34mass35SEP50fbet_L1J20:
    'and([]simple([(30et,500neta)(30et,peta500)])combgen([(10et)]dijet([(34djmass,26djdphi)])simple([(10et)(20et)])))',
    'qjet([(34qjmass)])',
    """partgen([]simple([(neta)(peta)])
                 combgen([]
                         qjet([(qjmass)])
                         partgen([]
                                 combgen([] 
                                         dijet([(djmass)])
                                         simple([(10et)(11et)]))
                                 combgen([] 
                                         dijet([(djmass)])
                                         simple([(12et)(13et)])))))""",

    'and([]simple([(30et,500neta)(30et,peta500)])combgen([(10et)]dijet([(34djmass,26djdphi)])simple([(10et)(20et)])))',
    'qjet([(34qjmass)])',
    """partgen([]simple([(neta)(peta)])
                 combgen([]
                         qjet([(qjmass)])
                         partgen([]
                                 combgen([] 
                                         dijet([(80djmass90)])
                                         simple([(84et)(84et)]))
                                 combgen([] 
                                         dijet([(81djmass91)])
                                         simple([(84et)(84et)])))))""",

        """partgen([]
                 partgen([]
                         simple([(neta)(peta)])
                        )             
                 combgen([]
                         qjet([(175qjmass177)])
                         partgen([]
                                 combgen([] 
                                         dijet([(79djmass90)])
                                         simple([(82et)(83et)]))
                                 combgen([] 
                                         dijet([(79djmass91)])
                                         simple([(82et)(83et)])))))""",
    
        """partgen([]
                 partgen([]
                         simple([(neta, 84et)(peta, 84et)])
                        )             
                 combgen([]
                         qjet([(170qjmass190)])
                         partgen([]
                                 combgen([] 
                                         dijet([(70djmass90)])
                                         simple([(10et)(11et)]))
                                 combgen([] 
                                         dijet([(71djmass91)])
                                         simple([(12et)(13et)])))))""",

        
        """partgen([]
                   combgen([] 
                           dijet([(djmass90)])
                           simple([(10et)(11et)]))
                   combgen([] 
                           dijet([(79djmass91)])
                           simple([(12et)(13et)])))""",

            
        """partgen([]
                   combgen([] 
                           dijet([(djmass50)])
                           simple([(10et)(11et)]))
                   combgen([] 
                           dijet([(79djmass101)])
                           simple([(12et)(13et)])))""",

        
        """partgen([]
                 partgen([]
                         simple([(neta, 50et)(peta, 51et)])
                        )             
                 combgen([]
                         qjet([(300qjmass400)])
                         partgen([]
                                 combgen([] 
                                         dijet([(50djmass200)])
                                         simple([(10et)(11et)]))
                                 

                                 combgen([] 
                                         dijet([(50djmass200)])
                                         simple([(12et)(13et)]))
                          )
                      )
            )""",

    # use an "And" node for VBENFQ with FB sharing
        """partgen([]
                 partgen([]
                         simple([(neta, 84et)(peta, 84et)])
                        )             
                 combgen([]
                         qjet([(170qjmass190)])
                         and([]
                                 combgen([] 
                                         dijet([(70djmass90)])
                                         simple([(10et)(11et)]))
                                 combgen([] 
                                         dijet([(71djmass91)])
                                         simple([(12et)(13et)])))))""",
    # forest
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
    )"""
    
]


if __name__ == '__main__':
    for i, tc in enumerate(test_strings):
        print (i, tc)

    print('There are %d test cases' % len(test_strings))
