#======================================================================
# TOP JobOptions to run on Muons Segm Validation
# Nektarios Chr. Benekos (UIUC) - Ilektra Christidi
#======================================================================
#-------------------------------
# Setup Athena common flags
#--------------------------------
from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags

# import AlgSequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# import ToolSvc
from AthenaCommon.AppMgr import ToolSvc

# import theApp
from AthenaCommon.AppMgr import theApp

#import ServiceMgr
from AthenaCommon.AppMgr import ServiceMgr  
svcMgr = theApp.serviceMgr()

#--------------------------------------------------------------
# Load POOL support - to read conditions in POOL, not events
#--------------------------------------------------------------
import AthenaPoolCnvSvc.AthenaPool
#--------------------------------------------------------------
# Access to IOVSvc, IOVDbSvc and CondDBMySQLCnvSvc
#--------------------------------------------------------------
import IOVDbSvc.IOVDb

from AthenaCommon import CfgMgr
from RecExConfig.RecFlags import rec as recFlags

###############################################################
#
#  Configuration jobOptions file
#
###############################################################
from RecExConfig.RecFlags import rec as recFlags
#--------------------------------------------------------------------------------
# Setup Internal flags for the Muon Spectrometer Commissioning (defaults values)
#--------------------------------------------------------------------------------
from MuonCommRecExample.MuonCommRecFlags import MuonCommRecFlags as MuDetCosmicFlags 
from MuonRecExample.MuonRecFlags import muonRecFlags
from MuonRecExample import ConfiguredMuonRec
ConfiguredMuonRec.defaultPackage = 'MuonCommRecExample'

# initial flags for calibration constants loading and PRDs
MuDetCosmicFlags.doM4 = False
MuDetCosmicFlags.doM5 = True
MuDetCosmicFlags.doP2 = False
# input 
MuDetCosmicFlags.doSim   = False
#temporary fix
MuDetCosmicFlags.doMDT_TBCabling=True 

#-----------------Results as vector of histos------------------------------------
MuDetCosmicFlags.doMuonboy=True
MuDetCosmicFlags.doMoore=True
#--------------------------------------------------------------------------------
# ----Write ESD
MuDetCosmicFlags.doESD = False
###############################################################
#-----------------------------------------------------------------
# Setup Internal flags for the Muon DQA Monitoring (defaults values)
#----------------------------------------------------------------- 
from MuonDQAMonitoring.MuonDQAFlags import MuonDQAFlags as MuonDQAFlags

####CPU and txt files
MuonDQAFlags.doCpuMuonMon = True
MuonDQAFlags.doMuonDQAPerfMonitoring = True
MuonDQAFlags.doAuditor = True
MuonDQAFlags.doFinalCPU = False  
MuonDQAFlags.doDumpFile = False
MuonDQAFlags.doSimpleFile = True

##MuonSegm Monitoring options
MuonDQAFlags.doMuonMonitoring = True
MuonDQAFlags.doMuonSegmMon = True

##Event Display 
MuonDQAFlags.doVP1=False 

#-----------------------------------------------------------------  
# setup Mx/Px data
#-----------------------------------------------------------------  
if MuDetCosmicFlags.doM4:
    ###FOR M4 --> list of good M4 data-->https://twiki.cern.ch/twiki/bin/view/Atlas/M4OnlineRunList
    MuonDQAFlags.RunNumber = 20304
    inDir='/castor/cern.ch/grid/atlas/t0/perm/DAQ/'
    MuonDQAFlags.InputDir = inDir
    datafiles='nsls ' +str(MuonDQAFlags.InputDir)+' | grep 00'+str(MuonDQAFlags.RunNumber)+'.physics.cosmics.'
    sampletype='daq.m4_combined.'+str(MuonDQAFlags.RunNumber)+'.physics.cosmics'  

if MuDetCosmicFlags.doM5:
    ###FOR M5 --> list of good M5 data-->https://twiki.cern.ch/twiki/bin/view/Atlas/M5MuonRunList
    MuonDQAFlags.RunNumber = 29548 #28940
    inDir='/castor/cern.ch/grid/atlas/t0/perm/DAQ/M5'
    MuonDQAFlags.InputDir = inDir
    datafiles='nsls ' +str(MuonDQAFlags.InputDir)+' | grep daq.m5_combined.00'+str(MuonDQAFlags.RunNumber)
    sampletype='daq.m5_combined.'+str(MuonDQAFlags.RunNumber)+'.physics.cosmics'  
                
