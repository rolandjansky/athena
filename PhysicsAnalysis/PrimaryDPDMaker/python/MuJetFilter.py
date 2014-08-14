# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        Mu+Jet Skimmer
##
## Author:      Lorenzo Feligioni
## Created:     April 2010
##
## Based on:    JetFilter 
##
## Description: This is a short algorithm to select events that contain a
##              muon+jet that fulfills a certain set of criteria.
##
##
##============================================================================

__doc__ = """
This is a short algorithm to select events that contain a
muon+jet that fulfills a certain set of criteria.

"""
__version__ = "0.0.0"
__author__  = "Lorenzo Feligioni <Lorenzo.Feligioni@cernNOSP.ch>"


import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

from PrimaryDPDMaker import PrimaryDPDHelpers

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

class MuJetFilter( PyAthena.AthFilterAlgorithm ):
    """
    This class is an algorithm which does the following things:

    1) Check whether the relevant triggers are fired
    2) Check whether a muon+jet was found
    3) Apply prescale to let non-muon+jet events in to fill the DPD (...ops DESD) budget


    This class is a short algorithm to select events that contain one 
    jet which fulfills a certain set of criteria.  All energies below
    are uncalibrated (EM scale).  The possible criteria are (with
    default values):
    jetCollectionType       = "JetCollection"
    jetCollectionName       = "AntiKt4TowerJets"
    useSoftMuonTagSelection = True
    cutEtMin                = 0.0*Units.GeV
    cutEtMax                = 14.0*Units.GeV
    cutEtaMax               = 2.5
    passAll                 = False
    """

    def __init__ ( self, name = "MuJetFilter", **kw ):
        ## initialize base class
        kw['name'] = name
        super(MuJetFilter, self).__init__(**kw)

        ## Define the cuts
        self.jetCollectionType       = kw.get('jetCollectionType', "JetCollection")
        self.jetCollectionName       = kw.get('jetCollectionName', "AntiKt4TowerJets")
        self.leptonContainerTypeList = kw.get('leptonContainerTypeList', ['Analysis::MuonContainer','Analysis::MuonContainer'])
        self.leptonContainerNameList = kw.get('leptonContainerNameList', ["StacoMuonCollection","MuidMuonCollection"])
        self.useSoftMuonTagSelection = kw.get('useSoftMuonTagSelection', True)
        self.cutEtMin                = kw.get('cutEtMin', 0.0*Units.GeV)
        self.cutEtMax                = kw.get('cutEtMax', 14.0*Units.GeV)
        self.cutEtaMax               = kw.get('cutEtaMax', 2.5)
        self.passAll                 = kw.get('passAll', False)

        ## Get the storgate handle
        self.storeGateSvc = None

        ## Handle to JetSignalState
        self.jss = None

        return


        
    def initialize(self):
        self.msg.info( '************************************' )
        self.msg.info( '==> initialize %s...', self.name() )
        self.msg.info( '  jetCollectionType       = %r', self.jetCollectionType )
        self.msg.info( '  jetCollectionName       = %r', self.jetCollectionName )
        self.msg.info( '  EtMin                   = %r', self.cutEtMin )
        self.msg.info( '  EtMax                   = %r', self.cutEtMax )
        self.msg.info( '  EtaMax                  = %r', self.cutEtaMax )
        self.msg.info( '  passAll                 = %r', self.passAll )
        self.msg.info( '************************************' )

        ## Initialize the counters
        self.nProcessed     = 0
        self.nJets          = 0
        self.nMuJets        = 0
        self.nEventPassed   = 0
        
        ## Import needed modules
        import PyUtils.RootUtils as ru
        ROOT = ru.import_root()

        ## Get the StoreGate service
        self.storeGateSvc = PyAthena.py_svc('StoreGateSvc')
        if self.storeGateSvc is None:
            self.msg.error("Problem retrieving StoreGateSvc pointer !!")
            return StatusCode.Failure

        ## Eagerly load library due to reflex bug (autoloading enums/namespaces doesn't work)
        PyAthena.load_library ('FourMomUtils')

        
        ## import some 4-mom utils
        import FourMomUtils.Bindings
        self.utils = { 'deltaR' : PyAthena.P4Helpers.deltaR }

        return StatusCode.Success


    def execute(self):
        
        self.nProcessed+=1
        self.msg.debug( '==> execute %s on %r. event...' % (self.name(), self.nProcessed) )
        isPassed=False
        
        # Load the jet collection from the input file
        try:
            jetCollection = self.storeGateSvc.retrieve( self.jetCollectionType, self.jetCollectionName )
            self.msg.debug( 'Loading the jet collection %s from the input file.' % self.jetCollectionName )
            pass
        except LookupError:
            self.msg.warning( 'Collection %s not found' % self.jetCollectionName )           
            self.setFilterPassed(False)
            return StatusCode.Success   

        
        # Load the lepton collections from the input file
        self.msg.debug( 'Going to load the leptons of type %s and key %s.' % (self.leptonContainerTypeList, self.leptonContainerNameList) )
        leptonList = []
        if self.leptonContainerTypeList.__len__() == self.leptonContainerNameList.__len__() :
            for i in range( self.leptonContainerTypeList.__len__() ) :
                try:
                    lepColl = self.storeGateSvc.retrieve( self.leptonContainerTypeList[i],
                                                          self.leptonContainerNameList[i] )
                    self.msg.debug( 'Loading the lepton collection %s from the input file.'
                                    % self.leptonContainerNameList[i] )
                    for lep in lepColl :
                        leptonList.append(lep)
                        pass
                    pass
                except LookupError:
                    self.msg.warning( 'Collection %s not found' % self.leptonContainerNameList[i] )           
                    self.setFilterPassed(False)
                    return StatusCode.Success   
                pass
            pass
        else :
            self.msg.error( 'List of leptons type %s and key %s has different lenght!'
                            % (self.leptonContainerTypeList, self.leptonContainerNameList) )
            pass


        # Create a class-member list of all jets that passed all cuts.
        # This can then be used by other filters to do overlap removal, etc.
        VIEW_ELEMENTS = 1
        goodJets = PyAthena.JetCollection(VIEW_ELEMENTS)


        # Print a debug message
        self.msg.debug( 'Starting loop over all jets. In this event, there are %s jets' % jetCollection.size() )
        

        ## Find muon jet
        leadEt = 0.0*Units.GeV
        jet_pass=0
        for jet in jetCollection :
            self.nJets += 1
            if abs(jet.eta()) < self.cutEtaMax :
                if jet.et() > self.cutEtMin:
                    if self.useSoftMuonTagSelection :
                        flw = jet.getFlavourTagWeight("SoftMuonTag")
                        self.msg.debug( 'Found jet, in this event, with a value of %s for the SoftMuonTag ' % jet.getFlavourTagWeight("SoftMuonTag"))
                        if flw>-100 : 
                            self.msg.debug( 'Found muon jet in this event' )
                            self.nMuJets += 1
                            isPassed=True
                        pass
                    pass
                pass
            pass

        if isPassed :
            self.nEventPassed += 1
            self.msg.debug( 'Event passed MuJetFilter' )
            
        
        self.setFilterPassed(isPassed)
        
        
        return StatusCode.Success


    
    def finalize(self):
        ## Import needed modules
        import math

        ## Calculate the efficiencies and their errors
        effiMuJets      = 0.0
        effiJets      = 0.0
        effiEvents    = 0.0
        effiErrJets   = 0.0
        effiErrMuJets   = 0.0
        effiErrEvents = 0.0
        try :
            # Efficiencies
            effiMuJets      = self.nMuJets / float(self.nProcessed)
            effiJets        = self.nJets / float(self.nProcessed)
            effiEvents      = self.nEventPassed / float(self.nProcessed)

            # Error on efficiencies
            effiErrMuJets   = math.sqrt(self.nMuJets) / float(self.nProcessed)
            effiErrJets     = math.sqrt(self.nJets) / float(self.nProcessed)
            effiErrEvents   = 100.0*math.sqrt(effiEvents*(1.-effiEvents)/float(self.nProcessed))

            effiEvents *= 100.
            
        except ZeroDivisionError :
            self.msg.warning( 'Division by zero error when calculating the uncertainties on the pass efficiencies...' )


        
        self.msg.info( '==> finalize %s...' % self.name() )
        self.msg.info( '***************************************************************' )
        self.msg.info( 'Cut-flow table of %s skimming algorithm:' % self.name() )
        self.msg.info( '-------------' )
        self.msg.info( ' Number of processed events: %r' % self.nProcessed )
        self.msg.info( ' Number of all jets:    %r and number of jets per event   : %3.3f +/- %3.3f' % ( self.nJets, effiJets, effiErrJets ) )
        self.msg.info( ' Number of all mu jets: %r and number of mu jets per event: %3.3f +/- %3.3f' % ( self.nJets, effiJets, effiErrJets ) )
        self.msg.info( ' Events accepted:    %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPassed, effiEvents, effiErrEvents ) )
        self.msg.info( '***************************************************************' )

        return StatusCode.Success

