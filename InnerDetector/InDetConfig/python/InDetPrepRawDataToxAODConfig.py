# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of InDetPrepRawDataToxAOD package
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import LHCPeriod

def InDetPixelPrepDataToxAODCfg(flags, name='InDetPixelPrepDataToxAOD', **kwargs):
    from PixelGeoModel.PixelGeoModelConfig import PixelReadoutGeometryCfg
    acc = PixelReadoutGeometryCfg(flags)

    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelChargeLUTCalibCondAlgCfg, PixelChargeCalibCondAlgCfg, PixelDCSCondStateAlgCfg, PixelDCSCondStatusAlgCfg, PixelDCSCondTempAlgCfg, PixelDCSCondHVAlgCfg
    if flags.GeoModel.Run is LHCPeriod.Run3:
        acc.merge(PixelChargeLUTCalibCondAlgCfg(flags))
    else:
        acc.merge(PixelChargeCalibCondAlgCfg(flags))
    acc.merge(PixelDCSCondStateAlgCfg(flags))
    acc.merge(PixelDCSCondStatusAlgCfg(flags))
    acc.merge(PixelDCSCondTempAlgCfg(flags))
    acc.merge(PixelDCSCondHVAlgCfg(flags))

    from InDetConfig.PixelCalibAlgsConfig import PixelChargeToTConversionCfg
    acc.merge(PixelChargeToTConversionCfg(flags))

    if "PixelConditionsSummaryTool" not in kwargs:
        from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg
        kwargs.setdefault("PixelConditionsSummaryTool", acc.popToolsAndMerge(PixelConditionsSummaryCfg(flags)))

    if "LorentzAngleTool" not in kwargs:
        from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleToolCfg
        kwargs.setdefault("LorentzAngleTool", acc.popToolsAndMerge(PixelLorentzAngleToolCfg(flags)))

    kwargs.setdefault("UseTruthInfo", flags.Input.isMC)
    kwargs.setdefault("WriteExtendedPRDinformation", True)

    acc.addEventAlgo(CompFactory.PixelPrepDataToxAOD(name, **kwargs))
    return acc

def ITkPixelPrepDataToxAODCfg(flags, name='ITkPixelPrepDataToxAOD', **kwargs):
    from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelReadoutGeometryCfg
    acc = ITkPixelReadoutGeometryCfg(flags)

    from PixelConditionsAlgorithms.ITkPixelConditionsConfig import ITkPixelChargeCalibCondAlgCfg, ITkPixelDCSCondStateAlgCfg, ITkPixelDCSCondStatusAlgCfg, ITkPixelDCSCondTempAlgCfg, ITkPixelDCSCondHVAlgCfg
    acc.merge(ITkPixelChargeCalibCondAlgCfg(flags))
    acc.merge(ITkPixelDCSCondStateAlgCfg(flags))
    acc.merge(ITkPixelDCSCondStatusAlgCfg(flags))
    acc.merge(ITkPixelDCSCondTempAlgCfg(flags))
    acc.merge(ITkPixelDCSCondHVAlgCfg(flags))

    from InDetConfig.PixelCalibAlgsConfig import ITkPixelChargeToTConversionCfg
    acc.merge(ITkPixelChargeToTConversionCfg(flags))

    if "PixelConditionsSummaryTool" not in kwargs:
        from PixelConditionsTools.ITkPixelConditionsSummaryConfig import ITkPixelConditionsSummaryCfg
        kwargs.setdefault("PixelConditionsSummaryTool", acc.popToolsAndMerge(ITkPixelConditionsSummaryCfg(flags)))

    if "LorentzAngleTool" not in kwargs:
        from SiLorentzAngleTool.ITkPixelLorentzAngleConfig import ITkPixelLorentzAngleToolCfg
        kwargs.setdefault("LorentzAngleTool", acc.popToolsAndMerge(ITkPixelLorentzAngleToolCfg(flags)))

    kwargs.setdefault("UseTruthInfo", flags.Input.isMC)
    kwargs.setdefault("WriteExtendedPRDinformation", True)
    kwargs.setdefault("PixelReadoutManager", "ITkPixelReadoutManager")
    kwargs.setdefault("PixelChargeCalibCondData", "ITkPixelChargeCalibCondData")
    kwargs.setdefault("PixelDCSStateCondData", "ITkPixelDCSStateCondData")
    kwargs.setdefault("PixelDCSStatusCondData", "ITkPixelDCSStatusCondData")
    kwargs.setdefault("ReadKeyTemp", "ITkPixelDCSTempCondData")
    kwargs.setdefault("ReadKeyHV", "ITkPixelDCSHVCondData")    
    kwargs.setdefault("SiClusterContainer", "ITkPixelClusters")
    kwargs.setdefault("MC_SDOs", "ITkPixelSDO_Map")
    kwargs.setdefault("MC_Hits", "ITkPixelHits")
    kwargs.setdefault("PRD_MultiTruth", "PRD_MultiTruthITkPixel")
    kwargs.setdefault("OutputClusterContainer", "ITkPixelClusters")

    acc.addEventAlgo(CompFactory.PixelPrepDataToxAOD(name, **kwargs))
    return acc
    
def InDetSCT_PrepDataToxAODCfg(flags, name='InDetSCTPrepDataToxAOD', **kwargs):
    from SCT_GeoModel.SCT_GeoModelConfig import SCT_ReadoutGeometryCfg
    acc = SCT_ReadoutGeometryCfg(flags)
    kwargs.setdefault("UseTruthInfo", flags.Input.isMC)
    acc.addEventAlgo(CompFactory.SCT_PrepDataToxAOD(name, **kwargs))
    return acc

def ITkStripPrepDataToxAODCfg(flags, name='ITkStripPrepDataToxAOD', **kwargs):
    from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripReadoutGeometryCfg
    acc = ITkStripReadoutGeometryCfg(flags)

    kwargs.setdefault("SiClusterContainer", "ITkStripClusters")
    kwargs.setdefault("MC_SDOs", "ITkStripSDO_Map")
    kwargs.setdefault("MC_Hits", "ITkStripHits")
    kwargs.setdefault("PRD_MultiTruth", "PRD_MultiTruthITkStrip")
    kwargs.setdefault("SctRdoContainer", "ITkStripRDOs")
    kwargs.setdefault("SctxAodContainer", "ITkStripClusters")
    kwargs.setdefault("SctxAodOffset", "ITkStripClustersOffsets")
    kwargs.setdefault("SCTDetEleCollKey", "ITkStripDetectorElementCollection")
    kwargs.setdefault("UseTruthInfo", flags.Input.isMC)

    acc.addEventAlgo(CompFactory.SCT_PrepDataToxAOD(name, **kwargs))
    return acc

def InDetTRT_PrepDataToxAODCfg(flags, name='InDetTRTPrepDataToxAOD', **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("UseTruthInfo", flags.Input.isMC)
    acc.addEventAlgo(CompFactory.TRT_PrepDataToxAOD(name, **kwargs))
    return acc
