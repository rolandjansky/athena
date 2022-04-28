# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

###############################################################
#
# HTTDumpOutputStatAlg job options file
#
#==============================================================
import os
import glob

from AthenaCommon.Logging import logging
from AthenaCommon.Constants import DEBUG

from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

from AthenaCommon.AppMgr import ToolSvc

#input
HTTInputFile = []
if 'HTTInputFile' in os.environ:
    for ex in os.environ['HTTInputFile'].split(','):
        files = glob.glob(ex)
        if files:
            HTTInputFile += files
        else:
            HTTInputFile += [ex]
else:
    HTTInputFile = ["httsim_input.root"]

msg = logging.getLogger('HTTDumpOutputStatAlg')
msg.info("Input file:  %r", HTTInputFile)

 #output
HTTOutputFile = []
if 'HTTOutputFile' in os.environ :
     for ex in os.environ['HTTOutputFile'].split(','):
        files = glob.glob(ex)
        if files:
            HTTOutputFile += files
        else:
            HTTOutputFile += [ex]
else :
     HTTOutputFile = ["httsim_output.root"]
msg.info("Output file %r",  HTTOutputFile)


# input
from TrigHTTInput.TrigHTTInputConf import HTTOutputHeaderTool
HTTReadInput = HTTOutputHeaderTool("HTTReadInput", OutputLevel = DEBUG)
HTTReadInput.InFileName=HTTInputFile
HTTReadInput.RWstatus="READ"
ToolSvc += HTTReadInput

# output
HTTWriteOutput = HTTOutputHeaderTool("HTTWriteOutput", OutputLevel = DEBUG)
HTTWriteOutput.InFileName=HTTOutputFile
HTTWriteOutput.RWstatus="RECREATE"
ToolSvc += HTTWriteOutput


from TrigHTTInput.TrigHTTInputConf import HTTDumpOutputStatAlg
theAlg = HTTDumpOutputStatAlg(OutputLevel = DEBUG)

theAlg.InputTool  = HTTReadInput
theAlg.OutputTool = HTTWriteOutput


theJob += theAlg
print (theJob)
###############################################################
