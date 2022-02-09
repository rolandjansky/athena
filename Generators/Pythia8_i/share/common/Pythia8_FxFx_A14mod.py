
#
# This version changes the tuned settings of A14 slightly in favour of
# the recommended FxFx settings
#

try:
    PYTHIA8_nJetMax
except RuntimeError:
    raise RuntimeError("Variable \"PYTHIA8_nJetMax\" is not defined, this is needed to configure Pythia8 FxFx matching settings. Please define it in your jobOptions")
else:
    print("PYTHIA8_nJetMax = %i"%PYTHIA8_nJetMax)


try:
    PYTHIA8_qCut
except RuntimeError:
    raise RuntimeError("Variable \"PYTHIA8_qCut\" is not defined, this is needed to configure Pythia8 FxFx matching settings. Please define it in your jobOptions")
else:
    print("PYTHIA8_qCut = %i"%PYTHIA8_qCut)
    


genSeq.Pythia8.Commands += ["JetMatching:merge            = on",
                            "JetMatching:scheme           = 1",
                            "JetMatching:setMad           = off",
                            "JetMatching:qCut             = %f"%PYTHIA8_qCut,
                            "JetMatching:coneRadius       = 1.0",
                            "JetMatching:etaJetMax        = 10.0",
                            "JetMatching:doFxFx        = on",
                            "JetMatching:qCutME        = 8.0",
                            "JetMatching:nJetMax       = %i"%PYTHIA8_nJetMax,
                            "JetMatching:jetAlgorithm = 2", #explicit setting of kt-merging for FxFx (also imposed by Py8-FxFx inteface)
                            "JetMatching:slowJetPower = 1", #explicit setting of kt-merging for FxFx (also imposed by Py8-FxFx inteface)
                            "JetMatching:nQmatch = 5", #4 corresponds to 4-flavour scheme (no matching of b-quarks), 5 for 5-flavour scheme
                            "JetMatching:eTjetMin = %f"%PYTHIA8_qCut, #This is 20 in the Pythia default, it should be <= qCut 
                            "SpaceShower:rapidityOrder  = off", # FxFx + A14 prescription
                            "SpaceShower:pTmaxFudge     = 1.0", # FxFx + A14 prescription
]

   
genSeq.Pythia8.UserHooks += [ 'JetMatchingMadgraph']
genSeq.Pythia8.FxFxXS = True

