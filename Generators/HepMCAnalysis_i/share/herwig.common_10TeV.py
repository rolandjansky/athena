#Refer to the common parameters for MC08 production

job.Herwig.HerwigCommand = [##beam energy
                            "hwproc pbeam1 5000",
                            "hwproc pbeam2 5000",
                            ##mass and widith
                            # PDG2007 TOP mass
                            "hwprop rmass 6 172.5",
                            # PDG2007 W mass
                            "hwprop rmass 198 80.403",
                            # PDG2007 W mass
                            "hwprop rmass 199 80.403",
                            # PDG2007 Z0 mass
                            "hwprop rmass 200 91.1876",
                            # PDG2007 W width
                            "hwpram gamw 2.141",
                            # PDG2007 Z0 width
                            "hwpram gamz 2.4952",      
                            ##PDF
                            #  CTEQ6L1 (LO fit with LO alpha_s)
                            "hwpram modpdf 10042",
                            #  external PDF library
                            "hwprch autpdf HWLHAPDF",
                            ##Fragmentation
                            # to fix the ratio of mesons to baryons in B decays
                            "hwpram clpow 1.20",
                            # to make Ks and Lambda stable
                            "hwdist pltcut 0.0000000000333",
                            # (D=10.) min. pT in hadronic jet production
                            "hwhard ptmin 10",
                            ##U.E.
                            #main switches
                            # Underlying event option (2->2 QCD)
                            "jmparm jmueo 1",
                            # turn on multiple interactions
                            "jmparm msflag 1",
                            "jmparm jmbug 0",
                            #tuning 
                            # for 10TeV (4.91 for 14TeV) minimum pT of secondary scatters
                            "jmparm ptjim 4.48",
                            #  Inverse proton redius squared
                            "jmparm jmrad 73 1.8",
                            #  soft underlying event off (Herwig parameter)
                            "hwpram prsof 0",
                            ##others
                            #Tau decay by TAUOLA
                            "hwdspn taudec TAUOLA",
                            # (D=1) include neutral B meson mixing
                            "hwdist mixing 1"]
