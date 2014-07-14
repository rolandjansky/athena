# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: TrigBJetD3PD.py 528259 2012-11-29 15:09:12Z ssnyder $
#
# This file holds a helper function to add all the b-jet trigger information to
# a D3PD.

#
# Make sure that the navigation flattening algorithm is called with the correct
# configuration befor the D3PDMaker code is executed:
#
from D3PDMakerCoreComps.D3PDObject import make_Void_D3PDObject
InitTrigD3PDObject = make_Void_D3PDObject( default_name = 'InitTrigBjet' )
def _initTrigHook( c, **kw ):
    from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
    cfg =  TriggerConfigGetter()

    from TriggerMenuAnalysis.TriggerMenuAnalysisConf import RoILinksCnvTool
    from AthenaCommon.AppMgr import ToolSvc
    if not hasattr( ToolSvc, 'RoILinksCnvTool' ):
        ToolSvc += RoILinksCnvTool( 'RoILinksCnvTool' )
    import TrigJetD3PDMaker.Setup_RoILinksCnvTool_IBv3
    ToolSvc.RoILinksCnvTool.Chains_Jet += [ 'L2_j.*', 'EF_j.*', 'L2_2j.*', 'EF_2j.*', 'L2_3j.*', 'EF_3j.*', 'L2_4j.*', 'EF_4j.*', 'L2_5j.*', 'EF_5j.*' ]
    ToolSvc.RoILinksCnvTool.Chains_Bjet = [ # these first regexps should deal with most b-jet triggers
                                            'L2_b.*',  'EF_b.*',
                                            'L2_2b.*', 'EF_2b.*',
                                            'L2_3b.*', 'EF_3b.*',

                                            # some mu-jet and jet chains
                                            'L2_mu4_j10_a4tc_EFFS',         'EF_mu4_j10_a4tc_EFFS',
                                            'EF_mu4_j10_a4tc_EFFS_matched', 'EF_mu4_j10_a4tc_EFFS_matched',
                                            'L2_mu4_L1J10_matched', 'EF_mu4_L1J10_matched',
                                            'L2_mu4_L1J20_matched', 'EF_mu4_L1J20_matched',
                                            'L2_mu4_L1J30_matched', 'EF_mu4_L1J30_matched',
                                            'L2_mu4_L1J50_matched', 'EF_mu4_L1J50_matched',
                                            'L2_mu4_L1J75_matched', 'EF_mu4_L1J75_matched',
                                            'L2_mu4_j95_L1matched', 'EF_mu4_j100_a4tc_EFFS_L1matched',
                                            'L2_mu4_j95_L1matched', 'EF_mu4_j135_a4tc_EFFS_L1matched',                                            
                                            'L2_mu4_j95_L1matched', 'EF_mu4_j180_a4tc_EFFS_L1matched',
                                            'L2_4j25', 'EF_4j30_a4tc_EFFS',

                                            # for the mu-jets in the v2,v3,v4 triggers (remember to include L2 and EF)
                                            'EF_mu4_j10_a4tc_EFFS_matched',
                                            'EF_mu4_j20_a4tc_EFFS_matched',
                                            'EF_mu4_MSonly_j15_a2hi_EFFS_matched',
                                            'EF_mu4_L1J5_matched',
                                            'EF_mu4_L1J10_matched',
                                            'EF_mu4_L1J15_matched',
                                            'EF_mu4_L1J20_matched',
                                            'EF_mu4_L1J30_matched',
                                            'EF_mu4_L1J50_matched',
                                            'EF_mu4_L1J75_matched',
                                            'EF_mu4_j100_a4tc_EFFS_L1matched',
                                            'EF_mu4_j135_a4tc_EFFS_L1matched',
                                            'EF_mu4_j180_a4tc_EFFS_L1matched',
               
                                            # lepton + bjet
                                            'EF_mu24_tight_b35_mediumEF_j35_a4tchad',
                                            'EF_e24vh_medium1_b35_mediumEF_j35_a4tchad',
                                            'EF_mu22_j30_a4tc_EFFS',
                            
                                            # mymujet
                                            'EF_mu4T_j15_a4tchad_matched',
                                            'EF_mu4T_j25_a4tchad_matched',
                                            'EF_mu4T_j35_a4tchad_matched',
                                            'EF_mu4T_j45_a4tchad_matched',
                                            'EF_mu4T_j55_a4tchad_matched',
                                            'EF_mu4T_j65_a4tchad_matched',
                                            'EF_mu4T_j80_a4tchad_matched',
                                            'EF_mu4T_j110_a4tchad_matched',
                                            'EF_mu4T_j145_a4tchad_matched',
                                            'EF_mu4T_j180_a4tchad_matched',
                                            'EF_mu4T_j220_a4tchad_matched',
                                            'EF_mu4T_j280_a4tchad_matched',
                                            'EF_mu4T_j360_a4tchad_matched',
                            
                                            # mu jet triggers with deltaR and deltaZ matching
                                            'EF_mu4T_j15_a4tchad_matchedZ',
                                            'EF_mu4T_j25_a4tchad_matchedZ',
                                            'EF_mu4T_j35_a4tchad_matchedZ',
                                            'EF_mu4T_j45_a4tchad_matchedZ',
                                            'EF_mu4T_j55_a4tchad_matchedZ',
                            
                                            # L1.5 for mu+jets
                                            'EF_mu4T_j45_a4tchad_L2FS_matched',
                                            'EF_mu4T_j55_a4tchad_L2FS_matched',
                                            'EF_mu4T_j65_a4tchad_L2FS_matched',
                                            'EF_mu4T_j80_a4tchad_L2FS_matched',
                                            'EF_mu4T_j110_a4tchad_L2FS_matched',
                                            'EF_mu4T_j145_a4tchad_L2FS_matched',
                                            'EF_mu4T_j180_a4tchad_L2FS_matched',
                                            'EF_mu4T_j220_a4tchad_L2FS_matched',
                                            'EF_mu4T_j280_a4tchad_L2FS_matched',
                                            'EF_mu4T_j360_a4tchad_L2FS_matched',
                                            'EF_mu4T_j45_a4tchad_L2FS_matchedZ',
                                            'EF_mu4T_j55_a4tchad_L2FS_matchedZ',
                            
                                            # muon jets no TRT
                                            'EF_mu4_MSonly_j15_a2hi_EFFS_L1TE10',

                                            # for L2
                                            'L2_mu4_j10_a4tc_EFFS_matched',
                                            'L2_mu4_j20_a4tc_EFFS_matched',
                                            'L2_mu4_MSonly_j15_a2hi_EFFS_matched',
                                            'L2_mu4_L1J5_matched',
                                            'L2_mu4_L1J10_matched',
                                            'L2_mu4_L1J15_matched',
                                            'L2_mu4_L1J20_matched',
                                            'L2_mu4_L1J30_matched',
                                            'L2_mu4_L1J50_matched',
                                            'L2_mu4_L1J75_matched',
                                            'L2_mu4_j100_a4tc_EFFS_L1matched',
                                            'L2_mu4_j135_a4tc_EFFS_L1matched',
                                            'L2_mu4_j180_a4tc_EFFS_L1matched',
               
                                            # lepton + bjet
                                            'L2_mu24_tight_b35_mediumEF_j35_a4tchad',
                                            'L2_e24vh_medium1_b35_mediumEF_j35_a4tchad',
                                            'L2_mu22_j30_a4tc_EFFS',
                            
                                            # mymujet
                                            'L2_mu4T_j10_c4cchad',
                                            'L2_mu4T_j20_c4cchad',
                                            'L2_mu4T_j30_c4cchad',
                                            'L2_mu4T_j40_c4cchad',
                                            'L2_mu4T_j50_c4cchad',
                                            'L2_mu4T_j65_c4cchad',
                                            'L2_mu4T_j80_c4cchad',
                                            'L2_mu4T_j105_c4cchad',
                                            'L2_mu4T_j135_c4cchad',
                                            'L2_mu4T_j175_c4cchad',
                                            'L2_mu4T_j215_c4cchad',
                                            'L2_mu4T_j275_c4cchad',
                                            'L2_mu4T_j355_c4cchad',
                            
                                            # L1.5 for mu+jets
                                            'L2_mu4T_j40_c4cchad_L2FS',
                                            'L2_mu4T_j50_c4cchad_L2FS',
                                            'L2_mu4T_j60_c4cchad_L2FS',
                                            'L2_mu4T_j75_c4cchad_L2FS',
                                            'L2_mu4T_j105_c4cchad_L2FS',
                                            'L2_mu4T_j140_c4cchad_L2FS',
                                            'L2_mu4T_j175_c4cchad_L2FS',
                                            'L2_mu4T_j215_c4cchad_L2FS',
                                            'L2_mu4T_j275_c4cchad_L2FS',
                                            'L2_mu4T_j355_c4cchad_L2FS',
                                            'L2_mu4T_j40_c4cchad_L2FS',
                                            'L2_mu4T_j50_c4cchad_L2FS',
                            
                                            # muon jets no TRT
                                            'L2_mu4_MSonly_j15_a2hi_EFFS_L1TE10',
                                            ]

    from AthenaCommon.AlgSequence import AlgSequence
    from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
    preseq = AlgSequence( D3PDMakerFlags.PreD3PDAlgSeqName() )
    if not hasattr( preseq, 'FlatHltNaviMaker' ):
        from TriggerMenuAnalysis.TriggerMenuAnalysisConf import FlatHltNaviMaker
        preseq += FlatHltNaviMaker( 'FlatHltNaviMaker' )
    return
