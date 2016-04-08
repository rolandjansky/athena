# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import D3PDMakerCoreComps
from HSG2DPDUtils.H4lD3PDObject import H4lD3PDObject

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


def H4lD3PD (file,
             level = 10,
             tuplename = 'h4ld3pd',
             seq = topSequence,
             D3PDSvc = 'D3PD::RootD3PDSvc'):
    
    alg = D3PDMakerCoreComps.MakerAlg(tuplename, seq,
                                      file = file, D3PDSvc = D3PDSvc)
    alg += H4lD3PDObject (level)
    return alg



