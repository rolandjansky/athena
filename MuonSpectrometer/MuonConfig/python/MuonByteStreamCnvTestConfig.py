"""Define ComponentAccumulator functions for configuration of muon data conversions

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from MuonByteStreamCnvTest.MuonByteStreamCnvTestConf import (
    MdtRdoToMdtDigit, RpcRdoToRpcDigit, TgcRdoToTgcDigit, STGC_RdoToDigit, MM_RdoToDigit, 
    MdtDigitToMdtRDO, RpcDigitToRpcRDO, TgcDigitToTgcRDO, STGC_DigitToRDO, MM_DigitToRDO, CscDigitToCscRDO
)
from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import CscDigitToCscRDOTool
from MuonConfig.MuonCalibConfig import CscCalibToolCfg

def MdtRdoToMdtDigitCfg(flags, name="MdtRdoToMdtDigitAlg", **kwargs):
    """Return ComponentAccumulator with configured MdtRdoToMdtDigit algorithm"""
    acc = ComponentAccumulator()
    if flags.Detector.Overlay:
        kwargs.setdefault("MdtRdoContainer", flags.Overlay.BkgPrefix + "MDTCSM")
        kwargs.setdefault("MdtDigitContainer", flags.Overlay.BkgPrefix + "MDT_DIGITS")
    acc.addEventAlgo(MdtRdoToMdtDigit(name, **kwargs))
    return acc


def RpcRdoToRpcDigitCfg(flags, name="RpcRdoToRpcDigitAlg", **kwargs):
    """Return ComponentAccumulator with configured RpcRdoToRpcDigit algorithm"""
    acc = ComponentAccumulator()
    if flags.Detector.Overlay:
        kwargs.setdefault("RpcRdoContainer", flags.Overlay.BkgPrefix + "RPCPAD")
        kwargs.setdefault("RpcDigitContainer", flags.Overlay.BkgPrefix + "RPC_DIGITS")
    acc.addEventAlgo(RpcRdoToRpcDigit(name, **kwargs))
    return acc


def TgcRdoToTgcDigitCfg(flags, name="TgcRdoToTgcDigitAlg", **kwargs):
    """Return ComponentAccumulator with configured TgcRdoToTgcDigit algorithm"""
    acc = ComponentAccumulator()
    if flags.Detector.Overlay:
        kwargs.setdefault("TgcRdoContainer", flags.Overlay.BkgPrefix + "TGCRDO")
        kwargs.setdefault("TgcDigitContainer", flags.Overlay.BkgPrefix + "TGC_DIGITS")
    acc.addEventAlgo(TgcRdoToTgcDigit(name, **kwargs))
    return acc


def STGC_RdoToDigitCfg(flags, name="STGC_RdoToDigitAlg", **kwargs):
    """Return ComponentAccumulator with configured STGC_RdoToDigit algorithm"""
    acc = ComponentAccumulator()
    if flags.Detector.Overlay:
        kwargs.setdefault("sTgcRdoContainer", flags.Overlay.BkgPrefix + "sTGCRDO")
        kwargs.setdefault("sTgcDigitContainer", flags.Overlay.BkgPrefix + "sTGC_DIGITS")
    acc.addEventAlgo(STGC_RdoToDigit(name, **kwargs))
    return acc


def MM_RdoToDigitCfg(flags, name="MM_RdoToDigitAlg", **kwargs):
    """Return ComponentAccumulator with configured MM_RdoToDigit algorithm"""
    acc = ComponentAccumulator()
    if flags.Detector.Overlay:
        kwargs.setdefault("MmRdoContainer", flags.Overlay.BkgPrefix + "MMRDO")
        kwargs.setdefault("MmDigitContainer", flags.Overlay.BkgPrefix + "MM_DIGITS")
    acc.addEventAlgo(MM_RdoToDigit(name, **kwargs))
    return acc


def MdtDigitToMdtRDOCfg(flags, name="MdtDigitToMdtRDO", **kwargs):
    """Return ComponentAccumulator with configured MdtDigitToMdtRDO algorithm"""
    acc = ComponentAccumulator()
    if flags.Digitization.PileUpPremixing:
        kwargs.setdefault("OutputObjectName", flags.Overlay.BkgPrefix + "MDTCSM")
    else:
        kwargs.setdefault("OutputObjectName", "MDTCSM")
    acc.addEventAlgo(MdtDigitToMdtRDO(name, **kwargs))
    return acc


def MdtOverlayDigitToMdtRDOCfg(flags, name="MdtOverlayDigitToMdtRDO", **kwargs):
    """Return ComponentAccumulator with Overlay configured MdtDigitToMdtRDO algorithm"""
    acc = ComponentAccumulator()
    kwargs.setdefault("InputObjectName", flags.Overlay.SigPrefix + "MDT_DIGITS")
    kwargs.setdefault("OutputObjectName", flags.Overlay.SigPrefix + "MDTCSM")
    acc.addEventAlgo(MdtDigitToMdtRDO(name, **kwargs))
    return acc


def RpcDigitToRpcRDOCfg(flags, name="RpcDigitToRpcRDO", **kwargs):
    """Return ComponentAccumulator with configured RpcDigitToRpcRDO algorithm"""
    acc = ComponentAccumulator()
    if flags.Digitization.PileUpPremixing:
        kwargs.setdefault("OutputObjectName", flags.Overlay.BkgPrefix + "RPCPAD")
    else:
        kwargs.setdefault("OutputObjectName", "RPCPAD")
    acc.addEventAlgo(RpcDigitToRpcRDO(name, **kwargs))
    return acc


def RpcOverlayDigitToRpcRDOCfg(flags, name="RpcOverlayDigitToRpcRDO", **kwargs):
    """Return ComponentAccumulator with Overlay configured RpcDigitToRpcRDO algorithm"""
    acc = ComponentAccumulator()
    kwargs.setdefault("InputObjectName", flags.Overlay.SigPrefix + "RPC_DIGITS")
    kwargs.setdefault("OutputObjectName", flags.Overlay.SigPrefix + "RPCPAD")
    acc.addEventAlgo(RpcDigitToRpcRDO(name, **kwargs))
    return acc


def TgcDigitToTgcRDOCfg(flags, name="TgcDigitToTgcRDO", **kwargs):
    """Return ComponentAccumulator with configured TgcDigitToTgcRDO algorithm"""
    acc = ComponentAccumulator()
    if flags.Digitization.PileUpPremixing:
        kwargs.setdefault("OutputObjectName", flags.Overlay.BkgPrefix + "TGCRDO")
    else:
        kwargs.setdefault("OutputObjectName", "TGCRDO")
    acc.addEventAlgo(TgcDigitToTgcRDO(name, **kwargs))
    return acc


def TgcOverlayDigitToTgcRDOCfg(flags, name="TgcOverlayDigitToTgcRDO", **kwargs):
    """Return ComponentAccumulator with Overlay configured TgcDigitToTgcRDO algorithm"""
    acc = ComponentAccumulator()
    kwargs.setdefault("InputObjectName", flags.Overlay.SigPrefix + "TGC_DIGITS")
    kwargs.setdefault("OutputObjectName", flags.Overlay.SigPrefix + "TGCRDO")
    acc.addEventAlgo(TgcDigitToTgcRDO(name, **kwargs))
    return acc


def CscDigitToCscRDOCfg(flags, name="CscDigitToCscRDO", **kwargs):
    """Return ComponentAccumulator with configured CscDigitToCscRDO algorithm"""
    # for Csc, configuration is in the tool CscDigitToCscRDOTool
    # configure dependency
    acc = CscCalibToolCfg(flags)
    CscCalibTool = acc.popPrivateTools()
    # configure basic parameters
    kwargs.setdefault("cscCalibTool", CscCalibTool)
    kwargs.setdefault("NumSamples", 4)
    kwargs.setdefault("Latency", 0)
    kwargs.setdefault("addNoise", True) # doMuonNoise flag not migrated
    if flags.Digitization.PileUpPremixing:
        kwargs.setdefault("OutputObjectName", flags.Overlay.BkgPrefix + "CSCRDO")
    else:
        kwargs.setdefault("OutputObjectName", "CSCRDO")
    # tool and container algorithm
    tool = CscDigitToCscRDOTool(name + "Tool", **kwargs)
    acc.addEventAlgo(CscDigitToCscRDO(name, CscDigitToRDOTool=tool))
    return acc


def CscOverlayDigitToCscRDOCfg(flags, name="CscOverlayDigitToCscRDO", **kwargs):
    """Return ComponentAccumulator with Overlay configured CscDigitToCscRDO algorithm"""
    kwargs.setdefault("NumSamples", 4)
    kwargs.setdefault("addNoise", False)
    kwargs.setdefault("InputObjectName", flags.Overlay.SigPrefix + "CSC_DIGITS")
    kwargs.setdefault("OutputObjectName", flags.Overlay.SigPrefix + "CSCRDO")
    return CscDigitToCscRDOCfg(flags, name, **kwargs)


def STGC_DigitToRDOCfg(flags, name="STGC_DigitToRDO", **kwargs):
    """Return ComponentAccumulator with configured STGC_DigitToRDO algorithm"""
    acc = ComponentAccumulator()
    if flags.Digitization.PileUpPremixing:
        kwargs.setdefault("OutputObjectName", flags.Overlay.BkgPrefix + "sTGCRDO")
    else:
        kwargs.setdefault("OutputObjectName", "sTGCRDO")
    acc.addEventAlgo(STGC_DigitToRDO(name, **kwargs))
    return acc


def STGC_OverlayDigitToRDOCfg(flags, name="STGC_OverlayDigitToRDO", **kwargs):
    """Return ComponentAccumulator with Overlay configured STGC_DigitToRDO algorithm"""
    acc = ComponentAccumulator()
    kwargs.setdefault("InputObjectName", flags.Overlay.SigPrefix + "sTGC_DIGITS")
    kwargs.setdefault("OutputObjectName", flags.Overlay.SigPrefix + "STGCRDO")
    acc.addEventAlgo(STGC_DigitToRDO(name, **kwargs))
    return acc


def MM_DigitToRDOCfg(flags, name="MM_DigitToRDO", **kwargs):
    """Return ComponentAccumulator with configured MM_DigitToRDO algorithm"""
    acc = ComponentAccumulator()
    if flags.Digitization.PileUpPremixing:
        kwargs.setdefault("OutputObjectName", flags.Overlay.BkgPrefix + "MMRDO")
    else:
        kwargs.setdefault("OutputObjectName", "MMRDO")
    acc.addEventAlgo(MM_DigitToRDO(name, **kwargs))
    return acc


def MM_OverlayDigitToRDOCfg(flags, name="MM_OverlayDigitToRDO", **kwargs):
    """Return ComponentAccumulator with Overlay configured MM_DigitToRDO algorithm"""
    acc = ComponentAccumulator()
    kwargs.setdefault("InputObjectName", flags.Overlay.SigPrefix + "MM_DIGITS")
    kwargs.setdefault("OutputObjectName", flags.Overlay.SigPrefix + "MMRDO")
    acc.addEventAlgo(MM_DigitToRDO(name, **kwargs))
    return acc


def SigMdtDigitToMdtRDOCfg(flags, name="SigMdtDigitToMdtRDO", **kwargs):
    """Return ComponentAccumulator with configured MdtDigitToMdtRDO algorithm"""
    acc = ComponentAccumulator()
    kwargs.setdefault("InputObjectName", flags.Overlay.Legacy.EventStore + "+MDT_DIGITS")
    kwargs.setdefault("OutputObjectName", flags.Overlay.Legacy.EventStore + "+MDTCSM")
    acc.addEventAlgo(MdtDigitToMdtRDO(name, **kwargs))
    return acc


def SigRpcDigitToRpcRDOCfg(flags, name="SigRpcDigitToRpcRDO", **kwargs):
    """Return ComponentAccumulator with configured RpcDigitToRpcRDO algorithm"""
    acc = ComponentAccumulator()
    kwargs.setdefault("InputObjectName", flags.Overlay.Legacy.EventStore + "+RPC_DIGITS")
    kwargs.setdefault("OutputObjectName", flags.Overlay.Legacy.EventStore + "+RPCPAD")
    acc.addEventAlgo(RpcDigitToRpcRDO(name, **kwargs))
    return acc


def SigTgcDigitToTgcRDOCfg(flags, name="SigTgcDigitToTgcRDO", **kwargs):
    """Return ComponentAccumulator with configured TgcDigitToTgcRDO algorithm"""
    acc = ComponentAccumulator()
    kwargs.setdefault("InputObjectName", flags.Overlay.Legacy.EventStore + "+TGC_DIGITS")
    kwargs.setdefault("OutputObjectName", flags.Overlay.Legacy.EventStore + "+TGCRDO")
    acc.addEventAlgo(TgcDigitToTgcRDO(name, **kwargs))
    return acc

