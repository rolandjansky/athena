# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Python function implementation of B-tagging configuration
# Wouter van den Wollenberg (2013-2015)
# E-mail: wouter.van.den.wollenberg@cern.ch, wwollenb@nikhef.nl

from BTagging.BTaggingFlags import BTaggingFlags
from RecExConfig.RecFlags import rec

# -- Import all the functions that set up all the tools --

# (TOOLS NOT PORTED TO RELEASE 19 ARE COMMENTED OUT)

# Common tools:
from BTagging.BTaggingConfiguration_CommonTools import *
from BTagging.BTaggingConfiguration_InDetVKalVxInJetTool import *
from BTagging.BTaggingConfiguration_InDetVKalVxNegativeTagInJetTool import *
from BTagging.BTaggingConfiguration_InDetVKalVxMultiVxInJetTool import *

# IP taggers
#from BTagging.BTaggingConfiguration_IP1DTag import *
#from BTagging.BTaggingConfiguration_IP2DFlipTag import *
from BTagging.BTaggingConfiguration_IP2DNegTag import *
#from BTagging.BTaggingConfiguration_IP2DPosTag import *
#from BTagging.BTaggingConfiguration_IP2DSpcFlipTag import *
#from BTagging.BTaggingConfiguration_IP2DSpcNegTag import *
#from BTagging.BTaggingConfiguration_IP2DSpcPosTag import *
#from BTagging.BTaggingConfiguration_IP2DSpcTag import *
from BTagging.BTaggingConfiguration_IP2DTag import *
#from BTagging.BTaggingConfiguration_IP3DFlipTag import *
from BTagging.BTaggingConfiguration_IP3DNegTag import *
#from BTagging.BTaggingConfiguration_IP3DPosTag import *
#from BTagging.BTaggingConfiguration_IP3DSpcFlipTag import *
#from BTagging.BTaggingConfiguration_IP3DSpcNegTag import *
#from BTagging.BTaggingConfiguration_IP3DSpcPosTag import *
#from BTagging.BTaggingConfiguration_IP3DSpcTag import *
from BTagging.BTaggingConfiguration_IP3DTag import *
from BTagging.BTaggingConfiguration_RNNIPTag import *

# Jet fitter taggers
#from BTagging.BTaggingConfiguration_JetFitterCOMB import *
#from BTagging.BTaggingConfiguration_JetFitterTagCOMBNNIP3DNeg import *
#from BTagging.BTaggingConfiguration_JetFitterTagCOMBNNIP3DPos import *
##from BTagging.BTaggingConfiguration_JetFitterTagCOMBNN import *
#from BTagging.BTaggingConfiguration_JetFitterTagNNFlip import *
##from BTagging.BTaggingConfiguration_JetFitterTagNN import *
#from BTagging.BTaggingConfiguration_JetFitterTag import *

# New jet fitter taggers
#from BTagging.BTaggingConfiguration_NewJetFitterCOMB import *
#from BTagging.BTaggingConfiguration_NewJetFitterFlipCOMB import *
#from BTagging.BTaggingConfiguration_NewJetFitterTagFlip import *
##from BTagging.BTaggingConfiguration_NewJetFitterTag import *

from BTagging.BTaggingConfiguration_NewJetFitterCollection import * #I collected all jetfitter functions currently needed for rel. 19 into here (Wouter)
from BTagging.BTaggingConfiguration_NewJetFitterIP3DNegCollection import *

# Lepton taggers
#from BTagging.BTaggingConfiguration_SoftElectronTag import *
#from BTagging.BTaggingConfiguration_SoftMuonTagChi2 import *
from BTagging.BTaggingConfiguration_SoftMuonTag import *

# Track counting
#from BTagging.BTaggingConfiguration_TrackCountingFlip import *
#from BTagging.BTaggingConfiguration_TrackCounting import *

# SV taggers
from BTagging.BTaggingConfiguration_SV0Tag import *
from BTagging.BTaggingConfiguration_SV1FlipTag import *
from BTagging.BTaggingConfiguration_SV1Tag import *
#from BTagging.BTaggingConfiguration_SV2FlipTag import *
from BTagging.BTaggingConfiguration_SV2Tag import *

# JetProb taggers
#from BTagging.BTaggingConfiguration_JetProbFlipTag import *
#from BTagging.BTaggingConfiguration_JetProbTag import *

# MultiSV tagger
#from BTagging.BTaggingConfiguration_MultiSVTag import *
from BTagging.BTaggingConfiguration_MultiSVbb1Tag import *
from BTagging.BTaggingConfiguration_MultiSVbb2Tag import *

# MV taggers
from BTagging.BTaggingConfiguration_MV1FlipTag import *
from BTagging.BTaggingConfiguration_MV1Tag import *
#from BTagging.BTaggingConfiguration_MV2FlipTag import *
#from BTagging.BTaggingConfiguration_MV2Tag import *
from BTagging.BTaggingConfiguration_MV2rTag import *
from BTagging.BTaggingConfiguration_MV2rFlipTag import *
from BTagging.BTaggingConfiguration_MV2rmuTag import *
from BTagging.BTaggingConfiguration_MV2rmuFlipTag import *
from BTagging.BTaggingConfiguration_MV1cTag import *
from BTagging.BTaggingConfiguration_MV1cFlipTag import *
from BTagging.BTaggingConfiguration_MV2c00Tag import *
from BTagging.BTaggingConfiguration_MV2c00FlipTag import *
from BTagging.BTaggingConfiguration_MV2c10Tag import *
from BTagging.BTaggingConfiguration_MV2c10FlipTag import *
from BTagging.BTaggingConfiguration_MV2c20Tag import *
from BTagging.BTaggingConfiguration_MV2c20FlipTag import *
from BTagging.BTaggingConfiguration_MV2c100Tag import *
from BTagging.BTaggingConfiguration_MV2c100FlipTag import *
from BTagging.BTaggingConfiguration_MV2cl100Tag import *
from BTagging.BTaggingConfiguration_MV2cl100FlipTag import *
from BTagging.BTaggingConfiguration_MV2mTag import *
from BTagging.BTaggingConfiguration_MV2mFlipTag import *
from BTagging.BTaggingConfiguration_MV2c10hpTag import *
from BTagging.BTaggingConfiguration_MV2c10hpFlipTag import *

#JetVertexCharge tool
from BTagging.BTaggingConfiguration_JetVertexCharge import *

#ExKtbb tool
from BTagging.BTaggingConfiguration_ExKtbbTag import *

# MultivariateTagManager
from BTagging.BTaggingConfiguration_MultivariateTagManager import *
from BTagging.BTaggingConfiguration_MultivariateFlipTagManager import *

# DL1 tagger
from BTagging.BTaggingConfiguration_DL1Tag import *
from BTagging.BTaggingConfiguration_DL1FlipTag import *

# TagNtupleDumper
from BTagging.BTaggingConfiguration_TagNtupleDumper import *

# Global dictionary keeping track of all tool collections
_BTaggingConfig_ToolCollections = {}

# We also need deepcopy here (for cloning metadata when cloning tools since these include nested dictionaries)
from copy import deepcopy

# Global dictionary to keep track of all configuration instances
_BTaggingConfig_Configurations = {}

def configurationExists(name = ""):
  """Returns whether a configuration with the given name exists or not."""
  if name in listConfigurations():
    return True
  return False

def getConfiguration(name = ""):
  """Returns a configuration instance of the given name. The default one has an empty string as name. Creates it if it does not exist."""
  if name in _BTaggingConfig_Configurations:
    return _BTaggingConfig_Configurations[name]
  else:
    return createConfiguration(name)

def listConfigurations():
  """Lists the names of all configurations currently instanced."""
  ReturnList = []
  for conf in _BTaggingConfig_Configurations:
    ReturnList.append(conf)
  return ReturnList

def createConfiguration(name = ""):
  """Create a new configuration instance with a given name and returns it."""
  global _BTaggingConfig_Configurations
  if name in _BTaggingConfig_Configurations:
    print '#BTAG# - ERROR - Attempting to create a configuration instance with name "'+name+'" but this configuration already exists!'
    raise ValueError
  NewConf = Configuration(name)
  _BTaggingConfig_Configurations[name] = NewConf
  if len(name) == 0:
    print '#BTAG# - INFO - Default configuration instance created.'
  else:
    print '#BTAG# - INFO - Non-default configuration instance created: "'+name+'"'
  return NewConf

