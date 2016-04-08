import commands
###########################################################################
include("LArCalibProcessing/LArCalib_Flags.py")

# output POOL file:
if not 'writePOOLfile' in dir():
    writePOOLfile = True
    
if not 'POOLFileOUT' in dir():
    POOLFileOUT = "LArMC_new.pool.root"

if not 'NTFileIN' in dir():
    NTFileIN = "LArMC.root"
    
if not 'NTFileOUT' in dir():
    NTFileOUT = "LArMC_new.root"
    
if not 'writeNTuple' in dir():
    writeNTuple  = True

if not 'OutputTag' in dir():
   OutputTag="-test"

if not 'GroupingType' in dir():
   GroupingType = "SingleGroup"

if not 'DBTag' in dir():   
   DBTag   = LArCalib_Flags.globalFlagDB

if not 'GeomTag' in dir():
   GeomTag = "ATLAS-GEO-21-00-00"

if not 'OutputDB' in dir():
    OutputDB = LArCalib_Flags.OutputDB

if 'OutputSQLiteFile' in dir():
    OutputDB = "sqlite://;schema="+OutputSQLiteFile+";dbname=OFLP200"

if 'DoRamp' not in dir():
   DoRamp=True

if 'DoADC2MEV' not in dir():
   DoADC2MEV=True

if 'DoNoise' not in dir():
   DoNoise=True

if 'DoFsampl' not in dir():
   DoFsampl=True

if 'DoMinbias' not in dir():
   DoMinbias=True

if 'DoHvscale' not in dir():
   DoHvscale=True

if 'DoAcorr' not in dir():
   DoAcorr=True

if 'DoShape' not in dir():
   DoShape=True

## Fixes....
if 'FixFsampl' not in dir():
   FixFsampl=False

###########################################################################
#
# Global settings
#
###########################################################################

include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

# this include doesn't work for MC case
## include( "LArCalibProcessing/LArCalib_MinimalSetup.py")
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.Luminosity.set_Value_and_Lock('zero')
globalflags.DataSource.set_Value_and_Lock('geant4')
globalflags.InputFormat.set_Value_and_Lock('pool')


from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = GeomTag

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOff()  #Switched off to avoid geometry
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()
#DetFlags.Print()

#Set up GeoModel (not really needed but crashes without)
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

#Get identifier mapping
include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )

###########################################################################
#                                                                         #
#    Input conditions data (CaliWave) for COOL and/or POOL                #
#                                                                         #
###########################################################################

include("AthenaPoolCnvSvc/AthenaPool_jobOptions.py")
include("LArCondAthenaPool/LArCondAthenaPool_joboptions.py")

## define the DB Gobal Tag :
svcMgr.IOVDbSvc.GlobalTag   = DBTag
svcMgr.IOVDbSvc.DBInstance=""

from IOVDbSvc.CondDB import conddb
PoolFileList     = []

if not 'DBConnectionCOOL' in dir():
   DBConnectionCOOL = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_LAR;dbname=COMP200"
conddb.addFolder("","/LAR/BadChannels/BadChannels<dbConnection>"+DBConnectionCOOL+"</dbConnection>")
conddb.addFolder("","/LAR/BadChannels/MissingFEBs<dbConnection>"+DBConnectionCOOL+"</dbConnection>")

#########################################################################################################
#
# Reading Ntuple ...
#########################################################################################################

from LArCalibTools.LArCalibToolsConf import LArSFcalFromFcalFromStdNtuple
LArSFcalFromFcalFromTuple = LArSFcalFromFcalFromStdNtuple()
LArSFcalFromFcalFromTuple.FileName =  NTFileIN 
LArSFcalFromFcalFromTuple.GroupingType = GroupingType
LArSFcalFromFcalFromTuple.DoRamp =  DoRamp 
LArSFcalFromFcalFromTuple.DoADC2MEV = DoADC2MEV  
LArSFcalFromFcalFromTuple.DoNoise = DoNoise  
LArSFcalFromFcalFromTuple.DoFsampl = DoFsampl   
LArSFcalFromFcalFromTuple.DoHvscale = DoHvscale  
LArSFcalFromFcalFromTuple.DoMinbias = DoMinbias  
LArSFcalFromFcalFromTuple.DoAcorr = DoAcorr  
LArSFcalFromFcalFromTuple.DoShape = DoShape  
LArSFcalFromFcalFromTuple.FixFsampl = FixFsampl  


topSequence += LArSFcalFromFcalFromTuple

###########################################################################
#
# Writing to Ntuple...
#
###########################################################################

