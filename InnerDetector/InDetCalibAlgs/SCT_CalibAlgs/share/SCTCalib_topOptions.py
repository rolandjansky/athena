#--------------------------------------------------------------
# topJobOptions to run SCTCalib/SCTCalibWriteSvc
# - Default setup is defined in this jobOs.
# - User specific setup to run each algorithm can be
#   configured in SCTCalibConfig.py
#--------------------------------------------------------------

import os,sys,time,glob,fnmatch

#--------------------------------------------------------------
# User setup to run each algorithm
# - SCTCalibConfig.py is used as input to the following
#   flags/properties
#--------------------------------------------------------------
include( "SCTCalibConfig.py" )

#--------------------------------------------------------------
# - "athena_checkUpload.py"" is used to get info for the runs
#    already uploaded in COOL
#--------------------------------------------------------------
# import athena_checkUpload
#from athena_checkUpload import *

#--------------------------------------------------------------
# Basic setup
#--------------------------------------------------------------
#--- basic job configuration
import AthenaCommon.AtlasUnixStandardJob

#--- McEventSelector
if not ReadBS :
    import AthenaCommon.AtlasUnixGeneratorJob

#--- get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#--- get a handle to the ServiceManager & ApplicationManager
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import theApp

#--------------------------------------------------------------
# AuditorSvc
#--------------------------------------------------------------
from GaudiSvc.GaudiSvcConf import AuditorSvc
ServiceMgr += AuditorSvc()
theAuditorSvc = ServiceMgr.AuditorSvc
#--- write out a summary of the time spent
theAuditorSvc.Auditors += [ "ChronoAuditor" ]
#--- write out a summary of the memory usage
theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
#--- write out a short message upon entering or leaving each algorithm
#theAuditorSvc.Auditors += [ "NameAuditor" ]
   
theApp.AuditAlgorithms = True
theApp.AuditServices   = True

#--------------------------------------------------------------
# Load AthenaSealSvc
#--------------------------------------------------------------
ServiceMgr.AthenaSealSvc.CheckDictionary = True

#--------------------------------------------------------------
# Setup geometry and conditions tag
# - Chosen from magnet current info
#--------------------------------------------------------------
#--- read magnet current from COOL
if DataSource == 'data' :
    solenoidCurrent = 0.0
    toroidCurrent   = 0.0
    from CoolConvUtilities.MagFieldUtils import getFieldForRun
    magField = getFieldForRun( RunNumber )
    if ( magField is not None ):
        solenoidCurrent = magField.solenoidCurrent()
        toroidCurrent   = magField.toroidCurrent()
    else:
        print "Magnetic field info not available"

    #--- set geometry from field info : both fields ON in default
    if GeometryTag == '' :
        if solenoidCurrent > 100. and toroidCurrent > 100. : 
            GeometryTag = GeometryTagSTF  # Solenoide=ON,  BarrelTroid=ON
        elif solenoidCurrent > 100. and not toroidCurrent > 100. :
            GeometryTag = GeometryTagNTF  # Solenoide=ON,  BarrelTroid=OFF
        elif not solenoidCurrent > 100. and toroidCurrent > 100. :
            GeometryTag = GeometryTagNSF  # Solenoide=OFF, BarrelTroid=ON
        elif not solenoidCurrent > 100. and not toroidCurrent > 100. :
            GeometryTag = GeometryTagNF   # Solenoide=OFF, BarrelTroid=OFF

    #--- set geometry from field info : both fields ON in default
    if ConditionsTag == '' :
        if solenoidCurrent > 100. and toroidCurrent > 100. : 
            ConditionsTag = ConditionsTagSTF
        elif solenoidCurrent > 100. and not toroidCurrent > 100. :
            ConditionsTag = ConditionsTagNTF
        elif not solenoidCurrent > 100. and toroidCurrent > 100. :
            ConditionsTag = ConditionsTagNSF
        elif not solenoidCurrent > 100. and not toroidCurrent > 100. :
            ConditionsTag = ConditionsTagNF

#--- Default in MC09
if DataSource == 'geant4' :
    GeometryTag   = GeometryTagMC
    ConditionsTag = ConditionsTagMC

#--------------------------------------------------------------
# Default setup for specific flags in SCTCalib
#--------------------------------------------------------------
#--- set flag for algs which use HIST
DoHIST = DoNoiseOccupancy or DoRawOccupancy or DoEfficiency or DoBSErrorDB or DoLorentzAngle

