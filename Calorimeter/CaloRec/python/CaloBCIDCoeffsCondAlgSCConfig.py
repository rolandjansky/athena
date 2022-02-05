# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# File: CaloRec/python/CaloBCIDCoeffsCondAlgSCConfig.py
# Purpose: Configure CaloBCIDCoeffsCondAlgSC.

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def CaloBCIDCoeffsCondAlgSCCfg (flags,  name = 'CaloBCIDCoeffsCondAlgSC', **kwargs):
    acc = ComponentAccumulator()

    if not flags.Input.isMC:
        #Data case
        pass
    else:
        #MC case
        kwargs.setdefault("MCSymKey", '')

        from LArRecUtils.LArRecUtilsConfig import LArOFCSCCondAlgCfg
        acc.merge(LArOFCSCCondAlgCfg(flags))
        kwargs.setdefault("OFCKey", 'LArOFCSC')

        requiredConditions=["PileupAverageSC","ShapeSC"]
        from LArConfiguration.LArElecCalibDBConfig import LArElecCalibDBMCSCCfg
        acc.merge(LArElecCalibDBMCSCCfg(flags,requiredConditions))
        #conddb.addFolder("LAR_OFL","/LAR/ElecCalibMCSC/LArPileupAverage<tag>LARElecCalibMCSCLArPileupAverage-IOVDEP-00</tag>",className="CondAttrListCollection"
        kwargs.setdefault("MinBiasAvgKey", "LArPileupAverageSC")
        kwargs.setdefault("ShapeKey", "LArShapeSC")

        kwargs.setdefault("OutputCoeffsKey", 'CaloBCIDCoeffsSC')

        kwargs.setdefault("IsSuperCell", True)

        acc.addCondAlgo (CompFactory.CaloBCIDCoeffsCondAlg(name, **kwargs))

    return acc
