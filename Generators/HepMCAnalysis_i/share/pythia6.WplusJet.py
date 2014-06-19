# The WplusJet settings
# stolen from EvgenJobOptions/share/MC8.106041.PythiaWmunuJet_Ptcut.py
# additional W-> enu and W->tau nu
job.Pythia.PythiaCommand += ["pysubs msel 14",
                             #"pydat3 mdcy 15 1 0", # Turn off tau decays
                             "pysubs ckin 3 10.0", # Lower P_T for hard 2 ->2 process
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
                             "pydat3 mdme 207 1 0", # Switch for W->munu
                             "pydat3 mdme 208 1 0"] # Switch for W->taunu
