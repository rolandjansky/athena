# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: PanTauAnalysis/python/PanTauSeedBuilderAlg.py
# @purpose: hold a set of customized configurables and factory functions for the seed builder

__doc__    = "hold a set of customized configurables and factory functions for the seed builder"
__author__ = "S.Fleischmann"
__version__= "$Revision: 540821 $"

from PanTauAlgs.PanTauAlgsConf import PanTau__TauSeedBuilderAlg
class PanTauSeedBuilderAlg( PanTau__TauSeedBuilderAlg ):

    __slots__ = { }

    def __init__( self, name = "TauSeedBuildCone4",
                  jetAlgName = "Cone4", **kwargs ):
        # have to call base init
        kwargs['name']                                  = name
        kwargs['ContainerName_eflowJets']               = "eflowObjects_tauMode"
        kwargs['ContainerName_PanTauGoodOutputSeeds']   = "TauSeedsEflow"+jetAlgName
        kwargs['ContainerName_PanTauBadOutputSeeds']    = "TauRejectedSeedsEflow"+jetAlgName

        super( PanTauSeedBuilderAlg, self ).__init__( **kwargs )

        from PanTauAnalysis.PanTauFlags import panTauFlags
        from RecExConfig.RecFlags import rec
        
        if panTauFlags.doTruthEtIsolations():
            from AthenaCommon.AppMgr import ToolSvc
            if not hasattr(ToolSvc, 'TruthIsolationTool'):
                ToolSvc += CfgMgr.TruthIsolationTool()
        
        
        ############################ 
        ### Set up the configurables 
        
        from PanTauAnalysis.PanTauSeedBuilderAlg_Flags import PanTauSeedBuilderAlgFlags as PSB_Flags
        
        # Basic config
        self.Config_DoTruth                     = rec.doTruth()
        self.RunTimeHists_Enabled               = PSB_Flags.Config_DoRunTimeHists()
        self.Config_UseTauRecAsSeeds            = PSB_Flags.Config_UseTauRecAsSeeds()
        self.Config_EfoCollectionDeltaR         = PSB_Flags.Config_EfoCollectionDeltaR()
        self.Config_DeltaRCoreRegion            = PSB_Flags.Config_DeltaRCoreRegion()
        self.Config_DoTauRecVicinityCheck       = PSB_Flags.Config_DoTauRecVicinityCheck()
        self.Config_DeltaRTauRecVicinityCheck   = PSB_Flags.Config_DeltaRTauRecVicinityCheck()
        self.Config_KeepOverlapTauRecVicinity   = PSB_Flags.Config_KeepOverlapTauRecVicinity()
        
        #Container Names
        self.ContainerName_eflowJets                    = PSB_Flags.ContainerName_eflowJets()
        self.ContainerName_eflowObjectsFromTaus         = PSB_Flags.ContainerName_eflowObjectsFromTaus()
        self.ContainerName_tauRec                       = PSB_Flags.ContainerName_tauRec()
        self.ContainerName_PrimaryVertices              = PSB_Flags.ContainerName_PrimaryVertices()
        self.ContainerName_TruthTaus                    = PSB_Flags.ContainerName_TruthTaus()
        self.ContainerName_PanTauGoodOutputSeeds        = PSB_Flags.ContainerName_PanTauGoodOutputSeeds() + jetAlgName
        self.ContainerName_PanTauBadOutputSeeds         = PSB_Flags.ContainerName_PanTauBadOutputSeeds() + jetAlgName
        self.ContainerName_TruePi0MatchedNeutralEFOs    = PSB_Flags.ContainerName_TruePi0MatchedNeutralEFOs()
        
        # Preselection
        self.Config_Presel_DoPreselection       = PSB_Flags.Config_Presel_DoPreselection()
        self.Config_Presel_MaxAbsEta            = PSB_Flags.Config_Presel_MaxAbsEta()
        self.Config_Presel_MinNumberChargedComp = PSB_Flags.Config_Presel_MinNumberChargedComp()
        self.Config_Presel_MaxNumberChargedComp = PSB_Flags.Config_Presel_MaxNumberChargedComp()
        self.Config_Presel_MinAbsSeedCharge     = PSB_Flags.Config_Presel_MinAbsSeedCharge()
        self.Config_Presel_MaxAbsSeedCharge     = PSB_Flags.Config_Presel_MaxAbsSeedCharge()
        self.Config_Presel_MinSeedPt            = PSB_Flags.Config_Presel_MinSeedPt()
        
        # EFO Selection
        self.Config_EfoSel_DoEfoSelection           = PSB_Flags.Config_EfoSel_DoEfoSelection()
        self.Config_EfoSel_EtaBins                  = PSB_Flags.Config_EfoSel_EtaBins()
        self.Config_EfoSel_EtaBinEtCuts             = PSB_Flags.Config_EfoSel_EtaBinEtCuts()
        self.Config_EfoSel_Pi0EtBins                = PSB_Flags.Config_EfoSel_Pi0EtBins()
        self.Config_EfoSel_Pi0EtBinBDTCuts         = PSB_Flags.Config_EfoSel_Pi0EtBinBDTCuts()
        self.Config_EfoSel_MinEflowObjectEt         = PSB_Flags.Config_EfoSel_MinEflowObjectEt()
        self.Config_EfoSel_MaxEflowObjectEta        = PSB_Flags.Config_EfoSel_MaxEflowObjectEta()
        self.Config_EfoSel_MinHADNeutralEt          = PSB_Flags.Config_EfoSel_MinHADNeutralEt()
        self.Config_EfoSel_MinEMNeutralEt           = PSB_Flags.Config_EfoSel_MinEMNeutralEt()
        self.Config_EfoSel_MinEMNeutralE            = PSB_Flags.Config_EfoSel_MinEMNeutralE()
        self.Config_EfoSel_MinPi0NeutralE           = PSB_Flags.Config_EfoSel_MinPi0NeutralE()
        self.Config_EfoSel_MinPi0NeutralEt          = PSB_Flags.Config_EfoSel_MinPi0NeutralEt()
        self.Config_EfoSel_UseEMNeutralHadronicVeto = PSB_Flags.Config_EfoSel_UseEMNeutralHadronicVeto()
        self.Config_EfoSel_ApplyEMNeutralECut       = PSB_Flags.Config_EfoSel_ApplyEMNeutralECut()
        self.Config_EfoSel_ApplyEMNeutralEtCut      = PSB_Flags.Config_EfoSel_ApplyEMNeutralEtCut()
        self.Config_EfoSel_ApplyPi0NeutralECut      = PSB_Flags.Config_EfoSel_ApplyPi0NeutralECut()
        self.Config_EfoSel_ApplyPi0NeutralEtCut     = PSB_Flags.Config_EfoSel_ApplyPi0NeutralEtCut()
        self.Config_EfoSel_ApplyPionMassHypothesis  = PSB_Flags.Config_EfoSel_ApplyPionMassHypothesis()
        self.Config_EfoSel_PiZeroBDTThreshold       = PSB_Flags.Config_EfoSel_PiZeroBDTThreshold()
        self.Config_EfoSel_TrackMatch_DeltaEta      = PSB_Flags.Config_EfoSel_TrackMatch_DeltaEta()
        self.Config_EfoSel_TrackMatch_DeltaPhi      = PSB_Flags.Config_EfoSel_TrackMatch_DeltaPhi()
        self.Config_EfoSel_TrackMatch_RelEMatch     = PSB_Flags.Config_EfoSel_TrackMatch_RelEMatch()
        
        print("__init__ PanTauSeedBuilderAlg successful")

    def setDefaults(cls, hdl):
        #print "*setDefaults before test"
        #### continue, only if it is our Configurable
        if not isinstance(hdl, PanTauSeedBuilderAlg):
            return
        #print "*setDefaults"

        from PanTauAnalysis.PanTauFlags import jobproperties as jp
        from AthenaCommon.AppMgr import ToolSvc
        #from AthenaCommon.Configurable import Configurable
        ## continue, only if option not already set
        if not hasattr(hdl, 'TauFeatureExtractionTools'):
            hdl.Tools_TauFeatureExtractionTools = ""#PublicToolHandleArray([])
            pass

        if not hasattr(hdl, 'TrackSelectorTool'):
            trackSel = PanTauSeedTrackSelector()
            ToolSvc += trackSel
            hdl.Tools_TrackSelectorTool = trackSel

        return

    #def __str__(self):
        #return ""
        #return super(PanTauSeedBuilderAlg,self).__str__()

    pass # PanTauSeedBuilderAlg


