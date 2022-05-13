# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#-----------------------------------------------------------------------------
# Athena imports
#-----------------------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

from AthenaCommon.AppMgr import ToolSvc

import AthenaCommon.Configurable as Configurable
from AthenaCommon.Constants import INFO
Configurable.log.setLevel(INFO)

#--------------------------------------------------------------
# HTT Includes
#--------------------------------------------------------------


from TrigHTTInput.TrigHTTInputConf import HTTDetectorTool, HTTDumpDetStatusAlgo

import TrigHTTConfTools.HTTTagConfig as HTTTagConfig
import TrigHTTMaps.HTTMapConfig as HTTMapConfig

tags = HTTTagConfig.getTags(stage='map')
map_tag = tags['map']
MapSvc = HTTMapConfig.addMapSvc(map_tag)

HTTDet = HTTDetectorTool()
ToolSvc += HTTDet

HTTDumpCond = HTTDumpDetStatusAlgo("HTTDumpDetStatusAlgo")
HTTDumpCond.DumpGlobalToLocalMap = True
theJob += HTTDumpCond
