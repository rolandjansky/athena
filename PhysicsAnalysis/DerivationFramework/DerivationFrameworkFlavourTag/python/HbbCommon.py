# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function


# import Common Algs
from DerivationFrameworkJetEtMiss.JetCommon import DFJetAlgs

from DerivationFrameworkCore.DerivationFrameworkMaster import (
    DerivationFrameworkIsMonteCarlo as isMC)

# I wish we didn't need this
from BTagging.BTaggingConfiguration import getConfiguration
ConfInst=getConfiguration()

from GaudiKernel.Configurable import WARNING, VERBOSE

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
    if not isMC: ExGhostLabels = ["GhostTrack"]

    SubjetContainerName = "%sEx%s%i%sSubJets" % (JetCollectionName.replace("Jets", ""), subjet_mode, nsubjet, talabel)
    ExKtbbTagToolName = str( "Ex%s%sbbTagTool%i_%s" % (subjet_mode, talabel, nsubjet, JetCollectionName) )

    if hasattr(jtm, ExKtbbTagToolName):
        ExKtbbTagToolInstance = jtm[ ExKtbbTagToolName ]
        print (" ExKtbbTagTool ", ExKtbbTagToolName, "is alredy in jtm.tools")
    else:
        print (" building ExKtbbTagTool ", ExKtbbTagToolName)

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
      print ("WARNING:  Subjet type must be Kt or CoM.  Using Kt as default!")
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
        print (" Algorithm Ex%s "%subjetAlgName, excomAlgName, "already built before sequence ", sequence)

        if hasattr(sequence, excomAlgName):
            print (" sequence Ex%s "%subjetAlgName, sequence, "already has an instance of algorithm", excomAlgName)
        else:
            print (" Add algorithm Ex%s "%subjetAlgName, excomAlgName, "to sequence", sequence)
            sequence += DFJetAlgs[excomAlgName]
            sequence += CfgMgr.xAODMaker__ElementLinkResetAlg(excomELresetName, SGKeys=[SubjetContainerName+"Aux."])
            sequence += DFJetAlgs[excomAlgName+"_btag"]
            sequence += CfgMgr.xAODMaker__ElementLinkResetAlg(excomELresetNameLJet, SGKeys=[JetCollectionExCoM+"Aux."])
    else:
        print (" Algorithm Ex%s "%subjetAlgName, excomAlgName, " to be built sequence ", sequence)
        if hasattr(jtm, excomJetRecToolName):
            print (" Ex%sJetRecTool "%subjetAlgName, excomJetRecToolName, " already built sequence ",  sequence)
            jetrec = jtm [ excomJetRecToolName ]
        else:
            print (" Ex%s tool "%subjetAlgName, excomJetRecToolName, " to be built sequence ", sequence)
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
            print (" Ex%sJetRecBTagTool "%subjetAlgName, excomJetRecBTagToolName, " already built sequence ",  sequence)
            jetrec_btagging = jtm [ excomJetRecBTagToolName ]
        else:
            print (" Ex%sJetRecBTagTool "%subjetAlgName, excomJetRecBTagToolName, " to be built sequence ",  sequence)

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
            if isMC:
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
# Associate the VR track jets to existing large-R jet collections
# 
# For any groomed jet collections, the association will be done to
# the parent jet collection (so for the
# AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets collection the 
# association will be done to the AntiKt10LCTopoJets collection).
# 
# If no jet collection is supplied, the association is done to
# AntiKt10LCTopoJets
##################################################################
def addVRJets(sequence, largeRColls = None, do_ghost=False, logger=None, doFlipTagger=False, training='201810', *pos_opts, **opts):
    from AthenaCommon import Logging

    if logger is None:
        logger = Logging.logging.getLogger('VRLogger')

    # define constants here, since we don't want every derivaiton
    # deciding for themselves what a VR jet is. If we do want this
    # flexibility, this code will need some rewriting to ensure that
    # there are no issues with train safety.
    if opts or pos_opts:
        logger.warning('Options specified for VR jets, they will be ignored')

    if largeRColls is None:
      largeRColls = [
          "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"
          ]

    VRName, ghostLab = buildVRJets(sequence, do_ghost, logger, doFlipTagger, training)
    toAssociate = {
        ghostLab : ghostLab.lower()
        }
    
    ungroomedNames = []
    for collection in largeRColls:
      ungroomedName, getters = linkVRJetsToLargeRJets(sequence, collection, toAssociate)
      ungroomedNames.append(ungroomedName)

    return ungroomedNames

