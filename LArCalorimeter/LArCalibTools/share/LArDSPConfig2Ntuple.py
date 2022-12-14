if 'inputfolder' not in dir():
   inputfolder="/LAR/Configuration/DSPConfiguration"

if 'inputtag' not in dir():
   inputtag=""

if 'inputdb' not in dir():
   inputdb="COOLONL_LAR/CONDBR2"

if 'RunNumber' not in dir():
   RunNumber=9999999

if 'OutputRootFile' not in dir():
   OutputRootFile="DSPConfig.root"

import AthenaCommon.AtlasUnixGeneratorJob #use MC event selector
from time import time
## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()  


from AthenaCommon.GlobalFlags import globalflags
globalflags.Luminosity.set_Value_and_Lock('zero')
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.InputFormat.set_Value_and_Lock('bytestream')
globalflags.DatabaseInstance.set_Value_and_Lock("CONDBR2")
from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-R2-2016-01-00-01"

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
                           
svcMgr.IOVDbSvc.GlobalTag="LARCALIB-RUN2-00"
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )

theApp.EvtMax = 1
svcMgr.EventSelector.RunNumber = RunNumber
svcMgr.EventSelector.InitialTimeStamp=int(time())
dbname="<db>COOLOFL_LAR/CONDBR2</db>"

from LArBadChannelTool.LArBadChannelAccess import LArBadChannelAccess
LArBadChannelAccess()
conddb.addFolder("",inputfolder+"<db>"+inputdb+"</db>")


## for f in LArFebConfigFolders:
##     conddb.addFolder("LAR_ONL",f)


from LArCalibTools.LArCalibToolsConf import LArDSPConfig2Ntuple
theLArDSPConfig2Ntuple=LArDSPConfig2Ntuple()
theLArDSPConfig2Ntuple.OutputLevel=DEBUG
theLArDSPConfig2Ntuple.AddFEBTempInfo=False
theLArDSPConfig2Ntuple.OffId=True
theLArDSPConfig2Ntuple.Folder=inputfolder

topSequence+=theLArDSPConfig2Ntuple


theApp.HistogramPersistency = "ROOT"
from GaudiSvc.GaudiSvcConf import NTupleSvc
svcMgr += NTupleSvc()
svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+OutputRootFile+"' OPT='NEW'" ]

#svcMgr.DetectorStore.Dump=True
svcMgr.MessageSvc.OutputLevel = INFO

svcMgr.IOVDbSvc.DBInstance=""
svcMgr.PoolSvc.ReadCatalog += ["xmlcatalog_file:/afs/cern.ch/user/l/larcalib/w0/stableConds/PoolCat_stable.xml",
                               "xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/poolcond/PoolCat_comcond_castor.xml",
                               ]


svcMgr.MessageSvc.debugLimit = 5000000

svcMgr.IOVDbSvc.OutputLevel=DEBUG
