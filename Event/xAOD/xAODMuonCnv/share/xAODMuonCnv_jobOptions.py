# $Id: ClusterCreator_jobOptions.py 297184 2013-10-03 11:18:41Z krasznaa $

# Set up the reading of the input AOD:
FNAME = "root://eosatlas.cern.ch//eos/atlas/atlasdatadisk/rucio/valid1/36/d9/AOD.01447493._000116.pool.root.1"

include( "AthenaPython/iread_file.py" )
# from RecExConfig.RecFlags  import rec
# from AthenaCommon.BeamFlags import jobproperties
# from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
# from AthenaCommon.GlobalFlags import globalflags

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Create a POOL output file with the StoreGate contents:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolStream( "StreamAOD", "xAOD.pool.root" )

# Set up its contents:
# xaodStream.AddItem( "xAOD::CaloClusterContainer_v1#*" )
# xaodStream.AddItem( "xAOD::CaloClusterAuxContainer_v1#*" )

xaodStream.AddItem( "xAOD::TruthParticleContainer#*")
xaodStream.AddItem( "xAOD::TruthParticleAuxContainer#*")
xaodStream.AddItem( "xAOD::TrackParticleContainer#*" )
xaodStream.AddItem( "xAOD::TrackParticleAuxContainer#*" )
xaodStream.AddItem( "xAOD::MuonContainer#*" )
xaodStream.AddItem( "xAOD::MuonAuxContainer#*" )
xaodStream.AddItem( "xAOD::MuonSegmentContainer#*")
xaodStream.AddItem( "xAOD::MuonSegmentAuxContainer#*")
xaodStream.AddItem( "xAOD::EventInfo#*" )
xaodStream.AddItem( "xAOD::EventAuxInfo#*" )
xaodStream.Print()

# Split all branches:
ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [
    "DEFAULT_SPLITLEVEL='99'" ]

# Force POOL to just simply use the StoreGate keys as branch names:
ServiceMgr.AthenaPoolCnvSvc.SubLevelBranchName = "<key>"

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = -1
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
# ServiceMgr.AthenaSealSvc.OutputLevel = VERBOSE

#from VP1Algs.VP1AlgsConf import VP1Alg
#theJob += VP1Alg()
