from JetRec.JetRecStandardToolManager import jtm
# Create a logger for this stream
import AthenaCommon
logger = AthenaCommon.Logging.logging.getLogger("EXOT27Utils")

def getJetRecTool(collection, tryGetParent=True):
  """Get the JetRecTool for a given collection from the jtm. If tryGetParent is
     set then if that tool has an InputContainer property set then try and 
     retrieve the JetRecTool for that parent, continue going until there is no
     InputContainer property or if the corresponding JetRecTool is not defined.
     In this case it is assumed that this means the InputContainer is already 
     present and not built. Will raise a KeyError if no JetRecTool can be
     found.
  """
  try:
    jetRecTool = jtm[collection]
  except KeyError as e:
     raise KeyError("JetRecTool {0} not present in jtm".format(collection) )
  if tryGetParent and hasattr(jetRecTool, "InputContainer") and jetRecTool.InputContainer:
    try:
      parent = getJetRecTool(jetRecTool.InputContainer, True)
      jetRecTool = parent
    except KeyError as e:
      pass
  return jetRecTool

def addPseudojetgettersToJetCollection(
    collection, getters, addToParent=True):
  # First, get the jet rec tool from jtm. The JetRec functions appear to always
  # add this tool using the collection name so retrieve it this way
  jetRecTool = getJetRecTool(collection, addToParent)
  # May have to unlock the tool to do this... Maybe this is awful practice, I
  # don't know...
  locked = jetRecTool.isLocked()
  if locked:
    logger.debug("Temporarily unlocking tool {0}".format(jetRecTool.getFullName() ) )
    jetRecTool.unlock()
  for getter in getters:
    # By convention, getters are stored in lower case
    getter_l = getter.lower()
    try:
      the_getter = jtm[getter_l]
    except KeyError as e:
      raise KeyError("Unable to locate PseudoJetGetter: {0}".format(getter_l) )
    if not the_getter.getFullName() in jetRecTool.PseudoJetGetters:
      jetRecTool.PseudoJetGetters.append(the_getter.getFullName() )
  if locked:
    logger.debug("Re-locking tool {0}".format(jetRecTool.getFullName() ) )
    jetRecTool.lock()
