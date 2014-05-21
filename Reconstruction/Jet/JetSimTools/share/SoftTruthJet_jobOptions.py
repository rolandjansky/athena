#----------------------------------------------------------------------
#      jobOptions for JetRec package
#
#----------------------------------------------------------------------
#----------------------------------------------------------------------
# DLL Libraries
#----------------------------------------------------------------------
theApp.Dlls += [ "JetRec" ]
#----------------------------------------------------------------------
# Algorithm steering
#----------------------------------------------------------------------

theApp.topAlg += [ "JetAlgorithm/SoftComponents" ]    
SoftComponents = Algorithm( "SoftComponents" )
SoftComponents.JetCollectionName = "SoftTruthJets"
SoftComponents.AlgTools = [
	"JetSoftTruthTool/SoftTool" ]

#----------------------------------------------------------------------
# AlgTool steering
#----------------------------------------------------------------------
#
SoftComponents.SoftTool.MinPt                = 0.*MeV
SoftComponents.SoftTool.MaxEta               = 5.0
SoftComponents.SoftTool.IncludeMuons         = False
SoftComponents.SoftTool.InputTJetLoc         = "Cone4TruthJets"
SoftComponents.SoftTool.AddInfo              = False



