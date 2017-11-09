# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from JetRec.JetRecConf import JetAlgorithm

#===================================================================
# ExKt/CoM Subjets
#===================================================================

# make exkt subjet finding tool
def buildExclusiveSubjets(ToolSvc, JetCollectionName, subjet_mode, nsubjet, doTrackSubJet, ExGhostLabels=None):
    from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import SubjetFinderTool
    from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import SubjetRecorderTool

    if ExGhostLabels == None:
      if subjet_mode == "CoM":
        ExGhostLabels = []
      else:
        ExGhostLabels = ["GhostBHadronsFinal", "GhostBHadronsInitial", "GhostBQuarksFinal", "GhostCHadronsFinal", "GhostCHadronsInitial", "GhostCQuarksFinal", "GhostHBosons", "GhostPartons", "GhostTQuarksFinal", "GhostTausFinal", "GhostTruth"]
        if "Track" not in JetCollectionName:
          ExGhostLabels += ["GhostTrack"]

    SubjetContainerName = "%sEx%s%iSubJets" % (JetCollectionName.replace("Jets", ""), subjet_mode, nsubjet)

    subjetrecorder = SubjetRecorderTool("subjetrecorder_%s%i_%s" % (subjet_mode, nsubjet, JetCollectionName))
    ToolSvc += subjetrecorder

    subjetlabel = "Ex%s%iSubJets" % (subjet_mode, nsubjet)

    doCoM = False
    algj = "Kt"
    if subjet_mode == "CoM" :
      doCoM = True