class Configuration:

  def __init__(self, name = ""):

    # Name of this configuration (default is empty)
    self._name = name

    # Configuration specific version of BTaggingFlags.OutputFilesX settings.
    self._OutputFilesPrefix = BTaggingFlags.OutputFilesPrefix
    self._OutputFilesSVname = BTaggingFlags.OutputFilesSVname
    self._OutputFilesJFVxname = BTaggingFlags.OutputFilesJFVxname
    self._OutputFilesBaseName = BTaggingFlags.OutputFilesBaseName
    self._OutputFilesBaseAuxName = BTaggingFlags.OutputFilesBaseAuxName
    self._OutputFilesBaseNameSecVtx = BTaggingFlags.OutputFilesBaseNameSecVtx
    self._OutputFilesBaseAuxNameSecVtx = BTaggingFlags.OutputFilesBaseAuxNameSecVtx
    self._OutputFilesBaseNameJFSecVtx = BTaggingFlags.OutputFilesBaseNameJFSecVtx
    self._OutputFilesBaseAuxNameJFSecVtx = BTaggingFlags.OutputFilesBaseAuxNameJFSecVtx

    # This dictionary keeps track of tools/taggers that have been initialized
    # the key is the type of the tool, the value is the actual tool. All functions
    # that set up tools should add to this dictionary!
    self._BTaggingConfig_InitializedTools = {}

    # This dictionaries keeps track of all btagtools and other things for each jet collection.
    self._BTaggingConfig_JetCollections = {}
    self._BTaggingConfig_SecVtxTools = {}
    self._BTaggingConfig_MainAssociatorTools = {}

    # This dictionary keeps track of all JetBTaggerTools for each jet collection.
    self._BTaggingConfig_JetBTaggerTools = {}

    # These dictionaries keeps track of all particle associators for each jet collection. Photon associators are counted
    # as electron associators.
    self._BTaggingConfig_TrackAssociators = {}
    self._BTaggingConfig_MuonAssociators = {}
    self._BTaggingConfig_ElectronAssociators = {}

    # These dictionaries keep track of CollectionNames and ContainerNames. Technically we could also put them into
    # the above dictionaries. But this should be clearer.
    self._BTaggingConfig_ElectronConCol = {}
    self._BTaggingConfig_PhotonConCol = {}
    self._BTaggingConfig_TrackConCol = {}
    self._BTaggingConfig_MuonConCol = {}

    # dictionary keeping track of tools tied to which jet collections and which tracks.
    self._BTaggingConfig_SetUpCombinations = {}

    # Boolean to toggle behaviour where various flags are not checked to see whether a tagger could run,
    # (this happens in the taggerIsPossible() function) and includes DetFlags. This currently seems needed for retagging? Users
    # can toggle this using the doNotCheckForTaggerObstacles() function.
    self._IgnoreTaggerObstacles = False

    # Keeping track whether this configuration has been initialized or not.
    self._Initialized = False

    # Whether to prefix xAOD names or not with the name of the configuration instance (to avoid overwriting other instances in output).
    self._PrefixxAODBaseName = True
    self._PrefixVertexFinderxAODBaseName = True

  def getOutputFilesPrefix(self):
    """Retrieves the configuration specific setting of BTaggingFlags.OutputFilesPrefix."""
    return self._OutputFilesPrefix

  def getOutputFilesSVname(self):
    """Retrieve the configuration specific setting of BTaggingFlags.OutputFilesSVname."""
    return self._OutputFilesSVname

  def getOutputFilesJFVxname(self):
    """Retrieve the configuration specific setting of BTaggingFlags.OutputFilesJFVxname."""
    return self._OutputFilesJFVxname

  def getOutputFilesBaseName(self):
    """Retrieve the configuration specific setting of BTaggingFlags.OutputFilesBaseName."""
    return self._OutputFilesBaseName

  def getOutputFilesBaseAuxName(self):
    """Retrieve the configuration specific setting of BTaggingFlags.OutputFilesBaseAuxName."""
    return self._OutputFilesBaseAuxName

  def getOutputFilesBaseNameSecVtx(self):
    """Retrieve the configuration specific setting of BTaggingFlags.OutputFilesBaseNameSecVtx."""
    return self._OutputFilesBaseNameSecVtx

  def getOutputFilesBaseAuxNameSecVtx(self):
    """Retrieve the configuration specific setting of BTaggingFlags.OutputFilesBaseAuxNameSecVtx."""
    return self._OutputFilesBaseAuxNameSecVtx
  def getOutputFilesBaseNameJFSecVtx(self):
    """Retrieve the configuration specific setting of BTaggingFlags.OutputFilesBaseNameJFSecVtx."""
    return self._OutputFilesBaseNameJFSecVtx

  def getOutputFilesBaseAuxNameJFSecVtx(self):
    """Retrieve the configuration specific setting of BTaggingFlags.OutputFilesBaseAuxNameJFSecVtx."""
    return self._OutputFilesBaseAuxNameJFSecVtx

  def setOutputFilesPrefix(self, Setting, Verbose=True):
    """Sets the configuration specific setting of the BTaggingFlags.OutputFilesPrefix."""
    self._OutputFilesPrefix = Setting
    if Verbose:
      print self.BTagTag()+' - DEBUG - Setting OutputFilesPrefix to '+str(Setting)

  def setOutputFilesSVname(self, Setting, Verbose=True):
    """Sets the configuration specific setting of the BTaggingFlags.OutputFilesSVname."""
    self._OutputFilesSVname = Setting
    if Verbose:
      print self.BTagTag()+' - DEBUG - Setting OutputFilesSVname to '+str(Setting)

  def setOutputFilesJFVxname(self, Setting, Verbose=True):
    """Sets the configuration specific setting of the BTaggingFlags.OutputFilesJFVxname."""
    self._OutputFilesJFVxname = Setting
    if Verbose:
      print self.BTagTag()+' - DEBUG - Setting OutputFilesJFVxname to '+str(Setting)

  def setOutputFilesBaseName(self, Setting, Verbose=True):
    """Sets the configuration specific setting of the BTaggingFlags.OutputFilesBaseName."""
    self._OutputFilesBaseName = Setting
    if Verbose:
      print self.BTagTag()+' - DEBUG - Setting OutputFilesBaseName to '+str(Setting)

  def setOutputFilesBaseAuxName(self, Setting, Verbose=True):
    """Sets the configuration specific setting of the BTaggingFlags.OutputFilesBaseAuxName."""
    self._OutputFilesBaseAuxName = Setting
    if Verbose:
      print self.BTagTag()+' - DEBUG - Setting OutputFilesBaseAuxName to '+str(Setting)

  def setOutputFilesBaseNameSecVtx(self, Setting, Verbose=True):
    """Sets the configuration specific setting of the BTaggingFlags.OutputFilesBaseNameSecVtx."""
    self._OutputFilesBaseNameSecVtx = Setting
    if Verbose:
      print self.BTagTag()+' - DEBUG - Setting OutputFilesBaseNameSecVtx to '+str(Setting)

  def setOutputFilesBaseAuxNameSecVtx(self, Setting, Verbose=True):
    """Sets the configuration specific setting of the BTaggingFlags.OutputFilesBaseAuxNameSecVtx."""
    self._OutputFilesBaseAuxNameSecVtx = Setting
    if Verbose:
      print self.BTagTag()+' - DEBUG - Setting OutputFilesBaseAuxNameSecVtx to '+str(Setting)

  def setOutputFilesBaseNameJFSecVtx(self, Setting, Verbose=True):
    """Sets the configuration specific setting of the BTaggingFlags.OutputFilesBaseNameJFSecVtx."""
    self._OutputFilesBaseNameJFSecVtx = Setting
    if Verbose:
      print self.BTagTag()+' - DEBUG - Setting OutputFilesBaseNameJFSecVtx to '+str(Setting)

  def setOutputFilesBaseAuxNameJFSecVtx(self, Setting, Verbose=True):
    """Sets the configuration specific setting of the BTaggingFlags.OutputFilesBaseAuxNameJFSecVtx."""
    self._OutputFilesBaseAuxNameJFSecVtx = Setting
    if Verbose:
      print self.BTagTag()+' - DEBUG - Setting OutputFilesBaseAuxNameJFSecVtx to '+str(Setting)

  def BTagTag(self):
    """Returns the tag used in messages made by this instance. This allows one to distinguish messages coming from different instances in the log."""
    if len(self._name) == 0:
      return "#BTAG#"
    else:
      return "#BTAG ("+self._name+")#"

  def GeneralToolSuffix(self):
    """Returns the suffix added to the name of all tools managed by this instance."""
    if len(self._name) == 0:
      return ""
    else:
      return "_"+self._name

  def PrefixxAODBaseName(self, Setting, Verbose=True):
    """Sets the value of _PrefixxAODBaseName which controls whether the name of this instance is prefixed to the xAODBaseName of tools."""
    if Verbose:
      print self.BTagTag()+' - DEBUG - Setting _PrefixxAODBaseName to '+str(Setting)
    self._PrefixxAODBaseName = Setting

  def PrefixVertexFinderxAODBaseName(self, Setting, Verbose=True):
    """Sets the value of _PrefixVertexFinderxAODBaseName which controls whether the name of this instance is prefixed to the VertexFinderxAODBaseName of tools."""
    if Verbose:
      print self.BTagTag()+' - DEBUG - Setting _PrefixVertexFinderxAODBaseName to '+str(Setting)
    self._PrefixVertexFinderxAODBaseName = Setting

  def getxAODBaseNamePrefix(self):
    """Returns either an empty string or the name of the instance depending on the setting of _PrefixxAODBaseName."""
    if not self._PrefixxAODBaseName:
      return ""
    return self._name

  def getVertexFinderxAODBaseName(self):
    """Returns either an empty string or the name of the instance depending on the setting of _PrefixVertexFinderxAODBaseName."""
    if not self._PrefixVertexFinderxAODBaseName:
      return ""
    return self._name

  def doNotCheckForTaggerObstacles(self):
      self._IgnoreTaggerObstacles = True

  def printAllTools(self, PrintJetBTaggerTools = True, PrintOtherTools = True):
      """Prints all tools. Printing tools during set up is not very useful since attributes are
      added dynamically, so the print-out would not be representative. Therefore we have this function
      which you can call at the end of the setup (or any other moment you like =) )."""
      if PrintJetBTaggerTools:
          print '--- JetBTaggerTools ---'
          for tool in self._BTaggingConfig_JetBTaggerTools:
              print str(tool)+': ',self._BTaggingConfig_JetBTaggerTools[tool]
          print '--- Main Associator Tools ---'
          for tool in self._BTaggingConfig_MainAssociatorTools:
              print str(tool)+': ',self._BTaggingConfig_MainAssociatorTools[tool]
      if PrintOtherTools:
          print '--- Associators ---'
          for tool in self._BTaggingConfig_TrackAssociators:
              print str(tool)+': ',self._BTaggingConfig_TrackAssociators[tool]
          for tool in self._BTaggingConfig_MuonAssociators:
              print str(tool)+': ',self._BTaggingConfig_MuonAssociators[tool]
          for tool in self._BTaggingConfig_ElectronAssociators:
              print str(tool)+': ',self._BTaggingConfig_ElectronAssociators[tool]
          print '--- BTagTools ---'
          for tool in self._BTaggingConfig_JetCollections:
              print str(tool)+': ',self._BTaggingConfig_JetCollections[tool]
          print '--- BTagTool support tools ---'
          for tool in self._BTaggingConfig_SecVtxTools:
              print str(tool)+': ',self._BTaggingConfig_SecVtxTools[tool]
          print '--- Other tools ---'
          for tool in self._BTaggingConfig_InitializedTools:
              print str(tool)+': ',self._BTaggingConfig_InitializedTools[tool]

  def updateBTaggers(self, JetCollection):
      """Updates the JetBTaggerTool for the selected JetCollection if they already exist. This is in order to keep
      them up-to-date, as someone might set up a tool after these main tools already exist. And such a tool might require a new associator
      which we need to add to the main associator tool."""
      if JetCollection in self._BTaggingConfig_JetBTaggerTools:
          self.setupJetBTaggerTools(JetCollections=JetCollection)

  def setupJetBTaggerTool(self, ToolSvc=None, JetCollection="", TaggerList=[], SetupScheme="", topSequence=None, Verbose = None, AddToToolSvc = True, options={}, StripJetsSuffix = True, TrackAssociatorName="MatchedTracks"):
      """Convenience function which takes only a single jet collection and returns an instance instead
      of a list; see setupJetBTaggerTools for more info. This function is mainly here for easy calling for BTagging from JetRec.

      This function also strips any trailing 'Jets' in the JetCollection if StripJetsSuffix=True (which is the default)."""
      if (JetCollection.lower()).endswith("jets"):
          if Verbose is None:
              if BTaggingFlags.OutputLevel < 3:
                  print(self.BTagTag()+" - DEBUG - Stripping trailing 'jets' from jet collection '"+JetCollection+"' prior to setup.")
          elif Verbose:
              print(self.BTagTag()+" - DEBUG - Stripping trailing 'jets' from jet collection '"+JetCollection+"' prior to setup.")
          JetCollection = JetCollection[:-4]
      btagger = self.setupJetBTaggerTools(ToolSvc, [JetCollection,], topSequence, Verbose, AddToToolSvc, options, TaggerList, SetupScheme, TrackAssociatorName)
      return btagger[0]

  def setupJetBTaggerTools(self, ToolSvc=None, JetCollections=[], topSequence=None, Verbose = None, AddToToolSvc = True, options={}, TaggerList=[], SetupScheme = "", TrackAssociatorName="MatchedTracks"):
      """Sets up JetBTaggerTool tools and adds them to the topSequence (one per jet collection). This function just updates
      the tool if such a tool already exists for the specified jet collections. This function should only be used for
      jet collections that one need reconstruction. Note that it is allowed to set topSequence to None,
      in this case the tool will not be added to the topSequence. (This is actually used in the default set up). Note
      that we do need the ToolSvc, this is needed to set up the associator tool.

      This function returns None for all jet collections specified if the BTaggingFlags do not allow for B-tagging to run.

      input: ToolSvc:          The ToolSvc. (not needed when updating)
             JetCollections:   List of jet collection name (can also accept string in the case of one collection).
             topSequence:      The topSequence. (not needed when updating)
             Verbose:          Whether to print some additional information. If None then BTaggingFlags.OutputLevel will be used.
             AddToToolSvc:     Whether to add the JetBTaggerTool to the ToolSvc or not.
             options:          List of options to be passed to the JetBTaggerTools. This has the following defaults:

      OutputLevel:                                  default: BTaggingFlags.OutputLevel
      name:                                         default: (BTaggingFlags.OutputFilesPrefix + JetCollection).lower()
      BTagName:                                     default: BTaggingFlags.OutputFilesPrefix + JetCollection
      BTagJFVtxName:                                default: BTaggingFlags.OutputFilesJFVxname
      BTagSVName:                                   default: BTaggingFlags.OutputFilesSVname

             TaggerList:       List of taggers to setup. If left empty the defaults are used. The entries correspond to the names found in
                               the BTaggingFlags class. If used all other taggers are switched off. Note however that if you enable a tagger
                               which depends on other taggers then those taggers it depends on will also be set up regardless of this list.
                               Note: It is up to the scheme to ultimately determine the taggers, although the Default scheme will adhere to this
                               list.
             SetupScheme:      Type of setup scheme to use. This allows the b-tagging group to specify more elaborate setup scripts. They are defined
                               in ./python/BTaggingConfiguration_LoadTools.py. If left empty (which is the default) the one specified in the BTaggingFlags
                               will be used.

      output: List of JetBTaggerTools (this includes those for which no tool was set up because it already existed)."""
      if len(SetupScheme) == 0:
        SetupScheme = BTaggingFlags.ConfigurationScheme
      from BTagging.BTaggingConfiguration_LoadTools import SetupJetCollection
      import sys
      if Verbose is None:
          Verbose = (BTaggingFlags.OutputLevel < 3)
      if Verbose:
          print self.BTagTag()+' - DEBUG - Setting up JetBTaggerTools for jet collections: '+str(JetCollections)
      returnlist = []
      # Allow for string input
      if type(JetCollections) == str:
          JetCollections = [JetCollections,]
      if not self.checkFlagsUsingBTaggingFlags():
          for jetcol in JetCollections:
              returnlist.append(None)
          return returnlist
      for jetcol in JetCollections:
          # Make sure we have this jet collection; if not use defaults
          if not SetupJetCollection(jetcol, TaggerList, SetupScheme, self):
              returnlist.append(None)
              continue
          if jetcol in self._BTaggingConfig_JetBTaggerTools:
              returnlist.append(self._BTaggingConfig_JetBTaggerTools.get(jetcol, None))
              print self.BTagTag()+' - INFO - Updating JetBTaggerTool for '+jetcol
              # self.ConfigureMainAssociatorTool(self._BTaggingConfig_MainAssociatorTools[jetcol], jetcol)
              continue
          # Check if this jet collection has been correctly set up.
          if not jetcol in self._BTaggingConfig_JetCollections:
              print self.BTagTag()+' - ERROR - setupJetBTaggerTools() called for jet collection with name "'+jetcol+'" but this jet collection has not been initialized.'
              raise RuntimeError
          print self.BTagTag()+' - INFO - Adding JetBTaggerTool for '+jetcol
          from BTagging.BTaggingConf import Analysis__JetBTaggerTool as JetBTaggerTool
          options = dict(options)
          options.setdefault('OutputLevel', BTaggingFlags.OutputLevel)

          # setup the Analysis__BTagTrackAssociation tool
          # Note that this tool is tied to the JetBTaggerTool
          from BTagging.BTaggingConf import Analysis__BTagTrackAssociation
          assoc = \
                  Analysis__BTagTrackAssociation(
                          'thisBTagTrackAssociation_'+jetcol+self.GeneralToolSuffix(),
                          AssociatedTrackLinks = TrackAssociatorName,
                          AssociatedMuonLinks = "MatchedMuons",
                          TrackContainerName = "InDetTrackParticles",
                          MuonContainerName = "Muons",
                          TrackAssociationName = "BTagTrackToJetAssociator",
                          MuonAssociationName = "Muons"
                          )

          ToolSvc += assoc

          self._BTaggingConfig_MainAssociatorTools[jetcol] = assoc
          options.setdefault('BTagTrackAssocTool', assoc)
          # setup for "augmentation" only under the "Retag" scheme
          options.setdefault('BTagAugmentation', (SetupScheme == "Retag"))
          # setup the secondary vertexing tool
          options.setdefault('BTagSecVertexing', self.getJetCollectionSecVertexingTool(jetcol))
          # Set remaining options
          options.setdefault('name', (self.getOutputFilesPrefix() + jetcol + self.GeneralToolSuffix()).lower())
          # GhostTag not in the jetcol name given by jetAuthor
          if ("GhostTag" in jetcol):
              options.setdefault('JetCollectionName', jetcol.replace('GhostTag',''))
          else:
              options.setdefault('JetCollectionName', jetcol)
          options.setdefault('BTagName', self.getOutputFilesPrefix() + jetcol)
          options.setdefault('BTagJFVtxName', self._OutputFilesJFVxname)
          options.setdefault('BTagSVName', self._OutputFilesSVname)
          options['BTagTool'] = self._BTaggingConfig_JetCollections.get(jetcol, None)
          jetbtaggertool = JetBTaggerTool(**options)
          # Setup the associator tool
          # -- add tool to topSequence
          if not topSequence is None:
              topSequence += jetbtaggertool
          # -- add tool to ToolSvc
          if AddToToolSvc:
              ToolSvc += jetbtaggertool
          if Verbose:
              print jetbtaggertool
              print self.BTagTag()+' - INFO - Attached to the beforementioned JetBTaggerTool we have the following BTagTool:'
              print self._BTaggingConfig_JetCollections.get(jetcol, None)
          # Register
          self._BTaggingConfig_JetBTaggerTools[jetcol] = jetbtaggertool
          returnlist.append(jetbtaggertool)
      return returnlist


  def registerTool(self, tool_type, tool, track = "", jetcol = "", ToolSvc = None, Verbose = False, MuonCollection='Muons', ElectronCollection='Electrons', PhotonCollection='Photons', CheckPrerequisites=False, CheckOnlyInsideToolCollection=False, DoNotSetUpParticleAssociators=False):
      """Registers a tool so it can be used in order to figure out dependencies.
      This should be called after a tool is configured. It automatically adds it to
      ToolSvc if it is specified.

      If ToolSvc is not specified prerequisites will not be set up and the tool will
      (obviously) not be added to ToolSvc. It will still be registered though and other
      tools that depend on this one will recognize it.

      input:   tool_type:                     The type of tool, should correspond to a b-tagging group tool name (like IP3DTag).
               tool:                          Actual tool.
               ToolSvc:                       The ToolSvc.
               track:                         Track association the tool is runnning on.
               jetcol:                        Jet collection the tool is running on.
               Verbose:                       Verbose or not.
               MuonCollection:                Muon collection (can be left blank if not needed or using default).
               ElectronCollection:            Electron collection (can be left blank if not needed or using default).
               PhotonCollection:              Photon collection (can be left blank if not needed or using default).
               CheckPrerequisites:            Whether to check for prerequisites and set them up if needed. Off by default.
               CheckOnlyInsideToolCollection: Whether to check only within the tool collection for prerequisites.
               DoNotSetUpParticleAssociators: Do not setup particle associators if needed.
      """
      if len(track) == 0:
          if self.getToolName(tool_type, "OroroMonroe", jetcol) != self.getToolName(tool_type, "TriangleWizard", jetcol):
              print self.BTagTag()+' - WARNING - Attempting to register tool "'+tool_type+'" without specifying required track association!'
              raise ValueError
      if len(jetcol) == 0:
          if self.getToolName(tool_type, track, "CarlSagan") != self.getToolName(tool_type, track, "RichardFeynman"):
              print self.BTagTag()+' - WARNING - Attemping to register tool "'+tool_type+'" without specifying required jet collection!'
              raise ValueError
      if tool is None:
          print self.BTagTag()+' - WARNING - A tool of type '+tool_type+' has type None!'
          raise ValueError
      if self.getToolName(tool_type, track, jetcol) in self._BTaggingConfig_InitializedTools:
          print self.BTagTag()+' - WARNING - A tool named '+self.getToolName(tool_type, track, jetcol)+' has already been set up before!'
          raise ValueError
      if BTaggingFlags.OutputLevel < 3:
          print self.BTagTag()+' - DEBUG - Registering tool: '+self.getToolName(tool_type, track, jetcol)
      if ToolSvc is None:
          if BTaggingFlags.OutputLevel < 3:
              print self.BTagTag()+' - DEBUG - INFO: ToolSvc is None during registration. This could be on purpose if want to add the tool to ToolSvc yourself and in particular is normal for BTagCalibrationBrokerTool.'
      if CheckPrerequisites and not ToolSvc is None:
        self.setupPrerequisites(tool_type, ToolSvc, TrackCollection = track, JetCollection = jetcol, Verbose = Verbose,
                                MuonCollection=MuonCollection, ElectronCollection=ElectronCollection, PhotonCollection=PhotonCollection,
                                CheckOnlyInsideToolCollection=CheckOnlyInsideToolCollection, DoNotSetUpParticleAssociators=DoNotSetUpParticleAssociators)
      self._BTaggingConfig_InitializedTools[self.getToolName(tool_type, track, jetcol)] = tool
      if not ToolSvc is None:
          ToolSvc += tool

  def getToolAttribute(self, attribute, tool_type, TrackCollection="", JetCollection="", RaiseException=True):
      """Returns an attribute of a tool. Technically the same can be returned by first getting a reference
      to the tool via a getTool() call and then just accessing the attribute directly. However at some point
      getTool will only return a PublicToolHandle and its attributes cannot be retrieved. At that point this
      function can be useful.

      Returns None if the tool does not exist or the requested attribute does not exist. If RaiseException is True
      an AttributeError is raised when the attribute does not exist."""
      tool = self.getTool(tool_type, TrackCollection, JetCollection)
      if tool is None:
        return None
      if not hasattr(tool, attribute) and RaiseException:
        print self.BTagTag()+' - ERROR - Attribute "'+attribute+'" does not exist for tool of type "'+tool_type+'".'
        raise AttributeError
      return getattr(tool, attribute, None)

  def getTool(self, tool_type, TrackCollection="", JetCollection=""):
      """Returns a reference to a tool of type tool_type for the track association and
      jet collection specified. Returns None if it does not exist.

      input: tool_type:       The type of the tool one wants to get.
             TrackCollection: The track association. Can be left out for tools that are not set up per track collection.
             JetCollection:   The jet collection. Can be left out for tools that are not set up per jet collection.
      output: The tool."""
      name = self.getToolName(tool_type, TrackCollection, JetCollection)
      if name in self._BTaggingConfig_InitializedTools:
          return self._BTaggingConfig_InitializedTools[name]
      else:
          return None

  def getTools(self, tool_type, jetcol):
      """Returns a list of tools set up with tool_type for jet collection jet col."""
      returnlist = []
      for name in self._BTaggingConfig_InitializedTools:
          # We check whether the tool has the correct jet collection, or has no specific jet collection assigned
          if name.find(tool_type)!=-1 and (name.find(jetcol)!=-1 or name.find(tool_type)+len(tool_type) == len(name)):
              if name.find(tool_type)==0:
                  returnlist.append(self._BTaggingConfig_InitializedTools[name])
              elif name[name.find(tool_type)-1] == '_':
                  returnlist.append(self._BTaggingConfig_InitializedTools[name])
      return returnlist

  def getRegisteredTools(self):
      """Returns a list of all tools that have been set up and registered.

      output: List of tools that have been set up."""
      returnlist = []
      for name in self._BTaggingConfig_InitializedTools:
          returnlist.append(name)
      return returnlist

  def getToolName(self, tool, track, jetcol):
      """Returns the name of the tool with the collections. This is used because some
      tools should not be replicated (so there should be one even if using multiple prefix chains.
      To facilitate this, this function will return the bare non-prefixed version if the tool does
      not need a prefix (and should this be available for all prefix chains).

      input: tool:          The tagger/tool name one wishes to check or set up.
             track:         The track association.
             jetcol:        The jet collection.
      output: The name of the tool."""
      # Some tools only need exactly one in ToolSvc
      if getToolMetadata(tool, 'OneInTotal'):
          return tool+self.GeneralToolSuffix()
      # Some tools need one per track association
      if not getToolMetadata(tool, 'OnePerTrackAssociationAndJetCollection'):
          # Append track association if non-default
          defaulttracks = getToolMetadata(tool, 'DefaultTracks')
          if not defaulttracks:
              defaulttracks = 'BTagTrackToJetAssociator'
          if track == defaulttracks:
              return tool+self.GeneralToolSuffix()
          return tool+'_'+track+self.GeneralToolSuffix()
      # All others require one per track and per jet collection
      return jetcol+'_'+tool+'_'+track+self.GeneralToolSuffix()

  def checkPrerequisites(self, tool_type, TrackCollection="", JetCollection="", CheckOnlyInsideToolCollection=False):
      """Checks whether all prerequisites are met for the tool/tagger. It returns a list of
      tools that have not yet been added in the order in which they should be added.

      input: tool_type:                       The type of tagger/tool name that one wishes to set up.
             TrackCollection:                 Requested track association.
             JetCollection:                   Requested jet collection.
             CheckOnlyInsideToolCollection:   Whether to ignore any prerequisites outside the tool collection.
      output: Ordered list of tools needed which are not yet in ToolSvc."""
      # Loop over the tool and all its dependencies and then their dependencies etc.
      to_check = [tool_type,]
      required = []
      while len(to_check) > 0:
          needed = self.checkPrerequisitesOneLevel(to_check[len(to_check)-1],
                                                   IgnoreList=required,
                                                   TrackCollection=TrackCollection,
                                                   JetCollection=JetCollection,
                                                   CheckOnlyInsideToolCollection=CheckOnlyInsideToolCollection)
          to_check.pop()
          to_check += needed
          required += needed
      # Now that we have a list of all tools yet to be set up, we need to make sure it is ordered properly
      # basically tool N's index in this list must be higher than that of all tools it depends on.
      if len(required) < 2:
          # No need to sort.
          return required
      stop_sorting = False
      highest_index = -1
      DeadSwitch = 2**len(required) # this is the maximum amount of permutations ever needed; if surpassed we have a cyclic dependency!
      while not stop_sorting:
          for n in range(len(required)):
              Dependencies = getToolDependencies(required[n])
              highest_index = -1
              for m in range(len(required)-n-1):
                  if required[n+m+1] in Dependencies:
                      if highest_index < n+m+1:
                          highest_index = n+m+1
              if highest_index > -1:
                  # move the current tool to just passed the highest_index
                  required.insert(highest_index+1, required[n])
                  required.pop(n)
                  break;
          if highest_index < 0:
              # we did not need to sort anything; therefore we are done
              stop_sorting = True
          else:
              DeadSwitch -= 1
              if DeadSwitch < 0:
                  print self.BTagTag()+' - ERROR - Dependencies for tool of type "'+tool_type+'" cannot be sorted; possible cyclic dependency!'
                  raise ValueError
      return required

  def checkPrerequisitesOneLevel(self, tool_type, IgnoreList, TrackCollection="", JetCollection="", CheckOnlyInsideToolCollection=False):
      """Checks whether all prerequisites are met for the tool/tagger. It returns a list of
      tools that have not yet been added. Only looks one level down. Users should use the
      checkPrerequisites() function instead.

      input: tool_type:                     The type of tagger/tool that one wishes to set up.
             IgnoreList:                    List of tools/taggers to ignore.
             TrackCollection:               The tracks to run on.
             JetCollection:                 The jet collection to be used.
             CheckOnlyInsideToolCollection: Whether to consider only tools within the tool collection.
      output: Ordered list of tools needed which are not yet in ToolSvc."""
      if (len(TrackCollection) == 0 or len(JetCollection) == 0) and not getToolMetadata(tool_type, 'OneInTotal'):
          print self.BTagTag()+' - ERROR - checkPrerequisitesOneLevel() called without specifying required track/jet collection for a tool does depend on this.'
          raise ValueError
      required = []
      ToolCollDict = getToolCollections()
      InToolCollection = ToolCollDict[getToolCollection(tool_type)]
      Dependencies = getToolDependencies(tool_type)
      for tool in Dependencies:
          if (not tool in IgnoreList) and (not tool in required) and ((not CheckOnlyInsideToolCollection) or (tool in InToolCollection)):
              # Check if this tool has been set up already
              if self._BTaggingConfig_SetUpCombinations.get((tool, JetCollection, TrackCollection), None) is None:
                  required.append(tool)
      return required

  def setupPrerequisites(self, tool_type, ToolSvc, TrackCollection="", JetCollection="", Verbose = False, MuonCollection = 'Muons', ElectronCollection = 'Electrons', PhotonCollection = 'Photons', CheckOnlyInsideToolCollection=False, DoNotSetUpParticleAssociators=False):
      """Adds all tools that are not yet in ToolSvc and yet are needed by a tool of type tool_type
      Such tools are set up using default settings and BTaggingFlags. If this is not desired one
      should set these tools up prior to setting up taggers that depend on them.

      input: tool_type:                     The type of tool that one wishes to set up.
             ToolSvc:                       The ToolSvc.
             TrackCollection:               The tracks to run on.
             JetCollection:                 The jet collection to be used.
             Verbose:                       Whether to print all prerequisites that are being set up.
             MuonCollection:                The muon collection.
             ElectronCollection:            The electron collection.
             PhotonCollection:              The photon collection.
             CheckOnlyInsideToolCollection: Whether to only consider tools in the same tool collection as tool_type when looking at prerequisites.
             DoNotSetUpParticleAssociators: Whether to not set up particle associators if they are needed.
      output: Returns whether anything has been set up."""
      required = self.checkPrerequisites(tool_type, TrackCollection=TrackCollection, JetCollection=JetCollection, CheckOnlyInsideToolCollection=CheckOnlyInsideToolCollection)
      if len(required) < 1:
          return False
      if Verbose:
          print self.BTagTag()+' - DEBUG - '+tool_type+' requires the following tools that have not yet been set up: '+str(required)
          print self.BTagTag()+' - DEBUG - Setting them up now with default settings:'
      for r in range(len(required)):
          if (len(TrackCollection) == 0 or len(JetCollection) == 0) and not getToolMetadata(required[r], 'OneInTotal'):
              print self.BTagTag()+' - ERROR - Setting up a tool without jet/track collection which depends on a tool that does depend on such collections.'
              raise ValueError
          if Verbose:
              print self.BTagTag()+' - DEBUG - Setting up '+required[r]+' for '+TrackCollection+' tracks and the '+JetCollection+' jet collection...'
          # Note that we do not check for prerequisites because we just did so
          self.addTool(required[r], ToolSvc, TrackCollections=TrackCollection,
                       JetCollections=JetCollection, CheckPrerequisites = False, Verbose = Verbose,
                       MuonCollection=MuonCollection, ElectronCollection=ElectronCollection, PhotonCollection=PhotonCollection,
                       DoNotSetUpParticleAssociators=DoNotSetUpParticleAssociators)
      return True

  def addTool(self, tool_type, ToolSvc, TrackCollections=[], JetCollections=[], options={}, Verbose = False, CheckPrerequisites = True, name="", MuonCollection = "Muons", ElectronCollection = "Electrons", PhotonCollection = "Photons", SuppressNonCloneWarning=False, CheckOnlyInsideToolCollection=False, DoNotSetUpParticleAssociators=False):
      """Sets up tools of the specified type for certain jet collections and track associations.

      input: tool_type:          The type of tool to be set up.
             ToolSvc:            The ToolSvc instance.
             TrackCollections:   List of all tracks for which this tool should run. One can also enter a single track association
                                 as a string.
             JetCollections:     List of all jet collections on which this tool should run. One can also enter a single jet
                                 collection as a string.
             options:            Python dictionary of options to be passed to the tool.
             Verbose:            Whether to print detailed information of the tool to be set up.
             CheckPrerequisites: Whether all prerequisite tools should also be automatically set up
                                 using default settings.
             name:               The desired name of the tool. If not specified a default name is used which
                                 is a combination of the prefix and the tool type. Note that it is
                                 generally ill-advised to choose your own name. The reason is that some
                                 prerequisite tools refer to other tools by-name and not via a pointer.
                                 It is possible to break things by forcing your own name. Only do so when
                                 you are sure there will be no conflict or are not using the dependency
                                 system.
                                 You must use this if you want multiple taggers of the same type running on the same jet collection
                                 (or different ones for different jet collections if the metadata is not set accordingly). In this case
                                 you are responsible for setting up the required prerequisites.
                                 If you are sure no additional taggers will be set up in your run after your jobOptions, you can also
                                 just use the changeToolMetadata() and switch the OnePerTrackAssociationAndJetCollection flag on
                                 for your tool. This way you can set up a different one for each jet collection/track collection combination. Note
                                 that you should modify metadata only prior to setting up your tool.
             MuonCollection:     The name of the muon collection to be used. Only used when the tool actually requires such a muon collection.
             ElectronCollection: The name of the electron collection to be used. Only used when the tool actually requires such an electron collection.
             PhotonCollection:   The name of the photon collection to be used. Only used when the tool actually requires a photon collection.

             CheckOnlyInsideToolCollection: Whether to look only in the tool collection for prerequisites.
             DoNotSetUpParticleAssociators: Do not attempt to set up default particle associators if needed but don't exist.

      There is one additional setting. This has to do with the following. If you change the default settings of a tool you might cause confusion
      especially if other clients will use this tool later on (and they don't know about your change). Therefore if you want a tool with non-default
      settings it is generally safer to clone it first because then it will be clear that it is the non-default tool. Setting up a non-cloned
      tool with non-default settings will therefore cause a warning. You can toggle this behavior with the following argument:

             SuppressNonCloneWarning: If True then no warning will be printed is a non-cloned tool is set up without default settings.

      output: The last of the tools having been set up. (For convenience; you can use other functions to retrieve the others if needed)"""
      if not SuppressNonCloneWarning:
          if not getToolMetadata(tool_type, 'IsAClone') is None:
              if len(options) > 0 and (getToolMetadata(tool_type, 'IsAClone') == False):
                  print self.BTagTag()+' - WARNING - You are changing the settings for an instance of '+tool_type+'. It is generally safer to first clone the tool and then use the non-default settings for the clone to avoid any confusion later on. You can also suppress this warning by running addTool with the SuppressNonCloneWarning set to True.'
          elif len(options) > 0:
              print self.BTagTag()+' - WARNING - You are changing the settings for an instance of '+tool_type+'. It is generally safer to first clone the tool and then use the non-default settings for the clone to avoid any confusion later on. You can also suppress this warning by running addTool with the SuppressNonCloneWarning set to True.'
      if type(TrackCollections) == str:
          if len(TrackCollections) == 0:
              TrackCollections = []
          else:
              TrackCollections = [TrackCollections,]
      if type(JetCollections) == str:
          if len(JetCollections) == 0:
              JetCollections = []
          else:
              JetCollections = [JetCollections,]
      # Check if track associations and jet collections are actually lists
      if type(TrackCollections) != list or type(JetCollections) != list:
          print self.BTagTag()+' - ERROR - addTool() requires TrackCollections and JetCollections as lists or strings!'
          raise ValueError
      if ToolSvc is None:
          print self.BTagTag()+' - ERROR - addTool() requires ToolSvc to be specified.'
          raise ValueError
      # First check if a btagtool exists for each of the desired jet collections
      for jetcol in JetCollections:
          self.addJetCollectionTool(jetcol, ToolSvc, Verbose)
      tool = None
      # Special routine for tools of which there is only one (these need no track/jet collections to be specified)
      # and only for the initial set up (for dependency calls track/jetcollections will be specified)
      if len(JetCollections) == 0 or len(TrackCollections) == 0:
          if getToolMetadata(tool_type, 'OneInTotal'):
              if CheckPrerequisites:
                self.setupPrerequisites(tool_type, ToolSvc, Verbose = Verbose, MuonCollection=MuonCollection, ElectronCollection=ElectronCollection,
                                        PhotonCollection=PhotonCollection, CheckOnlyInsideToolCollection=CheckOnlyInsideToolCollection,
                                        DoNotSetUpParticleAssociators=DoNotSetUpParticleAssociators)
              # Check if it already exists
              tool = self.getTool(tool_type)
              if tool is None:
                  tool = self.setupDefaultTool(tool_type, ToolSvc, Verbose=Verbose,
                                               name=name, options=options,
                                               MuonCollection=MuonCollection,
                                               ElectronCollection=ElectronCollection,
                                               PhotonCollection=PhotonCollection)
                  # add it to the empty collection
                  self.addToolToJetCollection(tool_type, tool, jetcol="", track="")
              return tool
          else:
              print self.BTagTag()+' - ERROR - No track/jetcollection specified for tool that does not have the "OneInTotal" metadata.'
              raise ValueError
      # Otherwise run over all the desired tracks and jet collections
      for track in TrackCollections:
          for jetcol in JetCollections:
              # Prerequisites
              if CheckPrerequisites:
                self.setupPrerequisites(tool_type, ToolSvc, TrackCollection=track, JetCollection=jetcol, Verbose = Verbose,
                                        MuonCollection=MuonCollection, ElectronCollection=ElectronCollection,
                                        PhotonCollection=PhotonCollection, CheckOnlyInsideToolCollection=CheckOnlyInsideToolCollection,
                                        DoNotSetUpParticleAssociators=DoNotSetUpParticleAssociators)
              if not DoNotSetUpParticleAssociators:
                # Do the correct track associators exist for this collection of tracks?
                if self.getTrackAssociator(track, jetcol) is None:
                  print self.BTagTag()+' - Track associator not found, setting up default'
                  self.setupTrackAssociator(track, jetcol, ToolSvc, Verbose=Verbose)
                if getToolMetadata(tool_type, 'NeedsElectronAssociator'):
                  if self.getElectronAssociator(ElectronCollection, jetcol) is None:
                    print self.BTagTag()+' - Electron associator not found, setting up default'
                    self.setupElectronAssociator(ElectronCollection, jetcol, ToolSvc, Verbose=Verbose,PhotonCollectionName=PhotonCollection)
                if getToolMetadata(tool_type, 'NeedsMuonAssociator'):
                  if self.getMuonAssociator(MuonCollection, jetcol) is None:
                    print self.BTagTag()+' - Muon associator not found, setting up default'
                    self.setupMuonAssociator(MuonCollection, jetcol, ToolSvc, Verbose=Verbose)
              # Check if it already exists
              tool = self.getTool(tool_type, TrackCollection = track, JetCollection = jetcol)
              if tool is None:
                  # setup the tool
                  tool = self.setupDefaultTool(tool_type, ToolSvc, Verbose=Verbose, track=track,
                                               jetcol=jetcol, name=name, options=options,
                                               MuonCollection=MuonCollection,
                                               ElectronCollection=ElectronCollection,PhotonCollection=PhotonCollection)
              # Now it exists; we need to add it to the required jet collection
              self.addToolToJetCollection(tool_type, tool, jetcol, track)
              self.updateBTaggers(jetcol)
      return tool

  def setupMuonAssociator(self, MuonCollectionName, JetCollection, ToolSvc, options={}, Verbose = False, ContainerName = ""):
      """Sets up a muon associator (and adds it to the ToolSvc).

      input: MuonCollectionName:  Muon collection name. Must be unique.
             JetCollection:       The jet collection name.
             ToolSvc:             The ToolSvc instance.
             options:             Python dictionary with options to be passed to the associator.
             Verbose:             Whether to print the associator settings afterwards.
             ContainerName:       Name of the muon container in SG. If left blank BTaggingFlags.MuonCollectionName will be used.

      output: The tool."""

      if "ParticlesToAssociateList" in options:
          # actually not used???

          print self.BTagTag() + " you have requested to tag the following collections of particles: "
          print options["ParticlesToAssociateList"]
          self._BTaggingConfig_MuonAssociators[(MuonCollectionName, JetCollection)] = 1
          self._BTaggingConfig_MuonConCol[(MuonCollectionName, JetCollection)] = (ContainerName, MuonCollectionName)
          return 1

      options = dict(options)
      options['name'] = self.getToolName('BTagMuonToJetAssociator', MuonCollectionName, JetCollection)
      tool = self.getTool('BTagMuonToJetAssociator', MuonCollectionName, JetCollection)
      if tool is None:
          if Verbose:
              print self.BTagTag()+' - DEBUG - Setting up MuonAssociator for combination: '+str((MuonCollectionName, JetCollection))
          tool = toolBTagMuonToJetAssociator(**options)
          self.registerTool('BTagMuonToJetAssociator', tool, track = MuonCollectionName, jetcol = JetCollection, ToolSvc = ToolSvc)
      else:
          if Verbose:
              print self.BTagTag()+' - DEBUG - Adding MuonAssociator for '+MuonCollectionName+' which already exists now also to jet collection: '+JetCollection
      if self._BTaggingConfig_MuonAssociators.get((MuonCollectionName, JetCollection), None) is None:
          self._BTaggingConfig_MuonAssociators[(MuonCollectionName, JetCollection)] = tool
          if len(ContainerName) == 0:
              ContainerName = BTaggingFlags.MuonCollectionName
          self._BTaggingConfig_MuonConCol[(MuonCollectionName, JetCollection)] = (ContainerName, MuonCollectionName)
          self.updateBTaggers(JetCollection)
      else:
          print self.BTagTag()+' - WARNING - SetupMuonAssociator() called for combination which already exists: '+str((MuonCollectionName,JetCollection))
      return tool

  def setupElectronAssociator(self, ElectronCollectionName, JetCollection, ToolSvc, options={}, Verbose = False, ElectronContainerName = "", PhotonContainerName = "", PhotonCollectionName = "Photons"):
      """Sets up an electron associator (and adds it to the ToolSvc).

      input: ElectronCollectionName: Name of the output electron collection. Should be unique.
             JetCollection:          The jet collection name.
             ToolSvc:                The ToolSvc instance.
             options:                Python dictionary with options to be passed to the associator.
             Verbose:                Whether to print the associator settings afterwards.
             ElectronContainerName:  Name of the electron container in SG. If left blank BTaggingFlags.ElectronCollectionName will be used.
             PhotonContainerName:    Name of the photon container in SG. If left blank BTaggingFlags.PhotonCollectionName will be used.
             PhotonCollectionName:   Name of the output photon collection. If left blank 'Photons' will be used. However this name should be unique
                                     therefore if an electron associator already exists the given name (including the default 'Photons') the name
                                     will instead be 'Photons_JETCOL_ELECCOL', where JETCOL is JetCollection and ELECCOL is ElectronCollection.
                                     A warning will be printed in this case. If this name is also already taken (this means someone is royally
                                     messing things up) an exception will be raised.
      output: The tool."""

      if "ParticlesToAssociateList" in options:
          print self.BTagTag() + " you have requested to tag the following collections of particles: "
          print options["ParticlesToAssociateList"]
          return 1


      options = dict(options)
      options['name'] = self.getToolName('BTagElectronToJetAssociator', ElectronCollectionName, JetCollection)
      tool = self.getTool('BTagElectronToJetAssociator', ElectronCollectionName, JetCollection)
      if tool is None:
          if Verbose:
              print self.BTagTag()+' - DEBUG - Setting up ElectronAssociator for combination: '+str((ElectronCollectionName, JetCollection))
          tool = toolBTagElectronToJetAssociator(**options)
          self.registerTool('BTagElectronToJetAssociator', tool, track = ElectronCollectionName, jetcol = JetCollection, ToolSvc = ToolSvc)
      else:
          if Verbose:
              print self.BTagTag()+' - DEBUG - Adding ElectronAssociator for '+ElectronCollectionName+' which already exists now also to jet collection: '+JetCollection
      if self._BTaggingConfig_ElectronAssociators.get((ElectronCollectionName, JetCollection), None) is None:
          self._BTaggingConfig_ElectronAssociators[(ElectronCollectionName, JetCollection)] = tool
          if len(ElectronContainerName) == 0:
              ElectronContainerName = BTaggingFlags.ElectronCollectionName
          if len(PhotonContainerName) == 0:
              PhotonContainerName = BTaggingFlags.PhotonCollectionName
          # Is the requested photon collection name unique?
          Unique = True
          for x in self._BTaggingConfig_PhotonConCol:
              if x[1] == PhotonCollectionName:
                  Unique = False
          if Unique:
              self._BTaggingConfig_PhotonConCol[(ElectronCollectionName, JetCollection)] = (PhotonContainerName, PhotonCollectionName)
          else:
              PhotonCollectionName = 'Photons_'+JetCollection+'_'+ElectronCollectionName
              print self.BTagTag()+' - WARNING - Photon collection name changed to '+PhotonContainerName+' because the given value was not unique.'
              for x in self._BTaggingConfig_PhotonConCol:
                  if x[1] == PhotonCollectionName:
                      print self.BTagTag()+' - ERROR - Photon collection name is not unique and default is also in use.'
                      raise ValueError
              self._BTaggingConfig_PhotonConCol[(ElectronCollectionName, JetCollection)] = (PhotonContainerName, PhotonCollectionName)
          self.updateBTaggers(JetCollection)
      else:
          print self.BTagTag()+' - WARNING - SetupElectronAssociator() called for combination which already exists: '+str((ElectronCollectionName,JetCollection))
      return tool

  def setupTrackAssociator(self, TrackCollection, JetCollection, ToolSvc, options={}, Verbose = False, ContainerName = ""):
      """Sets up a track associator (and adds it to the ToolSvc).

      input: TrackCollection:        The track association for which to set up the associator.
             JetCollection:          The jet collection name.
             ToolSvc:                The ToolSvc instance.
             options:                Python dictionary with options to be passed to the associator.
             Verbose:                Whether to print the associator settings afterwards.
             ContainerName:          Name of the track container in SG. If left blank BTaggingFlags.TrackParticleCollectionName will be used.
      output: The tool."""
      options = dict(options)
      options['name'] = self.getToolName('BTagTrackToJetAssociator', TrackCollection, JetCollection)

      if "ParticlesToAssociateList" in options:
          print self.BTagTag() + " you have requested to tag the following collections of particles: "
          print options["ParticlesToAssociateList"]
          options.pop("ParticlesToAssociateList")


      if "TracksToTagList" in options:
          print self.BTagTag() + " you have requested to tag the following collections of tracks: "
          print options["TracksToTagList"]
          return None



      tool = self.getTool('BTagTrackToJetAssociator', TrackCollection, JetCollection)
      if tool is None:
          if Verbose:
              print self.BTagTag()+' - DEBUG - Setting up TrackAssociator for combination: '+str((TrackCollection, JetCollection))
          tool = toolBTagTrackToJetAssociator(**options)
          self.registerTool('BTagTrackToJetAssociator', tool, track = TrackCollection, jetcol = JetCollection, ToolSvc = ToolSvc)
      else:
          if Verbose:
              print self.BTagTag()+' - INFO - Adding TrackAssociator for '+TrackCollection+' which already exists now also to jet collection: '+JetCollection
      if self._BTaggingConfig_TrackAssociators.get((TrackCollection, JetCollection), None) is None:
          self._BTaggingConfig_TrackAssociators[(TrackCollection, JetCollection)] = tool
          if len(ContainerName) == 0:
              ContainerName = BTaggingFlags.TrackParticleCollectionName
          self._BTaggingConfig_TrackConCol[(TrackCollection, JetCollection)] = (ContainerName, TrackCollection)
          self.updateBTaggers(JetCollection)
      else:
          print self.BTagTag()+' - WARNING - SetupTrackAssociator() called for combinations which already exists: '+str((TrackCollection,JetCollection))
      return tool

  def getMuonAssociatorData(self, JetCollection):
      """Returns a list of tuples containing data needed for the specified jet collection. The following format is used:
      (tool, MuonContainerName, MuonCollectionName)."""
      returnlist = []
      for assoc in self._BTaggingConfig_MuonAssociators:
          if JetCollection == assoc[1]:
              x = [self._BTaggingConfig_MuonAssociators[assoc],]
              for concol in self._BTaggingConfig_MuonConCol:
                  if concol[1] == JetCollection and concol[0] == assoc[0]:
                      x += [self._BTaggingConfig_MuonConCol[concol][0], self._BTaggingConfig_MuonConCol[concol][1] ]
              if len(x) == 3:
                  returnlist.append(x)
              else:
                  print self.BTagTag()+' - ERROR - getMuonAssociatorData() does not find enough metadata for jet collection: '+JetCollection
                  raise ValueError
      return returnlist

  def getElectronAssociatorData(self, JetCollection):
      """Returns a list of tuples containing data needed for the specified jet collection. The following format is used:
      (tool, ElectronContainerName, ElectronCollectionName, PhotonContainerName, PhotonCollectionName)."""
      returnlist = []
      for assoc in self._BTaggingConfig_ElectronAssociators:
          if JetCollection == assoc[1]:
              x = [self._BTaggingConfig_ElectronAssociators[assoc],]
              for concol in self._BTaggingConfig_ElectronConCol:
                  if concol[1] == JetCollection and concol[0] == assoc[0]:
                      x += [self._BTaggingConfig_ElectronConCol[concol][0], self._BTaggingConfig_ElectronConCol[concol][1] ]
              for concol in self._BTaggingConfig_PhotonConCol:
                  if concol[1] == JetCollection and concol[0] == assoc[0]:
                      x += [self._BTaggingConfig_PhotonConCol[concol][0], self._BTaggingConfig_PhotonConCol[concol][1] ]
              if len(x) == 5:
                  returnlist.append(x)
              else:
                  print self.BTagTag()+' - ERROR - getElectronAssociatorData() does not find enough metadata for jet collection: '+JetCollection
                  raise ValueError
      return returnlist

  def getTrackAssociatorData(self, JetCollection):
      """Returns a list of tuples containing data needed for the specified jet collection. The following format is used:
      (tool, TrackContainerName, TrackCollectionName)."""
      returnlist = []
      for assoc in self._BTaggingConfig_TrackAssociators:
          if JetCollection == assoc[1]:
              x = [self._BTaggingConfig_TrackAssociators[assoc],]
              for concol in self._BTaggingConfig_TrackConCol:
                  if concol[1] == JetCollection and concol[0] == assoc[0]:
                      x += [self._BTaggingConfig_TrackConCol[concol][0], self._BTaggingConfig_TrackConCol[concol][1] ]
              if len(x) == 3:
                  returnlist.append(x)
              else:
                  print self.BTagTag()+' - ERROR - getTrackAssociatorData() does not find enough metadata for jet collection: '+JetCollection
                  raise ValueError
      return returnlist

  def getMuonAssociatorAttribute(self, attribute, MuonCollection, JetCollection, RaiseException=True):
      """Returns an attribute of a muon associator. Technically the same can be returned by first getting a reference
      to the tool via a getMuonAssociator() call and then just accessing the attribute directly. However at some point
      getTool will only return a PublicToolHandle and its attributes cannot be retrieved. At that point this
      function can be useful.

      Returns None if the tool does not exist or the requested attribute does not exist. If RaiseException is True
      an AttributeError is raised when the attribute does not exist."""
      tool = self.getMuonAssociator(MuonCollection, JetCollection)
      if tool is None:
        return None
      if not hasattr(tool, attribute) and RaiseException:
        print self.BTagTag()+' - ERROR - Attribute "'+attribute+'" does not exist for a muon associator.'
        raise AttributeError
      return getattr(tool, attribute, None)

  def getMuonAssociator(self, MuonCollection, JetCollection):
      """Returns the muon associator for the specified muon and jet collection combination."""
      return self._BTaggingConfig_MuonAssociators.get((MuonCollection, JetCollection), None)

  def getElectronAssociatorAttribute(self, attribute, ElectronCollection, JetCollection, RaiseException=True):
      """Returns an attribute of an electron associator. Technically the same can be returned by first getting a reference
      to the tool via a getElectronAssociator() call and then just accessing the attribute directly. However at some point
      getTool will only return a PublicToolHandle and its attributes cannot be retrieved. At that point this
      function can be useful.

      Returns None if the tool does not exist or the requested attribute does not exist. If RaiseException is True
      an AttributeError is raised when the attribute does not exist."""
      tool = self.getElectronAssociator(ElectronCollection, JetCollection)
      if tool is None:
        return None
      if not hasattr(tool, attribute) and RaiseException:
        print self.BTagTag()+' - ERROR - Attribute "'+attribute+'" does not exist for an electron associator.'
        raise AttributeError
      return getattr(tool, attribute, None)

  def getElectronAssociator(self, ElectronCollection, JetCollection):
      """Returns the electron associator for the specified electron and jet collection combination."""
      return self._BTaggingConfig_ElectronAssociators.get((ElectronCollection, JetCollection), None)

  def getTrackAssociatorAttribute(self, attribute, TrackCollection, JetCollection, RaiseException=True):
      """Returns an attribute of a track associator. Technically the same can be returned by first getting a reference
      to the tool via a getTrackAssociator() call and then just accessing the attribute directly. However at some point
      getTool will only return a PublicToolHandle and its attributes cannot be retrieved. At that point this
      function can be useful.

      Returns None if the tool does not exist or the requested attribute does not exist. If RaiseException is True
      an AttributeError is raised when the attribute does not exist."""
      tool = self.getTrackAssociator(TrackCollection, JetCollection)
      if tool is None:
        return None
      if not hasattr(tool, attribute) and RaiseException:
        print self.BTagTag()+' - ERROR - Attribute "'+attribute+'" does not exist for a track associator.'
        raise AttributeError
      return getattr(tool, attribute, None)

  def getTrackAssociator(self, TrackCollection, JetCollection):
      """Returns the track associator for the specified track and jet collection combination."""
      return self._BTaggingConfig_TrackAssociators.get((TrackCollection, JetCollection), None)

  def addToolToJetCollection(self, tool_type, tool, jetcol, track):
      """Adds a tool to a jet collection if not done so already.

      input: tool_type:          The type of the tool to add.
             tool:               The actual tool.
             jetcol:             The jet collection.
             track:              The track selection this tool is running on."""
      if (tool_type, jetcol, track) in self._BTaggingConfig_SetUpCombinations:
          print self.BTagTag()+' - WARNING - A tool of type "'+tool_type+'" was already set up for the combination '+str((track, jetcol))+' ignoring duplicate...'
          return
      if getToolMetadata(tool_type, 'IsATagger'):
          btagger = self.getJetCollectionTool(jetcol)
          btagger.TagToolList += [tool,]
      if getToolMetadata(tool_type, 'IsAVertexFinder'):
          # now we need the removal and secvtxfinder tools
          SecVertexingTool = self.getJetCollectionSecVertexingTool(jetcol)
          # register our new vertex finder to them
          SecVertexingTool.SecVtxFinderList += [tool,]
          SecVertexingTool.SecVtxFinderTrackNameList += [track,]
          if not getToolMetadata(tool_type, 'VertexFinderxAODBaseName'):
              print self.BTagTag()+' - ERROR - Tool '+self.getToolName(tool_type, track, jetcol)+' is set as a vertex finder, but no VertexFinderxAODBaseName was specified in the metadata.'
              raise ValueError
          SecVertexingTool.SecVtxFinderxAODBaseNameList += [self.getVertexFinderxAODBaseName() + getToolMetadata(tool_type, 'VertexFinderxAODBaseName'), ]
          VertexFinderPassByPointer = getToolMetadata(tool_type, 'VertexFinderPassByPointer')
          if VertexFinderPassByPointer:
              for element in VertexFinderPassByPointer:
                  ToPass = self._BTaggingConfig_InitializedTools.get(self.getToolName(VertexFinderPassByPointer[element],track,jetcol), None)
                  if ToPass is None:
                      print self.BTagTag()+' - ERROR - Tool of type '+VertexFinderPassByPointer[element]+' to be passed as pointer to tool of type '+tool_type+' to the SecVertexingTool'
                      print self.BTagTag()+' - ERROR - but this tool was not found/registered.'
                      raise ValueError
                  setattr(SecVertexingTool, element, ToPass)
      self._BTaggingConfig_SetUpCombinations[(tool_type, jetcol, track)] = tool
      if BTaggingFlags.OutputLevel < 3:
          print self.BTagTag()+' - DEBUG - Added tool '+self.getToolName(tool_type, track, jetcol)+' to jet collection: '+jetcol
      # Add to the tool also the jet collection (needed for reference running)
      jetcollist = getToolMetadata(tool_type, 'JetCollectionList')
      if jetcollist:
          if not jetcol in getattr(tool, jetcollist):
              if BTaggingFlags.OutputLevel < 3:
                  print self.BTagTag()+' - DEBUG - Adding to tool '+self.getToolName(tool_type, track, jetcol)+' the jet collection: '+jetcol
              getattr(tool, jetcollist).append(jetcol)
      if BTaggingFlags.OutputLevel < 3:
          print self.BTagTag()+' - DEBUG - '+str((tool_type, jetcol, track))+' -> '+self.getToolName(tool_type, track, jetcol)+' added'

  def getJetCollectionMainAssociatorAttribute(self, attribute, JetCollection, RaiseException=True):
      """Returns an attribute of a main associator. Technically the same can be returned by first getting a reference
      to the tool via a getJetCollectionMainAssociatorTool() call and then just accessing the attribute directly.
      However at some point getTool will only return a PublicToolHandle and its attributes cannot be retrieved. At that point this
      function can be useful.

      Returns None if the tool does not exist or the requested attribute does not exist. If RaiseException is True
      an AttributeError is raised when the attribute does not exist."""
      tool = self.getJetCollectionMainAssociatorTool(JetCollection)
      if tool is None:
        return None
      if not hasattr(tool, attribute) and RaiseException:
        print self.BTagTag()+' - ERROR - Attribute "'+attribute+'" does not exist for a main associator tool.'
        raise AttributeError
      return getattr(tool, attribute, None)

  def getJetCollectionMainAssociatorTool(self, jetcol):
      """Returns the main associator tool corresponding to the specified jet collection.

      input: jetcol:             Desired jet collection.

      output: The tool."""
      tool = self._BTaggingConfig_MainAssociatorTools.get(jetcol, None)
      if tool is None:
          print self.BTagTag()+' - ERROR - getJetCollectionMainAssociatorTool() returns none for '+jetcol
          raise ValueError
      return tool

  def getJetCollectionSecVertexingToolAttribute(self, attribute, JetCollection, RaiseException=True):
      """Returns an attribute of a secondary vertexing tool. Technically the same can be returned by first getting a reference
      to the tool via a getJetCollectionSecVertexingTool() call and then just accessing the attribute directly.
      However at some point getTool will only return a PublicToolHandle and its attributes cannot be retrieved. At that point this
      function can be useful.

      Returns None if the tool does not exist or the requested attribute does not exist. If RaiseException is True
      an AttributeError is raised when the attribute does not exist."""
      tool = self.getJetCollectionSecVertexingTool(JetCollection)
      if tool is None:
        return None
      if not hasattr(tool, attribute) and RaiseException:
        print self.BTagTag()+' - ERROR - Attribute "'+attribute+'" does not exist for a secondary vertexing tool.'
        raise AttributeError
      return getattr(tool, attribute, None)

  def getJetCollectionSecVertexingTool(self, jetcol):
      """Returns the SecVtx tool corresponding to the specified jet collection.

      input: jetcol:             Desired jet collection.

      output: The tool."""
      tool = self._BTaggingConfig_SecVtxTools.get(jetcol, None)
      if tool is None:
          print self.BTagTag()+' - ERROR - getJetCollectionSecVertexingTool() returns none for '+jetcol
          raise ValueError
      return tool

  def jetCollectionExists(self, JetCollection):
      """Returns whether a jet collection already has some configuration; in particular whether it has an assigned BTagTool."""
      if self._BTaggingConfig_JetCollections.get(JetCollection, None) is None:
          return False
      return True

  def getJetCollectionToolAttribute(self, attribute, JetCollection, RaiseException=True):
      """Returns an attribute of a BTagTool. Technically the same can be returned by first getting a reference
      to the tool via a getJetCollectionTool() call and then just accessing the attribute directly.
      However at some point getTool will only return a PublicToolHandle and its attributes cannot be retrieved. At that point this
      function can be useful.

      Returns None if the tool does not exist or the requested attribute does not exist. If RaiseException is True
      an AttributeError is raised when the attribute does not exist."""
      tool = self.getJetCollectionTool(JetCollection)
      if tool is None:
        return None
      if not hasattr(tool, attribute) and RaiseException:
        print self.BTagTag()+' - ERROR - Attribute "'+attribute+'" does not exist for a BTagTool.'
        raise AttributeError
      return getattr(tool, attribute, None)

  def getJetCollectionTool(self, JetCollection):
      """Returns the btagtool corresponding to the specified jet collection.

      input: JetCollection:             Desired jet collection.

      output: The btagtool."""
      btagger = self._BTaggingConfig_JetCollections.get(JetCollection, None)
      if btagger is None:
          if BTaggingFlags.OutputLevel < 3:
              print self.BTagTag()+' - DEBUG - getJetCollectionTool() returns none for '+JetCollection
      return btagger

  def RegisterOutputContainersForJetCollection(self, JetCollection, Verbose=False):
      """Registers the jet collection to various containers in BTaggingFlags which govern which
      containers will be parsed to the output xAOD and ESD files. This used to happen in
      ./share/BTagging_jobOptions.py.

      input: JetCollection:       The name of the jet collection.
             Verbose:             Print a debug message."""
      author = self.getOutputFilesPrefix() + JetCollection # Get correct name with prefix
      if not (self._OutputFilesBaseName + author) in BTaggingFlags.btaggingAODList:
        BTaggingFlags.btaggingAODList.append(self._OutputFilesBaseName + author)
      if not (self._OutputFilesBaseAuxName + author + 'Aux.-BTagTrackToJetAssociatorBB') in BTaggingFlags.btaggingAODList:
        BTaggingFlags.btaggingAODList.append(self._OutputFilesBaseAuxName + author + 'Aux.-BTagTrackToJetAssociatorBB')
      if not (self._OutputFilesBaseName + author) in BTaggingFlags.btaggingESDList:
        BTaggingFlags.btaggingESDList.append(self._OutputFilesBaseName + author)
      if not (self._OutputFilesBaseAuxName + author + 'Aux.-BTagTrackToJetAssociatorBB') in BTaggingFlags.btaggingESDList:
        BTaggingFlags.btaggingESDList.append(self._OutputFilesBaseAuxName + author + 'Aux.-BTagTrackToJetAssociatorBB')
      # SeCVert
      if not (self._OutputFilesBaseNameSecVtx + author + self._OutputFilesSVname) in BTaggingFlags.btaggingAODList:
        BTaggingFlags.btaggingAODList.append(self._OutputFilesBaseNameSecVtx + author + self._OutputFilesSVname)
      if not (self._OutputFilesBaseAuxNameSecVtx + author + self._OutputFilesSVname + 'Aux.-vxTrackAtVertex') in BTaggingFlags.btaggingAODList:
        BTaggingFlags.btaggingAODList.append(self._OutputFilesBaseAuxNameSecVtx + author + self._OutputFilesSVname + 'Aux.-vxTrackAtVertex')
      if not (self._OutputFilesBaseNameSecVtx + author + self._OutputFilesSVname) in BTaggingFlags.btaggingESDList:
        BTaggingFlags.btaggingESDList.append(self._OutputFilesBaseNameSecVtx + author + self._OutputFilesSVname)
      if not (self._OutputFilesBaseAuxNameSecVtx + author + self._OutputFilesSVname + 'Aux.-vxTrackAtVertex') in BTaggingFlags.btaggingESDList:
        BTaggingFlags.btaggingESDList.append(self._OutputFilesBaseAuxNameSecVtx + author + self._OutputFilesSVname + 'Aux.-vxTrackAtVertex')
      # JFSeCVert
      if not (self._OutputFilesBaseNameJFSecVtx + author + self._OutputFilesJFVxname) in BTaggingFlags.btaggingAODList:
        BTaggingFlags.btaggingAODList.append(self._OutputFilesBaseNameJFSecVtx + author + self._OutputFilesJFVxname)
      if not (self._OutputFilesBaseAuxNameJFSecVtx + author + self._OutputFilesJFVxname + 'Aux.') in BTaggingFlags.btaggingAODList:
        BTaggingFlags.btaggingAODList.append(self._OutputFilesBaseAuxNameJFSecVtx + author + self._OutputFilesJFVxname + 'Aux.')
      if not (self._OutputFilesBaseNameJFSecVtx + author + self._OutputFilesJFVxname) in BTaggingFlags.btaggingESDList:
        BTaggingFlags.btaggingESDList.append(self._OutputFilesBaseNameJFSecVtx + author + self._OutputFilesJFVxname)
      if not (self._OutputFilesBaseAuxNameJFSecVtx + author + self._OutputFilesJFVxname + 'Aux.') in BTaggingFlags.btaggingESDList:
        BTaggingFlags.btaggingESDList.append(self._OutputFilesBaseAuxNameJFSecVtx + author + self._OutputFilesJFVxname + 'Aux.')
      if Verbose:
          print(self.BTagTag()+" - DEBUG - Added jet collection '"+JetCollection+"' to BTaggingFlags.btaggingAODList and BTaggingFlags.btaggingESDList.")

  def addJetCollectionTool(self, JetCollection, ToolSvc, Verbose = False, options={}):
      """Adds a jet collection, and sets up the corresponding BTagTool and secondary vertexing tools.
      If it already exists it is simply returned.

      input: JetCollection:      The name of the jet collections.
             ToolSvc:            The ToolSvc instance.
             Verbose:            Whether to print detailed information about the tool.
             options:            Python dictionary of options to be passed to the BTagTool.
      output: The btagtool for thhe desired jet collection."""
      if self._BTaggingConfig_JetCollections.get(JetCollection, None) is None:
          if not JetCollectionIsSupported(JetCollection):
              print(self.BTagTag()+" - WARNING - "+JetCollection+" is not a supported jet collection for b-tagging! Some taggers may crash!")
          btagtool = self.setupBTagTool(JetCollection, ToolSvc, Verbose = Verbose, options=options)
          if btagtool:
              self.RegisterOutputContainersForJetCollection(JetCollection, Verbose)
              if (JetCollection == "AntiKt4EMPFlow"):
                self.RegisterOutputContainersForJetCollection(JetCollection+"_PFlowTune", Verbose)
              
              if not JetCollection in BTaggingFlags.Jets:
                  BTaggingFlags.Jets += [JetCollection, ]
          return btagtool
      else:
          # The tool already exists, we just return it
          if len(options) > 0:
              print self.BTagTag()+' - WARNING - A JetCollection called "'+JetCollection+'" has already been set up; ignoring new btagtool options.'
          return self._BTaggingConfig_JetCollections.get(JetCollection, None)

  def setupSecVtxTool(self, name, ToolSvc, Verbose = False, options={}):
      """Adds a SecVtxTool instance and registers it.

      input: name:               The tool's name.
             ToolSvc:            The ToolSvc instance.
             Verbose:            Whether to print detailed information about the tool.
             options:            Python dictionary of options to be passed to the SecVtxTool.
      output: The tool."""
      options = dict(options)
      options.setdefault('SecVtxFinderList', [])
      options.setdefault('SecVtxFinderTrackNameList', [])
      options.setdefault('SecVtxFinderxAODBaseNameList', [])
      options.setdefault('PrimaryVertexName',BTaggingFlags.PrimaryVertexCollectionName)
      options.setdefault('OutputLevel', BTaggingFlags.OutputLevel)
      options['name'] = name
      from BTagging.BTaggingConf import Analysis__BTagSecVertexing
      tool = Analysis__BTagSecVertexing(**options)
      ToolSvc += tool
      return tool

  def setupBTagTool(self, jetcol, ToolSvc, Verbose = False, options={}):
      """Adds a new myBTagTool instance and registers it.

      input: jetcol:             The name of the jet collections.
             ToolSvc:            The ToolSvc instance.
             Verbose:            Whether to print detailed information about the tool.
             options:            Python dictionary of options to be passed to the BTagTool.
             (note the options storeSecondaryVerticesInJet is passed to the removal tool instead)

      The following default options exist:

      BTagLabelingTool                       default: None
      storeSecondaryVerticesInJet            default: BTaggingFlags.writeSecondaryVertices

      output: The btagtool for the desired jet collection."""
      options = dict(options)
      # Setup a removal tool for it
      options.setdefault('storeSecondaryVerticesInJet', BTaggingFlags.writeSecondaryVertices)
      thisSecVtxTool = self.setupSecVtxTool('thisBTagSecVertexing_'+jetcol+self.GeneralToolSuffix(), ToolSvc, Verbose)
      self._BTaggingConfig_SecVtxTools[jetcol] = thisSecVtxTool
      #options['BTagSecVertexingTool'] = thisSecVtxTool # MOVED TO JETBTAGGERTOOL
      del options['storeSecondaryVerticesInJet'] # we don't want it passed to the main b-tag tool
      options['name'] = 'myBTagTool_'+jetcol+self.GeneralToolSuffix()
      options.setdefault('BTagLabelingTool', None)
      btagtool = toolMainBTaggingTool(**options)
      if BTaggingFlags.OutputLevel < 3:
          print self.BTagTag()+' - DEBUG - Setting up BTagTool for jet collection: '+jetcol
      if self._BTaggingConfig_JetCollections.get(jetcol, None) is None:
          # Store this one to the jet collections
          self._BTaggingConfig_JetCollections[jetcol] = btagtool
          ToolSvc += btagtool
      else:
          print self.BTagTag()+' - ERROR - Setting up BTagTool for a jet collection which was already set up previously!'
          raise ValueError
      return btagtool

  def checkFlagsUsingBTaggingFlags(self):
      """Runs over all taggers and disables them if various detector components were switched off, we have a specific scoping level or running in reference mode.

      output: Returns whether b-tagging is to be active or not."""
      if not BTaggingFlags.Active:
          print '#BTAG# - WARNING - Disabling whole b-tagging since BTaggingFlags.Active is False...'
          return False
      print '#BTAG# - INFO - B-tagging descoping:  level = ',rec.ScopingLevel()
      from AthenaCommon.BFieldFlags import jobproperties
      if not jobproperties.Beam.beamType == "collisions":
          print '#BTAG# - WARNING - Disabling whole b-tagging since beamType is not collisions...'
          BTaggingFlags.Active = False
          return False
      if not self._IgnoreTaggerObstacles:
          for tagger in BTaggingFlags._tags:
              if not taggerIsPossible(tagger):
                  BTaggingFlags.Disable(tagger)
      else:
          print '#BTAG# - INFO - Not checking for obstacles for taggers (DetFlags etc.).'
      return True

  def setupDefaultTool(self, tool_type, ToolSvc, track="", jetcol="", Verbose = False, name="", options={}, MuonCollection='Muons', ElectronCollection='Electrons',PhotonCollection='Photons'):
      """Sets up the specified tool (by tool_type) using default configuration.

      input: tool_type:          The type of the tool to be set up.
             ToolSvc:            The ToolSvc (enter None if you don't want to register the tool).
             track:              The name of the tracks to be used.
             jetcol:             The name of the jet collection to be used.
             Verbose:            Whether to print the tool and all those that are pre set up.
             CheckPrerequisites: Whether to set up default prerequisites as well (if needed).
             Prefix:             Prefix to be used for this tool and all tools set up by default.
             name:               The name of the tool. If not specified a default name is used which
                                 is a combination of the prefix and the tool type. Note that it is
                                 generally ill-advised to choose your own name. The reason is that some
                                 prerequisite tools refer to other tools by-name and not via a pointer.
                                 It is possible to break things by forcing your own name. Only do so when
                                 you are sure there will be no conflict or are not using the dependency
                                 system.
             options:            Python dictionary (!) with options that will be passed to the tool,
                                 options for which there is no key in the dictionary will use defaults.
             MuonCollection:     Muon collection. Can be left blank if not needed or using defaults.
             ElectronCollection: Electron collection. Can be left blank if not needed or using defaults.
             PhotonCollection:   Photon collection. Can be left blank if not needed or using defaults.
      output: The tool itself."""
      # This is to hold the arguments that need to be passed to the b-tagging tool,
      # this is needed because the tool will only be added to ToolSvc at the end and
      # it is not allowed to add non-ToolSvc tools to the b-tagging tool.
      if len(name) < 1:
          name = self.getToolName(tool_type, track, jetcol)
      options = dict(options) # Make a new instance (otherwise we will change the actual dict)
      options['name'] = name
      # xAODBaseName
      if not getToolMetadata(tool_type, 'xAODBaseName') is None:
          options.setdefault('xAODBaseName', self.getxAODBaseNamePrefix() + getToolMetadata(tool_type, 'xAODBaseName'))
      # Add some dynamic settings to the options dictionary (this will raise a NotImplementedError is metadata is not found)
      PassByPointer = getToolMetadata(tool_type, 'PassByPointer')
      PassByName = getToolMetadata(tool_type, 'PassByName')
      PassTracksAs = getToolMetadata(tool_type, 'PassTracksAs')
      CalibrationFolders = getToolMetadata(tool_type, 'CalibrationFolders')
      PassMuonCollectionAs = getToolMetadata(tool_type, 'PassMuonCollectionAs')
      PassElectronCollectionAs = getToolMetadata(tool_type, 'PassElectronCollectionAs')
      PassPhotonCollectionAs = getToolMetadata(tool_type, 'PassPhotonCollectionAs')
      if PassMuonCollectionAs:
          options.setdefault(PassMuonCollectionAs, MuonCollection)
      if PassElectronCollectionAs:
          options.setdefault(PassElectronCollectionAs, ElectronCollection)
      if PassPhotonCollectionAs:
          options.setdefault(PassPhotonCollectionAs, PhotonCollection)
      if PassByPointer:
          for element in PassByPointer:
              # Find the tool
              ToPass = self._BTaggingConfig_InitializedTools.get(self.getToolName(PassByPointer[element],track,jetcol), None)
              if ToPass is None and not element in options: # allow for options overwrite
                  print self.BTagTag()+' - ERROR - Tool of type '+PassByPointer[element]+' to be passed as pointer to tool of type '+tool_type
                  print self.BTagTag()+' - ERROR - but this tool was not found/registered. (Can be overwritten using options passed to the set up script).'
                  raise ValueError
              options.setdefault(element, ToPass)
      if PassByName:
          for element in PassByName:
              # No check on if these tools actually exist; PassByName is commonly used to link to tools that have yet to be set up
              options.setdefault(element, self.getToolName(PassByName[element],track,jetcol))
      if PassTracksAs:
          if len(track) == 0:
              print self.BTagTag()+' - ERROR - setupDefaultTool() called for a tool that has PassTracksAs metadata but without specifying a track collection.'
              raise ValueError
          options.setdefault(PassTracksAs, track)
      if CalibrationFolders:
          if BTaggingFlags.CalibrationSingleFolder is False:
              print self.BTagTag()+' - ERROR - BTaggingFlags.CalibrationSingleFolder is False, the new calibration scheme does not support this!'
              print self.BTagTag()+' - ERROR - Calibrations will not function!'
              raise ValueError
          else:
              broker = self._BTaggingConfig_InitializedTools.get(self.getToolName('BTagCalibrationBrokerTool',track,jetcol), None)
              if broker is None:
                  print self.BTagTag()+' - ERROR - CalibrationBroker not found; calibrations will not function!'
                  raise ValueError
              elif not hasattr(broker, 'folders'):
                  print self.BTagTag()+' - ERROR - CalibrationBroker does not have "folders" as an attribute; calibrations will not function!'
                  raise ValueError
              for folder in CalibrationFolders:
                  ToAdd = broker.folderRoot + folder
                  if not ToAdd in broker.folders:
                      broker.folders.append(ToAdd)
      # Set up the actual tool
      try:
          exec('tool = tool'+tool_type+'(**options)')
      except:
          print self.BTagTag()+' - ERROR - Attempting to set up a tool of type "'+tool_type+'" but this tool was not/incorrectly implemented.'
          raise
      # Fill in jet collection lists if applicable
      jetcollist = getToolMetadata(tool_type, 'JetCollectionList')
      if jetcollist:
          setattr(tool, jetcollist, [jetcol,])
      # Register it with the dependency system
      self.registerTool(tool_type, tool, track, jetcol, ToolSvc = ToolSvc, Verbose = Verbose)
      return tool

