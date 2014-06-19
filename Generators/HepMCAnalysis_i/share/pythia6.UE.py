#The Underlying Events setting
#QCD 2->2 partonic scattering + non-diffractive + double diffractive
job.Pythia.PythiaCommand += ["pysubs msel 0",
                             "pysubs msub 11 1",
                             "pysubs msub 12 1",
                             "pysubs msub 13 1",
                             "pysubs msub 28 1",
                             "pysubs msub 53 1",
                             "pysubs msub 68 1",
                             "pysubs msub 94 1",
                             "pysubs msub 95 1",
                             "pysubs msub 96 1"]
