# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# File: CaloRec/python/CaloBCIDAvgAlgSCConfig.py
# Purpose: Configure CaloBCIDAvgAlgSC.

from __future__ import print_function
from AthenaConfiguration.ComponentFactory import CompFactory


from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def CaloBCIDAvgAlgSCCfg (flags, sequence=None):
    result = ComponentAccumulator()

    from CaloRec.CaloBCIDLumiCondAlgSCConfig import CaloBCIDLumiCondAlgSCCfg
    result.merge (CaloBCIDLumiCondAlgSCCfg (flags))
    result.addEventAlgo(CompFactory.CaloBCIDAvgAlg(
        name="CaloBCIDAvgAlgSC",
        IsSuperCell=True,
        BCIDLumiKey="CaloBCIDLumiSC",
        WriteKey="CaloBCIDAverageSC"), sequenceName=sequence)
    return result
