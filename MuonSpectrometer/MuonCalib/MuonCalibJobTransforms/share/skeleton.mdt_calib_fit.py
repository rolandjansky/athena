		

# get the logger
from AthenaCommon.Logging import *
from MuonCalibJobTransforms.GetFitBy import *
mdtCalibFitlog = logging.getLogger('mdtCalibFit')

def GetGroupByList(finest):
	if finest=="LAYER":
		return ["CHAMBER", "MULTILAYER", "LAYER"]
	ret=[]
	for group_by in ["CHAMBER", "MULTILAYER", "MEZZ_CARD", "TUBE"]:
		ret.append(group_by)
		if finest==group_by:
			return ret
	raise ValueError("Illegal group by parameter " + finest)
	return []
		
		

mdtCalibFitlog.info('************** STARTING MDT CalibFit !!!!**************')
#==============================================================
# Job definition parameters:
#==============================================================

filelist=runArgs.calibNtupleFileList
#file list
if runArgs.calibNtupleFileList[:4].upper() == "CSL:":
	import tempfile
	fl=tempfile.NamedTemporaryFile(mode="w", dir=".", delete=False)
	filelist = fl.name
	for inf in runArgs.calibNtupleFileList[4:].split(","):
		fl.write(inf)
		fl.write("\n")
	del fl


EvtMax = runArgs.maxEvents
SkipEvents = runArgs.skipEvents
CalibSite  = runArgs.calibSite
geover     = runArgs.geometryVersion
if geover.upper()=="AUTO":
	from CalibNtupleAnalysisAlg.CalibNtupleMetaData import *
	meta_data=CalibNtupleMetaData(filelist)
	if not 'GeoAtlas' in meta_data.MetaData:
		mdtCalibFitlog.fatal("geometry version not found in meta data")
		geover="INVALID"
	else:
		geover=meta_data.MetaData['GeoAtlas']
		mdtCalibFitlog.info("Set Geo-Version to " + geover)
	
	
if geover.find('NF') >=0:
  fieldoff=True
else:
  fieldoff=False

mdtCalibFitlog.info( '**** Transformation run arguments ****' )
mdtCalibFitlog.info( str(runArgs) )

#==============================================================
# Job Configuration parameters:
#==============================================================
from MuonCalibJobTransforms.MDTCalibFitConfig import mdtCalibFitConfig
for cf in runArgs.jobConfig:
    include(cf)
mdtCalibFitlog.info( '**** Transformation configuration arguments ****' )
mdtCalibFitlog.info( str(mdtCalibFitConfig) )

### Setup Athena common flags
#include ("AthenaCommon/AthenaCommonFlags.py")
#include.block("AthenaCommon/AthenaCommonFlags.py")

#include( "IOVDbSvc/IOVRecExCommon.py" )

doMuonCalibAtlas=True

from AthenaCommon.AppMgr import ServiceMgr

from AthenaCommon.AppMgr import ToolSvc
from CalibNtupleAnalysisAlg.CalibNtupleAnalysisConfig import *
from MuonCalibStandAloneExtraTools.CalibExtraNtupleAnalysisConfig import *
from AthenaCommon.GlobalFlags import globalflags
#GlobalFlags.DetGeo.set_atlas()
#GlobalFlags.DataSource.set_data()
#GlobalFlags.Print()
globalflags.DetGeo.set_Value_and_Lock('atlas')
ToolSvc = Service( "ToolSvc" )
globalflags.DataSource.set_Value_and_Lock('data')

from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.Muon_setOn()
DetFlags.detdescr.ID_setOff()
DetFlags.detdescr.LAr_setOff()
DetFlags.detdescr.Tile_setOff()



#Set geometry tag
from AtlasGeoModel import SetGeometryVersion, GeoModelInit

GeoModelSvc = ServiceMgr.GeoModelSvc
GeoModelSvc.AtlasVersion = geover

#Config=CalibNtupleAnalysisConfig()

if runArgs.calibrationAlgorithm == "MdtDqa" or (runArgs.calibrationAlgorithm == "Splitter" and runArgs.splitterStoreTracks) or runArgs.trackPCut > -2:
  Config=CalibExtraNtupleAnalysisConfig()
