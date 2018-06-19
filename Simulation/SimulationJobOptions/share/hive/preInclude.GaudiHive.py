#
# To configure multi-threaded simulation (AtlasG4) we set up some
# AthenaMT-related infrastructure and disable some features that do not
# work yet in MT.
#

# Detector flags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
DetFlags.Lucid_setOff()
DetFlags.Truth_setOn()

# Check that we correctly configured number of threads from command line
from AthenaCommon.ConcurrencyFlags import jobproperties as jp
nThreads = jp.ConcurrencyFlags.NumThreads()
if (nThreads < 1) :
   from AthenaCommon.Logging import log as msg
   msg.fatal('numThreads must be >0. Did you set the --threads=N option?')
   sys.exit(AthenaCommon.ExitCodes.CONFIGURATION_ERROR)

# Update message stream format to include slot number
msgFmt = "% F%40W%S%5W%e%s%7W%R%T %0W%M"
svcMgr.MessageSvc.Format = msgFmt

# Thread pool service and initialization
from GaudiHive.GaudiHiveConf import ThreadPoolSvc
svcMgr += ThreadPoolSvc("ThreadPoolSvc")
svcMgr.ThreadPoolSvc.ThreadInitTools = ["G4InitTool"]

# Performance monitoring probably not yet thread-safe
from PerfMonComps.PerfMonFlags import jobproperties	
jobproperties.PerfMonFlags.doMonitoring.set_Value_and_Lock(False)
jobproperties.PerfMonFlags.doDsoMonitoring.set_Value_and_Lock(False)
#jobproperties.doPerfMon.set_Value_and_Lock(False)
from RecExConfig.RecFlags import rec
rec.doPerfMon.set_Value_and_Lock( False )
rec.doDetailedPerfMon.set_Value_and_Lock( False )
rec.doSemiDetailedPerfMon.set_Value_and_Lock( False )

## Simulation flags
from G4AtlasApps.SimFlags import simFlags
from G4AtlasApps import callbacks
simFlags.load_atlas_flags()
# Disable the EtaPhi, VertexSpread and VertexRange checks
simFlags.EventFilter.set_Off()

# Setup the algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

# Instruct the input loader to populate the whiteboard with the GEN_EVENT
# data from the input file.
from AthenaCommon import CfgMgr
CfgMgr.SGInputLoader().Load += [('McEventCollection', 'StoreGateSvc+GEN_EVENT')]
