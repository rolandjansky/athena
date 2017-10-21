#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

#
## Based on CaloHiveDeps.py: Algorithm Data Dependencies for CaloBottoUp.py
## With small adjustments to make it work on ESD files

xAODMaker__EventInfoCnvAlg.ExtraInputs =  [('EventInfo','McEventInfo')]
xAODMaker__EventInfoCnvAlg.ExtraOutputs = [('xAOD::EventAuxInfo','EventInfoAux.'),
                                           ('xAOD::EventInfo','EventInfo') ]

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
                         ('xAOD::CaloClusterContainer','CaloTopoCluster')]