else:
  Config=CalibNtupleAnalysisConfig()

#===============================================================================
#Begin User Settings

#calibratino algorithm to run:
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
#       MDT DQA				MdtDqa
Config.CalibrationAlgorithm = runArgs.calibrationAlgorithm

Config.FileList = filelist

#crete segments from the rawdata part
Config.SegmentsFromRawdata=runArgs.segsFromRaw

#select segment author - 3=MuonBoy 4=Moore - Unused if SegmentsFromRawdata=True
Config.SegmentAuthor=runArgs.MuonReco

#select only segments on tracks
if hasattr(Config, "SelectTrackSegments"):
	Config.SelectTrackSegments= (runArgs.trackPCut >= -1)

#apply 2nd coordinate of tracks to the segment - only works if SelectTrackSegments=True
if hasattr(Config, "TrackSecondCoordinate"):
	Config.TrackSecondCoordinate=False

if hasattr(Config, "MomentumCut"):
	Config.MomentumCut = runArgs.trackPCut

if hasattr(Config, "FastTrackMatch"):
	Config.FastTrackMatch = runArgs.fastTrackMatch

if hasattr(Config, "SuperFastTrackMatch"):
	Config.SuperFastTrackMatch = runArgs.superFastTrackMatch

#first and last event to analyse
Config.FirstEvent=SkipEvents
Config.LastEvent=EvtMax

#number of segments to collect
#Config.NumberOfSegments=20000

#maximum number of segments for certain algorithms (e.g. Autocalibration)
Config.AutoLimitSegments = 20000

#calibration region
if Config.CalibrationAlgorithm != "RunScan":
        Config.CalibrationRegion = runArgs.selectChamber	

#perform a segment refit - not for algorithms likr MTT0, Integration, RunScan..
Config.SegmentRefit=runArgs.segmentRefit

#segemtn refit parametersConfig.
Config.RefineT0=runArgs.refineT0
Config.CurvedSegments= runArgs.curvedSegs
Config.RefitRoadWidth=3.0
Config.RefitTimeOut=2
Config.RefitMinSegmentHits=4
Config.RefitMaxSegmentHits=-1

#calibrate with b-field
Config.BFieldCorrection=runArgs.bfieldCorr

#recalculate second coordinate
Config.Recalc2ndCoordinate = False

#select calibration input
Config.CalibInputFromDB=runArgs.inputFromDB
Config.CalibInputFromFile=runArgs.inputFromFile

#select calibration output
Config.CalibOutputToDB=runArgs.outputToDB
Config.CalibOutputToFile=runArgs.outputToFile

#output calibration directory - set None if you do not want to output to diredctory
Config.CalibDir="calibration"

#output to database - MP/RM/MI/NONE
Config.CalibDB=CalibSite

#initial data quality list - suppress Tubes
if Config.CalibrationAlgorithm != "RunScan":
	Config.InitialDQList = None

#if set to true no segment recalibraiton will be done, independent on the tool
Config.SuppressRecalibration=runArgs.suppressRecalibration		


Config.ApplyTimeSlewingCorrections=runArgs.applyTimeSlewingCorrection

Config.ApplyRtScaling=runArgs.applyRtScaling

#===============================================================================
Config.RPCTimingCorr= False
Config.AutoConfig()
if hasattr(Config, "RpcTiming"):
	Config.RpcTiming.OffsetFile = 'rpc_offsets.root'
#===============================================================================

#set username and passwd and head_id
if hasattr(Config, "DbIoTool"):
	Config.DbIoTool.HeadId=runArgs.headId	
	
if hasattr(Config, "TrackSecondCoordinateTool"):
	Config.TrackSecondCoordinateTool.ControlHistograms = False

if hasattr(Config, "CalibNtupleLoader"):
       Config.CalibNtupleLoader.NtupleType = "NORMAL"
#	Config.CalibNtupleLoader.NtupleType = "AUTO"
#	Config.CalibNtupleLoader.NtupleType = "REGION"

