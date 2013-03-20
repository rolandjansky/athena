# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file PhysicsAnalysis/D3PDMaker/TrigTauD3PDMaker/python/TrigTauEgammaD3PD.py
# @author E. Ptacek <eptacek@uoregon.edu>
# @date April, 2010
# @brief Construct a tau and egamma trigger D3PD.
#

import D3PDMakerCoreComps

from TrigTauD3PDMaker.TrigTauD3PD         import TrigTauD3PDObjects
from TrigEgammaD3PDMaker.TrigEgammaD3PD   import TrigEgammaD3PDObjects

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

def TrigTauEgammaD3PD (file,
                       level = 10,
                       tuplename = 'trigtauegamma',
                       seq = topSequence):
    alg = D3PDMakerCoreComps.MakerAlg(tuplename, seq,
                                      file = file)
    TrigTauD3PDObjects(alg, 1)
    TrigEgammaD3PDObjects(alg, 1)
#    TrigTauD3PDObjects (alg, 1)
    return alg
