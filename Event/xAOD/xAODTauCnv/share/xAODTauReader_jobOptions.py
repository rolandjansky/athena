# $Id: xAODTauReader_jobOptions.py 632174 2014-11-29 15:23:36Z krasznaa $

# Set up the reading of a file:
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [
    "AOD.pool.root"
    ]

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Add the test algorithm:
theJob += CfgMgr.xAODMaker__TauJetReaderAlg( "TauJetReaderAlg",
                                             OutputLevel = DEBUG )

# Write out a file with the "new" tau objects:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolRootStream( "StreamAOD", "newAOD.pool.root" )
xaodStream.AddItem( [ "xAOD::TauJetContainer#*",
                      "xAOD::TauJetAuxContainer#*" ] )

# Some additional settings:
theApp.EvtMax = 10
