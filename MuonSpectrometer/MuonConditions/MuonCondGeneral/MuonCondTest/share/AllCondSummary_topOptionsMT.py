# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

run  = "mc" ## "mc" or "dataR1" or "dataR2"
what = "mdt" ## "mdt" or "csc" or "rpc" or "tgc"

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


## MDT 
if what=="mdt":
	if run=="mc":
		conddb.setGlobalTag("OFLCOND-MC16-SDR-20")
		conddb.addFolder("DCS_OFL", "/MDT/DCS/DROPPEDCH"        , className='CondAttrListCollection');
		conddb.addFolder("DCS_OFL", "/MDT/DCS/PSLVCHSTATE"      , className='CondAttrListCollection');
		#conddb.addFolder("MDT_OFL", "/MDT/DQMF/DEAD_ELEMENT"    , className='CondAttrListCollection');
		#conddb.addFolder("MDT_OFL", "/MDT/TUBE_STATUS/DEAD_TUBE", className='CondAttrListCollection');
	elif run=="dataR1":
		conddb.setGlobalTag("COMCOND-BLKPA-RUN1-09")
		conddb.addFolder("DCS_OFL", "/MDT/DCS/DROPPEDCH"        , forceData=True, className='CondAttrListCollection');
		conddb.addFolder("DCS_OFL", "/MDT/DCS/PSLVCHSTATE"      , forceData=True, className='CondAttrListCollection');
		conddb.addFolder("DCS_OFL", "/MDT/DCS/PSHVMLSTATE"      , forceData=True, className='CondAttrListCollection');
		conddb.addFolder("DCS_OFL", "/MDT/DCS/PSV0SETPOINTS"    , forceData=True, className='CondAttrListCollection');
		conddb.addFolder("DCS_OFL", "/MDT/DCS/PSV1SETPOINTS"    , forceData=True, className='CondAttrListCollection');
	elif run=="dataR2":
		conddb.setGlobalTag("CONDBR2-BLKPA-2018-15")
		conddb.addFolder("DCS_OFL", "/MDT/DCS/LV"               , forceData=True, className='CondAttrListCollection');
		conddb.addFolder("DCS_OFL", "/MDT/DCS/HV"               , forceData=True, className='CondAttrListCollection');


## CSC
if what=="csc":
	if run=="dataR1":
		conddb.setGlobalTag("COMCOND-BLKPA-RUN1-09")
		conddb.addFolder("CSC_OFL", "/CSC/STAT"          , className='CondAttrListCollection');
		#conddb.addFolder("DCS_OFL", "/CSC/DCS/LAYERSTATE", className='CondAttrListCollection');


## RPC
if what=="rpc":
	if run=="mc":
		conddb.setGlobalTag("OFLCOND-MC16-SDR-20")
		conddb.addFolder("DCS_OFL", "/RPC/DCS/DeadRopanels"   , className='CondAttrListCollection'); 
		conddb.addFolder("DCS_OFL", "/RPC/DCS/OffRopanels"    , className='CondAttrListCollection'); 
	if run=="dataR1":
		conddb.setGlobalTag("COMCOND-BLKPA-RUN1-09")
		conddb.addFolder("RPC_OFL", "/RPC/DQMF/ELEMENT_STATUS", className='CondAttrListCollection'); 
		conddb.addFolder("DCS_OFL", "/RPC/DCS/DeadRopanels"   , className='CondAttrListCollection'); 
		conddb.addFolder("DCS_OFL", "/RPC/DCS/OffRopanels"    , className='CondAttrListCollection'); 


## TGC
if what=="tgc":
	if run=="dataR1":
		#conddb.setGlobalTag("COMCOND-BLKPA-RUN1-09")
		#conddb.addFolder("DCS_OFL", "/TGC/1/DetectorStatus"   , className='CondAttrListCollection'); 
		## this folder is actually scheme ONLINE! DO NOT USE!
		pass





##--------------------------------------------------------------
## NEW Data Base Algorithms
##--------------------------------------------------------------

from AthenaCommon.CfgGetter import getPublicTool

## MDT
if what=="mdt":
	from MuonCondAlg.MuonCondAlgConf import MdtCondDbAlg
	alg = MdtCondDbAlg("MdtCondDbAlg")
	if run=="dataR1": alg.useRun1SetPoints = False

## CSC
if what=="csc":
	from MuonCondAlg.MuonCondAlgConf import CscCondDbAlg
	alg = CscCondDbAlg("CscCondDbAlg")

## RPC
if what=="rpc":
	from MuonCondAlg.MuonCondAlgConf import RpcCondDbAlg
	alg = RpcCondDbAlg("RpcCondDbAlg")

## TGC
if what=="tgc":
	from MuonCondAlg.MuonCondAlgConf import TgcCondDbAlg
	alg = TgcCondDbAlg("CscCondDbAlg")

## append to sequence
if run=="mc":
	alg.isData = False
elif run=="dataR1":
	alg.isData = True
	alg.isRun1 = True
elif run=="dataR2":
	alg.isData = True
	alg.isRun1 = False
#alg.OutputLevel = DEBUG
condSequence += alg



##--------------------------------------------------------------
## NEW Test Algorithm
##--------------------------------------------------------------
if what=="mdt":
	from MuonCondTest.MuonCondTestConf import MDTConditionsTestAlgMT
	job += MDTConditionsTestAlgMT()



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



