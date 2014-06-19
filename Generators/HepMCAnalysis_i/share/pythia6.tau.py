# setting for tau events
job.Pythia.PythiaCommand += [#"pypars mstp 41 1", # switch on all decays
                             "pysubs msel 11", # Selecting Z events (number 11):
                             "pysubs msub 1 1",
                             #"pypars mstp 43 2", # Z only, no photon
                             #for decay of Z into tau+tau-
                             "pydat3 mdme 174 1 0",
                             "pydat3 mdme 175 1 0",
                             "pydat3 mdme 176 1 0",
                             "pydat3 mdme 177 1 0",
                             "pydat3 mdme 178 1 0",
                             "pydat3 mdme 179 1 0",
                             "pydat3 mdme 180 1 0",
                             "pydat3 mdme 181 1 0",
                             "pydat3 mdme 182 1 0",
                             "pydat3 mdme 183 1 0",
                             "pydat3 mdme 184 1 0",
                             "pydat3 mdme 185 1 0",
                             "pydat3 mdme 186 1 1", # Ztotau+tau-
                             "pydat3 mdme 187 1 0"]
