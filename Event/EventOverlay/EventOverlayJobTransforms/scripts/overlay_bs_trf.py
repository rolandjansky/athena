#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Overlay simulated physics events into zero bias real data events; the real data events are in Byte Stream format."""

from PyJobTransformsCore.trf import *
from PyJobTransformsCore.full_trfarg import *
from PyJobTransformsCore.trfutil import *

from EventOverlayJobTransforms.OverlayConfig import overlayConfig
from EventOverlayJobTransforms.Configuration import DigiRndmSvcArg
from EventOverlayJobTransforms.Configuration import SamplingFractionDbTagArg
from EventOverlayJobTransforms.Configuration import NoiseControlArg
from EventOverlayJobTransforms.Configuration import AddCaloDigiArg

ListOfDefaultPositionalKeys = ['inputHitsFile', 'pileupBSFile', 'signalRDOFile', 'outputRDOFile', 'maxEvents', 'skipEvents', 'geometryVersion', 'digiSeedOffset1', 'digiSeedOffset2', 'conditionsTag', 'digiRndmSvc', 'samplingFractionDbTag', 'fSampltag', 'triggerConfig',  'DBRelease', 'jobConfig', 'preExec', 'postInclude', 'postExec', '--extraignorefilters']

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

class CscCondOverrideArg(BoolArg):
    """A hack to override CSC conditions folder"""
    def __init__(self,help,name='cscCondOverride'):
        BoolArg.__init__(self,help,name)

    def isFullArgument(self):
        return True

class fSampltagArg(StringArg):
    """The cool tag for /LAR/ElecCalib/fSampl/Symmetry, see https://twiki.cern.ch/twiki/bin/viewauth/Atlas/LArCalibMCPoolCool"""
    def __init__(self,help='default',name='fSampltag'):
        StringArg.__init__(self,help,name)
        
    def isFullArgument(self):
        return True

class preExecArg(StringArg):
    """Can add python statements to be executed early on"""
    def __init__(self,help='default',name='preExec'):
        StringArg.__init__(self,help,name)
    def isFullArgument(self):
        return True

class postExecArg(StringArg):
    """Can add python statements to be executed later on"""
    def __init__(self,help='default',name='postExec'):
        StringArg.__init__(self,help,name)
    def isFullArgument(self):
        return True

class OverlayJobTransform( JobTransform ):
    def __init__(self):
        JobTransform.__init__(self,
                              authors = [ Author('Ketevi A. Assamagan', 'ketevi.adikle.assamagan@cern.ch') ] ,
                              skeleton='EventOverlayJobTransforms/skeleton.commis.py' ,
                              help = __doc__,
                              config=overlayConfig )

        #add arguments
        self.add( InputHitsFileArg() )
        self.add( InputBSFileArg(name='pileupBSFile',  help='input RAW BS for pileup overlay') )
        self.add( OutputRDOFileArg() )
        self.add( MaxEventsArg() )
        self.add( SkipEventsArg() )
        self.add( JobConfigArg(package='EventOverlayJobTransforms') )
        self.add( GeometryVersionArg() )
        self.add( RandomSeedArg(name = 'digiSeedOffset1', help = 'random seed offset for digitization') )
        self.add( RandomSeedArg(name = 'digiSeedOffset2', help = 'random seed offset for digitization') )
        self.add( ConditionsTagArg() )
        self.add( SamplingFractionDbTagArg( ['QGSP_BERT', 'QGSP_BERT_BIRK', 'QGSP_EMV', 'QGSP', 'QGSP_BERT_EMV', 'FTFP_BERT'] ) )
        self.add( fSampltagArg() )
        self.add( DigiRndmSvcArg( ['AtRndmGenSvc','AtRanluxGenSvc','AtDSFMTGenSvc'] ), default='AtRanluxGenSvc' )
        self.add( OutputRDOFileArg(name='signalRDOFile', help='the RDO file of the MC signal alone'), default='NONE' )
        self.add( TriggerConfigArg(), default='NONE' )
        self.add( DBReleaseArg(), default='NONE' )
        self.add( PostIncludeArg(), default='NONE' )
        self.add( preExecArg(), default='NONE' )
        self.add( postExecArg(), default='NONE' ) 

        # avoid sqlite nfs lock problems
        self.add( SQLiteSupport() )

# execute it if not imported
if __name__ == '__main__':
    trf = OverlayJobTransform()
    sys.exit(trf.exeSysArgs().exitCode())

