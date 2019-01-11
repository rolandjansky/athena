
try:
    PYTHIA8_nJetMax
except RuntimeError:
    raise RuntimeError("Variable \"PYTHIA8_nJetMax\" is not defined, this is needed to configure Pythia8 FxFx matching settings. Please define it in your jobOptions")
else:
    print "PYTHIA8_nJetMax = %i"%PYTHIA8_nJetMax


try:
    PYTHIA8_qCut
except RuntimeError:
    raise RuntimeError("Variable \"PYTHIA8_qCut\" is not defined, this is needed to configure Pythia8 FxFx matching settings. Please define it in your jobOptions")
else:
    print "PYTHIA8_qCut = %i"%PYTHIA8_qCut
    


genSeq.Pythia8.Commands += ["JetMatching:merge            = on",
                            "JetMatching:scheme           = 1",
                            "JetMatching:setMad           = off",
                            "JetMatching:qCut             = %f"%PYTHIA8_qCut,
                            "JetMatching:coneRadius       = 1.0",
                            "JetMatching:etaJetMax        = 10.0",
                            "JetMatching:doFxFx        = on",
                            "JetMatching:qCutME        = 8.0",
                            "JetMatching:nJetMax       = %i"%PYTHIA8_nJetMax ]


   

genSeq.Pythia8.UserHook = 'JetMatchingMadgraph'
genSeq.Pythia8.FxFxXS = True


