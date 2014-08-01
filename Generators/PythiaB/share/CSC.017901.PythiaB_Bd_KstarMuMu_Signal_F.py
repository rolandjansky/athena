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
from GeneratorFilters.GeneratorFiltersConf import BSignalFilter
topAlg += BSignalFilter()

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
# T O   O P E N    T H E    U S E R    C H A N N E L
# For Bd -> Kstar(892) mu+ mu- 
#
PythiaB.PythiaCommand += ["pydat3 mdme 875 1 1", 
                                "pydat3 kfdp 875 1 13",
                                "pydat3 kfdp 875 2 -13",
                                "pydat3 kfdp 875 3 313",
                                "pydat3 kfdp 875 4 0",
                                "pydat3 kfdp 875 5 0",
                                "pydat3 brat 875 0.0000001"     ]
PythiaB.ForceDecayChannel = "BdKstarMuMu"
# lvl1 and lvl2 cuts pt_L1 eta_L1 pt_L2 eta_L2
PythiaB.DecayChannelParameters = [1., 6.0, 2.5, 1., 6.0, 2.5] ;
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
# for BsPhiMuMu, BdKstarMuMu BsGammaMuMu lvl1,lvl2 must be OFF
PythiaB.lvl1cut = [ 0.,  6., 2.5]
#  ------------- LVL2 muon/electron cuts  0=OFF 1=ON-------------
PythiaB.lvl2cut = [ 0.,  13.,     6.,   2.5]
#PythiaB.lvl2cut = { 0.,  11.,     6.,   2.5};
# ???  ------------- Offline cuts 0=OFF 1=ON -------------
PythiaB.offcut = [ 0., 0.5, 2.5, 3., 2.5, 0.5, 2.5]
#  ------------- Number of repeated hadronization mhadr -------------
PythiaB.mhadr =  4. 
#  ------------- For how many events store B-chain in NTUPLE -------------
BSignalFilter = topAlg.BSignalFilter
BSignalFilter.Cuts_Final_hadrons_switch = TRUE
BSignalFilter.Cuts_Final_hadrons_pT = 500.0
BSignalFilter.Cuts_Final_hadrons_eta = 2.5
BSignalFilter.BParticle_cuts = 511
###############################################################
# Add POOL persistency
try:
     Stream1.RequireAlgs += ["BSignalFilter"]
except Exception, e:
     pass
#==============================================================
#
# End of job options file
#
###############################################################
