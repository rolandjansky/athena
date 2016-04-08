# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from CalibNtupleAnalysisAlg.CalibNtupleAnalysisAlgConf import *
from MuonCalibStandAloneBase.MuonCalibStandAloneBaseConf import *
from MdtCalibIOSvc.MdtCalibIOSvcConf import *
from MuonCalibStandAloneTools.MuonCalibStandAloneToolsConf import *
from MuonCalibStandAloneTools.MuonCalibStandAloneToolsConfig import *
from MuonCalibDbOperations.MuonCalibDbOperationsConf import MuonCalib__CalibrationDbIOTool
from AthenaCommon.AppMgr import ServiceMgr, ToolSvc
from AthenaCommon.AlgSequence import AlgSequence 
from CalibNtupleAnalysisAlg.CalibNtupleMetaData import *

import sys

#ToolSvc = Service( "ToolSvc" )

class CalibNtupleAnalysisConfig:

#--------------------------------------user settings----------------------------
#calibration algorithm to run:
#select calibration algoritm
#	scan run for chambers           RunScan
#	t0-Fitters:                     ClassicT0, MTT0
#	create rt by integration        Integration
#	autocalibration                 ClassicAutocal, AnalyticAutocal,
#                                   CurvedAutocal
#	resolution                      SimpleResolution, Chi2Resolution
#	efficiency                      Efficiency
#	event display                   Display
#	rpc timing                      RpcTiming
#   control histograms              ControlHistograms
	
  CalibrationAlgorithm = "RunScan"
	
#file list
  FileList="fl.txt"

#create segments from the rawdata part
  SegmentsFromRawdata=False

#replace drift times by raw times in existing segments
  ApplyRawTimes=False

#update the magnetic field values in the segments
  UpdateBField=False
  RefitAfterBFieldUpdate=False

#select segment author - 3=MuonBoy 4=Moore - Unused if SegmentsFromRawdata=True
  SegmentAuthor=4
	
#first and last event to analyse
  FirstEvent=0
  LastEvent=-1

#number of segments to collect
  NumberOfSegments=-1

#maximum number of segments for certain algorithms (e.g. Autocalibration)
  AutoLimitSegments = 20000

#calibration region
  CalibrationRegion = ""

#adc cut
  ADCCut = 0

#minimum and maximum number of hits BEFORE refit
  MinSegmentHits = 0
  MaxSegmentHits = -1

#perform a segment refit - not for algorithms likr MTT0, Integration, RunScan..
  SegmentRefit=False;

#segment refit parameters	
  RefineT0=False
  CurvedSegments=False
  RefitRoadWidth=2.0
  RefitTimeOut=2
  RefitMinSegmentHits=4
  RefitMaxSegmentHits=-1
	
#calibrate with b-field
  BFieldCorrection=False

#apply RPC Timing Correction
  RPCTimingCorr=False

#recalculate second coordinate
  Recalc2ndCoordinate = False

#select calibration input
  CalibInputFromDB=False
  CalibInputFromFile=True

#select calibration output
  CalibOutputToDB=False
  CalibOutputToFile=True

#output calibration directory - set None if you do not want to output to diredctory
  CalibDir="calibration"

#output to database - MP/RM/MI/NONE
  CalibDB=None
	
#initial data quality list - suppress Tubes
  InitialDQList = None	
	
#if set to true no segment recalibraiton will be done, independent on the tool
  SuppressRecalibration=False	
	
#use good run list to filter events
  GoodRunList=None
	
#apply time slewing corrections
  ApplyTimeSlewingCorrections=False	
	
#apply multilayer rt-scaling
  ApplyRtScaling=True

  sToolSvc= ToolSvc
  sServiceMgr = ServiceMgr
	
  SegmentRecalibration={}
  SegmentRefitter={}

  def AutoConfig(self):
    self.metadata=CalibNtupleMetaData(self.FileList)
    if 'GeoAtlas' in self.metadata.MetaData:
      self.sServiceMgr.GeoModelSvc.AtlasVersion = self.metadata.MetaData['GeoAtlas']
    self._create_CalibNtupleAnalysisAlg()
  #ntuple loader
    self.CalibNtupleLoader =  MuonCalib__CalibNtupleLoader()
    self.sToolSvc += self.CalibNtupleLoader
    self.CalibNtupleLoader.FileList = self.FileList
    self.CalibNtupleLoader.FirstEvent = self.FirstEvent
    self.CalibNtupleLoader.LastEvent = self.LastEvent
    self.CalibNtupleLoader.NtupleType = "AUTO"
    self.CalibNtupleAnalysisAlg.CalibSegmentPreparationTools.append( self.CalibNtupleLoader)
  #region selection
    self._region_selection()
  #good run list selection
    self._good_run_list_filter()
  #raw times
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
    self._create_calib_tool()
		
