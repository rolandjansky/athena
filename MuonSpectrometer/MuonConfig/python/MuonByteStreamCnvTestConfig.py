"""Define ComponentAccumulator functions for configuration of muon data conversions

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from MuonConfig.MuonGeometryConfig import MuonIdHelperSvcCfg
from MuonConfig.MuonCablingConfig import MDTCablingConfigCfg

def MdtRDO_DecoderCfg(flags, name="Muon::MdtRDO_Decoder"):
    acc = MuonIdHelperSvcCfg(flags)
    acc.merge(MDTCablingConfigCfg(flags)) # To provide the Cabling map
    Muon_MdtRDO_Decoder = CompFactory.Muon.MdtRDO_Decoder
    mdtRDO_Decoder = Muon_MdtRDO_Decoder(name)
    mdtRDO_Decoder.MuonIdHelperSvc = acc.getService("MuonIdHelperSvc")
    acc.setPrivateTools(mdtRDO_Decoder)
    return acc

def MdtRdoToMdtDigitCfg(flags, name="MdtRdoToMdtDigitAlg", **kwargs):
    """Return ComponentAccumulator with configured MdtRdoToMdtDigit algorithm"""
    acc = MuonIdHelperSvcCfg(flags)
    decoderTool = acc.popToolsAndMerge(MdtRDO_DecoderCfg(flags))
    kwargs.setdefault("mdtRdoDecoderTool", decoderTool)
    if flags.Detector.OverlayMDT:
        kwargs.setdefault("MdtRdoContainer", flags.Overlay.BkgPrefix + "MDTCSM")
        kwargs.setdefault("MdtDigitContainer", flags.Overlay.BkgPrefix + "MDT_DIGITS")
    MdtRdoToMdtDigit = CompFactory.MdtRdoToMdtDigit
    acc.addEventAlgo(MdtRdoToMdtDigit(name, **kwargs))
    return acc


def RpcRdoToRpcDigitCfg(flags, name="RpcRdoToRpcDigitAlg", **kwargs):
    """Return ComponentAccumulator with configured RpcRdoToRpcDigit algorithm"""
    acc = ComponentAccumulator()
    from MuonConfig.MuonCablingConfig import RPCCablingConfigCfg
    acc.merge(RPCCablingConfigCfg(flags))
    if flags.Detector.OverlayRPC:
        kwargs.setdefault("RpcRdoContainer", flags.Overlay.BkgPrefix + "RPCPAD")
        kwargs.setdefault("RpcDigitContainer", flags.Overlay.BkgPrefix + "RPC_DIGITS")
    RpcRdoToRpcDigit = CompFactory.RpcRdoToRpcDigit
    acc.addEventAlgo(RpcRdoToRpcDigit(name, **kwargs))
    return acc


def TgcRdoToTgcDigitCfg(flags, name="TgcRdoToTgcDigitAlg", **kwargs):
    """Return ComponentAccumulator with configured TgcRdoToTgcDigit algorithm"""
    acc = ComponentAccumulator()
    from MuonConfig.MuonCablingConfig import TGCCablingConfigCfg
    acc.merge(TGCCablingConfigCfg(flags))
    if flags.Detector.OverlayTGC:
        kwargs.setdefault("TgcRdoContainer", flags.Overlay.BkgPrefix + "TGCRDO")
        kwargs.setdefault("TgcDigitContainer", flags.Overlay.BkgPrefix + "TGC_DIGITS")
    TgcRdoToTgcDigit = CompFactory.TgcRdoToTgcDigit
    acc.addEventAlgo(TgcRdoToTgcDigit(name, **kwargs))
    return acc


def STGC_RdoToDigitCfg(flags, name="STGC_RdoToDigitAlg", **kwargs):
    """Return ComponentAccumulator with configured STGC_RdoToDigit algorithm"""
    acc = ComponentAccumulator()
    if flags.Detector.OverlaysTGC:
        kwargs.setdefault("sTgcRdoContainer", flags.Overlay.BkgPrefix + "sTGCRDO")
        kwargs.setdefault("sTgcDigitContainer", flags.Overlay.BkgPrefix + "sTGC_DIGITS")
    STGC_RdoToDigit = CompFactory.STGC_RdoToDigit
    acc.addEventAlgo(STGC_RdoToDigit(name, **kwargs))
    return acc


def MM_RdoToDigitCfg(flags, name="MM_RdoToDigitAlg", **kwargs):
    """Return ComponentAccumulator with configured MM_RdoToDigit algorithm"""
    acc = ComponentAccumulator()
    if flags.Detector.OverlayMM:
        kwargs.setdefault("MmRdoContainer", flags.Overlay.BkgPrefix + "MMRDO")
        kwargs.setdefault("MmDigitContainer", flags.Overlay.BkgPrefix + "MM_DIGITS")
    MM_RdoToDigit = CompFactory.MM_RdoToDigit
    acc.addEventAlgo(MM_RdoToDigit(name, **kwargs))
    return acc


def MdtDigitToMdtRDOCfg(flags, name="MdtDigitToMdtRDO", **kwargs):
    """Return ComponentAccumulator with configured MdtDigitToMdtRDO algorithm"""
    acc = ComponentAccumulator()
    if flags.Digitization.PileUpPremixing:
        kwargs.setdefault("OutputObjectName", flags.Overlay.BkgPrefix + "MDTCSM")
    else:
        kwargs.setdefault("OutputObjectName", "MDTCSM")
    MdtDigitToMdtRDO = CompFactory.MdtDigitToMdtRDO
    acc.addEventAlgo(MdtDigitToMdtRDO(name, **kwargs))
    return acc


def RpcDigitToRpcRDOCfg(flags, name="RpcDigitToRpcRDO", **kwargs):
    """Return ComponentAccumulator with configured RpcDigitToRpcRDO algorithm"""
    acc = ComponentAccumulator()
    if flags.Digitization.PileUpPremixing:
        kwargs.setdefault("OutputObjectName", flags.Overlay.BkgPrefix + "RPCPAD")
    else:
        kwargs.setdefault("OutputObjectName", "RPCPAD")
    RpcDigitToRpcRDO = CompFactory.RpcDigitToRpcRDO
    acc.addEventAlgo(RpcDigitToRpcRDO(name, **kwargs))
    return acc


def TgcDigitToTgcRDOCfg(flags, name="TgcDigitToTgcRDO", **kwargs):
    """Return ComponentAccumulator with configured TgcDigitToTgcRDO algorithm"""
    acc = ComponentAccumulator()
    if flags.Digitization.PileUpPremixing:
        kwargs.setdefault("OutputObjectName", flags.Overlay.BkgPrefix + "TGCRDO")
    else:
        kwargs.setdefault("OutputObjectName", "TGCRDO")
    TgcDigitToTgcRDO = CompFactory.TgcDigitToTgcRDO
    acc.addEventAlgo(TgcDigitToTgcRDO(name, **kwargs))
    return acc


def CscDigitToCscRDOCfg(flags, name="CscDigitToCscRDO", **kwargs):
    """Return ComponentAccumulator with configured CscDigitToCscRDO algorithm"""
    # for Csc, configuration is in the tool CscDigitToCscRDOTool
    # configure dependency
    from MuonConfig.MuonCalibrationConfig import CscCalibToolCfg
    acc = CscCalibToolCfg(flags)
    CscCalibTool = acc.popPrivateTools()
    # configure basic parameters
    kwargs.setdefault("cscCalibTool", CscCalibTool)
    kwargs.setdefault("NumSamples", 4)
    kwargs.setdefault("Latency", 0)
    kwargs.setdefault("addNoise", not flags.Detector.OverlayCSC) # doMuonNoise flag not migrated
    if flags.Detector.OverlayCSC:
        kwargs.setdefault("InputObjectName", flags.Overlay.SigPrefix + "CSC_DIGITS")
        kwargs.setdefault("OutputObjectName", flags.Overlay.SigPrefix + "CSCRDO")
    elif flags.Digitization.PileUpPremixing:
        kwargs.setdefault("OutputObjectName", flags.Overlay.BkgPrefix + "CSCRDO")
    else:
        kwargs.setdefault("OutputObjectName", "CSCRDO")
    # tool and container algorithm
    CscDigitToCscRDOTool = CompFactory.CscDigitToCscRDOTool
    tool = CscDigitToCscRDOTool("CscDigitToCscRDOTool", **kwargs)
    CscDigitToCscRDO = CompFactory.CscDigitToCscRDO
    acc.addEventAlgo(CscDigitToCscRDO(name, CscDigitToRDOTool=tool))
    return acc


def STGC_DigitToRDOCfg(flags, name="STGC_DigitToRDO", **kwargs):
    """Return ComponentAccumulator with configured STGC_DigitToRDO algorithm"""
    acc = ComponentAccumulator()
    if flags.Digitization.PileUpPremixing:
        kwargs.setdefault("OutputObjectName", flags.Overlay.BkgPrefix + "sTGCRDO")
    else:
        kwargs.setdefault("OutputObjectName", "sTGCRDO")
    STGC_DigitToRDO = CompFactory.STGC_DigitToRDO
    acc.addEventAlgo(STGC_DigitToRDO(name, **kwargs))
    return acc


def MM_DigitToRDOCfg(flags, name="MM_DigitToRDO", **kwargs):
    """Return ComponentAccumulator with configured MM_DigitToRDO algorithm"""
    acc = ComponentAccumulator()
    if flags.Digitization.PileUpPremixing:
        kwargs.setdefault("OutputObjectName", flags.Overlay.BkgPrefix + "MMRDO")
    else:
        kwargs.setdefault("OutputObjectName", "MMRDO")
    MM_DigitToRDO = CompFactory.MM_DigitToRDO
    acc.addEventAlgo(MM_DigitToRDO(name, **kwargs))
    return acc


def SigMdtDigitToMdtRDOCfg(flags, name="SigMdtDigitToMdtRDO", **kwargs):
    """Return ComponentAccumulator with configured MdtDigitToMdtRDO algorithm"""
    acc = ComponentAccumulator()
    kwargs.setdefault("InputObjectName", flags.Overlay.SigPrefix + "MDT_DIGITS")
    kwargs.setdefault("OutputObjectName", flags.Overlay.SigPrefix + "MDTCSM")
    MdtDigitToMdtRDO = CompFactory.MdtDigitToMdtRDO
    acc.addEventAlgo(MdtDigitToMdtRDO(name, **kwargs))
    return acc


def SigRpcDigitToRpcRDOCfg(flags, name="SigRpcDigitToRpcRDO", **kwargs):
    """Return ComponentAccumulator with configured RpcDigitToRpcRDO algorithm"""
    acc = ComponentAccumulator()
    kwargs.setdefault("InputObjectName", flags.Overlay.SigPrefix + "RPC_DIGITS")
    kwargs.setdefault("OutputObjectName", flags.Overlay.SigPrefix + "RPCPAD")
    RpcDigitToRpcRDO = CompFactory.RpcDigitToRpcRDO
    acc.addEventAlgo(RpcDigitToRpcRDO(name, **kwargs))
    return acc


def SigTgcDigitToTgcRDOCfg(flags, name="SigTgcDigitToTgcRDO", **kwargs):
    """Return ComponentAccumulator with configured TgcDigitToTgcRDO algorithm"""
    acc = ComponentAccumulator()
    kwargs.setdefault("InputObjectName", flags.Overlay.SigPrefix + "TGC_DIGITS")
    kwargs.setdefault("OutputObjectName", flags.Overlay.SigPrefix + "TGCRDO")
    TgcDigitToTgcRDO = CompFactory.TgcDigitToTgcRDO
    acc.addEventAlgo(TgcDigitToTgcRDO(name, **kwargs))
    return acc

