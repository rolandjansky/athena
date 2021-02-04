# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# Setup Athena
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import GlobalFlags, globalflags
	
globalflags.DetGeo.set_Value_and_Lock('atlas') 
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.DatabaseInstance.set_Value_and_Lock("CONDBR2")


from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AlgSequence import AlgSequence

import AthenaCommon.AtlasUnixStandardJob

# use auditors
from AthenaCommon.AppMgr import ServiceMgr,athCondSeq

#from GaudiSvc.GaudiSvcConf import AuditorSvc
#ServiceMgr += AuditorSvc()
#theAuditorSvc = ServiceMgr.AuditorSvc
#theAuditorSvc.Auditors  += [ "ChronoAuditor"]
#theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
#theApp.AuditAlgorithms=True

from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.SCT_setOff()
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOn()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.SCT_setOff()
DetFlags.TRT_setOff()

import AtlasGeoModel.SetGeometryVersion
import AtlasGeoModel.GeoModelInit

from AthenaCommon.JobProperties import jobproperties
from AthenaCommon.AlgSequence import AlgSequence

job = AlgSequence()




##--------------------------------------------------------------
## Data Base Services
##--------------------------------------------------------------

from AthenaCommon.AlgSequence import AthSequencer

condSequence = AthSequencer("AthCondSeq")

# Conditions Service for reading conditions data in serial and MT Athena
from IOVSvc.IOVSvcConf import CondSvc
svcMgr += CondSvc()

# Conditions data access infrastructure for serial and MT Athena
from IOVSvc.IOVSvcConf import CondInputLoader
condSequence += CondInputLoader( "CondInputLoader")

import StoreGate.StoreGateConf as StoreGateConf
svcMgr += StoreGateConf.StoreGateSvc("ConditionStore")

from IOVDbSvc.CondDB import conddb

conddb.addFolderWithTag("MDT_OFL", "/MDT/RTBLOB" , "MDTRT-RUN2-UPD4-22"     , className='CondAttrListCollection', force=True, forceData=True)
conddb.addFolderWithTag("MDT_OFL", "/MDT/RTJSONS", "MDTRTJSONS-RUN2-UPD4-22", className='CondAttrListCollection', force=True, forceData=True)
conddb.addFolderWithTag("MDT_OFL", "/MDT/T0BLOB" , "MDTT0-RUN2-UPD4-18"     , className='CondAttrListCollection', force=True, forceData=True)
conddb.addFolderWithTag("MDT_OFL", "/MDT/T0JSONS", "MDTT0JSONS-RUN2-UPD4-18", className='CondAttrListCollection', force=True, forceData=True)

#conddb.setGlobalTag("OFLCOND-MC16-SDR-20")
#conddb.addFolder("MDT_OFL", "/MDT/RTBLOB" , className='CondAttrListCollection');
#conddb.addFolder("MDT_OFL", "/MDT/RTJSONS", className='CondAttrListCollection');
#conddb.addFolder("MDT_OFL", "/MDT/T0BLOB" , className='CondAttrListCollection');
#conddb.addFolder("MDT_OFL", "/MDT/T0JSONS", className='CondAttrListCollection');


##--------------------------------------------------------------
## NEW Test Algorithm
##--------------------------------------------------------------
from MuonCondAlg.MuonCondAlgConf import MdtCalibFormatAlgTest
alg = MdtCalibFormatAlgTest()
alg.ReadKey_RT_old = "/MDT/RTBLOB"
alg.ReadKey_RT_new = "/MDT/RTJSONS"
alg.ReadKey_T0_old = "/MDT/T0BLOB"
alg.ReadKey_T0_new = "/MDT/T0JSONS"
condSequence += alg


##--------------------------------------------------------------
## General Stuff
##--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob

#ServiceMgr.EventSelector.RunNumber  = 138460 #1204110576 seconds epoch
import time, calendar
#time in seconds , now
ServiceMgr.EventSelector.InitialTimeStamp  = calendar.timegm(time.gmtime())
#ServiceMgr.EventSelector.InitialTimeStamp  =  594682#found valid in db browser?
theApp.EvtMax                              =  10

ServiceMgr.MessageSvc.Format      = "% F%40W%S%7W%R%T %0W%M"
ServiceMgr.MessageSvc.OutputLevel = VERBOSE



