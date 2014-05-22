OutputLevel=INFO
doCBNT=False
EvtMax = 5

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
assert( job == AlgSequence() )     # this is a singleton

from HeapMon.EventNotifier import EventExecutionNotifier
een =EventExecutionNotifier('EventNotifier')
een.start_scan = 1 
een.stop_scan  = 4

job.insert(0,een) 

#DetDescrVersion="ATLAS-GEO-02-01-00"
#PoolRDOInput = [ "RDO.pool.root" ]

# DetFlags modifications are best set here (uncomment RecExCommon_flags first)
#include ("RecExCond/RecExCommon_flags.py")
# switch off ID, calo, or muons
#DetFlags.ID_setOff()
#DetFlags.Calo_setOff()
#DetFlags.Muon_setOff()
print job

include ("jobOptions.py")
#######################################################
