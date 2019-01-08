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
import CheckDb

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
HistBefore2010=False
print "- Flag to read HIST before 2010          : %s" %( HistBefore2010 )
print "- Flag to run NoisyStrip                 : %s" %( DoNoisyStrip )
print "- Flag to run NoisyLB                    : %s" %( DoNoisyLB )
print "- Flag to run HVTrip                     : %s" %( DoHV )
print "- Flag to run DeadStrip                  : %s" %( DoDeadStrip )
print "- Flag to run DeadChip                   : %s" %( DoDeadChip )
print "- Flag to run NoiseOccupancy             : %s" %( DoNoiseOccupancy )
print "- Flag to run RawOccupancy               : %s" %( DoRawOccupancy )
print "- Flag to run Efficiency                 : %s" %( DoEfficiency )
print "- Flag to run BSErrors for DB            : %s" %( DoBSErrorDB )
print "- Flag to run LorentzAngle               : %s" %( DoLorentzAngle )
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
print "- Tag for LorentzAngle                   : %s" %( TagID4LorentzAngle )
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
include( "BFieldAth/BFieldAth_jobOptions.py" )

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

#--- for MajorityConditionsTool
from  SCT_ConditionsTools.SCT_MajorityConditionsToolSetup import SCT_MajorityConditionsToolSetup
sct_MajorityConditionsToolSetup = SCT_MajorityConditionsToolSetup()
year=int(projectName[4:6])
if (year > 13):
    sct_MajorityConditionsToolSetup.setFolderDb("<db>COOLOFL_DCS/CONDBR2</db> /SCT/DCS/MAJ")
else:
    sct_MajorityConditionsToolSetup.setFolderDb("<db>COOLOFL_DCS/COMP200</db> /SCT/DCS/MAJ")
sct_MajorityConditionsToolSetup.setFolder("/SCT/DCS/MAJ")
sct_MajorityConditionsToolSetup.setup()
InDetSCT_MajorityConditionsTool = sct_MajorityConditionsToolSetup.getTool()
InDetSCT_MajorityConditionsTool.UseOverall       = True
InDetSCT_MajorityConditionsTool.MajorityFraction = 0.9
if ( InDetFlags.doPrintConfigurables() ):
    print InDetSCT_MajorityConditionsTool

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
SCTCalibWrite.EndRun   = int( RunNumber )

SCTCalibWrite.StreamName = "SCTCalibStream"

