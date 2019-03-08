import AthenaCommon.AtlasUnixGeneratorJob #use MC event selector
from string import split,join
from time import time
## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()  


from AthenaCommon.GlobalFlags import globalflags
globalflags.Luminosity.set_Value_and_Lock('zero')
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.InputFormat.set_Value_and_Lock('bytestream')

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
                           
svcMgr.IOVDbSvc.GlobalTag="LARCALIB-RUN2-02"
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )

theApp.EvtMax = 1
svcMgr.EventSelector.RunNumber = 500000
svcMgr.EventSelector.InitialTimeStamp=int(time())
dbname="<db>COOLOFL_LAR/CONDBR2</db>"

#conddb.addFolder("","/LAR/BadChannelsOfl/BadChannels<key>/LAR/BadChannels/BadChannels</key>"+dbname)
#conddb.addFolder("","/LAR/BadChannelsOfl/MissingFEBs<key>/LAR/BadChannels/MissingFEBs</key>"+dbname)
from LArBadChannelTool.LArBadChannelAccess import LArBadChannelAccess
LArBadChannelAccess()
#from LArBadChannelTool.LArBadFebAccess import LArBadFebAccess
#LArBadFebAccess()

from LArRecUtils.LArFebConfigCondAlgDefault import LArFebConfigCondAlgDefault
LArFebConfigCondAlgDefault()

from LArCalibTools.LArCalibToolsConf import LArGainThresholds2Ntuple
theLArGainThresholds2Ntuple=LArGainThresholds2Ntuple()
theLArGainThresholds2Ntuple.OutputLevel=DEBUG
theLArGainThresholds2Ntuple.AddFEBTempInfo=False
theLArGainThresholds2Ntuple.isSC=False

topSequence+=theLArGainThresholds2Ntuple


theApp.HistogramPersistency = "ROOT"
from GaudiSvc.GaudiSvcConf import NTupleSvc
svcMgr += NTupleSvc()
svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='thresholds.root' OPT='NEW'" ]

#svcMgr.DetectorStore.Dump=True
#svcMgr.MessageSvc.OutputLevel = DEBUG

svcMgr.IOVDbSvc.DBInstance=""
#svcMgr.PoolSvc.ReadCatalog += ["xmlcatalog_file:/afs/cern.ch/user/l/larcalib/w0/stableConds/PoolCat_stable.xml",
#                               "xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/poolcond/PoolCat_comcond_castor.xml",
#                               ]


svcMgr.MessageSvc.debugLimit = 5000000
svcMgr.IOVDbSvc.OutputLevel=DEBUG
