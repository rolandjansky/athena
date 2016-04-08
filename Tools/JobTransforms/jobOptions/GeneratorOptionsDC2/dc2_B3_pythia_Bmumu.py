# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###############################################################
#
# Job options file for generation of B-events 
#  in user selected exclusive channel
theApp.Dlls += [ "TruthExamples", "PythiaB" ]
theApp.Dlls += [ "GeneratorFilters" ]
#--------------------------------------------------------------
# Algorithms 
#--------------------------------------------------------------
theApp.TopAlg = ["PythiaB" ,  "BSignalFilter"]
theApp.ExtSvc += ["AtRndmGenSvc"]
#--------------------------------------------------------------
# Number of events to be accepted !! (default is 10)
# re-written if use B job submition script
# RunNumber, FirstEvent re-written if use B job submition script
#--------------------------------------------------------------
theApp.EvtMax = 25000
EventSelector = Service( "EventSelector" )
EventSelector.RunNumber   = 1
EventSelector.FirstEvent  = 1
#--------------------------------------------------------------
#User random number seeds - re-written if use B job submition script
#--------------------------------------------------------------
AtRndmGenSvc = Service( "AtRndmGenSvc" )
AtRndmGenSvc.Seeds = ["PYTHIA 4789899 989240512", "PYTHIA_INIT 820021 2347532"]
# AtRndmGenSvc.ReadFromFile = true;
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel               = 3
#--------------------------------------------------------------				 
#              PARAMETERS  SPECIFIC  TO   PYTHIAB
#--------------------------------------------------------------
#PythiaB.ForceBDecay = "no";
PythiaB = Algorithm( "PythiaB" )
PythiaB.ForceCDecay = "no"
#--------------------------------------------------------------				 
# -------------  FORCE   YOUR  B CHANNEL  HERE -------------
#--------------------------------------------------------------
# To force your B-decay channels decomment following 2 lines:
include( "PythiaB/CloseAntibQuark.py" )

PythiaB.ForceBDecay = "yes"
#   T H R E E   E X A M P L E S    T O   O P E N    Y O U R    C H A N N E L 				 
#   Decomment one of following examples or create your analogically
# open your exclusive channel here  Bs -> mumu 				 
PythiaB.PythiaCommand += ["pydat3 mdme 977 1 1",    
				"pydat3 kfdp 977 1 13",
				"pydat3 kfdp 977 2 -13"        ]
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
# 'mstj 26 0' = no mixing
# 'mstp 51 1' = CTEQ3(<-use this one)  'mstp 51 7' = CTEQ5
include( "PythiaB/Btune.py" )

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
PythiaB.offcut = [ 1., 0.5, 2.5, 6., 2.5, 0.5, 2.5]
#  ------------- Number of repeated hadronization mhadr -------------
PythiaB.mhadr =  14. 
#  ------------- For how many events store B-chain in NTUPLE -------------
BSignalFilter = Algorithm( "BSignalFilter" )
BSignalFilter.SignaltoNtup = 50
#  ------------- For how many events store B-chain in ascii -------------
PythiaB.SignalDumptoAscii =  5.
# End of job options file
#
###############################################################