########################################
# BTaggingFlags manipulation           #
########################################

def listTaggers():
    """Returns a list of all taggers."""
    return BTaggingFlags._tags

def disableAllTaggers():
    """Disables all BTaggingFlags that are tagger flags."""
    for tagger in BTaggingFlags._tags:
        BTaggingFlags.Disable(tagger)

def getActiveTaggers():
    """Returns a list of taggers that are active according to the BTaggingFlags module."""
    returnlist = []
    for tagger in BTaggingFlags._tags:
        if(BTaggingFlags.IsEnabled(tagger)):
            returnlist.append(tagger)
    return returnlist

def taggerIsPossible(tagger):
    """Returns whether a tagger can be used depending on various criteria."""
    # disable tagger is it is unknown
    if not tagger in BTaggingFlags._tags:
        print '#BTAG# - WARNING - Unknown tagger ('+tagger+') encountered, disabling...'
        return False
    # disable all taggers if B-tagging is not active
    if not BTaggingFlags.Active:
        print '#BTAG# - WARNING - Disabling '+tagger+' tagger because B-tagging has been disabled...'
        return False
    # disable some taggers if we are running in reference mode
    if BTaggingFlags.Runmodus == 'reference':
        if tagger in ['lifetime1D',
                      'lifetime2D',
                      'lifetime3D',
                      'secVtxFitTD',
                      'secVtxFitBU',
                      'JetProbFlip',
                      'IP2DFlip',
                      'IP2DPos',
                      'IP2DNeg',
                      'IP2DSpcFlip',
                      'IP2DSpcPos',
                      'IP2DSpcNeg',
                      'IP3DFlip',
                      'IP3DPos',
                      'IP3DNeg',
                      'IP3DSpcFlip',
                      'IP3DSpcPos',
                      'IP3DSpcNeg',
                      'SV1Flip',
                      'SV2Flip',
                      'TrackCountingFlip',
                      'MultiSVTag',
                      'DL1',
                      'DL1Flip',
                      'MV1',
                      'MV2',
                      'MV1Flip',
                      'MV2Flip']:
            print '#BTAG# - WARNING - Disabling '+tagger+' tagger due to reference run modus...'
            return False
    # disable taggers accordingly to reconstruction scoping levels
    if rec.ScopingLevel() >= 2:
        if tagger in BTaggingFlags.LowPriorityTaggers:
            print '#BTAG# - WARNING - Disabling low priority '+tagger+' tagger due to scoping level >= 2'
            return False
    if rec.ScopingLevel() >= 3:
        if tagger in BTaggingFlags.MediumPriorityTaggers:
            print '#BTAG# - WARNING - Disabling medium priority '+tagger+' tagger due to scoping level >= 3'
            return False
    if rec.ScopingLevel() >= 4:
        if tagger in BTaggingFlags.HighPriorityTaggers:
            print '#BTAG# - WARNING - Disabling high priority '+tagger+' tagger due to scoping level >= 4'
            return False
    # disable specific taggers if geometry is not defined
    from AthenaCommon.DetFlags import DetFlags
    if not DetFlags.detdescr.ID_on():
        print '#BTAG# - WARNING - Disabling '+tagger+' tagger because ID detdescr. was not found'
        return False
    if tagger in ['SoftEl']:
        if not DetFlags.detdescr.Calo_on():
            print '#BTAG# - WARNING - Disabling '+tagger+' tagger because calo detdescr. was not found'
            return False
    if tagger in ['SoftMu',
                  'SoftMuChi2',
                  'SecondSoftMu',
                  'SecondSoftMuChi2']:
        if not DetFlags.detdescr.Muon_on():
            print '#BTAG# - WARNING - Disabling '+tagger+' tagger because muon detdescr. was not found'
            return False
    return True

