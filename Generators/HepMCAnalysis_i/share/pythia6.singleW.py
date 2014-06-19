# The single W settings
# stolen from EvgenJobOptions/share/MC8.106021.PythiaWmunu_1Lepton.py
# Tau decays disabled
job.Pythia.PythiaCommand += ["pysubs msel 0",
                             #"pydat3 mdcy 15 1 0", # Turn off tau decays.
                             "pysubs msub 2 1", # Create W bosons.
                             # W decays
                             "pydat3 mdme 190 1 0",
                             "pydat3 mdme 191 1 0",
                             "pydat3 mdme 192 1 0",
                             "pydat3 mdme 194 1 0",
                             "pydat3 mdme 195 1 0",
                             "pydat3 mdme 196 1 0",
                             "pydat3 mdme 198 1 0",
                             "pydat3 mdme 199 1 0",
                             "pydat3 mdme 200 1 0",
                             "pydat3 mdme 206 1 1", # Switch for W->enu
                             "pydat3 mdme 207 1 1", # Switch for W->munu
                             "pydat3 mdme 208 1 1"] # Switch for W->taunu