InitTrigD3PDObject.defineHook( _initTrigHook )

def TrigBJetD3PDObjects( alg, level = 10, doAddL2ID = False, doAddEFID = False, addNaviInfo = True, addConfigInfo = True, addL1Jet = True, addOffTrack = False ):

    # Run FlatHltNaviMaker algorithm prior to do anything on D3PDs
    if (addNaviInfo):
        alg += InitTrigD3PDObject( level )

    # Trigger navigation info
    if (addNaviInfo):
        from TriggerD3PDMaker.TrigNaviD3PDObject import ChainEntryD3PDObject, \
                                                        CombLinksD3PDObjects
        if not hasattr( alg, "trig_Nav_Filler" ):
            alg += ChainEntryD3PDObject( level )           # prefix = "trig_Nav_"
        # prefix = "trig_RoI_"
        for roi in ['L2_b', 'EF_b', 'L2_j', 'EF_j']:
            if not hasattr (alg, 'trig_RoI_%s_Filler' % roi):
                alg += CombLinksD3PDObjects[ roi ]( level ) 

    # LVL1 Jet RoI
    if (addL1Jet):
      from TriggerD3PDMaker.JetROID3PDObject import JetROID3PDObject
      if not hasattr( alg, 'trig_L1_jet_Filler' ):
          alg += JetROID3PDObject( 10 );

    # Add the LVL2 jet information:
    if (addL1Jet):
      from TrigJetD3PDMaker.TrigJetD3PDObject import TrigJetD3PDObject
      if not hasattr( alg, 'trig_L2_jet_Filler' ):
        alg += TrigJetD3PDObject( level )

    # Add the EF jet information:
    if (addL1Jet):
      from TrigJetD3PDMaker.EFJetD3PDObject import EFJetD3PDObject
      if not hasattr( alg, 'trig_EF_jet_Filler' ):
        alg += EFJetD3PDObject( level )


    # Add the trigger configuration metadata:
    if (addConfigInfo):
      from TriggerD3PDMaker.TrigConfMetadata import addTrigConfMetadata
      addTrigConfMetadata( alg )

    # Add the full trigger decision information:
    from TriggerD3PDMaker.TrigDecisionD3PDObject import TrigDecisionD3PDObject
    if not hasattr( alg, 'TrigDecisionFiller' ):
        alg += TrigDecisionD3PDObject( level )

    # Add information about the LVL2 tracks
    if doAddL2ID:
        #from RecExConfig.RecFlags import rec
        #if rec.readAOD():
        #    print "WARNING: TrigBJetD3PDMaker will not dump L2 tracks when reading from AOD. The L2 tracks are not stored in the AOD."
        #else:
            from TrigInDetD3PDMaker.TrigInDetTrackD3PDObject import TrigInDetTrackD3PDObject
            alg += TrigInDetTrackD3PDObject( level, sgkey = 'HLT_TrigSiTrack_Jet',
                                             prefix = "trig_L2_sitrk_jet_",
                                             allowMissing = True );
            alg += TrigInDetTrackD3PDObject( level, sgkey = 'HLT_TrigIDSCAN_Jet',
                                             prefix = "trig_L2_idtrk_jet_",
                                             allowMissing = True );
            alg += TrigInDetTrackD3PDObject( level, sgkey = 'HLT_TrigL2SiTrackFinder_Jet',
                                             prefix = "trig_L2_star_sitrk_jet_",
                                             allowMissing = True );

    # Add information about the EF tracks:
    if doAddEFID:
        ## should not modify TrackD3PDFlags global flags!!!
        #from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags
        #EFTrackD3PDFlags = TrackD3PDFlags # We should probably make our own version of that! TODO
        #EFTrackD3PDFlags.storeTrackSummary = True
        #EFTrackD3PDFlags.storeTrackFitQuality = True
        #EFTrackD3PDFlags.storeTrackMomentum = True
        #EFTrackD3PDFlags.storeTrackInfo = True
        # Add A LOT of details!
        #EFTrackD3PDFlags.trackParametersAtGlobalPerigeeLevelOfDetails = 3
        #EFTrackD3PDFlags.trackParametersAtPrimaryVertexLevelOfDetails = 3
        #EFTrackD3PDFlags.trackParametersAtBeamSpotLevelOfDetails = 3
        #EFTrackD3PDFlags.storeDiagonalCovarianceAsErrors = True

        from TrackD3PDMaker.TrackD3PDObject import TrackD3PDObject
        EFTrackD3PDObject = TrackD3PDObject( _label = "trig_EF_bjetid_",
                                             _prefix = "trig_EF_bjetid_",
                                             _sgkey = "HLT_InDetTrigParticleCreation_Bjet_EFID",
                                             typeName = "Rec::TrackParticleContainer",
                                             #truthMapKey = 'TrackParticleTruthCollection',
                                             doTruth = True,
                                             #TruthParticleTarget='mc_',
                                             truthTarget='mc_', 
                                             storeHitTruthMatching = True,
                                             storeTrackSummary = True,
                                             storeTrackFitQuality = True,
                                             storeTrackMomentum = True,
                                             storeTrackInfo = True,
                                             trackParametersAtGlobalPerigeeLevelOfDetails = 3,
                                             trackParametersAtPrimaryVertexLevelOfDetails = 3,
                                             trackParametersAtBeamSpotLevelOfDetails = 3,
                                             storeDiagonalCovarianceAsErrors = True )
                                             #flags = EFTrackD3PDFlags )

        from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
        import TrackD3PDMaker
        BJetEFTrackVertAssoc = SimpleAssociation \
                            ( EFTrackD3PDObject,
                              TrackD3PDMaker.TrackParticleVertexAssociationTool,
                              prefix = 'vert',
                              blockname = "trigEFIDAssoc" )
        BJetEFTrackVertAssoc.defineBlock( 2, 'Vertex',
                                       TrackD3PDMaker.VertexPositionFillerTool )

        alg += EFTrackD3PDObject( level );

    if addOffTrack: 
        # Add offline track information
        from TrackD3PDMaker.TrackD3PDObject import TrackParticleD3PDObject
        alg += TrackParticleD3PDObject(level)
        
    #from TrackD3PDMaker.TrackD3PDObject import PixelTrackD3PDObject
    #from TrackD3PDMaker.TrackD3PDObject import SCTTrackD3PDObject
    #from TrackD3PDMaker.TrackD3PDObject import TRTTrackD3PDObject
    #alg += PixelTrackD3PDObject(level)
    #alg += SCTTrackD3PDObject(level)
    #alg += TRTTrackD3PDObject(level)

    #
    # Add the b-jet information:
    #
    from TrigBJetD3PDMaker.TrigL2BjetD3PDObject import TrigL2BjetD3PDObject
    from TrigBJetD3PDMaker.TrigEFBjetD3PDObject import TrigEFBjetD3PDObject
    alg += TrigL2BjetD3PDObject( level );
    alg += TrigEFBjetD3PDObject( level );


    # Add EF VxCandidates information
    from TrigBJetD3PDMaker.EFPVD3PDObject import EFPVD3PDObject
    alg += EFPVD3PDObject(level)


    # Add TrigVertex information for L2/EF PV information
    #from TrigInDetD3PDMaker.TrigVertexD3PDObject import TrigVertexD3PDObject
    from TrigBJetD3PDMaker.TrigVertexBjetD3PDObject import TrigVertexBjetD3PDObject
    trigVertexBjetD3PDObject = TrigVertexBjetD3PDObject(level)
#    trigVertexBjetD3PDObject.OutputLevel = 1
    alg += trigVertexBjetD3PDObject

    from TrigBJetD3PDMaker.TrigVertexEFBjetD3PDObject import TrigVertexEFBjetD3PDObject
    trigVertexEFBjetD3PDObject = TrigVertexEFBjetD3PDObject(level)
#    trigVertexEFBjetD3PDObject.OutputLevel = 1
    alg += trigVertexEFBjetD3PDObject