#      algj = "cambridge" # or CamKt if "cambridge" doesn't work
      algj = "FastJetPlugin" # for EECambridge plugin  #and EEKt or ee_kt

    subjetrecorder.SubjetLabel = subjetlabel
    subjetrecorder.SubjetContainerName = SubjetContainerName

    if not hasattr(ToolSvc, "ExKtTrackSelectorLoose"):
      from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
      ToolSvc += InDet__InDetTrackSelectionTool("ExKtTrackSelectorLoose",
                                                CutLevel = "Loose",
                                               )

    from JetTagTools.JetTagToolsConf import Analysis__ExKtbbTagTool
    ExKtbbTagToolInstance = Analysis__ExKtbbTagTool(
      name = "Ex%sbbTagTool%i_%s" % (subjet_mode, nsubjet, JetCollectionName),
      JetAlgorithm = algj,
      JetRadius = 10.0,
      PtMin = 1000,
      ExclusiveNJets = 2,
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
    ToolSvc += ExKtbbTagToolInstance

    return (ExKtbbTagToolInstance, SubjetContainerName)

#===================================================================
# Build ExKt Subjets
#===================================================================
def addExKt(FTAG5Seq, ToolSvc, ExKtJetCollection__FatJet, doTrackSubJet):
    ExKtJetCollection__SubJet = []
    for JetCollectionExKt in ExKtJetCollection__FatJet:
        # build ExKtbbTagTool instance
        (ExKtbbTagToolInstance, SubjetContainerName) = buildExclusiveSubjets(ToolSvc, JetCollectionExKt, "Kt", 2, doTrackSubJet)
        ExKtJetCollection__SubJet += [SubjetContainerName]

        # build subjet collection through JetRecTool
        from JetRec.JetRecConf import JetRecTool
        jetrec = JetRecTool(
                             name = "JetRecTool_ExKtbb_%s" % (JetCollectionExKt),
                             OutputContainer = JetCollectionExKt,
                             InputContainer = JetCollectionExKt,
                             JetModifiers = [ExKtbbTagToolInstance],
                           )

        ToolSvc += jetrec
        FTAG5Seq += JetAlgorithm(
                                 name = "JetAlgorithm_ExKtbb_%s" % (JetCollectionExKt),
                                 Tools = [jetrec],
                                )
    return ExKtJetCollection__SubJet

#===================================================================
# Build CoM Subjets
#===================================================================
def addExCoM(FTAG5Seq, ToolSvc, ExKtJetCollection__FatJet, doTrackSubJet):
    ExCoMJetCollection__SubJet = []
    for JetCollectionExCoM in ExKtJetCollection__FatJet:
        (ExCoMbbTagToolInstance, SubjetContainerName) = buildExclusiveSubjets(ToolSvc, JetCollectionExCoM, "CoM", 2, doTrackSubJet)
        ExCoMJetCollection__SubJet += [SubjetContainerName]

        from JetRec.JetRecConf import JetRecTool
        jetrec = JetRecTool(
                             name = "JetRecTool_ExCoMbb_%s" % (JetCollectionExCoM),
                             OutputContainer = JetCollectionExCoM,
                             InputContainer = JetCollectionExCoM,
                             JetModifiers = [ExCoMbbTagToolInstance],
                           )

        ToolSvc += jetrec
        FTAG5Seq += JetAlgorithm(
                                 name = "JetAlgorithm_ExCoMbb_%s" % (JetCollectionExCoM),
                                 Tools = [jetrec],
                                )
    return ExCoMJetCollection__SubJet

##################################################################
# Build variable-R subjets, recluster AntiKt10LCTopojet with ghost VR and copy ghost link to AntiKt10LCTopo 
##################################################################
def addVRJets(sequence, VRJetName, VRGhostLabel, VRJetAlg="AntiKt", VRJetRadius=0.4, VRJetInputs="pv0track", **VRJetOptions):
    from JetRec.JetRecStandard import jtm

    #==========================================================
    # Build VR jets
    #==========================================================

    VRJetAlgName = "jfind_%sJets" % (VRJetName)
    VRJetRecToolName = "%sJets" % (VRJetName)
    VRJetBTagName = "BTagging_%s" % (VRJetName)

    #make the btagging tool for VR jets
    btag_vrjets = ConfInst.setupJetBTaggerTool(ToolSvc, JetCollection=VRJetRecToolName, AddToToolSvc=True, Verbose=True,
                 options={"name"         : VRJetBTagName.lower(), 
                          "BTagName"     : VRJetBTagName, 
                          "BTagJFVtxName": "JFVtx",                  
                          "BTagSVName"   : "SecVtx",                 
                          },
                 SetupScheme = "",
                 TaggerList = ['IP2D', 'IP3D', 'MultiSVbb1',  'MultiSVbb2', 'SV1', 'JetFitterNN', 'SoftMu', 
                               'MV2c10', 'MV2c10mu', 'MV2c10rnn', 'JetVertexCharge', 'MV2cl100' , 'MVb', 'DL1', 'DL1rnn', 'DL1mu', 'RNNIP', 'MV2c10Flip']
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
                jtm.addJetFinder(VRJetRecToolName, VRJetAlg, VRJetRadius, VRJetInputs, modifiersin=[btag_vrjets,jtm.trackjetdrlabeler], **VRJetOptions) 
            else:
                jtm.addJetFinder(VRJetRecToolName, VRJetAlg, VRJetRadius, VRJetInputs, modifiersin=[btag_vrjets], **VRJetOptions)

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
    # AntiKt10LCTopo hard-coded for now
    #==========================================================
    LargeRJetAlg     = "jfind_akt10lctopo_%s" %(VRJetName.lower())
    LargeRJets       = "AKt10LCTopo_%sJets"   %(VRJetName)
    LargeRJetPrefix  = "AKt10LCTopo_%s"       %(VRJetName)
    newLCTopo        = "lctopo_%s"            %(VRJetName.lower())
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
            jtm.gettersMap[newLCTopo] = list(jtm.gettersMap["lctopo"])
            jtm.gettersMap[newLCTopo] += [ jtm[pjgettername] ] 
            jtm.addJetFinder(LargeRJets, "AntiKt", 1.0, newLCTopo , "lctopo_ungroomed",
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

    jetassoctool = getJetExternalAssocTool('AntiKt10LCTopo', LargeRJetPrefix, MomentPrefix='', ListOfOldLinkNames=[VRGhostLabel])
    applyJetAugmentation('AntiKt10LCTopo', LinkTransferAlg, sequence, jetassoctool)

##################################################################
# Build variable-R subjets, recluster AntiKt10TCCjet with ghost VR and copy ghost link to AntiKt10TCC - Copy and pasting this defintion is an ugly temporary solution. We need to properly rewrite this for a more general use and remove the hardcoded jet collection. 
##################################################################
def addVRJetsTCC(sequence, VRJetName, VRGhostLabel, VRJetAlg="AntiKt", VRJetRadius=0.4, VRJetInputs="pv0track", **VRJetOptions):
    from JetRec.JetRecStandard import jtm

    #==========================================================
    # Build VR jets
    #==========================================================

    VRJetAlgName = "jfind_%sJets" % (VRJetName)
    VRJetRecToolName = "%sJets" % (VRJetName)
    VRJetBTagName = "BTagging_%s" % (VRJetName)

    #make the btagging tool for VR jets
    btag_vrjets = ConfInst.setupJetBTaggerTool(ToolSvc, JetCollection=VRJetRecToolName, AddToToolSvc=True, Verbose=True,
                 options={"name"         : VRJetBTagName.lower(), 
                          "BTagName"     : VRJetBTagName, 
                          "BTagJFVtxName": "JFVtx",                  
                          "BTagSVName"   : "SecVtx",                 
                          },
                 SetupScheme = "",
                 TaggerList = ['IP2D', 'IP3D', 'MultiSVbb1',  'MultiSVbb2', 'SV1', 'JetFitterNN', 'SoftMu', 
                               'MV2c10', 'MV2c10mu', 'MV2c10rnn', 'JetVertexCharge', 'MV2cl100' , 'MVb', 'DL1', 'DL1rnn', 'DL1mu', 'RNNIP', 'MV2c10Flip']
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
                jtm.addJetFinder(VRJetRecToolName, VRJetAlg, VRJetRadius, VRJetInputs, modifiersin=[btag_vrjets,jtm.trackjetdrlabeler], **VRJetOptions) 
            else:
                jtm.addJetFinder(VRJetRecToolName, VRJetAlg, VRJetRadius, VRJetInputs, modifiersin=[btag_vrjets], **VRJetOptions)

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

    jetassoctool = getJetExternalAssocTool('AntiKt10TrackCaloCluster', LargeRJetPrefix, MomentPrefix='', ListOfOldLinkNames=[VRGhostLabel])
    applyJetAugmentation('AntiKt10TrackCaloCluster', LinkTransferAlg, sequence, jetassoctool)
    
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
