from __future__ import print_function
# Skeleton file for AOD to DAOD (Reduction framework) job
#
# $Id$
#
from AthenaCommon.Logging import logging
msg = logging.getLogger('AodReduction')
msg.info( '****************** STARTING AOD->REDUCTION_FORMAT MAKING *****************' )

if hasattr(runArgs, "reductionConf"):
    msg.info('Will attempt to make the following reduced formats: {0}'.format(runArgs.reductionConf))
else:
    msg.error('AOD Reduction job started, but with no "reductionConf" array - aborting')
    raise RuntimeError("No reductions configured")

include("RecJobTransforms/CommonRecoSkeletonJobOptions.py")

## Automatically turn ON/OFF and set output file name of each possible DPD
listOfFlags=[]
try:
    from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
    listOfFlags.append(primDPD)
except ImportError:
    print("WARNING PrimaryDPDFlags not available. Only OK if you're using job transforms without the AtlasAnalysis project.")
try:
    from D3PDMakerConfig.D3PDProdFlags import prodFlags
    listOfFlags.append( prodFlags )
except ImportError:
    print("WARNING D3PDProdFlags not available. Only OK if you're using job transforms without the AtlasAnalysis project.")

from PATJobTransforms.DPDUtils import SetupOutputDPDs
rec.DPDMakerScripts.append(SetupOutputDPDs(runArgs,listOfFlags))

if hasattr(runArgs,"inputAODFile"):
    globalflags.InputFormat.set_Value_and_Lock('pool')
    rec.readAOD.set_Value_and_Lock( True )
    rec.readRDO.set_Value_and_Lock( False )
    athenaCommonFlags.PoolAODInput.set_Value_and_Lock( runArgs.inputAODFile )
else:
    msg.error('AOD Reduction job started, but with no AOD inputs - aborting')
    raise RuntimeError("No AOD input")


## Pre-exec
if hasattr(runArgs,"preExec"):
    msg.info("transform pre-exec")
    for cmd in runArgs.preExec:
        msg.info(cmd)
        exec(cmd)

## Pre-include
if hasattr(runArgs,"preInclude"): 
    for fragment in runArgs.preInclude:
        include(fragment)

#========================================================
# Central topOptions (this is one is a string not a list)
#========================================================
if hasattr(runArgs,"topOptions"): include(runArgs.topOptions)
else: include( "RecExCommon/RecExCommon_topOptions.py" )


## Post-include
if hasattr(runArgs,"postInclude"): 
    for fragment in runArgs.postInclude:
        include(fragment)


## Post-exec
if hasattr(runArgs,"postExec"):
    msg.info("transform post-exec")
    for cmd in runArgs.postExec:
        msg.info(cmd)
        exec(cmd)


