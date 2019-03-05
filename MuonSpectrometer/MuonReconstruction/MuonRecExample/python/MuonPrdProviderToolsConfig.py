# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Include import include

from AthenaCommon.GlobalFlags import globalflags

source = globalflags.DataSource()

from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.CfgGetter import getService
from RecExConfig.RecAlgsFlags import recAlgs


def RpcPrepDataProviderTool(name="RpcPrepDataProviderTool",**kwargs):
  global source

  # setup dependencies which are not yet in C++
  import MuonCnvExample.MuonCablingConfig

  # If we run trigger from BS, then the RDO to PRD tools need to automatically run BS to RDO tools
  kwargs.setdefault("useBStoRdoTool", DetFlags.readRDOBS.RPC_on() and recAlgs.doTrigger())
  kwargs.setdefault("RawDataProviderTool", "RpcRawDataProviderTool")
### TODO: in C++ do not retrieve tool if useBStoRdoTool==False
##  if not kwargs["useBStoRdoTool"]:
##    kwargs["RawDataProviderTool"] = None # empty tool
    
  if source == 'data':
    kwargs["processingData"] = True
    kwargs["reduceCablingOverlap"] = True
    kwargs["produceRpcCoinDatafromTriggerWords"] = True
    kwargs["overlap_timeTolerance"] = 1000
    kwargs["solvePhiAmbiguities"] = True
    kwargs["etaphi_coincidenceTime"] = 1000
  elif source == 'geant4':
    pass
  #kwargs["etaphi_coincidenceTime"] = 100
  else:
    raise ValueError( "RpcPrepDataProviderTool: unsupported dataSource %s" % source )

  from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcRdoToPrepDataTool
  return Muon__RpcRdoToPrepDataTool(name,**kwargs)


def MdtPrepDataProviderTool(name="MdtPrepDataProviderTool", **kwargs):
  global source,include,getService

  # setup dependencies which are not yet in C++
  import MuonCnvExample.MuonCablingConfig
  getService("MdtCalibrationSvc")
  include("AmdcAth/AmdcAth_jobOptions.py")
  
  # If we run trigger from BS, then the RDO to PRD tools need to automatically run BS to RDO tools
  kwargs.setdefault("useBStoRdoTool", DetFlags.readRDOBS.MDT_on() and recAlgs.doTrigger())
  kwargs.setdefault("RawDataProviderTool", "MdtRawDataProviderTool")
### TODO: in C++ do not retrieve tool if useBStoRdoTool==False
##  if not kwargs["useBStoRdoTool"]:
##    kwargs["RawDataProviderTool"] = None # empty tool

  if source == 'data':
    kwargs.setdefault("UseTwin", True)

  from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import Muon__MdtRdoToPrepDataTool
  return Muon__MdtRdoToPrepDataTool(name,**kwargs)


def TgcPrepDataProviderTool(name="TgcPrepDataProviderTool", **kwargs):
  # setup dependencies which are not yet in C++  
  import MuonCnvExample.MuonCablingConfig

  # If we run trigger from BS, then the RDO to PRD tools need to automatically run BS to RDO tools
  kwargs.setdefault("useBStoRdoTool", DetFlags.readRDOBS.TGC_on() and recAlgs.doTrigger())
  kwargs.setdefault("RawDataProviderTool", "TgcRawDataProviderTool")
### TODO: in C++ do not retrieve tool if useBStoRdoTool==False
##  if not kwargs["useBStoRdoTool"]:
##    kwargs["RawDataProviderTool"] = None # empty tool

  from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TgcRdoToPrepDataTool
  return Muon__TgcRdoToPrepDataTool(name, **kwargs)
  
  
def CscPrepDataProviderTool(name="CscPrepDataProviderTool", **kwargs):
  # setup dependencies which are not yet in C++
  import MuonCnvExample.MuonCablingConfig

  # If we run trigger from BS, then the RDO to PRD tools need to automatically run BS to RDO tools
  kwargs.setdefault("useBStoRdoTool", DetFlags.readRDOBS.CSC_on() and recAlgs.doTrigger())
  kwargs.setdefault("RawDataProviderTool", "CscRawDataProviderTool")
### TODO: in C++ do not retrieve tool if useBStoRdoTool==False
##  if not kwargs["useBStoRdoTool"]:
##    kwargs["RawDataProviderTool"] = None # empty tool

  from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CscRdoToCscPrepDataTool
  return Muon__CscRdoToCscPrepDataTool(name, **kwargs)


################################################################################
# Also define the algorithms factory functions here
# Only CSC one is needed, because the C++ default is wrong
################################################################################

def CscRdoToCscPrepData(name="CscRdoToCscPrepData", **kwargs):
  # TODO: in C++ change default tool to the one set below. Then this whole function can be removed
  kwargs.setdefault("CscRdoToCscPrepDataTool", "CscPrepDataProviderTool")

  from MuonRdoToPrepData.MuonRdoToPrepDataConf import CscRdoToCscPrepData as Muon__CscRdoToCscPrepData
  return Muon__CscRdoToCscPrepData(name, **kwargs)

#############################################################################
def MM_PrepDataProviderTool(name="MM_PrepDataProviderTool", **kwargs): 
  kwargs.setdefault("ClusterBuilderTool","SimpleMMClusterBuilderTool")
  return Muon__MmRdoToPrepDataTool(name,**kwargs)

def STGC_PrepDataProviderTool(name="STGC_PrepDataProviderTool", **kwargs): 
  kwargs.setdefault("ClusterBuilderTool","SimpleSTgcClusterBuilderTool")
  return Muon__sTgcRdoToPrepDataTool(name,**kwargs)

### algorithms for other technologies can use C++ defaults


### TODO: remove following backwards compat as soon as all clients have migrated to using CfgGetter
from AthenaCommon.CfgGetter import getPrivateTool,getPrivateToolClone,getPublicTool,getPublicToolClone,getService,getServiceClone

if DetFlags.haveRDO.CSC_on() or DetFlags.digitize.CSC_on():
  CscRdoToPrepDataTool = getPublicTool("CscPrepDataProviderTool")

if DetFlags.haveRDO.MDT_on() or DetFlags.digitize.MDT_on():
  MdtRdoToPrepDataTool = getPublicTool("MdtPrepDataProviderTool")

if DetFlags.haveRDO.RPC_on() or DetFlags.digitize.RPC_on():
  RpcRdoToPrepDataTool = getPublicTool("RpcPrepDataProviderTool")

if DetFlags.haveRDO.TGC_on() or DetFlags.digitize.TGC_on():
  TgcRdoToPrepDataTool = getPublicTool("TgcPrepDataProviderTool")
