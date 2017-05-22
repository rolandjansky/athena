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

from GeneratorFilters.GeneratorFiltersConf import MultiLeptonFilter
topAlg += MultiLeptonFilter()

#--------------------------------------------------------------				 
#              PARAMETERS  SPECIFIC  TO   PYTHIAB
#--------------------------------------------------------------
PythiaB = topAlg.PythiaB
PythiaB.ForceCDecay = "no"
PythiaB.ForceBDecay = "yes"
#--------------------------------------------------------------				 
# -------------  FORCE   YOUR  B CHANNEL  HERE -------------
#--------------------------------------------------------------
# To force your B-decay channels decomment following 2 lines:
include( "PythiaB/CloseAntibQuark.py" )

PythiaB.PythiaCommand += ["pydat3 mdme 889 1 1",    
                          "pydat3 mdme 858 1 1",
				"pydat3 mdme 859 1 0",
				"pydat3 mdme 860 1 0"        ]
#--------------------------------------------------------------
# --------  PYTHIA PARAMETERS OPTIMAL FOR BEAUTY PRODUCTION  --
#--------------------------------------------------------------

include( "PythiaB/Btune.py" )

PythiaB.PythiaCommand += [ # Arthur Moraes tuning
    # lambda value for alpha_s
    "pydat1 parj 81 0.14",
    # affects the amount of color reconnection
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

PythiaB.PythiaCommand += ["pysubs ckin 3 15.",
                                "pysubs ckin 9 -3.5",
				"pysubs ckin 10 3.5",
				"pysubs ckin 11 -3.5",
				"pysubs ckin 12 3.5",
                                "pydat1 mstj 22 1",
          "pysubs msel 1"]
#--------------------------------------------------------------
# -------------  DEFINE SELECTION CUTS  -------------
#--------------------------------------------------------------				
#  ------------- Selections on b  quarks   -------------
# simulate  only b-flavour events
PythiaB.flavour =  5.
# PythiaB force exclusive decay channels only on b=-5 side 
# ------------------- b=5  --- and/or ---  b=-5 --------
PythiaB.cutbq = ["10. 2.5 and 10. 2.5"]
#PythiaB.cutbq = ["6. 2.5 and 6. 2.5"]
#  ------------- LVL1 muon cuts 0=OFF 1=ON -------------
PythiaB.lvl1cut = [ 1.,  6., 2.5]
#  ------------- LVL2 muon/electron cuts  0=OFF 1=ON-------------
PythiaB.lvl2cut = [ 0.,  13.,     6.,   2.5]
#  ------------- Offline cuts 0=OFF 1=ON -------------
PythiaB.offcut = [ 1., 0.0, 102.5, 0., 102.5, 3.0, 2.5]
#  ------------- Number of repeated hadronization mhadr -------------
PythiaB.mhadr =  15. 

# Add the filters:
BSignalFilter = topAlg.BSignalFilter
BSignalFilter.LVL1MuonCutOn = TRUE
BSignalFilter.LVL1MuonCutPT = 6000.0
BSignalFilter.LVL1MuonCutEta = 2.5
BSignalFilter.Cuts_Final_e_switch = TRUE
BSignalFilter.Cuts_Final_e_pT = 3000.0
BSignalFilter.Cuts_Final_e_eta = 2.5

MultiLeptonFilter = topAlg.MultiLeptonFilter
#MultiLeptonFilter.Ptcut = 3.0*GeV
MultiLeptonFilter.Ptcut = 3000.0
MultiLeptonFilter.Etacut = 2.5
MultiLeptonFilter.NLeptons = 3

###############################################################
# Add POOL persistency
try:
     Stream1.RequireAlgs +=  [ "BSignalFilter" ]
except Exception, e:
     pass

#==============================================================
#
# End of job options file
#
###############################################################