#--- set flag for DCSConditionsSvc
if DoHV and DataSource == 'data' :
    UseDCS = True
#--- set flag for ConfigurationConditionsSvc
if DoHV or DoHIST :
    UseConfiguration = False
#--- set flag for ReadCalibDataSvc
if DataSource == 'geant4' :
    UseCalibration = False
if not DoNoisyStrip :
    UseCalibration = False
#--- set flag for MajorityConditionsSvc
if not DoNoisyStrip :
    UseMajority = False
#--- set flag for ByteStreamErrorsSvc
if DoDeadStrip or DoDeadChip :
    UseBSError = True

#--- set flag for creating hitmaps
if DoHV or DoHIST :
    DoHitMaps = False
#--- set flag for creating BSErrors map
if DoDeadStrip or DoDeadChip :
    DoBSErrors = True

#--- overwrite EvtMax to 1 only when reading HIST
if DoHIST :
    EvtMax = 1

#--------------------------------------------------------------
# Read start/end time stamp and LB for HIST
#--------------------------------------------------------------
ProjTag   = ''
SORTime   = ''
EORTime   = ''
nLB       = ''
DAQConfig = ''

if os.path.exists("./runInfo.txt") :
    runInfo = open( 'runInfo.txt', 'r' ).read()
    list  = runInfo.split( ' ' )
    ProjTag   = list[1]
    SORTime   = list[2]
    EORTime   = list[3]
    nLB       = list[4]
    DAQConfig = list[5]
else :
    ProjTag   = 'cannot retrieve ProjTag'
    SORTime   = 'cannot retrieve SORTime'
    EORTime   = 'cannot retrieve EORTime'
    nLB       = 'cannot retrieve LB'
    DAQConfig = 'cannot retrieve DAQConfig'


#--------------------------------------------------------------
# Read /SCT/Derived/Monotoring in COOL
# - Used in a criteria for upload of NoisyStrips
#--------------------------------------------------------------
if DoNoisyStrip and NoisyUploadTest :
    include( "ReadCoolUPD4.py" )
    
    tag      = TagID4NoisyUploadTest
    numRuns  = NoisyReadNumRuns
    
    dbstring = 'ATLAS_COOLOFL_SCT/CONDBR2'
    folder   = '/SCT/Derived/Monitoring'
    RunList  = GetRunList( dbstring, folder, tag, RunNumber, numRuns )
    if ( len(RunList) != 0 ) :
        #--- List of data and average num of modules w/ >= 1 noisy strip
        print "---------------> Noisy strips in COOL : last ", numRuns, " runs <---------------"
        sumNoisyModulesInDB = 0
        sumNoisyStripsInDB = 0
        for i in range( len(RunList) ) :
            numNoisyModules = GetNumNoisyMods( dbstring, folder, tag, RunList[i] )
            numNoisyStrips  = GetNumNoisyStrips( dbstring, folder, tag, RunList[i] )
            print "[ run, modules, strips ] = [", RunList[i], ",", numNoisyModules, ",", numNoisyStrips, "]"
            sumNoisyModulesInDB = sumNoisyModulesInDB + numNoisyModules
            sumNoisyStripsInDB = sumNoisyStripsInDB + numNoisyStrips
        NoisyModuleAverageInDB = float(sumNoisyModulesInDB) / float(len(RunList))
        NoisyStripAverageInDB = float(sumNoisyStripsInDB) / float(len(RunList))
        #--- Num of noisy strips in the last run
        NoisyStripLastRunInDB = GetNumNoisyStrips( dbstring, folder, tag, RunList[0] )
        print "Average num of modules w/ >= 1 noisy strip  : ",         NoisyModuleAverageInDB
        print "Num of noisy strips in the last run", RunList[0], " : ", NoisyStripLastRunInDB
        print "Average num of noisy strips in last runs", RunList[0], " : ", NoisyStripAverageInDB
        print "----------------------------------------------------------------------"
        

