# This file can be used when the BTagger is run to have more
# information in the CBNT about the jets used for the BTagging

CBNT_Athena = Algorithm( "CBNT_Athena" )
CBNT_Athena.Members += [ "CBNT_Jet/CBNT_BTAGKtTowerJets" ]
CBNT_BTAGKtTowerJets = Algorithm( "CBNT_BTAGKtTowerJets" )
CBNT_BTAGKtTowerJets.InputJetContainer = "BTAGJetCollection"
CBNT_BTAGKtTowerJets.BlockTag = "BTagKtJets"
CBNT_BTAGKtTowerJets.VarTag ="BTagKtJets"
CBNT_BTAGKtTowerJets.OutputLevel = 3

