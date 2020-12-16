# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Import all needed modules:
import MuonD3PDMaker
from D3PDMakerCoreComps.D3PDObject   import make_SGDataVector_D3PDObject


# Create the configurable:
TrackRecordD3PDObject = \
           make_SGDataVector_D3PDObject ('TrackRecordCollection',
                                         'MuonEntryLayerFilter',
                                         'ms_entry_truth_',
                                         'TrackRecordD3PDObject',
                                         default_getterClass = MuonD3PDMaker.TrackRecordCollectionGetterTool)

# Add blocks to it:
TrackRecordD3PDObject.defineBlock( 0, "BasicInfo",
                                   MuonD3PDMaker.TrackRecordFillerTool )
TrackRecordD3PDObject.defineBlock( 0, "TruthHits",
                                   MuonD3PDMaker.MuonTruthHitsFillerTool )
