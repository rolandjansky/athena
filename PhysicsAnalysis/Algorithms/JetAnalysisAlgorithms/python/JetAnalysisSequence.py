# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool, \
                                       createPublicTool

def makeJetAnalysisSequence( dataType, jetCollection, runJvtUpdate = True,
                             runJvtEfficiency = True, runJvtSelection = False,
                             runGhostMuonAssociation = True, deepCopyOutput = False ):
    """Create a jet analysis algorithm sequence

    Keyword arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
      jetCollection -- The jet container to run on
      runGhostMuonAssociation -- Determines wheter or not to run the ghost muon association needed for MET
      runJvtUpdate -- Determines whether or not to update JVT on the jets
      runJvtEfficiency -- Determines whether or not to recalculate the JVT
                          efficiency
      runJvtSelection -- Determines whether or not to run the JVT selection
    """

    if not dataType in ["data", "mc", "afii"] :
        raise ValueError ("invalid data type: " + dataType)

    # Create the analysis algorithm sequence object:
    seq = AnaAlgSequence( "JetAnalysisSequence" )

    # Set up the jet calibration algorithm:
    if dataType == 'afii':
        configFile = 'JES_MC16Recommendation_AFII_EMTopo_April2018_rel21.config'
    else:
        configFile = 'JES_data2017_2016_2015_Consolidated_EMTopo_2018_Rel21.config'
        pass
    if dataType == 'data':
        calibSeq = 'JetArea_Residual_EtaJES_GSC_Insitu'
    elif dataType == 'afii':
        calibSeq = 'JetArea_Residual_EtaJES_GSC'
    else:
        calibSeq = 'JetArea_Residual_EtaJES_GSC_Smear'
        pass

    # Set up the jet ghost muon association algorithm:
    if runGhostMuonAssociation:
        alg = createAlgorithm( 'CP::JetGhostMuonAssociationAlg', 'JetGhostMuonAssociationAlg' )
        seq.append( alg, inputPropName = 'jets', outputPropName = 'jetsOut' )

    # Set up the jet calibration algorithm:
    alg = createAlgorithm( 'CP::JetCalibrationAlg', 'JetCalibrationAlg' )
    addPrivateTool( alg, 'calibrationTool', 'JetCalibrationTool' )
    alg.calibrationTool.JetCollection = jetCollection[ 0 : -4 ]
    alg.calibrationTool.ConfigFile = configFile
    alg.calibrationTool.CalibSequence = calibSeq
    if dataType == 'data':
        alg.calibrationTool.IsData = 1
    else :
        alg.calibrationTool.IsData = 0
        pass
    seq.append( alg, inputPropName = 'jets', outputPropName = 'jetsOut' )

    # Set up the jet uncertainty calculation algorithm:
    alg = createAlgorithm( 'CP::JetUncertaintiesAlg', 'JetUncertaintiesAlg' )
    addPrivateTool( alg, 'uncertaintiesTool', 'JetUncertaintiesTool' )
    alg.uncertaintiesTool.JetDefinition = jetCollection[ 0 : -4 ]
    alg.uncertaintiesTool.ConfigFile = \
        'rel21/Fall2018/R4_GlobalReduction_SimpleJER.config'
    if dataType == 'afii':
        alg.uncertaintiesTool.MCType = "AFII"
    else:
        alg.uncertaintiesTool.MCType = "MC16"
        pass
    seq.append( alg, inputPropName = 'jets', outputPropName = 'jetsOut',
                affectingSystematics = '(^JET_RelativeNonClosure.*)|(^JET_EffectiveNP.*)|(^JET_EtaIntercalibration.*)|(^JET_BJES.*)|(^JET_Pileup.*)|(^JET_Flavor.*)|(^JET_PunchThrough.*)|(^JET_SingleParticle.*)|(^JET_JER.*)' )

    # Define a list of cuts to apply later on and the
    # number of bits in the corresponding TAccept
    cutlist = []
    cutlength = []

    # Set up the JVT update algorithm:
    if runJvtUpdate :
        alg = createAlgorithm( 'CP::JvtUpdateAlg', 'JvtUpdateAlg' )
        addPrivateTool( alg, 'jvtTool', 'JetVertexTaggerTool' )
        seq.append( alg, inputPropName = 'jets', outputPropName = 'jetsOut' )

        alg = createAlgorithm( 'CP::JetModifierAlg', 'JetModifierAlg' )
        addPrivateTool( alg, 'modifierTool', 'JetForwardJvtTool')
        alg.modifierTool.OutputDec = "passFJVT" #Output decoration
        # fJVT WPs depend on the MET WP
        # see https://twiki.cern.ch/twiki/bin/view/AtlasProtected/EtmissRecommendationsRel21p2#fJVT_and_MET
        alg.modifierTool.UseTightOP = 1 # 1 = Tight, 0 = Loose
        alg.modifierTool.EtaThresh = 2.5 # Eta dividing central from forward jets
        alg.modifierTool.ForwardMaxPt = 120.0e3 #Max Pt to define fwdJets for JVT
        seq.append( alg, inputPropName = 'jets', outputPropName = 'jetsOut' )
        pass

    # Set up the jet efficiency scale factor calculation algorithm
    # Change the truthJetCollection property to AntiKt4TruthWZJets if preferred
    if runJvtEfficiency:
        alg = createAlgorithm( 'CP::JvtEfficiencyAlg', 'JvtEfficiencyAlg' )
        addPrivateTool( alg, 'efficiencyTool', 'CP::JetJvtEfficiency' )
        alg.selection = 'jvt_selection' if runJvtSelection else 'jvt_selection,as_char'
        alg.efficiency = 'jvt_efficiency'
        # Disable efficiency decorations if running on data
        # We still want to run the JVT selection
        if dataType == 'data':
            alg.efficiency = ''
            alg.truthJetCollection = ''
        alg.outOfValidity = 2
        alg.outOfValidityDeco = 'no_jvt'
        alg.skipBadEfficiency = 0
        seq.append( alg, inputPropName = 'jets', outputPropName = 'jetsOut',
                    affectingSystematics = '(^JET_JvtEfficiency$)|(^JET_fJvtEfficiency$)' )
        if runJvtSelection:
            cutlist.append('jvt_selection')
            cutlength.append(1)
        pass

    # Set up an algorithm used for debugging the jet selection:
    alg = createAlgorithm( 'CP::ObjectCutFlowHistAlg', 'JetCutFlowDumperAlg' )
    alg.histPattern = 'jet_cflow_%SYS%'
    alg.selection = cutlist
    alg.selectionNCuts = cutlength
    seq.append( alg, inputPropName = 'input' )

    # Set up an algorithm that makes a view container using the selections
    # performed previously:
    alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg', 'JetViewFromSelectionAlg' )
    alg.selection = cutlist
    seq.append( alg, inputPropName = 'input', outputPropName = 'output' )

    # Set up an algorithm dumping the properties of the jets, for debugging:
    alg = createAlgorithm( 'CP::KinematicHistAlg', 'JetKinematicDumperAlg' )
    alg.histPattern = 'jet_%VAR%_%SYS%'
    seq.append( alg, inputPropName = 'input' )

    # Set up a final deep copy making algorithm if requested:
    if deepCopyOutput:
        alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg',
                               'JetDeepCopyMaker' )
        alg.deepCopy = True
        seq.append( alg, inputPropName = 'input', outputPropName = 'output',
                    stageName = 'selection' )
        pass
    

    # Return the sequence:
    return seq
