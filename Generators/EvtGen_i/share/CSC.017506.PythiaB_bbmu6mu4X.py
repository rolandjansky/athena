###############################################################
# PRODUCTION SYSTEM FRAGMENT
# Job options file for generation of B events 
# no decay channel is specified.
# Only events containing at least one muon  
# with pT>6GeV |eta|<2.5 are written to output
# Selection criteria can be changed by datacards
#==============================================================
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

PythiaB.PythiaCommand += [
    "pysubs ckin 9 -4.5",
    "pysubs ckin 10 4.5",
    "pysubs ckin 11 -4.5",
    "pysubs ckin 12 4.5",
    "pydat1 mstj 26 0",
    "pydat1 mstj 22 2",
    "pydat1 parj 13 0.65",
    "pydat1 parj 14 0.12",
    "pydat1 parj 15 0.04",
    "pydat1 parj 16 0.12",
    "pydat1 parj 17 0.2",
    "pydat1 parj 55 -0.006",
    ]

PythiaB.PythiaCommand += [
                   "pypars mstp 70 2",
                   "pypars mstp 72 0",
                   "pypars mstp 81 21",
                   "pypars mstp 82 4",
                   "pypars mstp 84 1",
                   "pypars mstp 85 1",
                   "pypars mstp 86 2",
                   "pypars mstp 87 4",
                   "pypars mstp 88 0",
                   "pypars mstp 89 1",
                   "pypars mstp 90 1",
                   "pypars mstp 95 1",
                   "pypars parp 78 0.2",
                   "pypars parp 80 0.01",
                   "pypars parp 82 1.9",
                   "pypars parp 83 0.3",
                   "pypars parp 84 0.5",
                   "pypars parp 89 1800",
                   "pypars parp 90 0.22",
                   "pydat1 parj 81 0.14"]

PythiaB.PythiaCommand += ["pysubs ckin 3 10.",
				 "pysubs msel 1"]
#--------------------------------------------------------------
# -------------  DEFINE SELECTION CUTS  -------------
#--------------------------------------------------------------				
PythiaB.flavour =  5.				
#  ------------- Selections on b  quarks   -------------
PythiaB.cutbq = ["7. 4.5 or 7. 4.5"]
#  ------------- LVL1 muon cuts 0=OFF 1=ON -------------
PythiaB.lvl1cut = [ 1.,  6., 2.5]
#  ------------- LVL2 muon/electron cuts  0=OFF 1=ON-------------
PythiaB.lvl2cut = [ 1.,  13.,     4.,   2.5]
#PythiaB.lvl2cut = { 0.,  11.,     6.,   2.5};
#  ------------- Offline cuts 0=OFF 1=ON -------------
PythiaB.offcut = [ 0., 0., 102.5, 0., 102.5, 0., 102.5]
#  ------------- Number of repeated hadronization mhadr -------------
PythiaB.mhadr =  20. 
#  ------------- For how many events store B-chain in NTUPLE -------------

#==============================================================
#
# End of job options file
#
###############################################################
