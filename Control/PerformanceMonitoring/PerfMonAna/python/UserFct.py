# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file: UserFct.py
# @purpose: classes to allow users to specialize (and load) filtering functions
# @author: Sebastien Binet <binet@cern.ch>

""" Classes to allow users to specialize (and load) filtering functions
"""
#
#
__author__  = 'Sebastien Binet'
__version__ = "$Revision: 1.2 $"
__doc__     = "Classes to allow users to specialize (and load) filtering functions."

import imp
import inspect

class PluginFct:
    Name = 'filterFct'

class FilterFct:
    __sharedState = {
        ## default selector: selects everything
        PluginFct.Name : lambda x: x is not None
        }

    def __init__(self):
        self.__dict__ = self.__sharedState

    def __call__(self, histo):
        return self.__sharedState[PluginFct.Name](histo)

    def setFilter(self, fct):
        self.__sharedState[PluginFct.Name] = fct

def loadFilterFct( uri ):
    """Load and inspect a 'URI'-like resource. If this URI looks like a file,
    then it will be loaded and inspected for any function whose name is
    'filterFct'.
    Otherwise, the 'URI' will be evaluated as a lambda expression having a
    histogram 'h' as variable.
    """
    import os.path as osp
    def pluginFilter(obj):
        if inspect.isfunction(obj):
            return obj.__name__ == PluginFct.Name

    filterFct = FilterFct( )
    if len(uri) == 0:
        return filterFct

    if uri.startswith( 'file:' ) or osp.exists( uri ) :
        uri = osp.expanduser( osp.expandvars( uri.replace( 'file:', '' ) ) )
        plugin = open( uri, 'r' )
        mod = imp.load_source( plugin.name[:-3], plugin.name, plugin )
        plugin.close()

        filterFct.setFilter( inspect.getmembers(mod, pluginFilter)[0][1] )

    else:
        def userFct (m):
            return eval (uri)
        filterFct.setFilter( userFct )

    return filterFct

