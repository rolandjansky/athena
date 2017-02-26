
#
## Algorithm Data Dependencies for CaloBottoUp.py
#
### These should be moved to the jobOpts of the individual packages.

xAODMaker__EventInfoCnvAlg.ExtraInputs =  [('EventInfo','McEventInfo')]
xAODMaker__EventInfoCnvAlg.ExtraOutputs = [('xAOD::EventAuxInfo','EventInfoAux.'),
                                           ('xAOD::EventInfo','EventInfo') ]


topSequence.CaloCellMaker.ExtraInputs =   [('EventInfo','McEventInfo'),
                                           ('LArRawChannelContainer','LArRawChannels'),
                                           ('TileRawChannelContainer','TileRawChannelCnt')]
topSequence.CaloCellMaker.ExtraOutputs =  [('CaloCellContainer','AllCalo'),
                                           ('TileCellContainer','MBTSContainer')]


topSequence.CmbTowerBldr.ExtraInputs =  [('CaloCellContainer','AllCalo')]
topSequence.CmbTowerBldr.ExtraOutputs = [('CaloTowerContainer','CombinedTower')]



topSequence.CaloClusterMakerSWCmb.ExtraInputs =  [('CaloTowerContainer','CombinedTower')]
topSequence.CaloClusterMakerSWCmb.ExtraOutputs = [('xAOD::CaloClusterContainer','CombinedCluster'),
                                                  ('xAOD::CaloClusterAuxContainer','CombinedClusterAux.'),
                                                  ('CaloClusterCellLinkContainer','CombinedCluster_links')]



topSequence.CaloTopoCluster.ExtraInputs =  [('CaloCalibrationHitContainer','LArCalibrationHitActive'),
                                            ('CaloCalibrationHitContainer','LArCalibrationHitDeadMaterial'),
                                            ('CaloCalibrationHitContainer','LArCalibrationHitInactive'),
                                            ('CaloCellContainer','AllCalo')]
topSequence.CaloTopoCluster.ExtraOutputs = [('xAOD::CaloClusterContainer','CaloCalTopoClusters'),
                                            ('xAOD::CaloClusterAuxContainer','CaloCalTopoClustersAux.'),
                                            ('xAOD::CaloClusterContainer','CaloTopoCluster'),
                                            ('xAOD::CaloClusterAuxContainer','CaloTopoClusterAux.'),
                                            ('CaloClusterCellLinkContainer','CaloTopoCluster_links'),
                                            ('CaloClusterCellLinkContainer','CaloCalTopoClusters_links')]


StreamESD.ExtraInputs = [('CaloCellContainer','AllCalo'),
                         ('CaloTowerContainer','CombinedTower'), 
                         ('xAOD::CaloClusterContainer','CombinedCluster'),
                         ('xAOD::CaloClusterContainer','CaloTopoCluster')]


# # no different calorimeter clusters yet
# if useJets:
#     topSequence.jetalg4.ExtraInputs = [('xAOD::CaloClusterContainer','CaloTopoCluster')]
#     for x in range(0, nJets):
#         app=str((0.5+x)/10)
#         app=app.replace('.', 'p')
#         # because of ParticleJetMap mess
#         # algDep["jetalg"+app] = [('xAOD::CaloClusterContainer','CaloTopoCluster')]
#         topSequence.getattr("jetalg"+app).ExtraInputs = [('xAOD::JetContainer','AntiKt4EMTopoJets')]
#     StreamESD.ExtraInputs += [('xAOD::JetContainer','AntiKt4EMTopoJets')]
#     for x in range(0, nJets):
#         app=str((0.5+x)/10)
#         app=app.replace('.', 'p')
#         out="AntiKt"+app+"EMTopoJets"
#         StreamESD.ExtraInputs += [('xAOD::JetContainer',out)]

