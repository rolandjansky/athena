# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from DerivationFrameworkTrigger.DerivationFrameworkTriggerConf import (
    DerivationFramework__TriggerMatchingTool)
from AthenaCommon.AppMgr import ToolSvc
import AthenaCommon.AppMgr as AppMgr

class TriggerMatchingHelper(object):
  """ Helper class for setting up the trigger matching. """

  def __init__(
      self,
      matching_tool,
      trigger_list):
    """
      Create the helper.
      Arguments:
        - matching_tool: If this is a string then it is used as the name of tool
      to be created. If it's a tool then this is the tool to use.
        - trigger_list: If the tool is created inside this function or if the
      provided tool does not have its ChainNames property set this will be set
      as that property.
   """
    if isinstance(matching_tool, basestring):
        matching_tool = DerivationFramework__TriggerMatchingTool(
          matching_tool,
          ChainNames = trigger_list)
    elif not hasattr(matching_tool, "ChainNames") or \
          not matching_tool.ChainNames:
        matching_tool.ChainNames = trigger_list
    # ToolSvc += matching_tool 
    AppMgr.ToolSvc += matching_tool 
    self.matching_tool = matching_tool

  def output_containers(self):
    """ Get the full list of containers to be written out."""
    try:
      prefix = self.matching_tool.OutputContainerPrefix
    except AttributeError:
      prefix = self.matching_tool.getDefaultProperty("OutputContainerPrefix")
    return ["{0}{1}".format(prefix, chain) for chain in self.matching_tool.ChainNames]

  def add_to_slimming(self, slimming_helper):
    """ Add the created containers to the output """
    slimming_helper.AllVariables += self.output_containers()
    # Also add the types to the dictionary
    for cont in self.output_containers():
      slimming_helper.AppendToDictionary.update( {
        cont : "xAOD::TrigCompositeContainer",
        cont + "Aux" : "xAOD::AuxContainerBase!"})
