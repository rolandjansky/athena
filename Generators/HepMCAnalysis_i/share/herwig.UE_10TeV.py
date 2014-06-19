#refer to the U.E tuning for 10TeV in MC08 production
#main switches
## job.Herwig.HerwigCommand = ["hwproc iproc 11500",
##                             "jmparm jmueo 1",
##                             # turn on multiple interactions
##                             "jmparm msflag 1",
##                             "jmparm jmbug 0",
##                             #tuning
##                             # for 10TeV (4.91 for 14TeV) minimum pT of secondary scatters
##                             "jmparm ptjim 4.48",
##                             #  Inverse proton redius squared
##                             "jmparm jmrad 73 1.8",
##                             #  soft underlying event off (Herwig parameter)
##                             "hwpram prsof 0",]
job.Herwig.HerwigCommand = ["iproc 11500",
                            "jmueo 1",
                            # turn on multiple interactions
                            "msflag 1",
                            "jmbug 0",
                            #tuning
                            # for 10TeV (4.91 for 14TeV) minimum pT of secondary scatters
                            "ptjim 4.48",
                            #  Inverse proton redius squared
                            "jmrad 73 1.8",
                            #  soft underlying event off (Herwig parameter)
                            "prsof 0",]
