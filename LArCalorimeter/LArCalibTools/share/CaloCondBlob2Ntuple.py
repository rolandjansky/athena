# you could specify 'sqlitefile' for reading the local file
# and 'tag' to specify the folder level tag to dump

if not 'Folder' in dir():
   Folder="/LAR/CellCorrOfl/EnergyCorr"

if not 'output' in dir():
   output="CellCondBlob.root"

if not 'RunNumber' in dir():
   RunNumber=1000000000

if not 'GlobalTag' in dir():
   GlobalTag='COMCOND-BLKPA-006-06'

import AthenaCommon.AtlasUnixGeneratorJob #use MC event selector

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()  

from AthenaCommon.GlobalFlags import GlobalFlags
#GlobalFlags.DetGeo.set_commis()
#GlobalFlags.Luminosity.set_zero()
#GlobalFlags.DataSource.set_data()
#GlobalFlags.InputFormat.set_bytestream()

from AthenaCommon.GlobalFlags import  globalflags
globalflags.DataSource="data"
globalflags.InputFormat="bytestream"


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
svcMgr.IOVDbSvc.GlobalTag=GlobalTag
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )
#include("LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")

theApp.EvtMax = 1
svcMgr.EventSelector.RunNumber = RunNumber


from LArCalibTools.LArCalibToolsConf import CaloCondBlob2Ntuple
theCaloCondBlobs2Ntuple=CaloCondBlob2Ntuple()
theCaloCondBlobs2Ntuple.Folder=Folder
theCaloCondBlobs2Ntuple.AddFEBTempInfo=False
theCaloCondBlobs2Ntuple.AddBadChannelInfo=False
topSequence+=theCaloCondBlobs2Ntuple

if 'tag' in dir():
   tagstring="<tag>"+tag+"</tag>"
else:   
   tagstring=""     
if 'sqlitefile' in dir():
   conddb.addFolder("",Folder+"<db>sqlite://;schema="+sqlitefile+";dbname=COMP200</db>"+tagstring)
else:   
   conddb.addFolder('LAR_ONL',Folder+tagstring)


theApp.HistogramPersistency = "ROOT"
from GaudiSvc.GaudiSvcConf import NTupleSvc
svcMgr += NTupleSvc()
svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+output+"' OPT='NEW'" ]


#from GaudiSvc.GaudiSvcConf import THistSvc
#svcMgr+=THistSvc()
#svcMgr.THistSvc.Output =  [ "FILE1 DATAFILE='CellGroup.root' OPT='RECREATE'" ]

#svcMgr.DetectorStore.Dump=True
#svcMgr.MessageSvc.OutputLevel = DEBUG

