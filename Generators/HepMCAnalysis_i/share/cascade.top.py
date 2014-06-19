#settings for ttbar production
job.Cascade.CascadeInit = ["pt2cut 30",
                           "iccfm 1",
                           "acc1 1.0",
                           "acc2 0.5","ipro 11"]
job.Cascade.PythiaInit = ["pydat2 pmas 4 1 1.6",
                          "pydat2 pmas 5 1 4.75",
                          "pydat2 pmas 25 1 125.",
                          "pydat1 paru 112 0.2",
                          "pydat1 mstu 111 1",
                          "pydat1 mstj 48 1",
                          "pydat1 mstu 112 4",
                          "pydat1 mstu 113 3",
                          "pydat1 mstu 114 5",
                          "pypars mstp 128 0"]
