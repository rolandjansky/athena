# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


# import Common Algs
from DerivationFrameworkJetEtMiss.JetCommon import DFJetAlgs

# I wish we didn't need this
from BTagging.BTaggingConfiguration import getConfiguration
ConfInst=getConfiguration()

from GaudiKernel.Configurable import WARNING

# Import star stuff (it was like that when I got here)
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from JetRec.JetRecConf import JetAlgorithm

#===================================================================
# ExKt/CoM Subjets
#===================================================================

# make exkt subjet finding tool
def buildExclusiveSubjets(ToolSvc, JetCollectionName, subjet_mode, nsubjet, doGhostAssoc, doTrackSubJet, ExGhostLabels=["GhostBHadronsFinal", "GhostCHadronsFinal", "GhostTrack"], min_subjet_pt_mev = 0):
    #
    # a full list of ExGhostLabels = ["GhostBHadronsFinal", "GhostBHadronsInitial", "GhostBQuarksFinal", "GhostCHadronsFinal", "GhostCHadronsInitial",
    # "GhostCQuarksFinal", "GhostHBosons", "GhostPartons", "GhostTQuarksFinal", "GhostTausFinal", "GhostTruth", "GhostTrack"]
    #

    from JetRec.JetRecStandard import jtm

    subjetlabel = "Ex%s%iSubJets" % (subjet_mode, nsubjet)
    doCoM = False
    algj = "Kt"
    if subjet_mode == "CoM" :
      doCoM = True
      #
      #supported algorithms: Reconstruction/Jet/JetSubStructureUtils/Root/SubjetFinder.cxx
      #algorithms:"ee_kt" or "EEKt" for EE_Kt
      #           "cambridge" or "CamKt" for Cambridge
      #           "FastJetPlugin" for EECambridge plugin
      algj = "ee_kt"

    talabel = ""
    if doGhostAssoc:
      talabel = "GA"
    subjetlabel = "Ex%s%i%sSubJets" % (subjet_mode, nsubjet, talabel)

    # removing truth labels if runining on data
    if globalflags.DataSource()=='data': ExGhostLabels = ["GhostTrack"]

    SubjetContainerName = "%sEx%s%i%sSubJets" % (JetCollectionName.replace("Jets", ""), subjet_mode, nsubjet, talabel)
    ExKtbbTagToolName = str( "Ex%s%sbbTagTool%i_%s" % (subjet_mode, talabel, nsubjet, JetCollectionName) )

    if hasattr(jtm, ExKtbbTagToolName):
        ExKtbbTagToolInstance = jtm[ ExKtbbTagToolName ]
        print " ExKtbbTagTool ", ExKtbbTagToolName, "is alredy in jtm.tools"
    else:
        print " building ExKtbbTagTool ", ExKtbbTagToolName

        from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import SubjetFinderTool
        from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import SubjetRecorderTool

        subjetrecorder = SubjetRecorderTool("subjetrecorder_%s%i%s_%s" % (subjet_mode, nsubjet, talabel, JetCollectionName))
        ToolSvc += subjetrecorder
        subjetrecorder.SubjetLabel = subjetlabel
        subjetrecorder.SubjetContainerName = SubjetContainerName

        if not hasattr(ToolSvc, "ExKtTrackSelectorLoose"):
          from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
          ToolSvc += InDet__InDetTrackSelectionTool("ExKtTrackSelectorLoose",
                                                    CutLevel = "Loose",
                                                   )

        from JetTagTools.JetTagToolsConf import Analysis__ExKtbbTagTool
        ExKtbbTagToolInstance = Analysis__ExKtbbTagTool(
          name = ExKtbbTagToolName,
          JetAlgorithm = algj,
          JetRadius = 10.0,
          PtMin = min_subjet_pt_mev,
          ExclusiveNJets = nsubjet,
          doTrack = doTrackSubJet,
          InputJetContainerName = JetCollectionName,
          SubjetContainerName = SubjetContainerName,
          SubjetRecorder = subjetrecorder,
          SubjetLabel = subjetlabel,
          SubjetAlgorithm_BTAG = "AntiKt",
          SubjetRadius_BTAG = 0.4,
          TrackSelectionTool = ToolSvc.ExKtTrackSelectorLoose,
          PrimaryVtxContainerName = "PrimaryVertices",
          SubjetBoostConstituent = doCoM,
          GhostLabels = ",".join(ExGhostLabels)
        )
        jtm.add( ExKtbbTagToolInstance )
        ToolSvc += ExKtbbTagToolInstance

    return (ExKtbbTagToolInstance, SubjetContainerName)