#-----------------------protected functions------------------------------------
  def _create_CalibNtupleAnalysisAlg(self):
    topSequence = AlgSequence()
    self.CalibNtupleAnalysisAlg = MuonCalib__CalibNtupleAnalysisAlg2()
    self.CalibNtupleAnalysisAlg.NumberOfSegments = self.NumberOfSegments
    if self.CalibrationAlgorithm in LimitSegmentsFor:
      self.CalibNtupleAnalysisAlg.NumberOfSegments = self.AutoLimitSegments
    topSequence += self.CalibNtupleAnalysisAlg

  def _good_run_list_filter(self):
    if not self.GoodRunList:
      return
    from GoodRunsLists.GoodRunsListsConf import GoodRunsListSelectorTool
    self.__good_run_list_tool=GoodRunsListSelectorTool()
    self.sToolSvc += self.__good_run_list_tool
    self.__good_run_list_tool.GoodRunsListVec = [self.GoodRunList]
    self.__good_run_list_filter=MuonCalib__GoodRunListFilter()
    self.__good_run_list_filter.GoodRunListTool = self.__good_run_list_tool
    self.sToolSvc +=self.__good_run_list_filter
    self.CalibNtupleAnalysisAlg.CalibSegmentPreparationTools.append( self.__good_run_list_filter )
#		self.__good_run_list_tool.EventSelectorMode = True
    self.__good_run_list_tool.PassThrough = False
		
  def _initial_DQ_list(self):
    if self.InitialDQList:
      if self.InitialDQList.upper()=="NONE":
        return
      self.InitialDqFilter = MuonCalib__InitialDqFilter()
      self.InitialDqFilter.InitialDqFile = self.InitialDQList
      self.sToolSvc += self.InitialDqFilter
      self.CalibNtupleAnalysisAlg.CalibSegmentPreparationTools.append( self.InitialDqFilter)
							
  def _region_selection(self):
    if self.SegmentsFromRawdata:
      self.RegionSelection = MuonCalib__SegmentRawdataSelector()
    else:
      self.RegionSelection = 	MuonCalib__SegmentRegionSelector()
      self.RegionSelection.SelectSegmentAuthor = self.SegmentAuthor
    self.sToolSvc += self.RegionSelection
    self.CalibNtupleAnalysisAlg.CalibSegmentPreparationTools.append( self.RegionSelection )
    self.RegionSelectionSvc = RegionSelectionSvc()
    self.RegionSelectionSvc.Region = self.CalibrationRegion
    self.RegionSelectionSvc.PrintList = True
    self.RegionSelection.AdcCut = self.ADCCut
    self.RegionSelection.MinHits = self.MinSegmentHits
    self.RegionSelection.MaxHits = self.MaxSegmentHits
    self.sServiceMgr += self.RegionSelectionSvc

  def _second_coordinate(self):
    if self.Recalc2ndCoordinate:
      self.BarrelSecondCoordinatePreparationTool = MuonCalib__BarrelSecondCoordinatePreparationTool()
      self.sToolSvc += self.BarrelSecondCoordinatePreparationTool
      self.CalibNtupleAnalysisAlg.CalibSegmentPreparationTools.append( self.BarrelSecondCoordinatePreparationTool)
			
  def _configure_segment_recalibration(self, ToolName=""):
  #tools that do not need segment recalibration
    if self.CalibrationAlgorithm in NoRecalibration:
      return
    if ToolName:
      self.SegmentRecalibration[ToolName] = MuonCalib__SegmentRecalibration(ToolName)
    else:	
      ToolName = "default"	
      self.SegmentRecalibration[ToolName] = MuonCalib__SegmentRecalibration()
    self.sToolSvc += self.SegmentRecalibration[ToolName]
    self.CalibNtupleAnalysisAlg.CalibSegmentPreparationTools.append( self.SegmentRecalibration[ToolName] )
    if self.ApplyTimeSlewingCorrections:
      self.SegmentRecalibration[ToolName].TimeSlewingSwitch='FILE'
    else:
      self.SegmentRecalibration[ToolName].TimeSlewingSwitch='UNAPPLY'
  #tools that revert all calibrations
    if self.CalibrationAlgorithm in RevertT0:
      self.SegmentRecalibration[ToolName].T0Switch = "UNAPPLY"
      self.SegmentRecalibration[ToolName].BSwitch = "LEAVE"
      self.SegmentRecalibration[ToolName].RecalcR = False
    if self.CalibrationAlgorithm in RecalibT0 :
      self.SegmentRecalibration[ToolName].T0Switch = "FILE"
      self.SegmentRecalibration[ToolName].BSwitch = "LEAVE"
      self.SegmentRecalibration[ToolName].RecalcR = False
    if self.CalibrationAlgorithm in RecalibAll:
      self.SegmentRecalibration[ToolName].T0Switch = "FILE"
      self.SegmentRecalibration[ToolName].BSwitch = "LEAVE"
      self.SegmentRecalibration[ToolName].RecalcR = True
  #id b-field should be corrected, set BSwitch either to "UNAPPLY" or to "FILE"
    if self.CalibrationAlgorithm in RevertRtScaling:
      self.SegmentRecalibration[ToolName].RtMultilayerScaling = "UNAPPLY"
    if self.CalibrationAlgorithm in ApplRtScaling and self.ApplyRtScaling:
      self.SegmentRecalibration[ToolName].RtMultilayerScaling = "FILE"
    if self.BFieldCorrection:
      if self.SegmentRecalibration[ToolName].RecalcR:
        self.SegmentRecalibration[ToolName].BSwitch = "FILE"
