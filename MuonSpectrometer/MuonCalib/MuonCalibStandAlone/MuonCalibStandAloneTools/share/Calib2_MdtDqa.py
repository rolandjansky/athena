### Setup Athena common flags
include ("AthenaCommon/AthenaCommonFlags.py")
include.block("AthenaCommon/AthenaCommonFlags.py")

include( "IOVDbSvc/IOVRecExCommon.py" )

doMuonCalibAtlas=True

from AthenaCommon.AppMgr import ServiceMgr
from MuonCalibStandAloneBase.MuonCalibStandAloneBaseConf import RegionSelectionSvc
RegionSelectionSvc = RegionSelectionSvc()
#RegionSelectionSvc.Region = "[B??,5] & !([BEE])"
#RegionSelectionSvc.Region = "[BML,4,1]"
RegionSelectionSvc.Region = "[B??]"
# RegionSelectionSvc.Region = "[BIR,6,1]"
RegionSelectionSvc.PrintList = True
ServiceMgr += RegionSelectionSvc
print RegionSelectionSvc
##################################USER SETTINGS#################################

#select calibration algoritm
#	scan run for chambers			RunScan
#	t0-Fitters: 				ClassicT0, MTT0
#	create rt by integration 		Integration
#	autocalibration 			ClassicAutocal, AnalyticAutocal
#	resolution				SimpleResolution, Chi2Resolution
#	efficiency				Efficiency
#       MDT DQA                                 MdtDqa
#	event display				Display
#	rpc timing				RpcTiming
#       control histograms              	ControlHistograms
#       MDT chamber geometry determination      MDTChamberGeometryDetermination
CalibrationAlgorithm = "MdtDqa"

#select calibration site for the database access NONE|ROMA
#for different sites, you will have to edit 
#CalibNtupleAnalysisAlg/CalibNtupleAnalysisAlg2.py
CalibrationSite = "NONE"

#if set to true no drift ties and drift radii will be recalculated
#use this for validaion
NoRecalculation = False

####################DO NOT CHANGE THS #############################
include( "CalibNtupleAnalysisAlg/CalibNtupleAnalysisAlg13.py" )  ##
include( "MdtCalibIOSvc/MdtCalibIOSvc.py" )                      ##
include( "MuonCalibStandAloneTools/MuonCalibStandAloneTools.py" )##
#include( "RpcTimingTool/RpcTimingTool.py")		         ##
from MuonCalibStandAloneBase.MuonCalibStandAloneBaseConf import MuonCalib__CalibrationTeeIOTool
CalibrationTeeIOTool = MuonCalib__CalibrationTeeIOTool()	 ##
ToolSvc += CalibrationTeeIOTool					 ##
###################################################################

#set to true if you want to insert the calibration constants to the database
MdtCalibOutputDbSvc.PostprocessCalibration = True

# list on files to be read in 
CalibNtupleAnalysisAlg.FileList = "file_list.txt"

#first event to be read in
CalibNtupleAnalysisAlg.FirstEvent = 0

#Trigger bit selection
#CalibNtupleAnalysisAlg.RequestedTAV = [ 53 ]
#CalibNtupleAnalysisAlg.VetoedTAV = [ 54 ]
#CalibNtupleAnalysisAlg.RequestedTAP = [ 53 ]
#CalibNtupleAnalysisAlg.VetoedTAP = [ 53 ]


#last event to be read in
#CalibNtupleAnalysisAlg.LastEvent = 20000

#number of segments needed for calibration
#reading of tuple will stop when number of segments are read
#an error will occur when not enough segments are read
#CalibNtupleAnalysisAlg.NumberOfSegments = 2000

#select output location for file io
#CalibrationFileIOTool.outputLocation = "calib_alt"

#use a flat resolution as default
#MdtCalibOutputDbSvc.FlatDefaultResolution=0.5

#reject segments which extend over more than one station
CalibNtupleAnalysisAlg.RejectMultistationTracks = True