#++++++++++++++++++Algoritm specific settings+++++++++++++++++++++++++++++++++++
#-----------------for MdtDqa ----------------------------------------------
if Config.CalibrationAlgorithm == "MdtDqa":
        #----------- jobOptions for MdtDqaNtupleAnalysis :
        Config.CalibrationTool.MdtDqaFileName = runArgs.mdtDqaFile
        Config.CalibrationTool.Verbose = False
        Config.CalibrationTool.FillHistos = runArgs.fillHistos
        Config.CalibrationTool.doFinalize = runArgs.doFinalize
        Config.CalibrationTool.doSegHitResidsVsRadius = runArgs.hitResid
        Config.CalibrationTool.doTracks = False 
        Config.CalibrationTool.TrkAuthor = 0
        Config.CalibrationTool.DeadElementsAlgorithm = runArgs.DeadElAlg #0=Toni,1=Marco
        Config.CalibrationTool.ADCCUT=runArgs.ADCcut
        #----------- jobOptions for MdtDqaTubeEfficiency :
        Config.CalibrationTool.doEfficiency = runArgs.doEfficiency
        Config.CalibrationTool.EffiNSigma =  runArgs.EffNsig
        Config.CalibrationTool.EffiChi2Cut = runArgs.EffChiCut
        Config.CalibrationTool.EffiUseDefaultResolution = runArgs.EffUseDefRes
        Config.CalibrationTool.EffiHitADCCut = 0.
        Config.CalibrationTool.EffiGTFitON = runArgs.effiGtf
        Config.CalibrationTool.EffiUseNewCalibConstants = runArgs.effiReCal
        Config.CalibrationTool.EffiUseTimeCorrections = runArgs.effiTimeCorr

        #----------- jobOptions for MdtDqaGlobalTimeFit :
        Config.CalibrationTool.doGlobalTimeFit = runArgs.doGlobalTimeFit
        Config.CalibrationTool.rtDefaultBfieldON = 0
        Config.CalibrationTool.GTFitSeg_minNumHits = 5
        Config.CalibrationTool.GTFitSeg_maxNumHits = 10
        Config.CalibrationTool.GTFitSeg_chi2Cut = 15.0
        Config.CalibrationTool.GTFitDebug = False

#------------------------------------for run scan-------------------------------
if Config.CalibrationAlgorithm == "RunScan":
#	CalibNtupleAnalysisAlg.RefitSegments = False
	#minimum number of hits per fit
	Config.CalibrationTool.MinHits = 10000
	#maximum rate of bad hits per chamber
	Config.CalibrationTool.MaxBadFits = 0.9999
	#if set to true ommit output for chambers which are not to be fitted
	Config.CalibrationTool.SuppressNofit = True
	#san all chambers
#	CalibNtupleAnalysisAlg.InitialDQList="NONE"
#------------------------------------for MT t0 fitter---------------------------
if Config.CalibrationAlgorithm == "MTT0" :
	#add fitted function to histogram
	Config.CalibrationTool.AddFitFun = True
	#create TDirectory contining some debugging graphs
	Config.CalibrationTool.DrawDebugGraphs = True
	#range of histogram ind 25/32 bins
	Config.CalibrationTool.NumberOfNegativeBins = 1000
	Config.CalibrationTool.NumberOfPositiveBins = 3000
	#Fit for group of tubes
	#	TUBE 		one spectrum per tube
	#	LAYER 		one spectrum per layer
	#	MULTILAYER	one spectrum per multilayer
	#	CHAMBER		one spectrum per chamber
	#	MEZZ_CARD	one spectrum per mezzanine card
	if type(Config.CalibrationTool.GroupBy) is list:
		fit_by=runArgs.t0FitBy
		if  fit_by=="AUTO":
			fit_by=GetFitBy(runArgs.selectChamber)
		Config.CalibrationTool.GroupBy=GetGroupByList(fit_by)	
		Config.CalibrationTool.ADCGroupBy = GetGroupByList(runArgs.adcFitBy)
		Config.CalibrationTool.MinumumEntriesPerTimeFit=10000
		Config.CalibrationTool.MinumumEntriesPerADCFit=1000
	else:
		if runArgs.t0FitBy=="AUTO":
			Config.CalibrationTool.GroupBy = GetFitBy(runArgs.selectChamber)
		else:
			Config.CalibrationTool.GroupBy = runArgs.t0FitBy
		Config.CalibrationTool.ADCGroupBy = runArgs.adcFitBy
	Config.CalibrationTool.UseTopChi2 = True
	Config.CalibrationTool.ScrambleThreshold = 2
	Config.CalibrationTool.SclicingThreshold = 3

