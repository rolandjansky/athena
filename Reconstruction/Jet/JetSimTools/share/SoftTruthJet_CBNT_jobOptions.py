#----------------------------------------------------------------------
#      jobOptions for CBNT for JetRec package
# 

if CBNTAthenaAware:
    CBNT_AthenaAware = Algorithm("CBNT_AthenaAware")
    CBNT_AthenaAware.Members += [ "CBNTAA_Jet/CBNT_SoftTruthJets" ]
    CBNT_ConeTruthJets = Algorithm( "CBNT_SoftTruthJets" )
    CBNT_ConeTruthJets.InputJetCollection = "SoftTruthJets"
    CBNT_ConeTruthJets.BlockTag = "SoftMcJets"
    CBNT_ConeTruthJets.ToolNames = [ "CBNTAA_JetTool" ]
else:
    CBNT_Athena.Members += [ "CBNT_Jet/CBNT_SoftTruthJets" ]
    CBNT_SoftTruthJets = Algorithm( "CBNT_SoftTruthJets" )
    CBNT_SoftTruthJets.InputJetContainer = "SoftTruthJets"
    CBNT_SoftTruthJets.BlockTag = "SoftMcJets"
    CBNT_SoftTruthJets.VarTag = "SoftMcJets"
    CBNT_SoftTruthJets.JetEtMin = -1.*MeV

    
