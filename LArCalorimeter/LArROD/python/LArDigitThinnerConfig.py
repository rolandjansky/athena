"""Configuration for the LAr Digit Thinner algorithm

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg


def LArDigitThinnerCfg(flags, **kwargs):
    """Return ComponentAccumulator for LArDigitThinner algorithm"""
    # based on DefaultLArDigitThinner
    acc = ComponentAccumulator()


    if (not flags.Input.isMC) and (not flags.Overlay.DataOverlay):
        kwargs.setdefault("InputContainerName","FREE")
        if flags.Input.Format == "BS":
            from LArByteStream.LArRawDataReadingConfig import LArRawDataReadingCfg
            acc.merge(LArRawDataReadingCfg(flags,LArDigitKey="FREE"))
    

    acc.merge(LArOnOffIdMappingCfg(flags))

    if flags.Beam.Type == "cosmics":
        kwargs.setdefault("EnergyCuts_Barrel", [1000, 500, 1000, 1000])
        kwargs.setdefault("EnergyCuts_Endcap", [1000, 500, 1000, 1000])
        kwargs.setdefault("EnergyCut_HEC", 2000)
        kwargs.setdefault("EnergyCut_FCAL", 3000)
    else:
        kwargs.setdefault("EnergyCuts_Barrel", [1000, 1000, 1000, 1000])
        kwargs.setdefault("EnergyCuts_Endcap", [2000, 2000, 2000, 2000])
        kwargs.setdefault("EnergyCut_HEC", 5000)
        kwargs.setdefault("EnergyCut_FCAL", 20000)

    acc.addEventAlgo(CompFactory.LArDigitThinner(**kwargs))
    return acc
