# The single Z settings
# stolen from EvgenJobOptions/share/MC8.106051.PythiaZmumu_1Lepton.py
# additional Z-> ee and Z->tau tau 
# Tau decays disabled
job.Pythia.PythiaCommand += ["pysubs msel 11", # Create Z bosons
                             #"pydat3 mdcy 15 1 0", # Turn off tau decays.
                             "pysubs ckin 1 60.0", # Lower invariant mass.
                             # Z decays
                             "pydat3 mdme 174 1 0",
                             "pydat3 mdme 175 1 0",
                             "pydat3 mdme 176 1 0",
                             "pydat3 mdme 177 1 0",
                             "pydat3 mdme 178 1 0",
                             "pydat3 mdme 179 1 0",
                             "pydat3 mdme 182 1 1", # Switch for Z->ee.
                             "pydat3 mdme 183 1 0",
                             "pydat3 mdme 184 1 1", # Switch for Z->mumu.
                             "pydat3 mdme 185 1 0",
                             "pydat3 mdme 186 1 1", # Switch for Z->tautau
                             "pydat3 mdme 187 1 0",]
