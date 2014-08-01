# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import MuonD3PDMaker
import D3PDMakerCoreComps
import EventCommonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags

from RecExConfig.RecFlags import rec

MuonSegmentTruthD3PDObject = \
                   make_SGDataVector_D3PDObject ('Trk::SegmentCollection',
                                                 'MuonSegments',
                                                 'mu_segmentTruth_', 'MuonSegmentTruthD3PDObject')

MuonSegmentTruthD3PDObject.defineBlock (1, 'PatternTruthCombination',
                                        MuonD3PDMaker.MuonSegmentTruthFillerTool)