#use mdt raw tdc as drift time
#CalibNtupleAnalysisAlg.TakeMdtRaw = False

#set to true if you 
CalibNtupleAnalysisAlg.segmentsFromRawData = False

#refit segments
CalibNtupleAnalysisAlg.RefitSegments = False
CalibNtupleAnalysisAlg.RefitRoadwidth = 2
#refine t0
CalibNtupleAnalysisAlg.RefineT0 = False
#limit the number of rawdata hits which are converted to one segemnt
CalibNtupleAnalysisAlg.MaxRawdataHits = 16
CalibNtupleAnalysisAlg.RawdataAdcCut = 0

#Minimum number of hits per segment
CalibNtupleAnalysisAlg.MinSegmentHits=3

#Request hits in both multilayers
CalibNtupleAnalysisAlg.HitsInBothMultilayers = False

#get b-field from service
CalibNtupleAnalysisAlg.UseMagFieldSvc = True

#MdtCalibInputSvc.CalibrationInputTool = CalibrationFileIOTool

#read calibration constants from this directory
if CalibrationAlgorithm != "MTT0" and  CalibrationAlgorithm != "ClassicT0" and CalibrationAlgorithm != "RunScan"  and not NoRecalculation:
	MdtCalibInputSvc.CalibrationInputTool = CalibrationFileIOTool


#Set geometry tag
from AthenaCommon.AppMgr import ServiceMgr
from AtlasGeoModel import SetGeometryVersion, GeoModelInit

GeoModelSvc = ServiceMgr.GeoModelSvc
#GeoModelSvc.AtlasVersion = "ATLAS-CSC-01-02-00"


#select output tool
CalibrationTeeIOTool.IOTool1=CalibrationFileIOTool
CalibrationTeeIOTool.IOTool2=CalibrationOracleFileIOTool
MdtCalibOutputDbSvc.OutputTool=CalibrationTeeIOTool

#++++++++++++++++++Algoritm specific settings+++++++++++++++++++++++++++++++++++

#------------------------------------for run scan-------------------------------
if CalibrationAlgorithm == "RunScan":
	CalibNtupleAnalysisAlg.RefitSegments = False
	#minimum number of hits per fit
	CalibrationTool.MinHits = 2000
	#maximum rate of bad hits per chamber
	CalibrationTool.MaxBadFits = 0.05
	#if set to true ommit output for chambers which are not to be fitted
	CalibrationTool.SuppressNofit = True
	#san all chambers
	RegionSelectionSvc.Region = ""
#------------------------------------for MT t0 fitter---------------------------
if CalibrationAlgorithm == "MTT0" :
	CalibNtupleAnalysisAlg.RefitSegments = False
	CalibNtupleAnalysisAlg.RefineT0 = False
	#add fitted function to histogram
	CalibrationTool.AddFitFun = True
	#create TDirectory contining some debugging graphs
	CalibrationTool.DrawDebugGraphs = True
	#range of histogram ind 25/32 bins
	CalibrationTool.NumberOfNegativeBins = 1000
	CalibrationTool.NumberOfPositiveBins = 3000
	#Fit for group of tubes
	#	TUBE 		one spectrum per tube
	#	LAYER 		one spectrum per layer
	#	MULTILAYER	one spectrum per multilayer
	#	CHAMBER		one spectrum per chamber
	#	MEZZ_CARD	one spectrum per mezzanine card
	CalibrationTool.GroupBy = "MEZZ_CARD"

#------------------------------for classic t0 fitter----------------------------
if CalibrationAlgorithm == "ClassicT0":
	CalibNtupleAnalysisAlg.RefitSegments = False
	CalibNtupleAnalysisAlg.RefineT0 = False
	#range and binning of adc spectrum
	CalibrationTool.NAdcBins = 100
	CalibrationTool.MinAdc = 0.0
	CalibrationTool.MaxAdc = 300.0
	#range and binning of the time spectrum
	CalibrationTool.NTimeBins = 1280
	CalibrationTool.TimeMin = 0.0
	CalibrationTool.TimeMax = 2000.0
	#switch on/off fitting of time spectrum
	CalibrationTool.FitTimeSpectrum = True
	#Minimum number of entries
	CalibrationTool.MinEntries = 2000
	#?
	CalibrationTool.InitParam = 1
	#maximum chi^2
	CalibrationTool.MaxChi2 = 10.0
	
