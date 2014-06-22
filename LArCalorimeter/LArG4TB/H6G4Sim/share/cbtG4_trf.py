#!/usr/bin/env python

__doc__ = """Run Geant4 simulation on generated events and produce a HITS file."""

from PyJobTransformsCore.trf import *
import PyJobTransformsCore.basic_trfarg as trfarg
from PyJobTransformsCore.full_trfarg import *
from PyJobTransformsCore.trfutil import *

class BoolCTBArg(trfarg.BoolArg):
    """Options for boolean switches"""
    def __init__(self,help,name='BoolCTBArg'):
        trfarg.BoolArg.__init__(self,help,name)
 
    def isFullArgument(self):
        return True

    def jobOrTask(self):
        return 'job'

class CTBH62004G4JobTransform( JobTransform ):
    def __init__(self):
        JobTransform.__init__(self,
                              authors = [ Author('Denis Salihagic', 'Denis.Salihagic@cern.ch'),
                                          Author('Pavol Strizenec', 'pavol@mail.cern.ch') ] ,
                              skeleton='H6G4Sim/skeleton_G4TB_LArH6-2004.py' ,
                              help = __doc__ ,
                              )

        #add arguments
        self.add( OutputHitsFileArg() )
        self.add( MaxEventsArg(), default=10 )
        self.add( RandomSeedArg('random seed for simulation'), default=1234 )
        self.add( RandomSeedArg(name = 'genSeed1', help = 'random seed 1 for GenSeed'), default=2000160768. )
        self.add( RandomSeedArg(name = 'genSeed2', help = 'random seed 2 for GenSeed'), default=643921183. )
        self.add( GeometryVersionArg(), default='ATLAS-H6-2004-00' )
        self.add( RandomSeedArg(name = 'xCryo', help = 'X position of cryostat'), default=0. )
        self.add( RandomSeedArg(name = 'yTable', help = 'Y position of moveable table'), default=0.)
        self.add( RandomSeedArg(name = 'xSmear', help = 'Beam smear in X'), default=25.0 )
        self.add( RandomSeedArg(name = 'ySmear', help = 'Beam smear in Y'), default=25.0 )
        self.add( RandomSeedArg(name = 'beamParticle', help = 'Beam particle'), default = 211 )
        self.add( ConditionsTagArg(name = 'beamEnergy', help = 'Energy of beam'), default = str(40000.) )
        self.add( BasicStringArg(name = 'PhysicsList', help = 'Which physics list to use'), default='QGSP_BERT')
        self.add( BoolCTBArg(name = 'IsSingleRun', help = 'Is this for single run ?'), default=True )
        self.add( BoolCTBArg(name = 'DoFiltering', help = 'Do a geometrical filter ?'), default=True )
        self.add( BoolCTBArg(name = 'DoBirk', help = 'Do a BirkLaw ?'), default=True )
#        self.add( atlasG4Config.IgnoreConfigErrorArg(), default=False )
        # avoid sqlite nfs lock problems
        self.add( SQLiteSupport() )

#    def runJob(self):
        #install the DBRelease 'by hand' (needed since preRunAction()'s are not called)
        #self.getArgument('DBRelease').install()
        # get the list of arguments to be used in sub-transforms
        argDict = self.argumentOriginalDict()
        # no need to re-install the DBRelease in sub-transforms
        #argDict['DBRelease'] = 'NONE'
        #
        # AtlasG4 phase
        #
#        g4 = CTBH62004G4JobTransform()
#        g4.setJobReportOptions('None')
#        report = JobReport()
#        report.setProducer(self.name(),self.version())
#        report.addReport( g4.exeSysArgs() )

#        return report


# execute it if not imported
if __name__ == '__main__':
    trf = CTBH62004G4JobTransform()
    sys.exit(trf.exeSysArgs().exitCode())