########################################
# Tool collection & metadata functions #
########################################

def getToolDependencies(tool_type):
  """Returns a list of all dependencies of a tool with type tool_type."""
  Dependencies = getToolMetadata(tool_type, 'DependsOn')
  if Dependencies:
    return Dependencies
  else:
    return []

def addToolTypeToToolCollection(tool_type, ToolCollection):
    """Adds a given tool_type to a given tool collection. This also sets the metadata on the tool accordingly for consistency."""
    global _BTaggingConfig_ToolCollections
    changeToolMetadata(tool_type, "ToolCollection", ToolCollection)
    if ToolCollection in _BTaggingConfig_ToolCollections:
        _BTaggingConfig_ToolCollections[ToolCollection] += [tool_type, ]
    else:
        _BTaggingConfig_ToolCollections[ToolCollection] = [tool_type, ]

def getToolCollection(tool_type):
    """Returns the name of the tool collection the current tool is in."""
    return getToolMetadata(tool_type, "ToolCollection")

def getToolCollectionSize(ToolCollection):
    """Returns the size of a given tool collection. Returns 0 if it does not exist."""
    if ToolCollection in _BTaggingConfig_ToolCollections:
        return len(_BTaggingConfig_ToolCollections[ToolCollection])
    else:
        return 0

def checkToolCollectionStructure(CheckTransToolCollectionDependencies = True):
    """Computationally intensive function which does a check on on consistency of the tools.
    If checks whether there are no tools that depend on themselves, whether all tool types
    that a part of a tools PassByPointer metadata are also in its DependsOn metadata and whether
    all tools mentioned in dependencies actually exist.

    If CheckTransToolCollectionDependencies is True then it will print warnings for every instance
    where a tool from a tool collection depends on another tool from another tool collection whose
    size is bigger than one."""
    print '#BTAG# - DEBUG - Checking tool collection structure.'
    for tcoll in _BTaggingConfig_ToolCollections:
        for tool in _BTaggingConfig_ToolCollections[tcoll]:
            # This loops over all tools
            Dependencies = getToolMetadata(tool, "DependsOn")
            PassByPointer = getToolMetadata(tool, "PassByPointer")
            if Dependencies:
                for d in Dependencies:
                    tcoll2 = getToolCollection(d)
                    if tcoll2:
                        if getToolCollectionSize(tcoll2) > 1 and CheckTransToolCollectionDependencies:
                            if tcoll2 != tcoll:
                                print '#BTAG# - DEBUG - Trans tool collection dependency: '+tool+' ('+tcoll+') on '+d+' ('+tcoll2+')'
                    else:
                        print '#BTAG# - WARNING - Tool of type '+tool+' depends on tool of type '+d+' but '+d+' is not part of any tool collection.'
                    if d == tool:
                        print '#BTAG# - ERROR - Tool of type '+tool+' depends on itself!'
                        raise ValueError
            if PassByPointer:
                for p in PassByPointer:
                    if not PassByPointer[p] in Dependencies:
                        print '#BTAG# - WARNING - Tool of type '+tool+' needs a tool of type '+PassByPointer[p]+' passed to it by pointer but '+PassByPointer[p]+' is not one of '+tool+"'s dependencies."
    print '#BTAG# - DEBUG - Check completed.'