#--------------------------------------------------------------
# Print out summay of user setting
#--------------------------------------------------------------
print "------ Begin : User setting for SCTCalib/SCTCalibWriteSvc ------------"
print "- RunNumber                              : %s" %( RunNumber )
print "- Number of events                       : %s" %( EvtMax )
print "- Global flag for Geometry               : %s" %( GeometryTag )
print "- Global flag for ConditionsDB           : %s" %( ConditionsTag )
print "- Global flag for DataSource             : %s" %( DataSource )
print "- Beam flag                              : %s" %( beamType )
print "- Flag to read BS                        : %s" %( ReadBS )
print "- Flag to run NoisyStrip                 : %s" %( DoNoisyStrip )
print "- Flag to run NoisyLB                    : %s" %( DoNoisyLB )
print "- Flag to run HVTrip                     : %s" %( DoHV )
print "- Flag to run DeadStrip                  : %s" %( DoDeadStrip )
print "- Flag to run DeadChip                   : %s" %( DoDeadChip )
print "- Flag to run NoiseOccupancy             : %s" %( DoNoiseOccupancy )
print "- Flag to run LorentzAngle               : %s" %( DoLorentzAngle ) #Roger
print "- Flag to run RawOccupancy               : %s" %( DoRawOccupancy )
print "- Flag to run Efficiency                 : %s" %( DoEfficiency )
print "- Flag to run BSErrors for DB            : %s" %( DoBSErrorDB )
print "- Flag to write local DB                 : %s" %( WriteToCool )
print "- Flag to use DCSCondisionsSvc           : %s" %( UseDCS )
print "- Flag to use ConfigurationCondisionsSvc : %s" %( UseConfiguration )
print "- Flag to use ReadCalibDataSvc           : %s" %( UseCalibration )
print "- Flag to use MajorityCondisionsSvc      : %s" %( UseMajority )
print "- Flag to use ByteStreamErrorsSvc        : %s" %( UseBSError )
print "- Tag for NoisyStrip                     : %s" %( TagID4NoisyStrips )
print "- Tag for DeadStrip                      : %s" %( TagID4DeadStrips )
print "- Tag for DeadChip                       : %s" %( TagID4DeadChips )
print "- Tag for NoiseOccupancy                 : %s" %( TagID4NoiseOccupancy )
print "- Tag for RawOccupancy                   : %s" %( TagID4RawOccupancy )
print "- Tag for Efficiency                     : %s" %( TagID4Efficiency )
print "- Tag for BSErrorDB                      : %s" %( TagID4BSErrors )
print "------ End : User setting for SCTCalib/SCTCalibWriteSvc --------------"

#--------------------------------------------------------------
# Load Global Flags and set defaults
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
#--- default is atlas geometry
globalflags.DetGeo = 'atlas'
#--- set defaults
globalflags.DataSource    = DataSource
globalflags.ConditionsTag = ConditionsTag
if ReadBS :
    globalflags.InputFormat = 'bytestream'
else :
    globalflags.InputFormat = 'pool'
#--- set geometry version
globalflags.DetDescrVersion = GeometryTag
#--- printout
globalflags.print_JobProperties()

#--- beam flag : default is zero luminosity
from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.numberOfCollisions = 0.0
jobproperties.Beam.beamType           = beamType

#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------
from AthenaCommon.DetFlags import DetFlags
#--- switch on SCT only
DetFlags.all_setOff()
DetFlags.SCT_setOn()
#--- for the conditions access
DetFlags.haveRIO.SCT_on()
#--- for the cabling
DetFlags.detdescr.SCT_on()
#--- printout
DetFlags.Print()

#set variable projectName
#we need it to extract the year from the run
#else execution fails
from RecExConfig.RecFlags import rec
filelist=runArgs.inputNames
n=filelist[0].count('/')
filename=filelist[0].split('/')[n]
projectName=str(filename.split('.')[0])
rec.__dict__.get('projectName').set_Value(projectName)

#--------------------------------------------------------------
# GeoModel & MagneticFieldSvc
#--------------------------------------------------------------
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
import MagFieldServices.SetupField

#--------------------------------------------------------------
# Configuring the cabling
#--------------------------------------------------------------
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doPrintConfigurables = True
include( "InDetRecExample/InDetRecCabling.py" )

#--------------------------------------------------------------
# Setup for reading Bytestream
#--------------------------------------------------------------
if ReadBS :
    DetFlags.readRDOBS.SCT_on()
    include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
    include( "InDetRecExample/InDetReadBS_jobOptions.py" )

#--------------------------------------------------------------
# Configuring the conditions access
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
InDetFlags.useDCS = UseDCS #True if run HVTrip search
include( "InDetRecExample/InDetRecConditionsAccess.py" )

