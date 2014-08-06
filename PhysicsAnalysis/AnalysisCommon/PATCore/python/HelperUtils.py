# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        HelperUtils.py
##
## Author:      Karsten Koeneke (CERN)
## Created:     October 2011
##
## Description: Define some useful helpers. These cannot rely on Athena!
##
##=============================================================================


def GetTool(theTool):
    """
    Try to get the underlying ROOT tool, if needed.
    """
    try:
        theTool = theTool.getRootTool()
        pass
    except AttributeError :
        theTool = theTool
        pass
    return theTool



def SetToolProperties( theTool, **kw ):
    """
    Get all provided properties and overwrite the default values with them
    """
    keys = sorted( kw.keys() )
    for key in keys:
        if hasattr( theTool, key ):
            setattr( theTool, key, kw[key] )
            pass
        else:
            print "(SetToolProperties) ERROR: Could NOT find property %s for tool with name %s" % (key, theTool.getName())
            pass
        pass
    pass