#===================================================================
# Build ExKt or CoM Subjets, default is ExKt
#===================================================================
def addExKtCoM(sequence, ToolSvc, JetCollectionExCoM, nSubjets, doTrackSubJet, doGhostAssoc=False, ExGhostLabels=["GhostBHadronsFinal","GhostCHadronsFinal","GhostTrack"], min_subjet_pt_mev = 0, subjetAlgName = "Kt"):
    if(subjetAlgName != "Kt" and subjetAlgName != "CoM"):
      print "WARNING:  Subjet type must be Kt or CoM.  Using Kt as default!"
      subjetAlgName = "Kt"
    from JetRec.JetRecStandard import jtm
    ExCoMJetCollection__SubJet = []

    (ExCoMbbTagToolInstance, SubjetContainerName) = buildExclusiveSubjets(ToolSvc, JetCollectionExCoM, subjetAlgName, nSubjets, doGhostAssoc, doTrackSubJet, ExGhostLabels, min_subjet_pt_mev)

    ExCoMJetCollection__SubJet += [SubjetContainerName]


    excomELresetName = "ELreset_subjet_%s" % (SubjetContainerName.replace("Jets", ""))
    excomELresetNameLJet = "ELreset_Large_%sjet_%s" % (SubjetContainerName.replace("Jets", ""),JetCollectionExCoM.replace("Jets", ""))
    excomAlgName = "jfind_%s" % (SubjetContainerName)
    excomJetRecToolName = "%s" % (SubjetContainerName)
    excomBTagName = "BTagging_%s" % (SubjetContainerName.replace("Jets", ""))

    if excomAlgName in DFJetAlgs:
        print " Algorithm Ex%s "%subjetAlgName, excomAlgName, "already built before sequence ", sequence

        if hasattr(sequence, excomAlgName):
            print " sequence Ex%s "%subjetAlgName, sequence, "already has an instance of algorithm", excomAlgName
        else:
            print " Add algorithm Ex%s "%subjetAlgName, excomAlgName, "to sequence", sequence
            sequence += DFJetAlgs[excomAlgName]
            sequence += CfgMgr.xAODMaker__ElementLinkResetAlg(excomELresetName, SGKeys=[SubjetContainerName+"Aux."])
            sequence += DFJetAlgs[excomAlgName+"_btag"]
            sequence += CfgMgr.xAODMaker__ElementLinkResetAlg(excomELresetNameLJet, SGKeys=[JetCollectionExCoM+"Aux."])
    else:
        print " Algorithm Ex%s "%subjetAlgName, excomAlgName, " to be built sequence ", sequence
        if hasattr(jtm, excomJetRecToolName):
            print " Ex%sJetRecTool "%subjetAlgName, excomJetRecToolName, " already built sequence ",  sequence
            jetrec = jtm [ excomJetRecToolName ]
        else:
            print " Ex%s tool "%subjetAlgName, excomJetRecToolName, " to be built sequence ", sequence
            from JetRec.JetRecConf import JetRecTool
            jetrec = JetRecTool(
                                 name = excomJetRecToolName,
                                 OutputContainer = JetCollectionExCoM,
                                 InputContainer = JetCollectionExCoM,
                                 JetModifiers = [ExCoMbbTagToolInstance],
                               )
            jtm.add( jetrec )
            ToolSvc += jetrec

        excomJetRecBTagToolName = str( "%s_sub"%excomJetRecToolName )
        if hasattr(jtm, excomJetRecBTagToolName):
            print " Ex%sJetRecBTagTool "%subjetAlgName, excomJetRecBTagToolName, " already built sequence ",  sequence
            jetrec_btagging = jtm [ excomJetRecBTagToolName ]
        else:
            print " Ex%sJetRecBTagTool "%subjetAlgName, excomJetRecBTagToolName, " to be built sequence ",  sequence

            #make the btagging tool for excom jets
            from BTagging.BTaggingFlags import BTaggingFlags
            btag_excom = ConfInst.setupJetBTaggerTool(ToolSvc, JetCollection=excomJetRecToolName.replace("Jets", ""), AddToToolSvc=True, Verbose=True,
                         options={"name"         : excomBTagName.lower(),
                                  "BTagName"     : excomBTagName,
                                  "BTagJFVtxName": "JFVtx",
                                  "BTagSVName"   : "SecVtx",
                                  },
                         SetupScheme = "",
                         TaggerList = BTaggingFlags.StandardTaggers,
                         TrackAssociatorName="GhostTrack" if doGhostAssoc else "MatchedTracks"
                         )
            # running association + b-tagging on subjets now
            from BTagging.BTaggingConfiguration import comTrackAssoc, comMuonAssoc, defaultTrackAssoc, defaultMuonAssoc
            mods = [defaultTrackAssoc, defaultMuonAssoc, btag_excom]
            if(subjetAlgName=="CoM"): mods = [comTrackAssoc, comMuonAssoc, btag_excom]
            if globalflags.DataSource()!='data':
                mods.append(jtm.jetdrlabeler)

            jetrec_btagging = JetRecTool( name = excomJetRecBTagToolName,
                                      InputContainer  = SubjetContainerName,
                                      OutputContainer = SubjetContainerName,
                                      JetModifiers    = mods)
            jtm.add( jetrec_btagging )
            ToolSvc += jetrec_btagging

        jetalg_excom = JetAlgorithm(
                                 name = excomAlgName,
                                 Tools = [jetrec],
                                )
        sequence += jetalg_excom
        DFJetAlgs[excomAlgName] = jetalg_excom

        # Reset EL for ExCoM subjets after all of them are built
        # Otherwise crashing for CoM during TrackToJetAssociation due to failure of finding the large-R parent jet.
        sequence += CfgMgr.xAODMaker__ElementLinkResetAlg(excomELresetName, SGKeys=[SubjetContainerName+"Aux."])

        jetalg_excom_btag = JetAlgorithm(
                                 name = excomAlgName+"_btag",
                                 Tools = [jetrec_btagging],
                                )
        sequence += jetalg_excom_btag
        DFJetAlgs[excomAlgName+"_btag"] = jetalg_excom_btag
        sequence += CfgMgr.xAODMaker__ElementLinkResetAlg(excomELresetNameLJet, SGKeys=[JetCollectionExCoM+"Aux."])

    return ExCoMJetCollection__SubJet

