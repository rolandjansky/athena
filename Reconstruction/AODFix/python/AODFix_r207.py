# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.Logging import logging

logAODFix_r207 = logging.getLogger( 'AODFix_r207' )

from AODFix_base import AODFix_base

class AODFix_r207(AODFix_base):
    ''' This class just performs AODFix on 20.7.X releases

    Instance variables
      addMetadata (bool) (inherited): whether to add metaData
      doAODFix (bool) (inherited): whether to do AODFix
      prevAODFix (string): previous AODFix applied
      newAODFix (string): new AODFix to apply
      isMC (bool): is the input data MC?
    '''

    @staticmethod
    def latestAODFixVersion():
        """The latest version of the AODFix. Moving to new AODFix version scheme"""
        nextMajRel = "20.7.8"

        # items in this list will be excluded from the metadata, so will always rerun
        excludeFromMetadata = ["btagging"] 

        metadataList = [item.split("_")[0] for item in sorted(AODFix_r207.__dict__.keys()) 
                        if ("_" in item and "__" not in item)]

        for excl in excludeFromMetadata:
            if excl in metadataList:
                metadataList.remove(excl)

        metadataList.insert(0, nextMajRel)

        return "-".join(metadataList)

    def preInclude( self ):
        """
        This function calls the other preincludes base on metadata
        """
        if self.doAODFix:
            oldMetadataList = self.prevAODFix.split("-")
            # Note that the two (xAOD::)EventInfo fixing functions can't clash
            # with each other. Since the "pileup" fix is only applied on data,
            # and the "mcweight" fix is only applied on MC.
            if "pileup" not in oldMetadataList:
                self.pileup_preInclude()
                pass
            if "mcweight" not in oldMetadataList:
                self.mcweight_preInclude()
                pass
            pass
        return

    def postSystemRec(self):
        """
        This function calls the other postSystemRecs base on metadata"
        """

        if self.doAODFix:

            logAODFix_r207.debug("Executing AODFix_r207_postSystemRec")

            from AthenaCommon.AlgSequence import AlgSequence
            topSequence = AlgSequence()
            
            oldMetadataList = self.prevAODFix.split("-")
            if "mcutils" not in oldMetadataList:
                self.mcutils_postSystemRec(topSequence)
                pass
            if "btagging" not in oldMetadataList:
                self.btagging_postSystemRec(topSequence)
                pass
            if "met" not in oldMetadataList:
                self.met_postSystemRec(topSequence)
                pass
            if "pflow" not in oldMetadataList:
                self.pflow_postSystemRec(topSequence)
                pass
            # Muon AODfix
            if "muon" not in oldMetadataList:
                self.muon_postSystemRec(topSequence)
                pass

            # if "tau" not in oldMetadataList:
            #     self.tau_postSystemRec(topSequence)
            #     pass

            # Reset all of the ElementLinks. To be safe.
            from AthenaCommon import CfgMgr
            topSequence += \
                CfgMgr.xAODMaker__ElementLinkResetAlg( "AODFix_ElementLinkReset" )
            pass

        return


    # Below are the individual AODfixes, split up and documented
    # Name must follow format: <fixID>_<whereCalled>

    def mcweight_preInclude( self ):
        """
        This fixes the problem with propagating multiple MC weights correctly
        into the (xAOD::)EventInfo object in MC samples that provide multiple
        weights. The issue is described in:
           https://its.cern.ch/jira/browse/ATLASSIM-2989
        """

        # Tell the user what's happening:
        logAODFix_r207.debug( "Executing AODFix_r207_mcweight_preInclude" )

        # This fix is only needed for MC files that don't have this fix.
        from AthenaCommon.GlobalFlags import globalflags
        if globalflags.DataSource() != "geant4":
            return

        # Access the main algorithm sequence:
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()

        # Add the AODFix algorithms to this sub-sequence:
        from AthenaCommon import CfgMgr
        topSequence += \
            CfgMgr.EventInfoMCWeightFixAlg( "AODFix_EventInfoMCWeightFixAlg" )
        # Note that we must use the default algorithm name, as the CutFlowSvc's
        # setup will only be correct with that name. Otherwise the event
        # bookkeeping metadata doesn't see this fix...
        topSequence += \
            CfgMgr.xAODMaker__EventInfoCnvAlg( ForceOverwrite = True )

        return

    def pileup_preInclude(self):
        """
        This fixes the problem with the in-time pileup number in
        xAOD::EventInfo, as described in:
           https://its.cern.ch/jira/browse/ATLASRECTS-3204
        Incremented version to r207v4.
        """

        # Tell the user what's happening:
        logAODFix_r207.debug( "Executing AODFix_r207_pileup_preInclude" )

        # This fix is only needed for 20.7 data files made from data15,
        # without this fix.
        from AthenaCommon.GlobalFlags import globalflags
        if globalflags.DataSource() != "data":
            return
        from RecExConfig.RecFlags import rec
        if rec.projectName() != "data15_13TeV":
            return

        # Access the main algorithm sequence:
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()

        # Make sure that the LuminosityTool is configured. This needs at
        # least LumiBlockComps-01-00-27 to work correctly.
        from AthenaCommon.AppMgr import ToolSvc
        if not hasattr( ToolSvc, "LuminosityTool" ):
            from LumiBlockComps.LuminosityToolDefault import LuminosityToolDefault
            ToolSvc += LuminosityToolDefault()
            pass

        # Set up the xAOD::EventInfo creator algorithm to overwrite the
        # existing object. This needs at least xAODEventInfoCnv-00-00-17-01,
        # or xAODEventInfoCnv-00-00-23.
        from AthenaCommon import CfgMgr
        topSequence += \
            CfgMgr.xAODMaker__EventInfoCnvAlg( "AODFix_EventInfoCnvAlg",
                                               ForceOverwrite = True )

        return

    def muon_postSystemRec(self, topSequence):
        """This fixes the muon momenta to match that of its primary TrackParticle.
        JIRA: https://its.cern.ch/jira/browse/ATLASRECTS-3121
        Reco meeting: https://indico.cern.ch/event/532806/
        Incremented version to r207v3.
        """
        from xAODMuonCnv.xAODMuonCnvConf import xAOD__MuonAODFixAlg
        topSequence+=xAOD__MuonAODFixAlg()
        
    # Comment out for now so that it doesn't get added in the metadata

    # def tau_postSystemRec(self, topSequence):
    #     from tauRec.TauRecAODBuilder import TauRecAODProcessor
    #     TauRecAODProcessor()
    #     return

    def pflow_postSystemRec(self, topSequence):

        #Only run pflow AOD fix if input file contains AntKt4EMPFlowJets - should be the case for all samples, except HI samples
        from RecExConfig.ObjKeyStore import cfgKeyStore
        if cfgKeyStore.isInInputFile("xAOD::JetContainer","AntiKt4EMPFlowJets"):

            #Now we need to rerun the particle flow jet finding

            #This configuration should be identical to the AntiKt4EMPflowJets configured in:
            #http://acode-browser2.usatlas.bnl.gov/lxr-rel20/source/atlas/Reconstruction/Jet/JetRec/share/JetRec_jobOptions.py
            #with the addJetFinder function (in the svn tag for 20.7)
            from AthenaCommon import CfgMgr
            particleFlowJetFinder_AntiKt4 = CfgMgr.JetFinder("AntiKt4EMPFlowJetsFinder_AODFix")
            particleFlowJetFinder_AntiKt4.JetAlgorithm = "AntiKt"
            particleFlowJetFinder_AntiKt4.JetRadius = 0.4
            particleFlowJetFinder_AntiKt4.GhostArea = 0.01
            particleFlowJetFinder_AntiKt4.PtMin = 2000

            #Then we setup a jet builder to calculate the areas needed for the rho subtraction
            from AthenaCommon.AppMgr import ToolSvc
            particleFlowJetBuilder_AntiKt4 = CfgMgr.JetFromPseudojet("jblda_AODFix", Attributes = ["ActiveArea", "ActiveArea4vec"])
            ToolSvc += particleFlowJetBuilder_AntiKt4
            particleFlowJetFinder_AntiKt4.JetBuilder = particleFlowJetBuilder_AntiKt4
            ToolSvc += particleFlowJetFinder_AntiKt4

            #Now we setup a JetRecTool which will use the above JetFinder
            particleFlowJetRecTool = CfgMgr.JetRecTool("AntiKt4EMPFlowJets_AODFix")
            particleFlowJetRecTool.JetFinder = particleFlowJetFinder_AntiKt4
            ToolSvc += particleFlowJetRecTool

            jpjretriever = CfgMgr.JetPseudojetRetriever("jpjretriever")
            ToolSvc += jpjretriever

            pflowRetriever = CfgMgr.CP__RetrievePFOTool("pflowretriever_AODFix")
            ToolSvc += pflowRetriever

            trk_trackselloose = CfgMgr.InDet__InDetTrackSelectionTool(
                "trk_trackselloose_AODFix",
                CutLevel = "Loose"
                )
            ToolSvc += trk_trackselloose

            trackselloose = CfgMgr.JetTrackSelectionTool(
              "trackselloose",
              InputContainer  = "InDetTrackParticles",
              OutputContainer = "JetSelectedTracks",
              Selector        = trk_trackselloose
            )
            ToolSvc += trackselloose

            tracksel = CfgMgr.JetTrackSelectionTool(
                "tracksel_AODFix",
                InputContainer  = "InDetTrackParticles",
                OutputContainer = "JetSelectedTracks_AODFix",
                Selector        = trk_trackselloose
                )
            ToolSvc += tracksel

            tighttva = CfgMgr.CP__TightTrackVertexAssociationTool("jetTighTVAtool_AODFix", dzSinTheta_cut=3, doPV=True)
            ToolSvc += tighttva

            tvassoc = CfgMgr.TrackVertexAssociationTool(
                "tvassoc_AODFix",
                TrackParticleContainer  = "InDetTrackParticles",
                TrackVertexAssociation  = "JetTrackVtxAssoc_AODFix",
                VertexContainer         = "PrimaryVertices",
                TrackVertexAssoTool     = tighttva,
                )
            ToolSvc += tvassoc

            # Ghost tracks.
            ghostScaleFactor = 1e-40
            gtrackget = CfgMgr.TrackPseudoJetGetter(
                "gtrackget_AODFix",
                InputContainer = tracksel.OutputContainer,
                Label = "GhostTrack",
                OutputContainer = "PseudoJetGhostTracks_AODFix",
                TrackVertexAssociation = tvassoc.TrackVertexAssociation,
                SkipNegativeEnergy = True,
                GhostScale = ghostScaleFactor
                )
            ToolSvc += gtrackget

            # Muon segments
            gmusegget = CfgMgr.MuonSegmentPseudoJetGetter(
                "gmusegget_AODFix",
                InputContainer = "MuonSegments",
                Label = "GhostMuonSegment",
                OutputContainer = "PseudoJetGhostMuonSegment_AODFix",
                Pt = 1.e-20
                )
            ToolSvc += gmusegget

            # AntiKt2 track jets.
            gakt2trackget = CfgMgr.PseudoJetGetter(
                "gakt2trackget_AODFix", # give a unique name
                InputContainer = "AntiKt2PV0TrackJets", # SG key
                Label = "GhostAntiKt2TrackJet",   # this is the name you'll use to retrieve associated ghosts
                OutputContainer = "PseudoJetGhostAntiKt2TrackJet_AODFix",
                SkipNegativeEnergy = True,
                GhostScale = ghostScaleFactor,   # This makes the PseudoJet Ghosts, and thus the reco flow will treat them as so.
                )
            ToolSvc += gakt2trackget

            # AntiKt3 track jets.
            gakt3trackget = CfgMgr.PseudoJetGetter(
                "gakt3trackget_AODFix", # give a unique name
                InputContainer = "AntiKt3PV0TrackJets", # SG key
                Label = "GhostAntiKt3TrackJet",   # this is the name you'll use to retrieve associated ghosts
                OutputContainer = "PseudoJetGhostAntiKt3TrackJet_AODFix",
                SkipNegativeEnergy = True,
                GhostScale = ghostScaleFactor,   # This makes the PseudoJet Ghosts, and thus the reco flow will treat them as so.
                )
            ToolSvc += gakt3trackget

            # AntiKt4 track jets.
            gakt4trackget = CfgMgr.PseudoJetGetter(
                "gakt4trackget_AODFix", # give a unique name
                InputContainer = "AntiKt4PV0TrackJets", # SG key
                Label = "GhostAntiKt4TrackJet",   # this is the name you'll use to retrieve associated ghosts
                OutputContainer = "PseudoJetGhostAntiKt4TrackJet_AODFix",
                SkipNegativeEnergy = True,
                GhostScale = ghostScaleFactor,   # This makes the PseudoJet Ghosts, and thus the reco flow will treat them as so.
                )
            ToolSvc += gakt4trackget

            # Truth.
            mctc = None
            truthpartcopy = None
            gtruthget = None
            gtruthgetters = []
            rtools = []
            rtools += [tracksel, tvassoc]
            if self.isMC:
                mctc = CfgMgr.MCTruthClassifier(name = "JetMCTruthClassifier_AODFix",
                                                ParticleCaloExtensionTool="")
                ToolSvc += mctc

                truthpartcopy = CfgMgr.CopyTruthJetParticles("truthpartcopy_AODFix",
                                                             OutputName="JetInputTruthParticles_AODFix",
                                                             MCTruthClassifier=mctc)
                ToolSvc += truthpartcopy

                gtruthget = CfgMgr.PseudoJetGetter(
                    "gtruthget_AODFix",
                    Label = "GhostTruth",
                    InputContainer = truthpartcopy.OutputName,
                    OutputContainer = "PseudoJetGhostTruth_AODFix",
                    GhostScale = ghostScaleFactor,
                    SkipNegativeEnergy = True,
                    )
                ToolSvc += gtruthget

                # Truth flavor tags.
                truthFlavourTags = ["BHadronsInitial", "BHadronsFinal", "BQuarksFinal",
                                    "CHadronsInitial", "CHadronsFinal", "CQuarksFinal",
                                    "TausFinal",
                                    "WBosons", "ZBosons", "HBosons", "TQuarksFinal",
                                    "Partons",
                                    ]
                for ptype in truthFlavourTags:
                    gtruthgetters.append(CfgMgr.PseudoJetGetter(
                            "gtruthget_" + ptype + "_AODFix",
                            InputContainer = "TruthLabel" + ptype + "_AODFix",
                            Label = "Ghost" + ptype,
                            OutputContainer = "PseudoJetGhost" + ptype + "_AODFix",
                            SkipNegativeEnergy = True,
                            GhostScale = ghostScaleFactor,
                            ))
                    ToolSvc += gtruthgetters[-1]

                    toolname = "CopyTruthTag" + ptype + "_AODFix"
                    print "Scheduling " + toolname
                    ptmin = 5000
                    if ptype == "Partons":
                        ctp = CfgMgr.CopyTruthPartons(toolname)
                    elif ptype in ["WBosons", "ZBosons", "HBosons", "TQuarksFinal"]:
                        ctp = CfgMgr.CopyBosonTopLabelTruthParticles(toolname)
                        ctp.ParticleType = ptype
                        ptmin = 100000
                    else:
                        ctp = CfgMgr.CopyFlavorLabelTruthParticles(toolname)
                        ctp.ParticleType = ptype
                    ctp.OutputName = "TruthLabel" + ptype + "_AODFix"
                    ctp.PtMin = ptmin
                    ToolSvc += ctp
                    rtools.append( ctp )

                # build truth jet input :
                rtools += [ truthpartcopy]

            jetrun = CfgMgr.JetToolRunner("jetrun_AODFix",
                                 Tools=rtools,
                                 )
            ToolSvc += jetrun

            #Setup a pseudojetgetter to deal with input objects
            empflowget = CfgMgr.PFlowPseudoJetGetter(
                "empflowget_AODFix",
                Label = "EMPFlow",
                OutputContainer = "PseudoJetEMPFlow_AODFix",
                RetrievePFOTool = pflowRetriever,
                InputIsEM = True,
                CalibratePFO = False,
                SkipNegativeEnergy = True,
                UseVertices = True
            )
            ToolSvc += empflowget

            empfgetters = [empflowget,gtrackget,gmusegget,gakt2trackget,gakt3trackget,gakt4trackget]
            if self.isMC:
                empfgetters += [gtruthget]+gtruthgetters

            particleFlowJetRecTool.PseudoJetGetters = empfgetters
            particleFlowJetRecTool.Overwrite = True
            particleFlowJetRecTool.OutputContainer = "AntiKt4EMPFlowJets"

            #setup calibration tools - only want to do area subtraction (as is done in Tier0)
            jetdefn = particleFlowJetFinder_AntiKt4.JetAlgorithm + str(int(10*particleFlowJetFinder_AntiKt4.JetRadius+0.1)) + empflowget.Label
            pflowJetCalibToolName_AODFix="calib_"+jetdefn+"_pflow_A_AODFix"
            ToolSvc += CfgMgr.JetCalibrationTool("pflowJetCalibTool_AODFix",JetCollection=jetdefn,ConfigFile="PFlowJES_September2014.config",CalibSequence="JetArea",RhoKey="Kt4EMPFlowEventShape")
            particleFlowJetRecTool.JetModifiers = [ToolSvc.pflowJetCalibTool_AODFix]

            #now we setup the other modifiers we need
            ToolSvc += CfgMgr.JetFilterTool("jetFilter_AODFix",PtMin=5000)
            particleFlowJetRecTool.JetModifiers += [ToolSvc.jetFilter_AODFix]

            #common_ungroomed_modifiers start here
            ToolSvc += CfgMgr.JetWidthTool("width_AODFix")
            particleFlowJetRecTool.JetModifiers += [ToolSvc.width_AODFix]

            ToolSvc += CfgMgr.JetIsolationTool("jetisol_AODFix",IsolationCalculations = ["IsoDelta:2:SumPt", "IsoDelta:3:SumPt"])
            particleFlowJetRecTool.JetModifiers += [ToolSvc.jetisol_AODFix]

            ToolSvc +=  CfgMgr.KtDeltaRTool("ktdr_AODFix",JetRadius = 0.4);
            particleFlowJetRecTool.JetModifiers += [ToolSvc.ktdr_AODFix]

            ToolSvc +=  CfgMgr.NSubjettinessTool("nsubjettiness_AODFix",Alpha = 1.0)
            particleFlowJetRecTool.JetModifiers += [ToolSvc.nsubjettiness_AODFix]

            ToolSvc +=  CfgMgr.KTSplittingScaleTool("ktsplitter_AODFix")
            particleFlowJetRecTool.JetModifiers += [ToolSvc.ktsplitter_AODFix]

            ToolSvc += CfgMgr.AngularityTool("angularity_AODFix")
            particleFlowJetRecTool.JetModifiers += [ToolSvc.angularity_AODFix]

            ToolSvc += CfgMgr.DipolarityTool("dipolarity_AODFix", SubJetRadius = 0.3)
            particleFlowJetRecTool.JetModifiers += [ToolSvc.dipolarity_AODFix]

            ToolSvc += CfgMgr.PlanarFlowTool("planarflow_AODFix")
            particleFlowJetRecTool.JetModifiers += [ToolSvc.planarflow_AODFix]

            ToolSvc += CfgMgr.KtMassDropTool("ktmassdrop_AODFix")
            particleFlowJetRecTool.JetModifiers += [ToolSvc.ktmassdrop_AODFix]

            ToolSvc += CfgMgr.EnergyCorrelatorTool("encorr_AODFix", Beta = 1.0)
            particleFlowJetRecTool.JetModifiers += [ToolSvc.encorr_AODFix]

            ToolSvc += CfgMgr.CenterOfMassShapesTool("comshapes_AODFix")
            particleFlowJetRecTool.JetModifiers += [ToolSvc.comshapes_AODFix]

            #common_ungroomed_modifiers that are for MC only
            if self.isMC:
                ToolSvc += CfgMgr.Analysis__JetPartonTruthLabel("partontruthlabel_AODFix")
                particleFlowJetRecTool.JetModifiers += [ToolSvc.partontruthlabel_AODFix]

                ToolSvc += CfgMgr.Analysis__JetQuarkLabel("jetquarklabel_AODFix",McEventCollection = "TruthEvents")
                ToolSvc += CfgMgr.Analysis__JetConeLabeling("truthpartondr_AODFix",JetTruthMatchTool = ToolSvc.jetquarklabel_AODFix)
                particleFlowJetRecTool.JetModifiers += [ToolSvc.truthpartondr_AODFix]
            ## end of JM modification
            #common_ungroomed_modifiers end here

            # Modifiers for topo (and pflow) jets.
            jetens = CfgMgr.JetCaloEnergies("jetens_AODFix")
            ToolSvc += jetens

            larhvcorr = CfgMgr.JetLArHVTool("larhvcorr_AODFix")
            ToolSvc += larhvcorr

            caloqual_cluster = CfgMgr.JetCaloQualityTool(
                "caloqual_cluster_AODFix",
                TimingCuts = [5, 10],
                Calculations = ["LArQuality", "N90Constituents", "FracSamplingMax",
                                "NegativeE", "Timing", "HECQuality", "Centroid",
                                "AverageLArQF", "BchCorrCell"],
                )
            ToolSvc += caloqual_cluster

            # Jet vertex fraction with selection.
            jvf = CfgMgr.JetVertexFractionTool(
              "jvf_AODFix",
              VertexContainer = "PrimaryVertices",
              AssociatedTracks = "GhostTrack",
              TrackVertexAssociation = tvassoc.TrackVertexAssociation,
              TrackSelector = trackselloose,
              JVFName = "JVF"
            )
            ToolSvc += jvf

            # Jet vertex tagger.
            jvt = CfgMgr.JetVertexTaggerTool(
              "jvt_AODFix",
              VertexContainer = "PrimaryVertices",
              TrackParticleContainer  = "InDetTrackParticles",
              AssociatedTracks = "GhostTrack",
              TrackVertexAssociation = tvassoc.TrackVertexAssociation,
              TrackSelector = trackselloose,
              JVTName = "Jvt",
              K_JVFCorrScale = 0.01,
              Z0Cut = 3.0,
              PUTrkPtCut = 30000.0
            )
            ToolSvc += jvt

            # Jet track info.
            trkmoms = CfgMgr.JetTrackMomentsTool(
              "trkmoms_AODFix",
              VertexContainer = "PrimaryVertices",
              AssociatedTracks = "GhostTrack",
              TrackVertexAssociation = tvassoc.TrackVertexAssociation,
              TrackMinPtCuts = [500, 1000],
              TrackSelector = trackselloose
            )
            ToolSvc += trkmoms

            charge = CfgMgr.JetChargeTool("charge_AODFix", K=1.0)
            ToolSvc += charge                

            particleFlowJetRecTool.JetModifiers += [jetens,larhvcorr,caloqual_cluster,
                                                    jvf,jvt,trkmoms,charge]

            if self.isMC:
                truthassoc = CfgMgr.JetPtAssociationTool(
                    "truthassoc_AntiKt4_AODFix",
                    InputContainer="AntiKt4TruthJets", AssociationName="GhostTruth")
                ToolSvc += truthassoc

                jetdrlabeler = CfgMgr.ParticleJetDeltaRLabelTool(
                  "jetdrlabeler_AODFix",
                  LabelName = "HadronConeExclTruthLabelID",
                  BLabelName = "ConeExclBHadronsFinal",
                  CLabelName = "ConeExclCHadronsFinal",
                  TauLabelName = "ConeExclTausFinal",
                  BParticleCollection = "TruthLabelBHadronsFinal_AODFix",
                  CParticleCollection = "TruthLabelCHadronsFinal_AODFix",
                  TauParticleCollection = "TruthLabelTausFinal_AODFix",
                  PartPtMin = 5000.0,
                  JetPtMin = 4500.0,
                  DRMax = 0.3,
                  MatchMode = "MinDR"
                )
                ToolSvc += jetdrlabeler

                particleFlowJetRecTool.JetModifiers += [truthassoc,jetdrlabeler]

            JetType = 'PFlowJet'
            PFtermlist = ['PFlowJet','Muon','Ele','Gamma','Tau','Soft']
            newpfotool = CfgMgr.CP__RetrievePFOTool('MET_PFOTool_AODFix')
            ToolSvc += newpfotool

            #is this the right config ????? undefined before I put this here <griffith@cern.ch>
            newtrkseltool=CfgMgr.InDet__InDetTrackSelectionTool("IDTrkSel_METAssoc_AODFix",
                                                                CutLevel="TightPrimary",
                                                                maxZ0SinTheta=3,
                                                                maxD0overSigmaD0=2)

            from METReconstruction.METAssocConfig import AssocConfig, getAssociator
            PFAssociators = [ getAssociator(AssocConfig(config),suffix='AntiKt4EMPFlow',doPFlow=True,trkseltool=newtrkseltool,pfotool=newpfotool) for config in PFtermlist ]
            PFAssocTool = CfgMgr.met__METAssociationTool('MET_AssociationTool_AntiKt4EMPFlow_AODFix',
                                                         METAssociators = PFAssociators,
                                                         METSuffix = 'AntiKt4EMPFlow',
                                                         AllowOverwrite=True)
            ToolSvc += PFAssocTool

            from METUtilities.METMakerConfig import getMETMakerAlg

            METAssocAlg_PFlow = CfgMgr.met__METRecoAlg(name='METAssociation_PFlow_AODFix',RecoTools=[PFAssocTool])
            METMakerAlg_PF = getMETMakerAlg('AntiKt4EMPFlow',confsuffix="_AODFix")
            METMakerAlg_PF.AllowOverwrite = True
            
            #Set up our own sequence
            from AthenaCommon.AlgSequence import AthSequencer
            ParticleFlowJetSequence = AthSequencer("ParticleFlowSelectionSequence")
            topSequence += ParticleFlowJetSequence

            #Add an event filter to our sequence
            ParticleFlowJetSequence += CfgMgr.ParticleFlowEventFilter_r207("ParticleFlowEventFilter_r207")
            #Add the particle flow jet finding to our sequence
            ParticleFlowJetSequence += CfgMgr.JetAlgorithm("pflowJetAlg_AODFix", Tools = [jetrun,
                                                                                   ToolSvc.AntiKt4EMPFlowJets_AODFix])
            #Add MET map builder to our sequence
            ParticleFlowJetSequence += METAssocAlg_PFlow
            #Build the reference MET container
            ParticleFlowJetSequence += METMakerAlg_PF
    # End PFlow AODFix

    def met_postSystemRec(self, topSequence):
        # MET AODFix
        from METReconstruction.METAssocConfig import AssocConfig, getAssociator
        from METReconstruction.METReconstructionConf import METBadTrackEventFilter
        from AthenaCommon.AlgSequence import AthSequencer
        METMapSequence = AthSequencer("MET_LCEM_SelectionSequence_AODFix")
        topSequence += METMapSequence

        from AthenaCommon import CfgMgr
        newtrkseltool=CfgMgr.InDet__InDetTrackSelectionTool("IDTrkSel_METAssoc_AODFix",
                                             CutLevel="TightPrimary",
                                             maxZ0SinTheta=3,
                                             maxD0overSigmaD0=2)
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += newtrkseltool

        METMapSequence += METBadTrackEventFilter("METBadTrackEventFilter_AODFix",TrackSelectorTool=newtrkseltool)

        LCtermlist = ['LCJet','Muon','Ele','Gamma','Tau','Soft']
        LCAssociators = [ getAssociator(AssocConfig(config),suffix='AntiKt4LCTopo',doPFlow=False,trkseltool=newtrkseltool) for config in LCtermlist ]
        LCAssocTool = CfgMgr.met__METAssociationTool('MET_AssociationTool_AntiKt4LCTopo_AODFix',
                                                     METAssociators = LCAssociators,
                                                     METSuffix = 'AntiKt4LCTopo',
                                                     AllowOverwrite=True,
                                                     TCSignalState=1) # LC clusters
        ToolSvc += LCAssocTool

        EMtermlist = ['EMJet','Muon','Ele','Gamma','Tau','Soft']
        EMAssociators = [ getAssociator(AssocConfig(config),suffix='AntiKt4EMTopo',doPFlow=False,trkseltool=newtrkseltool) for config in EMtermlist ]
        EMAssocTool = CfgMgr.met__METAssociationTool('MET_AssociationTool_AntiKt4EMTopo_AODFix',
                                                     METAssociators = EMAssociators,
                                                     METSuffix = 'AntiKt4EMTopo',
                                                     AllowOverwrite=True,
                                                     TCSignalState=0) # EM clusters
        ToolSvc += EMAssocTool

        METAssocAlg_LCEM = CfgMgr.met__METRecoAlg(name='METAssociation_LCEM_AODFix',RecoTools=[LCAssocTool,EMAssocTool])
        METMapSequence += METAssocAlg_LCEM

        from METUtilities.METMakerConfig import getMETMakerAlg
        METMakerAlg_LC = getMETMakerAlg('AntiKt4LCTopo',confsuffix="_AODFix")
        METMakerAlg_LC.AllowOverwrite = True
        METMakerAlg_EM = getMETMakerAlg('AntiKt4EMTopo',confsuffix="_AODFix")
        METMakerAlg_EM.AllowOverwrite = True
        METMapSequence += METMakerAlg_LC
        METMapSequence += METMakerAlg_EM

    # End MET AODFix

    def mcutils_postSystemRec(self, topSequence):
        """
        This fixes the MCUtils/TruthUtil bug:
        https://its.cern.ch/jira/browse/ATLASRECTS-3008
        """

        from AthenaCommon.AppMgr import ToolSvc

        if self.isMC:

            from ParticleJetTools.ParticleJetToolsConf import CopyFlavorLabelTruthParticles
            from ParticleJetTools.ParticleJetToolsConf import ParticleJetDeltaRLabelTool

            ptypes = ["TausFinal","BHadronsFinal","CHadronsFinal"]
            for ptype in ptypes:
                AODFix_ctp = CopyFlavorLabelTruthParticles("AODFix_CopyTruthTag"+ptype)
                AODFix_ctp.ParticleType = ptype
                AODFix_ctp.OutputName = "AODFix_TruthLabel" + ptype
                ToolSvc += AODFix_ctp

            AODFix_JetDeltaRLabelTool = ParticleJetDeltaRLabelTool(
              "AODFix_jetdrlabeler",
              LabelName = "HadronConeExclTruthLabelID",
              BLabelName = "ConeExclBHadronsFinal",
              CLabelName = "ConeExclCHadronsFinal",
              TauLabelName = "ConeExclTausFinal",
              BParticleCollection = "AODFix_TruthLabelBHadronsFinal",
              CParticleCollection = "AODFix_TruthLabelCHadronsFinal",
              TauParticleCollection = "AODFix_TruthLabelTausFinal",
              PartPtMin = 5000.0,
              JetPtMin = 0.0,
              DRMax = 0.3,
              MatchMode = "MinDR"
            )
            ToolSvc += AODFix_JetDeltaRLabelTool

            AODFix_FatJetDeltaRLabelTool = ParticleJetDeltaRLabelTool(
              "AODFix_fatjetdrlabeler",
              LabelName = "HadronConeExclTruthLabelID",
              BLabelName = "ConeExclBHadronsFinal",
              CLabelName = "ConeExclCHadronsFinal",
              TauLabelName = "ConeExclTausFinal",
              BParticleCollection = "AODFix_TruthLabelBHadronsFinal",
              CParticleCollection = "AODFix_TruthLabelCHadronsFinal",
              TauParticleCollection = "AODFix_TruthLabelTausFinal",
              PartPtMin = 5000.0,
              JetPtMin = 0.0,
              DRMax = 1.0,
              MatchMode = "MinDR"
            )
            ToolSvc += AODFix_FatJetDeltaRLabelTool

            AODFix_TrackJetDeltaRLabelTool = ParticleJetDeltaRLabelTool(
              "AODFix_trackjetdrlabeler",
              LabelName = "HadronConeExclTruthLabelID",
              BLabelName = "ConeExclBHadronsFinal",
              CLabelName = "ConeExclCHadronsFinal",
              TauLabelName = "ConeExclTausFinal",
              BParticleCollection = "AODFix_TruthLabelBHadronsFinal",
              CParticleCollection = "AODFix_TruthLabelCHadronsFinal",
              TauParticleCollection = "AODFix_TruthLabelTausFinal",
              PartPtMin = 5000.0,
              JetPtMin = 4500.0,
              DRMax = 0.3,
              MatchMode = "MinDR"
            )

            ToolSvc += AODFix_TrackJetDeltaRLabelTool

            from JetRec.JetRecConf import JetRecTool
            JetRecTool_AntiKt4EMTopo = JetRecTool("AODFix_AntiKt4EMTopoJets", InputContainer="AntiKt4EMTopoJets", OutputContainer="AntiKt4EMTopoJets", JetModifiers = [ToolSvc.AODFix_jetdrlabeler])
            ToolSvc += JetRecTool_AntiKt4EMTopo    
            JetRecTool_AntiKt4LCTopo = JetRecTool("AODFix_AntiKt4LCTopoJets", InputContainer="AntiKt4LCTopoJets", OutputContainer="AntiKt4LCTopoJets", JetModifiers = [ToolSvc.AODFix_jetdrlabeler])
            ToolSvc += JetRecTool_AntiKt4LCTopo    
            JetRecTool_AntiKt10LCTopo = JetRecTool("AODFix_AntiKt10LCTopoJets", InputContainer="AntiKt10LCTopoJets", OutputContainer="AntiKt10LCTopoJets", JetModifiers = [ToolSvc.AODFix_fatjetdrlabeler])
            ToolSvc += JetRecTool_AntiKt10LCTopo    
            JetRecTool_AntiKt2PV0Track = JetRecTool("AODFix_AntiKt2PV0TrackJets", InputContainer="AntiKt2PV0TrackJets", OutputContainer="AntiKt2PV0TrackJets", JetModifiers = [ToolSvc.AODFix_trackjetdrlabeler])
            ToolSvc += JetRecTool_AntiKt2PV0Track    
            JetRecTool_AntiKt3PV0Track = JetRecTool("AODFix_AntiKt3PV0TrackJets", InputContainer="AntiKt3PV0TrackJets", OutputContainer="AntiKt3PV0TrackJets", JetModifiers = [ToolSvc.AODFix_trackjetdrlabeler])
            ToolSvc += JetRecTool_AntiKt3PV0Track    
            JetRecTool_AntiKt4PV0Track = JetRecTool("AODFix_AntiKt4PV0TrackJets", InputContainer="AntiKt4PV0TrackJets", OutputContainer="AntiKt4PV0TrackJets", JetModifiers = [ToolSvc.AODFix_trackjetdrlabeler])
            ToolSvc += JetRecTool_AntiKt4PV0Track   

            from JetRec.JetRecConf import JetAlgorithm
            topSequence += JetAlgorithm("jetalgAODFix", Tools = [ToolSvc.AODFix_CopyTruthTagCHadronsFinal,ToolSvc.AODFix_CopyTruthTagBHadronsFinal, ToolSvc.AODFix_CopyTruthTagTausFinal, ToolSvc.AODFix_AntiKt2PV0TrackJets, ToolSvc.AODFix_AntiKt3PV0TrackJets, ToolSvc.AODFix_AntiKt4PV0TrackJets, ToolSvc.AODFix_AntiKt4EMTopoJets, ToolSvc.AODFix_AntiKt4LCTopoJets, ToolSvc.AODFix_AntiKt10LCTopoJets])

    def btagging_postSystemRec(self, topSequence):
        """
        This fixes the uptodate BTagging calibration conditions tag.
        """

        from AthenaCommon.AppMgr import ToolSvc

        JetCollectionList = [ 'AntiKt4EMTopoJets','AntiKt4LCTopoJets',
                              'AntiKt2TrackJets','AntiKt3TrackJets',
                              'AntiKt4TrackJets']

        SA = 'AODFix_'
        from BTagging.BTaggingConfiguration import getConfiguration
        BTagConf = getConfiguration("AODFix")
        BTagConf.PrefixxAODBaseName(False)
        BTagConf.PrefixVertexFinderxAODBaseName(False)

        BTagConf.doNotCheckForTaggerObstacles()
        from BTagging.BTaggingConf import Analysis__StandAloneJetBTaggerAlg as StandAloneJetBTaggerAlg

        btag = "BTagging_"
        AuthorSubString = [ btag+name[:-4] for name in JetCollectionList]
        for i, jet in enumerate(JetCollectionList):
            try:
                btagger = BTagConf.setupJetBTaggerTool(ToolSvc, JetCollection=jet[:-4], 
                                                       AddToToolSvc=True, SetupScheme="Retag", 
                                                       TaggerList = ['IP3D','MV2c00','MV2c10',
                                                                     'MV2c20','MV2c100','MV2m',
                                                                     'MV2c10hp','MV2cl100',
                                                                     'MVb','JetVertexCharge'])
                jet = jet.replace("Track", "PV0Track")
                SAbtagger = StandAloneJetBTaggerAlg(name=SA + AuthorSubString[i].lower(),
                                          JetBTaggerTool=btagger,
                                          JetCollectionName = jet,
                                          )

                topSequence += SAbtagger
            except AttributeError as error:
                print '#BTAG# --> ' + str(error)
                print '#BTAG# --> ' + jet
                print '#BTAG# --> ' + AuthorSubString[i]
