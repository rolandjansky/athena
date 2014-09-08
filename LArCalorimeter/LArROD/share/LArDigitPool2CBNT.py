from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_commis()
GlobalFlags.Luminosity.set_zero()
GlobalFlags.DataSource.set_data()
GlobalFlags.InputFormat.set_pool()

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-Comm-00-00-00"

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOff()  #Switched off to avoid geometry
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()
#DetFlags.Print()

#Set up GeoModel (not really needed but crashes without)
#from AtlasGeoModel import SetGeometryVersion
#from AtlasGeoModel import GeoModelInit

DetDescrVersion="ATLAS-CommNF-03-00-00"

include ("AtlasGeoModel/SetGeometryVersion.py")
GeoModelSvc = Service( "GeoModelSvc" )
include ("AtlasGeoModel/GeoModelInit.py")
#GeoModelSvc.IgnoreTagDifference = True

#Get identifier mapping
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )
include( "LArIdCnv/LArIdCnv_joboptions.py" )
#import ServiceMgr
from AthenaCommon.AppMgr import ServiceMgr

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.AthenaPoolCnvSvc.PoolAttributes = [ "DEFAULT_BUFFERSIZE = '2048'" ]
ServiceMgr.EventSelector.InputCollections = [ "cosmicsESD.pool.root" ]

include( "CBNT_Athena/CBNT_AthenaAware_jobOptions.py" )
include( "CBNT_Athena/CBNT_EventInfo_jobOptions.py" )

if not hasattr(svcMgr,"THistSvc"):
    from GaudiSvc.GaudiSvcConf import THistSvc
    svcMgr+=THistSvc()

svcMgr.THistSvc.Output += ["AANT DATAFILE='Digits.root' OPT='RECREATE'"]
theApp.TopAlg += [ "AANTupleStream" ]
theApp.Dlls   += [ "AnalysisTools" ]
AANTupleStream = Algorithm( "AANTupleStream" )
AANTupleStream.ExtraRefNames = [ "" ]
AANTupleStream.OutputName = "Digits.root"
AANTupleStream.ExistDataHeader = False


from LArROD.LArRODConf import CBNTAA_LArDigits
theCBNTAA_LArDigits = CBNTAA_LArDigits("CBNTAA_LArDigits")
theCBNTAA_LArDigits.MaxChannels = 200000
theCBNTAA_LArDigits.SaveId=True
theCBNTAA_LArDigits.SaveSCAAddress = False
theCBNTAA_LArDigits.DumpCut=0
theCBNTAA_LArDigits.ContainerKey="HIGH"
CBNT_AthenaAware +=theCBNTAA_LArDigits

EventSelector.MaxEvt=2
