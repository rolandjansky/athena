# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: TrigMuonEFIsolationD3PDObject.py 501675 2012-05-21 13:59:19Z krasznaa $
#
# D3PD object saving information about the TrigMuonEFIsolation objects.
#

from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from RecExConfig.RecFlags import rec

from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.IndexAssociation import IndexAssociation
import TrigMuonD3PDMaker
import D3PDMakerCoreComps

TrigMuonEFIsolationD3PDObject = \
    make_SGDataVector_D3PDObject( "TrigMuonEFIsolationContainer",
                                  "HLT_MuonEFIsolation",
                                  "trig_EF_trigmuonefisolation_",
                                  "TrigMuonEFIsolationD3PDObject" )

TrigMuonEFIsolationD3PDObject.defineBlock(
    0, "BasicInfo",
    TrigMuonD3PDMaker.TrigMuonEFIsolationFillerTool,
    SavePtCone = True, SaveTrackPosition = True )

TrigMuonEFIsolationD3PDObject.defineBlock(
    1, "CaloInfo",
    TrigMuonD3PDMaker.TrigMuonEFIsolationFillerTool,
    SaveEtCone = True )

# This is already added by the basic filler tool:
#TrigMuonEFIsolationD3PDObject.defineBlock(
#    0, "EFInfoTrackIndex",
#    TrigMuonD3PDMaker.TrigMuonEFIsolationEFInfoTrackIndexFillerTool )

EFInfoIndexAssoc = \
    IndexAssociation( TrigMuonEFIsolationD3PDObject,
                      TrigMuonD3PDMaker.TrigMuonEFIsolationEFInfoAssocTool,
                      target = "trig_EF_trigmuonef_", prefix = "efinfo_", level = 0,
                      blockname = "EFInfoIndex", allowMissing = True,
                      UseTrigNav = rec.doTruth() )

EFMGInfoIndexAssoc = \
    IndexAssociation( TrigMuonEFIsolationD3PDObject,
                      TrigMuonD3PDMaker.TrigMuonEFIsolationEFInfoAssocTool,
                      target = "trig_EF_trigmugirl_", prefix = "efinfoMG_", level = 0,
                      blockname = "EFMGInfoIndex", allowMissing = True,
                      UseTrigNav = rec.doTruth() )