##################################################################
# Build variable-R subjets, recluster AntiKt10LCTopojet with ghost VR and copy ghost link to AntiKt10LCTopo
##################################################################
def addVRJets(sequence, do_ghost=False, logger=None, doFlipTagger=False, *pos_opts, **opts):
    from AthenaCommon import Logging

    if logger is None:
        logger = Logging.logging.getLogger('VRLogger')

    # define constants here, since we don't want every derivaiton
    # deciding for themselves what a VR jet is. If we do want this
    # flexibility, this code will need some rewriting to ensure that
    # there are no issues with train safety.
    if opts or pos_opts:
        logger.error('Options specified for VR jets, they will be ignored')

    VRName, ghostLab = buildVRJets(sequence, do_ghost, logger, doFlipTagger)
    linkVRJetsToLargeRJets(sequence, VRName, ghostLab)

def buildVRJets(sequence, do_ghost, logger, doFlipTagger):
    from JetRec.JetRecStandard import jtm

    VRJetName="AntiKtVR30Rmax4Rmin02Track"
    VRGhostLabel="GhostVR30Rmax4Rmin02TrackJet"
    VRJetAlg="AntiKt"
    VRJetRadius=0.4
    VRJetInputs='pv0track'
    VRJetOptions = dict(
        ghostArea = 0 , ptmin = 2000,
        variableRMinRadius = 0.02, variableRMassScale = 30000,
        calibOpt = "none")

    # Change some options if we have do_ghost set to true. Hopefully
    # this will be the only VR collection in the future.
    if do_ghost:
        ghost_suffix = "GhostTag"
        VRJetName += ghost_suffix
        VRGhostLabel += ghost_suffix
        VRJetOptions['ptmin'] = 5000

    #==========================================================
    # Build VR jets
    #==========================================================

    VRJetAlgName = "jfind_%sJets" % (VRJetName)
    VRJetRecToolName = "%sJets" % (VRJetName)
    VRJetBTagName = "BTagging_%s" % (VRJetName)

    from AthenaCommon.AppMgr import ToolSvc

    #make the btagging tool for VR jets
    from BTagging.BTaggingFlags import BTaggingFlags
    btag_vrjets = ConfInst.setupJetBTaggerTool(
        ToolSvc, JetCollection=VRJetRecToolName, AddToToolSvc=True, Verbose=True,
        options={"name"         : VRJetBTagName.lower(),
                 "BTagName"     : VRJetBTagName,
                 "BTagJFVtxName": "JFVtx",
                 "BTagSVName"   : "SecVtx",
        },
        SetupScheme = "",
	    TaggerList = BTaggingFlags.ExpertTaggers if doFlipTagger else BTaggingFlags.StandardTaggers,
        TrackAssociatorName="GhostTrack" if do_ghost else "MatchedTracks"
    )

    from BTagging.BTaggingConfiguration import defaultTrackAssoc, defaultMuonAssoc

    # Slice the array - this forces a copy so that if we modify it we don't also
    # change the array in jtm.
    pseudoJetGetters = jtm.gettersMap[VRJetInputs][:]

    # We want to include ghost associated tracks in the pv0 tracks so that
    # we can use the looser ghost association criteria for b-tagging.
    if VRJetInputs == "pv0track":
        pseudoJetGetters.append(jtm["gtrackget"])

    if VRJetAlgName in DFJetAlgs:
        print "Algorithm", VRJetAlgName, "already built before"

        if hasattr(sequence, VRJetAlgName):
            print "   Sequence", sequence, "already has an instance of algorithm", VRJetAlgName
        else:
            print "   Add algorithm", VRJetAlgName, "to sequence", sequence
            sequence += DFJetAlgs[VRJetAlgName]
    else:
        print "Create algorithm", VRJetAlgName

        if hasattr(jtm, VRJetRecToolName):
            print "   JetRecTool", VRJetRecToolName, "is alredy in jtm.tools in sequence ", sequence
        else:
            print "   Create JetRecTool", VRJetRecToolName
            #can only run trackjetdrlabeler with truth labels, so MC only

            mods = [defaultTrackAssoc, defaultMuonAssoc, btag_vrjets]

            if globalflags.DataSource()!='data':
                mods.append(jtm.trackjetdrlabeler)

            jtm.addJetFinder(
                VRJetRecToolName,
                VRJetAlg,
                VRJetRadius,
                pseudoJetGetters,
                modifiersin=mods,
                ivtxin=0,
                **VRJetOptions)

        from JetRec.JetRecConf import JetAlgorithm
        jetalg_smallvr30_track = JetAlgorithm(VRJetAlgName, Tools = [ jtm[VRJetRecToolName] ])
        sequence += jetalg_smallvr30_track
        DFJetAlgs[VRJetAlgName] = jetalg_smallvr30_track

    #==========================================================
    # Build PseudoJet Getter
    #==========================================================

    pjgettername = VRGhostLabel.lower()

    if hasattr(jtm, pjgettername):
        print "Found", pjgettername, "in jtm in sequence", sequence
    else:
        print "Add", pjgettername, "to jtm in sequence", sequence

        from JetRec.JetRecConf import PseudoJetGetter
        jtm += PseudoJetGetter(
          pjgettername,                                                          # give a unique name
          InputContainer = jetFlags.containerNamePrefix() + VRJetName + "Jets",  # SG key
          Label = VRGhostLabel,                                                  # this is the name you'll use to retrieve ghost associated VR track jets
          OutputContainer = "PseudoJet" + VRGhostLabel,
          SkipNegativeEnergy = True,
          GhostScale = 1.e-20,                                                   # this makes the PseudoJet Ghosts, and thus the reco flow will treat them as such
        )
    return VRJetName, VRGhostLabel