def getAllToolsThatDependOn(tool_type):
    """Returns a list of tool types of all tools that directly depend on a tool of a given tool type. This operation
    can be computationally intensive. And is mainly used for debugging purposes."""
    returnlist = []
    for tcoll in _BTaggingConfig_ToolCollections:
        for tname in _BTaggingConfig_ToolCollections[tcoll]:
            Dependencies = getToolMetadata(tname, "DependsOn")
            if Dependencies:
                if tool_type in Dependencies:
                    returnlist.append(tname)
    return returnlist

def getToolCollections():
    """Returns a dictionary containing all tool collections."""
    return _BTaggingConfig_ToolCollections

def cloneTool(ToolType, Suffix):
    """Clones a single tool, creating a copy with the same name except with a suffix. It returns the name of the new tool.
    This function does not update metadata; it is therefore recommended to use the cloneToolCollection() function for most
    purposes instead. The tool will be placed in its own tool collection."""
    if 'tool'+ToolType+Suffix in dir():
            print '#BTAG# - WARNING - Attempting to clone tool of type '+ToolType+' with suffix '+Suffix+' but this already exists'
    try:
        exec 'tool'+ToolType+Suffix+' = tool'+ToolType in globals()
        exec 'meta'+ToolType+Suffix+' = deepcopy(meta'+ToolType+')' in globals()
        # update xAODBaseNames if needed
        if not getToolMetadata(ToolType, "VertexFinderxAODBaseName") is None:
            changeToolMetadata(ToolType+Suffix, "VertexFinderxAODBaseName", str(getToolMetadata(ToolType, "VertexFinderxAODBaseName"))+Suffix)
        if not getToolMetadata(ToolType, "xAODBaseName") is None:
            changeToolMetadata(ToolType+Suffix, "xAODBaseName", str(getToolMetadata(ToolType, "xAODBaseName"))+Suffix)
    except:
        print '#BTAG# - ERROR - cloneToolCollection() called but one of the tools does not exist, is not implemented correctly, or does not allow cloning: '+ToolType
        raise
    addToolTypeToToolCollection(ToolType+Suffix, ToolType+Suffix)
    changeToolMetadata(ToolType+Suffix, "IsAClone", True)
    return 'tool'+ToolType+Suffix

