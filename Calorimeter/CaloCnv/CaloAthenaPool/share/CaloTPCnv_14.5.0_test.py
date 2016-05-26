infile = 'aod/AOD-14.5.0/AOD-14.5.0-full.pool.root'
keys = [
    #CaloCellLinkContainer_p2
    'MuonClusterCollection_Link',
    'egClusterCollection_Link',

    #CaloClusterContainer_p4
    'HLT@10',
    'LArClusterEMFrwd@10',
    'MuonClusterCollection@10',
    'Tau1P3PCellCluster@10',
    'Tau1P3PCellEM012ClusterContainer@10',
    'Tau1P3PPi0ClusterContainer@10',
    'LArClusterEMSofte@10',
    'egClusterCollection@10',
    'HLT_TrigCaloClusterMaker@10',
    'EMTopoCluster430@10',
    'CaloCalTopoCluster@10',

    #CaloTowerContainer_p1
    'HLT_TrigCaloTowerMaker',

    #CaloCompactCellContainer
    #'AODCellContainer',
         ]

#from AthenaCommon.JobProperties import jobproperties
#jobproperties.Global.DetDescrVersion = 'ATLAS-GEO-10-00-00'

from AthenaCommon.DetFlags      import DetFlags
DetFlags.detdescr.Calo_setOn()
noMuon = True
noID = True
include ('AthenaPoolUtilities/TPCnvTest.py')

