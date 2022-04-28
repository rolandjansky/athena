# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

###############################################################
#
# TrigHTTWrapper job options file
#
#==============================================================


from AthenaCommon.AppMgr import ToolSvc
import TrigHTTConfTools.HTTTagConfig as HTTTagConfig
import TrigHTTMaps.HTTMapConfig as HTTMapConfig

tags = HTTTagConfig.getTags(stage='map')
map_tag = tags['map']
MapSvc = HTTMapConfig.addMapSvc(map_tag)

from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()


import glob
import os
from AthenaCommon.Logging import logging
from AthenaCommon.Constants import DEBUG

#input
InputHTTLogHitFile = []
if 'InputHTTLogHitFile' in os.environ:
    for ex in os.environ['InputHTTLogHitFile'].split(','):
        files = glob.glob(ex)
        if files:
            InputHTTLogHitFile += files
        else:
            InputHTTLogHitFile += [ex]
else :
    InputHTTLogHitFile = ["httsim_loghits_wrap.OUT.root"]

msg = logging.getLogger('HTTReadLogicalHitsAlg')
msg.info("Input file %r",  InputHTTLogHitFile)


from TrigHTTInput.TrigHTTInputConf import HTTOutputHeaderTool
HTTReadInput = HTTOutputHeaderTool("HTTReadInput", OutputLevel = DEBUG)
HTTReadInput.InFileName=InputHTTLogHitFile
HTTReadInput.RWstatus="READ"
ToolSvc += HTTReadInput

from TrigHTTInput.TrigHTTInputConf import HTTReadLogicalHitsAlg
wrapper = HTTReadLogicalHitsAlg(OutputLevel = DEBUG)
wrapper.InputTool  = HTTReadInput

theJob += wrapper

###############################################################
