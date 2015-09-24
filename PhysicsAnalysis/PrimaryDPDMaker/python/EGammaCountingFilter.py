# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        EGammaCountingFilter
##
## Author:      Karsten Koeneke (DESY)
## Created:     November 2008
##
## Description: This is a short algorithm to select events that contain at
##              least one egamma object and/or an EMCluster.
##=============================================================================

__doc__ = """This is a short algorithm to select events that contain at
least one egamma object and/or an EMCluster.
"""
__version__ = "0.0.2"
__author__  = "Karsten Koeneke <karsten.koeneke@desy.de>"


import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

class EGammaCountingFilter( PyAthena.AthFilterAlgorithm ):
    """
    This class is a short algorithm to select events that contain at
    least one egamma object and/or an EMCluster. The possible
    criteria are (with default values):
    useElectronCollection   = False
    electronCollectionType  = 'ElectronContainer'
    electronCollectionName  = 'ElectronCollection'
    usePhotonCollection     = False
    photonCollectionType    = 'PhotonContainer'
    photonCollectionName    = 'PhotonCollection'
    useEMClusterCollection  = False
    emClusterCollectionType = 'CaloClusterContainer'
    emClusterCollectionName = 'LArClusterEM'
    """



    def __init__ ( self, name = "EGammaCountingFilter", **kw ):
        ## initialize base class
        kw['name'] = name
        super(EGammaCountingFilter, self).__init__(**kw)
        
        ## Define the cuts
        self.useElectronCollection   = kw.get('useElectronCollection', False)
        self.electronCollectionType  = kw.get('electronCollectionType', "ElectronContainer")
        self.electronCollectionName  = kw.get('electronCollectionName', "ElectronCollection")
        self.usePhotonCollection     = kw.get('usePhotonCollection', False)
        self.photonCollectionType    = kw.get('photonCollectionType', "PhotonContainer")
        self.photonCollectionName    = kw.get('photonCollectionName', "PhotonCollection")
        self.useEMClusterCollection  = kw.get('useEMClusterCollection', False)
        self.emClusterCollectionType = kw.get('emClusterCollectionType', "CaloClusterContainer")
        self.emClusterCollectionName = kw.get('emClusterCollectionName', "LArClusterEM")
        ## Get the storgate handle
        self.storeGateSvc            = None



    def initialize(self):
        self.msg.info( '************************************' )
        self.msg.info( '==> initialize %s...', self.name() )
        self.msg.info( 'Will apply the following selections:' )
        self.msg.info( '  using useElectronCollection   = %r', self.useElectronCollection )
        self.msg.info( '  using electronCollectionType  = %r', self.electronCollectionType )
        self.msg.info( '  using electronCollectionName  = %r', self.electronCollectionName )
        self.msg.info( '  using usePhotonCollection     = %r', self.usePhotonCollection )
        self.msg.info( '  using photonCollectionType    = %r', self.photonCollectionType )
        self.msg.info( '  using photonCollectionName    = %r', self.photonCollectionName )
        self.msg.info( '  using useEMClusterCollection  = %r', self.useEMClusterCollection )
        self.msg.info( '  using emClusterCollectionType = %r', self.emClusterCollectionType )
        self.msg.info( '  using emClusterCollectionName = %r', self.emClusterCollectionName )
        self.msg.info( '************************************' )

        ## Also, declare some counters and initialize them to zero
        self.nProcessed      = 0
        self.nElectrons      = 0
        self.nPhotons        = 0
        self.nClusters       = 0
        self.nEGamma         = 0
        self.nEventElectrons = 0
        self.nEventPhotons   = 0
        self.nEventClusters  = 0
        self.nEventEGamma    = 0

        ## Get the StoreGate service
        self.storeGateSvc = PyAthena.py_svc('StoreGateSvc')
        if self.storeGateSvc is None:
            self.msg.error("Problem retrieving StoreGateSvc pointer !!")
            return StatusCode.Failure
        
        return StatusCode.Success




    def execute(self):
        self.nProcessed+=1
        self.msg.debug( '==> execute %s on %r. event...' % (self.name(), self.nProcessed) )
        
        nElectrons = 0
        nPhotons   = 0
        nClusters  = 0
        nEGamma    = 0

        # See if the electron collection got built and contains some electrons
        if self.useElectronCollection :
            try:
                electronCollection = self.storeGateSvc.retrieve( self.electronCollectionType, self.electronCollectionName )
                nElectrons = electronCollection.size()
                nEGamma   += electronCollection.size()
                self.nElectrons += electronCollection.size()
                self.nEGamma    += electronCollection.size()
                pass
            except LookupError:
                self.msg.warning( 'Collection %s not found' % self.electronCollectionName )
                self.setFilterPassed(False)
                return StatusCode.Success
            pass
        

        # See if the photon collection got built and contains some electrons
        if self.usePhotonCollection :
            try:
                photonCollection = self.storeGateSvc.retrieve( self.photonCollectionType, self.photonCollectionName )
                nPhotons = photonCollection.size()
                nEGamma += photonCollection.size()
                self.nPhotons += photonCollection.size()
                self.nEGamma  += photonCollection.size()
                pass
            except LookupError:
                self.msg.warning( 'Collection %s not found' % self.photonCollectionName )
                self.setFilterPassed(False)
                return StatusCode.Success
            pass
        

        # See if the LArCluster collection got built and contains some EMClusters
        if self.useEMClusterCollection :
            try:
                clusterCollection = self.storeGateSvc.retrieve( self.clusterCollectionType, self.clusterCollectionName )
                nClusters = clusterCollection.size()
                nEGamma  += clusterCollection.size()
                self.nClusters += clusterCollection.size()
                self.nEGamma   += clusterCollection.size()
                pass
            except LookupError:
                self.msg.warning( 'Collection %s not found' % self.clusterCollectionName )
                self.setFilterPassed(False)
                return StatusCode.Success
            pass
        

        ## Translate the electron pass counters into event pass counters
        if nElectrons > 0 : self.nEventElectrons += 1
        if nPhotons > 0 :   self.nEventPhotons += 1
        if nClusters > 0 :  self.nEventClusters += 1
        if nEGamma > 0 :    self.nEventEGamma += 1


        ## Check if the event is accepted
        if nEGamma > 0 :
            self.msg.debug( '%s event passed ' % self.name() )
            self.setFilterPassed(True)
            pass
        else :
            self.msg.debug( '%s event failed ' % self.name() )
            self.setFilterPassed(False)
            pass

        return StatusCode.Success




    def finalize(self):
        ## Import needed modules
        import math

        ## Calculate the efficiencies and their errors
        effiElectrons = 0.0
        effiPhotons   = 0.0
        effiClusters  = 0.0
        effiEGamma    = 0.0
        effiEventHasElectron = 0.0
        effiEventHasPhoton   = 0.0
        effiEventHasCluster  = 0.0
        effiEventHasEGamma   = 0.0
        effiErrElectrons = 0.0
        effiErrPhotons   = 0.0
        effiErrClusters  = 0.0
        effiErrEGamma    = 0.0
        effiErrEventHasElectron = 0.0
        effiErrEventHasPhoton   = 0.0
        effiErrEventHasCluster  = 0.0
        effiErrEventHasEGamma   = 0.0
        try :
            # Efficiencies
            effiElectrons = self.nElectrons / (1.0*self.nProcessed)
            effiPhotons   = self.nPhotons / (1.0*self.nProcessed)
            effiClusters  = self.nClusters / (1.0*self.nProcessed)
            effiEGamma    = self.nEGamma / (1.0*self.nProcessed)
            effiEventHasElectron = self.nEventElectrons / (1.0*self.nProcessed)
            effiEventHasPhoton   = self.nEventPhotons / (1.0*self.nProcessed)
            effiEventHasCluster  = self.nEventClusters / (1.0*self.nProcessed)
            effiEventHasEGamma   = self.nEventEGamma / (1.0*self.nProcessed)

            # Error on efficiencies
            effiErrElectrons = math.sqrt(self.nElectrons) / self.nProcessed
            effiErrPhotons   = math.sqrt(self.nPhotons) / self.nProcessed
            effiErrClusters  = math.sqrt(self.nClusters) / self.nProcessed
            effiErrEGamma    = math.sqrt(self.nEGamma) / self.nProcessed
            effiErrEventHasElectron = math.sqrt(self.nEventElectrons) / self.nProcessed
            effiErrEventHasPhoton   = math.sqrt(self.nEventPhotons) / self.nProcessed
            effiErrEventHasCluster  = math.sqrt(self.nEventClusters) / self.nProcessed
            effiErrEventHasEGamma   = math.sqrt(self.nEventEGamma) / self.nProcessed
        except ZeroDivisionError :
            self.msg.warning( 'Division by zero error when calculating the uncertainties on the pass efficiencies...' )


        
        self.msg.info( '==> finalize %s...' % self.name() )
        self.msg.info( '***************************************************************' )
        self.msg.info( 'Cut-flow table of %s skimming algorithm:' % self.name() )
        self.msg.info( '-------------' )
        self.msg.info( ' Number of processed events: %r' % self.nProcessed )
        if self.useElectronCollection :
            self.msg.info( ' Number of all electrons: %r  and number of electrons per event: %3.3f +/- %3.3f' % \
                           ( self.nElectrons, effiElectrons, effiErrElectrons ) )
            pass
        if self.usePhotonCollection :
            self.msg.info( ' Number of all photons: %r    and number of photons per event:   %3.3f +/- %3.3f' % \
                           ( self.nPhotons, effiPhotons, effiErrPhotons ) )
            pass
        if self.useEMClusterCollection :
            self.msg.info( ' Number of all clusters: %r   and number of clusters per event:  %3.3f +/- %3.3f' % \
                           ( self.nClusters, effiClusters, effiErrClusters ) )
            pass
        self.msg.info( ' Number of all egamma: %r     and number of egamma per event:    %3.3f +/- %3.3f' % \
                       ( self.nEGamma, effiEGamma, effiErrEGamma ) )
        if self.useElectronCollection :
            self.msg.info( ' Number of events with electrons: %r  and per-cent of events with electrons: %3.3f +/- %3.3f' % \
                           ( self.nEventElectrons, effiEventHasElectron, effiErrEventHasElectron ) )
            pass
        if self.usePhotonCollection :
            self.msg.info( ' Number of events with photons: %r    and per-cent of events with photons:   %3.3f +/- %3.3f' % \
                           ( self.nEventPhotons, effiEventHasPhoton, effiErrEventHasPhoton ) )
            pass
        if self.useEMClusterCollection :
            self.msg.info( ' Number of events with clusters: %r   and per-cent of events with clusters:  %3.3f +/- %3.3f' % \
                           ( self.nEventClusters, effiEventHasCluster, effiErrEventHasCluster ) )
            pass
        self.msg.info( ' Number of events with egamma: %r     and per-cent of events with egamma:    %3.3f +/- %3.3f' % \
                       ( self.nEventEGamma, effiEventHasEGamma, effiErrEventHasEGamma ) )
        self.msg.info( '***************************************************************' )

        return StatusCode.Success

