# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def LArSuperCellBuilderAlgCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    from LArCabling.LArCablingConfig import LArOnOffIdMappingSCCfg
    acc.merge(LArOnOffIdMappingSCCfg(flags))
    kwargs.setdefault("CablingKey", 'LArOnOffIdMapSC')

    from LArRecUtils.LArADC2MeVSCCondAlgConfig import LArADC2MeVSCCondAlgCfg
    acc.merge(LArADC2MeVSCCondAlgCfg(flags))
    kwargs.setdefault("ADC2MeVKey", 'LArADC2MeVSC')

    requiredConditions=["ShapeSC","PedestalSC"]
    from LArConfiguration.LArElecCalibDBConfig import LArElecCalibDBMCSCCfg
    acc.merge(LArElecCalibDBMCSCCfg(flags,requiredConditions))
    kwargs.setdefault("ShapeKey", 'LArShapeSC') # Provided by LArFlatConditionsAlg<LArShapeSC>
    kwargs.setdefault("PedestalKey", 'LArPedestalSC') # Provided by LArFlatConditionsAlg<LArPedestalSC>

    from LArRecUtils.LArRecUtilsConfig import LArOFCSCCondAlgCfg
    acc.merge(LArOFCSCCondAlgCfg(flags))
    kwargs.setdefault("OFCKey", 'LArOFCSC') # Provided by LArOFCSCCondAlg

    kwargs.setdefault("CaloCellKey", 'SCellnoBCID') # Output

    kwargs.setdefault("LArDigitKey", 'LArDigitSCL2') # TODO Understand why this isn't LArDigitSCL1

    kwargs.setdefault("useDB", False)
    kwargs.setdefault("IsSuperCell", True)
    kwargs.setdefault("ECutFortQ",  -1024)
    acc.addEventAlgo(CompFactory.LArRawChannelBuilderAlg(name="LArSuperCellBuilderAlg", **kwargs))
    return acc


def LArSuperCellBCIDAlgCfg(flags, name = "LArSuperCellBCIDAlgDefault", **kwargs):
    acc = ComponentAccumulator()
    from LArCabling.LArCablingConfig import LArOnOffIdMappingSCCfg
    acc.merge(LArOnOffIdMappingSCCfg(flags))
    kwargs.setdefault("CablingKey", 'LArOnOffIdMapSC') # CHECK
    from CaloRec.CaloBCIDAvgAlgSCConfig import CaloBCIDAvgAlgSCCfg
    acc.merge(CaloBCIDAvgAlgSCCfg(flags))
    kwargs.setdefault("BCIDAvgKey", "StoreGateSvc+CaloBCIDAverageSC") # Provided by CaloBCIDAvgAlgSC
    kwargs.setdefault("SCellContainerIn", "SCellnoBCID") # Provided by LArSuperCellBuilderAlg
    kwargs.setdefault("SCellContainerOut", "SCell") # Output
    acc.addEventAlgo(CompFactory.LArSuperCellBCIDAlg(name, **kwargs))
    return acc