def linkVRJetsToLargeRJets(sequence, VRJetName, VRGhostLabel,
                           baseGetterMap='lctopo',
                           baseLargeRJets='AntiKt10LCTopo',
                           modifiers="lctopo_ungroomed"):
    from JetRec.JetRecStandard import jtm
    pjgettername = VRGhostLabel.lower()
    #==========================================================
    # Re-cluster large-R jet with VR ghost associated on it
    # AntiKt10LCTopo hard-coded for now
    #==========================================================
    LargeRJetAlg     = "jfind_%s_%s" %(baseLargeRJets.lower(), VRJetName.lower())
    LargeRJets       = "%s_%sJets"   %(baseLargeRJets, VRJetName)
    LargeRJetPrefix  = "%s_%s"       %(baseLargeRJets, VRJetName)
    newLCTopo        = "%s_%s"            %(baseGetterMap, VRJetName.lower())
    LinkTransferAlg  = "LinkTransfer_%s_%s"     %(baseLargeRJets, VRJetName)

    if LargeRJetAlg in DFJetAlgs:
        print "  Found ", LargeRJetAlg," in DFJetAlgs in", sequence
        if hasattr(sequence, LargeRJetAlg):
            print "   Algsequence", sequence, "already has an instance of", LargeRJetAlg
        else:
            print "   Added ", LargeRJetAlg," to sequence ", sequence
            sequence += DFJetAlgs[LargeRJetAlg]

    else:
        if hasattr(jtm, LargeRJets):
            print  LargeRJets, " is alredy in jtm.tools in sequence ", sequence
        else:
            print "  Create new ", LargeRJets,"in", sequence
            OutputJets.setdefault("CustomJets" , [] ).append(LargeRJets)
            jtm.gettersMap[newLCTopo] = list(jtm.gettersMap[baseGetterMap])
            jtm.gettersMap[newLCTopo] += [ jtm[pjgettername] ]
            jtm.addJetFinder(LargeRJets, "AntiKt", 1.0, newLCTopo , modifiers,
                             ghostArea = 0 , ptmin = 40000, ptminFilter = 50000,
                             calibOpt = "none")

        jetalg_largefr10_lctopo = JetAlgorithm(LargeRJetAlg, Tools = [ jtm[LargeRJets] ])
        sequence += jetalg_largefr10_lctopo
        DFJetAlgs[LargeRJetAlg] = jetalg_largefr10_lctopo

    #==========================================================
    # Transfer the link from re-clustered jet to original jet
    # AntiKt10LCTopo hard-coded for now
    # Issue here: If addVRJets() is called more than once for different VR, only link to first VR would be transferred
    # Solution: decouple the following part with parts above
    #==========================================================

    from DerivationFrameworkJetEtMiss.ExtendedJetCommon import (
        getJetExternalAssocTool, applyJetAugmentation)
    jetassoctool = getJetExternalAssocTool(baseLargeRJets, LargeRJetPrefix, MomentPrefix='', ListOfOldLinkNames=[VRGhostLabel])
    applyJetAugmentation(baseLargeRJets, LinkTransferAlg, sequence, jetassoctool)