#     else:
#	self.SegmentRecalibration[ToolName].BSwitch = "UNAPPLY"
								
  def _config_UpdateBField(self):
    if self.UpdateBField in NoSegmentRefit:
      self.UpdateBField = None
      return
    self.UpdateBField = 	MuonCalib__UpdateBField()
    self.sToolSvc += self.UpdateBField
    self.CalibNtupleAnalysisAlg.CalibSegmentPreparationTools.append(  self.UpdateBField )
								
  def _config_segment_refitter(self, ToolName=""):
    if self.CalibrationAlgorithm in NoSegmentRefit:
      return
    if ToolName:
      self.SegmentRefitter[ToolName] = MuonCalib__SegmentRefitter(ToolName)
    else:	
      ToolName = "default"	
    self.SegmentRefitter[ToolName] = 	MuonCalib__SegmentRefitter(ToolName)
    self.sToolSvc += self.SegmentRefitter[ToolName]
    self.CalibNtupleAnalysisAlg.CalibSegmentPreparationTools.append(  self.SegmentRefitter[ToolName] )
    self.SegmentRefitter[ToolName].Curved = self.CurvedSegments
    self.SegmentRefitter[ToolName].RefineT0 = self.RefineT0
    self.SegmentRefitter[ToolName].RoadWidth = self.RefitRoadWidth
    self.SegmentRefitter[ToolName].TimeOut = self.RefitTimeOut
    self.SegmentRefitter[ToolName].MinSegmentHits = self.RefitMinSegmentHits
    self.SegmentRefitter[ToolName].MaxSegmentHits = self.RefitMaxSegmentHits
						
  def _config_calibIO(self):
    if self.CalibDir:
      self.CalibrationFileIOTool = MuonCalib__CalibrationFileIOTool()
      self.sToolSvc += self.CalibrationFileIOTool
      self.CalibrationFileIOTool.outputLocation = self.CalibDir
    if self.CalibDB and not self.CalibDB.upper()=="NONE":
      self.DbIoTool = MuonCalib__CalibrationDbIOTool()
      self.sToolSvc += MuonCalib__CalibrationDbIOTool()
      if not self.__set_db():
        print "Unknown database location " + str(self.CalibOutputDB)
        sys.exit(1)
		
    self.MdtCalibOutputDbSvc = MdtCalibOutputDbSvc()
    self.MdtCalibOutputDbSvc.PostprocessCalibration = True
    self.sServiceMgr += self.MdtCalibOutputDbSvc
    if self.CalibOutputToDB and self.CalibOutputToFile:
      self.TeeIoTool=MuonCalib__CalibrationTeeIOTool()
      self.sToolSvc +=  self.TeeIoTool
      self.TeeIoTool.IOTool2 = self.CalibrationFileIOTool
      self.TeeIoTool.IOTool1 = self.DbIoTool
      self.MdtCalibOutputDbSvc.OutputTool=self.TeeIoTool
    elif self.CalibOutputToFile:
      self.MdtCalibOutputDbSvc.OutputTool = self.CalibrationFileIOTool
    elif self.CalibOutputToDB:
      self.CalibOutputDB = self.DbIoTool
      self.MdtCalibOutputDbSvc.OutputTool=self.DbIoTool
    #input 
    self.MdtCalibInputSvc = MdtCalibInputSvc()
    self.sServiceMgr += self.MdtCalibInputSvc
    if self.CalibInputFromDB:
      if not self.DbIoTool:
        print "Set calibratino database location!"
        sys.exit(1)
      self.MdtCalibInputSvc.CalibrationInputTool = self.DbIoTool
    if self.CalibInputFromFile:
      if not self.CalibrationFileIOTool:
        print "give a calibdir!"
        sys.exit(1)
      self.MdtCalibInputSvc.CalibrationInputTool = self.CalibrationFileIOTool

  def _create_calib_tool(self):
    self.CalibrationTool=CreateCalibTool(self.CalibrationAlgorithm)
    if self.CalibrationTool:
      self.sToolSvc += self.CalibrationTool
    self.CalibNtupleAnalysisAlg.CalibrationTool = self.CalibrationTool	
		
