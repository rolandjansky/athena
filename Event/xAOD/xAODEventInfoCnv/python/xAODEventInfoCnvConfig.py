"""Define methods to construct configured EventInfo conversion algorithms

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def EventInfoCnvAlgCfg(flags, name="EventInfoCnvAlg",
                       inputKey="McEventInfo",
                       outputKey="EventInfo", **kwargs):
    """Return a ComponentAccumulator for EventInfoCnvAlg algorithm"""

    acc = ComponentAccumulator()

    kwargs.setdefault("AODKey", inputKey)
    kwargs.setdefault("xAODKey", outputKey)

    # TODO: luminosity

    xAODMaker__EventInfoCnvAlg=CompFactory.xAODMaker__EventInfoCnvAlg
    alg = xAODMaker__EventInfoCnvAlg(name, **kwargs)
    acc.addEventAlgo(alg)

    return acc


def EventInfoOverlayAlgCfg(flags, name="EventInfoOverlay", **kwargs):
    """Return a ComponentAccumulator for EventInfoOverlay algorithm"""

    acc = ComponentAccumulator() 

    kwargs.setdefault("BkgInputKey", flags.Overlay.BkgPrefix + "EventInfo")
    kwargs.setdefault("SignalInputKey", flags.Overlay.SigPrefix + "EventInfo")
    kwargs.setdefault("OutputKey", "EventInfo")

    kwargs.setdefault("DataOverlay", flags.Overlay.DataOverlay)

    # Do the xAOD::EventInfo overlay
    xAODMaker__EventInfoOverlay=CompFactory.xAODMaker__EventInfoOverlay
    alg = xAODMaker__EventInfoOverlay(name, **kwargs)
    acc.addEventAlgo(alg)

    # Re-map signal address
    from SGComps.AddressRemappingConfig import AddressRemappingCfg
    acc.merge(AddressRemappingCfg([
        "xAOD::EventInfo#EventInfo->" + flags.Overlay.SigPrefix + "EventInfo",
        "xAOD::EventAuxInfo#EventInfoAux.->" + flags.Overlay.SigPrefix + "EventInfoAux.",
    ]))

    return acc


def EventInfoOverlayOutputCfg(flags, **kwargs):
    """Return event info overlay output configuration"""

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc = OutputStreamCfg(flags, "RDO")

    # Add signal output
    if flags.Output.doWriteRDO_SGNL:
        outConfig = OutputStreamCfg(flags, "RDO_SGNL",
                                    ItemList=[
                                        "xAOD::EventInfo#Sig_EventInfo",
                                        "xAOD::EventAuxInfo#Sig_EventInfoAux."
                                    ])
        acc.merge(outConfig)

    return acc


def EventInfoOverlayCfg(flags, **kwargs):
    """Return a ComponentAccumulator for the full EventInfoOverlay algorithm accumulator"""

    acc = ComponentAccumulator()

    # Check if running on legacy HITS
    if "EventInfo" not in flags.Input.Collections and "EventInfo" not in flags.Input.SecondaryCollections:
        acc.merge(EventInfoCnvAlgCfg(flags,
                                     inputKey=flags.Overlay.SigPrefix+"McEventInfo",
                                     outputKey=flags.Overlay.SigPrefix+"EventInfo",
                                     **kwargs))
        # Re-map signal address
        from SGComps.AddressRemappingConfig import AddressRemappingCfg
        acc.merge(AddressRemappingCfg([
            "EventInfo#McEventInfo->" + flags.Overlay.SigPrefix + "McEventInfo",
        ]))

    acc.merge(EventInfoOverlayAlgCfg(flags, **kwargs))
    acc.merge(EventInfoOverlayOutputCfg(flags, **kwargs))
    return acc
