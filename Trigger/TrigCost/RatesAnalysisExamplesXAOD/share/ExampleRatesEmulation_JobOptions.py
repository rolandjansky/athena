# Edit this file to 
include("RatesAnalysisExamplesXAOD/ExampleRates_Configuration.py")

#---- Algorithm Setup -------

algseq = CfgMgr.AthSequencer("AthAlgSeq") #gets the main AthSequencer
algseq += CfgMgr.ExampleRatesEmulation() #adds an instance of the example rates algorithm

algseq.ExampleRatesEmulation.PrescaleXML = PrescaleXML
algseq.ExampleRatesEmulation.DoUniqueRates = DoUniqueRates
algseq.ExampleRatesEmulation.DoHistograms = DoHistograms
algseq.ExampleRatesEmulation.NormaliseHistograms = NormaliseHistograms
algseq.ExampleRatesEmulation.UseBunchCrossingTool = DoRatesVsPositionInTrain
algseq.ExampleRatesEmulation.TargetLuminosity = TargetLuminosity
algseq.ExampleRatesEmulation.EnableLumiExtrapolation = EnableLumiExtrapolation

# The following are only needed for MC
algseq.ExampleRatesEmulation.IsMC = isMC
algseq.ExampleRatesEmulation.MCCrossSection = MCCrossSection
algseq.ExampleRatesEmulation.MCFilterEfficiency = MCFilterEfficiency
algseq.ExampleRatesEmulation.MCKFactor = MCKFactor

print algseq.ExampleRatesEmulation

include("RatesAnalysisExamplesXAOD/ExampleRates_postSetup.py")