#------------------------------for classic t0 fitter----------------------------
if Config.CalibrationAlgorithm == "ClassicT0":
	CalibNtupleAnalysisAlg.RefitSegments = runArgs.segmentRefit
	CalibNtupleAnalysisAlg.RefineT0 = runArgs.refineT0
	#range and binning of adc spectrum
	Config.CalibrationTool.NAdcBins = 100
	Config.CalibrationTool.MinAdc = 0.0
	Config.CalibrationTool.MaxAdc = 300.0
	#range and binning of the time spectrum
	Config.CalibrationTool.NTimeBins = 1280
	Config.CalibrationTool.TimeMin = 0.0
	Config.CalibrationTool.TimeMax = 2000.0
	#switch on/off fitting of time spectrum
	Config.CalibrationTool.FitTimeSpectrum = True
	#Minimum number of entries
	Config.CalibrationTool.MinEntries = 2000
	#InitParam 1 = automatic search for initial fit parameters
	Config.CalibrationTool.InitParam = 1
	#maximum chi^2
	Config.CalibrationTool.MaxChi2 = 10.0
	
#----------------------------for integration method-----------------------------
if Config.CalibrationAlgorithm == "Integration":
	#mazimum drift time
	Config.CalibrationTool.TMaxInt = 710
	#set to true if close hits are stored in the ntuple
	Config.CalibrationTool.HaseCloseHits = False
	#the inner radius of the drift tube 
	Config.CalibrationTool.InnerTubeRadius = 14.6
	#write out default resolution
	Config.MdtCalibOutputDbSvc.ForceDefaultResolution = True
	#set the lower and upper radius of the parabolic at the end of the r-t
	Config.CalibrationTool.LowerExtrapolationRadius = 13.0;
	Config.CalibrationTool.UpperExtrapolationRadius = 14.0;
	Config.CalibrationTool.StoreMultilayerTmaxDiff = runArgs.applyRtScaling

# --------------------for classical autocalibration-----------------------------
if Config.CalibrationAlgorithm  == "ClassicAutocal" :
	#range for the number of hits
	Config.CalibrationTool.MinimumNumberOfHits = 5
	Config.CalibrationTool.MaximumNumberOfHits = 10
	#maximum number of iterations
	Config.CalibrationTool.NumberOfIterations = 10
	#chi2 cuts - ?
	Config.CalibrationTool.Chi2CutVec = [ 50, 40, 30, 20, 10, 10, 10, 5, 5, 5 ]
	Config.CalibrationTool.Chi2Cut = 10.0


#-------------------for analytic autocalibration--------------------------------
if Config.CalibrationAlgorithm == "AnalyticAutocal":
	#accuracy of the initial rt relation
	Config.CalibrationTool.Accuracy = 2.0
	#type of the correction function LEGENDRE|CHEBYSHEV|POLYGON
	Config.CalibrationTool.FunctionType = "LEGENDRE"
	#order of the correction function
	Config.CalibrationTool.FunctionOrder = 10
	#use the full matrix - ?
	Config.CalibrationTool.FullMatrix = True
	# fix minimum/maximum
	Config.CalibrationTool.FixMin = True # fix r(t0)
	Config.CalibrationTool.FixMax = False # do not fix r(tmax)
	#maximum number of iterations
	Config.CalibrationTool.MaximumIterations = 10
	# split the segments up according the multilayers
	Config.CalibrationTool.Split = True
	# smoothening using the conventional approach after autocalibration
	# WARNING: if Split = True and the whole chamber is selected as calibration
	# region, curved segments over the whole chambers are
	# fitted in the smoothening procedure which cause no smoothening in
	# case of a multilayer as calibration region
	Config.CalibrationTool.ConventionalSmoothening = False
    # parabolic extrapolation for small and large radii;
    # set variable to True for parabolic extrapolation
	Config.CalibrationTool.ParabolicExtrapolation = True
    # control histograms
	Config.CalibrationTool.ControlHistograms=True

