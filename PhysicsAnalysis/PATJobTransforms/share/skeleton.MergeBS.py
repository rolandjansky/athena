###############################################################
# Simple job options for bytestream merging
# David Cote (DESY), November 2008
#==============================================================

from AthenaCommon.Logging import logging
recoLog = logging.getLogger('bs_to_bs')
recoLog.info( '****************** STARTING BYTESTREAM MERGING *****************' )

include ("RecExCommon/RecoUsefulFlags.py")

include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
svcMgr = theApp.serviceMgr()
ByteStreamInputSvc = svcMgr.ByteStreamInputSvc

## max events
theApp.EvtMax=-1
if hasattr(runArgs,"maxEvents"): theApp.EvtMax = runArgs.maxEvents

## Input
if hasattr(runArgs,"inputBSFile"):
    ByteStreamInputSvc.FullFileName += runArgs.inputBSFile

## Pre-include
if hasattr(runArgs,"preInclude"): 
    for fragment in runArgs.preInclude:
        include(fragment)

## Pre-exec
if hasattr(runArgs,"preExec"):
    recoLog.info("transform pre-exec")
    for cmd in runArgs.preExec:
        recoLog.info(cmd)
        exec(cmd)

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
bs=MSMgr.NewByteStream("outputBS")

#set max output file size to 15 GB
bs.bsOutputSvc.MaxFileMB = 15000
bs.bsOutputSvc.MaxFileNE = 15000000

if hasattr(runArgs,"outputBSFile"):
    from PATJobTransforms.DPDUtils import outputBSNameStripper
    simpleName = outputBSNameStripper(runArgs.outputBSFile)
    bs.SetOutputFileName(simpleName)

## Post-include
if hasattr(runArgs,"postInclude"): 
    for fragment in runArgs.postInclude:
        include(fragment)

## Post-exec
if hasattr(runArgs,"postExec"):
    recoLog.info("transform post-exec")
    for cmd in runArgs.postExec:
        recoLog.info(cmd)
        exec(cmd)

