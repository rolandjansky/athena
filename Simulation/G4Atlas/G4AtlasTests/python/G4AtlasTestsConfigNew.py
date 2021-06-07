# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def SiHitsTestToolCfg(collectionName, name="SiHitsTestTool", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')
    result = ComponentAccumulator()

    kwargs.setdefault("CollectionName", collectionName)
    SiHitsTestTool = CompFactory.SiHitsTestTool
    result.setPrivateTools(SiHitsTestTool(name, **kwargs))
    return result


def TrtHitsTestToolCfg(collectionName, name="TRTHitsTestTool", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')
    result = ComponentAccumulator()

    kwargs.setdefault("CollectionName", collectionName)
    TrtHitsTestTool = CompFactory.TrtHitsTestTool
    result.setPrivateTools(TrtHitsTestTool(name, **kwargs))
    return result


def LArHitsTestToolCfg(detectorName, name="LArHitsTestTool", **kwargs):
    mlog = logging.getLogger(name+detectorName)
    mlog.debug('Start configuration')
    result = ComponentAccumulator()

    kwargs.setdefault("DetectorName", detectorName)
    LArHitsTestTool = CompFactory.LArHitsTestTool
    result.setPrivateTools(LArHitsTestTool(name, **kwargs))
    return result


def TileHitsTestToolCfg(testMBTS, name="TileHitsTestTool", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')
    result = ComponentAccumulator()

    kwargs.setdefault("TestMBTS", testMBTS)
    TileHitsTestTool = CompFactory.TileHitsTestTool
    result.setPrivateTools(TileHitsTestTool(name, **kwargs))
    return result


def CaloCalibrationHitsTestToolCfg(calibHitType, name="CaloCalibrationHitsTestTool", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')
    result = ComponentAccumulator()

    kwargs.setdefault("CalibHitType", calibHitType)
    CaloCalibrationHitsTestTool = CompFactory.CaloCalibrationHitsTestTool
    result.setPrivateTools(CaloCalibrationHitsTestTool(name, **kwargs))
    return result


def MDTHitsTestToolCfg(detectorName, name="MDTHitsTestTool", **kwargs):
    mlog = logging.getLogger(name+detectorName)
    mlog.debug('Start configuration')
    result = ComponentAccumulator()

    kwargs.setdefault("DetectorName", detectorName)
    MDTHitsTestTool = CompFactory.MDTHitsTestTool
    result.setPrivateTools(MDTHitsTestTool(name, **kwargs))
    return result


def RPCHitsTestToolCfg(detectorName, name="RPCHitsTestTool", **kwargs):
    mlog = logging.getLogger(name+detectorName)
    mlog.debug('Start configuration')
    result = ComponentAccumulator()

    kwargs.setdefault("DetectorName", detectorName)
    RPCHitsTestTool = CompFactory.RPCHitsTestTool
    result.setPrivateTools(RPCHitsTestTool(name, **kwargs))
    return result


def CSCHitsTestToolCfg(detectorName, name="CSCHitsTestTool", **kwargs):
    mlog = logging.getLogger(name+detectorName)
    mlog.debug('Start configuration')
    result = ComponentAccumulator()

    kwargs.setdefault("DetectorName", detectorName)
    CSCHitsTestTool = CompFactory.CSCHitsTestTool
    result.setPrivateTools(CSCHitsTestTool(name, **kwargs))
    return result


def TGCHitsTestToolCfg(detectorName, name="TGCHitsTestTool", **kwargs):
    mlog = logging.getLogger(name+detectorName)
    mlog.debug('Start configuration')
    result = ComponentAccumulator()

    kwargs.setdefault("DetectorName", detectorName)
    TGCHitsTestTool = CompFactory.TGCHitsTestTool
    result.setPrivateTools(TGCHitsTestTool(name, **kwargs))
    return result


def LucidHitsTestToolCfg(name="LucidHitsTestTool", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')
    result = ComponentAccumulator()

    LucidHitsTestTool = CompFactory.LucidHitsTestTool
    result.setPrivateTools(LucidHitsTestTool(name, **kwargs))
    return result


def ALFASimHitsTestToolCfg(name="ALFASimHitsTestTool", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')
    result = ComponentAccumulator()

    ALFASimHitsTestTool = CompFactory.ALFA_SimHitsTestTool
    result.setPrivateTools(ALFASimHitsTestTool(name, **kwargs))
    return result


def ZDCHitsTestToolCfg(name="ZDCHitsTestTool", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')
    result = ComponentAccumulator()

    ZDCHitsTestTool = CompFactory.ZDCHitsTestTool
    result.setPrivateTools(ZDCHitsTestTool(name, **kwargs))
    return result


def LayerTestToolCfg(collectionName, name="LayerTestTool", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')
    result = ComponentAccumulator()

    kwargs.setdefault("CollectionName", collectionName)
    LayerTestTool = CompFactory.LayerTestTool
    result.setPrivateTools(LayerTestTool(name, **kwargs))
    return result


def TruthTestToolCfg(mcEventKey, name="TruthTestTool", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')
    result = ComponentAccumulator()

    kwargs.setdefault("McEventKey", mcEventKey)
    TruthTestTool = CompFactory.TruthTestTool
    result.setPrivateTools(TruthTestTool(name=name, **kwargs))
    return result
