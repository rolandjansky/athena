# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#min bias sample.
theApp.Dlls  += [ "Pythia_i"]
theApp.TopAlg = ["Pythia"]
AtRndmGenSvc = Service( "AtRndmGenSvc" )
AtRndmGenSvc.Seeds = ["PYTHIA 4789899 989240512", "PYTHIA_INIT 820021 2347532"]
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
Pythia = Algorithm( "Pythia" )
Pythia.PythiaCommand= [	 "pysubs msel 1",
			      "pypars mstp 82 4",
			      "pydat1 mstj 11 3",
			      "pydat1 mstj 22 2",
			      "pydat1 parj 54 -0.07",
			      "pydat1 parj 55 -0.006",
			      "pypars parp 82 1.8",
			      "pypars parp 84 0.5"]
#---------------------------------------------------------------
# Ntuple service output
#---------------------------------------------------------------
#==============================================================
#
# End of job options file
#
###############################################################
