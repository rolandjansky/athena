# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# File: AthExJobOptions/CustomToolUsingTool2.py
# Author: Wim Lavrijsen (WLavrijsen@lbl.gov)

__version__ = '1.0.0'
__author__  = 'Wim Lavrijsen (WLavrijsen@lbl.gov)'

__all__ = [ 'CustomToolUsingTool2' ]


# Customized class, enable through pseudo-metaclass
class CustomToolUsingTool2( object ):
   def __new__( cls, name = 'CustomToolUsingTool2' ):
    # first, look for already fully customized instance
      from AthExJobOptionsConf import ToolUsingTool
      try:
         return ToolUsingTool.configurables[ name ]
      except KeyError:
         pass

    # else, request a new instance, and add customization
      from AthExJobOptionsConf import ConcreteTool
      tool = ToolUsingTool( name )
      tool.Factor = 42.

      tool.TheToolTool = ConcreteTool( 'ToolTool' )

    # all done
      return tool
