###############################################################
#
# Job options file fragment for generation of B events 
# no decay channel is specified.
# Only events containing at least one muon  
# with pT>4GeV |eta|<2.5 are written to output
# Selection criteria can be changed by datacards
#==============================================================
#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence 
topAlg = AlgSequence("TopAlg") 

from PythiaB.PythiaBConf import PythiaB
topAlg += PythiaB()

#--------------------------------------------------------------
# Algorithms 
#--------------------------------------------------------------
#--------------------------------------------------------------				 
#              PARAMETERS  SPECIFIC  TO   PYTHIABMODULE
#--------------------------------------------------------------
PythiaB = topAlg.PythiaB
PythiaB.ForceBDecay = "no"
PythiaB.ForceCDecay = "no"
#--------------------------------------------------------------
# --------  PYTHIA PARAMETERS OPTIMAL FOR BEAUTY PRODUCTION  --
#--------------------------------------------------------------
#  'msel 5' is only for fast tests! 
#  for correct b-producion you should use 'msel 1'
# 'mstj 26 0' = no mixing was defined in Btune as default
# 'mstj 22 2' = no K0S, Lambda0 decays in Pythia - defined in Btune as default

include( "PythiaB/Btune.py" )

PythiaB.PythiaCommand += [
    # lambda value for alpha_s
    "pydat1 parj 81 0.14",
    # effects the amount of color reconnection
    "pypars parp 78 0.2",
    # suppression probability for string pieces reattaching beam remnant
    # to hard scatter
    "pypars parp 80 0.01",
    # effective pT_min for MI (pt0 scale)
    "pypars parp 82 1.9",
    # matter distribution
    "pypars parp 83 0.3",
    "pypars parp 84 0.5",
    # pT0 scale
    "pypars parp 89 1800",
    "pypars parp 90 0.22",
    # ISR
    "pypars mstp 70 2",
    # max scale for FSR dipoles
    "pypars mstp 72 0",
    # new multiple interaction model and new parton shower selected
    "pypars mstp 81 1",
    # MPI
    "pypars mstp 82 4",
    # ISR
    "pypars mstp 84 1",
    # FSR
    "pypars mstp 85 1",
    # MPI
    "pypars mstp 86 2",
    # sea quarks: large x behavior of assumed gluon distribution
    "pypars mstp 87 4",
    # color reconnection
    "pypars mstp 88 0",
    "pypars mstp 89 1",
    "pypars mstp 90 1",
    "pypars mstp 95 1",
    ]

PythiaB.PythiaCommand += ["pysubs ckin 3 6.",
				 "pysubs msel 1"]
#--------------------------------------------------------------
# -------------  DEFINE SELECTION CUTS  -------------
#--------------------------------------------------------------				
PythiaB.flavour =  5.				
#  ------------- Selections on b  quarks   -------------
PythiaB.cutbq = ["5. 4.5 or 5. 4.5"]
#  ------------- LVL1 muon cuts 0=OFF 1=ON -------------
PythiaB.lvl1cut = [ 1.,  4., 2.5]
#  ------------- LVL2 muon/electron cuts  0=OFF 1=ON-------------
PythiaB.lvl2cut = [ 0.,  13.,     6.,   2.5]
#PythiaB.lvl2cut = { 0.,  11.,     6.,   2.5};
#  ------------- Offline cuts 0=OFF 1=ON -------------
PythiaB.offcut = [ 0., 0., 102.5, 0., 102.5, 0., 102.5]
#  ------------- Number of repeated hadronization mhadr -------------
PythiaB.mhadr =  7. 
###############################################################

#==============================================================
#
# End of job options file
#
###############################################################