#----------------------------for integration method-----------------------------
if CalibrationAlgorithm == "Integration":
	CalibNtupleAnalysisAlg.RefineT0 = False
	#mazimum drift time
	CalibrationTool.TMaxInt = 710
	#set to true if close hits are stored in the ntuple
	CalibrationTool.HaseCloseHits = False
	#the inner radius of the drift tube 
	CalibrationTool.InnerTubeRadius = 14.6

# --------------------for classical autocalibration-----------------------------
if CalibrationAlgorithm  == "ClassicAutocal" :
	#range for the number of hits
	CalibrationTool.MinimumNumberOfHits = 5
	CalibrationTool.MaximumNumberOfHits = 10
	#maximum number of iterations
	CalibrationTool.NumberOfIterations = 10
	#chi2 cuts - ?
	CalibrationTool.Chi2CutVec = [ 50, 40, 30, 20, 10, 10, 10, 5, 5, 5 ]
	CalibrationTool.Chi2Cut = 10.0


#-------------------for analytic autocalibration--------------------------------
if CalibrationAlgorithm == "AnalyticAutocal":
	#accuracy of the initial rt relation
	CalibrationTool.Accuracy = 0.5
	#type of the correction function LEGENDRE|CHEBYSHEV|POLYGON
	CalibrationTool.FunctionType = "LEGENDRE"
	#order of the correction function
	CalibrationTool.FunctionOrder = 15
	#use the full matrix - ?
	CalibrationTool.FullMatrix = True
	# fix minimum/maximum
	CalibrationTool.FixMin = True # fix r(t0)
	CalibrationTool.FixMax = False # do not fix r(tmax)
	#maximum number of iterations
	CalibrationTool.MaximumIterations = 15
	
#-----------------for simple resolution-----------------------------------------
#--select NumberOfSegments=10000
#--switch on RefineT0 and use HistFitMethod for commissioning data
if CalibrationAlgorithm == "SimpleResolution":
    CalibrationTool.CurvedFit = False #use a curved track fit (set it to true
	                                  #for data with magnetic field)
#selection criteria
    CalibrationTool.RejectLow = 0.1 #default 0.1
    CalibrationTool.RejectTop = 0.40 #default 0.30
#convergence
    CalibrationTool.DeltaConv = 0.06 #default 0.05
    CalibrationTool.DiffMax = 0.007 #default 0.006 microns
    CalibrationTool.MaximumIterations = 20 #default 20
#Replace initial spatial resoluton with flat one (=0.3mm). Used for validation.
    CalibrationTool.InitialFlatResolution = False
#Obtain resolution using histogram fitting. Usefull in case of 'noisy' residuals.
    CalibrationTool.HistFitMethod = True
#Remove hits with large residuals (outliers).
#	CalibrationTool.SuppressOutliers = True
#	CalibrationTool.SuppressionLow = 0.17
#	CalibrationTool.SuppressionTop = 0.17
    CalibrationTool.OutputResolutionFunction = "spr_out.root"	
	

#-----------------for RCP Timing------------------------------------------------
if CalibrationAlgorithm == "RpcTiming":
	CalibNtupleAnalysisAlg.RefineT0 = False
	#analysis cue - defaults to "MODULE", "DOUBLET", "STATION", "PAIRS", "MEAN", "ALLSPEC"
#	CalibrationTool.Cue = [ "MODULE" ]
	#output and input file for timing constants
	#NONE: do not read/ write file
	#AUTO: filename is rpc_timing<run_nr>.txt
	CalibrationTool.TimingInfile = "NONE"
	CalibrationTool.TimingOutfile = "AUTO"
	#automatically select all stations for this analysis
	CalibNtupleAnalysisAlg.SelectStation = "ANY"
	CalibNtupleAnalysisAlg.SelectEta = 0
	CalibNtupleAnalysisAlg.SelectPhi = -1

