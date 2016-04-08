# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# makes pythia events with a b jet or pt>15 and |eta|<2.5 
theApp.Dlls += [ "TruthExamples", "PythiaB" ]
theApp.Dlls += [ "GeneratorFilters" ]
theApp.Dlls += [ "GaudiAud" ]
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors  = [ "ChronoAuditor" ]
theApp.TopAlg = ["PythiaB" ,  "BSignalFilter"]
AtRndmGenSvc = Service( "AtRndmGenSvc" )
AtRndmGenSvc.Seeds = ["PYTHIA 4789899 989240512", "PYTHIA_INIT 820021 2347532"]
#--------------------------------------------------------------				 
#              PARAMETERS  SPECIFIC  TO   PYTHIAB
#--------------------------------------------------------------
PythiaB = Algorithm( "PythiaB" )
PythiaB.ForceBDecay = "no"
PythiaB.ForceCDecay = "no"
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
				"pysubs msel 1",
				"mstp 51 7",
                                "pypars mstp 82 4",
                               "pydat1 mstj 11 3",
                                "pydat1 mstj 22 2",
                               "pydat1 parj 54 -0.07",
                              "pydat1 parj 55 -0.006",
                              "pypars parp 82 1.8",
                              "pypars parp 84 0.5"]
#--------------------------------------------------------------
# -------------  DEFINE SELECTION CUTS  -------------
#--------------------------------------------------------------				
#  ------------- Selections on b  quarks   -------------
# simulate  only b-flavour events
PythiaB.flavour =  5.
# PythiaB force exclusive decay channels only on b=-5 side 
# ------------------- b=5  --- and/or ---  b=-5 --------
PythiaB.cutbq = ["15. 2.5 or 15. 2.5"]
#  ------------- LVL1 muon cuts 0=OFF 1=ON -------------
PythiaB.lvl1cut = [ 0.,  6., 2.5]
#  ------------- LVL2 muon/electron cuts  0=OFF 1=ON-------------
PythiaB.lvl2cut = [ 0.,  13.,     6.,   2.5]
#PythiaB.lvl2cut = { 0.,  11.,     6.,   2.5};
#  ------------- Offline cuts 0=OFF 1=ON -------------
PythiaB.offcut = [ 0., 0.5, 2.5, 3., 2.5, 0.5, 2.5]
#  ------------- Number of repeated hadronization mhadr -------------
#PythiaB.mhadr =  14. ;
#  ------------- For how many events store B-chain in NTUPLE -------------
BSignalFilter = Algorithm( "BSignalFilter" )
BSignalFilter.SignaltoNtup = 50
#  ------------- For how many events store B-chain in ascii -------------
PythiaB.SignalDumptoAscii =  50.