def buildVRJets(sequence, do_ghost, logger = None, doFlipTagger=False, training='201810'):

    from AthenaCommon import Logging

    if logger is None:
        logger = Logging.logging.getLogger('VRLogger')

    supported_trainings = ['201810', '201903']
    # Check allowed trainings
    # Is there a better way to do this with a central DB?
    if training not in ['201810', '201903']:
      logger.warning("WARNING: Using an unsupported training tag! This is UNDEFINED and will probably break. Please choose a training tag from")
      logger.warning(supported_trainings)

    from JetRec.JetRecStandard import jtm

    # Making Chris Happy: all VR track-jet b-tagging should have the training campaign label
    trainingTag = '_BTagging%s' % (training)

    VRJetName="AntiKtVR30Rmax4Rmin02Track%s" % (trainingTag)
    VRGhostLabel="GhostVR30Rmax4Rmin02TrackJet%s" % (trainingTag)
    VRJetAlg="AntiKt"
    VRJetRadius=0.4
    VRJetInputs='pv0track'
    VRJetOptions = dict(
        ghostArea = 0 , ptmin = 4000,
        variableRMinRadius = 0.02, variableRMassScale = 30000,
        calibOpt = "none")

    # Change some options if we have do_ghost set to true. Hopefully
    # this will be the only VR collection in the future.
    if do_ghost:
        ghost_suffix = "GhostTag"
        VRJetName += ghost_suffix
        VRGhostLabel += ghost_suffix

    #==========================================================
    # Build VR jets
    #==========================================================

    from DerivationFrameworkJetEtMiss.ExtendedJetCommon import nameJetsFromAlg
    VRJetRecToolName = nameJetsFromAlg(VRJetName)
    VRJetAlgName = "jfind_%s" % (VRJetRecToolName)
    VRJetBTagName = "BTagging_%s" % (VRJetName.replace('BTagging',''))

    logger.info("VR Btag name: %s" % VRJetBTagName)
    logger.info("VR jet name: %s" % VRJetRecToolName)

    from AthenaCommon.AppMgr import ToolSvc

    #make the btagging tool for VR jets
    from BTagging.BTaggingFlags import BTaggingFlags
    BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]
    BTaggingFlags.CalibrationChannelAliases += ["%s->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo" % (VRJetName)]
    btag_vrjets = ConfInst.setupJetBTaggerTool(
        ToolSvc, JetCollection=VRJetRecToolName, AddToToolSvc=True, Verbose=True,
        options={"name"         : VRJetBTagName.lower(),
                 "BTagName"     : VRJetBTagName,
                 "BTagJFVtxName": "JFVtx",
                 "BTagSVName"   : "SecVtx",
        },
        SetupScheme = "",
        TaggerList = BTaggingFlags.ExpertTaggers if doFlipTagger else BTaggingFlags.StandardTaggers,
        TrackAssociatorName="GhostTrack" if do_ghost else "MatchedTracks",
    )

    # add Ghost label id
    from ParticleJetTools.ParticleJetToolsConf import (
        ParticleJetGhostLabelTool as GhostLabelTool)
    gl_tool = GhostLabelTool(
        name=VRJetRecToolName + "_GhostLabeling")
    ToolSvc += gl_tool

    from BTagging.BTaggingConfiguration import defaultTrackAssoc, defaultMuonAssoc

    # Slice the array - this forces a copy so that if we modify it we don't also
    # change the array in jtm.
    pseudoJetGetters = jtm.gettersMap[VRJetInputs][:]

    # We want to include ghost associated tracks in the pv0 tracks so that
    # we can use the looser ghost association criteria for b-tagging.
    if VRJetInputs == "pv0track":
        pseudoJetGetters.append(jtm["gtrackget"])

    if VRJetAlgName in DFJetAlgs:
        logger.info("Algorithm %s already built before" % VRJetAlgName)

        if hasattr(sequence, VRJetAlgName):
            logger.info("Sequence %s already has an instance of algorithm %s" % (sequence, VRJetAlgName))
        else:
            logger.info("Add algorithm %s to sequence %s" % (VRJetAlgName, sequence))
            sequence += DFJetAlgs[VRJetAlgName]
    else:
        logger.info("Create algorithm %s" % VRJetAlgName)

        if hasattr(jtm, VRJetRecToolName):
            logger.info("JetRecTool %s is alredy in jtm.tools in sequence %s" % (VRJetRecToolName, sequence))
        else:
            logger.info("Create JetRecTool %s" % VRJetRecToolName)
            #can only run trackjetdrlabeler with truth labels, so MC only

            mods = [defaultTrackAssoc, defaultMuonAssoc, btag_vrjets]

            if isMC:
                mods += [jtm.trackjetdrlabeler, gl_tool]

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
    from DerivationFrameworkJetEtMiss.ExtendedJetCommon import nameJetsFromAlg

    if hasattr(jtm, pjgettername):
        logger.info("Found %s in jtm in sequence %s" % (pjgettername, sequence))
    else:
        logger.info("Add %s to jtm in sequence %s" % (pjgettername, sequence))
        
        inputContainerName = jetFlags.containerNamePrefix() + nameJetsFromAlg(VRJetName)


        from JetRec.JetRecConf import PseudoJetGetter
        jtm += PseudoJetGetter(
          pjgettername,                                                          # give a unique name
          InputContainer = inputContainerName,                                   # SG key
          Label = VRGhostLabel,                                                  # this is the name you'll use to retrieve ghost associated VR track jets
          OutputContainer = "PseudoJet" + VRGhostLabel,
          SkipNegativeEnergy = True,
          GhostScale = 1.e-20,                                                   # this makes the PseudoJet Ghosts, and thus the reco flow will treat them as such
        )
    return VRJetName, VRGhostLabel