from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
class PanTauSeedTrackSelector( InDet__InDetDetailedTrackSelectorTool ):
    __slots__ = { }

    def __init__( self,
                  name = "PanTauSeedTrackSelector" ):
        #print "init seed track sel"
        from PanTauAnalysis.PanTauFlags import jobproperties as jp
        from AthenaCommon.AppMgr import ToolSvc
        # ATLAS extrapolator
        from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
        theAtlasExtrapolator=AtlasExtrapolator()
        ToolSvc+=theAtlasExtrapolator
        # call base class constructor:
        InDet__InDetDetailedTrackSelectorTool.__init__(self,name,\
                            pTMin                = 1000,
                            IPd0Max              = 2.0,
                            IPz0Max              = 10.,  # z0*sin(theta)
                            z0Max                = 9999.,# z0
                            useTrackSummaryInfo  = True,
                            nHitBLayer           = 0,
                            nHitPix              = 2,
                            nHitSct              = 0,
                            nHitSi               = 7,
                            nHitTrt              = 0,
                            useSharedHitInfo     = False,
                            fitChi2OnNdfMax      = 9999.,
                            TrackSummaryTool     = "",
                            Extrapolator         = theAtlasExtrapolator,
                            OutputLevel          = 4)
        print self

#class PanTauImpactParameterTrackSelector( InDet__InDetDetailedTrackSelectorTool ):
    #__slots__ = { }

    #def __init__( self,
                  #name = "PanTauImpactParamterTrackSelector" ):
        #from PanTauAnalysis.PanTauFlags import jobproperties as jp
        #from AthenaCommon.AppMgr import ToolSvc
        ## ATLAS extrapolator
        #from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
        #theAtlasExtrapolator=AtlasExtrapolator()
        #ToolSvc+=theAtlasExtrapolator
        ## call base class constructor:
        #InDet__InDetDetailedTrackSelectorTool.__init__(self,name,\
                            #pTMin                = 1000,
                            #IPd0Max              = 1.5,
                            #IPz0Max              = 1.5,  # z0*sin(theta)
                            #z0Max                = 9999.,# z0
                            #useTrackSummaryInfo  = True,
                            #nHitBLayer           = 1,
                            #nHitPix              = 2,
                            #nHitSct              = 0,
                            #nHitSi               = 7,
                            #nHitTrt              = 0,
                            #useSharedHitInfo     = False,
                            #fitChi2OnNdfMax      = 9999.,
                            #TrackSummaryTool     = "",
                            #Extrapolator         = theAtlasExtrapolator)