#-----------------for control histograms----------------------------------------
if CalibrationAlgorithm == "ControlHistograms":
	#refit segments with the given calibration
	CalibrationTool.segmentRefit = True
	#road width for segment refit in mm
	CalibrationTool.roadWidth = 1
	#store gate keys
	CalibrationTool.detectorStore = "DetectorStore"
	CalibrationTool.MDTIdHelper = "MDTIDHELPER"
	CalibrationTool.idToFixedIdToolType = "MuonCalib::IdToFixedIdTool"
	CalibrationTool.idToFixedIdToolName = "MuonCalib_IdToFixedIdTool"
	#name of the ROOT output file
	CalibrationTool.ROOTFileName = "NtupleControlHistogramsTool.root"

#-----------------for MDT chamber geometry determination------------------------
if CalibrationAlgorithm == "MDTChamberGeometryDetermination":
	#write out a ROOT control file
	CalibrationTool.writeControlHistograms = True
	#name of this ROOT file
	CalibrationTool.ROOTFileName = "NtupleMDTChamberGeometryDeterminationTool.root"
	#granularity: "TUBE", "LAYER", "MULTILAYER"
	CalibrationTool.granularity = "MULTILAYER"
	#reference multilayer (1 or 2)
	CalibrationTool.referenceMultilayer = 1
	#road width for pattern recognition in mm
	#(also used in the misalignment cuts)
	CalibrationTool.roadWidth = 2.0
	#geometry file tag for the output of the initial and final geometry
	CalibrationTool.geometryFileName = "MDTgeom"
	#store gate keys
	CalibrationTool.detectorStore = "DetectorStore"
	CalibrationTool.MDTIdHelper = "MDTIDHELPER"
	CalibrationTool.idToFixedIdToolType = "MuonCalib::IdToFixedIdTool"
	CalibrationTool.idToFixedIdToolName = "MuonCalib_IdToFixedIdTool"

#-----------------for efficiencies----------------------------------------------
if CalibrationAlgorithm == "Efficiency":
        CalibrationTool.nSegmentHits = 5
        CalibrationTool.roadWidth = 2.0
        CalibrationTool.fileName = "TubeEfficiencies"
        CalibrationTool.Debug = False
        CalibrationTool.Chi2Cut = 10
        CalibrationTool.excludeLayer = False

#-----------------for MdtDqa ----------------------------------------------
if CalibrationAlgorithm == "MdtDqa":
        #----------- jobOptions for MdtDqaNtupleAnalysis :
        CalibrationTool.MdtDqaFileName = "MdtDqa"
        CalibrationTool.Verbose = False
        CalibrationTool.FillHistos = True
        CalibrationTool.doFinalize = True
        CalibrationTool.doEfficiency = True
        #----------- jobOptions for NtupleTubeEfficiency :
        CalibrationTool.fileName = "TubeEfficiency"
        CalibrationTool.nSegmentHits = 5
        CalibrationTool.roadWidth = 2.0
        CalibrationTool.Chi2Cut = 10
        CalibrationTool.excludeLayer = False
        CalibrationTool.Debug = False

#-----------------for display---------------------------------------------------
if CalibrationAlgorithm == "Display":
        CalibrationTool.nSegmentHits = 5
        CalibrationTool.roadWidth = 2.0
        CalibrationTool.adcCut = CalibNtupleAnalysisAlg.RawdataAdcCut
        #CalibrationTool.displayEvent = 123
        #CalibrationTool.scaleDisplay = 0.5

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++### Setup Athena common flags


print CalibrationTool
print MdtCalibOutputDbSvc
print ServiceMgr
print MdtCalibInputSvc
print AlgSequence()
from ROOT import gROOT, TCanvas, TF1

