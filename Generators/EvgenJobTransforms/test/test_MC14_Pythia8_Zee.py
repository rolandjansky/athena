## Pythia8 Z->ee

evgenConfig.description = "Z->ee production with no lepton filter and AU2 CTEQ6L1 tune"
evgenConfig.keywords = ["electroweak", "Z", "leptons"]

include("MC14JobOptions/Pythia8_AU2_CTEQ6L1_Common.py")
include("MC14JobOptions/Pythia8_Photos.py")

genSeq.Pythia8.Commands += ["WeakSingleBoson:ffbar2gmZ = on", # create Z bosons
                            "PhaseSpace:mHatMin = 60.", # lower invariant mass
                            "23:onMode = off", # switch off all Z decays
                            "23:onIfAny = 11"] # switch on Z->ee decays
