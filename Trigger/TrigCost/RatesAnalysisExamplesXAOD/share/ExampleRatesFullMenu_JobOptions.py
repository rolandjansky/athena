# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Edit this file too 
include("RatesAnalysisExamplesXAOD/ExampleRates_Configuration.py")

#---- Algorithm Setup -------

algseq = CfgMgr.AthSequencer("AthAlgSeq") #gets the main AthSequencer
algseq += CfgMgr.ExampleRatesFullMenu() #adds an instance of the example rates algorithm

algseq.ExampleRatesFullMenu.PrescaleXML = PrescaleXML
algseq.ExampleRatesFullMenu.DoUniqueRates = DoUniqueRates
algseq.ExampleRatesFullMenu.DoHistograms = DoHistograms
algseq.ExampleRatesFullMenu.NormaliseHistograms = NormaliseHistograms
algseq.ExampleRatesFullMenu.UseBunchCrossingTool = DoRatesVsPositionInTrain
algseq.ExampleRatesFullMenu.TargetLuminosity = TargetLuminosity
algseq.ExampleRatesFullMenu.VetoStartOfTrain = VetoStartOfTrain
algseq.ExampleRatesFullMenu.EnableLumiExtrapolation = EnableLumiExtrapolation

# The following are only needed for MC
algseq.ExampleRatesFullMenu.IsMC = isMC
algseq.ExampleRatesFullMenu.MCCrossSection = MCCrossSection
algseq.ExampleRatesFullMenu.MCFilterEfficiency = MCFilterEfficiency
algseq.ExampleRatesFullMenu.MCKFactor = MCKFactor

print algseq.ExampleRatesFullMenu

include("RatesAnalysisExamplesXAOD/ExampleRates_postSetup.py")
