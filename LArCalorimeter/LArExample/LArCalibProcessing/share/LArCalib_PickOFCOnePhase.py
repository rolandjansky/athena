import commands

###########################################################################
#
# <Fabien.Tarrade@cern.ch>
#
# Example jobOptions to pickup OFC phys one phase 
#
# Last update: 09/12/2008 <Fabien.Tarrade@cern.ch>
#
###########################################################################

include("LArCalibProcessing/LArCalib_Flags.py")

###########################################################################
#               Input selection (OFC Phys)
###########################################################################

# RunNumber trigger IOV if reading from COOL is eneabled
if not 'RunNumber' in dir():
   RunNumber = '0000000'

if not 'SubDet' in dir():
   SubDet = "Barrel"  

if not 'Partition' in dir():
   if (SubDet=='EndCap'):
      Partition      = "EB-EC*"
   else:
      Partition      = "EB-EMB*"

## OFC Phys      

if not 'ReadOFCFromCOOL' in dir():
   ReadOFCFromCOOL = True

if not 'InputPoolDir' in dir():
   InputPoolDir = commands.getoutput("pwd")

if not 'InputPoolFileName' in dir():
   InputPoolFileName = "LArOFCShapePhys.pool.root"

## Shape Phys      
if not 'ReadShapeFromCOOL' in dir():
   ReadShapeFromCOOL = True

if not 'InputShapePoolDir' in dir():
   InputShapePoolDir = commands.getoutput("pwd")


## COOL Channel selection
if not 'ChannelSelection' in dir():
   # Read all
   ChannelSelection = " "
   ## Example to read only cool for Barrel C : Strips->Back
   #ChannelSelection = "<channelSelection>0,35:66</channelSelection>" 

from string import *
def DBConnectionFile(sqlitefile):
   return "sqlite://;schema="+sqlitefile+";dbname=CONDBR2"

###########################################################################
#                     OFC properties                                      #
###########################################################################

if not 'TimeOffsetCorrection' in dir():   
   TimeOffsetCorrection = 0

if not 'DefaultPhase' in dir():
   DefaultPhase = 1

if not 'OFCFolder' in dir() :
   OFCFolder  = "/LAR/ElecCalibOfl/OFC/PhysWave/RTM/5samples3bins17phases"
   #OFCFolder  = "/LAR/ElecCalibOfl/OFC/PhysWave/RTM/5samples"
   
if not 'ShapeFolder' in dir() :
   ShapeFolder  =  "/LAR/ElecCalibOfl/Shape/RTM/5samples3bins17phases"
   #ShapeFolder  =  "/LAR/ElecCalibOfl/Shape/RTM/5samples"

if not 'OFCKey' in dir() :
   OFCKey = "LArOFC"

if not 'ShapeKey' in dir():
   ShapeKey = "LArShape"

if not 'FillShape' in dir():
   FillShape = True # Do not fill a LArShapeComplete object for calibration OFC!

###########################################################################
#                               OFC output                                #
###########################################################################

if not 'GroupingType' in dir():
   GroupingType = "ExtendedSubDetector"
   
if not 'WriteNtuple' in dir():
   WriteNtuple = LArCalib_Flags.WriteNtuple

if not 'WritePoolFile' in dir():
   WritePoolFile = LArCalib_Flags.WritePoolFile

if not 'WriteIOV' in dir():
   WriteIOV = LArCalib_Flags.WriteIOV

if not 'IOVBegin' in dir():
   IOVBegin = int(RunNumber)

if not 'IOVEnd' in dir():
   IOVEnd = LArCalib_Flags.IOVEnd   

if not 'OutputOFCRootFileDir' in dir():
   OutputOFCRootFileDir = commands.getoutput("pwd")
   
if not 'OutputPoolFileDir' in dir():
   OutputPoolFileDir = commands.getoutput("pwd")

OFCFileTag = str(RunNumber)+"_"+Partition.replace("*","")

OFCFileTag += "_5samples_Picked"

if not 'OutputOFCRootFileName' in dir():
   OutputOFCRootFileName = "LArOFCPhys_"+OFCFileTag + ".root"
   
if not 'OutputPoolFileName' in dir():
   OutputPoolFileName = "LArOFCShapePhys_"+OFCFileTag + ".pool.root"
   
if not 'OutputDB' in dir():
   OutputDB = LArCalib_Flags.OutputDB

