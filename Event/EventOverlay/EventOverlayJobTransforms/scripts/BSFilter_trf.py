#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Filter BS data based on trigger bit"""

from PyJobTransformsCore.trf import *
from PyJobTransformsCore.full_trfarg import *
from PyJobTransformsCore.trfutil import *
from PyJobTransformsCore.TransformConfig import *
import PyJobTransformsCore.basic_trfarg as trfarg

from RecJobTransforms.RecConfig import recConfig

class OutputBSFilterFileArg(StringArg):
    """The output file prefix"""
    def __init__(self,help='default',name='OutputBSFilterFile'):
        StringArg.__init__(self,help,name)
        
    def isFullArgument(self):
        return True

class EventIdFileArg(StringArg):
    """The output text file for EventIdModifierSvc lines"""
    def __init__(self,help='default',name='EventIdFile'):
        StringArg.__init__(self,help,name)
        
    def isFullArgument(self):
        return True

class FilterFileArg(StringArg):
    """The name of the text file for filtering (HI) events"""
    def __init__(self,help='default',name='FilterFile'):
        StringArg.__init__(self,help,name)
        
    def isFullArgument(self):
        return True
 
class TriggerBitArg(IntegerArg):
    """The trigger bit to select"""
    def __init__(self,help='default',name='TriggerBit'):
        IntegerArg.__init__(self,help,name)
        
    def isFullArgument(self):
        return True

class PostIncludeArg(JobOptionsArg):
    """Joboptions file with user settings, to run after the job itself"""
    def __init__(self,help='default',package='',name='default'):
        # split comma separated string into list
        if type(package) == str: package = package.split(',')
        # always add 'EventOverlayJobTransforms' package (which contain common postIncludeConfig files)
        commonPack = 'EventOverlayJobTransforms'
        if commonPack not in package: package.append(commonPack)
        JobOptionsArg.__init__(self,help=help,package=package,name=name)
        self.__config = None

    def isFullArgument(self):
        return True

    
class BSFilterJobTransform( JobTransform ):
    def __init__(self):
        JobTransform.__init__(self,
                              authors = [ Author('Andrew Haas', 'ahaas@cern.ch') ] ,
                              skeleton='EventOverlayJobTransforms/skeleton.BSFilter.py' ,
                              help = __doc__,
                              config = recConfig )

        #add arguments
        self.add( InputBSFileArg() )
        self.add( OutputBSFilterFileArg() )
        self.add( EventIdFileArg(), default="" )
        self.add( FilterFileArg(), default="" )
        self.add( TriggerBitArg(), default=-1 )
        self.add( MaxEventsArg(), default=-1 )
        self.add( SkipEventsArg(), default=0 )
        self.add( PostIncludeArg(), default='NONE' ) 
        
        #add other features
        self.add( SQLiteSupport() )


# execute it if not imported
if __name__ == '__main__':
    trf = BSFilterJobTransform()
    sys.exit(trf.exeSysArgs().exitCode())

