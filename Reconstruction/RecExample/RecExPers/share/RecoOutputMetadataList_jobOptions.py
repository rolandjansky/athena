# Itemlist for metadata to be written by RecExCommon

from AthenaCommon.KeyStore import CfgItemList

# for the FileMetaData for MC
from RecExConfig.InputFilePeeker import inputFileSummary
if inputFileSummary['evt_type'][0] == 'IS_SIMULATION':
  svcMgr.IOVDbSvc.Folders += ['/Simulation/Parameters']

# ESD
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

# AOD extra
dfMetadataItemList = CfgItemList("DerivedMetadata",
                                 items = ["xAOD::TriggerMenuContainer#*",
                                          "xAOD::TriggerMenuAuxContainer#*",
                                          "xAOD::FileMetaData#FileMetaData",
                                          "xAOD::FileMetaDataAuxInfo#FileMetaDataAux.",
                                          "xAOD::TruthMetaDataContainer#TruthMetaData",
                                          "xAOD::TruthMetaDataAuxContainer#TruthMetaDataAux."],
                                 allowWildCard = True )

# AOD = ESD + AOD extra
dfMetadataItemList.add(recoMetadataItemList())
