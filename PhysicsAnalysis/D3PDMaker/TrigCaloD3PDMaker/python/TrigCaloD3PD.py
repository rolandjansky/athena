# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import D3PDMakerCoreComps
from TrigCaloD3PDMaker.TrigEMClusterD3PDObject import TrigEMClusterD3PDObject
from TrigCaloD3PDMaker.TrigEMRingerD3PDObject import TrigEMRingerD3PDObject

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

def TrigCaloD3PD(fname, tname='t', seq=topSequence):
    alg = D3PDMakerCoreComps.MakerAlg(tname, seq=seq, file=fname)
    alg += TrigEMClusterD3PDObject(3)
    alg += TrigEMRingerD3PDObject(3)
    return alg
