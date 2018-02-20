#this is just a simple wrapper for the joboptions
#to connect up the runArgs input and outputs

jps.AthenaCommonFlags.FilesInput = runArgs.inputAODFile
outputPRWFile = runArgs.outputNTUP_PILEUPFile

## Pre-exec
if hasattr(runArgs,"preExec"):
    recoLog.info("transform pre-exec")
    for cmd in runArgs.preExec:
        recoLog.info(cmd)
        exec(cmd)

#preInclude
if hasattr(runArgs,"preInclude"):
    recoLog.info("transform pre-include")
    include(runArgs.preInclude)

include("PileupReweighting/generatePRW_jobOptions.py")

#postInclude                                                                                                        
if hasattr(runArgs,"postInclude"):
    recoLog.info("transform pre-include")
    include(runArgs.postInclude)


## Post-exec                                                                                                          
if hasattr(runArgs,"postExec"):
    recoLog.info("transform post-exec")
    for cmd in runArgs.postExec:
        recoLog.info(cmd)
        exec(cmd)
