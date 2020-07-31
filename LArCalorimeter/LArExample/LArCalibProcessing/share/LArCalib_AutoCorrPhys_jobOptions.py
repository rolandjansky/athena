#Job Opts to compute AutoCorrelation based on a zero-bias physics run.

from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.FilesInput=["data11_7TeV.00190256.physics_ZeroBias.merge.RAW._lb0340._SFO-ALL._0001.1","data11_7TeV.00190256.physics_ZeroBias.merge.RAW._lb0341._SFO-ALL._0001.1",]


if not 'RunNumberList' in dir():
	from PyUtils.MetaReaderPeeker import metadata
	RunNumberList = metadata['runNumbers']

#LArCalibFolderOutputTag="-UPD3-00"

if not 'EventsRef' in dir():
   EventsRef     = 10
   
if not 'NSigma' in dir():
   NSigma = 5

if not 'NSamples' in dir():
   NSamples = 7

if not 'GroupingType' in dir():
   GroupingType = "ExtendedSubDetector"

include("LArCalibProcessing/LArCalib_Flags.py")

if not 'WriteNtuple' in dir():
   WriteNtuple = LArCalib_Flags.WriteNtuple

if not 'WritePoolFile' in dir():
   WritePoolFile = LArCalib_Flags.WritePoolFile

if not 'WriteIOV' in dir():
   WriteIOV = LArCalib_Flags.WriteIOV

if not 'IOVBegin' in dir():
   IOVBegin = min(RunNumberList)

if not 'IOVEnd' in dir():
   IOVEnd = LArCalib_Flags.IOVEnd

if not 'LArCalibFolderOutputTag' in dir():
   LArCalibFolderOutputTag = "-UPD3-00"

if not 'LArAutoCorrPhysFolder' in dir():
   LArAutoCorrPhysFolder = "/LAR/ElecCalibOfl/AutoCorrPhys"

if not 'LArAutoCorrPhysKey' in dir():
   LArAutoCorrPhysKey = "LArAutoCorr"

if not 'OutputAutoCorrRootFileDir' in dir():
   OutputAutoCorrRootFileDir  = "."

if not 'OutputAutoCorrPoolFileDir' in dir():
   OutputAutoCorrPoolFileDir  = "."

if not 'OutputDB' in dir():
   OutputDB = LArCalib_Flags.OutputDB

if 'OutputSQLiteFile' in dir():
   OutputDB = DBConnectionFile(OutputSQLiteFile)

if not 'KeyOutputAC' in dir():
   KeyOutputAC      = "LArAutoCorr"

if not 'BaseFileNameAutoCorr' in dir():
   BaseFileNameAutoCorr = "LArAutoCorr"

for RunNumber in RunNumberList:
   BaseFileNameAutoCorr = BaseFileNameAutoCorr + "_" + str(RunNumber)

if not 'OutputAutoCorrPoolFileName' in dir():
   OutputAutoCorrPoolFileName  = BaseFileNameAutoCorr + ".pool.root"

if not 'OutputAutoCorrRootFileName' in dir():
   OutputAutoCorrRootFileName  = BaseFileNameAutoCorr + ".root"


if not 'OutputSQLiteFile' in dir():
   OutputSQLiteFile = BaseFileNameAutoCorr +".db"

OutputDB="sqlite://;schema="+OutputSQLiteFile+";dbname=CONDBR2"

# FIXME : fix name and folder and tag
if not 'OutputObjectSpecAutoCorr' in dir():
   OutputObjectSpecAutoCorr = "LArAutoCorrComplete#"+LArAutoCorrPhysKey+"#"+LArAutoCorrPhysFolder
   OutputTagSpecAutoCorr=LArCalibFolderTag(LArAutoCorrPhysFolder,LArCalibFolderOutputTag)


#######################################################################################
# print summary
#######################################################################################

PedestalAutoCorrLog = logging.getLogger( "PedestalAutoCorrLog" )
PedestalAutoCorrLog.info( " ======================================================== " )
PedestalAutoCorrLog.info( " ***           LAr Pedestal/AutoCorr summary          *** " )
PedestalAutoCorrLog.info( " ======================================================== " )
PedestalAutoCorrLog.info( " RunNumber                         = "+str(RunNumberList) )
#PedestalAutoCorrLog.info( " LArGain                           = "+str(GainList) )
PedestalAutoCorrLog.info( " OutputAutoCorrPoolFullFileName    = "+OutputAutoCorrPoolFileDir + "/" + OutputAutoCorrPoolFileName )
PedestalAutoCorrLog.info( " OutputObjectSpecAutoCorr          = "+OutputObjectSpecAutoCorr )
PedestalAutoCorrLog.info( " OutputTagSpecAutoCorr             = "+OutputTagSpecAutoCorr )
PedestalAutoCorrLog.info( " IOVBegin                          = "+str(IOVBegin) )
PedestalAutoCorrLog.info( " IOVEnd                            = "+str(IOVEnd) )
PedestalAutoCorrLog.info( " LArCalibOutputDB                  = "+OutputDB )
PedestalAutoCorrLog.info( " ======================================================== " )

#######################################################################################
include ("LArConditionsCommon/LArMinimalSetup.py")

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

## define the DB Gobal Tag :
svcMgr.IOVDbSvc.GlobalTag   = "COMCOND-ES1PST-005-04"
try:
   svcMgr.IOVDbSvc.DBInstance=""
