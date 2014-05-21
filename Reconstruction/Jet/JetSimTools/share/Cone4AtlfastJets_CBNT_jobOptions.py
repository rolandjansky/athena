#----------------------------------------------------------------------
#      jobOptions for CBNT for JetRec package
#

CBNT_Athena.Members += [ "CBNT_Jet/CBNT_Cone4AtlJets" ]
CBNT_Cone4AtlJets = Algorithm( "CBNT_Cone4AtlJets" )
CBNT_Cone4AtlJets.InputJetContainer = "Cone4AtlfastJets"
CBNT_Cone4AtlJets.BlockTag = "C4AtlJets"
CBNT_Cone4AtlJets.VarTag = "C4AtlJets"