##################################################################
# Build variable-R subjets, recluster AntiKt10TCCjet with ghost VR and copy ghost link to AntiKt10TCC - Copy and pasting this defintion is an ugly temporary solution. We need to properly rewrite this for a more general use and remove the hardcoded jet collection.
##################################################################
def addVRJetsTCC(sequence, VRJetName, VRGhostLabel, VRJetAlg="AntiKt", VRJetRadius=0.4, VRJetInputs="pv0track", **VRJetOptions):
    from JetRec.JetRecStandard import jtm

    from AthenaCommon.AppMgr import ToolSvc
    #==========================================================
    # Build VR jets
    #==========================================================

    VRJetAlgName = "jfind_%sJets" % (VRJetName)
    VRJetRecToolName = "%sJets" % (VRJetName)
    VRJetBTagName = "BTagging_%s" % (VRJetName)

    #make the btagging tool for VR jets
    from BTagging.BTaggingFlags import BTaggingFlags
    btag_vrjets = ConfInst.setupJetBTaggerTool(ToolSvc, JetCollection=VRJetRecToolName, AddToToolSvc=True, Verbose=True,
                 options={"name"         : VRJetBTagName.lower(),
                          "BTagName"     : VRJetBTagName,
                          "BTagJFVtxName": "JFVtx",
                          "BTagSVName"   : "SecVtx",
                          },
                 SetupScheme = "",
                 TaggerList = BTaggingFlags.StandardTaggers
                 )

    if VRJetAlgName in DFJetAlgs:
        print "Algorithm", VRJetAlgName, "already built before"

        if hasattr(sequence, VRJetAlgName):
            print "   Sequence", sequence, "already has an instance of algorithm", VRJetAlgName
        else:
            print "   Add algorithm", VRJetAlgName, "to sequence", sequence
            sequence += DFJetAlgs[VRJetAlgName]
    else:
        print "Create algorithm", VRJetAlgName

        if hasattr(jtm, VRJetRecToolName):
            print "   JetRecTool", VRJetRecToolName, "is alredy in jtm.tools in sequence ", sequence
        else:
            print "   Create JetRecTool", VRJetRecToolName
            #can only run trackjetdrlabeler with truth labels, so MC only
            if globalflags.DataSource()!='data':
                jtm.addJetFinder(VRJetRecToolName, VRJetAlg, VRJetRadius, VRJetInputs, modifiersin=[trackassoc, muonassc, btag_vrjets,jtm.trackjetdrlabeler], **VRJetOptions)
            else:
                jtm.addJetFinder(VRJetRecToolName, VRJetAlg, VRJetRadius, VRJetInputs, modifiersin=[trackassoc, muonassoc, btag_vrjets], **VRJetOptions)

        from JetRec.JetRecConf import JetAlgorithm
        jetalg_smallvr30_track = JetAlgorithm(VRJetAlgName, Tools = [ jtm[VRJetRecToolName] ])
        sequence += jetalg_smallvr30_track
        DFJetAlgs[VRJetAlgName] = jetalg_smallvr30_track

    #==========================================================
    # Build PseudoJet Getter
    #==========================================================

    pjgettername = VRGhostLabel.lower()

    if hasattr(jtm, pjgettername):
        print "Found", pjgettername, "in jtm in sequence", sequence
    else:
        print "Add", pjgettername, "to jtm in sequence", sequence

        from JetRec.JetRecConf import PseudoJetGetter
        jtm += PseudoJetGetter(
          pjgettername,                                                          # give a unique name
          InputContainer = jetFlags.containerNamePrefix() + VRJetName + "Jets",  # SG key
          Label = VRGhostLabel,                                                  # this is the name you'll use to retrieve ghost associated VR track jets
          OutputContainer = "PseudoJet" + VRGhostLabel,
          SkipNegativeEnergy = True,
          GhostScale = 1.e-20,                                                   # this makes the PseudoJet Ghosts, and thus the reco flow will treat them as such
        )

    #==========================================================
    # Re-cluster large-R jet with VR ghost associated on it
    # AntiKt10TCC hard-coded for now
    #==========================================================
    LargeRJetAlg     = "jfind_akt10trackcalocluster_%s" %(VRJetName.lower())
    LargeRJets       = "AKt10TrackCaloCluster_%sJets"   %(VRJetName)
    LargeRJetPrefix  = "AKt10TrackCaloCluster_%s"       %(VRJetName)
    newLCTopo        = "tcc_%s"            %(VRJetName.lower())
    LinkTransferAlg  = "LinkTransfer_%s"     %(VRJetName)

    if LargeRJetAlg in DFJetAlgs:
        print "  Found ", LargeRJetAlg," in DFJetAlgs in", sequence
        if hasattr(sequence, LargeRJetAlg):
            print "   Algsequence", sequence, "already has an instance of", LargeRJetAlg
        else:
            print "   Added ", LargeRJetAlg," to sequence ", sequence
            sequence += DFJetAlgs[LargeRJetAlg]

    else:
        if hasattr(jtm, LargeRJets):
            print  LargeRJets, " is alredy in jtm.tools in sequence ", sequence
        else:
            print "  Create new ", LargeRJets,"in", sequence
            OutputJets.setdefault("CustomJets" , [] ).append(LargeRJets)
            jtm.gettersMap[newLCTopo] = list(jtm.gettersMap["tcc"])
            jtm.gettersMap[newLCTopo] += [ jtm[pjgettername] ]
            jtm.addJetFinder(LargeRJets, "AntiKt", 1.0, newLCTopo , "tcc_ungroomed",
                             ghostArea = 0 , ptmin = 40000, ptminFilter = 50000,
                             calibOpt = "none")
        from JetRec.JetRecConf import JetAlgorithm
        jetalg_largefr10_tcc = JetAlgorithm(LargeRJetAlg, Tools = [ jtm[LargeRJets] ])
        sequence += jetalg_largefr10_tcc
        DFJetAlgs[LargeRJetAlg] = jetalg_largefr10_tcc

    #==========================================================
    # Transfer the link from re-clustered jet to original jet
    # AntiKt10TCC hard-coded for now
    # Issue here: If addVRJets() is called more than once for different VR, only link to first VR would be transferred
    # Solution: decouple the following part with parts above
    #==========================================================
    from DerivationFrameworkJetEtMiss.ExtendedJetCommon import (
        getJetExternalAssocTool, applyJetAugmentation)
    jetassoctool = getJetExternalAssocTool('AntiKt10TrackCaloCluster', LargeRJetPrefix, MomentPrefix='', ListOfOldLinkNames=[VRGhostLabel])
    applyJetAugmentation('AntiKt10TrackCaloCluster', LinkTransferAlg, sequence, jetassoctool)

