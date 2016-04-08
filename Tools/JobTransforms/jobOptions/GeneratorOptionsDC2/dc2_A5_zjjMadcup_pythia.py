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
Pythia.PythiaCommand = ["pyinit user madcup"]
#---------------------------------------------------------------
#---------------------------------------------------------------
