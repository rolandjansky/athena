"""Configuration for the LAr Digit Thinner algorithm

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType, Format
from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg
from LArConfiguration.LArConfigFlags import RawChannelSource 

def LArDigitThinnerCfg(flags, **kwargs):
    """Return ComponentAccumulator for LArDigitThinner algorithm"""
    # based on DefaultLArDigitThinner
    acc = ComponentAccumulator()

    if not flags.Input.isMC and not flags.Overlay.DataOverlay:
        kwargs.setdefault("InputContainerName","FREE")
        if flags.Input.Format is Format.BS:
            from LArByteStream.LArRawDataReadingConfig import LArRawDataReadingCfg
            acc.merge(LArRawDataReadingCfg(flags,LArDigitKey="FREE"))


    acc.merge(LArOnOffIdMappingCfg(flags))

    if flags.Beam.Type is BeamType.Cosmics:
        kwargs.setdefault("EnergyCuts_Barrel", [1000, 500, 1000, 1000])
        kwargs.setdefault("EnergyCuts_Endcap", [1000, 500, 1000, 1000])
        kwargs.setdefault("EnergyCut_HEC", 2000)
        kwargs.setdefault("EnergyCut_FCAL", 3000)
    else:
        kwargs.setdefault("EnergyCuts_Barrel", [1000, 1000, 1000, 1000])
        kwargs.setdefault("EnergyCuts_Endcap", [2000, 2000, 2000, 2000])
        kwargs.setdefault("EnergyCut_HEC", 5000)
        kwargs.setdefault("EnergyCut_FCAL", 20000)

    if flags.LAr.RawChannelSource is RawChannelSource.Calculated:
        kwargs.setdefault("RawChannelContainerName","LArRawChannels_FromDigits")

    acc.addEventAlgo(CompFactory.LArDigitThinner(**kwargs))
    return acc
