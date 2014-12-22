# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: TrigMuonD3PD.py 637271 2014-12-22 02:43:34Z ssnyder $

#
# Set up the trigger navigation flattening for the muon chains. Done in the
# same way as it was done for the egammaD3PD...
#
from AthenaCommon.AlgSequence import AlgSequence
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from D3PDMakerCoreComps.D3PDObject import make_Void_D3PDObject
InitTrigD3PDObject = make_Void_D3PDObject( default_name = 'InitTrigMuon' )

def _initTrigHook( c, **kw ):
    import TrigMuonD3PDMaker.Setup_RoILinksCnvTool_AllMuonChains
    preseq = AlgSequence( D3PDMakerFlags.PreD3PDAlgSeqName() )
    if not hasattr( preseq, 'FlatHltNaviMaker' ):
        from TriggerMenuAnalysis.TriggerMenuAnalysisConf import FlatHltNaviMaker
        preseq += FlatHltNaviMaker( 'FlatHltNaviMaker' )

    return

InitTrigD3PDObject.defineHook( _initTrigHook )

##
# @short Function for passing parameters to individual D3PDObjects
#
# This function helps in making it possible to pass specific options
# to the D3PDObjects added by the TrigEgammaD3PDObjects function.
# The formalist is the same as for all the other D3PDMaker function.
#
# @param level The overall detail level of the D3PDObject
# @param name  An object name to refer to this object later on
# @param kwin  Input keyworded parameters
# @param kw    Extra keywords provided here
# @returns The list of parameters to give to the D3PDObject
#
def _args( level, name, kwin, **kw ):
    result = kw.copy()
    result[ "level" ] = level
    for ( k, v ) in kwin.items():
        if k.startswith( name + "_" ):
            result[ k[ len( name ) + 1 : ] ] = v
            pass
        pass
    return result

