# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# PyAthena algorithm to select projective tracks, to be used in Cosmic Muon selection. 
#  Cuts: d0_max:  maximum track d0
#        z0_max:  maximum track z0
#        num_si_hit_min: minimum number of Si hits (Pixel+SCT) on the track
#
#  Hong Ma <hma@bnl.gov>, Nov 27, 2008
#


import AthenaPython.PyAthena as PyAthena
from math import fabs
from sys import stdout
from AthenaPython.PyAthena import StatusCode
from AthenaCommon.Logging import logging

import PyUtils.RootUtils as ru
ROOT = ru.import_root()

SummaryType_numberOfPixelHits = 2
SummaryType_numberOfSCTHits = 3

class IdProjFilter ( PyAthena.AthFilterAlgorithm ):
    def __init__(self, name = "IdProjFilter", **kw):
        ## init base class
        kw['name'] = name
        super(IdProjFilter,self).__init__(**kw)

        self.logger= logging.getLogger( name )
        self.d0_max = 300.
        self.z0_max = 300.
        self.num_si_hit_min = 1 

    def initialize(self):

        self.logger.info(" initializing ") 

        self.sg = PyAthena.py_svc("StoreGateSvc")

        if not self.sg:
            self.msg.error("Could not retrieve StoreGateSvc")
            return StatusCode.Failure
        
        return StatusCode.Success

    def execute(self):

        tps = None
        try:
            tps  = self.sg.retrieve('Rec::TrackParticleContainer','TrackParticleCandidate')
            pass
        except LookupError:
            self.msg.warning( 'Collection TrackParticleCandidate not found' )           
            self.setFilterPassed(False)
            return StatusCode.Success

        
        self.logger.debug(" TrackParticleCandidates %s", tps.size())

        passed=False 
        for i in range(tps.size()):
            t = tps[i]
            tt=t.originalTrack()
            per=tt.perigeeParameters()
            param=per.parameters()
            sum = t.trackSummary()

            npix = sum.get(SummaryType_numberOfPixelHits)
            nsct =sum.get(SummaryType_numberOfSCTHits)
            d0 = param[0]
            z0 = param[1]
            
            self.logger.debug(" Track eta %s, phi %s, momentum %s ", t.eta(),t.phi(),t.p()) 
            self.logger.debug(" Number of Pixel hits  %s, SCT hits %s ", npix, nsct)
            self.logger.debug(" d0= %s and z0= %s  ", d0, z0 )

            if ( npix+nsct >= self.num_si_hit_min ) and (fabs(d0)<self.d0_max) and (fabs(z0)<self.z0_max) :
                passed = True
            

        self.logger.debug(" filter passed = %s ", passed )

        self.setFilterPassed(passed) 

        return StatusCode.Success
            
    def finalize(self):

        self.logger.info ( "=== IdProjFilter Done ===" ) 

        return StatusCode.Success

