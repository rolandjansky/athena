"""Define methods to construct configured SCT overlay algorithms

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def SCTRawDataProviderAlgCfg(flags, name="SCTRawDataProvider", **kwargs):
    """Return a ComponentAccumulator for SCT raw data provider"""
    # Temporary until available in the central location
    acc = ComponentAccumulator()

    kwargs.setdefault("RDOKey", flags.Overlay.BkgPrefix + "SCT_RDOs")
    kwargs.setdefault("LVL1IDKey", flags.Overlay.BkgPrefix + "SCT_LVL1ID")
    kwargs.setdefault("BCIDKey", flags.Overlay.BkgPrefix + "SCT_BCID")

    SCTRawDataProvider = CompFactory.SCTRawDataProvider
    alg = SCTRawDataProvider(name, **kwargs)
    acc.addEventAlgo(alg)

    # load the SCTEventFlagWriter
    SCT_ConfigurationConditionsTool = CompFactory.SCT_ConfigurationConditionsTool
    acc.addPublicTool(SCT_ConfigurationConditionsTool())

    channelFolder = "/SCT/DAQ/Config/Chip"
    moduleFolder = "/SCT/DAQ/Config/Module"
    murFolder = "/SCT/DAQ/Config/MUR"
    SCT_ConfigurationCondAlg = CompFactory.SCT_ConfigurationCondAlg
    acc.addCondAlgo(SCT_ConfigurationCondAlg(ReadKeyChannel = channelFolder,
                                             ReadKeyModule = moduleFolder,
                                             ReadKeyMur = murFolder))
    from IOVDbSvc.IOVDbSvcConfig import addFolders
    acc.merge(addFolders(flags, [channelFolder, moduleFolder, murFolder], "SCT", className="CondAttrListVec"))

    SCTEventFlagWriter = CompFactory.SCTEventFlagWriter
    alg = SCTEventFlagWriter()
    acc.addEventAlgo(alg)

    return acc


def SCTDataOverlayExtraCfg(flags, **kwargs):
    """Return a ComponentAccumulator with SCT data overlay specifics"""
    acc = ComponentAccumulator()

    # We need to convert BS to RDO for data overlay
    acc.merge(SCTRawDataProviderAlgCfg(flags))

    # Add SCT cabling conditions
    from SCT_Cabling.SCT_CablingConfig import SCT_CablingCondAlgCfg
    acc.merge(SCT_CablingCondAlgCfg(flags))

    return acc


def SCTOverlayAlgCfg(flags, name="SCTOverlay", **kwargs):
    """Return a ComponentAccumulator for SCTOverlay algorithm"""
    acc = ComponentAccumulator()

    kwargs.setdefault("BkgInputKey", flags.Overlay.BkgPrefix + "SCT_RDOs")
    kwargs.setdefault("SignalInputKey", flags.Overlay.SigPrefix + "SCT_RDOs")
    kwargs.setdefault("OutputKey", "SCT_RDOs")

    # Do SCT overlay
    SCTOverlay = CompFactory.SCTOverlay
    alg = SCTOverlay(name, **kwargs)
    acc.addEventAlgo(alg)

    # Setup output
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
        "SCT_RDO_Container#SCT_RDOs"
    ]))

    return acc


def SCTTruthOverlayCfg(flags, name="SCTSDOOverlay", **kwargs):
    """Return a ComponentAccumulator for the SCT SDO overlay algorithm"""
    acc = ComponentAccumulator()

    # We do not need background SCT SDOs
    kwargs.setdefault("BkgInputKey", "")

    kwargs.setdefault("SignalInputKey",
                      flags.Overlay.SigPrefix + "SCT_SDO_Map")
    kwargs.setdefault("OutputKey", "SCT_SDO_Map")

    # Do SCT truth overlay
    InDetSDOOverlay = CompFactory.InDetSDOOverlay
    alg = InDetSDOOverlay(name, **kwargs)
    acc.addEventAlgo(alg)

    # Setup output
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
        "InDetSimDataCollection#SCT_SDO_Map"
    ]))

    return acc


def SCTOverlayCfg(flags):
    """Configure and return a ComponentAccumulator for SCT overlay"""
    acc = ComponentAccumulator()

    # Add data overlay specifics
    if flags.Overlay.DataOverlay:
        acc.merge(SCTDataOverlayExtraCfg(flags))

    # Add SCT overlay digitization algorithm
    from SCT_Digitization.SCT_DigitizationConfigNew import SCT_OverlayDigitizationBasicCfg
    acc.merge(SCT_OverlayDigitizationBasicCfg(flags))
    # Add SCT overlay algorithm
    acc.merge(SCTOverlayAlgCfg(flags))
    # Add SCT truth overlay
    acc.merge(SCTTruthOverlayCfg(flags))

    return acc
