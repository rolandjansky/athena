# Skeleton file for AOD to DAOD (Reduction framework) job
#
# $Id: skeleton.AODtoDAOD_tf.py 731616 2016-03-22 15:25:39Z cranshaw $
#
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.Logging import logging
msg = logging.getLogger('AODtoDAOD')
msg.info( '****************** STARTING AOD->DAOD MAKING *****************' )

def getSubSequences(sequence,sequenceList):
    sequenceList.append(sequence)
    for item in sequence:
        if type(item).__name__ == 'AthSequencer':
            getSubSequences(item,sequenceList)
    return

if not hasattr(runArgs, "outputNTUP_PILEUPFile"):
    if hasattr(runArgs, "reductionConf"):
        msg.info('Will attempt to make the following reduced formats: {0}'.format(runArgs.reductionConf))
    else:
        msg.error('AOD Reduction job started, but with no "reductionConf" array - aborting')
        raise RuntimeError, "No reductions configured"

include("RecJobTransforms/CommonRecoSkeletonJobOptions.py")

try:
    import os
    project = os.environ ['AtlasProject']
    version = os.environ ['AtlasVersion']
    release = project + '-' + version
    rec.AtlasReleaseVersion = release
except:
    print "WARNING: Unable to construct AtlasReleaseVersion from environment"

# Check if we are running exclusively truth output
truthOutput=False
otherOutput=False
if 'reductionConf' in dir(runArgs):
    truthOutput = all(['TRUTH' in x for x in runArgs.reductionConf])
    otherOutput = any(['TRUTH' not in x for x in runArgs.reductionConf])
else:
    truthOutput = all(['TRUTH' in x for x in dir(runArgs) if 'output' in x and 'File' in x])
    otherOutput = any(['TRUTH' not in x for x in dir(runArgs) if 'output' in x and 'File' in x])
onlyTruthOutput = truthOutput and not otherOutput

if hasattr(runArgs,"inputAODFile") and not onlyTruthOutput:
    globalflags.InputFormat.set_Value_and_Lock('pool')
    rec.readAOD.set_Value_and_Lock( True )
    rec.readRDO.set_Value_and_Lock( False )
    rec.doDPD.set_Value_and_Lock(True)
    rec.OutputFileNameForRecoStep.set_Value_and_Lock("AODtoDAOD")
    athenaCommonFlags.PoolAODInput.set_Value_and_Lock( runArgs.inputAODFile )
elif hasattr(runArgs,'inputEVNTFile') or hasattr(runArgs,'jobConfig') or onlyTruthOutput:
    # Assume that we're running from EVNT or straight through evgen
    globalflags.InputFormat.set_Value_and_Lock('pool')
    rec.readAOD.set_Value_and_Lock( True )
    rec.readRDO.set_Value_and_Lock( False )
    if hasattr(runArgs,'inputEVNTFile') or hasattr(runArgs,'jobConfig'):
        rec.OutputFileNameForRecoStep.set_Value_and_Lock("EVNTtoDAOD")
        athenaCommonFlags.FilesInput = runArgs.inputEVNTFile
    else:
        rec.OutputFileNameForRecoStep.set_Value_and_Lock("AODtoDAOD")
        athenaCommonFlags.FilesInput = runArgs.inputAODFile
        athenaCommonFlags.PoolAODInput.set_Value_and_Lock( runArgs.inputAODFile )
    rec.AutoConfiguration.set_Value_and_Lock(['ProjectName','BeamType','RealOrSim','DoTruth','InputType'])
    rec.doInDet.set_Value_and_Lock(False)
    rec.doCalo.set_Value_and_Lock(False)
    rec.doMuon.set_Value_and_Lock(False)
    rec.doForwardDet.set_Value_and_Lock(False)
    rec.doFileMetaData.set_Value_and_Lock(False)
    rec.doTruth.set_Value_and_Lock( True )
    rec.doTrigger.set_Value_and_Lock( False )
    rec.doWriteTAG.set_Value_and_Lock( False )
    rec.doApplyAODFix.set_Value_and_Lock( False )
    from AthenaCommon.DetFlags import DetFlags
    DetFlags.detdescr.BField_setOff()
    globalflags.ConditionsTag = ''
    # Leave the remainder for the internal setup
else:
    msg.error('AOD Reduction job started, but with no AOD inputs - aborting')
    raise RuntimeError, "No AOD input"

if hasattr(runArgs, "outputNTUP_PILEUPFile"):
    from DerivationFrameworkDataPrep.DataPrepJobProperties import DataPrepFlags
    DataPrepFlags.outputFile = runArgs.outputNTUP_PILEUPFile
    runArgs.outputDAOD_DAPR0File = "DAOD_DAPR0.root"
    runArgs.reductionConf = "DAPR0"
listOfFlags=[]

try:
    from DerivationFrameworkCore.DerivationFrameworkProdFlags import derivationFlags
    listOfFlags.append(derivationFlags)
except ImportError:
    print "WARNING DerivationFrameworkProdFlags not available."  

from PATJobTransforms.DPDUtils import SetupOutputDPDs
rec.DPDMakerScripts.append(SetupOutputDPDs(runArgs,listOfFlags))

passThroughMode = False
if hasattr(runArgs,"passThrough"):
    passThroughMode = runArgs.passThrough

#if (passThroughMode==True):
#    msg.warning("Pass through mode is ON: decision of derivation kernels will be IGNORED!")
#    rec.doDPD.passThroughMode = True

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

# Intervene and strip SkimmingTools from algs if pass through mode requested
# recusion is necessary to ensure all kernels from all subsequences are 
# caught
if passThroughMode:
    sequenceList = []
    mainSeq = AlgSequence()
    getSubSequences(mainSeq,sequenceList)
    for seq in sequenceList:
        for item in seq:
            if type(item).__name__=='DerivationFramework__DerivationKernel':
                item.SkimmingTools = []
    msg.info( 'Pass through mode was requested. Skimming tools have been removed from all kernels.')

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

