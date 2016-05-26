infile = 'esd/ESD-20.1.7.2.pool.root'
keys = [
    #CaloClusterCellLinkContainer
    'InDetTrackParticlesAssociatedClusters_links@10',
    'MuonClusterCollection_links@10',
    'LArClusterEM_links@10',
    'egammaClusters_links@10',
    'LArClusterEM7_11Nocorr_links@10',
    'CaloCalTopoClusters_links@10',
         ]

#from AthenaCommon.JobProperties import jobproperties
#jobproperties.Global.DetDescrVersion = 'ATLAS-GEO-10-00-00'

from AthenaCommon.DetFlags      import DetFlags
DetFlags.detdescr.Calo_setOn()
noMuon = True
noID = True
include ('AthenaPoolUtilities/TPCnvTest.py')

