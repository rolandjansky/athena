# Itemlist for metadata to be written by RecExCommon

from AthenaCommon.KeyStore import CfgItemList

# for the FileMetaData for MC
from PyUtils.MetaReaderPeeker import metadata
if metadata['eventTypes'][0] == 'IS_SIMULATION' \
    and len(metadata) > 0 \
    and '/Simulation/Parameters' in metadata:
    svcMgr.IOVDbSvc.Folders += ['/Simulation/Parameters']

# ESD
recoMetadataItemList = CfgItemList("RecoMetadata",
                                    items = ["xAOD::TriggerMenuContainer#*",
                                             "xAOD::TriggerMenuAuxContainer#*",
                                             "xAOD::TriggerMenuJsonContainer#*",
                                             "xAOD::TriggerMenuJsonAuxContainer#*",
                                             "IOVMetaDataContainer#*",
                                             "xAOD::LumiBlockRangeContainer#*",
	                                     "xAOD::LumiBlockRangeAuxContainer#*",
#                                             "xAOD::CutBookkeeperContainer#IncompleteCutBookkeepers",
#                                             "xAOD::CutBookkeeperAuxContainer#IncompleteCutBookkeepersAux.",
#                                             "xAOD::CutBookkeeperContainer#CutBookkeepers",
#                                             "xAOD::CutBookkeeperAuxContainer#CutBookkeepersAux.",
                                             "xAOD::CutBookkeeperContainer#*",
                                             "xAOD::CutBookkeeperAuxContainer#*",
                                             "ByteStreamMetadataContainer#*",
                                             "xAOD::TruthMetaDataContainer#TruthMetaData",
                                             "xAOD::TruthMetaDataAuxContainer#TruthMetaDataAux."], 
                                    allowWildCard = True )

# AOD extra
dfMetadataItemList = CfgItemList("DerivedMetadata",
                                 items = ["xAOD::FileMetaData#FileMetaData",
                                          "xAOD::FileMetaDataAuxInfo#FileMetaDataAux.",
                                          "xAOD::TruthMetaDataContainer#TruthMetaData",
                                          "xAOD::TruthMetaDataAuxContainer#TruthMetaDataAux."],
                                 allowWildCard = True )

# AOD = ESD + AOD extra
dfMetadataItemList.add(recoMetadataItemList())
