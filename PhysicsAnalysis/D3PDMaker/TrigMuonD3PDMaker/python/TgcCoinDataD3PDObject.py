# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# D3PD object saving information about TgcCoinData.
#
import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.D3PDObject import make_SG_D3PDObject
from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
from D3PDMakerCoreComps.IndexAssociation import IndexAssociation
import TrigMuonD3PDMaker

from RecExConfig.ObjKeyStore import cfgKeyStore
if cfgKeyStore.isInInput( "Muon::TgcCoinDataContainer", "TrigT1CoinDataCollection" ):
    _haveRDO = True
else:
    _haveRDO = False
    from AthenaCommon.Logging import logging
    _mlog = logging.getLogger( "TgcCoinDataD3PDObject" )
    _mlog.warning( "No Muon::TgcCoinDataContainer object in input; skipping." )



TgcCoinDataD3PDObject = make_SG_D3PDObject( "Muon::TgcCoinDataContainer",
                                            "TrigT1CoinDataCollection",
                                            "tgc_",
                                            "TgcCoinDataD3PDObject",
                                            default_allowMissing = not _haveRDO )

if _haveRDO:
#    def _initHook( c, **kw ):
#        print "KANNO:DEBUG print c"
#        print c
#        print "KANNO:DEBUG print c end"
#        print "KANNO:DEBUG print kw"
#        print kw['name']
#        print "KANNO:DEBUG print kw end"
#        _tmp1 = getattr( c,     kw['name']+"_TgcSLAssoc" )
#        _tmp2 = getattr( _tmp1, kw['name']+"_TgcSLAssoc_TgcSLHierarchyIndex" )
#        _tmp3 = getattr( _tmp2, kw['name']+"_TgcSLAssoc_TgcSLHierarchyIndexIndex" )
#        _tmp3.Target = kw['prefix']+"tracklet_"

#"d3pdalg.tgc_nextBC_Filler.tgc_nextBC_Filler_TgcSLAssoc.tgc_nextBC_Filler_TgcSLAssoc_TgcSLHierarchyIndex.tgc_nextBC_Filler_TgcSLAssoc_TgcSLHierarchyIndexIndex.Target"

    #
    # Associate TgcCoinData objects to TrigT1CoinDataCollection objects:
    # for SL
    TgcSLAssoc = \
       ContainedVectorMultiAssociation( TgcCoinDataD3PDObject,
                                        TrigMuonD3PDMaker.TgcCoinDataContainerAssociationTool,
                                        AssocSL = True, AssocHpt = False, prefix = "sl_",
                                        blockname = "TgcSLAssoc" )

    TgcSLAssoc.defineBlock( 1, "BasicInfo",
                            TrigMuonD3PDMaker.TgcCoinDataFillerTool,
                            WriteSL = True )

    TgcSLHierarchyAssoc = SimpleAssociation( TgcSLAssoc,
                                             TrigMuonD3PDMaker.TgcCoinHierarchyAssociationTool,
                                             prefix = "motherwiretracklet_",
                                             blockname = "TgcSLHierarchyAssoc" )
    TgcSLHierarchyAssoc.defineBlock( 1, "HierarchyInfo",
                                     TrigMuonD3PDMaker.TgcCoinDataFillerTool,
                                     WriteTrackletHpt = True )

    #TgcSLHierarchyIndex = IndexAssociation( TgcSLAssoc,
    #                                        TrigMuonD3PDMaker.TgcCoinHierarchyAssociationTool,
    #                                        target = "tgc_tracklet_", prefix = "motherwiretracklet_", level = 1,
    #                                        blockname = "TgcSLHierarchyIndex", allowMissing = True )

    # for Hpt
    TgcHptAssoc = \
       ContainedVectorMultiAssociation( TgcCoinDataD3PDObject,
                                        TrigMuonD3PDMaker.TgcCoinDataContainerAssociationTool,
                                        AssocSL = False, AssocHpt = True, prefix = "hpt_",
                                        blockname = "TgcHptAssoc" )

    TgcHptAssoc.defineBlock( 1, "BasicInfo",
                             TrigMuonD3PDMaker.TgcCoinDataFillerTool,
                             WriteTrackletHpt = True )

    # for Tracklet
    TgcTrackletAssoc = \
       ContainedVectorMultiAssociation( TgcCoinDataD3PDObject,
                                        TrigMuonD3PDMaker.TgcCoinDataContainerAssociationTool,
                                        AssocSL = False, AssocHpt = False, prefix = "tracklet_",
                                        blockname = "TgcTrackletAssoc" )

    TgcTrackletAssoc.defineBlock( 1, "BasicInfo",
                                  TrigMuonD3PDMaker.TgcCoinDataFillerTool,
                                  WriteTrackletHpt = True )
