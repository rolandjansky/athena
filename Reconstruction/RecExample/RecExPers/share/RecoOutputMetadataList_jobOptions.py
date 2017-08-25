from AthenaCommon.KeyStore import CfgItemList

recoMetadataItemList = CfgItemList("RecoMetadata",
                                    items = ["IOVMetaDataContainer#*",
                                             "xAOD::LumiBlockRangeContainer#*",
	                                     "xAOD::LumiBlockRangeAuxContainer#*",
                                             "xAOD::CutBookkeeperContainer#*",
                                             "xAOD::CutBookkeeperAuxContainer#*",
                                             "ByteStreamMetadataContainer#*",
                                             "xAOD::TruthMetaDataContainer#TruthMetaData",
                                             "xAOD::TruthMetaDataAuxContainer#TruthMetaDataAux."], 
                                    allowWildCard = True )

dfMetadataItemList = CfgItemList("DerivedMetadata",
                                 items = ["xAOD::TriggerMenuContainer#*",
                                          "xAOD::TriggerMenuAuxContainer#*",
                                          "xAOD::FileMetaData#FileMetaData",
                                          "xAOD::FileMetaDataAuxInfo#FileMetaDataAux.",
                                          "xAOD::TruthMetaDataContainer#TruthMetaData",
                                          "xAOD::TruthMetaDataAuxContainer#TruthMetaDataAux."],
                                 allowWildCard = True )

dfMetadataItemList.add(recoMetadataItemList())
