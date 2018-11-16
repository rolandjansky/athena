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