if 'OutputSQLiteFile' in dir():
   OutputDB = DBConnectionFile(OutputSQLiteFile)

#inputs


#outputs
if not 'FolderShapeOutput' in dir():
   FolderShapeOutput = "/LAR/ElecCalibOfl/Shape/RTM/5samples1phase"
if not 'FolderOFCOutput' in dir():
   FolderOFCOutput = "/LAR/ElecCalibOfl/OFC/PhysWave/RTM/5samples1phase"

rs=FolderTagResover()

#inputs
if not 'OFCFolderTag' in dir():
   OFCFolderTag=rs.getFolderTagSuffix(OFCFolder)
   OFCFolderTagSpec= LArCalibFolderTag(OFCFolder,OFCFolderTag)

if not 'ShapeFolderTag' in dir():
   ShapeFolderTag=rs.getFolderTagSuffix(ShapeFolder)
   ShapeFolderTagSpec= LArCalibFolderTag(ShapeFolder,ShapeFolderTag)
   

#outputs
if not 'LArOFCFolderOutputTag' in dir():
   LArOFCFolderOutputTag=rs.getFolderTagSuffix(FolderOFCOutput)

if not 'LArShapeFolderOutputTag' in dir(): 
   LArShapeFolderOutputTag=rs.getFolderTagSuffix(FolderShapeOutput)

del rs #Close database connection


if not 'OutputObjectSpecOFC' in dir():
   OutputObjectSpecOFC       = "LArOFCComplete#"+OFCKey+"#"+FolderOFCOutput 
   OutputObjectSpecTagOFC    = LArCalibFolderTag(FolderOFCOutput,LArOFCFolderOutputTag)

if not 'OutputObjectSpecShape' in dir():
   OutputObjectSpecShape       = "LArShapeComplete#"+ShapeKey+"#"+FolderShapeOutput 
   OutputObjectSpecTagShape    =  LArCalibFolderTag(FolderShapeOutput,LArShapeFolderOutputTag)
     

if not 'DBConnectionCOOL' in dir():
   DBConnectionCOOL = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_LAR;dbname=CONDBR2;"

if ( ReadOFCFromCOOL ):      
   if 'InputOFCSQLiteFile' in dir():
      InputDBConnectionOFC = DBConnectionFile(InputOFCSQLiteFile)
   else:
      InputDBConnectionOFC = DBConnectionCOOL
      
if ( ReadShapeFromCOOL ):      
   if 'InputShapeSQLiteFile' in dir():
      InputDBConnectionShape = DBConnectionFile(InputShapeSQLiteFile)
   else:
      InputDBConnectionShape = DBConnectionCOOL

## Bad Channel
      
if not 'ReadBadChannelFromCOOL' in dir():
   ReadBadChannelFromCOOL = True   

if ( ReadBadChannelFromCOOL ):      
   if 'InputBadChannelSQLiteFile' in dir():
      InputDBConnectionBadChannel = DBConnectionFile(InputBadChannelSQLiteFile)
   else:
      #InputDBConnectionBadChannel = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_LAR;dbname=CONDBR2;"
      InputDBConnectionBadChannel = "COOLOFL_LAR/CONDBR2"            

###########################################################################
# Print summary
###########################################################################

OFCLog = logging.getLogger( "OFCLog" )
OFCLog.info( " ======================================================== " )
OFCLog.info( " ***                 LAr OFC summary                  *** " )
OFCLog.info( " ======================================================== " )
OFCLog.info( " RunNumber                          = "+str(RunNumber) )
if ( ReadOFCFromCOOL ):
   OFCLog.info( " InputDBConnectionOFC          = "+InputDBConnectionOFC )
else :
   OFCLog.info( " InputPoolFileName             = "+InputFileName )
if 'OFCLArCalibFolderTag' in dir() :   
   OFCLog.info( " OFCLArCalibFolderTag          = "+OFCLArCalibFolderTag )
if ( ReadOFCFromCOOL and ChannelSelection != " " ):
   OFCLog.info( " ChannelSelection                   = "+ChannelSelection )
