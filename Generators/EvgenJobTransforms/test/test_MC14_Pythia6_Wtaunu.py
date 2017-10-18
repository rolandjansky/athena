## PYTHIA W->tau,nu

evgenConfig.description = "W->tau,nu production with the AUET2B CTEQ6L1 tune"
evgenConfig.keywords = ["electroweak", "W", "leptons", "tau"]

include("MC14JobOptions/Pythia_AUET2B_CTEQ6L1_Common.py")
include("MC14JobOptions/Pythia_Photos.py")
include("MC14JobOptions/Pythia_Tauola.py")

genSeq.Pythia.PythiaCommand += ["pysubs msel 0",
                                "pysubs msub 2 1", # Create W bosons
                                "pysubs ckin 1 60.0"] # Lower invariant mass

include("MC14JobOptions/Pythia_Decay_Wtaunu.py")
