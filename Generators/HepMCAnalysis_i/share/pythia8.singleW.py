# single W production
# W decays into leptons and tau decays disabled
job.Pythia8_i.Commands += ["WeakSingleBoson:ffbar2W = on", # single W production
                           "24:onMode = off", # z decay channel
                           "24:onIfAny = 11 13 15"
                           #"15:onMode = off", # no tau decays
                           #"SecondHard:TwoJets = on" # second Hard Process
                           ]