##################################################################
# Build variable-R calorimeter jets
##################################################################
def addVRCaloJets(sequence,outputlist,dotruth=True,writeUngroomed=False):
    if DerivationFrameworkIsMonteCarlo and dotruth:
        addTrimmedJets('AntiKt', 1.0, 'Truth', rclus=0.2, ptfrac=0.05, variableRMassScale=600000, variableRMinRadius=0.2, mods="truth_groomed",
                       algseq=sequence, outputGroup=outputlist, writeUngroomed=writeUngroomed)
    addTrimmedJets('AntiKt', 1.0, 'PV0Track', rclus=0.2, ptfrac=0.05, variableRMassScale=600000, variableRMinRadius=0.2, mods="groomed",
                   algseq=sequence, outputGroup=outputlist, writeUngroomed=writeUngroomed)
    addTrimmedJets('AntiKt', 1.0, 'LCTopo', rclus=0.2, ptfrac=0.05, variableRMassScale=600000, variableRMinRadius=0.2, mods="lctopo_groomed",
                   algseq=sequence, outputGroup=outputlist, writeUngroomed=writeUngroomed)

#===================================================================
# Utils: Copy Jets
#===================================================================
def addCopyJet(FTAG5Seq, ToolSvc, InputJetCollectionName, OutputJetCollectionName, **options):
  doShallowCopy = options.pop("ShallowCopy", False)

  from JetRec.JetRecConf import JetRecTool
  jetrec = JetRecTool(
                       name = "JetRecTool_CopyJet_%s_%s" % (InputJetCollectionName, OutputJetCollectionName),
                       OutputContainer = OutputJetCollectionName,
                       InputContainer = InputJetCollectionName,

                       ShallowCopy = doShallowCopy,
                       **options
                     )

  ToolSvc += jetrec
  FTAG5Seq += JetAlgorithm(
                            name = "JetAlgorithm_CopyJet_%s_%s" % (InputJetCollectionName, OutputJetCollectionName),
                            Tools = [jetrec],
                          )

  return OutputJetCollectionName


