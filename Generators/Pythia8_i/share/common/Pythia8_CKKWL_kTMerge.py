try:
    PYTHIA8_nJetMax
except RuntimeError:
    raise RuntimeError("Variable \"PYTHIA8_nJetMax\" is not defined, this is needed to configure Pythia8 CKKW-L kT merging settings. Please define it in your jobOptions")
else:
    print ("PYTHIA8_nJetMax = %i" % PYTHIA8_nJetMax)

try:
    PYTHIA8_Process
except RuntimeError:
    raise RuntimeError("Variable \"PYTHIA8_Process\" is not defined, this is needed to configure Pythia8 CKKW-L kT merging settings. Please define it in your jobOptions")
else:
    print ("PYTHIA8_Process = %s" % PYTHIA8_Process)

try:
    PYTHIA8_TMS
except RuntimeError:
    raise RuntimeError("Variable \"TMS\" is not defined, this is needed to configure Pythia8 CKKW-L kT merging settings. Please define it in your jobOptions")
else:
    print ("PYTHIA8_TMS = %f" % PYTHIA8_TMS)

try:
    PYTHIA8_Dparameter
except RuntimeError:
    raise RuntimeError("Variable \"Dparameter\" is not defined, this is needed to configure Pythia8 CKKW-L kT merging settings. Please define it in your jobOptions")
else:
    print ("PYTHIA8_Dparameter = %f" % PYTHIA8_Dparameter)

try:
    PYTHIA8_nQuarksMerge
except RuntimeError:
    raise RuntimeError("Variable \"PYTHIA8_nQuarksMerge\" is not defined, this is needed to configure Pythia8 CKKW-L kT merging settings. Please define it in your jobOptions")
else:
    print ("PYTHIA8_nQuarksMerge = %i" % PYTHIA8_nQuarksMerge)

genSeq.Pythia8.Commands += ["Merging:doKTMerging = on",
                            "Merging:ktType = 1",
                            "Merging:nJetMax = %i" % PYTHIA8_nJetMax,
                            "Merging:Process = %s" % PYTHIA8_Process,
                            "Merging:TMS = %f" % PYTHIA8_TMS,
                            "Merging:Dparameter = %f" % PYTHIA8_Dparameter,
                            "Merging:nQuarksMerge = %i" % PYTHIA8_nQuarksMerge]

