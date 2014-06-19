# The diJet settings
# stolen from EvgenJobOptions/share/MC8.108067.J1_pythia_jetjet_muFIXED.py
# no Muon selection
job.Pythia.PythiaCommand += ["pysubs msel 0",
                             "pysubs ckin 3 70.",
                             "pysubs msub 11 1",
                             "pysubs msub 12 1",
                             "pysubs msub 13 1",
                             "pysubs msub 68 1",
                             "pysubs msub 28 1",
                             "pysubs msub 53 1",
                             "pypars mstp 82 4",]
