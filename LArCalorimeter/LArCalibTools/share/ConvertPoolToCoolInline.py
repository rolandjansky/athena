import AthenaCommon.AtlasUnixGeneratorJob #use MC event selector
from string import split,join
## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()  


from AthenaCommon.GlobalFlags import  globalflags
globalflags.DataSource="data"
globalflags.InputFormat="bytestream"
globalflags.DatabaseInstance="CONDBR2"

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-R2-2015-04-00-00"

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOff()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

#Set up GeoModel (not really needed but crashes without)
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit 

#Get identifier mapping (needed by LArConditionsContainer)
svcMgr.IOVDbSvc.GlobalTag="COMCOND-BLKPA-006-07"
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )

theApp.EvtMax = 1
#svcMgr.EventSelector.RunNumber = 210308
svcMgr.EventSelector.RunNumber = 2147483647
conddb.addFolder("LAR_OFL","/LAR/BadChannelsOfl/BadChannels<key>/LAR/BadChannels/BadChannels</key>")
conddb.addFolder("LAR_OFL","/LAR/BadChannelsOfl/MissingFEBs<key>/LAR/BadChannels/MissingFEBs</key>")


conddb.addFolder("","/LAR/ElecCalibOnl/uA2MeV<db>COOLONL_LAR/COMP200</db>")
conddb.addFolder("","/LAR/ElecCalibOnl/DAC2uA<db>COOLONL_LAR/COMP200</db>")
conddb.addFolder("","/LAR/ElecCalibOnl/MphysOverMcal<db>COOLONL_LAR/COMP200</db>")
conddb.addFolder("","/LAR/ElecCalibOnl/HVScaleCorr<db>COOLONL_LAR/COMP200</db>")
conddb.addFolder("","/LAR/ElecCalibOnl/Pedestal<db>COOLONL_LAR/COMP200</db>")
conddb.addFolder("","/LAR/ElecCalibOnl/Ramp<db>COOLONL_LAR/COMP200</db>")
conddb.addFolder("","/LAR/ElecCalibOnl/OFC<db>COOLONL_LAR/COMP200</db>")
conddb.addFolder("","/LAR/ElecCalibOnl/Shape<db>COOLONL_LAR/COMP200</db>")


from LArCalibTools.LArCalibToolsConf import LArCompleteToFlat
theLArCompleteToFlat=LArCompleteToFlat()

theLArCompleteToFlat.uA2MeVInput="LAruA2MeV"
theLArCompleteToFlat.DAC2uAVInput="LArDAC2uA"
theLArCompleteToFlat.HVScaleCorrInput="LArHVScaleCorr"
theLArCompleteToFlat.PedestalInput="Pedestal"
theLArCompleteToFlat.RampInput="LArRamp"
theLArCompleteToFlat.MphysOverMcalInput="LArMphysOverMcal"
theLArCompleteToFlat.OFCInput="LArOFC"
theLArCompleteToFlat.ShapeInput="LArShape"
#theLArCompleteToFlat.DSPThresholdsInput="LArDSPThresholds"

topSequence+=theLArCompleteToFlat


## from LArCalibTools.LArCalibToolsConf import LArPedestals2Ntuple
## LArPedestals2Ntuple=LArPedestals2Ntuple("LArPedestals2Ntuple")
## LArPedestals2Ntuple.AddFEBTempInfo=False
## LArPedestals2Ntuple.ContainerKey="PedestalFlat"
## topSequence+=LArPedestals2Ntuple

## from LArCalibTools.LArCalibToolsConf import LArOFC2Ntuple
## LArOFCs2Ntuple=LArOFC2Ntuple("LArOFCs2Ntuple")
## LArOFCs2Ntuple.AddFEBTempInfo=False
## LArOFCs2Ntuple.ContainerKey="OFCFlat"
## topSequence+=LArOFCs2Ntuple

 



svcMgr.PoolSvc.ReadCatalog += ['xmlcatalog_file:'+'/afs/cern.ch/atlas/conditions/poolcond/catalogue/poolcond/PoolCat_comcond_castor.xml']


theApp.HistogramPersistency = "ROOT"
from GaudiSvc.GaudiSvcConf import NTupleSvc
svcMgr += NTupleSvc()
svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='flatPedestalOFC.root' OPT='NEW'" ]


folderlist=("/LAR/ElecCalibFlat/uA2MeV",
            "/LAR/ElecCalibFlat/DAC2uA",
            "/LAR/ElecCalibFlat/MphysOverMcal",
            "/LAR/ElecCalibFlat/HVScaleCorr",
            "/LAR/ElecCalibFlat/Pedestal",
            "/LAR/ElecCalibFlat/Ramp",
            "/LAR/ElecCalibFlat/OFC",
            "/LAR/ElecCalibFlat/Shape",
            )

folderspec=[]
tagspec=[]
for f in folderlist:
    folderspec.append("CondAttrListCollection#"+f)
    tagspec.append("".join(f.split("/"))+"-00")
    pass


from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
OutputConditionsAlg=OutputConditionsAlg("OutputConditionsAlg","dummy.pool.root",
                                        #["CondAttrListCollection#/LAR/ElecCalibFlat/Pedestal","CondAttrListCollection#/LAR/ElecCalibFlat/OFC",],
                                        #["testped","testofc"],
                                        folderspec,[],
                                        True)


from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
svcMgr += IOVRegistrationSvc()
svcMgr.IOVRegistrationSvc.OutputLevel = DEBUG
svcMgr.IOVRegistrationSvc.RecreateFolders = True
svcMgr.IOVRegistrationSvc.SVFolder = True
svcMgr.IOVRegistrationSvc.userTags = False

svcMgr.IOVRegistrationSvc.OverrideNames += [ "Pedestal","PedestalRMS","OFCa"   ,"OFCb"    ,"TimeOffset","RampVec","Shape","ShapeDer","HVScaleCorr","MphysOverMcal","DAC2uA","uA2MeV"]
types=[]
for i in range(len(svcMgr.IOVRegistrationSvc.OverrideNames)):
    types.append("Blob16M");
svcMgr.IOVRegistrationSvc.OverrideTypes += types;
svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema=allConstantsSV_test.db;dbname=CONDBR2"


#svcMgr.DetectorStore.Dump=True
svcMgr.MessageSvc.OutputLevel = INFO

svcMgr.IOVDbSvc.DBInstance=""
