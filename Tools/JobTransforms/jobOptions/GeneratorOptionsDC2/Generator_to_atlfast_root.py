# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

include( "AtlfastAlgs/Atlfast_CBNT.py" )

theApp.Dlls += [ "CBNT_Truth" ]
CBNT_Athena = Algorithm( "CBNT_Athena" )
CBNT_Athena.Members += [ "CBNT_SpclMC"]; 
CBNT_Athena.Members += [ "Atlfast::CBNT_Atlfast/CBNT_Atlfast" ]
CBNT_Atlfast = Algorithm( "CBNT_Atlfast" )
CBNT_Atlfast.NtupleLocID="/FILE1/CBNT/3333"
#switch on/off ntuple blocks
CBNT_Atlfast.FillIsolatedElectrons = TRUE
CBNT_Atlfast.FillIsolatedPhotons   = TRUE
CBNT_Atlfast.FillIsolatedMuons     = TRUE
CBNT_Atlfast.FillNonIsolatedMuons  = TRUE
CBNT_Atlfast.FillJets              = TRUE
CBNT_Atlfast.FillJetsB             = TRUE
CBNT_Atlfast.FillHistory           = FALSE
CBNT_Atlfast.FillEventData         = TRUE
GlobalEventDataMaker = Algorithm( "GlobalEventDataMaker" )
GlobalEventDataMaker.Invisibles += [1000022]; # make susy LSP invisible
AtlfastB = Algorithm( "AtlfastB" )
AtlfastB.TauEff=1.0; # put this to 1.0 to simplify validation
#--------------------------------------------------------------
# Setup CBNT
#--------------------------------------------------------------
theApp.Dlls += [ "CBNT_Utils" ]
theApp.Dlls += [ "CBNT_Athena" ]
theApp.TopAlg += ["CBNT_Athena"]
# Histogram & Ntuple Persistency
#--------------------------------------------------------------
theApp.Dlls                += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
#--------------------------------------------------------------
# NTuple output file
#--------------------------------------------------------------
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output = [ "FILE1 DATAFILE='atlfast.ntup' OPT='NEW' TYP='ROOT'" ]; 
