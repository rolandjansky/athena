# CAUTION: when including this, you have to set the following process-dependent setting in your JO:
# If two strong couplings are involved at Born level, `FUSING_DIRECT_FACTOR=1` (e.g. Zbb).
# If there are four such couplings, `FUSING_DIRECT_FACTOR=2` (e.g. ttbb).

# taken from Sherpa examples: Examples/V_plus_Bs/LHC_Fusing/

genSeq.Sherpa_i.Parameters += [ "SHERPA_LDADD=SherpaFusing",
                                "USERHOOK=Fusing_Direct",
                                "CSS_SCALE_SCHEME=20",
                                "CSS_EVOLUTION_SCHEME=30", ]
