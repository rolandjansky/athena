# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.Logging import logging

logAODFix_r201 = logging.getLogger( 'AODFix_r201' )

from AODFix_base import AODFix_base

class AODFix_r201(AODFix_base):
    ''' This class just performs AODFix on 20.1.X releases

    Instance variables
      addMetadata (bool) (inherited): whether to add metaData
      doAODFix (bool) (inherited): whether to do AODFix
      prevAODFix (string): previous AODFix applied
      newAODFix (string): new AODFix to apply
    '''

    @staticmethod
    def latestAODFixVersion():
        """The latest version of the AODFix."""
        return "r201v1"

    def postSystemRec(self):
        if self.doAODFix:

            logAODFix_r201.debug("Executing AODFix_r201_postSystemRec")

            from AthenaCommon.AppMgr import ToolSvc
            from AthenaCommon.AlgSequence import AlgSequence
            topSequence = AlgSequence()

            idtp = "InDetTrackParticles"
            pvs = "PrimaryVertices"
            from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
            ToolSvc += InDet__InDetTrackSelectionTool(
                "AODFix_idtracksel",
                minPt                = 400.0,
                maxAbsEta            = 2.5,
                minNSiHits           = 7,
                maxNPixelSharedHits  = 1,
                maxOneSharedModule   = True,
                maxNSiHoles          = 2,
                maxNPixelHoles       = 1,
                )
            from JetRecTools.JetRecToolsConf import JetTrackSelectionTool
            ToolSvc += JetTrackSelectionTool(
                "AODFix_tracksel",
                InputContainer  = idtp,
                OutputContainer = "AODFix_JetSelectedTracks",
                Selector        = ToolSvc.AODFix_idtracksel
                )
            from TrackVertexAssociationTool.TrackVertexAssociationToolConf import CP__TightTrackVertexAssociationTool
            ToolSvc += CP__TightTrackVertexAssociationTool("AODFix_jetTightTVAtool", dzSinTheta_cut=3, doPV=True)

            from JetRecTools.JetRecToolsConf import TrackVertexAssociationTool
            ToolSvc += TrackVertexAssociationTool(
                "AODFix_tvassoc",
                TrackParticleContainer  = idtp,
                TrackVertexAssociation  = "AODFix_JetTrackVtxAssoc",
                VertexContainer         = pvs,
                TrackVertexAssoTool     = ToolSvc.AODFix_jetTightTVAtool,
                )

            # Jet track info.
            from JetMomentTools.JetMomentToolsConf import JetTrackMomentsTool
            ToolSvc += JetTrackMomentsTool(
                "AODFix_trkmoms",
                VertexContainer = pvs,
                AssociatedTracks = "GhostTrack",
                TrackVertexAssociation = ToolSvc.AODFix_tvassoc.TrackVertexAssociation,
                TrackMinPtCuts = [500, 1000],
                TrackSelector = ToolSvc.AODFix_tracksel
                )

            from JetMomentTools.JetMomentToolsConf import JetClusterMomentsTool
            ToolSvc += JetClusterMomentsTool(
                "AODFix_clsmoms",
                DoClsSecondLambda=False,
                DoClsCenterLambda=False,
                DoClsSecondR=False
                )

            from JetMomentTools.JetMomentToolsConf import JetECPSFractionTool
            ToolSvc += JetECPSFractionTool(
                "AODFix_ecpsfrac"
                )

            from JetMomentTools.JetMomentToolsConf import JetConstitFourMomTool
            ToolSvc += JetConstitFourMomTool(
                "AODFix_constfourmom"
                )

            rtools = [ToolSvc.AODFix_tracksel, ToolSvc.AODFix_tvassoc]

            jetmommods = [ToolSvc.AODFix_trkmoms]
            LCjetmods = [ToolSvc.AODFix_clsmoms,ToolSvc.AODFix_ecpsfrac]
            EMjetmods = [ToolSvc.AODFix_constfourmom]

            from JetRec.JetRecConf import JetRecTool
            ToolSvc += JetRecTool("AODFix_AntiKt4LCTopo",
                                  InputContainer  = "AntiKt4LCTopoJets",
                                  OutputContainer = "AntiKt4LCTopoJets",
                                  JetModifiers    = jetmommods + LCjetmods)
            ToolSvc += JetRecTool("AODFix_AntiKt4EMTopo",
                                  InputContainer  = "AntiKt4EMTopoJets",
                                  OutputContainer = "AntiKt4EMTopoJets",
                                  JetModifiers    = jetmommods + EMjetmods)
            rtools += [ ToolSvc.AODFix_AntiKt4LCTopo, ToolSvc.AODFix_AntiKt4EMTopo ]

            from JetRec.JetRecConf import JetAlgorithm
            topSequence += JetAlgorithm("jetalgAODFix", Tools = rtools)
