try:
    PYTHIA8_nJetMax
except RuntimeError:
    raise RuntimeError("Variable \"PYTHIA8_nJetMax\" is not defined, this is needed to configure Pythia8 MLM matching settings. Please define it in your jobOptions")
else:
    print("PYTHIA8_nJetMax = %i" % PYTHIA8_nJetMax)

try:
    PYTHIA8_qCut
except RuntimeError:
    raise RuntimeError("Variable \"qCut\" is not defined, this is needed to configure Pythia8 MLM matching settings. Please define it in your jobOptions")
else:
    print("PYTHIA8_qCut = %f" % PYTHIA8_qCut)

try:
    PYTHIA8_coneRadius
except RuntimeError:
    raise RuntimeError("Variable \"coneRadius\" is not defined, this is needed to configure Pythia8 MLM matching settings. Please define it in your jobOptions")
else:
    print("PYTHIA8_coneRadius = %f" % PYTHIA8_coneRadius)

try:
    PYTHIA8_nQmatch
except RuntimeError:
    raise RuntimeError("Variable \"PYTHIA8_nQmatch\" is not defined, this is needed to configure Pythia8 MLM matching settings. Please define it in your jobOptions")
else:
    print("PYTHIA8_nQmatch = %i" % PYTHIA8_nQmatch)

# decrease relative precision
genSeq.Pythia8.Commands += ["Check:epTolErr               = 1e-2"]

if "UserHooks" in genSeq.Pythia8.__slots__.keys():
    genSeq.Pythia8.UserHooks += ['JetMatchingMadgraph']
else:
    genSeq.Pythia8.UserHook = 'JetMatchingMadgraph'

genSeq.Pythia8.Commands += ["JetMatching:merge            = on",
                            "JetMatching:scheme           = 1",
                            "JetMatching:setMad           = off",     # if on, following "qCut" setting can be removed
                            "JetMatching:nQmatch          = %i" % PYTHIA8_nQmatch,
                            "JetMatching:qCut             = %f" % PYTHIA8_qCut,
                            "JetMatching:eTjetMin         = %f" % PYTHIA8_qCut,
                            "JetMatching:coneRadius       = %f" % PYTHIA8_coneRadius,
                            "JetMatching:etaJetMax        = 1000.0",
                            "JetMatching:nJetMax          = %i" % PYTHIA8_nJetMax ]