##################################################################
# Build variable-R calorimeter jets
##################################################################
def addVRCaloJets(sequence,outputlist,dotruth=True,writeUngroomed=False):
    if isMC and dotruth:
        addTrimmedJets('AntiKt', 1.0, 'Truth', rclus=0.2, ptfrac=0.05, variableRMassScale=600000, variableRMinRadius=0.2, mods="truth_groomed",
                       algseq=sequence, outputGroup=outputlist, writeUngroomed=writeUngroomed)
    addTrimmedJets('AntiKt', 1.0, 'PV0Track', rclus=0.2, ptfrac=0.05, variableRMassScale=600000, variableRMinRadius=0.2, mods="groomed",
                   algseq=sequence, outputGroup=outputlist, writeUngroomed=writeUngroomed)
    addTrimmedJets('AntiKt', 1.0, 'LCTopo', rclus=0.2, ptfrac=0.05, variableRMassScale=600000, variableRMinRadius=0.2, mods="lctopo_groomed",
                   algseq=sequence, outputGroup=outputlist, writeUngroomed=writeUngroomed)

###################################################################
## Utils: get jetRectoTool
###################################################################
def getJetRecTool(collection, getParent=True):
  """Get the JetRecTool for a given collection from the jtm. If getParent is
     set then if that tool has an InputContainer property set then try and 
     retrieve the JetRecTool for that parent, continue going until there is no
     InputContainer property. Will raise a KeyError if no JetRecTool can be
     found at any stage.
  """
  from JetRec.JetRecStandardToolManager import jtm
  try:
    jetRecTool = jtm[collection]
  except KeyError as e:
     raise KeyError("JetRecTool {0} not present in jtm".format(collection) )
  if getParent and hasattr(jetRecTool, "InputContainer") and jetRecTool.InputContainer:
    jetRecTool = getJetRecTool(jetRecTool.InputContainer, True)
  return jetRecTool