def cloneToolCollection(ToolCollection, Suffix):
    """Clones a tool collection, creating copies with the same name except with a suffix. Returns a list of the names of
    the new tools. The metadata for the tools will be adjusted within the tool collection as well, so that dynamic passing
    of tools remains working within the tool collection.

    This is useful if you want to set up multiple tools of the same type but with different settings. See the documentation
    for more information."""
    if not ToolCollection in _BTaggingConfig_ToolCollections:
        print '#BTAG#- ERROR - Trying to clone a tool collection called '+ToolCollection+' but this does not exist'
        options = []
        tcollections = getToolCollections()
        for tcol in tcollections:
            options.append(tcol)
        print '#BTAG#- ERROR - Valid tool collections are: '+str(options)
        raise RuntimeError
    ToolsToClone = _BTaggingConfig_ToolCollections[ToolCollection]
    returnlist = []
    for tool in ToolsToClone:
        if 'tool'+tool+Suffix in dir():
            print '#BTAG#- WARNING - Attempting to clone tool of type '+tool+' with suffix '+Suffix+' but this already exists, this operation will overwrite the previous clone'
        try:
            exec 'tool'+tool+Suffix+' = tool'+tool in globals()
            exec 'meta'+tool+Suffix+' = deepcopy(meta'+tool+')' in globals()
            # update xAODBaseNames if needed
            if not getToolMetadata(tool, "VertexFinderxAODBaseName") is None:
                changeToolMetadata(tool+Suffix, "VertexFinderxAODBaseName", str(getToolMetadata(tool, "VertexFinderxAODBaseName"))+Suffix)
            if not getToolMetadata(tool, "xAODBaseName") is None:
                changeToolMetadata(tool+Suffix, "xAODBaseName", str(getToolMetadata(tool, "xAODBaseName"))+Suffix)
            # Update metadata
            PassByPointers = {}
            PassByNames = {}
            DependsOn = {}
            exec 'if "PassByPointer" in meta'+tool+Suffix+':\n\tPassByPointers = meta'+tool+Suffix+'["PassByPointer"]'
            exec 'if "PassByName" in meta'+tool+Suffix+':\n\tPassByNames = meta'+tool+Suffix+'["PassByName"]'
            exec 'if "DependsOn" in meta'+tool+Suffix+':\n\tDependsOn = meta'+tool+Suffix+'["DependsOn"]'
            # Here we use the Python property that everything is a reference
            for pointer in PassByPointers:
                if PassByPointers[pointer] in ToolsToClone:
                    PassByPointers[pointer] += Suffix
            for name in PassByNames:
                if PassByNames[name] in ToolsToClone:
                    PassByNames[name] += Suffix
            for dependency in DependsOn:
                if dependency in ToolsToClone:
                    DependsOn[DependsOn.index(dependency)] += Suffix
            # Add to new tool collection
            addToolTypeToToolCollection(tool+Suffix, ToolCollection+Suffix)
            changeToolMetadata(tool+Suffix, "IsAClone", True)
        except:
            print '#BTAG#- ERROR - cloneToolCollection() called but one of the tools does not exist, is not implemented correctly, or does not allow cloning: '+tool
            raise
    return returnlist

