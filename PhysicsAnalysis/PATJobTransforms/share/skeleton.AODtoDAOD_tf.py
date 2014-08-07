# Skeleton file for AOD to DAOD (Reduction framework) job
#
# $Id: skeleton.AODtoDAOD_tf.py 597900 2014-05-20 08:16:34Z graemes $
#
from AthenaCommon.Logging import logging
msg = logging.getLogger('ADOtoDAOD')
msg.info( '****************** STARTING AOD->DAOD MAKING *****************' )

if hasattr(runArgs, "reductionConf"):
    msg.info('Will attempt to make the following reduced formats: {0}'.format(runArgs.reductionConf))
else:
    msg.error('AOD Reduction job started, but with no "reductionConf" array - aborting')
    raise RuntimeError, "No reductions configured"

include("RecJobTransforms/CommonRecoSkeletonJobOptions.py")


if hasattr(runArgs,"inputAODFile"):
    globalflags.InputFormat.set_Value_and_Lock('pool')
    rec.readAOD.set_Value_and_Lock( True )
    rec.readRDO.set_Value_and_Lock( False )
    rec.doDPD.set_Value_and_Lock(True)
    athenaCommonFlags.PoolAODInput.set_Value_and_Lock( runArgs.inputAODFile )
else:
    msg.error('AOD Reduction job started, but with no AOD inputs - aborting')
    raise RuntimeError, "No AOD input"

listOfFlags=[]

try:
    from DerivationFrameworkCore.DerivationFrameworkProdFlags import derivationFlags
    listOfFlags.append(derivationFlags)
except ImportError:
    print "WARNING DerivationFrameworkProdFlags not available."  

from PATJobTransforms.DPDUtils import SetupOutputDPDs
rec.DPDMakerScripts.append(SetupOutputDPDs(runArgs,listOfFlags))


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


