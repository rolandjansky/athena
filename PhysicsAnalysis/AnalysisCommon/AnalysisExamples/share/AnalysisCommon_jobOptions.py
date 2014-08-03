# import the data types 
import EventKernel.ParticleDataType

# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ServiceMgr

# load additional libraries for back navigation
from AnalysisExamples.AnalysisFlags import AnalysisFlags

from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

import AthenaPoolCnvSvc.ReadAthenaPool

if AnalysisFlags.DoNavigation:
   include( "RecExCommon/AllDet_detDescr.py" ) 
   ServiceMgr.EventSelector.BackNavigation = True

# the Dlls
# Don't need them
#
#include ( "ParticleBuilderOptions/ESD_PoolCnv_jobOptions.py" )
#include ( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py" )
#include ( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py" )
#include ( "EventAthenaPool/EventAthenaPool_joboptions.py" )

# write out a summary of the time spent
#theAuditorSvc = AuditorSvc()
#theAuditorSvc.Auditors  += [ "NameAuditor"]
#theAuditorSvc.Auditors  += [ "ChronoAuditor", "MemStatAuditor"]

# Athena-Aware NTuple making Tools

include ("CBNT_Athena/CBNT_AthenaAware_jobOptions.py")
include ("CBNT_Athena/CBNT_EventInfo_jobOptions.py")