#--- for MajorityConditionsSvc
# use new CONDBR2, A.N., 2014-11-30
year=int(projectName[4:6])
if (year > 13):
    conddb.addFolder('',"<db>COOLOFL_DCS/CONDBR2</db> /SCT/DCS/MAJ", className="CondAttrListCollection")
else:
    conddb.addFolder('',"<db>COOLOFL_DCS/COMP200</db> /SCT/DCS/MAJ", className="CondAttrListCollection")

#--- For Conditions algorithm for Athena MT (start)
from IOVSvc.IOVSvcConf import CondSvc
ServiceMgr += CondSvc()
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")
from IOVSvc.IOVSvcConf import CondInputLoader
condSeq += CondInputLoader("CondInputLoader")
import StoreGate.StoreGateConf as StoreGateConf
ServiceMgr += StoreGateConf.StoreGateSvc("ConditionStore")
from  SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_MajorityCondAlg
condSeq += SCT_MajorityCondAlg("SCT_MajorityCondAlg")
#--- For Conditions algorithm for Athena MT (end)

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_MajorityConditionsSvc
InDetSCT_MajorityConditionsSvc = SCT_MajorityConditionsSvc( name = "InDetSCT_MajorityConditionsSvc" )
InDetSCT_MajorityConditionsSvc.UseOverall       = True
InDetSCT_MajorityConditionsSvc.MajorityFraction = 0.9
ServiceMgr += InDetSCT_ConfigurationConditionsSvc
if ( InDetFlags.doPrintConfigurables() ):
    print InDetSCT_MajorityConditionsSvc

#--- conditions tag
conddb.setGlobalTag( globalflags.ConditionsTag() ) 
#IOVDbSvc.CacheRun = M    # can also just do M runs at a time. 
#IOVDbSvc.CacheTime = 3600 # in seconds

#--------------------------------------------------------------
# Saving ROOT histograms
#--------------------------------------------------------------
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
if DoHitMaps : 
    ServiceMgr.THistSvc.Output += [ "HitMaps  DATAFILE='SCTHitMaps.root'  OPT='RECREATE'" ]
if DoNoisyLB :    
    ServiceMgr.THistSvc.Output += [ "LB       DATAFILE='SCTLB.root'       OPT='RECREATE'" ]
if DoBSErrors :    
    ServiceMgr.THistSvc.Output += [ "BSErrors DATAFILE='SCTBSErrors.root' OPT='RECREATE'" ]

#--------------------------------------------------------------
# Configuring SCTCalibWriteSvc
#--------------------------------------------------------------
from SCT_CalibAlgs.SCT_CalibAlgsConf import SCTCalibWriteSvc
ServiceMgr += SCTCalibWriteSvc()

SCTCalibWrite = ServiceMgr.SCTCalibWriteSvc

SCTCalibWrite.BeginRun = int( RunNumber )
if DoNoisyStrip and NoisyUpdate :
    SCTCalibWrite.EndRun   = -1   
else :
    SCTCalibWrite.EndRun   = int( RunNumber ) 

SCTCalibWrite.StreamName = "SCTCalibStream"

SCTCalibWrite.TagID4NoisyStrips    = TagID4NoisyStrips
SCTCalibWrite.TagID4DeadStrips     = TagID4DeadStrips
SCTCalibWrite.TagID4DeadChips      = TagID4DeadChips
SCTCalibWrite.TagID4NoiseOccupancy = TagID4NoiseOccupancy
SCTCalibWrite.TagID4RawOccupancy   = TagID4RawOccupancy
SCTCalibWrite.TagID4Efficiency     = TagID4Efficiency
SCTCalibWrite.TagID4BSErrors       = TagID4BSErrors
SCTCalibWrite.TagID4LorentzAngle   = TagID4LorentzAngle #Roger
SCTCalibWrite.OutputLevel = INFO

print SCTCalibWrite

#--------------------------------------------------------------
# Setup for writing local COOL DB
#--------------------------------------------------------------
from AthenaCommon.AppMgr import ToolSvc
from AthenaServices.AthenaServicesConf import AthenaOutputStreamTool
SCTCalibStream = AthenaOutputStreamTool( "SCTCalibStream", OutputFile = "sctcalib.pool.root" )
ToolSvc += SCTCalibStream

