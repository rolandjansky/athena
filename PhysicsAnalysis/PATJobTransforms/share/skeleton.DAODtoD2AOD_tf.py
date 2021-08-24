# Skeleton file for DAOD_PHYS to D2AOD (derivation framework) job
#
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.Logging import logging
msg = logging.getLogger('DAODtoD2AOD')
msg.info( '****************** STARTING DAOD_PHYS->D2AOD MAKING *****************' )

def getSubSequences(sequence,sequenceList):
    sequenceList.append(sequence)
    for item in sequence:
        if type(item).__name__ == 'AthSequencer':
            getSubSequences(item,sequenceList)
    return

if hasattr(runArgs, "reductionConf"):
    msg.info('Will attempt to make the following reduced formats: {0}'.format(runArgs.reductionConf))
else:
    msg.error('DAOD_PHYS skimming job started, but with no "reductionConf" array - aborting')
    raise RuntimeError("No reductions configured")

include("RecJobTransforms/CommonRecoSkeletonJobOptions.py")

try:
    import os
    project = os.environ ['AtlasProject']
    version = os.environ ['AtlasVersion']
    release = project + '-' + version
    rec.AtlasReleaseVersion = release
except:
    print("WARNING: Unable to construct AtlasReleaseVersion from environment")

if hasattr(runArgs,"inputDAOD_PHYSFile"):
    globalflags.InputFormat.set_Value_and_Lock('pool')
    rec.readAOD.set_Value_and_Lock( True )
    rec.readRDO.set_Value_and_Lock( False )
    rec.doTrigger.set_Value_and_Lock( False )
    rec.doTruth.set_Value_and_Lock( False ) 
    rec.doDPD.set_Value_and_Lock(True)
    rec.OutputFileNameForRecoStep.set_Value_and_Lock("DAODtoD2AOD")
    athenaCommonFlags.PoolAODInput.set_Value_and_Lock( runArgs.inputDAOD_PHYSFile )
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Input.Files = athenaCommonFlags.PoolAODInput()
else:
    msg.error('DAOD_PHYS skimming job started, but with no DAOD_PHYS inputs - aborting')
    raise RuntimeError("No DAOD_PHYS input")

listOfFlags=[]

try:
    from DerivationFrameworkCore.DerivationFrameworkProdFlags import derivationFlags
    listOfFlags.append(derivationFlags)
except ImportError:
    print("WARNING DerivationFrameworkProdFlags not available.") 

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

