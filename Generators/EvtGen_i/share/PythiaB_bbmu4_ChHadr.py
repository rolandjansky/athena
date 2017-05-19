###############################################################
# PRODUCTION FRAGMENT
#
# Job options file for generation of bb events, 
#
# Only events containing at least one muon  
# with pT>4GeV |eta|<2.8
# and at least one Charm Hadron in the decay of interest
# are written to output.
#
# Author : Leonid Gladilin (gladilin@mail.cern.ch), 2008-0004-14
#
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
#
PythiaB.ForceDecayChannel = "CharmHadrons"
# lvl1 and lvl2 cuts pt_L1 eta_L1 pt_L2 eta_L2
#                                  D+-  Ds+- Lc+- D*(2)pi D*mu D*el D0(2) D0mu D0el |eta|
PythiaB.DecayChannelParameters = [ 4.5, 4.5, -4.5, 4.5,   4.5, 0.0, 0.0,  0.0, 0.0,  2.8]
#--------------------------------------------------------------
# --------  PYTHIA PARAMETERS OPTIMAL FOR BEAUTY PRODUCTION  --
#--------------------------------------------------------------
#  'msel 5' is only for fast tests! 
#  for correct b-producion you should use 'msel 1'
# 'mstj 26 0' = no mixing was defined in Btune as default
# 'mstj 22 2' = no K0S, Lambda0 decays in Pythia - defined in Btune as default

PythiaB.PythiaCommand += [
    "pysubs ckin 7 -4.",
    "pysubs ckin 8 4.",
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

PythiaB.PythiaCommand += ["pysubs ckin 3 5.",
				 "pysubs msel 1"]

#--------------------------------------------------------------
# -------------  DEFINE SELECTION CUTS  -------------
#--------------------------------------------------------------				
PythiaB.flavour =  5.				
#  ------------- Selections on b  quarks   -------------
PythiaB.cutbq = ["4. 4.5 or 4. 4.5"]
#  ------------- LVL1 muon cuts 0=OFF 1=ON -------------
PythiaB.lvl1cut = [ 1.,  4., 2.8]
#  ------------- LVL2 muon/electron cuts  0=OFF 1=ON-------------
PythiaB.lvl2cut = [ 0.,  13.,     6.,   2.8]
#  ------------- Offline cuts 0=OFF 1=ON -------------
PythiaB.offcut = [ 0., 0.5, 2.5, 3., 2.5, 0.5, 2.5]
#  ------------- Number of repeated hadronization mhadr -------------
PythiaB.mhadr =  10
#==============================================================
#
# End of job options file
#
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 10
#--------------------------------------------------------------
#
#---------------------------------------------------------------
# Pool Persistency
#---------------------------------------------------------------
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "StreamEVGEN" )
Stream1.OutputFile = "pythia.bbmu4ch.pool.root"
Stream1.ItemList += [ 'EventInfo#*', 'McEventCollection#*' ]
###############################################################
#
###############################################################
