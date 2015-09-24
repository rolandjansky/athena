# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        Z -> ee Skimmer
##
## Author:      Karsten Koeneke (DESY)
## Created:     April 2008
##
## Description: This is a short algorithm to select events that contain Z -> ee
##              which fulfill a certain set of criteria.
##=============================================================================

__doc__ = """This is a short algorithm to select events that contain Z -> ee
which fulfill a certain set of criteria.
"""
__version__ = "0.0.1"
__author__  = "Karsten Koeneke <karsten.koeneke@desy.de>"


import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

from PrimaryDPDMaker import PrimaryDPDHelpers

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

import math


class RAWZeeFilter( PyAthena.AthFilterAlgorithm ):
    """
    This class is a short algorithm to select events that contain Z -> ee
    which fulfill a certain set of criteria. The possible
    criteria are (with default values):

    the type of electrons by its jetCollectionName:
    electronCollectionType  = 'ElectronContainer'
    electronCollectionName  = 'ElectronAODCollection'
    
    minimum E_T of both electrons:
    cutEtMin                = 15.0*Units.GeV 

    maximum |eta| of both electrons:
    cutEtaMax               = 2.5 

    author selection of both electrons:
    cutAuthor             = 'egamma'

    IsEM selectrion of both electrons:
    cutIsEMEl               = 'Medium'
    
    di-electron mass lower bound:
    cutDiElMassLower        = 60.0*Units.GeV

    di-electron mass upper bound:
    cutDiElMassUpper        = 120.0*Units.GeV

    pass all events if true:
    passAll                 = False
    """



    def __init__ ( self, name = "RAWZeeFilter", **kw ):
        # initialize base class
        kw['name'] = name
        super(RAWZeeFilter, self).__init__(**kw)

        # Define the cuts
        self.clusterCollectionType  = kw.get('clusterCollectionType', "CaloClusterContainer")
        self.clusterCollectionName  = kw.get('clusterCollectionName', "egClusterCollection")
        self.electronCollectionType = kw.get('electronCollectionType', "ElectronContainer")
        self.electronCollectionName = kw.get('electronCollectionName', "ElectronAODCollection")
        self.cutEtMin               = kw.get('cutEtMin', -10.0*Units.GeV)
        self.cutEtaMax              = kw.get('cutEtaMax', 5.0)
        self.cutAuthor              = kw.get('cutAuthor', "either") # Possible values: 'softe', 'egamma', 'either'
        self.cutIsEM                = kw.get('cutIsEM', "Loose") # Possible values: "None", "Loose", 'Medium', 'Tight', 'TightNoIsolation'
        self.removeEtaCrack         = kw.get('removeEtaCrack', False)
        self.crackEtaMin            = kw.get('crackEtaMin', 1.37)
        self.crackEtaMax            = kw.get('crackEtaMax', 1.52)
        self.cutMassLower           = kw.get('cutMassLower', -10.0*Units.GeV)
        self.cutMassUpper           = kw.get('cutMassUpper', 100000.0*Units.GeV)
        self.passAll                = kw.get('passAll', False)
        self.storeGateSvc     = None


    def initialize(self):
        self.msg.info( '************************************' )
        self.msg.info( '==> initialize %s...', self.name() )
        self.msg.info( 'Will apply the following cuts:' )
        self.msg.info( '  using cluster collection   = %r %r' % (self.clusterCollectionType, self.clusterCollectionName) )
        self.msg.info( '  using electron collection  = %r %r' % (self.electronCollectionType, self.electronCollectionName) )
        self.msg.info( '  EtMin                      = %r', self.cutEtMin )
        self.msg.info( '  EtaMax                     = %r', self.cutEtaMax )
        self.msg.info( '  IsEmEl                     = %s', self.cutIsEM )
        self.msg.info( '  AuthorEl                   = %s', self.cutAuthor )
        self.msg.info( '  MassLower                  = %r', self.cutMassLower )
        self.msg.info( '  MassUpper                  = %r', self.cutMassUpper )
        self.msg.info( '  removeEtaCrack             = %r', self.removeEtaCrack )
        self.msg.info( '  crackEtaMin                = %r', self.crackEtaMin )
        self.msg.info( '  crackEtaMax                = %r', self.crackEtaMax )
        self.msg.info( '  passAll                    = %r', self.passAll )
        self.msg.info( '************************************' )

        # Also, declare some counters and initialize them to zero
        self.nProcessed       = 0
        self.nElectrons       = 0
        self.nGoodElectrons   = 0
        self.nEventPassEta    = 0
        self.nEventPassEt     = 0
        self.nEventPassAuthor = 0
        self.nEventPassIsEM   = 0
        self.nEventPassClEta  = 0
        self.nEventPassClEt   = 0
        self.nEventPassMass   = 0

        # Import needed modules
        import PyUtils.RootUtils as ru
        ROOT = ru.import_root()

        # Get the StoreGate service
        self.storeGateSvc = PyAthena.py_svc('StoreGateSvc')
        if self.storeGateSvc is None:
            self.msg.error("Problem retrieving StoreGateSvc pointer !!")
            return StatusCode.Failure
        
        return StatusCode.Success



    def execute(self):
        self.nProcessed+=1
        self.msg.debug( '==> execute %s on %r. event...' % (self.name(), self.nProcessed) )
        
        import PyUtils.RootUtils as ru
        ROOT = ru.import_root()
        from ROOT import TLorentzVector
        nPassEta    = 0
        nPassEt     = 0
        nPassAuthor = 0
        nPassIsEM   = 0
        nPassClEta  = 0
        nPassClEt   = 0
        nPassMass   = 0

        # Create a list of good electron TLorentzVectors
        goodElectronTLVList = []


        # If passAll is selected, accept all events
        if self.passAll :
            self.msg.debug( '%s event passed because passAll is true' % self.name() )
            self.setFilterPassed(True)
            return StatusCode.Success


        # Then... here we go!
        acceptEvent  = False
        # Get the electron collections from StoreGate
        clusterCollection = None
        try:
            clusterCollection  = self.storeGateSvc.retrieve( self.clusterCollectionType, self.clusterCollectionName )
            pass
        except LookupError:
            self.msg.warning( 'Collection %s not found' % self.clusterCollectionName )           
            self.setFilterPassed(False)
            return StatusCode.Success

        electronCollection = None
        try:
            electronCollection = self.storeGateSvc.retrieve( self.electronCollectionType, self.electronCollectionName )
            pass
        except LookupError:
            self.msg.warning( 'Collection %s not found' % self.electronCollectionName )           
            self.setFilterPassed(False)
            return StatusCode.Success

        #look for two electrons 
        for i in range( electronCollection.size() ) :
            self.nElectrons += 1
            electron1 = electronCollection[i]
            # Check if the eta requirements are fullfilled
            if abs(electron1.cluster().eta()) < self.cutEtaMax :
                # Check if the EM calorimeter barrel-encap crack should be removed.
                if not self.removeEtaCrack or ( self.removeEtaCrack \
                                                and ( abs(electron1.cluster().eta()) < self.crackEtaMin or abs(electron1.cluster().eta()) > self.crackEtaMax ) ) :
                    #Update event counter...
                    nPassEta += 1
                    if electron1.cluster().et() > self.cutEtMin :
                        #Update event counter...
                        nPassEt += 1

                        # Check that the electron in question comes from the right reconstruction algorithm using PrimaryDPDHelpers
                        if PrimaryDPDHelpers.checkEgammaAuthor( electron1, self.cutAuthor ) :
                            nPassAuthor += 1
                            # Check the quality of the electron using PrimaryDPDHelpers
                            if PrimaryDPDHelpers.checkElectronIsEM( electron1, self.cutIsEM ) :
                                nPassIsEM += 1                                    
                                self.nGoodElectrons += 1
                                electronTLVec = TLorentzVector( electron1.cluster().px(),
                                                                electron1.cluster().py(),
                                                                electron1.cluster().pz(),
                                                                electron1.cluster().e() )
                                goodElectronTLVList.append( electronTLVec )
                                pass  # End: Check IsEM
                            pass  # End: Check Author
                        pass  # End: Check Et
                    pass  # End: Check eta crack removal
                pass  # End: Check eta acceptance
            pass  # End: for i in range( electronCollection.size() )


        for cluster in clusterCollection :
            # Stop the loop when a good candidate was found
            if not acceptEvent :
                # Check if the eta requirements are fullfilled
                if abs(cluster.eta()) < self.cutEtaMax :
                    # Check if the EM calorimeter barrel-encap crack should be removed.
                    if not self.removeEtaCrack or \
                           (self.removeEtaCrack and ( abs(cluster.eta()) < self.crackEtaMin or abs(cluster.eta()) > self.crackEtaMax ) ):
                        # Update event counter...
                        nPassClEta += 1
                        if cluster.et() > self.cutEtMin :
                            # Update event counter...
                            nPassClEt += 1
            
                            # Calculate the di-jet invariant mass using TLorentzVectors from ROOT
                            clusterTLVec  = TLorentzVector( cluster.px(), cluster.py(), cluster.pz(), cluster.e() )

                            for electronTLV in goodElectronTLVList :
                                electronclusterTLVec = electronTLV + clusterTLVec
                                diEmMass = electronclusterTLVec.M()

                                # Select events inside the invariant di-electron mass window
                                if diEmMass > self.cutMassLower and diEmMass < self.cutMassUpper :
                                    nPassMass += 1
                                    acceptEvent = True
                                    pass  # End: Check Mass 

                                pass # End: loop over the good electrons
                            pass # End: if cluster.et()
                        pass # End: if removeCrack
                    pass # End: if cluster.eta()
                pass # End: if not acceptEvent
            pass # End: loop over clusters



        ## Translate the electron pass counters into event pass counters
        if nPassEta > 0 :    self.nEventPassEta    += 1
        if nPassEt > 0 :     self.nEventPassEt     += 1
        if nPassAuthor > 0 : self.nEventPassAuthor += 1
        if nPassIsEM > 0 :   self.nEventPassIsEM   += 1
        if nPassClEta > 0 :  self.nEventPassClEta  += 1
        if nPassClEt > 0 :   self.nEventPassClEt   += 1
        if nPassMass > 0 :   self.nEventPassMass   += 1

        ## Check if the event is accepted
        if acceptEvent :
            self.msg.debug( '%s event passed' % self.name() )
            self.setFilterPassed(True)
            pass
        else:
            self.msg.debug( '%s event failed' % self.name() )
            self.setFilterPassed(False)
            pass
        return StatusCode.Success



    def finalize(self):
        ## Import needed modules
        import math

        ## Calculate the efficiencies and their errors
        effiElectrons     = 0.0
        effiGoodElectrons = 0.0
        effiPassEta       = 0.0
        effiPassEt        = 0.0
        effiPassAuthor    = 0.0
        effiPassIsEM      = 0.0
        effiPassClEta     = 0.0
        effiPassClEt      = 0.0
        effiPassMass      = 0.0
        effiErrElectrons     = 0.0
        effiErrGoodElectrons = 0.0
        effiErrPassEta       = 0.0
        effiErrPassEt        = 0.0
        effiErrPassAuthor    = 0.0
        effiErrPassIsEM      = 0.0
        effiErrPassClEta     = 0.0
        effiErrPassClEt      = 0.0
        effiErrPassMass      = 0.0
        try :
            # Efficiencies
            effiElectrons     = self.nElectrons / (1.0*self.nProcessed)
            effiGoodElectrons = self.nGoodElectrons / (1.0*self.nProcessed)
            effiPassEta       = 100.0*self.nEventPassEta / self.nProcessed
            effiPassEt        = 100.0*self.nEventPassEt / self.nProcessed
            effiPassAuthor    = 100.0*self.nEventPassAuthor / self.nProcessed
            effiPassIsEM      = 100.0*self.nEventPassIsEM / self.nProcessed
            effiPassClEta     = 100.0*self.nEventPassClEta / self.nProcessed
            effiPassClEt      = 100.0*self.nEventPassClEt / self.nProcessed
            effiPassMass      = 100.0*self.nEventPassMass / self.nProcessed

            # Error on efficiencies
            effiErrElectrons     = math.sqrt(self.nElectrons) / self.nElectrons
            effiErrGoodElectrons = math.sqrt(self.nGoodElectrons) / self.nGoodElectrons
            effiErrPassEta       = 100.0 * math.sqrt(self.nEventPassEta) / self.nProcessed
            effiErrPassEt        = 100.0 * math.sqrt(self.nEventPassEt) / self.nProcessed
            effiErrPassAuthor    = 100.0 * math.sqrt(self.nEventPassAuthor) / self.nProcessed
            effiErrPassIsEM      = 100.0 * math.sqrt(self.nEventPassIsEM) / self.nProcessed
            effiErrPassClEta     = 100.0 * math.sqrt(self.nEventPassClEta) / self.nProcessed
            effiErrPassClEt      = 100.0 * math.sqrt(self.nEventPassClEt) / self.nProcessed
            effiErrPassMass      = 100.0 * math.sqrt(self.nEventPassMass) / self.nProcessed
            pass
        except ZeroDivisionError :
            self.msg.warning( 'Division by zero error when calculating the uncertainties on the pass efficiencies...' )
            pass

        
        self.msg.info( '==> finalize %s...' % self.name() )
        self.msg.info( '***************************************************************' )
        self.msg.info( 'Cut-flow table of %s skimming algorithm:' % self.name() )
        self.msg.info( '-------------' )
        self.msg.info( ' Number of processed events: %r' % self.nProcessed )
        self.msg.info( ' Number of all electrons:                     %r and number of electrons per event: %3.3f +/- %3.3f'  % ( self.nElectrons,     effiElectrons,     effiErrElectrons ) )
        self.msg.info( ' Number of electrons after E_T and eta cuts:  %r and number of electrons per event: %3.3f +/- %3.3f'  % ( self.nGoodElectrons, effiGoodElectrons, effiErrGoodElectrons ) )
        self.msg.info( ' Events after electron eta cut:               %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPassEta,       effiPassEta,       effiErrPassEta ) )
        self.msg.info( ' Events after electron E_T cut:               %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPassEt,        effiPassEt,        effiErrPassEt ) )
        self.msg.info( ' Events after electron author requirement:    %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPassAuthor,    effiPassAuthor,    effiErrPassAuthor ) )
        self.msg.info( ' Events after electron IsEM requirement:      %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPassIsEM,      effiPassIsEM,      effiErrPassIsEM ) )
        self.msg.info( ' Events after cluster eta cut:                %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPassClEta,     effiPassClEta,     effiErrPassClEta ) )
        self.msg.info( ' Events after cluster E_T cut:                %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPassClEt,      effiPassClEt,      effiErrPassClEt ) )
        self.msg.info( ' Events after di-em invariant mass cut:       %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPassMass,      effiPassMass,      effiErrPassMass ) )
        self.msg.info( '***************************************************************' )

        return StatusCode.Success

