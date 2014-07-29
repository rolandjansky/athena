#______________________________________________________________________________________________________________
# author: liza.mijovic@_nospam_cern.ch 
# generate 10 LQ Pythia events at HERA,   demonstrating the use of new pyinit block use_PYINIT for 3mom frame
# source (physics contents of the setup): Pythia Examples http://projects.hepforge.org/pythia6/examples/
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
# HERA-specific : 
# 3mom frame: need to set beam and target's P-vectors explicitly,
# need to set beam and target's P-vectors explicitly,
# PYJETS commonblock not accessible via PythiaCommand += syntax, but only via PYGIVE 
Pythia.PygiveCommand += [ "P(1,1)=0.0", 
                          "P(1,2)=0.0",
                          "P(1,3)=-27.5",
                          "P(2,1)=0.0",
                          "P(2,2)=0.0",
                          "P(2,3)=820.0"]
# e+, p beam and target - can use any PYTHIA-supported syntax to specify these
Pythia.PythiaCommand +=["pyinit use_PYINIT 3mom e+ p 0.0"]

#-------------------------------------------------------------------------------------------------------------
#using PYGIVE (ATLAS independant syntax for params.)
Pythia.PygiveCommand += [ "msel=0",                  # Users decay choice.
                          "msub(145)=1",             # LQ process
                          "pmas(39,1)=200.0",        # LQ mass
                          "kfdp(539,1)=2",           # quark species
                          "kfdp(539,2)=-11",         # lepton species
                          "paru(151)=0.01",          # LQ Yukawa coupling value
                          "mstj(50)=0",              # no initial and final state partons interference due to long LQ lifetime
                          "mstp(67)=0"                           
                          ]

#-------------------------------------------------------------------------------------------------------------
# our pp tuned parameters might as well be worse as defaults ...  
# use no ATLAS parameter settings
#Pythia.Tune_Name="ATLAS_0" : you'll get TestHepMC crashes by doing this only 
# use only recommended ATLAS parameter settings (i.e. the ones necessary for succsfull && consistent simulation within Athena)
Pythia.Tune_Name="ATLAS_-1" # keep ER structure and new PS/MI ... see .log file for exact parameters settings
# use recommended + common (PDF, mass, MI schema etc., but not parameters commonly used for tuning) ATLAS parameter
#Pythia.Tune_Name="ATLAS_-2"
#______________________________________________________________________________________________________________