#========================================================================
# Hbb Tagger
#========================================================================
def get_unique_name(strings):
    clean_strings = []
    for string in strings:
        chars = []
        for char in string:
            chars += char if (char.isalpha() or char.isdigit()) else '_'
        clean_strings.append(''.join(chars))
    return '_'.join(clean_strings)

def addHbbTagger(
        sequence, ToolSvc, logger=None,
        output_level=WARNING,
        jet_collection="AntiKt10LCTopoTrimmedPtFrac5SmallR20",
        nn_file_name="BoostedJetTaggers/HbbTagger/Summer2018/Apr13HbbNetwork.json",
        nn_config_file="BoostedJetTaggers/HbbTaggerDNN/PreliminaryConfigNovember2017.json"):
    if logger is None:
        logger = Logging.logging.getLogger('HbbTaggerLog')

    fat_calibrator_name = get_unique_name(["HbbCalibrator", jet_collection])
    is_data = not DerivationFrameworkIsMonteCarlo
    if not hasattr(ToolSvc, fat_calibrator_name):
        fatCalib = CfgMgr.JetCalibrationTool(
            fat_calibrator_name,
            OutputLevel=output_level,
            JetCollection=jet_collection,
            ConfigFile="JES_MC16recommendation_FatJet_JMS_comb_19Jan2018.config",
            CalibSequence="EtaJES_JMS",
            CalibArea="00-04-81",
            IsData=is_data)
        ToolSvc += fatCalib
        logger.info('set up {}'.format(fatCalib))
    else:
        logger.info('took {} from tool svc'.format(fat_calibrator_name))
        fatCalib = getattr(ToolSvc, fat_calibrator_name)

    # short name for naming tools
    nn_short_file = nn_file_name.split('/')[-1].split('.')[0]

    hbb_tagger_name = get_unique_name(["HbbTagger",nn_short_file])
    if not hasattr(ToolSvc, hbb_tagger_name):
        hbbTagger = CfgMgr.HbbTaggerDNN(
            hbb_tagger_name,
            OutputLevel=output_level,
            neuralNetworkFile=nn_file_name,
            configurationFile=nn_config_file)
        ToolSvc += hbbTagger
        logger.info('set up {}'.format(hbbTagger))
    else:
        logger.info('took {} from tool svc'.format(hbb_tagger_name))
        hbbTagger = getattr(ToolSvc, hbb_tagger_name)

    tagger_alg_name = get_unique_name(
        ["HbbTaggerAlg",jet_collection, nn_short_file])
    if not hasattr(sequence, tagger_alg_name):
        if tagger_alg_name in DFJetAlgs:
            sequence += DFJetAlgs[tagger_alg_name]
            logger.info('took {} from jet algs'.format(tagger_alg_name))
        else:
            tagger_alg = CfgMgr.HbbTaggingAlgorithm(
                tagger_alg_name,
                OutputLevel=output_level,
                jetCollectionName=(jet_collection + "Jets"),
                minPt=250e3,
                maxEta=2.0,
                tagger=hbbTagger,
                calibrationTool=fatCalib)
            DFJetAlgs[tagger_alg_name] = tagger_alg
            sequence += tagger_alg
            logger.info('set up {}'.format(tagger_alg))
    else:
        logger.info('{} already scheduled for {}'.format(
            tagger_alg_name, jet_collection))

