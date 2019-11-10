"""Configuration for the LAr Digit Thinner algorithm

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from LArROD.LArRODConf import LArDigitThinner
from LArByteStream.LArByteStreamConf import LArRawDataReadingAlg
from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg


def LArDigitThinnerCfg(flags, **kwargs):
    """Return ComponentAccumulator for LArDigitThinner algorithm"""
    # based on DefaultLArDigitThinner
    acc = ComponentAccumulator()

    if (not flags.Input.isMC) and flags.Input.Format == "bytestream":
        acc.merge(LArRawDataReadingAlg(InputContainerName="FREE"))

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

    acc.addEventAlgo(LArDigitThinner(**kwargs))
    return acc
