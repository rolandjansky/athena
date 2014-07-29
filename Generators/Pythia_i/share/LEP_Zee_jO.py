#______________________________________________________________________________________________________________
# author: liza.mijovic@_nospam_cern.ch 
# generate 1000 Zmumu Pythia events at LEP, demonstrating the use of new pyinit block use_PYINIT for e+ e- beams
#______________________________________________________________________________________________________________

from AthenaCommon.AlgSequence import AlgSequence
topAlg = AlgSequence("TopAlg")

from Pythia_i.Pythia_iConf import Pythia
topAlg += Pythia()
Pythia = topAlg.Pythia

theApp.EvtMax = 1000

from AthenaServices.AthenaServicesConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()
ServiceMgr.AtRndmGenSvc.Seeds = ["PYTHIA 4789899 989240512",
                                 "PYTHIA_INIT 820021 2347532"]
#-------------------------------------------------------------------------------------------------------------
# initialization setup:
#-------------------------------------------------------------------------------------------------------------
# general Athena syntax is : 
# Pythia.PythiaCommand +=["pyinit use_PYINIT FRAME BEAM TARGET WIN"]
# which is translated to PYTHIA PYINIT SUBROUTINE CALL as: 
# PYINIT(FRAME,BEAM,TARGET,WIN)
# allowed FRAME BEAM TARGET WIN values are same as for Pythia standalone running
# when using pyinit use_PYINIT please make sure that:
# 1) all four FRAME BEAM TARGET WIN values are passed, even if some of them correspond to LHC defaults
# 2) WIN is passed as a double (i.e. not as an integer) 
# 3) info on the Athena and PYTHIA setup can be checked is printed-out before event generation is started 
#-------------------------------------------------------------------------------------------------------------
# LEP-specific initialization: 
# also set frame even though it it the same as default (needed for PythiaCommand vector parsing)
# set beam and target to e+ and e-
# set win to 91.2 GeV
Pythia.PythiaCommand +=["pyinit use_PYINIT CMS e+ e- 91.2"]

#-------------------------------------------------------------------------------------------------------------
#using PYGIVE (ATLAS independant syntax for params.)
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
#-------------------------------------------------------------------------------------------------------------
# our pp tuned parameters might as well be worse as defaults ...  
# use no ATLAS parameter settings
#Pythia.Tune_Name="ATLAS_0" : you'll get TestHepMC crashes by doing this only 
# use only recommended ATLAS parameter settings (i.e. the ones necessary for succsfull && consistent simulation within Athena)
Pythia.Tune_Name="ATLAS_-1" # keep ER structure and new PS/MI ... see .log file for exact parameters settings
# use recommended + common (PDF, mass, MI schema etc., but not parameters commonly used for tuning) ATLAS parameter
#Pythia.Tune_Name="ATLAS_-2"
#______________________________________________________________________________________________________________