#--------------------------------------------------------------
# Configuring SCTCalib
#--------------------------------------------------------------
from SCT_CalibAlgs.SCT_CalibAlgsConf import SCTCalib
topSequence += SCTCalib()

SCTCalib = topSequence.SCTCalib

#--- Run number
SCTCalib.RunNumber = RunNumber
#--- Run stat/end time read from runInfo.txt
SCTCalib.RunStartTime = SORTime
SCTCalib.RunEndTime   = EORTime
SCTCalib.LBMax        = nLB
#--- Flag for ConditionsSvc, sorting, hitmaps, BSErros
SCTCalib.UseDCS           = UseDCS           # False in default
SCTCalib.UseConfiguration = UseConfiguration # True  in default
SCTCalib.UseCalibration   = UseCalibration   # True  in default
SCTCalib.UseMajority      = UseMajority      # True  in default
SCTCalib.UseBSError       = UseBSError       # False in default
SCTCalib.DoHitMaps        = DoHitMaps        # True  in default
SCTCalib.DoBSErrors       = DoBSErrors       # True  in default
#--- Flags for input files
SCTCalib.ReadBS         = ReadBS
#--- Input files
if DoNoisyStrip or DoHV or DoDeadStrip or DoDeadChip :
    if ReadBS :
        include( "input_BS.py" )
    else :
        include( "input_NTUP.py" )
if DoNoiseOccupancy or DoRawOccupancy or DoEfficiency or DoBSErrorDB or DoLorentzAngle: 
    include( "input_HIST.py" )
#--- Methods to run
SCTCalib.DoNoisyStrip     = DoNoisyStrip      # True  in default
SCTCalib.DoNoisyLB        = DoNoisyLB         # True  in default
SCTCalib.DoHV             = DoHV              # False in default
SCTCalib.DoDeadStrip      = DoDeadStrip       # False in default
SCTCalib.DoDeadChip       = DoDeadChip        # False in default
SCTCalib.DoNoiseOccupancy = DoNoiseOccupancy  # False in default
SCTCalib.DoRawOccupancy   = DoRawOccupancy    # False in default
SCTCalib.DoEfficiency     = DoEfficiency      # False in default
SCTCalib.DoBSErrorDB      = DoBSErrorDB       # False in default
SCTCalib.DoLorentzAngle   = DoLorentzAngle    # False in default 
#--- Local DB
SCTCalib.WriteToCool = WriteToCool # True in default
#--- Properties for noisy strips
SCTCalib.NoisyMinStat           = NoisyMinStat
SCTCalib.NoisyStripAll          = NoisyStripAll
SCTCalib.NoisyStripThrDef       = NoisyStripThrDef
SCTCalib.NoisyStripThrOffline   = NoisyStripThrOffline
SCTCalib.NoisyStripThrOnline    = NoisyStripThrOnline
SCTCalib.NoisyWaferFinder       = NoisyWaferFinder
SCTCalib.NoisyWaferWrite        = NoisyWaferWrite
SCTCalib.NoisyWaferAllStrips    = NoisyWaferAllStrips
SCTCalib.NoisyWaferThrBarrel    = NoisyWaferThrBarrel
SCTCalib.NoisyWaferThrECA       = NoisyWaferThrECA
SCTCalib.NoisyWaferThrECC       = NoisyWaferThrECC
SCTCalib.NoisyWaferFraction     = NoisyWaferFraction
SCTCalib.NoisyChipFraction      = NoisyChipFraction
#added agrohsje
SCTCalib.NoisyUploadTest        = NoisyUploadTest
SCTCalib.NoisyModuleAverageInDB = NoisyModuleAverageInDB
SCTCalib.NoisyStripLastRunInDB  = NoisyStripLastRunInDB
SCTCalib.NoisyStripAverageInDB  = NoisyStripAverageInDB

