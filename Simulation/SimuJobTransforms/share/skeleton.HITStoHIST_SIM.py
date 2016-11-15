include("SimuJobTransforms/CommonSkeletonJobOptions.py")
include( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )


# get the logger
from AthenaCommon.Logging import logging
valilog = logging.getLogger('SimValid_tf')
valilog.info( '****************** STARTING VALIDATION *****************' )
valilog.info( str(runArgs) )

#==============================================================
# Job definition parameters:
#==============================================================
#already in CommonSkeletonJobOptions.py
#from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool

from PartPropSvc.PartPropSvcConf import PartPropSvc

import os
from glob import glob
if hasattr(runArgs,"inputHITSFile"):
    globalflags.InputFormat.set_Value_and_Lock('pool')
    athenaCommonFlags.PoolHitsInput.set_Value_and_Lock( runArgs.inputHITSFile )
    athenaCommonFlags.FilesInput = runArgs.inputHITSFile
    ServiceMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()


## Output HIST File
if hasattr(runArgs,"outputHIST_SIMFile"):
    print "Output is"
    print  runArgs.outputHIST_SIMFile
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()
    ServiceMgr.THistSvc.Output +=["HitAnalysis DATAFILE='"+runArgs.outputHIST_SIMFile+"' OPT='RECREATE'"]

else:
    valilog.warning('No output file set')
    ServiceMgr.THistSvc.Output +=["CaloHitAnalysis DATAFILE = 'output.HIST_SIM.root' OPT='RECREATE'"]


#==============================================================
# Job Configuration parameters:
#==============================================================
## Pre-exec
if hasattr(runArgs,"preExec"):
    valilog.info("transform pre-exec")
    for cmd in runArgs.preExec:
        valilog.info(cmd)
        exec(cmd)

## Pre-include
if hasattr(runArgs,"preInclude"):
    for fragment in runArgs.preInclude:
        include(fragment)


include("HitAnalysis/HitAnalysis_topOptions.py")

## Post-include
if hasattr(runArgs,"postInclude"):
    for fragment in runArgs.postInclude:
        include(fragment)

## Post-exec
if hasattr(runArgs,"postExec"):
    digilog.info("transform post-exec")
    for cmd in runArgs.postExec:
        digilog.info(cmd)
        exec(cmd)
