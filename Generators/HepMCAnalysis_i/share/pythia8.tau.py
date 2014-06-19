# Z production
# Z decays into tau+tau-
job.Pythia8_i.Commands += ["WeakBosonAndParton:qqbar2gmZg = on",
                           "WeakBosonAndParton:qg2gmZq = on",
                           "23:onMode = off", # choose decay modes for Z0 : switch off everything but Z0 -> leptons
                           "23:onIfAny = 15"] # 23:onIfAny = 11 13 15, choose tau
