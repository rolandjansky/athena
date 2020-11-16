# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        ConfiguredJetCleaningTools
##
## Author:      Karsten Koeneke (CERN)
## Created:     July 2011
##
## Description: Apply the default configurations for the JetCleaningTools,
##              but allow for overwriting them with user-defined values.
##
##=============================================================================

# Import the needed general stuff
from PATCore.HelperUtils import SetToolProperties
from AthenaCommon import CfgMgr

# Import the needed stuff specific to the JetCleaning
from JetSelectorTools.JetSelectorToolsConf import ECUtils__EventCleaningTool as EventCleaningTool
from JetSelectorTools.JetCleaningCutDefs import JetCleaningToolConfig_Loose, JetCleaningToolConfig_Tight


def recEventCleaningTool(name='EventCleaningTool'):
    """
    Arguments:
      name                  - set the name of the master tool.
    """
    # Configure the master tool
    ecTool = EventCleaningTool(name)
    
    return ecTool


def ConfiguredJetCleaningTool_Loose( name, **kw ):
    """
    Configure the JetCleaningTool with the default Loose cuts
    and allow for (re-)setting of all provided cuts.
    """
    # Set some default properties, but don't overwrite them if they already exist
    kw["CutLevel"] = kw.get( "CutLevel" , "LooseBad" )

    # Create and instance of the tool
    tool = CfgMgr.JetCleaningTool(name, **kw)

    # Configure it with the standard configuration
    JetCleaningToolConfig_Loose( tool )

    # Get all provided properties and overwrite the default values with them
    SetToolProperties( tool, **kw )
    
    return tool



def ConfiguredJetCleaningTool_Tight( name, **kw ):
    """
    Configure the JetCleaningTool with the default Tight cuts
    and allow for (re-)setting of all provided cuts.
    """
    # Set some default properties, but don't overwrite them if they already exist
    kw["CutLevel"] = kw.get( "CutLevel" , "TightBad" )
    
    # Create and instance of the tool
    tool = CfgMgr.JetCleaningTool(name, **kw)

    # Configure it with the standard configuration
    JetCleaningToolConfig_Tight( tool )

    # Get all provided properties and overwrite the default values with them
    SetToolProperties( tool, **kw )
    
    return tool



