#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Overlay simulated physics events into zero bias real data events; the real data events are in POOL format."""

from PyJobTransformsCore.trf import *
from PyJobTransformsCore.full_trfarg import *
from PyJobTransformsCore.trfutil import *

from EventOverlayJobTransforms.OverlayConfig import overlayConfig
from EventOverlayJobTransforms.Configuration import DigiRndmSvcArg
from EventOverlayJobTransforms.Configuration import SamplingFractionDbTagArg
from EventOverlayJobTransforms.Configuration import NoiseControlArg
from EventOverlayJobTransforms.Configuration import AddCaloDigiArg

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

class ReadByteStreamArg(BoolArg):
    """Are the real data input files byte stream or POOL?"""
    def __init__(self,help,name='ReadByteStream'):
        BoolArg.__init__(self,help,name)

    def isFullArgument(self):
        return True

class MuonForceUseArg(BoolArg):
    """A hack for muon 8-fold versus 12-fold problems"""
    def __init__(self,help,name='muonForceUse'):
        BoolArg.__init__(self,help,name)

    def isFullArgument(self):
        return True

class OverlayJobTransform( JobTransform ):
    def __init__(self):
        JobTransform.__init__(self,
                              authors = [ Author('Ketevi A. Assamagan', 'ketevi.adikle.assamagan@cern.ch') ] ,
                              skeleton='EventOverlayJobTransforms/skeleton.overlay.py' ,
                              help = __doc__,
                              config=overlayConfig )

        #add arguments
        self.add( InputHitsFileArg() )
        self.add( InputRDOFileArg(name='pileupBSFile',  help='input RAW RDO for pileup overlay') )
        self.add( OutputRDOFileArg() )
        self.add( MaxEventsArg() )
        self.add( SkipEventsArg() )
        self.add( GeometryVersionArg() )
        self.add( RandomSeedArg(name = 'digiSeedOffset1', help = 'random seed offset for digitization') )
        self.add( RandomSeedArg(name = 'digiSeedOffset2', help = 'random seed offset for digitization') )
        self.add( ConditionsTagArg(), default='NONE' )
        self.add( OutputRDOFileArg(name='signalRDOFile', help='the RDO file of the MC signal alone'), default='NONE' )
        self.add( DigiRndmSvcArg( ['AtRndmGenSvc','AtRanluxGenSvc']), default = 'AtRanluxGenSvc')
        self.add( SamplingFractionDbTagArg(['QGSP_BERT','QGSP_EMV', 'QGSP', 'QGSP_BERT_EMV']), default = 'QGSP_EMV' )
        self.add( TriggerConfigArg(), default='default' )
        self.add( DBReleaseArg(), default='NONE' )
        self.add( JobConfigArg(package='EventOverlayJobTransforms'), default='NONE' )
        self.add( PostIncludeArg(), default='NONE' ) 
        self.add( ReadByteStreamArg("True/False: are the input real data files POOL or ByteStream?"), default=False)

        # avoid sqlite nfs lock problems
        self.add( SQLiteSupport() )

# execute it if not imported
if __name__ == '__main__':
    trf = OverlayJobTransform()
    sys.exit(trf.exeSysArgs().exitCode())


