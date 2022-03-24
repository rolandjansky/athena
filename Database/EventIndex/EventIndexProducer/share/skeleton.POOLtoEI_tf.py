# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

import logging
eiLog = logging.getLogger('pool_to_ei')
eiLog.info('****************** STARTING POOL->EI MAKING *****************')


# Input

inputFileGiven = 0
for filetype in ('POOL', 'AOD', 'ESD', 'EVNT', 'HITS', 'RDO', 'AOD_MRG'):
    if hasattr(runArgs, "input" + filetype + "File"):
        In = getattr(runArgs, "input" + filetype + "File")
        if type(In) is not list:
            In = [In]
        inputFileGiven += 1

if inputFileGiven == 0:
    raise RuntimeError("No input file given")
if inputFileGiven > 1:
    raise RuntimeError("Only one input file format is allowed")


# Output

if hasattr(runArgs, "outputEIFile"):
    Out = runArgs.outputEIFile
else:
    Out = None

# options

if hasattr(runArgs, "maxEvents"):
    EvtMax = runArgs.maxEvents

if hasattr(runArgs, "trigger"):
    DoTriggerInfo = runArgs.trigger
else:
    DoTriggerInfo = True

if hasattr(runArgs, "provenance"):
    DoProvenanceRef = runArgs.provenance
else:
    DoProvenanceRef = True

if hasattr(runArgs, "sendtobroker"):
    SendToBroker = runArgs.sendtobroker
else:
    SendToBroker = False

if hasattr(runArgs, "eidsname"):
    EiDsName = runArgs.eidsname
else:
    EiDsName = None

if hasattr(runArgs, "testbrk"):
    TestBrk = runArgs.testbrk
else:
    TestBrk = False

if hasattr(runArgs, "eifmt"):
    EiFmt = runArgs.eifmt
else:
    EiFmt = 0

# Tier0 job identification
if hasattr(runArgs, "_taskid"):
    TaskID = runArgs._taskid
else:
    TaskID = None

if hasattr(runArgs, "_jobid"):
    JobID = runArgs._jobid
else:
    JobID = None

if hasattr(runArgs, "_attempt"):
    AttemptNumber = runArgs._attempt
else:
    AttemptNumber = None

# Pre-exec
if hasattr(runArgs, "preExec"):
    eiLog.info("transform pre-exec")
    for cmd in runArgs.preExec:
        eiLog.info(cmd)
        exec(cmd)

# Pre-include
if hasattr(runArgs, "preInclude"):
    for fragment in runArgs.preInclude:
        include(fragment)

# load joboptions
include('EventIndexProducer/POOL2EI_joboptions.py')


# Post-include
if hasattr(runArgs, "postInclude"):
    for fragment in runArgs.postInclude:
        include(fragment)

# Post-exec
if hasattr(runArgs, "postExec"):
    eiLog.info("transform post-exec")
    for cmd in runArgs.postExec:
        eiLog.info(cmd)
        exec(cmd)
