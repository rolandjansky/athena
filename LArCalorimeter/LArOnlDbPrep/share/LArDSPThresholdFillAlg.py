fill=True

import AthenaCommon.AtlasUnixGeneratorJob
## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.AppMgr import ToolSvc

from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.InputFormat.set_Value_and_Lock('bytestream')
#globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-18-01-01')
globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-20-00-01')
globalflags.DetGeo.set_Value_and_Lock('commis')
globalflags.Luminosity.set_Value_and_Lock('zero')

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

#Set up GeoModel (not really needed but crashes without)
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

svcMgr.IOVDbSvc.GlobalTag = "COMCOND-ES1PA-006-02"

#Get identifier mapping (needed by LArConditionsContainer)
include("LArConditionsCommon/LArIdMap_comm_jobOptions.py")

# for LArBadChannelTool, instead of conddb.AddFolder below
#include("LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py") 

theApp.EvtMax = 1
#conddb.setGlobalTag("COMCOND-006-01") #For id mapping

folder="/LAR/Configuration/DSPThreshold/Templates"
key="LArDSPThresholds"
#tag="LARConfigurationDSPThresholdTemplates-Qt5sigma-samp5sigma"

#ModeType="noise" # fixed, group, noise

#fileName=ModeType+"tQThr5sigmasampleThr5sigma"
fileName=ModeType+tag

from CaloTools.CaloNoiseFlags import jobproperties
jobproperties.CaloNoiseFlags.FixedLuminosity.set_Value_and_Lock(1.45*30/8)

from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
theCaloNoiseTool = CaloNoiseToolDefault()
theCaloNoiseTool.OutputLevel=INFO
ToolSvc+=theCaloNoiseTool

from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
theLArBadChannelMasker=LArBadChannelMasker("LArBadChannelMasker")
theLArBadChannelMasker.DoMasking=True
theLArBadChannelMasker.ProblemsToMask=[
    "highNoiseHG","highNoiseMG","highNoiseLG"
    ]
##    "deadReadout","deadPhys","deadCalib","short","almostDead",
ToolSvc+=theLArBadChannelMasker

from LArOnlDbPrep.LArOnlDbPrepConf import LArDSPThresholdFillAlg
theLArDSPThresholdFillAlg=LArDSPThresholdFillAlg()
theLArDSPThresholdFillAlg.OutputLevel=INFO
theLArDSPThresholdFillAlg.Key=key
theLArDSPThresholdFillAlg.OutFile=fileName+".txt"
theLArDSPThresholdFillAlg.mode=ModeType
theLArDSPThresholdFillAlg.MaskBadChannels=True
theLArDSPThresholdFillAlg.BadChannelMasker=theLArBadChannelMasker
# Set masked channel thresholds lower for diagnostics
#theLArDSPThresholdFillAlg.MaskedtQThreshold=0.
#theLArDSPThresholdFillAlg.MaskedsamplesThreshold=0.

##if ModeType=="fixed":
##    theLArDSPThresholdFillAlg.tQThreshold=10000
##    theLArDSPThresholdFillAlg.samplesThreshold=10000
if ModeType=="fixed":
    theLArDSPThresholdFillAlg.tQThreshold=Qtval
    theLArDSPThresholdFillAlg.samplesThreshold=Sampval
if ModeType=="group":
    theLArDSPThresholdFillAlg.ThresholdsPerCellGroup=["250,1000",
                                                      "[HEC,FCAL/*/*/*/*/*] 500, 1000"]
if ModeType=="noise":
    theLArDSPThresholdFillAlg.sigmaNoiseSamples=Sampval
    theLArDSPThresholdFillAlg.sigmaNoiseQt=Qtval
    theLArDSPThresholdFillAlg.usePileupNoiseSamples=Samppileup
    theLArDSPThresholdFillAlg.usePileupNoiseQt=Qtpileup
    theLArDSPThresholdFillAlg.NoiseTool=theCaloNoiseTool


if fill:
    theLArDSPThresholdFillAlg.Fill=True
    theLArDSPThresholdFillAlg.Dump=True
else:
    theLArDSPThresholdFillAlg.Fill=False
    theLArDSPThresholdFillAlg.Dump=True

topSequence+=theLArDSPThresholdFillAlg


MessageSvc = svcMgr.MessageSvc
MessageSvc.OutputLevel = WARNING

if fill:
    OutputList=[ "LArDSPThresholdsComplete#"+key+"#"+folder ]
    OutputTagList=[tag]

    WriteIOV=True
    from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
##    theOutputConditionsAlg=OutputConditionsAlg("OutputConditionsAlg","LArDSPthresholds_"+fileName+".pool.root",
    theOutputConditionsAlg=OutputConditionsAlg("OutputConditionsAlg","LArDSPthresholdTemplates.pool.root",
                                               OutputList,OutputTagList,WriteIOV)
    
    #theOutputConditionsAlg.Run1=1000


    from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
    svcMgr += IOVRegistrationSvc()
    svcMgr.IOVRegistrationSvc.OutputLevel = WARNING
    ##svcMgr.IOVRegistrationSvc.RecreateFolders = True
    svcMgr.IOVRegistrationSvc.RecreateFolders = False
else:
    conddb.addFolder("",folder+"<tag>"+tag+"</tag>")

# For LArBadChannelTool
conddb.addFolder(LArDB,"/LAR/BadChannels/BadChannels")
conddb.addFolder(LArDB,"/LAR/BadChannels/MissingFEBs")

svcMgr.PoolSvc.FileOpen = "update"
svcMgr.PoolSvc.WriteCatalog="xmlcatalog_file:PoolFileCatalog_LARConfigurationDSPThresholdTemplates.xml"
##svcMgr.PoolSvc.WriteCatalog="xmlcatalog_file:PoolFileCatalog_"+fileName+".xml"
print svcMgr.PoolSvc

#svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema=test.db;dbname=COMP200"
#svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema="+fileName+".db;dbname=COMP200"
svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema=DSPThresholdTemplates.db;dbname=COMP200"

svcMgr.DetectorStore.Dump=True
svcMgr.EventSelector.RunNumber=0xFFFFFF
