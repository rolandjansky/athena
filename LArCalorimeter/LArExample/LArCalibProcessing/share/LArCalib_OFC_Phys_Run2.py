
include("LArCalibProcessing/LArCalib_Flags.py")

if not 'IOVEnd' in dir():
   IOVEnd = 2147483647

if not 'RunNumber' in dir():
   RunNumber=2147483647

if not 'OutputPoolFileDir' in dir():
   OutputPoolFileDir = os.getcwd()

if not 'OutputPoolFileName' in dir():
   OutputPoolFileName = "LArOFCShapePhys"


if not "NColl" in dir():
   NColl=20

if not 'InputKey' in dir():   
   InputKey = "LArPhysWave"  # no shift applied to the PhysWave   

if not 'TimeShiftGuardRegion' in dir():
   TimeShiftGuardRegion = 12

if not "ChannelSelection" in dir():
   ChannelSelection=""

if not "GroupingType" in dir():
   GroupingType = "ExtendedSubDetector"

if not "RunThreaded" in dir():
   RunThreaded=False

##Picking 
if not 'DefaultPhase' in dir():
   DefaultPhase = 4

if not 'TimeOffsetCorrection' in dir():   
   TimeOffsetCorrection = 12


def DBConnectionFile(sqlitefile):
   return "sqlite://;schema="+sqlitefile+";dbname=CONDBR2"


###########################################################################
# OFC properties (2 instances in total)
###########################################################################

from LArCalibProcessing.OFCDefinitions import OFCDefs


#input folders:
if "AutoCorrFolder" not in dir():
   AutoCorrFolder  = LArCalib_Flags.LArAutoCorrFolder
if "PhysAutoCorrFolder" not in dir():
   PhysAutoCorrFolder  = LArCalib_Flags.LArPhysAutoCorrFolder

#Shift (can be output or input)
if "PhysWaveShiftFolder" not in dir():
   PhysWaveShiftFolder= LArCalib_Flags.PhysWaveShiftFolder

#if "OFCBinFolder" not in dir():
#   OFCBinFolder=LArCalib_Flags.OFCBinFolder

if "ShapeResidualsFolder"  not in dir():
   ShapeResidualsFolder=LArCalib_Flags.ShapeResidualsFolder


if 'OutputSQLiteFile' in dir():
   OutputDB = DBConnectionFile(OutputSQLiteFile)
else:
   OutputDB= DBConnectionFile("freshOFCs.db")

DBConnectionCOOL = "COOLOFL_LAR/CONDBR2"

if "InputAutoCorrPhysSQLiteFile" in dir():
   InputDBConnectionAutoCorrPhys = DBConnectionFile(InputAutoCorrPhysSQLiteFile)
else:
   InputDBConnectionAutoCorrPhys = DBConnectionCOOL
if 'InputAutoCorrSQLiteFile' in dir():
   InputDBConnectionAutoCorr = DBConnectionFile(InputAutoCorrSQLiteFile)
else:
   InputDBConnectionAutoCorr = DBConnectionCOOL

if 'InputPhysWaveSQLiteFile' in dir():
   InputDBConnectionPhysWave = DBConnectionFile(InputPhysWaveSQLiteFile)
else:
   InputDBConnectionPhysWave = DBConnectionCOOL


if not "InputDBConnectionPhysWaveShift" in dir():
   InputDBConnectionPhysWaveShift = DBConnectionCOOL

#if not "InputDBConnectionOFCBin" in dir():
#   InputDBConnectionOFCBin = DBConnectionCOOL

if not "InputDBConnectionShapeResidual" in dir():
   InputDBConnectionShapeResidual = DBConnectionCOOL


#Try to get IOV start from input physWave:
if not 'IOVBegin' in dir():
   IOVBegin = 0
   from LArCalibProcessing.extractFolderInfo import *
   folderinfo=extractFolderInfo(dbname=InputDBConnectionPhysWave,checkFolders=[LArCalib_Flags.LArPhysWaveFolder,],
                                selection="3:7,9:13,15:19,21:25,27:31,33:37") #Exclude FCAL channels that have infinete IOV
   print folderinfo
   if len(folderinfo)==1:
      IOVBegin=folderinfo[0][4]
      print "IOVStart taken from input PhysWave:", IOVBegin
   else:
      print "Failed to extract IOV start of input PhysWave"
      pass
   pass






