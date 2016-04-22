# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from MuonCalibStandAloneTools.MuonCalibStandAloneToolsConfig import *
from MuonCalibStandAloneExtraTools.MuonCalibStandAloneExtraToolsConf import *

#list tools that do not need any recalibration
NoRecalibration += ["Splitter"]

#list tools that needs the t0 reverted
#RevertT0 += []

#list tools, that needs the t0 recalibrated
#RecalibT0 += []

#list tools that need all recalibrated
#RecalibAll += []

#algorithms for which no segment refit should be done, even if requested by user
NoSegmentRefit += ["Splitter"]

#algorithms fotr which the number of read segments should be limited
#LimitSegmentsFor += []

#algorithms, that need validated t0 and rt
ValidatedT0 += [ "MdtDqa", "Splitter" ]

ValidatedRt += [ "MdtDqa", "Splitter" ]


def CreateCalibExtraTool(CalibrationTool):
	dir()
	if CalibrationTool == "MdtDqa":
		return MuonCalib__NtupleMdtDqaTool()
	if CalibrationTool == "Splitter":
		return MuonCalib__NtupleSplitterTool()
	else:
		print "WARNING: Unknown tool name '" + CalibrationTool + "'!"
		return None
	
