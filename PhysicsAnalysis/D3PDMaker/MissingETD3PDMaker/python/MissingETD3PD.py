# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import D3PDMakerCoreComps
from MissingETD3PDMaker.MissingETD3PDObject import MissingETD3PDObject

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

def MissingETD3PD (file,
                   level = 10,
                   tuplename = 'METD3PD',
                   seq = topSequence,
                   D3PDSvc = 'D3PD::RootD3PDSvc'):

    alg = D3PDMakerCoreComps.MakerAlg(tuplename, seq,
                                      file = file, D3PDSvc = D3PDSvc)

    alg += MissingETD3PDObject (level=level, allowMissing = True, exclude=['MET_Base', 'MET_Truth_Int', 'MET_RefFinal_Phi', 'MET_MuonBoy_Et', 'MET_RefJet_SumEt'])

    return alg
