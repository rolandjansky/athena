###############################################################
#
# Job options file for generation of B-events 
#  in user selected exclusive channel
#
#==============================================================
#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------
from EvgenJobOptions.PythiaBEvgenConfig import evgenConfig
evgenConfig.minevents = 500
#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topAlg = AlgSequence("TopAlg")

include ( "EvgenJobOptions/MC8_PythiaB_Common.py" )

#--------------------------------------------------------------	
#              PARAMETERS  SPECIFIC  TO   PYTHIAB
#--------------------------------------------------------------
PythiaB.ForceCDecay = "no"
#--------------------------------------------------------------	
# -------------  FORCE   YOUR  B CHANNEL  HERE -------------
#--------------------------------------------------------------
# To force your B-decay channels decomment following 2 lines:
include( "EvgenJobOptions/MC8_PythiaB_CloseAntibQuark.py" )

PythiaB.ForceBDecay = "yes"
#   T H R E E   E X A M P L E S    T O   O P E N    Y O U R    C H A N N E L 
#   Decomment one of following examples or create your analogically
# open your exclusive channel here  Bs -> J/psi(mumu) phi 
PythiaB.PythiaCommand += ["pydat3 mdme 982 1 1",    
 				"pydat3 mdme 858 1 0",
 				"pydat3 mdme 860 1 0"        ]
# This phis which have originated from Bs to decay into two kaons. Other phis 
# are left to decay as normal. Comment this line to prevent the phi forcing. 
# See user_finsel.F (in src directory) for more such forcing options.   
PythiaB.ForceDecayChannel = "BsAngles"
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
#-------------------------------------------------------------
#  'msel 5' is only for fast tests! 
#  for correct b-producion you should use 'msel 1'
# 'mstj 26 0' = no mixing was defined in Btune as default
# 'mstj 22 2' = no K0S, Lambda0 decays in Pythia - defined in Btune as default

include( "EvgenJobOptions/MC8_PythiaB_Btune.py" )

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
PythiaB.lvl1cut = [ 0.,  6., 2.5]
#  ------------- LVL2 muon/electron cuts  0=OFF 1=ON-------------
PythiaB.lvl2cut = [ 0.,  13.,     6.,   2.5]
#PythiaB.lvl2cut = { 0.,  11.,     6.,   2.5};
#  ------------- Offline cuts 0=OFF 1=ON -------------
PythiaB.offcut = [ 1., 0.0, 102.5, 0., 102.5, 0., 102.5]
#  ------------- Number of repeated hadronization mhadr -------------
PythiaB.mhadr =  40. 
#==============================================================
#
# End of job options file
#
###############################################################

