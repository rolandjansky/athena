"""Define methods to construct configured RPC Digitization tools and algorithms

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
from RPC_Digitization.RPC_DigitizationConf import RpcDigitizationTool, RPC_Digitizer
from PileUpComps.PileUpCompsConf import PileUpXingFolder

# The earliest and last bunch crossing times for which interactions will be sent
# to the RpcDigitizationTool.
def RPC_FirstXing():
    return -150

def RPC_LastXing():
    return 125

def RPC_RangeToolCfg(flags, name="RPC_Range", **kwargs):
    """Return a PileUpXingFolder tool configured for RPC"""
    kwargs.setdefault("FirstXing", RPC_FirstXing())
    kwargs.setdefault("LastXing", RPC_LastXing())
    kwargs.setdefault("CacheRefreshFrequency", 1.0)
    kwargs.setdefault("ItemList", ["RPCSimHitCollection#RPC_Hits"])
    return PileUpXingFolder(name, **kwargs)

def RPC_DigitizationToolCfg(flags, name="RPC_DigitizationTool", **kwargs):
    """Return ComponentAccumulator with configured RpcDigitizationTool"""
    from MuonConfig.MuonCondAlgConfig import RpcCondDbAlgCfg # MT-safe conditions access
    acc = RpcCondDbAlgCfg(flags)
    if flags.Digitization.DoXingByXingPileUp:
        kwargs.setdefault("FirstXing", RPC_FirstXing())
        kwargs.setdefault("LastXing", RPC_LastXing())
    kwargs.setdefault("OutputObjectName", "RPC_DIGITS")
    if flags.Digitization.PileUpPremixing:
        kwargs.setdefault("OutputSDOName", flags.Overlay.BkgPrefix + "RPC_SDO")
    else:
        kwargs.setdefault("OutputSDOName", "RPC_SDO")
    # config
    kwargs.setdefault("DeadTime", 100)
    kwargs.setdefault("PatchForRpcTime", True)	    
    # kwargs.setdefault("PatchForRpcTimeShift", 9.6875)  
    kwargs.setdefault("PatchForRpcTimeShift", 12.5)  
    kwargs.setdefault("turnON_efficiency", True)
    kwargs.setdefault("turnON_clustersize", True)
    kwargs.setdefault("testbeam_clustersize", 0)
    kwargs.setdefault("ClusterSize1_2uncorr", 0)
    kwargs.setdefault("CutProjectedTracks", 100)
    kwargs.setdefault("RPCInfoFromDb", True)
    kwargs.setdefault("Efficiency_fromCOOL", True)
    kwargs.setdefault("EfficiencyPatchForBMShighEta", False)
    kwargs.setdefault("ClusterSize_fromCOOL", True)
    kwargs.setdefault("DumpFromDbFirst", False)
    kwargs.setdefault("PanelId_OFF_fromlist", False)
    kwargs.setdefault("PanelId_OK_fromlist", False)
    kwargs.setdefault("IgnoreRunDependentConfig", False)
    kwargs.setdefault("PrintCalibrationVector",False )
    kwargs.setdefault("PhiAndEtaEff_A",[0.938, 0.938, 0.938, 0.938, 0.938, 0.938, 0.938, 0.938, 0.938])
    kwargs.setdefault("OnlyPhiEff_A"  ,[0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022])
    kwargs.setdefault("OnlyEtaEff_A"  ,[0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022])
    kwargs.setdefault("PhiAndEtaEff_C",[0.938, 0.938, 0.938, 0.938, 0.938, 0.938, 0.938, 0.938, 0.938])
    kwargs.setdefault("OnlyPhiEff_C"  ,[0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022])
    kwargs.setdefault("OnlyEtaEff_C"  ,[0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022])
    kwargs.setdefault("FracClusterSize1_A",   [0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664])
    kwargs.setdefault("FracClusterSize2_A",   [0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986])
    kwargs.setdefault("FracClusterSizeTail_A",[0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035, 0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035 ])
    kwargs.setdefault("MeanClusterSizeTail_A",[0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598])
    kwargs.setdefault("FracClusterSize1_C",   [0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664])
    kwargs.setdefault("FracClusterSize2_C",   [0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986])
    kwargs.setdefault("FracClusterSizeTail_C",[0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035, 0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035 ])
    kwargs.setdefault("MeanClusterSizeTail_C",[0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598])
    acc.setPrivateTools(RpcDigitizationTool(name, **kwargs))
    return acc

def RPC_OverlayDigitizationToolCfg(flags, name="RPC_DigitizationTool", **kwargs):
    """Return ComponentAccumulator with RpcDigitizationTool configured for Overlay"""
    acc = ComponentAccumulator()
    kwargs.setdefault("OnlyUseContainerName", False)
    kwargs.setdefault("OutputObjectName", "StoreGateSvc+" + flags.Overlay.SigPrefix + "RPC_DIGITS")
    if not flags.Overlay.DataOverlay:
        kwargs.setdefault("OutputSDOName", "StoreGateSvc+" + flags.Overlay.SigPrefix + "RPC_SDO")
    acc.setPrivateTools(RpcDigitizationTool(name, **kwargs))
    return acc


def RPC_DigitizerBasicCfg(toolCfg, flags, name, **kwargs):
    """Return ComponentAccumulator with toolCfg configured RpcDigitization algorithm"""
    acc = MuonGeoModelCfg(flags)
    tool = acc.popToolsAndMerge(RPC_DigitizationToolCfg(flags))
    kwargs.setdefault("DigitizationTool", tool)
    acc.addEventAlgo(RPC_Digitizer(name,**kwargs))
    return acc

def RPC_DigitizerOutputCfg(toolCfg, flags, name, **kwargs):
    """Return ComponentAccumulator with toolCfg configured RPC Digitizer algorithm and OutputStream"""
    acc = RPC_DigitizerBasicCfg(toolCfg, flags, name, **kwargs)
    acc.merge(OutputStreamCfg(flags, "RDO", ["MuonSimDataCollection#*", "RpcPadContainer#*"]))
    return acc


def RPC_DigitizerCfg(flags, name="RPC_Digitizer", **kwargs):
    """Return ComponentAccumulator with standard RpcDigitization algorithm and Output"""
    return RPC_DigitizerOutputCfg(RPC_DigitizationToolCfg, flags, name, **kwargs)

def RPC_DigitizerOverlayCfg(flags, name="RPC_Digitizer", **kwargs):
    """Return ComponentAccumulator with Overlay configured RpcDigitization and Output"""
    return RPC_DigitizerOutputCfg(RPC_OverlayDigitizationToolCfg, flags, name, **kwargs)