if ( writeNTuple ):

  if DoRamp:  
    from LArCalibTools.LArCalibToolsConf import LArRamps2Ntuple
    LArRamps2Ntuple = LArRamps2Ntuple( "LArRamps2Ntuple" )
    LArRamps2Ntuple.ContainerKey    = ["LArRamp"]
    LArRamps2Ntuple.AddFEBTempInfo = False
    LArRamps2Ntuple.AddBadChannelInfo = False
    topSequence += LArRamps2Ntuple

  if DoADC2MEV:
    from LArCalibTools.LArCalibToolsConf import LAruA2MeV2Ntuple
    LAruA2MeV2Ntuple=LAruA2MeV2Ntuple("LAruA2MeV2Ntuple")
    LAruA2MeV2Ntuple.AddFEBTempInfo=False
    topSequence+=LAruA2MeV2Ntuple


  if DoNoise:
    from LArCalibTools.LArCalibToolsConf import LArNoise2Ntuple
    LArNoise2Ntuple=LArNoise2Ntuple("LArNoise2Ntuple")
    LArNoise2Ntuple.AddFEBTempInfo=False
    topSequence+=LArNoise2Ntuple

  if DoFsampl:
    from LArCalibTools.LArCalibToolsConf import LArfSampl2Ntuple
    LArfSampl2Ntuple=LArfSampl2Ntuple("LArfSampl2Ntuple")
    LArfSampl2Ntuple.AddFEBTempInfo=False
    topSequence+=LArfSampl2Ntuple

  if DoHvscale:
    from LArCalibTools.LArCalibToolsConf import LArHVScaleCorr2Ntuple
    theLArHVScaleCorr2Ntuple = LArHVScaleCorr2Ntuple("LArHVScaleCorr2Ntuple")
    theLArHVScaleCorr2Ntuple.AddFEBTempInfo = False
    topSequence += theLArHVScaleCorr2Ntuple

  if DoMinbias: 
    from LArCalibTools.LArCalibToolsConf import LArMinBias2Ntuple
    LArMinBias2Ntuple=LArMinBias2Ntuple("LArMinBias2Ntuple")
    LArMinBias2Ntuple.AddFEBTempInfo=False
    topSequence+=LArMinBias2Ntuple

  if DoAcorr:  
    from LArCalibTools.LArCalibToolsConf import LArAutoCorr2Ntuple
    LArAutoCorr2Ntuple=LArAutoCorr2Ntuple("LArAutoCorr2Ntuple")
    LArAutoCorr2Ntuple.AddFEBTempInfo=False
    LArAutoCorr2Ntuple.ContainerKey="LArAutoCorr"
    topSequence+=LArAutoCorr2Ntuple

  if DoShape:
    from LArCalibTools.LArCalibToolsConf import LArShape2Ntuple
    LArShape2Ntuple = LArShape2Ntuple("LArShape2Ntuple")
    LArShape2Ntuple.ContainerKey = "LArShape"
    LArShape2Ntuple.AddFEBTempInfo=False
    topSequence+=LArShape2Ntuple


  theApp.HistogramPersistency = "ROOT"
  from GaudiSvc.GaudiSvcConf import NTupleSvc
  svcMgr += NTupleSvc()
  svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+NTFileOUT+"' OPT='NEW'" ]


###########################################################################
#
# writing to POOL file...
#
###########################################################################

if ( writePOOLfile ):

    OutputObjectSpec   = []
    OutputObjectSpecTag = []

    if DoRamp:
       OutputObjectSpec += ["LArRampMC#LArRamp#/LAR/ElecCalibMC/Ramp"]
       OutputObjectSpecTag += [LArCalibFolderTag("/LAR/ElecCalibMC/Ramp",OutputTag)]

    if DoADC2MEV:
       OutputObjectSpec += ["LArDAC2uAMC#LArDAC2uA#/LAR/ElecCalibMC/DAC2uA", "LAruA2MeVMC#LAruA2MeV#/LAR/ElecCalibMC/uA2MeV"]
       OutputObjectSpecTag += [LArCalibFolderTag("/LAR/ElecCalibMC/DAC2uA",OutputTag), LArCalibFolderTag("/LAR/ElecCalibMC/uA2MeV",OutputTag)]

    if DoNoise:
       OutputObjectSpec += ["LArNoiseMC#LArNoise#/LAR/ElecCalibMC/Noise"]
       OutputObjectSpecTag += [LArCalibFolderTag("/LAR/ElecCalibMC/Noise",OutputTag)]

    if DoFsampl:
       OutputObjectSpec += ["LArfSamplMC#LArfSampl#/LAR/ElecCalibMC/fSampl"]
       OutputObjectSpecTag += [LArCalibFolderTag("/LAR/ElecCalibMC/fSampl",OutputTag)]

    if DoHvscale:
       OutputObjectSpec += ["LArHVScaleCorrComplete#LArHVScaleCorr#/LAR/ElecCalibMC/HVScaleCorr"]
       OutputObjectSpecTag += [LArCalibFolderTag("/LAR/ElecCalibMC/HVScaleCorr",OutputTag)]

    if DoMinbias:
       OutputObjectSpec += ["LArMinBiasMC#LArMinBias#/LAR/ElecCalibMC/MinBias", "LArMinBiasAverageMC#LArMinBiasAverage#/LAR/ElecCalibMC/MinBiasAverage"]
       OutputObjectSpecTag += [LArCalibFolderTag("/LAR/ElecCalibMC/MinBias",OutputTag), LArCalibFolderTag("/LAR/ElecCalibMC/MinBiasAverage",OutputTag)]

    if DoAcorr:
       OutputObjectSpec += ["LArAutoCorrMC#LArAutoCorr#/LAR/ElecCalibMC/AutoCorr"]
       OutputObjectSpecTag += [LArCalibFolderTag("/LAR/ElecCalibMC/AutoCorr",OutputTag)]

    if DoShape:
       OutputObjectSpec += ["LArShape32MC#LArShape#/LAR/ElecCalibMC/Shape"]
       OutputObjectSpecTag += [LArCalibFolderTag("/LAR/ElecCalibMC/Shape",OutputTag)]

    from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
    theOutputConditionsAlg = OutputConditionsAlg("OutputConditionsAlg",POOLFileOUT,
                                                 OutputObjectSpec,OutputObjectSpecTag,True)

    theOutputConditionsAlg.Run1 = 0

    svcMgr.IOVDbSvc.dbConnection  = OutputDB

    from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
    svcMgr += IOVRegistrationSvc()
    svcMgr.IOVRegistrationSvc.RecreateFolders = True

##########################################################################

theApp.EvtMax = 1

###########################################################################

svcMgr.MessageSvc.OutputLevel  = INFO
svcMgr.MessageSvc.defaultLimit = 10000
svcMgr.MessageSvc.Format       = "% F%20W%S%7W%R%T %0W%M"

svcMgr+=CfgMgr.AthenaEventLoopMgr(OutputLevel = INFO)

###########################################################################

