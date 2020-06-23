# taken from Sherpa examples: Examples/V_plus_Bs/LHC_Fusing/

genSeq.Sherpa_i.Parameters += [ "SHERPA_LDADD=SherpaFusing",
                                "USERHOOK=Fusing_Fragmentation",
                                "CSS_SCALE_SCHEME=20",
                                "CSS_EVOLUTION_SCHEME=30",
                                "FUSING_FRAGMENTATION_STORE_AS_WEIGHT=1",
]
