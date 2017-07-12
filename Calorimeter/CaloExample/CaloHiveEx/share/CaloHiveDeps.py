#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#


#
## Algorithm Data Dependencies for CaloBottoUp.py
#
### These should be moved to the jobOpts of the individual packages.

xAODMaker__EventInfoCnvAlg.ExtraInputs =  [('EventInfo','StoreGateSvc+McEventInfo')]
xAODMaker__EventInfoCnvAlg.ExtraOutputs = [('xAOD::EventAuxInfo','StoreGateSvc+EventInfoAux.'),
                                           ('xAOD::EventInfo','StoreGateSvc+EventInfo') ]


topSequence.CaloCellMaker.ExtraInputs =   [('EventInfo','StoreGateSvc+McEventInfo'),
                                           ('LArRawChannelContainer','StoreGateSvc+LArRawChannels'),
                                           ('TileRawChannelContainer','StoreGateSvc+TileRawChannelCnt')]
topSequence.CaloCellMaker.ExtraOutputs =  [('CaloCellContainer','StoreGateSvc+AllCalo'),
                                           ('TileCellContainer','StoreGateSvc+MBTSContainer')]


topSequence.CmbTowerBldr.ExtraInputs =  [('CaloCellContainer','StoreGateSvc+AllCalo')]
topSequence.CmbTowerBldr.ExtraOutputs = [('CaloTowerContainer','StoreGateSvc+CombinedTower')]



topSequence.CaloClusterMakerSWCmb.ExtraInputs =  [('CaloTowerContainer','StoreGateSvc+CombinedTower')]
topSequence.CaloClusterMakerSWCmb.ExtraOutputs = [('xAOD::CaloClusterContainer','StoreGateSvc+CombinedCluster'),
                                                  ('xAOD::CaloClusterAuxContainer','StoreGateSvc+CombinedClusterAux.'),
                                                  ('CaloClusterCellLinkContainer','StoreGateSvc+CombinedCluster_links')]



topSequence.CaloTopoCluster.ExtraInputs =  [('CaloCalibrationHitContainer','StoreGateSvc+LArCalibrationHitActive'),
                                            ('CaloCalibrationHitContainer','StoreGateSvc+LArCalibrationHitDeadMaterial'),
                                            ('CaloCalibrationHitContainer','StoreGateSvc+LArCalibrationHitInactive'),
                                            ('CaloCellContainer','StoreGateSvc+AllCalo')]
topSequence.CaloTopoCluster.ExtraOutputs = [('xAOD::CaloClusterContainer','StoreGateSvc+CaloCalTopoClusters'),
                                            ('xAOD::CaloClusterAuxContainer','StoreGateSvc+CaloCalTopoClustersAux.'),
                                            ('xAOD::CaloClusterContainer','StoreGateSvc+CaloTopoCluster'),
                                            ('xAOD::CaloClusterAuxContainer','StoreGateSvc+CaloTopoClusterAux.'),
                                            ('CaloClusterCellLinkContainer','StoreGateSvc+CaloTopoCluster_links'),
                                            ('CaloClusterCellLinkContainer','StoreGateSvc+CaloCalTopoClusters_links')]


StreamESD.ExtraInputs = [('CaloCellContainer','StoreGateSvc+AllCalo'),
                         ('CaloTowerContainer','StoreGateSvc+CombinedTower'), 
                         ('xAOD::CaloClusterContainer','StoreGateSvc+CombinedCluster'),
                         ('xAOD::CaloClusterContainer','StoreGateSvc+CaloTopoCluster')]


# # no different calorimeter clusters yet
# if useJets:
#     topSequence.jetalg4.ExtraInputs = [('xAOD::CaloClusterContainer','StoreGateSvc+CaloTopoCluster')]
#     for x in range(0, nJets):
#         app=str((0.5+x)/10)
#         app=app.replace('.', 'p')
#         # because of ParticleJetMap mess
#         # algDep["jetalg"+app] = [('xAOD::CaloClusterContainer','StoreGateSvc+CaloTopoCluster')]
#         topSequence.getattr("jetalg"+app).ExtraInputs = [('xAOD::JetContainer','StoreGateSvc+AntiKt4EMTopoJets')]
#     StreamESD.ExtraInputs += [('xAOD::JetContainer','StoreGateSvc+AntiKt4EMTopoJets')]
#     for x in range(0, nJets):
#         app=str((0.5+x)/10)
#         app=app.replace('.', 'p')
#         out="AntiKt"+app+"EMTopoJets"
#         StreamESD.ExtraInputs += [('xAOD::JetContainer',out)]

