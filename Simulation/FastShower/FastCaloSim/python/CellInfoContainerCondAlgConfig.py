# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from LArGeoAlgsNV.LArGMConfig import LArGMCfg

def CellInfoContainerCondAlgCfg(configFlags):
    result = LArGMCfg(configFlags)
    result.addCondAlgo(CompFactory.CellInfoContainerCondAlg())
    return result


    