###########################################################################
# Print summary
###########################################################################

OFCLog = logging.getLogger( "OFCLog" )
OFCLog.info( " ======================================================== " )
OFCLog.info( " ***                 LAr OFC summary                  *** " )
OFCLog.info( " ======================================================== " )
OFCLog.info( " RunNumber           = "+str(RunNumber) )
OFCLog.info( " Output DB            = "+OutputDB )
OFCLog.info( " PhysWave DB          = "+InputDBConnectionPhysWave)
OFCLog.info( " AutoCorr DB          = "+InputDBConnectionAutoCorr)
OFCLog.info( " AutoCorrPhys DB      = "+InputDBConnectionAutoCorrPhys)
OFCLog.info( " PhysWaveShift DB     = "+InputDBConnectionPhysWaveShift)
#OFCLog.info( " OFCBin DB            = "+InputDBConnectionOFCBin)
OFCLog.info( " Shape residual DB    = "+InputDBConnectionShapeResidual)

OFCLog.info( " IOV Start            = "+str(IOVBegin))
OFCLog.info( " IOV End              = "+str(IOVEnd))


OFCLog.info( " ======================================================== " )


###########################################################################
#
# Global settings
#
###########################################################################

include("AthenaCommon/Atlas_Gen.UnixStandardJob.py")

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.Luminosity.set_Value_and_Lock('zero')
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.InputFormat.set_Value_and_Lock('bytestream')
globalflags.DatabaseInstance.set_Value_and_Lock("CONDBR2")

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-R2-2015-04-00-00"

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn()  #Switched off to avoid geometry
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()
#DetFlags.Print()

#Set up GeoModel (not really needed but crashes without)
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetupRecoGeometry

#Get identifier mapping
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )

from IOVDbSvc.CondDB import conddb
svcMgr.IOVDbSvc.GlobalTag   = "LARCALIB-RUN2-00" #LArCalib_Flags.globalFlagDB   
svcMgr.IOVDbSvc.DBInstance=""
svcMgr.IOVDbSvc.dbConnection  = OutputDB

from LArCalibProcessing.LArCalibCatalogs import larCalibCatalogs
svcMgr.PoolSvc.ReadCatalog += larCalibCatalogs

if 'poolcat' in dir():
  svcMgr.PoolSvc.WriteCatalog="xmlcatalog_file:"+poolcat


#Input Database Folders:
#Assuming a tag-hierarchy in the input sqlite file! 


#input PhysWave
InputDBConnectionPhysWave="<db>%s</db>" % InputDBConnectionPhysWave
conddb.addFolder("",LArCalib_Flags.LArPhysWaveFolder+InputDBConnectionPhysWave+ ChannelSelection) 

#input AutoCorr
atag=""
if "AutoCorrFolderTag" in dir():
   atag+="<tag>%s</tag>" %AutoCorrFolderTag
InputDBConnectionAutoCorr="<db>%s</db>" % InputDBConnectionAutoCorr
conddb.addFolder("",LArCalib_Flags.LArAutoCorrFolder+InputDBConnectionAutoCorr+atag+ChannelSelection) 

#input PhysicsAutoCorr:
#rs=FolderTagResover(InputDBConnectionAutoCorrPhys)
if "PhysAutoCorrFolderTag" not in dir():
   PhysAutoCorrFolderTag=LArCalibFolderTag(PhysAutoCorrFolder,"_mu_"+str(NColl)+"-RUN2-UPD3-00")
InputDBConnectionAutoCorrPhys="<db>%s</db>" %InputDBConnectionAutoCorrPhys
conddb.addFolder("",PhysAutoCorrFolder+"<tag>"+PhysAutoCorrFolderTag+"</tag>"+InputDBConnectionAutoCorrPhys+"<key>LArPhysAutoCorr</key>")
#del rs

