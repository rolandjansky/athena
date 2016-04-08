#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Produce skimmed AODs for HSG2
Skimming criterium is a loose di-lepton selection"""

from PyJobTransformsCore.trf import *
from PyJobTransformsCore.full_trfarg import *
from PyJobTransformsCore.trfutil import *
from HSG2DPDUtils.HSG2TrfConfig import ElectronQualityArg, MuonQualityArg

class HSG2DiLeptonJobTransform( JobTransform ):
    def __init__(self):
        JobTransform.__init__(self,
                              authors = [ Author('Andrea Di Simone','andrea.di.simone@cern.ch')] ,
                              skeleton='HSG2DPDUtils/skeleton.hsg2_daod_2lepton.py',
                              help = __doc__ )


        #add arguments
        self.add( InputAODFileArg() )
        self.add( OutputDPDFileArg(name='outputDAOD_2LHSG2File') )
        self.add( MaxEventsArg() )
        self.add( DBReleaseArg(), default='NONE' )
        self.add( ElectronQualityArg(['Loose','Medium','Tight','any']), default='any')
        self.add( MuonQualityArg(['any','combined','standalone','lowPt','combined+lowPt','inMS']), default='inMS')
        
# execute it if not imported
if __name__ == '__main__':
    trf = HSG2DiLeptonJobTransform()
    sys.exit(trf.exeSysArgs().exitCode())
