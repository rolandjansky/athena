# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        EM-Cluster Filter
##
## Author:      Karsten Koeneke (DESY)
## Created:     April 2008
##
## Description: This is a short algorithm to select events that contain one
##              EM cluster which fulfill a certain set of criteria.
##=============================================================================

__doc__ = """This is a short algorithm to select events that contain one 
EM cluster which fulfill a certain set of criteria.
"""
__version__ = "0.0.2"
__author__  = "Karsten Koeneke <karsten.koeneke@desy.de>"


import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

class EMClusterFilter( PyAthena.AthFilterAlgorithm ):
    """
    This class is a short algorithm to select events that contain one 
    EM cluster which fulfill a certain set of criteria. The possible
    criteria are (with default values):
    cutEtMinEMCl     = 30.0*Units.GeV)
    cutEtaMax        = 2.5
    removeEtaCrack   = False
    crackEtaMin      = 1.37
    crackEtaMax      = 1.52
    doPrescale       = False
    prescaleEtLimits = []  # Should be given in ascending order!
    prescaleFactors  = []
    passAll          = False
    """



    def __init__ ( self, name = "EMClusterFilter", **kw ):
        ## initialize base class
        kw['name'] = name
        super(EMClusterFilter, self).__init__(**kw)
        
        ## Define the cuts
        self.clusterCollectionType = kw.get('clusterCollectionType', "CaloClusterContainer")
        self.clusterCollectionName = kw.get('clusterCollectionName', "egClusterCollection")
        self.cutEtMinEMCl     = kw.get('cutEtMinEMCl', 30.0*Units.GeV)
        self.cutEtaMax        = kw.get('cutEtaMax', 2.5)
        self.removeEtaCrack   = kw.get('removeEtaCrack', False)
        self.crackEtaMin      = kw.get('crackEtaMin', 1.37)
        self.crackEtaMax      = kw.get('crackEtaMax', 1.52)
        self.passAll          = kw.get('passAll', False)
        ## Get the storgate handle
        self.storeGateSvc     = None



    def initialize(self):
        self.msg.info( '************************************' )
        self.msg.info( '==> initialize %s...', self.name() )
        self.msg.info( 'Will apply the following cuts:' )
        self.msg.info( '  EtMinEMCl        = %r', self.cutEtMinEMCl )
        self.msg.info( '  EtaMax           = %r', self.cutEtaMax )
        self.msg.info( '  removeEtaCrack   = %r', self.removeEtaCrack )
        self.msg.info( '  crackEtaMin      = %r', self.crackEtaMin )
        self.msg.info( '  crackEtaMax      = %r', self.crackEtaMax )
        self.msg.info( '  passAll          = %r', self.passAll )
        self.msg.info( '************************************' )

        ## Also, declare some counters and initialize them to zero
        self.nProcessed        = 0
        self.nClusters         = 0
        self.nEventEMClPassEta = 0
        self.nEventEMClPassEt  = 0

        ## Import needed modules
        import PyUtils.RootUtils as ru
        ROOT = ru.import_root()

        ## Get the StoreGate service
        self.storeGateSvc = PyAthena.py_svc('StoreGateSvc')
        if self.storeGateSvc is None:
            self.msg.error("Problem retrieving StoreGateSvc pointer !!")
            return StatusCode.Failure
        
        return StatusCode.Success



    def execute(self):
        self.nProcessed+=1
        self.msg.debug( '==> execute %s on %r. event...' % (self.name(), self.nProcessed) )
        
        nEMClPassEta = 0
        nEMClPassEt  = 0
        goodClusters = []

        ## If passAll is selected, accept all events
        if self.passAll :
            self.msg.debug( '%s event passed because passAll is true' % self.name() )
            self.setFilterPassed(True)
            return StatusCode.Success


        #Then... here we go!
        hasCluster  = False
        clusterCollection = None
        try:
            clusterCollection = self.storeGateSvc.retrieve( self.clusterCollectionType, self.clusterCollectionName )
            pass
        except LookupError:
            self.msg.warning( 'Collection %s not found' % self.clusterCollectionName )           
            self.setFilterPassed(True)
            return StatusCode.Success

        for cluster in clusterCollection :
            self.nClusters += 1
            if not hasCluster :
                if abs(cluster.etaBE(2)) < self.cutEtaMax :
                    # Check if the EM calorimeter barrel-encap crack should be removed.
                    if not self.removeEtaCrack or ( self.removeEtaCrack and \
                    ( abs(cluster.etaBE(2)) < self.crackEtaMin or abs(cluster.etaBE(2)) > self.crackEtaMax ) ):
                        # Check if the higher Et cluster is already found.
                        if not hasCluster :
                            nEMClPassEta += 1
                            # If the Et of this cluster is larger than the minimum required Et
                            # of the high-Et cut, then the high-Et cluster is found
                            if cluster.et() > self.cutEtMinEMCl :
                                nEMClPassEt += 1
                                hasCluster = True
                                goodClusters += [cluster]
                                pass
                            pass
                        pass
                    pass
                pass
            pass

        ## Translate the cluster pass counters into event pass counters
        if nEMClPassEta > 0 : self.nEventEMClPassEta += 1
        if nEMClPassEt > 0 : self.nEventEMClPassEt += 1

        ## Check if the event is accepted
        if hasCluster :
            self.msg.debug( '%s event passed' % self.name() )
            self.setFilterPassed(True)
            pass
        else :
            self.msg.debug( '%s event failed' % self.name() )
            self.setFilterPassed(False)
            pass

        return StatusCode.Success



    def finalize(self):
        ## Import needed modules
        import math

        ## Calculate the efficiencies and their errors
        effiClusters        = 0.0
        effiEMClPassEta     = 0.0
        effiEMClPassEt      = 0.0
        effiErrClusters     = 0.0
        effiErrEMClPassEta  = 0.0
        effiErrEMClPassEt   = 0.0
        try :
            # Efficiencies
            effiClusters     = self.nClusters / (1.0*self.nProcessed)
            effiEMClPassEta    = 100.0*self.nEventEMClPassEta / self.nProcessed
            effiEMClPassEt     = 100.0*self.nEventEMClPassEt / self.nProcessed

            # Error on efficiencies
            effiErrClusters     = math.sqrt(self.nClusters) / self.nClusters
            effiErrEMClPassEta    = 100 * math.sqrt(self.nEventEMClPassEta) / self.nProcessed
            effiErrEMClPassEt     = 100 * math.sqrt(self.nEventEMClPassEt) / self.nProcessed
        except ZeroDivisionError :
            self.msg.warning( 'Division by zero error when calculating the uncertainties on the pass efficiencies...' )


        
        self.msg.info( '==> finalize %s...' % self.name() )
        self.msg.info( '***************************************************************' )
        self.msg.info( 'Cut-flow table of %s skimming algorithm:' % self.name() )
        self.msg.info( '-------------' )
        self.msg.info( ' Number of processed events: %r' % self.nProcessed )
        self.msg.info( ' Number of all clusters:   %r and number of clusters per event: %3.3f +/- %3.3f' % ( self.nClusters, effiClusters, effiErrClusters ) )
        self.msg.info( ' Events after eta cut:     %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventEMClPassEta,    effiEMClPassEta,    effiErrEMClPassEta ) )
        self.msg.info( ' Events after E_T cut:     %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventEMClPassEt,     effiEMClPassEt,     effiErrEMClPassEt ) )
        self.msg.info( '***************************************************************' )

        return StatusCode.Success

