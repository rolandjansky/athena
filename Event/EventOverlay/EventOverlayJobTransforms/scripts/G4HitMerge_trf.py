#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Merge two G4 HITS files into one file, adding the HITS from event 1 in one file to those from event 1 in the other file, 2 to 2, etc."""

from PyJobTransformsCore.trf import *
from PyJobTransformsCore.full_trfarg import *
from PyJobTransformsCore.trfutil import *
from PyJobTransformsCore.TransformConfig import *
import PyJobTransformsCore.basic_trfarg as trfarg

from RecJobTransforms.RecConfig import recConfig

class InputHITS1FileArg(StringArg):
    """The first input HITS file"""
    def __init__(self,help='default',name='inputHITS1File'):
        StringArg.__init__(self,help,name)
        
    def isFullArgument(self):
        return True

class InputHITS2FileArg(StringArg):
    """The second input HITS file"""
    def __init__(self,help='default',name='inputHITS2File'):
        StringArg.__init__(self,help,name)
        
    def isFullArgument(self):
        return True

class OutputHITSFileArg(StringArg):
    """The output HITS file"""
    def __init__(self,help='default',name='outputHITSFile'):
        StringArg.__init__(self,help,name)
        
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

    
class G4HitMergeJobTransform( JobTransform ):
    def __init__(self):
        JobTransform.__init__(self,
                              authors = [ Author('Andrew Haas', 'ahaas@cern.ch'), Author('William Lockman','William.Lockman@cern.ch') ] ,
                              skeleton='EventOverlayJobTransforms/skeleton.G4HitMerge.py' ,
                              help = __doc__,
                              config = recConfig )

        #add arguments
        self.add( InputHITS1FileArg() )
        self.add( InputHITS2FileArg() )
        self.add( OutputHITSFileArg() )
        self.add( MaxEventsArg() )
        self.add( SkipEventsArg() )
        self.add( GeometryVersionArg() )
        self.add( DBReleaseArg(), default='NONE' )
        self.add( ConditionsTagArg(), default='NONE' )
        self.add( PostIncludeArg(), default='NONE' ) 
        
        #add other features
        self.add( SQLiteSupport() )


# execute it if not imported
if __name__ == '__main__':
    trf = G4HitMergeJobTransform()
    sys.exit(trf.exeSysArgs().exitCode())