#------------------------------------------------------------
  def __set_db(self):
    self.DbIoTool.SiteName  = self.CalibDB
    self.DbIoTool.UseValidaedT0 = self.CalibrationAlgorithm in ValidatedT0
    self.DbIoTool.UseValidaedRt = self.CalibrationAlgorithm in ValidatedRt
    if (self.CalibDB == "MP"):
      self.DbIoTool.ConnectionString = "oracle://oracle01.mppmu.mpg.de/ATLMPI.mppmu.mpg.de/ATLAS_MUONCALIB_MPI"
      self.DbIoTool.WorkingSchema = "ATLAS_MUONCALIB_MPI"
    elif (self.CalibDB == "MP_NEW"):
  # MPI
      self.DbIoTool.ConnectionString = "oracle://oracle01.mppmu.mpg.de/ATLMPI.mppmu.mpg.de/ATLAS_MUONCALIB_READER"
      self.DbIoTool.WorkingSchema = "ATLAS_MUONCALIB_MPI_META"
      self.DbIoTool.WriterConnectionString = "oracle://oracle01.mppmu.mpg.de/ATLMPI.mppmu.mpg.de/ATLAS_MUONCALIB_WRITER"
    elif (self.CalibDB == "MP_INTR"):
  # MPI
      self.DbIoTool.ConnectionString = "oracle://INTR/ATLAS_MUONCALIB_READER"
      self.DbIoTool.WorkingSchema = "ATLAS_MUONCALIB_MPI_META"
      self.DbIoTool.WriterConnectionString = "oracle://INVALID/ATLAS_MUONCALIB_WRITER"
    elif (self.CalibDB == "MI"):
  # Michigan
      self.DbIoTool.ConnectionString = "oracle://umors.grid.umich.edu/muoncal.grid.umich.edu/atlas_muoncalib_mich"
      self.DbIoTool.WorkingSchema = "atlas_muoncalib_mich"
    elif (self.CalibDB == "MI_NEW"):
  # Michigan
      self.DbIoTool.ConnectionString = "oracle://umors.grid.umich.edu/muoncal.grid.umich.edu/ATLAS_MUONCALIB_READER"
      self.DbIoTool.WorkingSchema = "ATLAS_MUONCALIB_UMICH"
      self.DbIoTool.WriterConnectionString = "oracle://umors.grid.umich.edu/muoncal.grid.umich.edu/ATLAS_MUONCALIB_WRITER"
    elif (self.CalibDB == "MI_INTR"):
  # Michigan
      self.DbIoTool.ConnectionString = "oracle://INTR/ATLAS_MUONCALIB_READER"
      self.DbIoTool.WorkingSchema = "ATLAS_MUONCALIB_UM_META"
      self.DbIoTool.WriterConnectionString = "oracle://INVALID/ATLAS_MUONCALIB_WRITER"
    elif (self.CalibDB == "RM_NEW"):
  # ROMA
      self.DbIoTool.ConnectionString = "oracle://atlas-cluster.cr.cnaf.infn.it:1521/atlcnafsrv.cr.cnaf.infn.it/ATLAS_MUONCALIB_READER"
      self.DbIoTool.WorkingSchema = "ATLAS_MUONCALIB_RM_META"
      #self.DbIoTool.WorkingSchema = "ATLAS_MUONCALIB_RM_2012"
      self.DbIoTool.WriterConnectionString = "oracle://atlas-cluster.cr.cnaf.infn.it:1521/atlcnafsrv.cr.cnaf.infn.it/ATLAS_MUONCALIB_WRITER"
    elif (self.CalibDB == "RM_INTR"):
  # ROMA
      self.DbIoTool.ConnectionString = "oracle://INTR/ATLAS_MUONCALIB_READER"
      self.DbIoTool.WorkingSchema = "ATLAS_MUONCALIB_RM_META"
      self.DbIoTool.WriterConnectionString = "oracle://INVALID/ATLAS_MUONCALIB_WRITER"
    elif (self.CalibDB == "RM"):
  # ROMA
      self.DbIoTool.ConnectionString = "oracle://atlas-cluster.cr.cnaf.infn.it/atlcnaf.cr.cnaf.infn.it//atlas_muoncalib_rome"
      self.DbIoTool.WorkingSchema = "atlas_muoncalib_rome"
    else:
      return False
    return True