if MuDetCosmicFlags.doP2:
    ###FOR P2 --> list of good P2 data-->https://twiki.cern.ch/twiki/bin/view/Atlas/P2MuonRunList
    MuonDQAFlags.RunNumber = 33623 #33842 # 34267
    inDir='/castor/cern.ch/grid/atlas/t0/perm/DAQ/MuonWeekDec07'
    MuonDQAFlags.InputDir = inDir
    datafiles='nsls ' +str(MuonDQAFlags.InputDir)+' | grep daq.muon_combined.00'+str(MuonDQAFlags.RunNumber)
    sampletype='daq.P2_combined.'+str(MuonDQAFlags.RunNumber)+'.physics.cosmics'  


###############################################################   
# ------------------------------------
# import GlobalFlags and DetFlags
# ------------------------------------
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_commis()
GlobalFlags.Print()

from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.Muon_setOn()
DetFlags.detdescr.ID_setOff()
DetFlags.detdescr.LAr_setOff()
DetFlags.detdescr.Tile_setOff()
DetFlags.Print()
   
# -----------------------------------------
# Set Data Inputs (real data or simulation)
# -----------------------------------------
if MuDetCosmicFlags.doSim:
    MuDetCosmicFlags.doTruth = True
#reading simulated data from pool     
    GlobalFlags.DataSource.set_geant4()
    GlobalFlags.InputFormat.set_pool()
    include ("MuonCommRecExample/MuDetCosmicDigitization_jobOptions.py")
else:
    MuDetCosmicFlags.doTruth = False  
#reading real data from ByteStream
    GlobalFlags.DataSource.set_data()
    GlobalFlags.InputFormat.set_bytestream()

# synchronise new-style flags
recFlags.doTruth = MuDetCosmicFlags.doTruth

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
if MuDetCosmicFlags.doSim:
    print 'Run on simulated data '
    include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
    # Read Pool data
    ServiceMgr.EventSelector.InputCollections += [
    "rfio:/castor/cern.ch/user/l/lytken/cosmic_1250/digitization/MuonVolume/withEndCaps/dig.05AprProd-10000000.root",
    "rfio:/castor/cern.ch/user/l/lytken/cosmic_1250/digitization/MuonVolume/withEndCaps/dig.05AprProd-10000001.root",    
    "rfio:/castor/cern.ch/user/l/lytken/cosmic_1250/digitization/MuonVolume/withEndCaps/dig.05AprProd-10000099.root"
    ] 
    ServiceMgr.PoolSvc.AttemptCatalogPatch = True

else:
    print 'Run on Real data '   
    include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
    svcMgr.ROBDataProviderSvc.filterEmptyROB = True
    include( "ByteStreamCnvSvcBase/BSAddProvSvc_RIO_jobOptions.py" )
    include( "ByteStreamCnvSvcBase/BSAddProvSvc_RDO_jobOptions.py" )
    # Specify input file
    ByteStreamInputSvc = svcMgr.ByteStreamInputSvc 
    # Input directory
    ByteStreamInputSvc.InputDirectory = [ MuonDQAFlags.InputDir ]
    # Prefix 
    ByteStreamInputSvc.FilePrefix     = [ MuonDQAFlags.FilePrefix ]
    # Run number 
    ByteStreamInputSvc.RunNumber      = [ MuonDQAFlags.RunNumber ]
    if MuonDQAFlags.doSimpleFile:
        fileNames = os.popen(datafiles).readlines()
        fileNames.sort()
        ByteStreamInputSvc.FullFileName +=  [ os.path.join(inDir,f).strip() for f in fileNames ]
        print "Input files:"
        print ByteStreamInputSvc.FullFileName       
       
# ------------------------------------
# Detector Description /IOV Svc
# ------------------------------------
include ("MuonCommRecExample/MuDetCosmicDetDescr_jobOptions.py" )
#--------------------------------------------------------------
# include joboptions to read Muon PREPRAWDATA,  cabling maps
#--------------------------------------------------------------
include ("MuonCommRecExample/MuDetCosmicPRD_jobOptions.py")
#--------------------------------------------------------------
#initialize MuCTPI converter
#--------------------------------------------------------------
if MuDetCosmicFlags.doMUCTPI or MuDetCosmicFlags.doCTP:
    include ("MuonCommRecExample/ReadMuCTPI_jobOptions.py")
#--------------------------------------------------------------
# Conditions database access (only for real data for the moment)
#--------------------------------------------------------------          
if not MuDetCosmicFlags.doSim:
    include ("MuonCommRecExample/MuDetCosmicConDB_jobOptions.py")           
#--------------------------------------------------------------
# Setup reconstruction
#--------------------------------------------------------------
include("MuonCommRecExample/MuonCommRec_jobOptions.py")
#--------------------------------------------------------------
# Write out ESD  
#--------------------------------------------------------------
if MuDetCosmicFlags.doESD:
    include( "MuonCommRecExample/MuDetCosmicPOOL_jobOptions.py")
