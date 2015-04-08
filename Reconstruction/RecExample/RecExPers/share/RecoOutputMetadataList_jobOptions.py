from AthenaCommon.KeyStore import CfgItemList

recoMetadataItemList = CfgItemList("RecoMetadata",
                                    items = ["IOVMetaDataContainer#*",
                                             "xAOD::LumiBlockRangeContainer#*",
	                                     "xAOD::LumiBlockRangeAuxContainer#*",
                                             "xAOD::CutBookkeeperContainer#*",
                                             "xAOD::CutBookkeeperAuxContainer#*",
                                             "ByteStreamMetadataContainer#*"], 
                                    allowWildCard = True )

dfMetadataItemList = CfgItemList("DerivedMetadata",
                                 items = ["xAOD::TriggerMenuContainer#*","xAOD::TriggerMenuAuxContainer#*"],
                                 allowWildCard = True )
dfMetadataItemList.add(recoMetadataItemList())
