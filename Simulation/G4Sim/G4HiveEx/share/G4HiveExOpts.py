#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

# 
## G4HiveExOpts.py - an example job options for multi-threaded simulation
## in AthenaHive. This configuration can only be run if Hive is enabled.
## Run it by specifying the number of threads at the command line:
## $ athena --threads=4 ./G4HiveExOpts.py
#

from AthenaCommon.ConcurrencyFlags import jobproperties as jp
nThreads = jp.ConcurrencyFlags.NumThreads()
if (nThreads < 1) :
   from AthenaCommon.Logging import log as msg
   msg.fatal('numThreads must be >0. Did you set the --threads=N option?')
   sys.exit(AthenaCommon.ExitCodes.CONFIGURATION_ERROR)

#
## Override Setup for Hive
#
msgFmt = "% F%40W%S%5W%e%s%7W%R%T %0W%M"
svcMgr.MessageSvc.Format = msgFmt
# svcMgr.MessageSvc.useColors = True

# svcMgr.AthenaHiveEventLoopMgr.OutputLevel = DEBUG
# svcMgr.ForwardSchedulerSvc.OutputLevel = DEBUG

#
## Override defaults. otherwise these are ALL EQUAL to the number of threads
## set with the command line opt --threads=N
#

# numStores = 1
# numAlgsInFlight = 1

# svcMgr.EventDataSvc.NSlots = numStores
# svcMgr.ForwardSchedulerSvc.MaxEventsInFlight = numStores
# svcMgr.ForwardSchedulerSvc.MaxAlgosInFlight = numAlgsInFlight

# Thread pool service and initialization
from GaudiHive.GaudiHiveConf import ThreadPoolSvc
svcMgr += ThreadPoolSvc("ThreadPoolSvc")
svcMgr.ThreadPoolSvc.ThreadInitTools = ["G4InitTool"]

# Algorithm resource pool
from GaudiHive.GaudiHiveConf import AlgResourcePool
svcMgr += AlgResourcePool( OutputLevel = INFO );


#
## Uncomment following to avoid long waits when segfaulting,
## and add "Root.Stacktrace: no" to your .rootrc file
#
# import ROOT
# ROOT.SetSignalPolicy( ROOT.kSignalFast )

## Output threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#ServiceMgr.MessageSvc.OutputLevel = INFO



######################################################################################
#
## AthenaCommon flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput = [
    '/afs/cern.ch/user/z/zmarshal/public/ForSteve/mc15_13TeV.424000.ParticleGun_single_mu_Pt100.evgen.EVNT.e3580/EVNT.04922446._000001.pool.root.1',
    #'/afs/cern.ch/atlas/offline/ProdData/16.6.X/16.6.7.Y/ttbar_muplusjets-pythia6-7000.evgen.pool.root',
]

# Dirty way to set number of events via command line
if 'evtMax' in dir(): pass
else: evtMax = -1

# check to see if we're running hybrid mp/mt
nProc = jp.ConcurrencyFlags.NumProcs()
if (nProc > 0) :

   #
   ## For MP/Hive we need to set the chunk size
   #

   from AthenaCommon.Logging import log as msg
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

athenaCommonFlags.PoolHitsOutput = "atlasG4.hits.pool.root"
athenaCommonFlags.EvtMax = evtMax

######################################################################################
#
## Job options for Geant4 ATLAS detector simulations
#

## Detector flags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
DetFlags.Lucid_setOff()
DetFlags.Truth_setOn()

## Global conditions tag
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.ConditionsTag = "OFLCOND-MC12-SIM-00"

## Simulation flags
from G4AtlasApps.SimFlags import simFlags
from G4AtlasApps import callbacks
simFlags.load_atlas_flags()
#simFlags.RandomSvc = 'AtDSFMTGenSvc'

## Layout tags: see simFlags.SimLayout for allowed values
## Use the default layout:
simFlags.SimLayout.set_On()

## Set the EtaPhi, VertexSpread and VertexRange checks on
simFlags.EventFilter.set_Off()

## Set the LAr parameterization
#simFlags.LArParameterization = 2

## No magnetic field
simFlags.MagneticField.set_On()

## Change the field stepper or use verbose G4 tracking
#from G4AtlasApps import callbacks
#simFlags.InitFunctions.add_function("postInit", callbacks.use_simplerunge_stepper)
#simFlags.InitFunctions.add_function("preInitG4", callbacks.use_verbose_tracking)

# Debug output
#CfgGetter.getPublicTool('G4UA::AthenaTrackingActionTool').OutputLevel = DEBUG

# Setup the algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

# Currently, Hive requires an algorithm to load the initial data into the
# whiteboard and kickstart the data dependency chain. This alg must be at the
# front of the AlgSequence.
from AthenaCommon import CfgMgr
topSeq += CfgMgr.SGInputLoader(OutputLevel = INFO, ShowEventDump=False)

# Dependencies are either specified automatically via VarHandles (the preferred
# way), or via the ExtraInputs/ExtraOutputs properties. Data objects in the
# latter method are specified as vector of tuples as [(ClassID,'key'),...] or
# [('ClassName','key'),...] 

# SGInputLoader is a module in SGComps that will do a typeless StoreGate read
# of data on disk, to preload it in the Whiteboard for other Alorithms to use.
# Is uses the same syntax as Algorithmic dependency declarations
topSeq.SGInputLoader.Load = [('McEventCollection','GEN_EVENT')]

# Add the beam effects algorithm
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("BeamEffectsAlg", tryDefaultConfigurable=True)

## Add the G4 simulation service
from G4AtlasApps.PyG4Atlas import PyG4AtlasSvc
svcMgr += PyG4AtlasSvc()

# TODO: make this declaration more automatic
topSeq.G4AtlasAlg.ExtraInputs =  [('McEventCollection','BeamTruthEvent')]
topSeq.G4AtlasAlg.ExtraOutputs = [('SiHitCollection','SCT_Hits')]

topSeq.StreamHITS.ExtraInputs += topSeq.G4AtlasAlg.ExtraOutputs

# Disable all of the LAr SDs because they are not yet thread-safe
sdMaster = ToolSvc.SensitiveDetectorMasterTool
larSDs = [sd for sd in sdMaster.SensitiveDetectors if sd.name().startswith('LAr')]
for sd in larSDs: sdMaster.SensitiveDetectors.remove(sd)

# Increase verbosity of the output stream
#topSeq.StreamHITS.OutputLevel = DEBUG

# Disable alg filtering - doesn't work in multi-threading
topSeq.StreamHITS.AcceptAlgs = []

# theAuditorSvc.Auditors = ["ChronoAuditor", "NameAuditor", "AlgContextAuditor"]

#
## set which Algorithms can be cloned
#

#  set algCardinality = 1 to disable cloning for all Algs
algCardinality = jp.ConcurrencyFlags.NumThreads()

if (algCardinality != 1):
    for alg in topSeq:
        name = alg.name()
        if name in ["StreamHITS"]:
            print 'Disabling cloning/cardinality for', name
            # Don't clone these algs
            alg.Cardinality = 1
            alg.IsClonable = False
        else:
            alg.Cardinality = algCardinality
            alg.IsClonable = True