except:
   pass

theByteStreamInputSvc=svcMgr.ByteStreamInputSvc
svcMgr.EventSelector.Input=athenaCommonFlags.FilesInput()

theByteStreamAddressProviderSvc =svcMgr.ByteStreamAddressProviderSvc
theByteStreamAddressProviderSvc.TypeNames += ["LArFebHeaderContainer/LArFebHeader"]
theByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/FREE"]


# Get Electronics noise values for Pedestal(RMS), AutoCorr and ramps

## The reference is the Oracle DB
include("LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")
selection=""
conddb.addFolder("LAR_ONL","/LAR/ElecCalibOnl/Pedestal<key>PedestalElec</key>"+selection)
conddb.addFolder("LAR_OFL","/LAR/ElecCalibOfl/AutoCorrs/AutoCorr<key>LArAutoCorrElec</key><tag>LARElecCalibOflAutoCorrsAutoCorr-UPD3-00</tag>"+selection)

from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg = TriggerConfigGetter()

from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool

bct=BunchCrossingTool()

from LArCalibUtils.LArCalibUtilsConf import LArAutoCorrMaker
LArAutoCorrMaker=LArAutoCorrMaker("LArAutoCorrMaker")
LArAutoCorrMaker.KeyList = ["FREE"]
LArAutoCorrMaker.events_ref = EventsRef
LArAutoCorrMaker.nsigma     = NSigma
LArAutoCorrMaker.KeyOutput  = KeyOutputAC
LArAutoCorrMaker.GroupingType = GroupingType
LArAutoCorrMaker.physics      = 1
LArAutoCorrMaker.MinBCFromFront=10
#LArAutoCorrMaker.OutputLevel = DEBUG
LArAutoCorrMaker.BunchCrossingTool=bct
topSequence += LArAutoCorrMaker

# extrapolation to other gains
from LArCalibUtils.LArCalibUtilsConf import LArAutoCorrExtrapolate
LArAutoCorrExtrapolate=LArAutoCorrExtrapolate("LArAutoCorrExtrapolate")
LArAutoCorrExtrapolate.KeyOutput = LArAutoCorrPhysKey

topSequence += LArAutoCorrExtrapolate



if ( WriteNtuple ) :

   Nvector = (NSamples+1)*NSamples/2 + 1
   from LArCalibTools.LArCalibToolsConf import LArAutoCorr2Ntuple
   LArAutoCorr2Ntuple = LArAutoCorr2Ntuple( "LArAutoCorr2Ntuple" )
   LArAutoCorr2Ntuple.Nsamples     = Nvector
   LArAutoCorr2Ntuple.ContainerKey = KeyOutputAC
   LArAutoCorr2Ntuple.AddFEBTempInfo=False
   topSequence += LArAutoCorr2Ntuple

   theApp.HistogramPersistency = "ROOT"
   from GaudiSvc.GaudiSvcConf import NTupleSvc
   if os.path.exists(OutputAutoCorrRootFileDir + "/" + OutputAutoCorrRootFileName):
      os.remove(OutputAutoCorrRootFileDir + "/" + OutputAutoCorrRootFileName)
   svcMgr += NTupleSvc()
   svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+OutputAutoCorrRootFileDir + "/" + OutputAutoCorrRootFileName+"' OPT='NEW'" ]


if ( WritePoolFile ) :

        from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg

        if os.path.exists(OutputAutoCorrPoolFileDir + "/" + OutputAutoCorrPoolFileName):
           os.remove(OutputAutoCorrPoolFileDir + "/" + OutputAutoCorrPoolFileName)
        OutputConditionsAlgAC=OutputConditionsAlg("OutputConditionsAlgAC",OutputAutoCorrPoolFileDir + "/" + OutputAutoCorrPoolFileName,
                                                      [OutputObjectSpecAutoCorr],[OutputTagSpecAutoCorr],WriteIOV)
        OutputConditionsAlgAC.Run1     = IOVBegin
        if IOVEnd>0:
           OutputConditionsAlgAC.Run2=IOVEnd

        svcMgr.IOVDbSvc.dbConnection  = OutputDB

        from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
        svcMgr += IOVRegistrationSvc()
        svcMgr.IOVRegistrationSvc.OutputLevel = DEBUG
        svcMgr.IOVRegistrationSvc.RecreateFolders = False


###########################################################################

svcMgr.MessageSvc.OutputLevel  = INFO
svcMgr.MessageSvc.defaultLimit = 10000
svcMgr.MessageSvc.Format       = "% F%20W%S%7W%R%T %0W%M"

svcMgr+=CfgMgr.AthenaEventLoopMgr(OutputLevel = INFO)

from AthenaCommon.AppMgr import theAuditorSvc
from AthenaCommon.ConfigurableDb import getConfigurable
theAuditorSvc += getConfigurable("MemStatAuditor")(OutputLevel = WARNING)
theApp.AuditAlgorithms=True
theAuditorSvc += getConfigurable("ChronoAuditor")(OutputLevel = WARNING)
theAuditorSvc += getConfigurable("NameAuditor")(OutputLevel = WARNING)
svcMgr.ChronoStatSvc.OutputLevel  = INFO

if ( WriteNtuple ) :
    LArAutoCorr2Ntuple.OutputLevel = DEBUG

svcMgr.PoolSvc.ReadCatalog+=['xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/poolcond/PoolCat_comcond_castor.xml']