##
# @short Function adding the Muon Trigger information to the D3PD
#
# The idea here is that one could add all the HLT muon information to his/her
# D3PD with something like this:
#
#  <code>
#    from TrigMuonD3PDMaker.TrigMuonD3PD import TrigMuonD3PDObjects
#    TrigMuonD3PDObjects( myd3pdalg, level = 1, addNaviInfo = False )
#  </code>
#
# $Revision: 637271 $
# $Date: 2014-12-22 03:43:34 +0100 (Mon, 22 Dec 2014) $
#
def TrigMuonD3PDObjects( d3pdalg, level = 10, addNaviInfo = True, addConfigInfo = True,
                         **kw ):

    #
    # Add the LVL1 information if it's not added yet:
    #
    if not hasattr( d3pdalg, "trig_L1_mu_Filler" ):
        from TriggerD3PDMaker.MuonROID3PDObject import MuonROID3PDObject
        d3pdalg += MuonROID3PDObject( **_args( level, "MuonROI", kw ) )

    #
    # Configure the navigation flattening if it's requested:
    #
    if addNaviInfo:
        d3pdalg += InitTrigD3PDObject( level )

    #
    # Add the LVL2 information:
    # MuonFeature not available in xAOD.
    #
    #from TrigMuonD3PDMaker.MuonFeatureD3PDObject import MuonFeatureD3PDObject
    #d3pdalg += MuonFeatureD3PDObject( **_args( level, "MuonFeature", kw ) )

    #from TrigMuonD3PDMaker.MuonFeatureDetailsD3PDObject import MuonFeatureDetailsD3PDObject
    #d3pdalg += MuonFeatureDetailsD3PDObject( **_args( level, "MuonFeatureDetails", kw ) )

    #from TrigMuonD3PDMaker.CombinedMuonFeatureD3PDObject import CombinedMuonFeatureD3PDObject
    #d3pdalg += CombinedMuonFeatureD3PDObject( **_args( level, "CombinedMuonFeature", kw,
    #                                                    exclude = [ "MFInfo", "IDInfo" ] ) )

    #from TrigMuonD3PDMaker.IsoMuonFeatureD3PDObject import IsoMuonFeatureD3PDObject
    #d3pdalg += IsoMuonFeatureD3PDObject( **_args( level, "IsoMuonFeature", kw ) )

    #from TrigMuonD3PDMaker.TileMuFeatureD3PDObject import TileMuFeatureD3PDObject
    #d3pdalg += TileMuFeatureD3PDObject( **_args( level, "TileMuFeature", kw ) )

    #from TrigMuonD3PDMaker.TileTrackMuFeatureD3PDObject import TileTrackMuFeatureD3PDObject
    #d3pdalg += TileTrackMuFeatureD3PDObject( **_args( level, "TileTrackMuFeature", kw,
    #                                                  exclude = [ "TileMuInfo" ] ) )

    #
    # Add the EF information:
    #
    from TrigMuonD3PDMaker.TrigMuonEFInfoD3PDObject import TrigMuonEFInfoD3PDObject
    d3pdalg += TrigMuonEFInfoD3PDObject( **_args( level, "TrigMuonEF", kw ) )
    # d3pdalg += TrigMuonEFInfoD3PDObject( **_args( level, "TrigMuGirl", kw,
    #                                               sgkey = "HLT_eMuonEFInfo",
    #                                               prefix = "trig_EF_trigmugirl_",
    #                                               exclude = [ "Source" ] ) )

    # from TrigMuonD3PDMaker.TrigMuonEFIsolationD3PDObject import TrigMuonEFIsolationD3PDObject
    # d3pdalg += TrigMuonEFIsolationD3PDObject( **_args (level,
    #                                                    "TrigMuonEFIsolation",
    #                                                    kw) )

    # TrigInDetTrack not in xAOD.
    # #
    # # Add separate LVL2 tracking information:
    # #
    # from RecExConfig.ObjKeyStore import cfgKeyStore
    # from TrigInDetD3PDMaker.TrigInDetTrackD3PDObject import TrigInDetTrackD3PDObject
    # d3pdalg += TrigInDetTrackD3PDObject( **_args( level, "TrigSiTrack_Muon", kw,
    #                                               sgkey = 'HLT_TrigSiTrack_Muon',
    #                                               prefix = 'trig_L2_sitrack_muon_',
    #                                               allowMissing = True ) )
    # d3pdalg += TrigInDetTrackD3PDObject( **_args( level, "TrigIDSCAN_Muon", kw,
    #                                               sgkey = 'HLT_TrigIDSCAN_Muon',
    #                                               prefix = 'trig_L2_idscan_muon_' ) )
    # if cfgKeyStore.isInInputFile( "TrigInDetTrackCollection",
    #                               "HLT_TrigL2SiTrackFinder_Muon" ):
    #     d3pdalg += TrigInDetTrackD3PDObject( **_args( level, "TrigL2SiTrackFinder_Muon", kw,
    #                                                   sgkey = 'HLT_TrigL2SiTrackFinder_Muon',
    #                                                   prefix = 'trig_L2_sitrackfinder_muon_' ) )
    #     pass

    # d3pdalg += TrigInDetTrackD3PDObject( **_args( level, "TrigSiTrack_muonIso", kw,
    #                                               sgkey = 'HLT_TrigSiTrack_muonIso',
    #                                               prefix = 'trig_L2_sitrack_isomuon_',
    #                                               allowMissing = True ) )
    # d3pdalg += TrigInDetTrackD3PDObject( **_args( level, "TrigIDSCAN_muonIso", kw,
    #                                               sgkey = 'HLT_TrigIDSCAN_muonIso',
    #                                               prefix = 'trig_L2_idscan_isomuon_' ) )
    # if cfgKeyStore.isInInputFile( "TrigInDetTrackCollection",
    #                               "HLT_TrigL2SiTrackFinder_muonIso" ):
    #     d3pdalg += TrigInDetTrackD3PDObject( **_args( level, "TrigL2SiTrackFinder_muonIso", kw,
    #                                                   sgkey = 'HLT_TrigL2SiTrackFinder_muonIso',
    #                                                   prefix = 'trig_L2_sitrackfinder_isomuon_' ) )
    #     pass

    # if cfgKeyStore.isInInputFile( "TrigInDetTrackCollection",
    #                               "HLT_TrigL2SiTrackFinder" ):
    #     d3pdalg += TrigInDetTrackD3PDObject( **_args( level, "TrigL2SiTrackFinder", kw,
    #                                                   sgkey = 'HLT_TrigL2SiTrackFinder',
    #                                                   prefix = 'trig_L2_sitrackfinder_' ) )
    #     pass

    # #
    # # Add RoiDescriptor (back-extrapoltaion of mufast tracks)
    # #
    # from TriggerD3PDMaker.TrigRoiDescD3PDObject import TrigRoiDescD3PDObject
    # d3pdalg += TrigRoiDescD3PDObject( **_args( level, "RoIDescID", kw,
    #                                            sgkey  = 'HLT_forID',
    #                                            prefix = 'trig_roidescriptor_forID_' ) )

    # # Trigger navigation info
    # # note: level for CombLinksD3PDObjects should be >=1, otherwise navigation info will not be filled
    # if addNaviInfo:
    #     from TriggerD3PDMaker.TrigNaviD3PDObject import \
    #          ChainEntryD3PDObject, CombLinksD3PDObjects
    #     if not hasattr( d3pdalg, 'trig_Nav_Filler' ):
    #         d3pdalg += ChainEntryD3PDObject( **_args( 1, "ChainEntry", kw ) )
    #     d3pdalg += CombLinksD3PDObjects[ 'L2_mu' ]( **_args( 1, "CombLinksL2Mu", kw ) )
    #     d3pdalg += CombLinksD3PDObjects[ 'EF_mu' ]( **_args( 1, "CombLinksEFMu", kw ) )
    #     d3pdalg += CombLinksD3PDObjects[ 'L2_TileMu' ]( **_args( 1, "CombLinksL2TileMu", kw ) )
    #     d3pdalg += CombLinksD3PDObjects[ 'EF_TileMu' ]( **_args( 1, "CombLinksEFTileMu", kw ) )

    # # Add the trigger configuration metadata:
    # if addConfigInfo:
    #     from TriggerD3PDMaker.TrigConfMetadata import addTrigConfMetadata
    #     addTrigConfMetadata( d3pdalg )
    
    return d3pdalg

##
# @short Function creating the muon trigger D3PD
#
# This function can be used to create a stand-alone muon trigger D3PD making
# algorithm. Then the user can add additional tools to the algorithm if
# (s)he wishes.
#
# $Revision: 637271 $
# $Date: 2014-12-22 03:43:34 +0100 (Mon, 22 Dec 2014) $
#
def TrigMuonD3PD( file,
                  level = 10,
                  tuplename = 'trigger',
                  seq = AlgSequence() ):
    import D3PDMakerCoreComps
    alg = D3PDMakerCoreComps.MakerAlg( tuplename, seq,
                                       file = file )
    TrigMuonD3PDObjects( alg, level )
    return alg
