# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool, \
                                       createPublicTool
from PathResolver.PathResolver import FindCalibFile
import ROOT
import re

def getFullListOfSystematics(configFile, mcType, jetDefinition):
  """Get the list of systematics defined in a configuration file.

    Keyword arguments:
      configFile -- The full path to the configuration file
      mcType -- The type of MC being used
      jetDefinition -- The jet definition being used. This is only used to make sure the config file is valid.
  """
  # Get the ROOT TEnv
  env = ROOT.TEnv(configFile)
  # Check the mcType and jetDefinition are supported
  supportedJetDefs = env.Lookup("SupportedJetDefs")
  if supportedJetDefs:
    # Case-blind comparison, stripping out any spaces to make sure that they
    # don't interfere
    if jetDefinition.lower() not in [
      value.strip().lower() for value in supportedJetDefs.GetValue().split(',')]:
      raise ValueError( (
          "Requested jet definition {0} not supported by config {1}! " +
          "Supported definitions are {2}").format(
            jetDefinition, configFile, supportedJetDefs.GetValue() ) )
  supportedMCs = env.Lookup("SupportedMCTypes")
  if supportedMCs:
    # Case-blind comparison, stripping out any spaces to make sure that they
    # don't interfere
    supportedValues = [v.strip() for v in supportedMCs.GetValue().split(',')]
    try:
      idx = [v.lower() for v in supportedValues].index(mcType.lower() )
    except ValueError:
      raise ValueError( (
          "Requested mc type {0} not supported by config {1}! " +
          "Supported types are {2}").format(
            mcType, configFile, supportedValues) )
    # make sure that mcType is in the right format for the systematic names
    mcType = supportedValues[idx]


  # To iterate through the TEnv we can iterate through its hash table
  names = []
  for resource in env.GetTable():
    # We want to extract the names of JESComponents/JESGroups which look like
    # JESComponent|JESGroup.number.Name
    split = resource.GetName().split(".")
    if len(split) != 3 or \
       split[0] not in ["JESComponent", "JESGroup"] or \
       not split[1].isdigit() or \
       split[2] != "Name":
      continue

    # unpack the list
    resType, number, prop = split
    # See if this is part of a group
    group = env.GetValue("{0}.{1}.Group".format(resType, number), 0)
    if group == 0 or (resType == "JESGroup" and group == int(number) ):
      # The group field either isn't present, is set to 0 (which counts as null
      # here) or this is a JESGroup whose group number is set to be itself.
      # In all of these cases this is one of the affected systematics
      names.append(resource.GetValue() )

  # Now we need to return them in the right format
  return ["JET_{0}__continuous".format(name.replace("MCTYPE", mcType) ) for name in names]


