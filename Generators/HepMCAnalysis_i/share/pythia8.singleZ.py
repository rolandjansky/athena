# single Z production
# Z decays into leptons and tau decays disabled
job.Pythia8_i.Commands += ["WeakSingleBoson:ffbar2gmZ = on", # single Z production
                           "23:onMode = off", # z decay channel
                           "23:onIfAny = 11 13 15",
                           "PhaseSpace:mHatMin = 60."
                           #"15:onMode = off", # no tau decays
                           #"SecondHard:TwoJets = on" # second Hard Process
                           ]