OFCLog.info( " OutputOFCRootFullFileName          = "+OutputOFCRootFileDir+"/"+OutputOFCRootFileName )
OFCLog.info( " OutputPoolFullFileName             = "+OutputPoolFileDir+"/"+OutputPoolFileName )
OFCLog.info( " OutputObjectSpecOFC                = "+str(OutputObjectSpecOFC) )
OFCLog.info( " OutputObjectSpecTagOFC             = "+str(OutputObjectSpecTagOFC) )
OFCLog.info( " IOVBegin                           = "+str(IOVBegin) )
OFCLog.info( " IOVEnd                             = "+str(IOVEnd) )
OFCLog.info( " LArCalibOutputDB                   = "+OutputDB )
OFCLog.info( " ======================================================== " )

###########################################################################
#
# Global settings
#
###########################################################################

include("AthenaCommon/Atlas_Gen.UnixStandardJob.py")

#
# Provides ByteStreamInputSvc name of the data file to process in the offline context
#

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

include("LArCalibProcessing/LArCalib_MinimalSetup.py")

###########################################################################
#                                                                         #
#    Input conditions data (OFC) for COOL and/or POOL                     #
#                                                                         #
###########################################################################

include("AthenaPoolCnvSvc/AthenaPool_jobOptions.py")
include("LArCondAthenaPool/LArCondAthenaPool_joboptions.py")

from IOVDbSvc.CondDB import conddb
PoolFileList     = []

include ("LArCalibProcessing/LArCalib_BadChanTool.py")

if not 'InputBadChannelSQLiteFile' in dir():
   OFCLog.info( "Read Bad Channels from Oracle DB")
else :   
   OFCLog.info( "Read Bad Channels from SQLite file") 

