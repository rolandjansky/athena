# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        MissingEtFilter.py
##
## Author:      Karsten Koeneke (DESY)
## Created:     July 2009
##
## Based on:    LargeMetFilter.py by UllaBlumenschein
##
## Description: This is a short algorithm to select events that contain 
##              a certain amount of missing transverse energy. A minimum
##              deltaPhi cut between the missing Et and the leading jet
##              can be applied.
##
##=============================================================================

__doc__ = """This is a short algorithm to select events that contain
a certain amount of missing transverse energy. A minimum deltaPhi cut
between the missing Et and the leading jet can be applied.
"""
__version__ = "0.0.1"
__author__  = "Ulla Blumenschein <Ulla.Blumenschein@cern.ch>"
__author__  = "Karsten Koeneke <karsten.koeneke@cern.ch>"


import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

class MissingEtFilter( PyAthena.AthFilterAlgorithm ):
    """
    This is a short algorithm to select events that contain
    a certain amount of missing transverse energy. A minimum deltaPhi cut
    between the missing Et and the leading jet can be applied.
    cutMetMin            = 0.0*Units.GeV
    jetContainerTypeList = []
    jetContainerNameList = []
    useUncalibratedJets  = True
    useLeadingJet        = False
    minDeltaPhi          = 0.0
    requireMet           = True  # What to do in case the MET container can't be retrieved
    passAll              = False
    """



    def __init__ ( self, name = "MissingEtFilter", **kw ):
        ## initialize base class
        kw['name'] = name
        super(MissingEtFilter, self).__init__(**kw)
        
        ## Define the cuts
        self.metCollectionType     = kw.get('metCollectionType', "MissingET")
        self.metCollectionName     = kw.get('metCollectionName', "MET_RefFinal")
        self.jetCollectionTypeList = kw.get('jetCollectionTypeList', [])
        self.jetCollectionNameList = kw.get('jetCollectionNameList', [])
        self.useUncalibratedJets   = kw.get('useUncalibratedJets', True)
        self.cutMetMin             = kw.get('cutMetMin', 0.0*Units.GeV)
        self.useLeadingJet         = kw.get('useLeadingJet', False)
        self.minDeltaPhi           = kw.get('minDeltaPhi', 0.0)
        self.requireMet            = kw.get('requireMet', True) # What to do in case the MET container can't be retrieved
        self.passAll               = kw.get('passAll', False)
        
        ## Also, declare some counters and initialize them to zero
        self.nProcessed         = 0
        self.nEventPassMet      = 0
        self.nEventPassDeltaPhi = 0

        ## Get the storgate handle
        self.storeGateSvc = None

        ## Handle to JetSignalState
        self.jss = None

        return


    def initialize(self):
        self.msg.info( '************************************' )
        self.msg.info( '==> initialize %s...', self.name() )
        self.msg.info( '  jetCollectionTypeList = %r', self.jetCollectionTypeList )
        self.msg.info( '  jetCollectionNameList = %r', self.jetCollectionNameList )
        self.msg.info( 'Will apply the following cuts:' )
        self.msg.info( '  MetMin                = %r', self.cutMetMin )
        self.msg.info( '  minDeltaPhi           = %r', self.minDeltaPhi )
        self.msg.info( '  useLeadingJet         = %r', self.useLeadingJet )
        self.msg.info( '  requireMet            = %r', self.requireMet )
        self.msg.info( '************************************' )


        ## Get the StoreGate service
        self.storeGateSvc = PyAthena.py_svc('StoreGateSvc')
        if self.storeGateSvc is None:
            self.msg.error("Problem retrieving StoreGateSvc pointer !!")
            return StatusCode.Failure


        ## Import needed modules
        import PyUtils.RootUtils as ru
        ROOT = ru.import_root()  # noqa: F841

        ## Eagerly load library due to reflex bug (autoloading enums/namespaces doesn't work)
        #PyAthena.load_library ('FourMomUtils')

        ## Get the JetSignalState helper
        #self.jss = PyAthena.JetSignalState()
        self.jss = PyAthena.SignalStateHelper(PyAthena.P4SignalState.JETFINAL)

        ## import some 4-mom utils
        import FourMomUtils.Bindings  # noqa: F401
        self.utils = { 'deltaR' :   PyAthena.P4Helpers.deltaR ,
                       'deltaEta' : PyAthena.P4Helpers.deltaEta ,
                       'deltaPhi' : PyAthena.P4Helpers.deltaPhi }


        return StatusCode.Success




    def execute(self):
        # Increment the event counter
        self.nProcessed+=1

        # Print a message to the log file
        self.msg.debug( '==> execute %s on %r. event...', self.name(), self.nProcessed )
        

        ## If passAll is selected, accept all events
        if self.passAll :
            self.msg.debug( '%s event passed because passAll is true', self.name() )
            self.setFilterPassed(True)
            return StatusCode.Success


        # Try to access container. Avoid crash if Containers absent in first data
        met = None
        try:
            met = self.storeGateSvc.retrieve( self.metCollectionType, self.metCollectionName )
            pass
        except LookupError:
            self.msg.warning( 'Collection %s not found', self.metCollectionName )
            if self.requireMet:
                self.setFilterPassed(False)
            else:
                self.setFilterPassed(True)
                pass
            return StatusCode.Success   

        
        # Load the jet collections from the input file
        jetList = []
        leadJet   = None
        if self.useLeadingJet :
            self.msg.debug( 'Going to load the jets of type %s and key %s.', self.jetCollectionTypeList, self.jetCollectionNameList)
            if self.jetCollectionTypeList.__len__() == self.jetCollectionNameList.__len__() :
                for i in range( self.jetCollectionTypeList.__len__() ) :
                    try:
                        lepColl = self.storeGateSvc.retrieve( self.jetCollectionTypeList[i],
                                                              self.jetCollectionNameList[i] )
                        self.msg.debug( 'Loading the jet collection %s from the input file.',
                                        self.jetCollectionNameList[i] )
                        for lep in lepColl :
                            jetList.append(lep)
                            pass
                        pass
                    except LookupError:
                        if self.nProcessed <100:
                            self.msg.warning( 'Collection %s not found', self.jetCollectionNameList[i] )
                            pass
                        pass
                    pass
                pass
            else :
                self.msg.error( 'List of jets type %s and key %s has different lenght!',
                                self.jetCollectionTypeList, self.jetCollectionNameList )
                pass
                

            # Turn all jets into uncalibrated state
            if self.useUncalibratedJets :
                self.jss.setSignalState( PyAthena.P4SignalState.UNCALIBRATED)
                ## self.msg.debug( 'Setting the jet state to uncalibrated jets.' )
                ## for jet in jetList:
                ##     self.jss.setState(jet, PyAthena.P4SignalState.UNCALIBRATED)
                ##     pass
                ## pass
            
            
            # Find the leading jet
            leadJetEt = 0.0
            controlObject = self.jss.controlObject
            releaseObject = self.jss.releaseObject
            for jet in jetList :
                controlObject(jet)
                if jet.et() > leadJetEt :
                    leadJetEt = jet.et()
                    leadJet   = jet
                    pass
                releaseObject()
                pass


        
            ## # Turn the jets back into calibrated state
            ## if self.useUncalibratedJets :
            ##     self.msg.debug( 'Setting the jet state back to calibrated jets.' )
            ##     for jet in jetList :
            ##         self.jss.setState(jet, PyAthena.P4SignalState.CALIBRATED)
            ##         pass
            ##     pass
            ## pass # End: if self.useLeadingJet 


        # Initialize some variables
        PassMet      = False
        PassDeltaPhi = True


        # Now, do the selection
        if met.et() >  self.cutMetMin :
            PassMet = True
            # Check that a jet object was actually provided
            if self.useLeadingJet and leadJet :
                if self.utils['deltaPhi']( leadJet, met.phi() ) < self.minDeltaPhi :
                    PassDeltaPhi = False
                    pass
                pass
            pass
        
        

        ## Translate the jet pass counters into event pass counters
        if PassMet :      self.nEventPassMet      += 1
        if PassDeltaPhi : self.nEventPassDeltaPhi += 1


        ## Check if the event is accepted
        if PassMet and PassDeltaPhi :
            self.msg.debug( '%s event passed ', self.name() )
            self.setFilterPassed(True)
        else:
            self.msg.debug( '%s event failed ', self.name() )
            self.setFilterPassed(False)
            pass
        
        return StatusCode.Success



    def finalize(self):
        ## Import needed modules
        import math

        ## Calculate the efficiencies and their errors
        effPassMet     = 0.0
        effDeltaPhi    = 0.0
        efferrPassMet  = 0.0
        efferrDeltaPhi = 0.0        
        try :
            # Efficiencies
            effPassMet  = self.nEventPassMet / float(self.nProcessed)
            effDeltaPhi = self.nEventPassDeltaPhi / float(self.nProcessed)

            # Error on efficiencies, use binomial errors
            efferrPassMet  = 100.0*math.sqrt(effPassMet*(1.-effPassMet)/float(self.nProcessed))
            efferrDeltaPhi = 100.0*math.sqrt(effDeltaPhi*(1.-effDeltaPhi)/float(self.nProcessed))

            effPassMet  *= 100.0
            effDeltaPhi *= 100.0
            
        except ZeroDivisionError :
            self.msg.warning( 'Division by zero error when calculating the uncertainties on the pass efficiencies...' )
            pass

        
        self.msg.info( '==> finalize %s...', self.name() )
        self.msg.info( '***************************************************************' )
        self.msg.info( 'Cut-flow table of %s skimming algorithm:', self.name() )
        self.msg.info( '-------------' )
        self.msg.info( ' Number of processed events: %r', self.nProcessed )
        self.msg.info( ' Events after Met cut:       %r, efficiency = (%3.3f +/- %3.3f)%%',
                       self.nEventPassMet, effPassMet, efferrPassMet )
        self.msg.info( ' Events after deltaPhi cut:  %r, efficiency = (%3.3f +/- %3.3f)%%',
                       self.nEventPassDeltaPhi, effDeltaPhi, efferrDeltaPhi )
        self.msg.info( '***************************************************************' )

        return StatusCode.Success