#-------------------for curved autocalibration----------------------------------
if Config.CalibrationAlgorithm == "CurvedAutocal":
	#accuracy of the initial rt relation
	Config.CalibrationTool.Accuracy = 2.0
	#type of the correction function LEGENDRE|CHEBYSHEV|POLYGON
	Config.CalibrationTool.FunctionType = "LEGENDRE"
	#order of the correction function
	Config.CalibrationTool.FunctionOrder = 10
	# fix minimum/maximum
	Config.CalibrationTool.FixMin = True # fix r(t0)
	Config.CalibrationTool.FixMax = False # do not fix r(tmax)
	#maximum number of iterations
	Config.CalibrationTool.MaximumIterations = 10
    # parabolic extrapolation for small and large radii;
    # set variable to True for parabolic extrapolation
	Config.CalibrationTool.ParabolicExtrapolation = True
	#control histograms
	Config.CalibrationTool.ControlHistograms=True
	Config.CalibrationTool.MultilayerRtScale = runArgs.applyRtScaling

#-----------------for simple resolution-----------------------------------------
#--select NumberOfSegments=10000
#--switch on RefineT0 and use HistFitMethod for commissioning data
if Config.CalibrationAlgorithm == "SimpleResolution":
    Config.CalibrationTool.CurvedFit = True #use a curved track fit (set it to true
	                                  #for data with magnetic field)
#selection criteria
    Config.CalibrationTool.RejectLow = 0.1 #default 0.1
    Config.CalibrationTool.RejectTop = 0.40 #default 0.30
#convergence
    Config.CalibrationTool.DeltaConv = 0.06 #default 0.05
    Config.CalibrationTool.DiffMax = 0.007 #default 0.006 microns
    Config.CalibrationTool.MaximumIterations = 20 #default 20
#Replace initial spatial resoluton with flat one (=0.3mm). Used for validation.
    Config.CalibrationTool.InitialFlatResolution = False
#Obtain resolution using histogram fitting. Usefull in case of 'noisy' residuals.
    Config.CalibrationTool.HistFitMethod = True
#Remove hits with large residuals (outliers).
#	Config.CalibrationTool.SuppressOutliers = True
#	Config.CalibrationTool.SuppressionLow = 0.17
#	Config.CalibrationTool.SuppressionTop = 0.17
    Config.CalibrationTool.OutputResolutionFunction = "spr_out.root"	
	

#-----------------for RCP Timing------------------------------------------------
if Config.CalibrationAlgorithm == "RpcTiming":
	CalibNtupleAnalysisAlg.RefineT0 = runArgs.refineT0
	#analysis cue - defaults to "MODULE", "DOUBLET", "STATION", "PAIRS", "MEAN", "ALLSPEC"
#	Config.CalibrationTool.Cue = [ "MODULE" ]
	#output and input file for timing constants
	#NONE: do not read/ write file
	#AUTO: filename is rpc_timing<run_nr>.txt
	Config.CalibrationTool.TimingInfile = "NONE"
	Config.CalibrationTool.TimingOutfile = "AUTO"
	#automatically select all stations for this analysis
	CalibNtupleAnalysisAlg.SelectStation = "ANY"
	CalibNtupleAnalysisAlg.SelectEta = 0
	CalibNtupleAnalysisAlg.SelectPhi = -1

#-----------------for control histograms----------------------------------------
if Config.CalibrationAlgorithm == "ControlHistograms":
	#name of the ROOT output file
	Config.CalibrationTool.ROOTFileName = "NtupleControlHistogramsTool.root"

