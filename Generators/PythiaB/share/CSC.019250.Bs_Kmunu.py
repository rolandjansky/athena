###############################################################
#
# Job options file for generation of B-events 
#  in user selected exclusive channel
#
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
#              PARAMETERS  SPECIFIC  TO   PYTHIAB
#--------------------------------------------------------------
#PythiaB.ForceBDecay = "no";
PythiaB = topAlg.PythiaB
PythiaB.ForceCDecay = "no"
#--------------------------------------------------------------				 
# -------------  FORCE   YOUR  B CHANNEL  HERE -------------
#--------------------------------------------------------------
# To force your B-decay channels decomment following 2 lines:
include( "PythiaB/CloseAntibQuark.py" )

PythiaB.ForceBDecay = "yes"
#   T H R E E   E X A M P L E S    T O   O P E N    Y O U R    C H A N N E L 				 
#   Decomment one of following examples or create your analogically
# open your exclusive channel here  Bs -> J/psi(mumu) phi 				 
# PythiaB.PythiaCommand += ["pydat3 mdme 982 1 1",    
# 				"pydat3 mdme 858 1 0",
# 				"pydat3 mdme 860 1 0"        ]

# open your exclusive channel here  B0 -> Kmunu 				 
PythiaB.PythiaCommand += ["pydat3 mdme 962 1 1",
                          "pydat3 kfdp 962 1 -321",
                          "pydat3 kfdp 962 2 13",
                          "pydat3 kfdp 962 3 14"        ]



# open your exclusive channel here  Bs -> Ds pi with Ds->phi pi
##include "Dsphipi.txt" 				 
#PythiaB.PythiaCommand += {"pydat3 mdme 967 1 1",
#                                "pydat3 mdme 831 1 1"      };
#PythiaB.ForceCDecay = "yes";                                
# open your exclusive channel here  Bs -> J/psi(mumu) K0 
#PythiaB.PythiaCommand += {"pydat3 mdme 889 1 1",    
#				"pydat3 mdme 858 1 0",
#				"pydat3 mdme 860 1 0"        };
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

PythiaB.PythiaCommand += ["pysubs ckin 3 15.",
                          "pysubs ckin 9 -3.5",
                          "pysubs ckin 10 3.5",
                          "pysubs ckin 11 -3.5",
                          "pysubs ckin 12 3.5",
                          "pysubs msel 1"]
#--------------------------------------------------------------
# -------------  DEFINE SELECTION CUTS  -------------
#--------------------------------------------------------------				
#  ------------- Selections on b  quarks   -------------
# simulate  only b-flavour events
PythiaB.flavour =  5.
# PythiaB force exclusive decay channels only on b=-5 side 
# ------------------- b=5  --- and/or ---  b=-5 --------
PythiaB.cutbq = ["0. 102.5 and 10. 2.5"]
#  ------------- LVL1 muon cuts 0=OFF 1=ON -------------
PythiaB.lvl1cut = [ 1.,  6., 2.5]
#  ------------- LVL2 muon/electron cuts  0=OFF 1=ON-------------
PythiaB.lvl2cut = [ 0.,  13.,     6.,   2.5]
#PythiaB.lvl2cut = { 0.,  11.,     6.,   2.5};
#  ------------- Offline cuts 0=OFF 1=ON -------------
PythiaB.offcut = [ 1., 4., 2.5, 6., 2.5, 0.5, 2.5]
#  ------------- Number of repeated hadronization mhadr -------------
PythiaB.mhadr =  6. 
