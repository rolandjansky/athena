#----------------------------------------------------------------------
#      jobOptions for CBNT for JetRec package
#

CBNT_Athena.Members += [ "CBNT_Jet/CBNT_KtAtlJets" ]
CBNT_KtAtlJets = Algorithm( "CBNT_KtAtlJets" )
CBNT_KtAtlJets.InputJetContainer = "KtAtlfastJets"
CBNT_KtAtlJets.BlockTag = "KtAtlJets"
CBNT_KtAtlJets.VarTag = "KtAtlJets"