def makeJetAnalysisSequence( dataType, jetCollection, runJvtUpdate = True,
                             runJvtEfficiency = True, runJvtSelection = False,
                             runGhostMuonAssociation = True, deepCopyOutput = False,
                             largeRMass = "Comb", namePrefix="", nameSuffix="",
                             reduction = "Global", JEROption = "Simple", variablesToShift=[] ):
    """Create a jet analysis algorithm sequence

    Keyword arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
      jetCollection -- The jet container to run on
      runGhostMuonAssociation -- Determines wheter or not to run the ghost muon association needed for MET
      runJvtUpdate -- Determines whether or not to update JVT on the jets
      runJvtEfficiency -- Determines whether or not to recalculate the JVT
                          efficiency
      runJvtSelection -- Determines whether or not to run the JVT selection
      largeRMass -- Which large-R mass definition to use. Ignored if not running on large-R jets ("Comb", "Calo", "TA")
      namePrefix -- Prefix to be added to the start of all public configurable names
      nameSuffix -- Suffix to be added to the end of all public configurable names
      reduction -- Which NP reduction scheme should be used (All, Global, Category, Scenario)
      JEROption -- Which variant of the reduction should be used (All, Full, Simple). Note that not all combinations of reduction and JEROption are valid!
      variablesToShift -- The subset of large-R jet variables to shift. If left empty all of them will be used.
    """

    if not dataType in ["data", "mc", "afii"] :
        raise ValueError ("invalid data type: " + dataType)

    # interpret the jet collection
    collection_pattern = re.compile(
        r"AntiKt(\d+)(EMTopo|EMPFlow|LCTopo|TrackCaloCluster)(TrimmedPtFrac5SmallR20)?Jets")
    match = collection_pattern.match(jetCollection)
    if not match:
      raise ValueError(
          "Jet collection {0} does not match expected pattern!".format(jetCollection) )
    radius = int(match.group(1) )
    if radius not in [2, 4, 6, 10]:
      raise ValueError("Jet collection has an unsupported radius '{0}'!".format(radius) )
    jet_input = match.group(2)
    trim = match.group(3)

    if largeRMass not in ["Comb", "Calo", "TA"]:
      raise ValueError ("Invalid large-R mass defintion {0}!".format(largeRMass) )

    # Create the analysis algorithm sequence object:
    seq = AnaAlgSequence( namePrefix+"JetAnalysisSequence"+nameSuffix )

    # Set up the jet calibration algorithm:
    # Set isData first, given that some of the calibrations require this to
    # always be set to false but most are what you would expect.
    isDataParam = 1 if dataType == 'data' else 0
    # Assemble the calibration tool properties, these differ mainly based on the
    # radius of the jet so split on that
    if radius == 4:
      # get the right string to use in the config files.
      if jet_input == "EMTopo":
        config_input = "EMTopo"
      elif jet_input == "EMPFlow":
        config_input = "PFlow"
      else:
        raise ValueError(
          "Unsupported input type '{0}' for R=0.4 jets!".format(jet_input) )
      if dataType == 'afii':
        configFile = "JES_MC16Recommendation_AFII_{0}_April2018_rel21.config"
      else:
        configFile = "JES_data2017_2016_2015_Consolidated_{0}_2018_Rel21.config"
      configFile = configFile.format(config_input)

      if dataType == 'data':
          calibSeq = 'JetArea_Residual_EtaJES_GSC_Insitu'
      elif dataType == 'afii':
          calibSeq = 'JetArea_Residual_EtaJES_GSC'
      else:
          calibSeq = 'JetArea_Residual_EtaJES_GSC_Smear'
          pass
    elif radius in [2, 6]:
      # R-scan jets
      if jet_input != "LCTopo":
        raise ValueError(
            "Unsupported input type '{0}' for R-scan jets!".format(jet_input) )
      configFile = "JES_MC16Recommendation_Rscan{0}LC_22Feb2018_rel21.config".format(radius)
      calibSeq = "JetArea_Residual_EtaJES_GSC"
    else:
      # Large-R (=1.0) jets
      if jet_input not in ["LCTopo", "TrackCaloCluster"]:
        raise ValueError(
            "Unsupported input type '{0}' for large-R jets!".format(jet_input) )
      if jet_input == "TrackCaloCluster":
        # Only one mass definition supported
        if largeRMass != "calo":
          raise ValueError(
              "Unsupported large-R TCC jet mass '{0}'!".format(largeRMass) )
        configFile = "JES_MC16recommendation_FatJet_TCC_JMS_calo_30Oct2018.config"
      else:
        if largeRMass == "Comb":
          configFile = "JES_MC16recommendation_FatJet_Trimmed_JMS_comb_17Oct2018.config"
        elif largeRMass == "Calo":
          configFile = "JES_MC16recommendation_FatJet_Trimmed_JMS_calo_12Oct2018.config"
        else:
          configFile = "JES_MC16recommendation_FatJet_Trimmed_JMS_TA_12Oct2018.config"
      # All the large-R jets have the same calibSeq
      calibSeq = "EtaJES_JMS"
      # They are all calibrated as-if MC
      isDataParam = 0

    # Set up the jet ghost muon association algorithm:
    if runGhostMuonAssociation:
        alg = createAlgorithm( 'CP::JetGhostMuonAssociationAlg', 
            namePrefix+'JetGhostMuonAssociationAlg'+nameSuffix )
        seq.append( alg, inputPropName = 'jets', outputPropName = 'jetsOut' )

    # Set up the jet calibration algorithm:
    alg = createAlgorithm( 'CP::JetCalibrationAlg', 
        namePrefix+'JetCalibrationAlg'+nameSuffix )
    addPrivateTool( alg, 'calibrationTool', 'JetCalibrationTool' )
    alg.calibrationTool.JetCollection = jetCollection[ 0 : -4 ]
    alg.calibrationTool.ConfigFile = configFile
    alg.calibrationTool.CalibSequence = calibSeq
    alg.calibrationTool.IsData = isDataParam
    seq.append( alg, inputPropName = 'jets', outputPropName = 'jetsOut' )

    # Jet uncertainties
    if radius in [2, 6]:
      # TODO we need a central logging mechanism I think (i.e. set up the
      # relevant handlers etc.)
      print "WARNING: uncertainties for r-scan jets are not yet released!"
      pass
    elif jetCollection == "AntiKt4LCTopoJets":
      print "WARNING: uncertainties are not provided for AntiKt4LCTopoJets!"
      pass
    else:
      # Let's prepare the configuration
      if radius == 4:
        if reduction == "All" and JEROption == "All":
          configFile = "R4_AllNuisanceParameters_AllJERNP.config"
        elif "Scenario" in reduction:
          if JEROption != "Simple":
            raise ValueError(
                "Invalid uncertainty configuration - Scenario* reductions can "
                "only be used together with the Simple JEROption")
          configFile = "R4_{0}_SimpleJER.config".format(reduction)
        elif reduction in ["Global", "Category"] and JEROption in ["Simple", "Full"]:
          configFile = "R4_{0}Reduction_{1}JER.config".format(reduction, JEROption)
        else:
          raise ValueError(
              "Invalid combination of reduction and JEROption settings: "
              "reduction: {0}, JEROption: {1}".format(reduction, JEROption) )
        # add the correct directory on the front
        configFile = "rel21/Fall2018/"+configFile
        mcType = "AFII" if dataType == 'afii' else "MC16"
      else:
        # Large-R version
        configFile = "rel21/Moriond2018/R10_{0}Mass_all.config".format(largeRMass)
        mcType = "MC16a"
       
      # Set up the jet uncertainty calculation algorithm:
      alg = createAlgorithm( 'CP::JetUncertaintiesAlg', 
          namePrefix+'JetUncertaintiesAlg'+nameSuffix )
      addPrivateTool( alg, 'uncertaintiesTool', 'JetUncertaintiesTool' )
      alg.uncertaintiesTool.JetDefinition = jetCollection[ 0 : -4 ]
      alg.uncertaintiesTool.ConfigFile = configFile
      alg.uncertaintiesTool.MCType = mcType
      if radius == 10 and variablesToShift:
        alg.uncertaintiesTool.VariablesToShift = variablesToShift
      # Get the full list of systematics
      try:
        calibArea = alg.uncertaintiesTool.CalibArea
      except AttributeError:
        calibArea = alg.uncertaintiesTool.getDefaultProperty("CalibArea")
      fullPath = FindCalibFile("JetUncertainties/"+calibArea+"/"+configFile)
      if not fullPath:
        raise ValueError("Unable to locate config file {0}.".format(configFile) )
      affectedSysts = getFullListOfSystematics(fullPath, mcType, jetCollection[:-4])

      if radius == 10 and variablesToShift:
        # Only report systematics for the variables that we shift
        affectedSysts = [s for s in affectedSysts if any(
            s.endswith(v+"__continuous") for v in variablesToShift)]
      seq.append( alg, inputPropName = 'jets', outputPropName = 'jetsOut',
                  affectingSystematics = "^("+"|".join(affectedSysts)+")")

      

    # Define a list of cuts to apply later on and the
    # number of bits in the corresponding TAccept
    cutlist = []
    cutlength = []

    # The following pieces only make sense for R=0.4 jet collections. It might
    # be a good idea to make this function throw an error or warning if the JVT
    # is requested on a R!=0.4 jet but for now I'm not because some of the
    # defaults for the JVT arguments are True
    if radius == 4:
      # Set up the JVT update algorithm:
      if runJvtUpdate :
          alg = createAlgorithm( 'CP::JvtUpdateAlg', namePrefix+'JvtUpdateAlg'+nameSuffix )
          addPrivateTool( alg, 'jvtTool', 'JetVertexTaggerTool' )
          seq.append( alg, inputPropName = 'jets', outputPropName = 'jetsOut' )

          alg = createAlgorithm( 'CP::JetModifierAlg', namePrefix+'JetModifierAlg'+nameSuffix )
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
          alg = createAlgorithm( 'CP::JvtEfficiencyAlg', namePrefix+'JvtEfficiencyAlg'+nameSuffix )
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
    alg = createAlgorithm( 'CP::ObjectCutFlowHistAlg', namePrefix+'JetCutFlowDumperAlg'+nameSuffix )
    alg.histPattern = namePrefix+'jet_cflow_%SYS%'+nameSuffix
    alg.selection = cutlist
    alg.selectionNCuts = cutlength
    seq.append( alg, inputPropName = 'input' )

    # Set up an algorithm that makes a view container using the selections
    # performed previously:
    alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg', namePrefix+'JetViewFromSelectionAlg'+nameSuffix )
    alg.selection = cutlist
    seq.append( alg, inputPropName = 'input', outputPropName = 'output' )

    # Set up an algorithm dumping the properties of the jets, for debugging:
    alg = createAlgorithm( 'CP::KinematicHistAlg', namePrefix+'JetKinematicDumperAlg'+nameSuffix )
    alg.histPattern = namePrefix+'jet_%VAR%_%SYS%'+nameSuffix
    seq.append( alg, inputPropName = 'input' )

    # Set up a final deep copy making algorithm if requested:
    if deepCopyOutput:
        alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg',
                               namePrefix+'JetDeepCopyMaker'+nameSuffix )
        alg.deepCopy = True
        seq.append( alg, inputPropName = 'input', outputPropName = 'output',
                    stageName = 'selection' )
        pass
    

    # Return the sequence:
    return seq
