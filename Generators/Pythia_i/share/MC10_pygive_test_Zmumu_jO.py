#_________________________________________________________________________________
# Jul. 09
# LM: parameters setup using PYGIVE example
# generate Zmumu Pythia events using mc09 tune for MRST2007lomod (LO* for MC)
# 1. setup Z production using PYGIVE calls
#_________________________________________________________________________________

from AthenaCommon.AlgSequence import AlgSequence
topAlg = AlgSequence("TopAlg")

from Pythia_i.Pythia_iConf import Pythia
topAlg += Pythia()
Pythia = topAlg.Pythia

theApp.EvtMax = 10

from AthenaServices.AthenaServicesConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()
ServiceMgr.AtRndmGenSvc.Seeds = ["PYTHIA 4789899 989240512",
                                 "PYTHIA_INIT 820021 2347532"]

# set MC09:
Pythia.Tune_Name="ATLAS_20100001"

#_________________________________________________________________________________

#1. setup Z production using PYGIVE calls
#---------------------------------------------------------------------------------
#using PythiaCommand
#Pythia.PythiaCommand += [ "pysubs msel 0",         # Users decay choice.
#                         "pydat1 parj 90 20000",   # Turn off FSR.
#                         "pydat3 mdcy 15 1 0",     # Turn off tau decays.
#                         "pysubs msub 1 1",        # Create Z bosons.
#                         "pysubs ckin 1 60.0",     # Lower invariant mass.
#                         "pydat3 mdme 174 1 0",
#                         "pydat3 mdme 175 1 0",
#                         "pydat3 mdme 176 1 0",
#                         "pydat3 mdme 177 1 0",
#                         "pydat3 mdme 178 1 0",
#                         "pydat3 mdme 179 1 0",
#                         "pydat3 mdme 182 1 0",    # Switch for Z->ee.
#                         "pydat3 mdme 183 1 0",
#                         "pydat3 mdme 184 1 1",    # Switch for Z->mumu.
#                         "pydat3 mdme 185 1 0",
#                         "pydat3 mdme 186 1 0",    # Switch for Z->tautau.
#                         "pydat3 mdme 187 1 0"]
#---------------------------------------------------------------------------------
#using PYGIVE 
Pythia.PygiveCommand += [ "msel=0",           # Users decay choice.
                          "parj(90)=20000",   # Turn off FSR.
                          "mdcy(15,1)=0",     # Turn off tau decays.
                          "msub(1)=1",        # Create Z bosons.
                          "ckin(1)=60.0",     # Lower invariant mass.
                          "mdme(174,1)=0",
                          "mdme(175,1)=0",
                          "mdme(176,1)=0",
                          "mdme(177,1)=0",
                          "mdme(178,1)=0",
                          "mdme(179,1)=0",
                          "mdme(182,1)=0",    # Switch for Z->ee.
                          "mdme(183,1)=0",
                          "mdme(184,1)=1",    # Switch for Z->mumu.
                          "mdme(185,1)=0",
                          "mdme(186,1)=0",    # Switch for Z->tautau.
                          "mdme(187,1)=0"]
#---------------------------------------------------------------------------------

# ... Tauola
#include ( "EvgenJobOptions/MC10_Tauola_Fragment.py" )

# ... Photos
#include ( "EvgenJobOptions/MC10_Photos_Fragment.py" )

