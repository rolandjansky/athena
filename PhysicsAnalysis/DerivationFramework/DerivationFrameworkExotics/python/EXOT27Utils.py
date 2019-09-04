from JetRec.JetRecStandardToolManager import jtm
import DerivationFrameworkJetEtMiss.JetCommon as JetCommon
import DerivationFrameworkJetEtMiss.ExtendedJetCommon as ExtendedJetCommon
from JetRec.JetRecConf import JetAlgorithm
# Create a logger for this stream
import AthenaCommon
from BTagging.BTaggingConfiguration import getConfiguration
ConfInst=getConfiguration()
logger = AthenaCommon.Logging.logging.getLogger("EXOT27Utils")

def getJetRecTool(collection, getParent=True):
  """Get the JetRecTool for a given collection from the jtm. If getParent is
     set then if that tool has an InputContainer property set then try and 
     retrieve the JetRecTool for that parent, continue going until there is no
     InputContainer property. Will raise a KeyError if no JetRecTool can be
     found at any stage.
  """
  try:
    jetRecTool = jtm[collection]
  except KeyError as e:
     raise KeyError("JetRecTool {0} not present in jtm".format(collection) )
  if getParent and hasattr(jetRecTool, "InputContainer") and jetRecTool.InputContainer:
    jetRecTool = getJetRecTool(jetRecTool.InputContainer, True)
  return jetRecTool

def linkPseudoJetGettersToExistingJetCollection(
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
    This code is largely based on the linkVRJetsToLargeRJets from HbbCommon
  """
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
  LargeRJets          = LargeRJetPrefix+"Jets"
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

  assocTool = ExtendedJetCommon.getJetExternalAssocTool(
      ungroomedJetAlg,
      LargeRJetPrefix,
      MomentPrefix = '',
      ListOfOldLinkNames=[g.Label for g in newGetters]
      )
  ExtendedJetCommon.applyJetAugmentation(
      ungroomedJetAlg, LinkTransferAlg, sequence, assocTool)

  return originalUngroomedName, [g.Label for g in newGetters]

#####
# Here put the trigger lists that we depend on
# This is preferable to the trigger API because that selects many extra
# triggers. We are now outside of running so new triggers can be added!
#####
met_triggers = [
        "HLT_xe70_mht",
        "HLT_xe90_mht_L1XE50",
        "HLT_xe100_mht_L1XE50",
        "HLT_xe110_mht_L1XE50",
        "HLT_xe110_pufit_L1XE50",
        "HLT_xe110_pufit_L1XE55",
        "HLT_xe110_pufit_xe65_L1XE50",
        "HLT_xe110_pufit_xe70_L1XE50"
]
single_electron_triggers = [
        "HLT_e24_lhmedium_L1EM20VH",  # data 2015 only, no MC
        "HLT_e60_lhmedium",  # 2015
        "HLT_e120_lhloose",  # 2015
        "HLT_e24_lhtight_nod0_ivarloose",  # 2016 A, B-D3
        "HLT_e60_lhmedium_nod0",  # 2016 A, B-D3, D4-F, G-, 2017 B-, 2018
        "HLT_e60_medium",  # 2016 A, B-D3, D4-F, G-
        "HLT_e300_etcut",  # 2016 A, B-D3, D4-F, G-, 2017 B-, 2018
        "HLT_e140_lhloose_nod0",  # 2016 A, B-D3, D4-F, G-, 2017 B-, 2018
        "HLT_e26_lhtight_nod0_ivarloose"  # 2016 D4-F, G-, 2017 B-, 2018
]
single_muon_triggers = [
        "HLT_mu20_iloose_L1MU15",  # 2015
        "HLT_mu50",  # 2016 A, B-D3, D4-E, F-G2, G3-I3, I4-, 2017 B-, 2018
        "HLT_mu24_iloose_L1MU15",  # MC only
        "HLT_mu24_iloose",  # data 2016 A only, no MC
        "HLT_mu24_ivarloose",  # MC only -> this trigger was not included
        "HLT_mu24_ivarloose_L1MU15",  # data 2016 A only, no MC -> this trigger was not included
        "HLT_mu40",  # 2015, 2016 A
        "HLT_mu24_ivarmedium",  # 2016 B-D3, D4-E
        "HLT_mu24_imedium",  # 2016 B-D3, D4-E -> this trigger was not included
        "HLT_mu26_ivarmedium",  # 2016 D4-E, F-G2, G3-I3, I4-, 2017 B-, 2018
        "HLT_mu26_imedium",  # 2016 D4-E, F-G2 -> this trigger was not included
]
single_photon_triggers = [
        "HLT_g140_loose",
        "HLT_g300_etcut",
        "HLT_g120_loose",
        "HLT_g200_etcut"
]
