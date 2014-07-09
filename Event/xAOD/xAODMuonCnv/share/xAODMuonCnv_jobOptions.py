# $Id: ClusterCreator_jobOptions.py 297184 2013-10-03 11:18:41Z krasznaa $

# Set up the reading of the input AOD:
FNAME = "/home/emoyse/mc12_8TeV.147807.PowhegPythia8_AU2CT10_Zmumu.merge.AOD.e1169_s1469_s1470_r3658_r3549_tid01085597_00/AOD.01085597._000308.pool.root.1"
include( "AthenaPython/iread_file.py" )
# from RecExConfig.RecFlags  import rec
# from AthenaCommon.BeamFlags import jobproperties
# from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
# from AthenaCommon.GlobalFlags import globalflags

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

from MuonCombinedRecExample.MuonCombinedKeys import MuonCombinedKeys as MuonCbKeys

#InDet
from xAODTrackParticleCnv.xAODTrackParticleCnvConf import xAODMaker__TrackParticleCnvAlg
alg = xAODMaker__TrackParticleCnvAlg()
theJob += alg

#SA
from xAODTrackParticleCnv.xAODTrackParticleCnvConf import xAODMaker__TrackParticleCnvAlg
alg1 = xAODMaker__TrackParticleCnvAlg("TrackParticleCnvAlg_ExtrapolatedMS")
alg1.AODContainerName = MuonCbKeys.ExtrapolatedMSParticles() #ExtrapolatedMuonSpectrometerParticles
alg1.xAODContainerName = "x"+MuonCbKeys.ExtrapolatedMSParticles()
alg1.OutputLevel = VERBOSE
theJob += alg1

# StatCombined
from xAODTrackParticleCnv.xAODTrackParticleCnvConf import xAODMaker__TrackParticleCnvAlg
alg3 = xAODMaker__TrackParticleCnvAlg("TrackParticleCnvAlg_StatCombined")
alg3.AODContainerName = MuonCbKeys.StatCombinedParticles() #xStatCombinedMuonParticles
alg3.xAODContainerName = "x"+MuonCbKeys.StatCombinedParticles()
alg3.OutputLevel = VERBOSE
theJob += alg3

# Combined
from xAODTrackParticleCnv.xAODTrackParticleCnvConf import xAODMaker__TrackParticleCnvAlg
alg4 = xAODMaker__TrackParticleCnvAlg("TrackParticleCnvAlg_CombinedFit")
alg4.AODContainerName = MuonCbKeys.CombinedFitParticles() #CombinedFitParticles
alg4.xAODContainerName = "x"+MuonCbKeys.CombinedFitParticles()
alg4.OutputLevel = VERBOSE
theJob += alg4

# ST
from xAODTrackParticleCnv.xAODTrackParticleCnvConf import xAODMaker__TrackParticleCnvAlg
alg5 = xAODMaker__TrackParticleCnvAlg("TrackParticleCnvAlg_SegmentTagTrackParticles")
alg5.AODContainerName = MuonCbKeys.SegmentTagTrackParticles() #SegmentTagTrackParticles
alg5.xAODContainerName = "x"+MuonCbKeys.SegmentTagTrackParticles()
alg5.OutputLevel = VERBOSE
theJob += alg5

#Muons
from xAODMuonCnv.xAODMuonCnvConf import xAODMaker__MuonCnvAlg
muonAlg = xAODMaker__MuonCnvAlg()
muonAlg.OutputLevel = VERBOSE
theJob += muonAlg

# This algorithm *must* always be last in the sequence:
from xAODCreatorAlgs.xAODCreatorAlgsConf import xAODMaker__EventFormatCreator
alg5 = xAODMaker__EventFormatCreator()
#alg5.OutputLevel = VERBOSE
theJob += alg5

# Create a POOL output file with the StoreGate contents:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolStream( "StreamXAOD", "xAOD.pool.root" )

# Set up its contents:
# xaodStream.AddItem( "xAOD::CaloClusterContainer_v1#*" )
# xaodStream.AddItem( "xAOD::CaloClusterAuxContainer_v1#*" )
xaodStream.AddItem( "xAOD::TrackParticleContainer_v1#*" )
xaodStream.AddItem( "xAOD::TrackParticleAuxContainer_v1#*" )
xaodStream.AddItem( "xAOD::MuonContainer_v1#*" )
xaodStream.AddItem( "xAOD::MuonAuxContainer_v1#*" )
xaodStream.AddMetaDataItem( "xAOD::EventFormat_v1#*" )
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
