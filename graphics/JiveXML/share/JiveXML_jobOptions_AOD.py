#
# jOs for RTT testing
# jpt 8Jun09
# moved back from RecExCommon/readAOD to AnalysisSkeleton
# Note: static AOD input file updated by hand. Should if possible
# be replaced by AOD input file produced in the same RTT 
#
### applying InputCollection is somewhat moody (over-written)
### so apply symbolic link instead:
import os
os.system('ln -sf /afs/cern.ch/atlas/project/Atlantis/rttInputs/AOD_23Mar12.pool.root AOD.pool.root')

#### doesn't work with devval AODs  9Nov09 jpt
#include("UserAnalysis/AnalysisSkeleton_topOptions.py")
#### replaced with minimal jOs reading AOD:
# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ToolSvc

from AthenaCommon.AppMgr import ServiceMgr
# Event selector
import AthenaPoolCnvSvc.ReadAthenaPool
#EventSelector.BackNavigation = True

# Particle Properties
from PartPropSvc.PartPropSvcConf import PartPropSvc

# the POOL converters
include( "ParticleBuilderOptions/ESD_PoolCnv_jobOptions.py" )
include( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py")
include( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

ServiceMgr.EventSelector.InputCollections = [ "AOD.pool.root" ]
####

## Test writting Atlantis xml output from AODs
## Described in TWiki page JiveXMLWithAODorESD
##
include( "JiveXML/JiveXML_jobOptionBase.py" )
include ("AnalysisJiveXML/AnalysisJiveXML_DataTypes.py")