#input PhysWaveShift (usually from oracle)
InputDBConnectionPhysWaveShift="<db>%s</db>" % InputDBConnectionPhysWaveShift
conddb.addFolder("",PhysWaveShiftFolder+InputDBConnectionPhysWaveShift)

#InputDBConnectionOFCBin="<db>%s</db>" % InputDBConnectionOFCBin
#conddb.addFolder("",OFCBinFolder+InputDBConnectionOFCBin)

InputDBConnectionShapeResidual="<db>%s</db>" % InputDBConnectionShapeResidual
conddb.addFolder("",ShapeResidualsFolder+InputDBConnectionShapeResidual)


#Decoder Tool for electronic-noise only autocorrelation
from LArCalibUtils.LArCalibUtilsConf import LArAutoCorrDecoderTool
theLArAutoCorrDecoderTool = LArAutoCorrDecoderTool()
theLArAutoCorrDecoderTool.UseAlwaysHighGain=True
ToolSvc += theLArAutoCorrDecoderTool

#Decoder Tool for pile-up autocorrelation
theLArPhysAutoCorrDecoderTool = LArAutoCorrDecoderTool("LArPhysAutoCorrDecoderTool")
theLArPhysAutoCorrDecoderTool.DecodeMode=1
theLArPhysAutoCorrDecoderTool.UseAlwaysHighGain=True
theLArPhysAutoCorrDecoderTool.KeyAutoCorr="LArPhysAutoCorr"
ToolSvc += theLArPhysAutoCorrDecoderTool


#Instantiate LArOFCAlgos 

from LArCalibUtils.LArCalibUtilsConf import LArOFCAlg
from LArCalibUtils.LArCalibUtilsConf import LArOFPhasePicker

