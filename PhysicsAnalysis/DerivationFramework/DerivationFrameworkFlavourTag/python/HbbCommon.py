# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# import Common Algs
from DerivationFrameworkJetEtMiss.JetCommon import DFJetAlgs

from DerivationFrameworkCore.DerivationFrameworkMaster import (
    DerivationFrameworkIsMonteCarlo as isMC)

# I wish we didn't need this
from BTagging.BTaggingConfiguration import getConfiguration
ConfInst=getConfiguration()

from AthenaCommon import CfgMgr

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
  except KeyError:
     raise KeyError("JetRecTool {0} not present in jtm".format(collection) )
  if getParent and hasattr(jetRecTool, "InputContainer") and jetRecTool.InputContainer:
    jetRecTool = getJetRecTool(jetRecTool.InputContainer, True)
  return jetRecTool


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

   sequence += CfgMgr.xAODMaker__ElementLinkResetAlg("ELReset_AfterBtag", SGKeys=[name+"Aux." for name in ExKtJetCollection__SubJet])