SCTCalib.NoisyModuleList        = NoisyModuleList
SCTCalib.NoisyModuleDiff        = NoisyModuleDiff
SCTCalib.NoisyStripDiff         = NoisyStripDiff 
#--- Properties for HV check
SCTCalib.HVQlength     = HVQlength
SCTCalib.OutputLowHits = OutputLowHits
SCTCalib.LowHitsCut    = LowHitsCut
#--- Properties for dead strips/chips
SCTCalib.DeadStripMinStat      = DeadStripMinStat
SCTCalib.DeadStripMinStatBusy  = DeadStripMinStatBusy
SCTCalib.DeadChipMinStat       = DeadChipMinStat
SCTCalib.DeadStripSignificance = DeadStripSignificance
SCTCalib.DeadChipSignificance  = DeadChipSignificance
SCTCalib.BusyThr4DeadFinding   = BusyThr4DeadFinding
SCTCalib.NoisyThr4DeadFinding  = NoisyThr4DeadFinding
SCTCalib.DeadChipUploadTest    = DeadChipUploadTest
SCTCalib.DeadStripUploadTest   = DeadStripUploadTest
#--- Properties for HIST
SCTCalib.NoiseOccupancyTriggerAware = NoiseOccupancyTriggerAware
SCTCalib.NoiseOccupancyMinStat      = NoiseOccupancyMinStat
SCTCalib.RawOccupancyMinStat        = RawOccupancyMinStat
SCTCalib.EfficiencyMinStat          = EfficiencyMinStat
SCTCalib.BSErrorDBMinStat           = BSErrorDBMinStat

#Lorentz Angle Properties
SCTCalib.LorentzAngleMinStat        = LorentzAngleMinStat
SCTCalib.LorentzAngleDebugMode        = LorentzAngleDebugMode

#--- Tags for XMLs : have to be consistent with SCTCalibWriteSvc
SCTCalib.TagID4NoisyStrips    = TagID4NoisyStrips
SCTCalib.TagID4DeadStrips     = TagID4DeadStrips
SCTCalib.TagID4DeadChips      = TagID4DeadChips
SCTCalib.TagID4NoiseOccupancy = TagID4NoiseOccupancy
SCTCalib.TagID4RawOccupancy   = TagID4RawOccupancy
SCTCalib.TagID4Efficiency     = TagID4Efficiency
SCTCalib.TagID4BSErrors       = TagID4BSErrors
SCTCalib.TagID4LorentzAngle   = TagID4LorentzAngle 
#--- Output XMLs
SCTCalib.BadStripsAllFile          = 'BadStripsAllFile.xml'          # All NoisyStrips
SCTCalib.BadStripsNewFile          = 'BadStripsNewFile.xml'          # Newly found NoisyStrips
SCTCalib.BadStripsSummaryFile      = 'BadStripsSummaryFile.xml'      # Summary of NoisyStrips
SCTCalib.BadModulesFile            = 'BadModulesFile.xml'            # HVTrip
SCTCalib.DeadStripsFile            = 'DeadStripsFile.xml'            # DeadStrip
SCTCalib.DeadChipsFile             = 'DeadChipsFile.xml'             # DeadChip
SCTCalib.DeadSummaryFile           = 'DeadSummaryFile.xml'           # Summary of Dead Search
SCTCalib.NoiseOccupancyFile        = 'NoiseOccupancyFile.xml'        # NoiseOccupancy
SCTCalib.NoiseOccupancySummaryFile = 'NoiseOccupancySummaryFile.xml' # Summary of NoiseOccupancy
SCTCalib.RawOccupancySummaryFile   = 'RawOccupancySummaryFile.xml'   # Summary of RawOccupancy
SCTCalib.EfficiencyModuleFile      = 'EfficiencyModuleSummary.xml'   # Efficiency
SCTCalib.EfficiencySummaryFile     = 'EfficiencySummaryFile.xml'     # Summary of Efficiency
SCTCalib.BSErrorSummaryFile        = 'BSErrorSummaryFile.xml'        # Summary of BS Errors
SCTCalib.BSErrorModuleFile         = 'BSErrorModuleSummary.xml'        # Summary of BS Errors

SCTCalib.LorentzAngleFile          = 'LorentzAngleFile.xml'          # Summary of LorentzAngle ------- Roger

#SCTCalib.NoisyModuleAverageInDB    = NoisyModuleAverageInDB  #noisyModulesAverageInDB
#SCTCalib.NoisyStripLastRunInDB     = NoisyStripLastRunInDB   #noisyStripsInDB_lastRun

SCTCalib.OutputLevel     = INFO # DEBUG
SCTCalib.AuditAlgorithms = True # False
SCTCalib.AuditBeginRun   = True # False
SCTCalib.AuditEndRun     = True # False

print SCTCalib

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
ServiceMgr.EventSelector.RunNumber = RunNumber
theApp.EvtMax                      = EvtMax

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.infoLimit   = 1000000