SCTCalibWrite.TagID4NoisyStrips    = TagID4NoisyStrips
SCTCalibWrite.TagID4DeadStrips     = TagID4DeadStrips
SCTCalibWrite.TagID4DeadChips      = TagID4DeadChips
SCTCalibWrite.TagID4NoiseOccupancy = TagID4NoiseOccupancy
SCTCalibWrite.TagID4RawOccupancy   = TagID4RawOccupancy
SCTCalibWrite.TagID4Efficiency     = TagID4Efficiency
SCTCalibWrite.TagID4BSErrors       = TagID4BSErrors
SCTCalibWrite.TagID4LorentzAngle   = TagID4LorentzAngle

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
SCTCalib.UseConfiguration = UseConfiguration # True  in default
SCTCalib.UseCalibration   = UseCalibration   # True  in default
SCTCalib.UseMajority      = UseMajority      # True  in default
SCTCalib.UseBSError       = UseBSError       # False in default
SCTCalib.DoHitMaps        = DoHitMaps        # True  in default
SCTCalib.DoBSErrors       = DoBSErrors       # True  in default
#--- Flags for input files
SCTCalib.ReadBS         = ReadBS
#SCTCalib.HistBefore2010 = HistBefore2010
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
SCTCalib.NoisyMinStat         = NoisyMinStat
SCTCalib.NoisyStripAll        = NoisyStripAll
SCTCalib.NoisyStripThrDef     = NoisyStripThrDef
SCTCalib.NoisyStripThrOffline = NoisyStripThrOffline
SCTCalib.NoisyStripThrOnline  = NoisyStripThrOnline
SCTCalib.NoisyWaferFinder     = NoisyWaferFinder
SCTCalib.NoisyWaferWrite      = NoisyWaferWrite
SCTCalib.NoisyWaferAllStrips  = NoisyWaferAllStrips
SCTCalib.NoisyWaferThrBarrel  = NoisyWaferThrBarrel
SCTCalib.NoisyWaferThrECA     = NoisyWaferThrECA
SCTCalib.NoisyWaferThrECC     = NoisyWaferThrECC
SCTCalib.NoisyWaferFraction   = NoisyWaferFraction
SCTCalib.NoisyChipFraction    = NoisyChipFraction
#--- Properties for dead strips/chips
SCTCalib.DeadStripMinStat      = DeadStripMinStat
SCTCalib.DeadStripMinStatBusy  = DeadStripMinStatBusy
SCTCalib.DeadChipMinStat       = DeadChipMinStat
SCTCalib.DeadStripSignificance = DeadStripSignificance
SCTCalib.DeadChipSignificance  = DeadChipSignificance
SCTCalib.BusyThr4DeadFinding   = BusyThr4DeadFinding
SCTCalib.NoisyThr4DeadFinding  = NoisyThr4DeadFinding
#--- Properties for HIST
SCTCalib.NoiseOccupancyMinStat = NoiseOccupancyMinStat
SCTCalib.RawOccupancyMinStat   = RawOccupancyMinStat
SCTCalib.EfficiencyMinStat     = EfficiencyMinStat
SCTCalib.BSErrorDBMinStat      = BSErrorDBMinStat
SCTCalib.LorentzAngleMinStat   = LorentzAngleMinStat
#--- Tags for XMLs : have to be consistent with SCTCalibWriteSvc
SCTCalib.TagID4NoisyStrips    = TagID4NoisyStrips
SCTCalib.TagID4DeadStrips     = TagID4DeadStrips
SCTCalib.TagID4DeadChips      = TagID4DeadChips
SCTCalib.TagID4NoiseOccupancy = TagID4NoiseOccupancy
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
SCTCalib.EfficiencySummaryFile     = 'EfficiencySummaryFile.xml'     # Summary of Efficiency
SCTCalib.BSErrorSummaryFile        = 'BSErrorSummaryFile.xml'        # Summary of BS Errors
SCTCalib.LorentzAngleFile          = 'LorentzAngleFile.xml'          # LorentzAngle
SCTCalib.LorentzAngleSummaryFile   = 'LorentzAngleSummaryFile.xml'   # Summary of LorentzAngle

#which db to check
Server='ATLAS_COOLPROD'
Schema='ATLAS_COOLOFL_SCT'
Dbname='COMP200'
Folder='/SCT/Derived/Monitoring'
#Tag='SctDerivedMonitoring-003-00'
Tag='SctDerivedMonitoring-UPD1-000'
#
RunsInDBTotal=[167661]
RunsInDB=CheckDb.RunListInDB(RunsInDBTotal, Server,Schema,Dbname,Folder,Tag,'*')
print "Runs in DB: %s"%RunsInDB
numNoisyModulesInDB=CheckDb.GetNumberOfNoisyModulesInDB(RunsInDB, Server,Schema,Dbname,Folder,Tag,'*')
noisyModulesAverageInDB=CheckDb.GetNoisyModulesAverageInDB(numNoisyModulesInDB)
noisyStripsInDB=CheckDb.GetNumberOfNoisyStripsInDB(RunsInDB, Server,Schema,Dbname,Folder,Tag,'*')
noisyStripsInDB_lastRun=0
if noisyStripsInDB:
 noisyStripsInDB_lastRun=noisyStripsInDB[0]
nNoisyStrips=0
if RunsInDB:
 nNoisyStrips=RunsInDB[0]

SCTCalib.AverageNumNoisyModsInDB   = noisyModulesAverageInDB
SCTCalib.NumNoisyStripsPrevRunInDB = noisyStripsInDB_lastRun

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
#ServiceMgr.MessageSvc.debugLimit  = 10000
#ServiceMgr.MessageSvc.alwaysLimit = 10000
