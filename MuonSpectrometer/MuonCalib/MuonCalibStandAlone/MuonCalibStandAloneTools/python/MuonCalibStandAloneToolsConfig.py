# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from MuonCalibStandAloneTools.MuonCalibStandAloneToolsConf import *

#list tools that do not need any reclaibration
NoRecalibration = [ "RunScan", "DbCheck", "MTT0", "ResidualVsTimeTool" ]
#list tools that needs the t0 revertetd
RevertT0 = [ "ClassicT0" ]
#list tools, that needs the t0 recalibrated
RecalibT0 = [ "Integration" ]
#list tools that need all recalibrated
RecalibAll = [ "CalibADC", "ClassicAutocal", "AnalyticAutocal", "CurvedAutocal", "SimpleResolution",  "Chi2Resolution", "Efficiency", "Display", "RPCTiming", "ControlHistograms", "MDTChamberGeometryDetermination", "MdtDqa"]
#list tools for which rt-scaling is to be reverted
RevertRtScaling = [ "Integration", "MTT0" ]
#list tools for which rt-scaling is to be reapplied
ApplRtScaling = RecalibAll[:]


#algorithms for which no segment refit should be done, even if requested by user
NoSegmentRefit = [ "RunScan", "DbCheck", "MTT0", "ClassicT0", "Integration"]

#algorithms fotr which the number of read segments should be limited
LimitSegmentsFor = ["ClassicAutocal", "AnalyticAutocal", "CurvedAutocal", "SimpleResolution"]

#algorithms, that need validated t0 and rt
ValidatedT0 = [ "CalibADC", "Integration",  "ClassicAutocal", "AnalyticAutocal", "CurvedAutocal", "SimpleResolution",  "Chi2Resolution", "Efficiency", "Display", "RPCTiming", "ControlHistograms", "MDTChamberGeometryDetermination" ,  "DbCheck"]

ValidatedRt = [ "CalibADC", "ControlHistograms",  "DbCheck", "RPCTiming", "Display"]

def CreateCalibTool(CalibrationTool):
	if CalibrationTool=="MTT0":
		return MuonCalib__NtupleMTT0Tool()
	if CalibrationTool=="CalibADC":
		return MuonCalib__NtupleCalibADCTool()
	elif CalibrationTool=="ClassicT0":
		return MuonCalib__NtupleClassicT0Tool()
	elif CalibrationTool=="Integration":
		return MuonCalib__NtupleIntegrationTool()
	elif CalibrationTool=="AnalyticAutocal":
		return MuonCalib__NtupleAnalyticAutocalibrationTool()
	elif CalibrationTool=="ClassicAutocal":
		return MuonCalib__NtupleClassicAutocalibrationTool()
	elif CalibrationTool=="CurvedAutocal":
		return MuonCalib__NtupleCurvedAutocalibrationTool()
	elif CalibrationTool=="SimpleResolution":
		return MuonCalib__NtupleSimpleResolutionTool()
	elif CalibrationTool=="Chi2Resolution":
		return MuonCalib__NtupleChi2ResolutionTool()
	elif CalibrationTool=="Efficiency":
		return MuonCalib__NtupleEfficiencyTool()
	elif CalibrationTool=="RunScan":
		return MuonCalib__NtupleRunScanTool()
	elif CalibrationTool=="ControlHistograms":
		CalibrationTool = MuonCalib__NtupleControlHistogramsTool()
	#store gate keys
		CalibrationTool.detectorStore = "DetectorStore"
		CalibrationTool.MDTIdHelper = "MDTIDHELPER"
		CalibrationTool.idToFixedIdToolType = "MuonCalib::IdToFixedIdTool"
		CalibrationTool.idToFixedIdToolName = "MuonCalib_IdToFixedIdTool"
		return CalibrationTool
	elif CalibrationTool=="MDTChamberGeometryDetermination":
		return MuonCalib__NtupleMDTChamberGeometryDeterminationTool()
	elif CalibrationTool=="Display":
		return MuonCalib__NtupleDisplayTool()
	elif CalibrationTool=="WireCentricity":
		return MuonCalib__NtupleWireCentricityTool()
	elif CalibrationTool == "DbCheck":
		return MuonCalib__NtupleDbCheckTool()
	elif CalibrationTool == "RPCTiming":
		return MuonCalib__NtupleRPCTimingTool()
	elif CalibrationTool == "ResidualVsTimeTool":
		return MuonCalib__NtupleResidualVsTimeTool()
	else:
		print "WARNING: Unnknown tool name '" + CalibrationTool + "'!"
		return None
