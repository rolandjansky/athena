# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Python function implementation of B-tagging configuration
# Wouter van den Wollenberg (2013-2015)
# E-mail: wouter.van.den.wollenberg@cern.ch, wwollenb@nikhef.nl

from __future__ import print_function

from BTagging.BTaggingFlags import BTaggingFlags

# -- Import all the functions that set up all the tools --

# Global dictionary keeping track of all tool collections
_BTaggingConfig_ToolCollections = {}

# We also need deepcopy here (for cloning metadata when cloning tools since these include nested dictionaries)
#from copy import deepcopy

# Global dictionary to keep track of all configuration instances
_BTaggingConfig_Configurations = {}

def getConfiguration(name = ""):
  """Returns a configuration instance of the given name. The default one has an empty string as name. Creates it if it does not exist."""
  if name in _BTaggingConfig_Configurations:
    return _BTaggingConfig_Configurations[name]
  else:
    return createConfiguration(name)

def createConfiguration(name = ""):
  """Create a new configuration instance with a given name and returns it."""
  global _BTaggingConfig_Configurations
  if name in _BTaggingConfig_Configurations:
    print ('#BTAG# - ERROR - Attempting to create a configuration instance with name "'+name+'" but this configuration already exists!')
    raise ValueError
  NewConf = Configuration(name)
  _BTaggingConfig_Configurations[name] = NewConf
  if len(name) == 0:
    print ('#BTAG# - INFO - Default configuration instance created.')
  else:
    print ('#BTAG# - INFO - Non-default configuration instance created: "'+name+'"')
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

  def getOutputFilesPrefix(self):
    """Retrieves the configuration specific setting of BTaggingFlags.OutputFilesPrefix."""
    return self._OutputFilesPrefix

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

