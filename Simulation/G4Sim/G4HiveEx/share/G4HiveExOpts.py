#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

# 
## G4HiveExOpts.py - an example job options for multi-threaded simulation
## in AthenaHive. This configuration can only be run if Hive is enabled.
## Run it by specifying the number of threads at the command line:
## $ athena --threads=4 ./G4HiveExOpts.py
#

from AthenaCommon.Logging import log as msg

from AthenaCommon.ConcurrencyFlags import jobproperties as jp
nThreads = jp.ConcurrencyFlags.NumThreads()
if (nThreads < 1) :
   msg.fatal('numThreads must be >0. Did you set the --threads=N option?')
   sys.exit(AthenaCommon.ExitCodes.CONFIGURATION_ERROR)

#
## Override Setup for Hive
#
msgFmt = "% F%40W%S%5W%e%s%7W%R%T %0W%M"
svcMgr.MessageSvc.Format = msgFmt
# svcMgr.MessageSvc.useColors = True
# svcMgr.AthenaHiveEventLoopMgr.OutputLevel = DEBUG

#
## Override defaults. otherwise these are ALL EQUAL to the number of threads
## set with the command line opt --threads=N
#

# numStores = 1
# svcMgr.EventDataSvc.NSlots = numStores

from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.ShowControlFlow( True )
AlgScheduler.ShowDataDependencies( True )
# AlgScheduler.OutputLevel( DEBUG )
# AlgScheduler.setThreadPoolSize( nThreads )

# Thread pool service and initialization
from GaudiHive.GaudiHiveConf import ThreadPoolSvc
svcMgr += ThreadPoolSvc("ThreadPoolSvc")
svcMgr.ThreadPoolSvc.ThreadInitTools = ["G4InitTool"]


######################################################################################
#
## AthenaCommon flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput = [
    '/afs/cern.ch/atlas/offline/ProdData/15.6.11.3/mu_E200_eta0-60-10000.evgen.pool.root',
    #'/afs/cern.ch/atlas/offline/ProdData/16.6.X/16.6.7.Y/ttbar_muplusjets-pythia6-7000.evgen.pool.root',
]

# Dirty way to set number of events via command line
if 'evtMax' in dir(): pass
else: evtMax = -1

# Check to see if we're running hybrid mp/mt
nProc = jp.ConcurrencyFlags.NumProcs()
if (nProc > 0) :

   # For MP/Hive we need to set the chunk size
   if (evtMax == -1) :
      msg.fatal('EvtMax must be >0 for hybrid configuration')
      sys.exit(AthenaCommon.ExitCodes.CONFIGURATION_ERROR)

   if ( evtMax % nProc != 0 ) :
      msg.warning('EvtMax[%s] is not divisible by nProcs[%s]: ' +
                  'MP Workers will not process all requested events',
                  evtMax, nProc)

   chunkSize = int (evtMax / nProc)
   from AthenaMP.AthenaMPFlags import jobproperties as jps
   jps.AthenaMPFlags.ChunkSize = chunkSize
   msg.info('AthenaMP workers will process %s events each', chunkSize)

athenaCommonFlags.PoolHitsOutput = "g4hive.hits.pool.root"
athenaCommonFlags.EvtMax = evtMax

######################################################################################
#
## Job options for Geant4 ATLAS detector simulations
#

# Detector flags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
DetFlags.Lucid_setOff()
DetFlags.Truth_setOn()

# Global conditions tag
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.ConditionsTag = "OFLCOND-MC12-SIM-00"

# Simulation flags
from G4AtlasApps.SimFlags import simFlags
from G4AtlasApps import callbacks
simFlags.load_atlas_flags()

# Layout tags: see simFlags.SimLayout for allowed values
# Use the default layout:
simFlags.SimLayout.set_On()

# Set the EtaPhi, VertexSpread and VertexRange checks on/off
simFlags.EventFilter.set_Off()

# Set the LAr parameterization
#simFlags.LArParameterization = 2

# Magnetic field
simFlags.MagneticField.set_On()

# Debug outputs of user actions
#CfgGetter.getPublicTool('G4UA::AthenaTrackingActionTool').OutputLevel = DEBUG

# Setup the algorithm and output sequences
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()
from AthenaCommon.AppMgr import theApp
StreamHITS = theApp.getOutputStream( "StreamHITS" )

include("G4AtlasApps/G4Atlas.flat.configuration.py")

# SGInputLoader is a module in SGComps that will do a typeless StoreGate read
# of data on disk, to preload it in the Whiteboard for other Alorithms to use.
# It uses the same syntax as Algorithmic dependency declarations.
from AthenaCommon import CfgMgr
topSeq += CfgMgr.SGInputLoader(OutputLevel=INFO, ShowEventDump=False)
topSeq.SGInputLoader.Load = [('McEventCollection','StoreGateSvc+GEN_EVENT')]

# Add the beam effects algorithm
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("BeamEffectsAlg", tryDefaultConfigurable=True)

## Add the G4 sim to the alg sequence
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)

# Explicitly specify the data-flow dependencies of G4AtlasAlg and StreamHITS.
# This is done like this because currently our VarHandles do not live in the
# algorithm but rather in Geant4 components.
# TODO: make this declaration more automatic
topSeq.G4AtlasAlg.ExtraInputs =  [('McEventCollection','StoreGateSvc+BeamTruthEvent')]
topSeq.G4AtlasAlg.ExtraOutputs = [('SiHitCollection','StoreGateSvc+SCT_Hits')]
StreamHITS.ExtraInputs += topSeq.G4AtlasAlg.ExtraOutputs

# Increase verbosity of the output stream
#StreamHITS.OutputLevel = DEBUG

# Disable alg filtering - doesn't work yet in multi-threading
StreamHITS.AcceptAlgs = []

# Override algorithm cloning settings
algCardinality = jp.ConcurrencyFlags.NumThreads()
if (algCardinality != 1):
    for alg in topSeq:
        name = alg.name()
        alg.Cardinality = algCardinality
outSeq.StreamHITS.Cardinality = 1
