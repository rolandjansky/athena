# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

###############################################################
#
#   HTTOverlapRemovalValAlg job options file
#   @file TrigHTTAlgorithms/share/HTTOverlapRemovalValidation_jobOptions.py
#   @author Zhaoyuan.Cui@cern.ch
#   @date Dec. 4, 2020
#   @brief jobOptions for overlap removal validation algorithm
#==============================================================

import os
import glob

from TrigHTTConfig.formatMessageSvc import formatMessageSvc
formatMessageSvc()

import TrigHTTConfig.HTTTagConfig as HTTTagConfig
import TrigHTTMaps.HTTMapConfig as HTTMapConfig
import TrigHTTBanks.HTTBankConfig as HTTBankConfig
import TrigHTTConfig.HTTConfigCompInit as HTTConfig

tags = HTTTagConfig.getTags(stage='bank')
map_tag = tags['map']
bank_tag = tags['bank']
MapSvc = HTTMapConfig.addMapSvc(map_tag)
BankSvc = HTTBankConfig.addBankSvc(map_tag, bank_tag)

from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

from AthenaCommon.AppMgr import ToolSvc, ServiceMgr
from AthenaCommon.Constants import DEBUG

from GaudiSvc.GaudiSvcConf import THistSvc
# THistSvc configuration
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["MONITOROUT DATAFILE='OR_monitor_out_${HTTORSampleType}_${HTTORAlgo}_grouping-${HTTORnhg}.root' OPT='RECREATE'"]

# InputFile
HTTInputFile=[]
if 'HTTInputFile' in os.environ :
     for ex in os.environ['HTTInputFile'].split(','):
         files=glob.glob(ex)
         HTTInputFile += files
else :
     HTTInputFile = ["httsim_loghits_wra.OUT.root"]
print("Input file:  ",  HTTInputFile)

ES = HTTConfig.addEvtSelSvc(map_tag)
ES.sampleType=os.environ["HTTORSampleType"]

# Read in file
from TrigHTTInput.TrigHTTInputConf import HTTOutputHeaderTool
HTTReadInput = HTTOutputHeaderTool("HTTReadInput", OutputLevel = DEBUG)
HTTReadInput.InFileName=HTTInputFile
HTTReadInput.RWstatus="READ"
ToolSvc += HTTReadInput

# OR tools
from TrigHTTAlgorithms.TrigHTTAlgorithmsConf import HTTOverlapRemovalTool
ORtool = HTTOverlapRemovalTool(OutputLevel = DEBUG)
ORtool.ORAlgo = os.environ["HTTORAlgo"]
ORtool.NumOfHitPerGrouping = int(os.environ["HTTORnhg"])
ToolSvc += ORtool

from TrigHTTMonitor.TrigHTTMonitorConf import HTTOverlapRemovalMonitorTool
ORmonitor = HTTOverlapRemovalMonitorTool(OutputLevel = DEBUG)
ORmonitor.BarcodeFracCut = 0.5
ToolSvc += ORmonitor

from TrigHTTAlgorithms.TrigHTTAlgorithmsConf import HTTOverlapRemovalValAlg
theAlg = HTTOverlapRemovalValAlg(OutputLevel = DEBUG);
theAlg.InputTool = HTTReadInput
theAlg.OverlapRemoval = ORtool
theAlg.OverlapRemovalMonitor = ORmonitor

theJob +=theAlg
############################################################
