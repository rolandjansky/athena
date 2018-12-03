# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================
## Name:      HSG1Filter
##
## Author:    James Saxon
## Created:   February 2013
##=============================

__doc__     = "Perform the HSG1 preselection."
__author__  = "James Saxon"

import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

from PrimaryDPDMaker import PrimaryDPDHelpers

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

# ---- Load the egammaPID and egammaParameters information
# This is needed to always be up-to-date with the egamma
# IsEM selections and also the author selections
from ROOT import egammaPID
from ROOT import egammaParameters

from math import fabs


#######################################################
####    THIS CLASS DEFINES THE PHOTON SELECTION    ####
#######################################################

class HSG1PreselectionFilter( PyAthena.AthFilterAlgorithm ):

    """
    This class performs (most of) the HSG1 preselection.
    """

    def __init__ ( self, name = "HSG1PreselectionFilter", **kw ):

        ## initialize base class
        kw['name'] = name
        super(HSG1PreselectionFilter, self).__init__(**kw)
        
        ## Define the cuts
        self.photonCollectionType = kw.get('photonCollectionType', "PhotonContainer")
        self.photonCollectionName = kw.get('photonCollectionName', "PhotonAODCollection")

        ## Get the storgate handle
        self.storeGateSvc   = None

        
    def initialize(self):
        self.msg.info( '************************************' )
        self.msg.info( '==> initialize %s...', self.name() )
        self.msg.info( 'Will apply the following cuts:' )
        self.msg.info( '  photonCollectionName       = %r', self.photonCollectionName )
        self.msg.info( '************************************' )

        ## Also, declare some counters and initialize them to zero
        self.nProcessed = 0
        self.nLeading   = 0
        self.nTotal     = 0
        self.nPassed    = 0

        ## Get the StoreGate service
        self.storeGateSvc = PyAthena.py_svc('StoreGateSvc')
        if self.storeGateSvc is None:
            self.msg.error("Problem retrieving StoreGateSvc pointer !!")
            return StatusCode.Failure
        
        return StatusCode.Success

    def execute(self):

        self.nProcessed += 1
        self.nLeading    = 0
        self.nTotal      = 0
        self.msg.debug( '==> execute %s on %r. event...' % (self.name(), self.nProcessed) )

        #--- Photons!
        photonCollection = None
        try:

          photonCollection = self.storeGateSvc.retrieve( self.photonCollectionType,
                                                           self.photonCollectionName )
          for photon in photonCollection :

            if photon.isem(egammaPID.PhotonLoose):
              continue

            if photon.isem(0x800000):
              continue

            if photon.isgoodoq(34214):
              continue

#            eta = fabs(photon.cluster().etaBE(2))
#            if 1.37 < eta < 1.55 or 2.37 < eta:
#              continue

            # using > 5% as a (very) conservative max(alpha)
            if photon.hlv().perp() > 37.*Units.GeV:
              self.nLeading += 1

            if photon.hlv().perp() > 27.*Units.GeV:
              self.nTotal += 1

        except LookupError:
            self.msg.warning( 'Collection %s not found' % self.photonCollectionName )

        if self.nLeading and self.nTotal >= 2:
          self.msg.debug( '%s event passed' % self.name() )
          self.setFilterPassed(True)
          self.nPassed += 1

        else:
          self.msg.debug( '%s event failed' % self.name() )
          self.setFilterPassed(False)

        return StatusCode.Success


    def finalize(self):
      self.msg.info( '==> finalize %s...' % self.name() )
      self.msg.info( '***************************************************************' )
      self.msg.info( 'Cut-flow table of %s skimming algorithm:' % self.name() )
      self.msg.info( '-------------' )
      self.msg.info( ' Number of processed events: %r' % self.nProcessed )

      self.msg.info( ' Number of events passed   : %r' % self.nPassed )

      return StatusCode.Success

