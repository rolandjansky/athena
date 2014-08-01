###########################################################
##  jobOptions for evgen of the process:                 ##
##  h --> v-pion v-pion (v-pion --> u-boson)             ##
##                                                       ##
##  author: Daniel Ventura, U. Washington                ##
##          ventura@cern.ch                              ##
###########################################################

from AthenaCommon.AlgSequence import AlgSequence
topAlg = AlgSequence("TopAlg")

# Number of events to be processed (default is 10)
NumEvts = 10
###############################################
#
# Initialize Pythia
#

theApp.EvtMax = NumEvts

from AthenaServices.AthenaServicesConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()
ServiceMgr.AtRndmGenSvc.Seeds = ["PYTHIA 4789899 989240512","PYTHIA_INIT 820021 2347532"]



#include ( "EvgenJobOptions/MC8_Pythia_Common.py" )
from PythiaExo_i.PythiaLLP_iConf import PythiaLLP
topAlg += PythiaLLP()
Pythia = topAlg.PythiaLLP

# HV Higgs lepton-jets production
Pythia.LLPpyupda = "LLP_leptonJets.pyupda.in"
Pythia.PythiaCommand += ["pysubs msel 0"]
Pythia.PythiaCommand += ["pydat1 parj 22 2"]
Pythia.PythiaCommand += ["pydat1 parj 71 500000"] #max ct of particles
Pythia.PythiaCommand += ["pypars mstp 127 1"]

#Create gg --> H0 --> vpi vpi 
Pythia.PythiaCommand += ["pysubs msub 152 1"]

#set the mass of the H0
Pythia.PythiaCommand += ["pydat2 pmas 35 1 120.D0"]

#set the mass of the v-pion
Pythia.PythiaCommand += ["pydat2 pmas 6000111  1 4.D0"]

#set the lifetime of the v-pion (c tau in mm)
Pythia.PythiaCommand += ["pydat2 pmas 6000111 4 0.D0"]

#set the mass of the u-boson
Pythia.PythiaCommand += ["pydat2 pmas 6001022 1 0.6D0"]
Pythia.PythiaCommand += ["pydat2 pmas 6001022 4 80.D0"]

#--------------------------------------------------------------

# Configuration for EvgenJobTransforms

#--------------------------------------------------------------

from EvgenJobOptions.PythiaEvgenConfig import evgenConfig
evgenConfig.efficiency = 0.95

