# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        Fancy Jet Skimmer (allows to also cut on jet moments)
##
## Author:      Andy Haas (NYU)
## Created:     Nov. 2013
##
## Based on:    JetFilter.py
##
## Description: This is a short algorithm to select events that contain a jet
##              that fulfills a certain set of criteria, including jet moments
##
##
##============================================================================

__doc__ = """This is a short algorithm to select events that contain one 
jet which fulfills a certain set of criteria, including jet moments.
"""
__version__ = "0.0.1"
__author__  = "Andy Haas <ahaas@cern.ch>"


import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

import PyUtils.RootUtils as ru
ROOT = ru.import_root()


## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

class FancyJetFilter( PyAthena.AthFilterAlgorithm ):
    """
    This class is a short algorithm to select events that contain one 
    jet which fulfills a certain set of criteria.  All energies below
    are uncalibrated (EM scale).  The possible criteria are (with
    default values):
    jetCollectionType       = "xAOD::JetCollection"
    jetCollectionName       = "AntiKt4LCTopoJets"
    useUncalibratedJets     = True
    cutEtMin                = 0.0*Units.GeV
    cutEtMax                = 14.0*Units.TeV
    cutEtaMin               = 0
    cutEtaMax               = 2.5
    cutSumPtTrkMax          = -1.0
    removeOverlap           = False
    deltaR                  = 0.2
    leptonContainerTypeList = []
    leptonContainerNameList = []
    nLeadingsForCheck       = 10000000
    minNumberPassed         = 1
    recordGoodJets          = False
    goodJetCollectionName   = self.name()
    passAll                 = False
    """



    def __init__ ( self, name = "FancyJetFilter", **kw ):
        ## initialize base class
        kw['name'] = name
        super(FancyJetFilter, self).__init__(**kw)

        ## Define the cuts
        self.jetCollectionType       = kw.get('jetCollectionType', "xAOD::JetContainer")
        self.jetCollectionName       = kw.get('jetCollectionName', "AntiKt4LCTopoJets")
        self.useUncalibratedJets     = kw.get('useUncalibratedJets', True)
        self.cutEtMin                = kw.get('cutEtMin', 0.0*Units.GeV)
        self.cutEtMax                = kw.get('cutEtMax', 14.0*Units.TeV)
        self.cutEtaMin               = kw.get('cutEtaMin', 0.0)
        self.cutEtaMax               = kw.get('cutEtaMax', 2.5)
        self.cutSumPtTrkMax          = kw.get('cutSumPtTrkMax', -1.0)
        self.removeOverlap           = kw.get('removeOverlap', False)
        self.deltaR                  = kw.get('deltaR', 0.2)
        self.leptonContainerTypeList = kw.get('leptonContainerTypeList', [])
        self.leptonContainerNameList = kw.get('leptonContainerNameList', [])
        self.nLeadingsForCheck       = kw.get('nLeadingsForCheck', 10000000)
        self.minNumberPassed         = kw.get('minNumberPassed', 1)
        self.recordGoodJets          = kw.get('recordGoodJets', False)
        self.goodJetCollectionName   = kw.get('goodJetCollectionName', self.name() )
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
        self.msg.info( '  recordGoodJets          = %r', self.recordGoodJets )
        self.msg.info( '  goodJetCollectionName   = %r', self.goodJetCollectionName )
        self.msg.info( 'Will apply the following cuts:' )
        self.msg.info( '  useUncalibratedJets     = %r', self.useUncalibratedJets )
        self.msg.info( '  EtMin (GeV)             = %r', self.cutEtMin/Units.GeV )
        self.msg.info( '  EtMax (GeV)             = %r', self.cutEtMax/Units.GeV )
        self.msg.info( '  EtaMin                  = %r', self.cutEtaMin )
        self.msg.info( '  EtaMax                  = %r', self.cutEtaMax )
        self.msg.info( '  cutSumPtTrkMax          = %r', self.cutSumPtTrkMax )
        self.msg.info( '  removeOverlap           = %r', self.removeOverlap )
        self.msg.info( '  deltaR                  = %r', self.deltaR )
        self.msg.info( '  leptonContainerTypeList = %r', self.leptonContainerTypeList )
        self.msg.info( '  leptonContainerNameList = %r', self.leptonContainerNameList )
        self.msg.info( '  nLeadingsForCheck       = %r', self.nLeadingsForCheck )
        self.msg.info( '  minNumberPassed         = %r', self.minNumberPassed )
        self.msg.info( '  passAll                 = %r', self.passAll )
        self.msg.info( '************************************' )

        ## Initialize the counters
        self.nProcessed     = 0
        self.nJets          = 0
        self.nEventPassed   = 0

        ## Import needed modules
        import PyUtils.RootUtils as ru
        ROOT = ru.import_root()  # noqa: F841

        ## Get the StoreGate service
        self.storeGateSvc = PyAthena.py_svc('StoreGateSvc')
        if self.storeGateSvc is None:
            self.msg.error("Problem retrieving StoreGateSvc pointer !!")
            return StatusCode.Failure

        ## Eagerly load library due to reflex bug (autoloading enums/namespaces doesn't work)
        PyAthena.load_library ('FourMomUtils')

        ## Get the JetSignalState helper
        self.jss = PyAthena.SignalStateHelper(PyAthena.P4SignalState.JETFINAL)
        
        ## import some 4-mom utils
        import FourMomUtils.Bindings  # noqa: F401
        self.utils = { 'deltaR' : PyAthena.P4Helpers.deltaR }


        return StatusCode.Success



    def execute(self):
        self.nProcessed+=1
        self.msg.debug( '==> execute %s on %r. event...', self.name(), self.nProcessed )
        

        ## If passAll is selected, accept all events
        if self.passAll :
            self.msg.debug( '%s event passed because passAll is true', self.name() )
            self.setFilterPassed(True)
            self.nEventPassed += 1
            return StatusCode.Success


        # Load the jet collection from the input file
        jetCollection = None
        try:
            jetCollection = self.storeGateSvc.retrieve( self.jetCollectionType, self.jetCollectionName )
            self.msg.debug( 'Loading the jet collection %s from the input file.', self.jetCollectionName )
        except LookupError:
            self.msg.warning( 'Collection %s not found', self.jetCollectionName )
            self.setFilterPassed(True)
            return StatusCode.Success   
   
        ## Turn all jets into uncalibrated state
        if self.useUncalibratedJets :
            self.jss.setSignalState( PyAthena.P4SignalState.UNCALIBRATED)
            self.msg.debug( 'Setting the jet state to uncalibrated jets.' )


        # Create a class-member list of all jets that passed all cuts.
        # This can then be used by other filters to do overlap removal, etc.
        VIEW_ELEMENTS = 1
        goodJets = PyAthena.JetCollection(VIEW_ELEMENTS)


        # Print a debug message
        self.msg.debug( 'Starting loop over all jets. In this event, there are %s jets', jetCollection.size() )


        ## Find leading Et
        leadEt = 0.0*Units.GeV
        jet_pass=0
        jet_n=0

        controlObject = self.jss.controlObject
        releaseObject = self.jss.releaseObject

        for jet in jetCollection :
            controlObject(jet)            
            self.msg.debug( ' jet = %s', jet)
            self.nJets += 1
            jet_n += 1
            if jet_n <= self.nLeadingsForCheck :
                if abs(jet.eta()) >= self.cutEtaMin and abs(jet.eta()) < self.cutEtaMax :
                    if jet.et() > self.cutEtMin :

                        #ACH
                        pass_trk = False
                        for mk in jet.getMomentKeys():
                            if mk not in ['sumPtTrk']: continue
                            value_mom=jet.getMoment (mk, True)
                            self.msg.debug("mom: %s %s", (mk, value_mom) )
                            if self.cutSumPtTrkMax>0.0 and value_mom < self.cutSumPtTrkMax:
                                pass_trk = True
                        
                        if not pass_trk:
                            self.msg.debug("failed sumPtTrk cut")
                            continue
                        else:
                            self.msg.debug("passed sumPtTrk cut")
                        if self.removeOverlap :
                            self.msg.debug( 'FancyJetFilter: I am in overlap!' )
                            doesOverlap = False
                            for lep in leptonList :   # noqa: F821 (FIXME, leptonList unknown)
                                if self.utils['deltaR'](jet, lep) <= self.deltaR :
                                    doesOverlap = True
                                    break
                                pass
                            if not doesOverlap :
                                if jet.et() > leadEt :
                                    leadEt = jet.et()
                                    pass
                                jet_pass+=1
                                jetCopy = jet
                                goodJets.push_back( jetCopy )
                                pass
                            pass
                        else :
                            self.msg.debug( 'FancyJetFilter: I am NOT in overlap!' )
                            if jet.et() > leadEt :
                                leadEt = jet.et()
                                pass
                            jet_pass+=1
                            jetCopy = jet
                            goodJets.push_back( jetCopy )
                            self.msg.debug("added a good jet")
                            pass
                        pass
                    pass
                pass
            pass

        # Print a debug message
        self.msg.debug( 'Done with the loop over the jets. Have %s jets in collection, put %s jets in goodJets, and the leading jet has et = %s GeV.', jetCollection.size(), goodJets.__len__(), leadEt/Units.GeV )

        releaseObject()


        ## Check if the event is accepted
        if  goodJets.__len__() >= self.minNumberPassed and leadEt <= self.cutEtMax :
            self.msg.debug( 'The %d leading jets are within [%f , %f] GeV', jet_pass, self.cutEtMin/Units.GeV, self.cutEtMax/Units.GeV)
            self.msg.debug( '%s event passed.', self.name() )
            self.setFilterPassed(True)
            self.nEventPassed += 1
            pass
        else:
            self.msg.debug( '%s event failed', self.name() )
            self.setFilterPassed(False)
            pass
        
        return StatusCode.Success



    def finalize(self):
        ## Import needed modules
        import math

        ## Calculate the efficiencies and their errors
        effiJets      = 0.0
        effiEvents    = 0.0
        effiErrJets   = 0.0
        effiErrEvents = 0.0
        try :
            # Efficiencies
            effiJets      = self.nJets / float(self.nProcessed)
            effiEvents    = self.nEventPassed / float(self.nProcessed)

            # Error on efficiencies
            effiErrJets   = math.sqrt(self.nJets) / float(self.nProcessed)
            effiErrEvents = 100.0*math.sqrt(effiEvents*(1.-effiEvents)/float(self.nProcessed))

            effiEvents *= 100.
            
        except ZeroDivisionError :
            self.msg.warning( 'Division by zero error when calculating the uncertainties on the pass efficiencies...' )


        
        self.msg.info( '==> finalize %s...', self.name() )
        self.msg.info( '***************************************************************' )
        self.msg.info( 'Cut-flow table of %s skimming algorithm:', self.name() )
        self.msg.info( '-------------' )
        self.msg.info( ' Number of processed events: %r', self.nProcessed )
        self.msg.info( ' Number of all jets:    %r and number of jets per event: %3.3f +/- %3.3f', self.nJets, effiJets, effiErrJets )
        self.msg.info( ' Events accepted:    %r and resulting efficiency = (%3.3f +/- %3.3f)%%', self.nEventPassed, effiEvents, effiErrEvents )
        self.msg.info( '***************************************************************' )

        return StatusCode.Success

