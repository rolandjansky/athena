# Z plus Jet
job.Pythia8_i.Commands += ["WeakBosonAndParton:qqbar2gmZg = on",
                           "WeakBosonAndParton:qg2gmZq = on",
                           "WeakBosonAndParton:ffbar2gmZgm = on ",
                           "WeakBosonAndParton:fgm2gmZf = on",
                           # z decay channel
                           "23:onMode = off",
                           "23:onIfAny = 11 13 15",
                           "23:mMin = 60.0",
                           # Lower P_T for hard 2 ->2 process.
                           "PhaseSpace:pTHatMin = 10."]
