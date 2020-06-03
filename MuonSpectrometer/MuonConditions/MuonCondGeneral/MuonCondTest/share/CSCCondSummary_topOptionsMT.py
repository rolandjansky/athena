# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

run = "mc" ## "mc" or "dataR1" or "dataR2"

from AthenaCommon.GlobalFlags import GlobalFlags, globalflags
if run == "dataR1":
	globalflags.DetGeo.set_Value_and_Lock('atlas') 
	globalflags.DataSource.set_Value_and_Lock('data')
	globalflags.DatabaseInstance.set_Value_and_Lock("COMP200")
if run == "dataR2":
	globalflags.DetGeo.set_Value_and_Lock('atlas') 
	globalflags.DataSource.set_Value_and_Lock('data')
	globalflags.DatabaseInstance.set_Value_and_Lock("CONDBR2")




#--------------------------------------------------------------
# Setup Athena
#--------------------------------------------------------------

from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AlgSequence import AlgSequence

import AthenaCommon.AtlasUnixStandardJob

# use auditors
from AthenaCommon.AppMgr import ServiceMgr,athCondSeq

from GaudiSvc.GaudiSvcConf import AuditorSvc

ServiceMgr += AuditorSvc()
theAuditorSvc = ServiceMgr.AuditorSvc
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
theApp.AuditAlgorithms=True

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

if run=="mc":
	conddb.setGlobalTag("OFLCOND-MC16-SDR-20")
elif run=="dataR1":
	conddb.setGlobalTag("COMCOND-BLKPA-RUN1-09")
elif run=="dataR2":
	conddb.setGlobalTag("CONDBR2-BLKPA-2018-15")

conddb.addFolder("CSC_OFL", "/CSC/FTHOLD" , className='CondAttrListCollection');
conddb.addFolder("CSC_OFL", "/CSC/NOISE"  , className='CondAttrListCollection');
conddb.addFolder("CSC_OFL", "/CSC/PED"    , className='CondAttrListCollection');
conddb.addFolder("CSC_OFL", "/CSC/PSLOPE" , className='CondAttrListCollection');
conddb.addFolder("CSC_OFL", "/CSC/RMS"    , className='CondAttrListCollection');
conddb.addFolder("CSC_OFL", "/CSC/STAT"   , className='CondAttrListCollection');
conddb.addFolder("CSC_OFL", "/CSC/T0BASE" , className='CondAttrListCollection');
conddb.addFolder("CSC_OFL", "/CSC/T0PHASE", className='CondAttrListCollection');






##--------------------------------------------------------------
## NEW Data Base Algorithms
##--------------------------------------------------------------

from MuonCondAlg.MuonCondAlgConf import CscCondDbAlg
alg = CscCondDbAlg("CscCondDbAlg")

if "mc" in run:
	alg.isData = False
elif run=="dataR1":
	alg.isData           = True
	alg.isRun1           = True
elif run=="dataR2":
	alg.isData = True
	alg.isRun1 = False

#alg.OutputLevel = DEBUG
condSequence += alg



##--------------------------------------------------------------
## NEW Test Algorithm
##--------------------------------------------------------------
from MuonCondTest.MuonCondTestConf import CSCConditionsTestAlgMT
job += CSCConditionsTestAlgMT()



##--------------------------------------------------------------
## General Stuff
##--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob

ServiceMgr.EventSelector.RunNumber  = 138460 #1204110576 seconds epoch
import time, calendar
#time in seconds , now
ServiceMgr.EventSelector.InitialTimeStamp  = calendar.timegm(time.gmtime())
#ServiceMgr.EventSelector.InitialTimeStamp  =  594682#found valid in db browser?
theApp.EvtMax                              =  2 

ServiceMgr.MessageSvc.Format      = "% F%40W%S%7W%R%T %0W%M"
ServiceMgr.MessageSvc.OutputLevel = VERBOSE