from PanTauAlgs.PanTauAlgsConf import PanTau__TauImpactParameterExtractionTool
class PanTauImpactParameterExtractionTool( PanTau__TauImpactParameterExtractionTool ):
    __slots__ = { }

    def __init__( self,
                  name = "PanTauImpactParameterExtractionTool" ):
        from PanTauAnalysis.PanTauFlags import jobproperties as jp
        from AthenaCommon.AppMgr import ToolSvc

        from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
        theAtlasExtrapolator=AtlasExtrapolator()
        ToolSvc += theAtlasExtrapolator
        # track selector for impact parameter:
        from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
        impactParamTrackSel = InDet__InDetDetailedTrackSelectorTool(name+"TrackSel",
                            pTMin                = 1000,
                            IPd0Max              = 1.5,
                            IPz0Max              = 1.5,  # z0*sin(theta)
                            z0Max                = 9999.,# z0
                            useTrackSummaryInfo  = True,
                            nHitBLayer           = 1,
                            nHitPix              = 2,
                            nHitSct              = 0,
                            nHitSi               = 7,
                            nHitTrt              = 0,
                            useSharedHitInfo     = False,
                            fitChi2OnNdfMax      = 9999.,
                            TrackSummaryTool     = "",
                            Extrapolator         = theAtlasExtrapolator)
        print impactParamTrackSel
        ToolSvc += impactParamTrackSel
        # TrackToVertexIPEstimator (needed for impact parameter calculation)
        from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__TrackToVertexIPEstimator
        trackToVertexIPEstimator = Trk__TrackToVertexIPEstimator(Extrapolator = theAtlasExtrapolator)
        print trackToVertexIPEstimator
        ToolSvc += trackToVertexIPEstimator
        # call base class constructor:
        PanTau__TauImpactParameterExtractionTool.__init__(self,name,\
                                                          TrackToVertexIPEstimator    = trackToVertexIPEstimator,
                                                          TrackSelectorTool           =  impactParamTrackSel)
        print self


def createTauSeedBuilder(   jetAlgName = "Cone4",
                            tauFeatureExtractionTools = None,
                            trackSelectorTool = None):
                            #**kw ):
    """
    Factory function to create a fully and correctly configured customized
    TauSeedBuilderAlg
    """
    from PanTauAnalysis.PanTauFlags import jobproperties as jp
    from AthenaCommon.AppMgr import ToolSvc
    
    if trackSelectorTool is None:
        trackSelectorTool = PanTauSeedTrackSelector()
        #print trackSelectorTool
        ToolSvc += trackSelectorTool
    
    if tauFeatureExtractionTools is None:
        tauFeatureExtractionTools = []
        if jp.PanTauFlags.GetJetFeatures() :
            from PanTauAlgs.PanTauAlgsConf import PanTau__TauJetFeatureExtractionTool
            tauJetExtr = PanTau__TauJetFeatureExtractionTool()
            print tauJetExtr
            ToolSvc += tauJetExtr
            tauFeatureExtractionTools += [tauJetExtr]
        if jp.PanTauFlags.GetImpactParameterFeatures() :
            tauImpactParameterExtr = PanTauImpactParameterExtractionTool()
            ToolSvc += tauImpactParameterExtr
            #print tauImpactParameterExtr
            tauFeatureExtractionTools += [tauImpactParameterExtr]
            #print tauImpactParameterExtr

    return PanTauSeedBuilderAlg( 'TauSeedBuild'+jetAlgName, jetAlgName,
                            Tools_TauFeatureExtractionTools    = tauFeatureExtractionTools,
                            Tools_TrackSelectorTool = trackSelectorTool)


                         #**kw )
