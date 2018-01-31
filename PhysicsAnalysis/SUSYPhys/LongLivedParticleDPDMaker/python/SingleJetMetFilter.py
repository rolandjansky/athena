# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        SingleJet+LargeMet Skimmer
##
## Author:      Shimpei Yamamoto
## Email:       shimpei.yamamoto@cern.ch
## Created:     Febrary 2011
##
## Description: A short filter algorithm to select events that contain one ISR
##              jet and large Met.
##=============================================================================

__doc__ = """This filter provides a a short algorithm to select events that contain one ISR jet and large Met."""
__version__ = "0.0.1"
__author__  = "Shimpei Yamamoto <shimpei.yamamoto@cern.ch>"


import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode
from PrimaryDPDMaker import PrimaryDPDHelpers

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

class SingleJetMetFilter( PyAthena.AthFilterAlgorithm ):

    def __init__ ( self, name = "SingleJetMetFilter", **kw ):
        kw['name'] = name
        super(SingleJetMetFilter, self).__init__(**kw)

        ## Define the cuts
        self.jetCollectionType       = kw.get('jetCollectionType', "JetCollection")
        self.jetCollectionName       = kw.get('jetCollectionName', "AntiKt4TopoJets")
        self.cutEt1Min               = kw.get('cutEt1Min', 0.0*Units.GeV)
        self.cutEta1Max              = kw.get('cutEta1Max', 4.5)
        self.cutEt2Min               = kw.get('cutEt2Min', 0.0*Units.GeV)
        self.cutEta2Max              = kw.get('cutEta2Max', 4.5)
        self.doSecondJetVeto         = kw.get('doSecondJetVeto', False)
        self.requireMet              = kw.get('requireMet   ', True)
        self.metCollectionType       = kw.get('metCollectionType', "MissingET")
        self.metCollectionNames      = kw.get('metCollectionNames', [])
        self.cutMetMin               = kw.get('cutMetMin', 0.0*Units.GeV)
        self.passAll                 = kw.get('passAll', False)

        return

    def initialize(self):
        self.msg.info( '************************************' )
        self.msg.info( '==> initialize %s...', self.name() )
        self.msg.info( '  jetCollectionType       = %s', self.jetCollectionType )
        self.msg.info( '  jetCollectionName       = %s', self.jetCollectionName )
        self.msg.info( '  metCollectionType       = %s', self.metCollectionType )
        self.msg.info( '  metCollectionNames      = %s', self.metCollectionNames )
        self.msg.info( 'Will apply the following cuts:' )
        self.msg.info( '  Et1Min                  = %r', self.cutEt1Min )
        self.msg.info( '  Eta1Max                 = %r', self.cutEta1Max )
        self.msg.info( '  Et2Min                  = %r', self.cutEt2Min )
        self.msg.info( '  Eta2Max                 = %r', self.cutEta2Max )
        self.msg.info( '  doSecondJetVeto         = %r', self.doSecondJetVeto )
        self.msg.info( '  requireMet              = %r', self.requireMet )
        self.msg.info( '  MetMin                  = %r', self.cutMetMin )
        self.msg.info( '************************************' )

        self.nProcessed         = 0
        self.nEventPassed       = 0
        self.nEventPassJet      = 0
        self.nEventPassMet      = 0

        ## Get the StoreGate service
        self.storeGateSvc = PyAthena.py_svc('StoreGateSvc')
        if not self.storeGateSvc :
            self.msg.error("Problem retrieving StoreGateSvc pointer !!")
            return StatusCode.Failure
        
        return StatusCode.Success


    def execute(self):
        self.nProcessed+=1        

        ## Accept all events if passAll is set to be true
        if self.passAll :
            self.msg.debug( '%s event passed because passAll is true' % self.name() )
            self.setFilterPassed(True)
            return StatusCode.Success

        # Get the jet collection from StoreGate
        jetCollection = None

        try:
            jetCollection = self.storeGateSvc.retrieve( self.jetCollectionType, self.jetCollectionName )
        except LookupError:
            self.msg.warning( 'Collection %s not found' % self.jetCollectionName )           
            self.setFilterPassed(False)
            return StatusCode.Success   


        selectedJetIdx = []
        for i in range( jetCollection.size() ) :
            jet = jetCollection[i]
            if jet.et() > self.cutEt1Min and abs(jet.eta()) < self.cutEta1Max :
                selectedJetIdx.append(i)

        passedJet = True
        if len(selectedJetIdx) >= 1 :
            if self.doSecondJetVeto :
                for i in range( jetCollection.size() ) :
                  if i == selectedJetIdx[0] :
                      pass
                  else :
                      jet = jetCollection[i]
                      if jet.et() > self.cutEt2Min and abs(jet.eta()) < self.cutEta2Max :
                          passedJet = False
                          break
            else :
                passedJet = True
        else :
          passedJet = False

        if passedJet :
            self.nEventPassJet+=1


        if self.requireMet :
            passedMet = False
            for i in range( len(self.metCollectionNames) ) :
                try:
                    met = self.storeGateSvc.retrieve( self.metCollectionType, self.metCollectionNames[i] )
                except LookupError:
                    self.msg.warning( 'Collection %s not found' % self.metCollectionNames[i] )           
                    self.setFilterPassed(False)
                    return StatusCode.Success   

                if met.et() > self.cutMetMin :
                    passedMet = True
                    self.nEventPassMet+=1
                    break
                else :
                    pass
            
            if passedJet and passedMet :
                self.nEventPassed+=1
                self.setFilterPassed(True)
            else :
                self.setFilterPassed(False)

        else :
            self.nEventPassMet+=1
            if passedJet :
                self.nEventPassed+=1
                self.setFilterPassed(True)
            else :
                self.setFilterPassed(False)
 
        return StatusCode.Success

    def finalize(self):
        ## Import needed modules
        import math

        ## Calculate the efficiencies and their errors
        effPassJet       = 0.0
        effPassMet       = 0.0
        effPassAll       = 0.0
        effErrPassJet    = 0.0
        effErrPassMet    = 0.0        
        effErrPassAll    = 0.0        

        try :
            # Efficiencies
            effPassJet      = self.nEventPassJet / float(self.nProcessed)
            effPassMet      = self.nEventPassMet / float(self.nProcessed)
            effPassAll      = self.nEventPassed / float(self.nProcessed)

            # Error on efficiencies
            effErrPassJet = 100.0*math.sqrt(effPassJet*(1.-effPassMet)/float(self.nProcessed))
            effErrPassMet = 100.0*math.sqrt(effPassMet*(1.-effPassMet)/float(self.nProcessed))
            effErrPassAll = 100.0*math.sqrt(effPassAll*(1.-effPassAll)/float(self.nProcessed))

            effPassJet *= 100.
            effPassMet *= 100.
            effPassAll *= 100.
            
        except ZeroDivisionError :
            self.msg.warning( 'Division by zero error when calculating the uncertainties on the pass efficiencies...' )

        self.msg.info( '==> finalize %s...' % self.name() )
        self.msg.info( '***************************************************************' )
        self.msg.info( 'Cut-flow table of %s skimming algorithm:' % self.name() )
        self.msg.info( '-------------' )
        self.msg.info( ' Number of processed events:              %r' % self.nProcessed )
        self.msg.info( ' Number of events with jet requirement:   %r, eff. = (%3.3f +/- %3.3f)%%' % ( self.nEventPassJet, effPassJet, effErrPassJet ) )
        self.msg.info( ' Number of events with met requirement:   %r, eff. = (%3.3f +/- %3.3f)%%' % ( self.nEventPassMet, effPassMet, effErrPassMet ) )
        self.msg.info( ' Number of events with all requirements:  %r, eff. = (%3.3f +/- %3.3f)%%' % ( self.nEventPassed, effPassAll, effErrPassAll ) )
        self.msg.info( '***************************************************************' )


        return StatusCode.Success

