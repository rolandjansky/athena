
## Detector flags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOn()
DetFlags.Lucid_setOff()
DetFlags.Truth_setOn()



from AthenaCommon.ConcurrencyFlags import jobproperties as jp
nThreads = jp.ConcurrencyFlags.NumThreads()
if (nThreads < 1) :
   from AthenaCommon.Logging import log as msg
   msg.fatal('numThreads must be >0. Did you set the --threads=N option?')
   sys.exit(AthenaCommon.ExitCodes.CONFIGURATION_ERROR)

# Thread pool service and initialization
from GaudiHive.GaudiHiveConf import ThreadPoolSvc
svcMgr += ThreadPoolSvc("ThreadPoolSvc")
svcMgr.ThreadPoolSvc.ThreadInitTools = ["G4InitTool"]

# Algorithm resource pool
from GaudiHive.GaudiHiveConf import AlgResourcePool
svcMgr += AlgResourcePool( OutputLevel = INFO );

from AthenaCommon.AlgSequence import AlgSequence
from PerfMonComps.PerfMonFlags import jobproperties	
jobproperties.PerfMonFlags.doMonitoring.set_Value_and_Lock(False)
jobproperties.PerfMonFlags.doDsoMonitoring.set_Value_and_Lock(False)
#jobproperties.doPerfMon.set_Value_and_Lock(False)
from RecExConfig.RecFlags import rec
rec.doPerfMon.set_Value_and_Lock( False )
rec.doDetailedPerfMon.set_Value_and_Lock( False )
rec.doSemiDetailedPerfMon.set_Value_and_Lock( False )

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

## Simulation flags
from G4AtlasApps.SimFlags import simFlags
from G4AtlasApps import callbacks
simFlags.load_atlas_flags()

## No magnetic field
simFlags.MagneticField.set_Off()

# Currently, Hive requires an algorithm to load the initial data into the
# whiteboard and kickstart the data dependency chain. This alg must be at the
# front of the AlgSequence.
topSeq = AlgSequence()
from AthenaCommon import CfgMgr
topSeq += CfgMgr.SGInputLoader(OutputLevel = INFO, ShowEventDump=False)

# SGInputLoader is a module in SGComps that will do a typeless StoreGate read
# of data on disk, to preload it in the Whiteboard for other Alorithms to use.
# Is uses the same syntax as Algorithmic dependency declarations
topSeq.SGInputLoader.Load = [('McEventCollection','GEN_EVENT')]

## Add the G4 simulation service
#from G4AtlasApps.PyG4Atlas import PyG4AtlasSvc
#svcMgr += PyG4AtlasSvc()