#-----------------for MDT chamber geometry determination------------------------
if Config.CalibrationAlgorithm == "MDTChamberGeometryDetermination":
	#write out a ROOT control file
	Config.CalibrationTool.writeControlHistograms = True
	#name of this ROOT file
	Config.CalibrationTool.ROOTFileName = "NtupleMDTChamberGeometryDeterminationTool.root"
	#granularity: "TUBE", "LAYER", "MULTILAYER"
	Config.CalibrationTool.granularity = "MULTILAYER"
	#reference multilayer (1 or 2)
	Config.CalibrationTool.referenceMultilayer = 1
	#road width for pattern recognition in mm
	#(also used in the misalignment cuts)
	Config.CalibrationTool.roadWidth = 2.0
	#geometry file tag for the output of the initial and final geometry
	Config.CalibrationTool.geometryFileName = "MDTgeom"
	#store gate keys
	Config.CalibrationTool.detectorStore = "DetectorStore"
	Config.CalibrationTool.MDTIdHelper = "MDTIDHELPER"
	Config.CalibrationTool.idToFixedIdToolType = "MuonCalib::IdToFixedIdTool"
	Config.CalibrationTool.idToFixedIdToolName = "MuonCalib_IdToFixedIdTool"

#-----------------for efficiencies----------------------------------------------
if Config.CalibrationAlgorithm == "Efficiency":
        Config.CalibrationTool.nSegmentHits = 5
        Config.CalibrationTool.roadWidth = 2.0
        Config.CalibrationTool.fileName = "TubeEfficiencies"
        Config.CalibrationTool.Debug = False
        Config.CalibrationTool.Chi2Cut = 10
        Config.CalibrationTool.excludeLayer = False

#-----------------for display---------------------------------------------------
if Config.CalibrationAlgorithm == "Display":
        Config.CalibrationTool.nSegmentHits = 5
        Config.CalibrationTool.roadWidth = 2.0
        Config.CalibrationTool.adcCut = CalibNtupleAnalysisAlg.RawdataAdcCut
        #Config.CalibrationTool.displayEvent = 123
        #Config.CalibrationTool.scaleDisplay = 0.5

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++### Setup Athena common flags
if Config.CalibrationAlgorithm == "Splitter":
	levels={"NONE":0, "NAME":1, "NAME_PHI":2, "NAME_PHI_ETA":3}
	if  runArgs.splitterStoreTracks:
		Config.CalibrationTool.FillTrackAuthors = [-1]
		if hasattr(Config, "RegionSelection"):
			if hasattr(Config.RegionSelection, "TrackAutors"):
				if len(Config.RegionSelection.TrackAutors):
					Config.CalibrationTool.FillTrackAuthors = Config.RegionSelection.TrackAutors
		if hasattr(Config.RegionSelection, "MomentumCut"):
			Config.CalibrationTool.TrackPCut = Config.RegionSelection.MomentumCut
	Config.CalibrationTool.FileSplitLevel =levels[runArgs.splitterFileSplitLevel]
	Config.CalibrationTool.NtupleSplitLevel =levels[runArgs.splitterNtupleSplitLevel]
	Config.CalibrationTool.FilenamePrefix="OutNtuple"
	Config.CalibrationTool.MaxPatterns=200000
	Config.CalibrationTool.StoreTruth=False
	Config.CalibrationTool.StoreMdtRawdata=runArgs.splitterStoreRaw
	Config.CalibrationTool.StoreRpcRawdata=runArgs.splitterStoreRaw
	Config.CalibrationTool.StoreTgcRawdata=runArgs.splitterStoreRaw
	Config.CalibrationTool.TriggerForSegmentStationOnly=True
	Config.CalibrationTool.AdcCut=runArgs.splitterADCCut
	from MuonCalibStandAloneTools.MuonCalibStandAloneToolsConf import MuonCalib__NtupleRunScanTool
	Config.CalibrationTool2 = MuonCalib__NtupleRunScanTool()
	ToolSvc += Config.CalibrationTool2
	Config.CalibrationTool2.MinHits = 10000
	#maximum rate of bad hits per chamber
	Config.CalibrationTool2.MaxBadFits = 0.9999
	#if set to true ommit output for chambers which are not to be fitted
	Config.CalibrationTool2.SuppressNofit = True
	Config.CalibrationTool.ToolToRun=Config.CalibrationTool2
print AlgSequence()
print ToolSvc
print ServiceMgr
from ROOT import gROOT, TCanvas, TF1
theApp.EvtMax = 1