###################################################################
## Utils: link jets (copied over from Jon Burr in EXOT27Utils)
###################################################################
def linkVRJetsToLargeRJets(
    sequence, collection, getters):
  """Re-run jet finding for a jet collection using a new list of 
    PseudoJetGetters. These PseudoJetGetters should already have been loaded
    into jtm.
    collection should be the name of the jet collection, which should already
    have been sequenced, so it's jet rec tool will exist in jtm.
    getters should be a map of PseudoJetGetters, each key being the name of the
    (non ghost) collection, with the value being the name of the 
    PseudoJetGetter in jtm.
    Returns the name of the ungroomed collection that is the parent of
    'collection' (this will be the same as 'collection' if this isn't groomed)
    *and* the list of ghost labels (these are the element link names).
  """
  from JetRec.JetRecStandardToolManager import jtm
  import DerivationFrameworkJetEtMiss.JetCommon as JetCommon
  from DerivationFrameworkJetEtMiss.ExtendedJetCommon import nameJetsFromAlg
  logger = Logging.logging.getLogger('HbbTaggerLog')
  # First, retrieve the original JetRecTool - this is the one that made the
  # *ungroomed* jets, not the groomed ones. Ghost association is done to
  # ungroomed objects
  originalJetRecTool = getJetRecTool(collection, getParent = True)
  originalUngroomedName = originalJetRecTool.name()
  ungroomedJetAlg = originalUngroomedName
  if ungroomedJetAlg.endswith("Jets"):
    ungroomedJetAlg = ungroomedJetAlg[:-4]
  originalFinder = jtm[originalJetRecTool.JetFinder.getName()]
  originalGetters = [jtm[g.getName()] for g in originalJetRecTool.PseudoJetGetters]
  newGetters = [jtm[g] for g in getters.values()]

  # Next, prepare the names of the new objects we'll need from jtm

  comb_name = "_".join(getters.keys() )
  LargeRJetFindingAlg = "jfind_{0}_{1}".format(collection, comb_name).lower()
  LargeRJetPrefix     = "{0}_{1}".format(collection, comb_name)
  LargeRJets = nameJetsFromAlg(LargeRJetPrefix)
  LinkTransferAlg     = "LinkTransfer_{0}_{1}".format(collection, comb_name)

  # Check to see if this large R jet collection is already known to JetCommon
  if LargeRJetFindingAlg in JetCommon.DFJetAlgs:
    logger.info("Found {0} in DFJetAlgs".format(LargeRJetFindingAlg) )
    # Is it in our sequence?
    if hasattr(sequence, LargeRJetFindingAlg):
      logger.info("Algorithm already exists in the input sequence. Will not "
          "add again")
    else:
      logger.info("Adding algorithm into the sequence {0}".format(sequence) )
      sequence += JetCommon.DFJetAlgs[LargeRJetFindingAlg]
  else:
    # Check to see if the corresponding JetRecTool already exists
    if hasattr(jtm, LargeRJets):
      logger.info("JetRecTool {0} already exists in jtm".format(LargeRJets) )
    else:
      logger.info("Create a new JetRecTool {0}".format(LargeRJets) )
      JetCommon.OutputJets.setdefault("CustomJets", []).append(LargeRJets)
      originalModifiers = [jtm[m.getName()] for m in originalJetRecTool.JetModifiers]
      jtm.addJetFinder(
          output = LargeRJets,
          alg = originalFinder.JetAlgorithm,
          radius = originalFinder.JetRadius,
          gettersin = originalGetters + newGetters,
          modifiersin = originalModifiers,
          ghostArea = 0,
          ptmin = originalFinder.PtMin,
          variableRMinRadius = originalFinder.VariableRMinRadius,
          variableRMassScale = originalFinder.VariableRMassScale,
          calibOpt = "none")
      # Note that we don't need ptminFilter as this was included in the original
      # list of JetModifiers
    logger.info(
        "Creating new jet algorithm {0} and adding it to sequence {1}".format(
          LargeRJetFindingAlg, sequence) )
    theJetAlg = JetAlgorithm(LargeRJetFindingAlg, Tools = [jtm[LargeRJets] ])
    sequence += theJetAlg
    JetCommon.DFJetAlgs[LargeRJetFindingAlg] = theJetAlg

  # Everything so far has been to create the links on a copy of the ungroomed
  # collection. Now we need to copy those links over to the original ungroomed
  # collection.
  from DerivationFrameworkJetEtMiss.ExtendedJetCommon import getJetExternalAssocTool, applyJetAugmentation
  assocTool = getJetExternalAssocTool(
      ungroomedJetAlg,
      LargeRJetPrefix,
      MomentPrefix = '',
      ListOfOldLinkNames=[g.Label for g in newGetters]
      )
  applyJetAugmentation(
      ungroomedJetAlg, LinkTransferAlg, sequence, assocTool)

  return originalUngroomedName, [g.Label for g in newGetters]



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
    is_data = not isMC
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
def addExKtDoubleTaggerRCJets(sequence, ToolSvc):
   jetToolName = "DFReclustertingTool"
   algoName = "DFJetReclusteringAlgo"

   ExKtJetCollection__FatJet = "AntiKt8EMPFlowJets"
   ExKtJetCollection__SubJet = []

   if jetToolName not in DFJetAlgs:
     ToolSvc += CfgMgr.JetReclusteringTool(jetToolName,InputJetContainer="AntiKt4EMPFlowJets", OutputJetContainer="AntiKt8EMPFlowJets")
     getattr(ToolSvc,jetToolName).ReclusterRadius = 0.8
     getattr(ToolSvc,jetToolName).InputJetPtMin = 20
     getattr(ToolSvc,jetToolName).RCJetPtMin = 1
     getattr(ToolSvc,jetToolName).TrimPtFrac = 0
     getattr(ToolSvc,jetToolName).DoArea = False
     getattr(ToolSvc,jetToolName).GhostTracksInputContainer = "InDetTrackParticles"
     getattr(ToolSvc,jetToolName).GhostTracksVertexAssociationName  = "JetTrackVtxAssoc"
     if isMC:
       getattr(ToolSvc,jetToolName).GhostTruthBHadronsInputContainer = "BHadronsFinal"
       getattr(ToolSvc,jetToolName).GhostTruthCHadronsInputContainer = "CHadronsFinal"

     sequence += CfgMgr.AthJetReclusteringAlgo(algoName, JetReclusteringTool = getattr(ToolSvc,jetToolName))
     DFJetAlgs[jetToolName] = getattr(ToolSvc,jetToolName)

   # build subjets
   GhostLabels = ["GhostTrack"]
   if isMC:
     GhostLabels += ["GhostBHadronsFinal"]
     GhostLabels += ["GhostCHadronsFinal"]
   # N=2 subjets
   ExKtJetCollection__SubJet += addExKtCoM(sequence, ToolSvc, ExKtJetCollection__FatJet, nSubjets=2, doTrackSubJet=True, ExGhostLabels=GhostLabels, min_subjet_pt_mev=1)
   # N=3 subjets
   ExKtJetCollection__SubJet += addExKtCoM(sequence, ToolSvc, ExKtJetCollection__FatJet, nSubjets=3, doTrackSubJet=True, ExGhostLabels=GhostLabels, min_subjet_pt_mev=1)
   #Ghosttracks
   # N=2 subjets
   ExKtJetCollection__SubJet += addExKtCoM(sequence, ToolSvc, ExKtJetCollection__FatJet, nSubjets=2, doTrackSubJet=True, doGhostAssoc=True, ExGhostLabels=GhostLabels, min_subjet_pt_mev=1)
   # N=3 subjets
   ExKtJetCollection__SubJet += addExKtCoM(sequence, ToolSvc, ExKtJetCollection__FatJet, nSubjets=3, doTrackSubJet=True, doGhostAssoc=True, ExGhostLabels=GhostLabels, min_subjet_pt_mev=1)

   sequence += CfgMgr.xAODMaker__ElementLinkResetAlg("ELReset_AfterSubjetBuild", SGKeys=[name+"Aux." for name in ExKtJetCollection__SubJet])

   from BTagging.BTaggingFlags import BTaggingFlags
   BTaggingFlags.CalibrationChannelAliases += [ jetname[:-4].replace("PV0", "")+"->AntiKt4EMTopo" for jetname in ExKtJetCollection__FatJet ]
   BTaggingFlags.CalibrationChannelAliases += [ jetname[:-4].replace("PV0", "")+"->AntiKt4EMTopo" for jetname in ExKtJetCollection__SubJet ]

   sequence += CfgMgr.xAODMaker__ElementLinkResetAlg("ELReset_AfterBtag", SGKeys=[name+"Aux." for name in ExKtJetCollection__SubJet])