def addRecommendedXbbTaggers(sequence, ToolSvc, logger=None):
    addHbbTagger(sequence, ToolSvc, logger)
    addHbbTagger(
        sequence, ToolSvc,
        nn_file_name="BoostedJetTaggers/HbbTagger/Summer2018/MulticlassNetwork.json",
        nn_config_file="BoostedJetTaggers/HbbTaggerDNN/MulticlassConfigJune2018.json")

xbbTaggerExtraVariables = [
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.HbbScore",
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.XbbScoreHiggs.XbbScoreTop.XbbScoreQCD"]

#====================================================================
# Large-R RC jets w/ ExKt 2 & 3 subjets
#===================================================================
def addExKtDoubleTaggerRCJets(sequence, ToolSvc):#, ExKtJetCollection__FatJetConfigs, ExKtJetCollection__FatJet, ExKtJetCollection__SubJet):#, jetToolName, algoName):
   DFisMC = (globalflags.DataSource()=='geant4')
   jetToolName = "DFReclustertingTool"
   algoName = "DFJetReclusteringAlgo"

   ExKtJetCollection__FatJet = "AntiKt8EMTopoJets"
   ExKtJetCollection__SubJet = []

   if jetToolName not in DFJetAlgs:
     ToolSvc += CfgMgr.JetReclusteringTool(jetToolName,InputJetContainer="AntiKt4EMTopoJets", OutputJetContainer="AntiKt8EMTopoJets")
     getattr(ToolSvc,jetToolName).ReclusterRadius = 0.8
     getattr(ToolSvc,jetToolName).InputJetPtMin = 0
     getattr(ToolSvc,jetToolName).RCJetPtMin = 1
     getattr(ToolSvc,jetToolName).RCJetPtFrac = 0
     getattr(ToolSvc,jetToolName).DoArea = False
     getattr(ToolSvc,jetToolName).GhostTracksInputContainer = "InDetTrackParticles"
     getattr(ToolSvc,jetToolName).GhostTracksVertexAssociationName  = "JetTrackVtxAssoc"
     if(DFisMC):
       getattr(ToolSvc,jetToolName).GhostTruthInputBContainer = "BHadronsFinal"
       getattr(ToolSvc,jetToolName).GhostTruthInputCContainer = "CHadronsFinal"

     sequence += CfgMgr.AthJetReclusteringAlgo(algoName, JetReclusteringTool = getattr(ToolSvc,jetToolName))
     DFJetAlgs[jetToolName] = getattr(ToolSvc,jetToolName)

   # build subjets
   GhostLabels = ["GhostTrack"]
   if(DFisMC):
     GhostLabels += ["GhostBHadronsFinal"]
     GhostLabels += ["GhostCHadronsFinal"]
   # N=2 subjets
   ExKtJetCollection__SubJet += addExKtCoM(sequence, ToolSvc, ExKtJetCollection__FatJet, nSubjets=2, doTrackSubJet=True, ExGhostLabels=GhostLabels, min_subjet_pt_mev=1)
   # N=3 subjets
   ExKtJetCollection__SubJet += addExKtCoM(sequence, ToolSvc, ExKtJetCollection__FatJet, nSubjets=3, doTrackSubJet=True, ExGhostLabels=GhostLabels, min_subjet_pt_mev=1)

   sequence += CfgMgr.xAODMaker__ElementLinkResetAlg("ELReset_AfterSubjetBuild", SGKeys=[name+"Aux." for name in ExKtJetCollection__SubJet])

   from BTagging.BTaggingFlags import BTaggingFlags
   BTaggingFlags.CalibrationChannelAliases += [ jetname[:-4].replace("PV0", "")+"->AntiKt4EMTopo" for jetname in ExKtJetCollection__FatJet ]
   BTaggingFlags.CalibrationChannelAliases += [ jetname[:-4].replace("PV0", "")+"->AntiKt4EMTopo" for jetname in ExKtJetCollection__SubJet ]

   sequence += CfgMgr.xAODMaker__ElementLinkResetAlg("ELReset_AfterBtag", SGKeys=[name+"Aux." for name in ExKtJetCollection__SubJet])
