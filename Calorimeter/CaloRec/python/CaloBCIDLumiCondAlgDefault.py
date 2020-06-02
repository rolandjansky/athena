# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# File: CaloRec/python/CaloBCIDLumiCondAlgDefault.py
# Created: Mar 2020, sss
# Purpose: Configure CaloBCIDLumiCondAlg.


from AthenaCommon.AlgSequence import AthSequencer
from AthenaConfiguration.ComponentFactory import CompFactory


def CaloBCIDLumiCondAlgDefault():
    from AthenaCommon.GlobalFlags import globalflags

    name = 'CaloBCIDLumiCondAlg'
    condSeq = AthSequencer ('AthCondSeq')

    if hasattr (condSeq, name):
        return getattr (condSeq, name)

    from CaloRec.CaloBCIDCoeffsCondAlgDefault import CaloBCIDCoeffsCondAlgDefault
    CaloBCIDCoeffsCondAlgDefault()

    if globalflags.DataSource()=='data':
        from LumiBlockComps.LuminosityCondAlgDefault import LuminosityCondAlgDefault
        LuminosityCondAlgDefault()
    else: #MC case
        from LumiBlockComps.BunchCrossingCondAlgDefault import BunchCrossingCondAlgDefault
        BunchCrossingCondAlgDefault()

    CaloBCIDLumiCondAlg = CompFactory.CaloBCIDLumiCondAlg # CaloRec
    alg = CaloBCIDLumiCondAlg (name,
                               CoeffsKey = 'CaloBCIDCoeffs',
                               BunchCrossingCondDataKey = 'BunchCrossingData',
                               LuminosityCondDataKey = 'LuminosityCondData',
                               isMC = globalflags.DataSource()!='data',
                               OutputLumiKey = 'CaloBCIDLumi')
    condSeq += alg
    return alg

    
