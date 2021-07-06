# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import MuonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags

from MuonD3PDMaker.MuonD3PDMakerFlags    import MuonD3PDFlags
from RecExConfig.RecFlags import rec

MuonSegmentD3PDObject = \
           make_SGDataVector_D3PDObject ('Trk::SegmentCollection',
                                         D3PDMakerFlags.MuonSegmentSGKey(),
                                         'mu_seg_', 'MuonSegmentD3PDObject')

MuonSegmentD3PDObject.defineBlock (1, 'Location',
                            MuonD3PDMaker.MuonSegmentLocationFillerTool)
MuonSegmentD3PDObject.defineBlock (1, 'Author',
                            MuonD3PDMaker.MuonSegmentAuthorFillerTool)
MuonSegmentD3PDObject.defineBlock (1, 'FitQuality',
                            MuonD3PDMaker.MuonSegmentFitQualityFillerTool)

# Include the chamber t0 for the relevant MuonSegment collection, leave out the other 
MuonSegmentD3PDObject.defineBlock (1, 'MuonboySegmentT0',
                            MuonD3PDMaker.MuonSegmentT0FillerTool,doMuonBoyCSCTiming=True)

MuonSegmentD3PDObject.defineBlock (1, 'MooreSegmentT0',
                            MuonD3PDMaker.MuonSegmentT0FillerTool,doMuonBoyCSCTiming=False)

if rec.doTruth() and MuonD3PDFlags.doSegmentTruth():
    MuonSegmentD3PDObject.defineBlock (1, 'PatternTruthCombination',
                                      MuonD3PDMaker.MuonSegmentTruthFillerTool)