def getToolAllMetadata(tool_type):
    """Returns the metadata of a tool as a Python dictionary about a tool of type tool_type.

    input: tool:          The tool one wishes to check.

    output: The metadata as a dictionary."""
    try:
        exec('D = meta'+tool_type)
    except:
        print '#BTAG#- ERROR - Tool type "'+tool_type+'" has no metadata dictionary or does not exist!'
        raise NotImplementedError
    return D

def getToolMetadata(tool_type, meta):
    """Returns a specific piece of metadata about a tool of type tool_type.

    input: tool:          The tool one wishes to check.
           meta:          The name of the metadata.

    output: The metadata or None if it is not defined."""
    D = getToolAllMetadata(tool_type)
    if meta in D:
        return D[meta]
    else:
        return None

def changeClonedToolCollectionMetadata(ToolCollection, Suffix, metakey, metavalue):
    """Changes a piece of metadata on all tools which are clones from a given ToolCollection and have a given Suffix. This
    is useful to set, for example, the 'OnePerTrackAssociationAndJetCollection' on all tools. Modifying certain types
    of metadata (such as 'DependsOn') en masse can be disastrous.

    Note, you can actually also use this on the default (i.e. non-cloned) tool collections by settings the Suffix to "", but
    this is not recommended."""
    if not ToolCollection in _BTaggingConfig_ToolCollections:
        print '#BTAG#- ERROR - Attempting to modify metadata for cloned tool collection for a tool collection that does not exist: '+ToolCollection
        raise ValueError
    for tool in _BTaggingConfig_ToolCollections[ToolCollection]:
        changeToolMetadata(tool + Suffix, metakey, metavalue)

