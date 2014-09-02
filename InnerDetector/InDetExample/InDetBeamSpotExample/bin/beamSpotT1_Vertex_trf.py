#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#James Walder

__doc__ ="""Simple Transform to understand what goes on"""


from PyJobTransformsCore.trf import *
from PyJobTransformsCore.trfutil import *
from PyJobTransformsCore.full_trfarg import *
#from RecJobTransforms.RecConfig import recConfig

class BeamSpotVertexTransform( JobTransform ):
    def __init__(self):
        JobTransform.__init__(self,
                              authors = [ Author('James Walder','jwalder@cern.ch')],
                              skeleton = 'InDetBeamSpotExample/skeleton.BeamSpotVertex.py',
                              help    = __doc__)
                              #,config = recConfig)
        print "JW: In Init"
        self.add( InputDPDFileArg(name='InputDESDM_BEAMSPOTFile'))
        self.add( NtupleFileArg(name='beamspotFile'))
        self.add( NtupleFileArg(name='monitoringFile'),default='mon.root')
        self.add( BasicStringArg(help = 'db filename', name='beamspotDb'), default = 'beamspot.db')
        self.add( ListOfStringsArg(name='jobConfig', help='jobConfig extra options'), default = '')
        self.add( JobOptionsArg(name='topOptions',help='InDetBeamSpotExampleJobOption config file'),
                  default='InDetBeamSpotExample/VertexTemplate.py')
        self.add( ListOfStringsArg(name='preExec', help='intial options'), default = '')
        self.add( ListOfStringsArg(name='postExec', help='final options'), default = '')
        self.add( MaxEventsArg(),  default ='-1' )

        
    def doPostRunActions(self):
        print "JW: in PostRun"
        

if __name__ == '__main__':
    print "JW: Run from Main"
    trf = BeamSpotVertexTransform()
    sys.exit(trf.exeSysArgs().exitCode())
