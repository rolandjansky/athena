# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

def RpcPrepDataProviderTool(name="RpcPrepDataProviderTool",**kwargs):
  # setup dependencies which are not yet in C++
  import MuonCnvExample.MuonCablingConfig  # noqa: F401

  if globalflags.DataSource.is_data():
    kwargs["reduceCablingOverlap"] = True
    kwargs["produceRpcCoinDatafromTriggerWords"] = True
    kwargs["overlap_timeTolerance"] = 1000
    kwargs["solvePhiAmbiguities"] = True
    kwargs["etaphi_coincidenceTime"] = 1000
  elif globalflags.DataSource.is_geant4():
    pass
  #kwargs["etaphi_coincidenceTime"] = 100
  else:
    raise ValueError( "RpcPrepDataProviderTool: unsupported dataSource %s" % globalflags.DataSource() )

  from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcRdoToPrepDataToolMT
  if athenaCommonFlags.isOnline:
      kwargs["ReadKey"] = ""
  return Muon__RpcRdoToPrepDataToolMT(name,**kwargs)


def MdtPrepDataProviderTool(name="MdtPrepDataProviderTool", **kwargs):
  # setup dependencies which are not yet in C++
  import MuonCnvExample.MuonCablingConfig     # noqa: F401
  from MuonRecExample import MuonAlignConfig  # noqa: F401
  from MuonCnvExample import MuonCalibConfig  # noqa: F401
  from AthenaCommon.Include import include
  MuonCalibConfig.setupMdtCondDB()
  include("AmdcAth/AmdcAth_jobOptions.py")

  if globalflags.DataSource.is_data():
    kwargs.setdefault("UseTwin", True)
  kwargs.setdefault("CalibrationTool", MuonCalibConfig.MdtCalibrationTool())

  from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import Muon__MdtRdoToPrepDataToolMT
  return Muon__MdtRdoToPrepDataToolMT(name,**kwargs)


def TgcPrepDataProviderTool(name="TgcPrepDataProviderTool", **kwargs):
  # setup dependencies which are not yet in C++
  import MuonCnvExample.MuonCablingConfig  # noqa: F401

  from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TgcRdoToPrepDataToolMT
  return Muon__TgcRdoToPrepDataToolMT(name, **kwargs)


def CscPrepDataProviderTool(name="CscPrepDataProviderTool", **kwargs):
  # setup dependencies which are not yet in C++
  import MuonCnvExample.MuonCablingConfig  # noqa: F401

  from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CscRdoToCscPrepDataToolMT
  return Muon__CscRdoToCscPrepDataToolMT(name, **kwargs)


################################################################################
# Also define the algorithms factory functions here
# Only CSC one is needed, because the C++ default is wrong
################################################################################

def CscRdoToCscPrepData(name="CscRdoToCscPrepData", **kwargs):
  # TODO: in C++ change default tool to the one set below. Then this whole function can be removed
  # kwargs.setdefault("CscRdoToCscPrepDataTool", "CscPrepDataProviderTool")

  from MuonRdoToPrepData.MuonRdoToPrepDataConf import CscRdoToCscPrepData as Muon__CscRdoToCscPrepData
  return Muon__CscRdoToCscPrepData(name, **kwargs)

#############################################################################
def MM_PrepDataProviderTool(name="MM_PrepDataProviderTool", **kwargs):
  kwargs.setdefault("ClusterBuilderTool","SimpleMMClusterBuilderTool")
  kwargs.setdefault("NSWCalibTool","NSWCalibTool")
  from MuonMM_CnvTools.MuonMM_CnvToolsConf import Muon__MmRdoToPrepDataToolMT
  return Muon__MmRdoToPrepDataToolMT(name,**kwargs)

def STGC_PrepDataProviderTool(name="STGC_PrepDataProviderTool", **kwargs):
  kwargs.setdefault("ClusterBuilderTool","SimpleSTgcClusterBuilderTool")
  from MuonSTGC_CnvTools.MuonSTGC_CnvToolsConf import Muon__sTgcRdoToPrepDataToolMT
  return Muon__sTgcRdoToPrepDataToolMT(name,**kwargs)

### algorithms for other technologies can use C++ defaults
