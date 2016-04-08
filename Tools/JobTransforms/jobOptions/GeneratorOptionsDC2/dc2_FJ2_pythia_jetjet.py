# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# dijet production with pythia
theApp.Dlls  += [ "TruthExamples", "Pythia_i" ]
theApp.TopAlg = ["Pythia"]
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
AtRndmGenSvc = Service( "AtRndmGenSvc" )
AtRndmGenSvc.Seeds = ["PYTHIA 4789899 989240512", "PYTHIA_INIT 820021 2347532"]
# AtRndmGenSvc.ReadFromFile = true;
Pythia = Algorithm( "Pythia" )
Pythia.PythiaCommand = [
			      "pysubs msel 0",
			      "pysubs ckin 3 81.5",
			      "pysubs ckin 13 2.5",
			      "pysubs ckin 14 5",
			      "pysubs msub 11 1",
			      "pysubs msub 12 1",
			      "pysubs msub 13 1",
			      "pysubs msub 68 1",
			      "pysubs msub 28 1",
			      "pysubs msub 53 1",
			      "pypars mstp 82 4",
			      "pydat1 mstj 11 3",
			      "pydat1 mstj 22 2",
			      "pydat1 parj 54 -0.07",
			      "pydat1 parj 55 -0.006",
			      "pypars parp 82 1.8",
			      "pypars parp 84 0.5",
			      "pyinit pylisti 12",
			      "pyinit pylistf 1",
			      "pystat 1 3 4 5",
			      "pyinit dumpr 1 5",
			      "pypars mstp 128 0"]
#---------------------------------------------------------------
#---------------------------------------------------------------
