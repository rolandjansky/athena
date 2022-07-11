"""Define ComponentAccumulator functions for configuration of muon data conversions

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import ProductionStep
from MuonConfig.MuonGeometryConfig import MuonIdHelperSvcCfg
from MuonConfig.MuonCablingConfig import MDTCablingConfigCfg


def MdtRDO_DecoderCfg(flags, name="Muon::MdtRDO_Decoder", **kwargs):
    acc = MuonIdHelperSvcCfg(flags)
    acc.merge(MDTCablingConfigCfg(flags)) # To provide the Cabling map
    kwargs.setdefault("MuonIdHelperSvc", acc.getService("MuonIdHelperSvc"))
    acc.setPrivateTools(CompFactory.Muon.MdtRDO_Decoder(name, **kwargs))
    return acc


def MdtRdoToMdtDigitCfg(flags, name="MdtRdoToMdtDigitAlg", **kwargs):
    """Return ComponentAccumulator with configured MdtRdoToMdtDigit algorithm"""
    acc = MuonIdHelperSvcCfg(flags)
    kwargs.setdefault("mdtRdoDecoderTool", acc.popToolsAndMerge(MdtRDO_DecoderCfg(flags)))

    if flags.Common.isOverlay:
        kwargs.setdefault("MdtRdoContainer", f"{flags.Overlay.BkgPrefix}MDTCSM")
        kwargs.setdefault("MdtDigitContainer", f"{flags.Overlay.BkgPrefix}MDT_DIGITS")
    else:
        kwargs.setdefault("MdtRdoContainer", "MDTCSM")
        kwargs.setdefault("MdtDigitContainer", "MDT_DIGITS")

    if flags.Common.isOverlay and not flags.Overlay.DataOverlay:
        from SGComps.SGInputLoaderConfig import SGInputLoaderCfg
        acc.merge(SGInputLoaderCfg(flags, [f'MdtCsmContainer#{kwargs["MdtRdoContainer"]}']))

    acc.addEventAlgo(CompFactory.MdtRdoToMdtDigit(name, **kwargs))
    return acc


def RpcRdoToRpcDigitCfg(flags, name="RpcRdoToRpcDigitAlg", **kwargs):
    """Return ComponentAccumulator with configured RpcRdoToRpcDigit algorithm"""
    acc = ComponentAccumulator()
    from MuonConfig.MuonCablingConfig import RPCCablingConfigCfg
    acc.merge(RPCCablingConfigCfg(flags))

    if flags.Common.isOverlay:
        kwargs.setdefault("RpcRdoContainer", f"{flags.Overlay.BkgPrefix}RPCPAD")
        kwargs.setdefault("RpcDigitContainer", f"{flags.Overlay.BkgPrefix}RPC_DIGITS")
    else:
        kwargs.setdefault("RpcRdoContainer", "RPCPAD")
        kwargs.setdefault("RpcDigitContainer", "RPC_DIGITS")

    if flags.Common.isOverlay and not flags.Overlay.DataOverlay:
        from SGComps.SGInputLoaderConfig import SGInputLoaderCfg
        acc.merge(SGInputLoaderCfg(flags, [f'RpcPadContainer#{kwargs["RpcRdoContainer"]}']))

    acc.addEventAlgo(CompFactory.RpcRdoToRpcDigit(name, **kwargs))
    return acc


def TgcRdoToTgcDigitCfg(flags, name="TgcRdoToTgcDigitAlg", **kwargs):
    """Return ComponentAccumulator with configured TgcRdoToTgcDigit algorithm"""
    acc = ComponentAccumulator()
    from MuonConfig.MuonCablingConfig import TGCCablingConfigCfg
    acc.merge(TGCCablingConfigCfg(flags))

    if flags.Common.isOverlay:
        kwargs.setdefault("TgcRdoContainer", f"{flags.Overlay.BkgPrefix}TGCRDO")
        kwargs.setdefault("TgcDigitContainer",f"{flags.Overlay.BkgPrefix}TGC_DIGITS")
    else:
        kwargs.setdefault("TgcRdoContainer", "TGCRDO")
        kwargs.setdefault("TgcDigitContainer", "TGC_DIGITS")

    if flags.Common.isOverlay and not flags.Overlay.DataOverlay:
        from SGComps.SGInputLoaderConfig import SGInputLoaderCfg
        acc.merge(SGInputLoaderCfg(flags, [f'TgcRdoContainer#{kwargs["TgcRdoContainer"]}']))

    acc.addEventAlgo(CompFactory.TgcRdoToTgcDigit(name, **kwargs))
    return acc


def STGC_RdoToDigitCfg(flags, name="STGC_RdoToDigitAlg", **kwargs):
    """Return ComponentAccumulator with configured STGC_RdoToDigit algorithm"""
    acc = ComponentAccumulator()
    if flags.Common.isOverlay:
        kwargs.setdefault("sTgcRdoContainer", f"{flags.Overlay.BkgPrefix}sTGCRDO")
        kwargs.setdefault("sTgcDigitContainer", f"{flags.Overlay.BkgPrefix}sTGC_DIGITS")
    else:
        kwargs.setdefault("sTgcRdoContainer", "sTGCRDO")
        kwargs.setdefault("sTgcDigitContainer", "sTGC_DIGITS")

    kwargs.setdefault("sTgcRdoDecoderTool", acc.popToolsAndMerge(STgcRdoDecoderCfg(flags)))

    if flags.Common.isOverlay and not flags.Overlay.DataOverlay:
        from SGComps.SGInputLoaderConfig import SGInputLoaderCfg
        acc.merge(SGInputLoaderCfg(flags, [f'Muon::STGC_RawDataContainer#{kwargs["sTgcRdoContainer"]}']))

    acc.addEventAlgo(CompFactory.STGC_RdoToDigit(name, **kwargs))
    return acc


def MM_RdoToDigitCfg(flags, name="MM_RdoToDigitAlg", **kwargs):
    """Return ComponentAccumulator with configured MM_RdoToDigit algorithm"""
    acc = ComponentAccumulator()
    if flags.Common.isOverlay:
        kwargs.setdefault("MmRdoContainer", f"{flags.Overlay.BkgPrefix}MMRDO")
        kwargs.setdefault("MmDigitContainer", f"{flags.Overlay.BkgPrefix}MM_DIGITS")
    else:
        kwargs.setdefault("MmRdoContainer", "MMRDO")
        kwargs.setdefault("MmDigitContainer", "MM_DIGITS")

    kwargs.setdefault("mmRdoDecoderTool", acc.popToolsAndMerge(MMRdoDecoderCfg(flags)))

    if flags.Common.isOverlay and not flags.Overlay.DataOverlay:
        from SGComps.SGInputLoaderConfig import SGInputLoaderCfg
        acc.merge(SGInputLoaderCfg(flags, [f'Muon::MM_RawDataContainer#{kwargs["MmRdoContainer"]}']))

    acc.addEventAlgo(CompFactory.MM_RdoToDigit(name, **kwargs))
    return acc


def MdtDigitToMdtRDOCfg(flags, name="MdtDigitToMdtRDO", **kwargs):
    """Return ComponentAccumulator with configured MdtDigitToMdtRDO algorithm"""
    acc = ComponentAccumulator()
    kwargs.setdefault("MuonIdHelperSvc", acc.getPrimaryAndMerge(MuonIdHelperSvcCfg(flags)).name)

    if flags.Common.ProductionStep == ProductionStep.PileUpPresampling:
        kwargs.setdefault("OutputObjectName", f"{flags.Overlay.BkgPrefix}MDTCSM")
    else:
        kwargs.setdefault("OutputObjectName", "MDTCSM")

    acc.addEventAlgo(CompFactory.MdtDigitToMdtRDO(name, **kwargs))
    return acc


def RpcDigitToRpcRDOCfg(flags, name="RpcDigitToRpcRDO", **kwargs):
    """Return ComponentAccumulator with configured RpcDigitToRpcRDO algorithm"""
    acc = ComponentAccumulator()
    kwargs.setdefault("MuonIdHelperSvc", acc.getPrimaryAndMerge(MuonIdHelperSvcCfg(flags)).name)

    if flags.Common.ProductionStep == ProductionStep.PileUpPresampling:
        kwargs.setdefault("OutputObjectName", flags.Overlay.BkgPrefix + "RPCPAD")
    else:
        kwargs.setdefault("OutputObjectName", "RPCPAD")

    acc.addEventAlgo(CompFactory.RpcDigitToRpcRDO(name, **kwargs))
    return acc


def TgcDigitToTgcRDOCfg(flags, name="TgcDigitToTgcRDO", **kwargs):
    """Return ComponentAccumulator with configured TgcDigitToTgcRDO algorithm"""
    acc = ComponentAccumulator()
    kwargs.setdefault("MuonIdHelperSvc", acc.getPrimaryAndMerge(MuonIdHelperSvcCfg(flags)).name)

    if flags.Common.ProductionStep == ProductionStep.PileUpPresampling:
        kwargs.setdefault("OutputObjectName", f"{flags.Overlay.BkgPrefix}TGCRDO")
    else:
        kwargs.setdefault("OutputObjectName", "TGCRDO")

    acc.addEventAlgo(CompFactory.TgcDigitToTgcRDO(name, **kwargs))
    return acc


def CscDigitToCscRDOToolCfg(flags, name="CscDigitToCscRDOTool", **kwargs):
    """Return ComponentAccumulator with configured CscDigitToCscRDOTool"""
    acc = ComponentAccumulator()
    # configure basic parameters
    kwargs.setdefault("MuonIdHelperSvc", acc.getPrimaryAndMerge(MuonIdHelperSvcCfg(flags)).name)
    from MuonConfig.MuonCalibrationConfig import CscCalibToolCfg
    kwargs.setdefault("cscCalibTool", acc.popToolsAndMerge(CscCalibToolCfg(flags)))
    kwargs.setdefault("NumSamples", 4)
    kwargs.setdefault("Latency", 0)
    kwargs.setdefault("addNoise", not flags.Common.isOverlay) # doMuonNoise flag not migrated

    if flags.Common.isOverlay:
        kwargs.setdefault("InputObjectName", f"{flags.Overlay.SigPrefix}CSC_DIGITS")
        kwargs.setdefault("OutputObjectName", f"{flags.Overlay.SigPrefix}CSCRDO")
    if flags.Common.ProductionStep == ProductionStep.PileUpPresampling:
        kwargs.setdefault("OutputObjectName", f"{flags.Overlay.BkgPrefix}CSCRDO")
    else:
        kwargs.setdefault("OutputObjectName", "CSCRDO")

    from RngComps.RandomServices import AthRNGSvcCfg
    kwargs.setdefault("RndmSvc", acc.getPrimaryAndMerge(AthRNGSvcCfg(flags)).name)
    acc.setPrivateTools(CompFactory.CscDigitToCscRDOTool("CscDigitToCscRDOTool", **kwargs))
    return acc


def CscDigitToCscRDOCfg(flags, name="CscDigitToCscRDO", **kwargs):
    """Return ComponentAccumulator with configured CscDigitToCscRDO algorithm"""
    # for CSC, configuration is in the tool CscDigitToCscRDOTool
    acc = ComponentAccumulator()
    kwargs.setdefault("CscDigitToRDOTool", acc.popToolsAndMerge(CscDigitToCscRDOToolCfg(flags)))

    if flags.Concurrency.NumThreads > 0:
        kwargs.setdefault("Cardinality", flags.Concurrency.NumThreads)

    acc.addEventAlgo(CompFactory.CscDigitToCscRDO(name, **kwargs))
    return acc


def STGC_DigitToRDOCfg(flags, name="STGC_DigitToRDO", **kwargs):
    """Return ComponentAccumulator with configured STGC_DigitToRDO algorithm"""
    acc = ComponentAccumulator()
    kwargs.setdefault("MuonIdHelperSvc", acc.getPrimaryAndMerge(MuonIdHelperSvcCfg(flags)).name)

    if flags.Common.ProductionStep == ProductionStep.PileUpPresampling:
        kwargs.setdefault("OutputObjectName", f"{flags.Overlay.BkgPrefix}sTGCRDO")
    else:
        kwargs.setdefault("OutputObjectName", "sTGCRDO")

    from MuonConfig.MuonCalibrationConfig import NSWCalibToolCfg
    kwargs.setdefault("CalibrationTool", acc.popToolsAndMerge(NSWCalibToolCfg(flags)))
    the_alg  = CompFactory.STGC_DigitToRDO(name, **kwargs)
    acc.addEventAlgo(the_alg)
    return acc


def MM_DigitToRDOCfg(flags, name="MM_DigitToRDO", **kwargs):
    """Return ComponentAccumulator with configured MM_DigitToRDO algorithm"""
    acc = ComponentAccumulator()
    kwargs.setdefault("MuonIdHelperSvc", acc.getPrimaryAndMerge(MuonIdHelperSvcCfg(flags)).name)

    if flags.Common.ProductionStep == ProductionStep.PileUpPresampling:
        kwargs.setdefault("OutputObjectName", f"{flags.Overlay.BkgPrefix}MMRDO")
    else:
        kwargs.setdefault("OutputObjectName", "MMRDO")

    from MuonConfig.MuonCalibrationConfig import NSWCalibToolCfg
    kwargs.setdefault("CalibrationTool", acc.popToolsAndMerge(NSWCalibToolCfg(flags)))    
    the_alg =  CompFactory.MM_DigitToRDO(name, **kwargs) 
    acc.addEventAlgo(the_alg)
    return acc


def SigMdtDigitToMdtRDOCfg(flags, name="SigMdtDigitToMdtRDO", **kwargs):
    """Return ComponentAccumulator with configured MdtDigitToMdtRDO algorithm"""
    acc = ComponentAccumulator()
    kwargs.setdefault("MuonIdHelperSvc", acc.getPrimaryAndMerge(MuonIdHelperSvcCfg(flags)).name)
    kwargs.setdefault("InputObjectName", f"{flags.Overlay.SigPrefix}MDT_DIGITS")
    kwargs.setdefault("OutputObjectName", f"{flags.Overlay.SigPrefix}MDTCSM")
    acc.addEventAlgo(CompFactory.MdtDigitToMdtRDO(name, **kwargs))
    return acc


def SigRpcDigitToRpcRDOCfg(flags, name="SigRpcDigitToRpcRDO", **kwargs):
    """Return ComponentAccumulator with configured RpcDigitToRpcRDO algorithm"""
    acc = ComponentAccumulator()
    kwargs.setdefault("MuonIdHelperSvc", acc.getPrimaryAndMerge(MuonIdHelperSvcCfg(flags)).name)
    kwargs.setdefault("InputObjectName", f"{flags.Overlay.SigPrefix}RPC_DIGITS")
    kwargs.setdefault("OutputObjectName", f"{flags.Overlay.SigPrefix}RPCPAD")
    acc.addEventAlgo(CompFactory.RpcDigitToRpcRDO(name, **kwargs))
    return acc


def SigTgcDigitToTgcRDOCfg(flags, name="SigTgcDigitToTgcRDO", **kwargs):
    """Return ComponentAccumulator with configured TgcDigitToTgcRDO algorithm"""
    acc = ComponentAccumulator()
    kwargs.setdefault("MuonIdHelperSvc", acc.getPrimaryAndMerge(MuonIdHelperSvcCfg(flags)).name)
    kwargs.setdefault("InputObjectName", f"{flags.Overlay.SigPrefix}TGC_DIGITS")
    kwargs.setdefault("OutputObjectName", f"{flags.Overlay.SigPrefix}TGCRDO")
    acc.addEventAlgo(CompFactory.TgcDigitToTgcRDO(name, **kwargs))
    return acc

def STgcRdoDecoderCfg(flags, name="STGC_RDO_Decoder", **kwargs):
    result = ComponentAccumulator()
    from MuonConfig.MuonCalibrationConfig import NSWCalibToolCfg
    kwargs.setdefault("CalibrationTool", result.popToolsAndMerge(NSWCalibToolCfg(flags)))    
    the_tool = CompFactory.Muon.STGC_RDO_Decoder(name, **kwargs)
    result.setPrivateTools(the_tool)
    return result
    
def MMRdoDecoderCfg(flags, name="MM_RDO_Decoder", **kwargs):
    result = ComponentAccumulator()
    from MuonConfig.MuonCalibrationConfig import NSWCalibToolCfg
    kwargs.setdefault("CalibrationTool", result.popToolsAndMerge(NSWCalibToolCfg(flags)))    
    the_tool = CompFactory.Muon.MM_RDO_Decoder(name, **kwargs)
    result.setPrivateTools(the_tool)   
    return result

def MdtRdoDecoderCfg(flags,name= "MDT_RDO_Decoder", **kwargs):
    result = ComponentAccumulator()
    from MuonConfig.MuonCablingConfig import MDTCablingConfigCfg
    result.merge(MDTCablingConfigCfg(flags))
    the_tool = CompFactory.Muon.MdtRDO_Decoder(name, **kwargs)
    result.setPrivateTools(the_tool)
    return result
  