if 'BadChannelsLArCalibFolderTag' in dir() :
   BadChannelsTagSpec = LArCalibFolderTag (BadChannelsFolder,BadChannelsLArCalibFolderTag) 
   conddb.addFolder("",BadChannelsFolder+"<tag>"+BadChannelsTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>")
else :
   conddb.addFolder("",BadChannelsFolder+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>")

if 'MissingFEBsLArCalibFolderTag' in dir() :
   MissingFEBsTagSpec = LArCalibFolderTag (MissingFEBsFolder,MissingFEBsLArCalibFolderTag)   
   conddb.addFolder("",MissingFEBsFolder+"<tag>"+MissingFEBsTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>")
else :
   conddb.addFolder("",MissingFEBsFolder+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>")
   
## define the DB Gobal Tag :
svcMgr.IOVDbSvc.GlobalTag   = LArCalib_Flags.globalFlagDB
try:
   svcMgr.IOVDbSvc.DBInstance=""
except: 
   pass


from LArCalibProcessing.LArCalibCatalogs import larCalibCatalogs
svcMgr.PoolSvc.ReadCatalog += larCalibCatalogs

if ( ReadOFCFromCOOL ):
   if not ('InputOFCSQLiteFile')  in dir():
      OFCLog.info( "Read OFC from Oracle" )
   else :
      OFCLog.info( "Read OFC from SQLite file" )
      
if ( ReadOFCFromCOOL ):
      conddb.addFolder("",OFCFolder+"<tag>"+OFCFolderTagSpec+"</tag><key>LArOFCIn</key><dbConnection>"+InputDBConnectionOFC+"</dbConnection>"+ ChannelSelection)

else:
   if 'InputPoolFileName' in dir():
      OFCLog.info( "Read OFC and Shape from POOL file")
      PoolFileList += [ InputPoolDir+"/"+InputPoolFileName ]
   else:
      OFCLog.info( "No PoolFileList found! Please list the POOL files containing OFC or read from COOL." )
      theApp.exit(-1)



if ( ReadShapeFromCOOL ):
   if not ('InputShapeSQLiteFile')  in dir():
      OFCLog.info( "Read Shape from Oracle" )
   else :
      OFCLog.info( "Read Shape from SQLite file" )
      
if ( ReadShapeFromCOOL ):
      conddb.addFolder("",ShapeFolder+"<tag>"+ShapeFolderTagSpec+"</tag><key>LArShapeIn</key><dbConnection>"+InputDBConnectionShape+"</dbConnection>"+ ChannelSelection)


conddb.addFolder("","/LAR/ElecCalibOfl/OFCBin/Dummy<dbConnection>"+DBConnectionCOOL+"</dbConnection>"+ ChannelSelection)     
   
      
      
if ( len(PoolFileList)>0 ):

   from AthenaCommon.ConfigurableDb import getConfigurable
   svcMgr += getConfigurable( "ProxyProviderSvc" )()
   svcMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]

   svcMgr += getConfigurable( "CondProxyProvider" )()
   svcMgr.CondProxyProvider.InputCollections += PoolFileList

###########################################################################
#                      pickup one phase OFC                               #
###########################################################################

from LArCalibUtils.LArCalibUtilsConf import LArOFPhasePicker
LArOFPhasePicker = LArOFPhasePicker("LArOFPhasePicker")
LArOFPhasePicker.KeyPhase = "LArOFCPhase"
LArOFPhasePicker.KeyOFC_new = OFCKey
LArOFPhasePicker.KeyOFC = "LArOFCIn"
LArOFPhasePicker.KeyShape_new = ShapeKey
LArOFPhasePicker.KeyShape = "LArShapeIn"
LArOFPhasePicker.GroupingType = GroupingType
LArOFPhasePicker.DefaultPhase = DefaultPhase
LArOFPhasePicker.TimeOffsetCorrection = TimeOffsetCorrection
topSequence += LArOFPhasePicker

###########################################################################

if ( WriteNtuple ) :

   from LArCalibTools.LArCalibToolsConf import LArOFC2Ntuple
   LArOFC2Ntuple = LArOFC2Ntuple("LArOFC2Ntuple")
   LArOFC2Ntuple.ContainerKey = OFCKey
   LArOFC2Ntuple.AddFEBTempInfo = False
   topSequence+=LArOFC2Ntuple
   
   theApp.HistogramPersistency = "ROOT"
   from GaudiSvc.GaudiSvcConf import NTupleSvc
   if os.path.exists(OutputOFCRootFileDir+"/"+OutputOFCRootFileName): 
      os.remove(OutputOFCRootFileDir+"/"+OutputOFCRootFileName)
   svcMgr += NTupleSvc()
   svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+OutputOFCRootFileDir+"/"+OutputOFCRootFileName+"' OPT='NEW'" ]

   
if (  WritePoolFile ) :

   from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
   if os.path.exists(OutputPoolFileDir+"/"+OutputPoolFileName): 
      os.remove(OutputPoolFileDir+"/"+OutputPoolFileName)
   OutputConditionsAlg = OutputConditionsAlg("OutputConditionsAlg",OutputPoolFileDir+"/"+OutputPoolFileName,
                                                     [OutputObjectSpecOFC],[OutputObjectSpecTagOFC],WriteIOV)
   OutputConditionsAlg.Run1  = IOVBegin
   if IOVEnd>0:
      OutputConditionsAlg.Run2  = IOVEnd

   if ( FillShape ):
      OutputConditionsAlg.ObjectList += [OutputObjectSpecShape]
      OutputConditionsAlg.IOVTagList += [OutputObjectSpecTagShape]

   svcMgr.IOVDbSvc.dbConnection  = OutputDB
   
   from AthenaCommon.AppMgr import ServiceMgr
   from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
   svcMgr += IOVRegistrationSvc()
   svcMgr.IOVRegistrationSvc.OutputLevel = DEBUG
   svcMgr.IOVRegistrationSvc.RecreateFolders = True
#   svcMgr.IOVRegistrationSvc.SVFolder = True
   		
###########################################################################
#               Use EventSelector to select IOV                           #
###########################################################################	

from McEventSelector.McEventSelectorConf import McEventSelector
svcMgr += McEventSelector("EventSelector")
svcMgr.EventSelector.RunNumber	= 10000000 # not runNumberRunNumber
svcMgr.EventSelector.EventsPerRun      = 1
svcMgr.EventSelector.FirstEvent	= 1
svcMgr.EventSelector.InitialTimeStamp  = 0
svcMgr.EventSelector.TimeStampInterval = 1

##########################################################################
#          don't remove otherwise infinite loop                          #
##########################################################################

theApp.EvtMax = 1

###########################################################################

svcMgr.MessageSvc.OutputLevel  = WARNING
svcMgr.MessageSvc.defaultLimit = 10000
svcMgr.MessageSvc.Format       = "% F%20W%S%7W%R%T %0W%M"

svcMgr+=CfgMgr.AthenaEventLoopMgr(OutputLevel = VERBOSE)

from AthenaCommon.AppMgr import theAuditorSvc
from AthenaCommon.ConfigurableDb import getConfigurable
theAuditorSvc += getConfigurable("MemStatAuditor")(OutputLevel = DEBUG)
theAuditorSvc += getConfigurable("ChronoAuditor")()
theAuditorSvc += getConfigurable("NameAuditor")()

###########################################################################