for ofcdef in OFCDefs:
   if ofcdef == 0: continue
   LArPhysOFCAlg=LArOFCAlg(ofcdef.Algoname)
   LArPhysOFCAlg.RunThreaded=RunThreaded
   LArPhysOFCAlg.ReadCaliWave = False
   LArPhysOFCAlg.KeyList      = [ InputKey ]
   LArPhysOFCAlg.Normalize    = True
   LArPhysOFCAlg.Verify       = True
   LArPhysOFCAlg.FillShape = True
   LArPhysOFCAlg.GroupingType = GroupingType

   LArPhysOFCAlg.Nphase    = ofcdef.Nphase
   LArPhysOFCAlg.Dphase    = ofcdef.Dphase
   LArPhysOFCAlg.Ndelay    = 24
   LArPhysOFCAlg.Nsample   = ofcdef.Nsamples

   LArPhysOFCAlg.AddTimeOffset = -1.0*TimeShiftGuardRegion
   LArPhysOFCAlg.LArPhysWaveBinKey ="LArPhysWaveShift"

   LArPhysOFCAlg.KeyShape = ofcdef.KeyShape

   LArPhysOFCAlg.UseDelta = ofcdef.useDelta[0]
   LArPhysOFCAlg.KeyOFC   = ofcdef.KeyOFC[0]
   if ofcdef.PhysAutoCorr[0]:
      LArPhysOFCAlg.DecoderTool = theLArPhysAutoCorrDecoderTool
   else:
      LArPhysOFCAlg.DecoderTool = theLArAutoCorrDecoderTool

   if len(ofcdef.PhysAutoCorr)==2: #Second OFC flavor defined
      LArPhysOFCAlg.ComputeOFCV2=True
      LArPhysOFCAlg.UseDeltaV2 = ofcdef.useDelta[1]
      LArPhysOFCAlg.KeyOFCV2   = ofcdef.KeyOFC[1]
      if ofcdef.PhysAutoCorr[1]:
         LArPhysOFCAlg.DecoderToolV2 = theLArPhysAutoCorrDecoderTool
      else:
         LArPhysOFCAlg.DecoderToolV2 = theLArAutoCorrDecoderTool

   if ofcdef.Nsamples==4:
         LArPhysOFCAlg.ReadDSPConfig   = ofcdef.ReadDSPConfig
         LArPhysOFCAlg.DSPConfigFolder = ofcdef.DSPConfigFolder
         conddb.addFolder("LAR_ONL","/LAR/Configuration/DSPConfiguration")
         #conddb.addFolder("","<db>sqlite://;schema=/afs/cern.ch/atlas/maxidisk/d20/Automation/AtlasProduction-20.1.0.2/run/dspconfig_std.db;dbname=CONDBR2</db>/LAR/Configuration/DSPConfiguration")
         #conddb.addFolder("","<db>sqlite://;schema=/afs/cern.ch/atlas/maxidisk/d20/Automation/AtlasProduction-20.1.0.2/run/dspconfig_run1.db;dbname=CONDBR2</db>/LAR/Configuration/DSPConfiguration")

   LArPhysOFCAlg.OutputLevel=WARNING

   topSequence+=LArPhysOFCAlg

   LArOFPhasePick=LArOFPhasePicker(ofcdef.Algoname+"Picker")
   LArOFPhasePick.KeyPhase = ""
   LArOFPhasePick.KeyOFC =  ofcdef.KeyOFC[0]
   LArOFPhasePick.KeyOFC_new =  ofcdef.KeyOFC[0]+"_picked"
   LArOFPhasePick.KeyShape =  ofcdef.KeyShape
   LArOFPhasePick.KeyShape_new =  ofcdef.KeyShape+"_picked"
   #LArOFPhasePick.OutputLevel=VERBOSE
   LArOFPhasePick.GroupingType = GroupingType
   LArOFPhasePick.DefaultPhase = DefaultPhase
   LArOFPhasePick.TimeOffsetCorrection = TimeOffsetCorrection
   topSequence += LArOFPhasePick

   if len(ofcdef.PhysAutoCorr)==2: #Second OFC flavor defined
      LArOFPhasePick2=LArOFPhasePicker(ofcdef.Algoname+"Picker2")
      #LArOFPhasePick2.OutputLevel=VERBOSE
      LArOFPhasePick2.KeyPhase = ""
      LArOFPhasePick2.KeyOFC =  ofcdef.KeyOFC[1]
      LArOFPhasePick2.KeyOFC_new =  ofcdef.KeyOFC[1]+"_picked"
      LArOFPhasePick2.doShape=False
      LArOFPhasePick2.GroupingType = GroupingType
      LArOFPhasePick2.DefaultPhase = DefaultPhase
      LArOFPhasePick2.TimeOffsetCorrection = TimeOffsetCorrection
      topSequence += LArOFPhasePick2
      pass

   #Shape residual correction
   from LArCalibUtils.LArCalibUtilsConf import LArShapeCorrector
   theLArShapeCorrector=LArShapeCorrector("ShapeCorr_"+ofcdef.Algoname)
   theLArShapeCorrector.KeyShape=ofcdef.KeyShape+"_picked"
   theLArShapeCorrector.KeyShape_newcorr=ofcdef.KeyShape+"_picked_corr"
   topSequence+=theLArShapeCorrector
   pass #end of loop over OFC definitions




from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg

#Output Folders:
suffix="-RUN2-UPD3-00" #Hardcode for now
for ofcdef in OFCDefs:
   if ofcdef == 0: continue
   PoolFile=OutputPoolFileDir+"/"+OutputPoolFileName+ofcdef.Algoname+".pool.root"
   if os.path.exists(PoolFile): os.remove(PoolFile)
      
   OutputObjectSpec=["LArShapeComplete#"+ofcdef.KeyShape+"#"+ofcdef.FolderShape,]
   OutputTagSpec=[LArCalibFolderTag(ofcdef.FolderShape,suffix),]
   #OutputTagSpec=["LArShapeComplete#"+ofcdef.KeyShape+"#"+ofcdef.FolderShape,]
   for i in range(len(ofcdef.KeyOFC)):
      OutputObjectSpec.append("LArOFCComplete#"+ofcdef.KeyOFC[i]+"#"+ofcdef.FolderOFC)
      if ofcdef.PhysAutoCorr[i]:
         OutputTagSpec.append(LArCalibFolderTag(ofcdef.FolderOFC,"-mu-"+str(NColl)+suffix))
      else:
         OutputTagSpec.append(LArCalibFolderTag(ofcdef.FolderOFC,suffix))
      
   outAlg=OutputConditionsAlg("Output"+ofcdef.Algoname,PoolFile,OutputObjectSpec,OutputTagSpec)
   outAlg.Run1 = IOVBegin
   if IOVEnd>0:
      outAlg.Run2 = IOVEnd
   pass

   #Now the same of the picked OFCs/Shape (+corr shape):
   PoolFilePicked=OutputPoolFileDir+"/"+OutputPoolFileName+ofcdef.Algoname+"_picked.pool.root"
   if os.path.exists(PoolFilePicked): os.remove(PoolFilePicked)
   
   OutputObjectSpec=["LArShapeComplete#"+ofcdef.KeyShape+"_picked#"+ofcdef.FolderShapePicked,
                     "LArShapeComplete#"+ofcdef.KeyShape+"_picked_corr#"+ofcdef.FolderShapePicked
                     ]
   OutputTagSpec=[LArCalibFolderTag(ofcdef.FolderShapePicked,suffix),
                  LArCalibFolderTag(ofcdef.FolderShapePicked,"-corr"+suffix),
                  ]
   
   for i in range(len(ofcdef.KeyOFC)):
      OutputObjectSpec.append("LArOFCComplete#"+ofcdef.KeyOFC[i]+"_picked#"+ofcdef.FolderOFCPicked)
      if ofcdef.PhysAutoCorr[i]:
         OutputTagSpec.append(LArCalibFolderTag(ofcdef.FolderOFCPicked,"-mu-"+str(NColl)+suffix))
      else:
         OutputTagSpec.append(LArCalibFolderTag(ofcdef.FolderOFCPicked,suffix))
      pass
   
   outAlgPicked=OutputConditionsAlg("Output"+ofcdef.Algoname+"Picked",PoolFilePicked,OutputObjectSpec,OutputTagSpec)
   outAlgPicked.Run1 = IOVBegin
   if IOVEnd>0:
      outAlgPicked.Run2 = IOVEnd
      pass

   
from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
svcMgr += IOVRegistrationSvc()
#svcMgr.IOVRegistrationSvc.OutputLevel = DEBUG
svcMgr.IOVRegistrationSvc.RecreateFolders = False

###########################################################################
#               Use EventSelector to select IOV                           #
###########################################################################	

from McEventSelector.McEventSelectorConf import McEventSelector
svcMgr += McEventSelector("EventSelector")
svcMgr.EventSelector.RunNumber	= RunNumber
svcMgr.EventSelector.EventsPerRun      = 1
svcMgr.EventSelector.FirstEvent	= 1
svcMgr.EventSelector.InitialTimeStamp  = 0
svcMgr.EventSelector.TimeStampInterval = 1

##########################################################################
#          don't remove otherwise infinite loop                          #
##########################################################################

theApp.EvtMax = 1

###########################################################################

#svcMgr.MessageSvc.OutputLevel  = WARNING
svcMgr.MessageSvc.defaultLimit = 100000000
#svcMgr.MessageSvc.Format       = "% F%20W%S%7W%R%T %0W%M"

#svcMgr+=CfgMgr.AthenaEventLoopMgr(OutputLevel = VERBOSE)

from AthenaCommon.AppMgr import theAuditorSvc
from AthenaCommon.ConfigurableDb import getConfigurable
theAuditorSvc += getConfigurable("MemStatAuditor")(OutputLevel = DEBUG)
#theAuditorSvc += getConfigurable("ChronoAuditor")()
#theAuditorSvc += getConfigurable("NameAuditor")()

###########################################################################
