# The ZplusJet settings
# stolen from EvgenJobOptions/share/MC8.106071.PythiaZmumuJet_Ptcut.py
# additional Z-> ee and Z->tau tau 
job.Pythia.PythiaCommand += ["pysubs msel 13",
                             #"pydat3 mdcy 15 1 0", # Turn off tau decays
                             "pysubs ckin 3 10.0", # Lower P_T for hard 2 ->2 process
                             "pysubs ckin 41 60.0", # Lower Z invariant mass
                             "pysubs ckin 43 60.0", # Lower Z invariant mass
                             # Z decays
                             "pydat3 mdme 174 1 0",
                             "pydat3 mdme 175 1 0",
                             "pydat3 mdme 176 1 0",
                             "pydat3 mdme 177 1 0",
                             "pydat3 mdme 178 1 0",
                             "pydat3 mdme 179 1 0",
                             "pydat3 mdme 182 1 1", # Switch for Z->ee
                             "pydat3 mdme 183 1 0",
                             "pydat3 mdme 184 1 1", # Switch for Z->mumu
                             "pydat3 mdme 185 1 0",
                             "pydat3 mdme 186 1 1", # Switch for Z->tautau
                             "pydat3 mdme 187 1 0"]
