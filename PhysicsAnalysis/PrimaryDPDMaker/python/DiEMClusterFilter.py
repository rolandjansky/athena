# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        Di-EM-Cluster Filter
##
## Author:      Karsten Koeneke (DESY)
## Created:     April 2008
##
## Description: This is a short algorithm to select events that contain two
##              EM clusters which fulfill a certain set of criteria.
##=============================================================================

__doc__ = """This is a short algorithm to select events that contain two 
EM clusters which fulfill a certain set of criteria.
"""
__version__ = "0.0.2"
__author__  = "Karsten Koeneke <karsten.koeneke@desy.de>"


import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

class DiEMClusterFilter( PyAthena.AthFilterAlgorithm ):
    """
    This class is a short algorithm to select events that contain two 
    EM clusters which fulfill a certain set of criteria. The possible
    criteria are (with default values):
    cutEtMinEMCl1    = 12.0*Units.GeV)
    cutEtMinEMCl2    = 12.0*Units.GeV)
    cutEtaMax      = 2.5
    removeEtaCrack = False
    crackEtaMin    = 1.37
    crackEtaMax    = 1.52
    passAll        = False
    """



    def __init__ ( self, name = "DiEMClusterFilter", **kw ):
        ## initialize base class
        kw['name'] = name
        super(DiEMClusterFilter, self).__init__(**kw)
        
        ## Define the cuts
        self.clusterCollectionType = kw.get('clusterCollectionType', "CaloClusterContainer")
        self.clusterCollectionName = kw.get('clusterCollectionName', "egClusterCollection")
        self.cutEtMinEMCl1    = kw.get('cutEtMinEMCl1', 12.0*Units.GeV)
        self.cutEtMinEMCl2    = kw.get('cutEtMinEMCl2', 12.0*Units.GeV)
        self.cutEtaMax      = kw.get('cutEtaMax', 2.5)
        self.removeEtaCrack = kw.get('removeEtaCrack', False)
        self.crackEtaMin    = kw.get('crackEtaMin', 1.37)
        self.crackEtaMax    = kw.get('crackEtaMax', 1.52)
        self.passAll        = kw.get('passAll', False)
        ## Get the storgate handle
        self.storeGateSvc   = None



    def initialize(self):
        self.msg.info( '************************************' )
        self.msg.info( '==> initialize %s...', self.name() )
        self.msg.info( 'Will apply the following cuts:' )
        self.msg.info( '  EtMinEMCl1       = %r', self.cutEtMinEMCl1 )
        self.msg.info( '  EtMinEMCl2       = %r', self.cutEtMinEMCl2 )
        self.msg.info( '  EtaMax         = %r', self.cutEtaMax )
        self.msg.info( '  removeEtaCrack = %r', self.removeEtaCrack )
        self.msg.info( '  crackEtaMin    = %r', self.crackEtaMin )
        self.msg.info( '  crackEtaMax    = %r', self.crackEtaMax )
        self.msg.info( '  passAll        = %r', self.passAll )
        self.msg.info( '************************************' )

        ## Also, declare some counters and initialize them to zero
        self.nProcessed         = 0
        self.nClusters          = 0
        self.nEventEMCl1PassEta = 0
        self.nEventEMCl2PassEta = 0
        self.nEventEMCl1PassEt  = 0
        self.nEventEMCl2PassEt  = 0

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

        nEMCl1PassEta    = 0
        nEMCl2PassEta    = 0
        nEMCl1PassEt     = 0
        nEMCl2PassEt     = 0

        ## If passAll is selected, accept all events
        if self.passAll :
            self.msg.debug( '%s event passed because passAll is true' % self.name() )
            self.setFilterPassed(True)
            return StatusCode.Success


        #Then... here we go!
        hasFirstCluster  = False
        hasSecondCluster = False
        clusterCollection = None
        try:
            clusterCollection = self.storeGateSvc.retrieve( self.clusterCollectionType, self.clusterCollectionName )
            pass
        except LookupError:
            self.msg.warning( 'Collection %s not found' % self.clusterCollectionName )           
            self.setFilterPassed(False)
            return StatusCode.Success
            
        for cluster in clusterCollection :
            self.nClusters += 1
            if not hasFirstCluster or not hasSecondCluster:
                if abs(cluster.eta()) < self.cutEtaMax :
                    # Check if the EM calorimeter barrel-encap crack should be removed.
                    if not self.removeEtaCrack or ( self.removeEtaCrack and \
                    ( abs(cluster.eta()) < self.crackEtaMin or abs(cluster.eta()) > self.crackEtaMax ) ):
                        # Check if the higher Et cluster is already found.
                        if not hasFirstCluster :
                            nEMCl1PassEta += 1
                            # If the Et of this cluster is larger than the minimum required Et
                            # of the high-Et cut, then the high-Et cluster is found
                            if cluster.et() > self.cutEtMinEMCl1 :
                                nEMCl1PassEt += 1
                                hasFirstCluster = True
                            # If this cluster doesn't pass the higher-Et cut, maybe it passes the
                            # lower-Et cut; but only if the lower Et cluster is not found yet.
                            elif not hasSecondCluster and cluster.et() > self.cutEtMinEMCl2 :
                                nEMCl2PassEt += 1
                                nEMCl1PassEta -= 1
                                nEMCl2PassEta += 1
                                hasSecondCluster = True
                        else :
                            nEMCl2PassEta += 1
                            if cluster.et() > self.cutEtMinEMCl2 :
                                nEMCl2PassEt += 1
                                hasSecondCluster = True

        ## Translate the cluster pass counters into event pass counters
        if nEMCl1PassEta > 0 : self.nEventEMCl1PassEta += 1
        if nEMCl2PassEta > 0 : self.nEventEMCl2PassEta += 1
        if nEMCl1PassEt > 0 : self.nEventEMCl1PassEt += 1
        if nEMCl2PassEt > 0 : self.nEventEMCl2PassEt += 1

        ## Check if the event is accepted
        if hasFirstCluster and hasSecondCluster :
            self.msg.debug( '%s event passed' % self.name() )
            self.setFilterPassed(True)
        else:
            self.msg.debug( '%s event failed' % self.name() )
            self.setFilterPassed(False)
        return StatusCode.Success



    def finalize(self):
        ## Import needed modules
        import math

        ## Calculate the efficiencies and their errors
        effiClusters     = 0.0
        effiEMCl1PassEta    = 0.0
        effiEMCl1PassEt     = 0.0
        effiEMCl2PassEta    = 0.0
        effiEMCl2PassEt     = 0.0
        effiErrClusters     = 0.0
        effiErrEMCl1PassEta    = 0.0
        effiErrEMCl1PassEt     = 0.0
        effiErrEMCl2PassEta    = 0.0
        effiErrEMCl2PassEt     = 0.0
        try :
            # Efficiencies
            effiClusters     = self.nClusters / (1.0*self.nProcessed)
            effiEMCl1PassEta    = 100.0*self.nEventEMCl1PassEta / self.nProcessed
            effiEMCl1PassEt     = 100.0*self.nEventEMCl1PassEt / self.nProcessed
            effiEMCl2PassEta    = 100.0*self.nEventEMCl2PassEta / self.nProcessed
            effiEMCl2PassEt     = 100.0*self.nEventEMCl2PassEt / self.nProcessed

            # Error on efficiencies
            effiErrClusters     = math.sqrt(self.nClusters) / self.nClusters
            effiErrEMCl1PassEta    = 100 * math.sqrt(self.nEventEMCl1PassEta) / self.nProcessed
            effiErrEMCl1PassEt     = 100 * math.sqrt(self.nEventEMCl1PassEt) / self.nProcessed
            effiErrEMCl2PassEta    = 100 * math.sqrt(self.nEventEMCl2PassEta) / self.nProcessed
            effiErrEMCl2PassEt     = 100 * math.sqrt(self.nEventEMCl2PassEt) / self.nProcessed
        except ZeroDivisionError :
            self.msg.warning( 'Division by zero error when calculating the uncertainties on the pass efficiencies...' )


        
        self.msg.info( '==> finalize %s...' % self.name() )
        self.msg.info( '***************************************************************' )
        self.msg.info( 'Cut-flow table of %s skimming algorithm:' % self.name() )
        self.msg.info( '-------------' )
        self.msg.info( ' Number of processed events: %r' % self.nProcessed )
        self.msg.info( ' Number of all clusters:    %r and number of clusters per event: %3.3f +/- %3.3f' % ( self.nClusters, effiClusters, effiErrClusters ) )
        self.msg.info( ' Events after first eta cut:     %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventEMCl1PassEta,    effiEMCl1PassEta,    effiErrEMCl1PassEta ) )
        self.msg.info( ' Events after first E_T cut:     %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventEMCl1PassEt,     effiEMCl1PassEt,     effiErrEMCl1PassEt ) )
        self.msg.info( ' Events after second eta cut:    %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventEMCl2PassEta,    effiEMCl2PassEta,    effiErrEMCl2PassEta ) )
        self.msg.info( ' Events after second E_T cut:    %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventEMCl2PassEt,     effiEMCl2PassEt,     effiErrEMCl2PassEt ) )
        self.msg.info( '***************************************************************' )

        return StatusCode.Success

