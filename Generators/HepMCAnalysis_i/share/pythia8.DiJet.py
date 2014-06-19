# all 2 -> 2 QCD processes
job.Pythia8_i.Commands += ["HardQCD:gg2gg = on",
                           "HardQCD:gg2qqbar = on",
                           "HardQCD:qg2qg = on",
                           "HardQCD:qqbar2gg = on",
                           "HardQCD:qq2qq = on",
                           "HardQCD:qqbar2qqbarNew = on",
                           #"PhaseSpace:pTHatMin = 35.",
                           #"PhaseSpace:pTHatMax = 70.",
                           "PhaseSpace:pTHatMin = 50.",
                           "PhaseSpace:pTHatMax = 100."
                           #"SecondHard:TwoJets = on" # second Hard Process
                           ]