def changeToolMetadata(tool_type, metakey, metavalue):
    """Changes a piece of metadata for a tool of type tool_type. Only do this before such a tool has been set up. For
    permanent changes adjust the actual metadata in the tool's source file."""
    try:
        exec('global meta'+tool_type)
        exec('meta'+tool_type+'[metakey] = metavalue')
    except:
        print '#BTAG#- ERROR - Trying to adjust metadata for a tool that does not exist or has no metadata: '+tool_type
        raise ValueError

def setToolDefaultTracks(tool_type, default):
    """Sets the default tracks for a certain tool of type tool_type to be equal to default. This is useful in that
    tools running on their default tracks do not have the tracks prefix to their name. So by doing this you can change
    the track collection for a certain tool but still use the same name. Should only be done before the tool has been set
    up (it will not apply this change retroactively).

    For permanent changes it is better to adjust the actual metadata."""
    changeToolMetadata(tool_type, 'DefaultTracks', default)
    print '#BTAG#- INFO - Default track association for tool of type "'+tool_type+'" changed to "'+default+'".'

def JetCollectionIsSupported(JetCollection):
    """Returns whether a given jet collection is supported for b-tagging. This takes information
    from a list in BTaggingFlags.py (SupportedJetCollections), but will ignore any jet collection
    for which there is a calibration alias to a supported collection.

    Any jet collection supplied to this function should have the -Jets part cut off."""
    jetcol = JetCollection.replace("Jets","",1) # just in case
    if jetcol in BTaggingFlags.SupportedJetCollections:
        return True
    # check for calibration alias
    for alias in BTaggingFlags.CalibrationChannelAliases:
        arrow = alias.find("->")
        if arrow == -1:
            print(self.BTagTag()+" - WARNING - Incorrectly specified rule in BTaggingFlags.CalibrationChannelAliases: "+alias)
        else:
            if jetcol == alias[:arrow]:
                aliasesto = (alias[arrow+2:]).split(",")
                for s in BTaggingFlags.SupportedJetCollections:
                    if s in aliasesto:
                        return True
    return False

################################
# Some general tool functions  #
################################

def toolMainBTaggingTool(name, useBTagFlagsDefaults = True, **options):
    """Sets up the main B-tagging tool and returns it.

    The following options have BTaggingFlags defaults:

    Runmodus                            default: BTaggingFlags.Runmodus
    PrimaryVertexName                   default: BTaggingFlags.PrimaryVertexCollectionName
    BaselineTagger                      default: BTaggingFlags.BaselineTagger
    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'Runmodus'                     : BTaggingFlags.Runmodus,
                     'PrimaryVertexName'            : BTaggingFlags.PrimaryVertexCollectionName,
                     'BaselineTagger'               : BTaggingFlags.BaselineTagger,
                     'OutputLevel'                  : BTaggingFlags.OutputLevel }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from BTagging.BTaggingConf import Analysis__BTagTool
    return Analysis__BTagTool(**options)

metaBTagTrackToJetAssociator = {}

def toolBTagTrackToJetAssociator(name, useBTagFlagsDefaults = True, **options):
    """Sets up a BTagTrackToJetAssociator tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel' : BTaggingFlags.OutputLevel }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from ParticleJetTools.ParticleJetToolsConf import Analysis__ParticleToJetAssociator
    return Analysis__ParticleToJetAssociator(**options)

metaBTagMuonToJetAssociator = { 'DefaultTracks' : 'Muons' } #This is a slight misuse of notation, but it works

def toolBTagMuonToJetAssociator(name, useBTagFlagsDefaults = True, **options):
    """Sets up a BTagMuonToJetAssociator tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    trackCone                           default: 1.0 (for the time being... has to be studied)
    useVariableSizedTrackCone           default: False

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'               : BTaggingFlags.OutputLevel,
                     'trackCone'                 : 1.0,
                     'useVariableSizedTrackCone' : False }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from ParticleJetTools.ParticleJetToolsConf import Analysis__ParticleToJetAssociator
    return Analysis__ParticleToJetAssociator(**options)

metaBTagElectronToJetAssociator = { 'DefaultTracks' : 'Electrons' } #This is a slight misuse of notation, but it works

def toolBTagElectronToJetAssociator(name, useBTagFlagsDefaults = True, **options):
    """Sets up a BTagElectronToJetAssociator tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    trackCone                           default: 0.4 (for the time being... has to be studied)
    useVariableSizedTrackCone           default: False

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'               : BTaggingFlags.OutputLevel,
                     'trackCone'                 : 0.4,
                     'useVariableSizedTrackCone' : False }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from ParticleJetTools.ParticleJetToolsConf import Analysis__ParticleToJetAssociator
    return Analysis__ParticleToJetAssociator(**options)

# Fill up the tool collections during import
# DO NOT DELETE THIS SECTION; IT IS VITAL!
for tool in dir():
    if tool.find('meta') == 0:
        tname = tool[4:]
        exec 'metadict = meta'+tname
        tcoll = tname
        if 'ToolCollection' in metadict:
            tcoll = metadict['ToolCollection']
        addToolTypeToToolCollection(tname, tcoll)



from AthenaCommon.AppMgr import ToolSvc
from ParticleJetTools.ParticleJetToolsConf import JetParticleShrinkingConeAssociation, JetParticleFixedConeAssociation, JetParticleCenterOfMassAssociation

defaultTrackAssoc = \
    JetParticleShrinkingConeAssociation(
        "DefaultBTaggingTrackAssoc",
        InputParticleCollectionName="InDetTrackParticles",
        OutputCollectionName="MatchedTracks",
        coneSizeFitPar1=+0.239,
        coneSizeFitPar2=-1.220,
        coneSizeFitPar3=-1.64e-5
    )

defaultMuonAssoc = \
    JetParticleFixedConeAssociation(
        "DefaultBTaggingMuonAssoc",
        InputParticleCollectionName="Muons",
        OutputCollectionName="MatchedMuons",
        coneSize=0.4,
    )

comTrackAssoc = JetParticleCenterOfMassAssociation(
        "CoMBTaggingTrackAssoc",
        inputTrackCollectionName="InDetTrackParticles",
        partMatchCone = 0.8,
        parentJetCone = 1.0,
        OutputCollectionName="MatchedTracks",
    )   

comMuonAssoc = JetParticleCenterOfMassAssociation(
        "CoMBTaggingMuonAssoc",
        inputTrackCollectionName="Muons",
        partMatchCone = 0.8,
        parentJetCone = 1.0,
        OutputCollectionName="MatchedMuons",
    )   


ToolSvc += defaultTrackAssoc
ToolSvc += defaultMuonAssoc
ToolSvc += comTrackAssoc
ToolSvc += comMuonAssoc
