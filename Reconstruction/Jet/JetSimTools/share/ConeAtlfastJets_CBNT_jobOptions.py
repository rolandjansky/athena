#----------------------------------------------------------------------
#      jobOptions for CBNT for JetRec package
#

CBNT_Athena.Members += [ "CBNT_Jet/CBNT_ConeAtlJets" ]
CBNT_ConeAtlJets = Algorithm( "CBNT_ConeAtlJets" )
CBNT_ConeAtlJets.InputJetContainer = "ConeAtlfastJets"
CBNT_ConeAtlJets.BlockTag = "CAtlJets"
CBNT_ConeAtlJets.VarTag = "CAtlJets"
