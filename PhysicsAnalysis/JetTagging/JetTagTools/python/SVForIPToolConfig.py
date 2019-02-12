# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# import the SVForIPTool configurable
from JetTagTools.JetTagToolsConf import Analysis__SVForIPTool

# define the class
class SVForIPToolCfg( Analysis__SVForIPTool ):
    # constructor
    def __init__(self, name = 'SVForIPTool', options = {}):
        """Sets up a SVForIPTool_IP2D tool and returns it.

        input:    name: The name of the tool (should be unique).
                  **options: Python dictionary with options for the tool.
        output: The actual tool."""

        options['name'] = name

        return Analysis__SVForIPTool.__init__(self, **options) 