#--------------------------------------------------------------
# Monitoring Histogram  
#--------------------------------------------------------------
if MuonDQAFlags.doMuonMonitoring:
    if MuonDQAFlags.doMuonSegmMon:
        include("MuonSegmMonitoring/MuonSegmMonitoring_options.py")
        if MuDetCosmicFlags.doMoore and not MuDetCosmicFlags.doMuonboy:
            ToolSvc.MuonSegmValidation.WhichSegmCollections = [1,0]
        if MuDetCosmicFlags.doMuonboy and not MuDetCosmicFlags.doMoore:
            ToolSvc.MuonSegmValidation.WhichSegmCollections = [0,1] 	     
### Histograms Service
    from GaudiSvc.GaudiSvcConf import THistSvc
    svcMgr +=THistSvc()
    atlaspak='MuonSegmMonitoring'
    RootHistOutputFileName = str(atlaspak)+'.'+str(sampletype)+'.root'

    from MuonDQAUtils.MuonDQAUtilsConf import Muon__MuonDQATrackHistTool
    Muon__MuonDQATrackHistTool = Muon__MuonDQATrackHistTool(name = "MuonDQATrackHistTool" )
    ToolSvc += Muon__MuonDQATrackHistTool

# Writing the histogram file
    if os.path.exists(RootHistOutputFileName):
        os.remove(RootHistOutputFileName)
    # Writing the histogram files from each sub-algorithm
    svcMgr.THistSvc.Output  += ["GLOBAL DATAFILE='%s' OPT='NEW'" % RootHistOutputFileName ]
  
#--------------------------------------------------------------
# Graphics  
#--------------------------------------------------------------
if MuDetCosmicFlags.doGraphics:
    include( "MuonCommRecExample/MuDetCosmicGraphics_jobOptions.py" )
#--------------------------------------------------------------
# Performace monitoring (CPU and Memory)
#--------------------------------------------------------------
if MuonDQAFlags.doCpuMuonMon:
    include("MuonSegmMonitoring/MuonSegmMonitoring_CPUPerfMon.py")
# -------------------------------------------------------------
# Number of events to be processed (default is 10)
# -------------------------------------------------------------
# get EventSelector
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import EventSelectorByteStream
svcMgr += EventSelectorByteStream("EventSelector")
theApp.EvtSel = "EventSelector"
# for EventType
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamCnvSvc
svcMgr += ByteStreamCnvSvc()
# Properties 
EventSelector = svcMgr.EventSelector
EventSelector.ByteStreamInputSvc     = "ByteStreamInputSvc"; 
EventPersistencySvc = svcMgr.EventPersistencySvc
EventPersistencySvc.CnvServices += [ "ByteStreamCnvSvc" ]

MuDetCosmicFlags.EvtMax=10
# Number of events to be processed (default is 10)
theApp.EvtMax = MuDetCosmicFlags.EvtMax

MuDetCosmicFlags.SkipEvents=0
svcMgr.EventSelector.SkipEvents=MuDetCosmicFlags.SkipEvents 
 
# -------------------------------------------------------------
#Output level
# -------------------------------------------------------------
ServiceMgr += CfgMgr.MessageSvc( Format = "% F%50W%S%7W%R%T %0W%M",
                                 defaultLimit=1000000 , 
				 OutputLevel = INFO, 
                                 useColors = False  # use color output text
				 )
#--------------------------------------------------------------
#For Callgrind Tests
#--------------------------------------------------------------
if MuonDQAFlags.doCallgrind:
     from Valkyrie.ValkyrieConf import ValgrindSvc
     valgrindSvc = ValgrindSvc( OutputLevel = VERBOSE )
     if MuonDQAFlags.doMuonHitMon:
         valgrindSvc.ProfiledAlgs += MuonDQAFlags.CallgrindSegmAlgs
         valgrindSvc.IgnoreFirstNEvents = MuonDQAFlags.CallgrindSkip      # to start instrumentation after the second event only
     # Add service and turn on auditing
     svcMgr += valgrindSvc
     theApp.CreateSvc += [svcMgr.ValgrindSvc.getFullJobOptName()]
     theApp.AuditAlgorithms = True
     theApp.AuditTools      = True
     theApp.AuditServices   = True

#--------------------------------------------------------------------------- 
# THIS IS TEMPORY HACK TO GET RID OF MUON ERRORS BEING PRINTED FOR EVERY EVENT
# NEED A REAL FIX 
#--------------------------------------------------------------------------- 
ToolSvc += CfgMgr.MuonSegmValidation(OutputLevel = INFO)  
Service( "THistSvc" ).OutputLevel = WARNING 
#--------------------------------------------------------------------------- 
print "List all DLL"
print theApp.Dlls
print "List all ExtSvc"
print theApp.ExtSvc
print "List of all top algorithms"
print theApp.TopAlg
#--------------------------------------------------------------------------- 
print topSequence
print ServiceMgr
print ToolSvc
#======================================================================
