# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name: SlimTracks.py   
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
## 
## Based on: DPDUtils
##
## Description: Remove information from tracks below pT = 5 GeV
##              Remove: ErrorMatrix and Summary.
##                      
##=============================================================================


from gaudimodule import PyAlgorithm
import PyParticleTools.PyParticleTools as PyParticleTools
import AthenaCommon.SystemOfUnits as Units

## Import the logging module
from AthenaCommon.Logging import logging


class SlimTracks( PyAlgorithm ):
    def __init__ ( self, name = "SlimTracks", **kw ) :
        # initialize base class
        kw['name'] = name
        PyAlgorithm.__init__(self, **kw)

        # Set default values for properties
        self.maxPtTrack = kw.get('maxPtTrack', 5.0*Units.GeV)

        # Get the logger for this class
        self.msg = None


    def initialize(self):
        # Get the logger for this class
        self.msg = logging.getLogger( self.name() )
        self.msg.info( '************************************' )
        self.msg.info( '==> initialize %s...' % self.name() )
        self.msg.info( 'Will apply the following selections:' )
        self.msg.info( '  slim tracks with pt less than = %r', self.maxPtTrack )

        return True


    def execute(self):
        trs = None
        try:
            trs = PyParticleTools.getTrackParticles("TrackParticleCandidate")
        except LookupError:
            self.msg.warning( 'Collection TrackParticleCandidate not found' )           
            return StatusCode.Success
           
        for tr in trs:
            if tr.pt() < self.maxPtTrack :
                tr.removeSummary()
                tr.removeErrorMatrix()
        return True


    def finalize(self):
        return True
