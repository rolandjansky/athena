# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# File: CaloRec/python/CaloBCIDLumiCondAlgSCDefault.py
# Created: Mar 2020, sss
# Purpose: Configure CaloBCIDLumiCondAlg.


from AthenaCommon.AlgSequence import AthSequencer
from AthenaConfiguration.ComponentFactory import CompFactory


def CaloBCIDLumiCondAlgSCDefault():
    from AthenaCommon.GlobalFlags import globalflags

    name = 'CaloBCIDLumiCondAlgSC'
    condSeq = AthSequencer ('AthCondSeq')

    if hasattr (condSeq, name):
        return getattr (condSeq, name)

    from CaloRec.CaloBCIDCoeffsCondAlgSCDefault import CaloBCIDCoeffsCondAlgSCDefault
    CaloBCIDCoeffsCondAlgSCDefault()

    if globalflags.DataSource()=='data':
        print("Not yet available")
        return None
        from LumiBlockComps.LuminosityCondAlgDefault import LuminosityCondAlgDefault
        LuminosityCondAlgDefault()
    else: #MC case
        from LumiBlockComps.BunchCrossingCondAlgDefault import BunchCrossingCondAlgDefault
        BunchCrossingCondAlgDefault()

    CaloBCIDLumiCondAlg = CompFactory.CaloBCIDLumiCondAlg # CaloRec
    alg = CaloBCIDLumiCondAlg (name,
                               CoeffsKey = 'CaloBCIDCoeffsSC',
                               BunchCrossingCondDataKey = 'BunchCrossingData',
                               LuminosityCondDataKey = 'LuminosityCondData',
                               isMC = globalflags.DataSource()!='data',
                               OutputLumiKey = 'CaloBCIDLumiSC')
    condSeq += alg
    return alg

    
