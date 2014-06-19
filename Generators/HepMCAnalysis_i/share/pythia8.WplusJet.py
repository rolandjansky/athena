# W plus Jet production
# W decays into leptons
# tau decays disabled
job.Pythia8_i.Commands += ["WeakBosonAndParton:qqbar2Wg = on",
                           "WeakBosonAndParton:qg2Wq = on",
                           "WeakBosonAndParton:ffbar2Wgm = on",
                           "WeakBosonAndParton:fgm2Wf = on",
                           # z decay channel
                           "24:onMode = off",
                           "24:onIfAny = 11 13 15",
                           "PhaseSpace:pTHatMin = 10." # Lower P_T for hard 2 ->2 process
                           #"15:onMode = off", # no tau decays
                           #"SecondHard:TwoJets = on" # second Hard Process
                           ]
