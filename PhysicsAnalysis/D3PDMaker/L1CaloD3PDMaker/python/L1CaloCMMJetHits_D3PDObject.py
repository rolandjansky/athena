# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# D3PD object saving L1Calo CMM Jet Hits
# @author John Morris <john.morris@cern.ch>
#

from D3PDMakerCoreComps.D3PDObject  import make_SGDataVector_D3PDObject
import EventCommonD3PDMaker
import L1CaloD3PDMaker

L1CaloCMMJetHits_D3PDObject = make_SGDataVector_D3PDObject('DataVector<LVL1::CMMJetHits>',
                                                           'CMMJetHits',
                                                           'L1CaloCMM_JetHits_',
                                                           'L1CaloCMMJetHits_D3PDObject')

#Level 0
L1CaloCMMJetHits_D3PDObject.defineBlock(0,'CMMJetHits',
                                        L1CaloD3PDMaker.L1CaloCMMJetHitsFillerTool)
