# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from MuonCalibStandAloneExtraTools.MuonCalibStandAloneExtraToolsConfig import *
print  "NoRecalibration=", NoRecalibration
from CalibNtupleAnalysisAlg.CalibNtupleAnalysisConfig import *
print  "NoRecalibration=", NoRecalibration
from MuonCalibStandAloneExtraTools.MuonCalibStandAloneExtraToolsConf import *
print  "NoRecalibration=", NoRecalibration
#ToolSvc = Service( "ToolSvc" )


def GenTrackAuthorCodes(author, primary_only=False, combined_only=False, stand_alone_only=False):
	track_types=range(0, 5)
	if combined_only:
		track_types=[2,3]
	if stand_alone_only:
		track_types=[0]
	
	ret=[]
	for tp in track_types:
		ret.append(tp + 100* author + 10)
	if not primary_only:
		for tp in track_types:
			ret.append(tp + 100* author)
	
	return ret	

class CalibExtraNtupleAnalysisConfig(CalibNtupleAnalysisConfig):

	LoadExtraNtuple=True
	SelectTrackSegments=True
	TrackSecondCoordinate=False
	MomentumCut=-1.0
	FastTrackMatch=True
	SuperFastTrackMatch=False
	
	def AutoConfig(self):
		self._create_CalibNtupleAnalysisAlg()
	#ntuple loader
		if self.LoadExtraNtuple:
			self.CalibNtupleLoader =  MuonCalib__CalibExtraNtupleLoader()
		else:	
			self.CalibNtupleLoader =  MuonCalib__CalibNtupleLoader()
		self.sToolSvc += self.CalibNtupleLoader
		self.CalibNtupleLoader.FileList = self.FileList
		self.CalibNtupleLoader.FirstEvent = self.FirstEvent
		self.CalibNtupleLoader.LastEvent = self.LastEvent
		if not self.LoadExtraNtuple:
			self.CalibNtupleLoader.NtupleType = "AUTO"
		self.CalibNtupleAnalysisAlg.CalibSegmentPreparationTools.append( self.CalibNtupleLoader)
	#region selection
		if not self.SelectTrackSegments or not self.LoadExtraNtuple:
			self._region_selection()
		else:
			self._track_region_selection()
	#raw times
		self._good_run_list_filter()
		if self.ApplyRawTimes:
			self.RawTimesReplacement=MuonCalib__ApplyRawTimes()
			self.sToolSvc += self.RawTimesReplacement
			self.CalibNtupleAnalysisAlg.CalibSegmentPreparationTools.append(self.RawTimesReplacement)
	#initial DQ list
		self._initial_DQ_list()
	#apply RPC Timing Correction
		if self.RPCTimingCorr:
			self.RpcTiming=MuonCalib__RpcTimingCorr()
			self.sToolSvc += self.RpcTiming
			self.CalibNtupleAnalysisAlg.CalibSegmentPreparationTools.append(self.RpcTiming)	
		if self.SelectTrackSegments and self.TrackSecondCoordinate and self.LoadExtraNtuple:
			self.TrackSecondCoordinateTool = MuonCalib__TrackSecondCoordinateToSegment()
			self.sToolSvc += self.TrackSecondCoordinateTool
			self.CalibNtupleAnalysisAlg.CalibSegmentPreparationTools.append(self.TrackSecondCoordinateTool)
	#recalculate 2nd coordinate:
		self._second_coordinate()
	#segment recalibration
		if not self.SuppressRecalibration:
			self._configure_segment_recalibration("SegmentRecalibration")
	#segment refitter
		if self.SegmentRefit:
			self._config_segment_refitter()		
    #B field updater
		if self.UpdateBField:
			self._config_UpdateBField()
			self._configure_segment_recalibration("SegmentRecalibration2")
			if self.SegmentRefit and self.RefitAfterBFieldUpdate:
				self._config_segment_refitter("SegmentRefitter2")
	#calibration IO
		self._config_calibIO()
	#create tool
		self._create_calib_tool_extra()
		
		
		

#------------------------------------------------------------------------------
	def _create_calib_tool_extra(self):
		self._create_calib_tool()
		if  self.CalibrationTool:
			return
		self.CalibrationTool=CreateCalibExtraTool(self.CalibrationAlgorithm)
		if self.CalibrationTool:
			self.sToolSvc += self.CalibrationTool
		self.CalibNtupleAnalysisAlg.CalibrationTool = self.CalibrationTool	
		
			
	
	def _track_region_selection(self):
		if self.SuperFastTrackMatch:
			self.RegionSelection = MuonCalib__ExtraRegionSelectorSuperFast()
			if self.SegmentAuthor == 3:
				self.RegionSelection.TrackAutors=GenTrackAuthorCodes(1)
			elif self.SegmentAuthor == 4:
				self.RegionSelection.TrackAutors=GenTrackAuthorCodes(0)
			elif self.SegmentAuthor == 5:
				self.RegionSelection.TrackAutors=GenTrackAuthorCodes(2)
			
		elif self.FastTrackMatch:
			self.RegionSelection = MuonCalib__ExtraRegionSelectorFast()
			self.RegionSelection.SelectSegmentAuthor = self.SegmentAuthor
			if self.SegmentAuthor == 3:
				self.RegionSelection.TrackAutors=GenTrackAuthorCodes(1)
			elif self.SegmentAuthor == 4:
				self.RegionSelection.TrackAutors=GenTrackAuthorCodes(0)
			elif self.SegmentAuthor == 5:
				self.RegionSelection.TrackAutors=GenTrackAuthorCodes(2)
		else:
			self.RegionSelection = MuonCalib__ExtraRegionSelector()
			self.RegionSelection.MuonBoyTracks = (self.SegmentAuthor == 3)
			self.RegionSelection.SelectSegmentAuthor = self.SegmentAuthor
		self.RegionSelectionSvc = RegionSelectionSvc()
		self.RegionSelectionSvc.Region = self.CalibrationRegion
		self.RegionSelection.MomentumCut = self.MomentumCut
		self.RegionSelection.MinHits =  self.MinSegmentHits
		self.RegionSelection.MaxHits =  self.MaxSegmentHits
		self.sToolSvc += self.RegionSelection
		self.CalibNtupleAnalysisAlg.CalibSegmentPreparationTools.append( self.RegionSelection )
		RegionSelectionSvc.PrintList = True
		self.sServiceMgr += self.RegionSelectionSvc
		
