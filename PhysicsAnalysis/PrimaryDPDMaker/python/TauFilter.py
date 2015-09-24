# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        Tau Skimmer
##
## Author:      Ulla Blumenschein (Goettingen)
## Created:     August 2008
##
## Based on:    PhotonFilter.py by Karsten Koeneke (DESY)
##
## Description: This is a short algorithm to select events that contain a jet
##              that fulfills a certain set of criteria.
##=============================================================================

__doc__ = """This is a short algorithm to select events that contain one 
jet which fulfills a certain set of criteria.
"""
__version__ = "0.0.1"
__author__  = "Ulla Blumenschein <Ulla.Blumenschein@cern.ch>"


import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

from PrimaryDPDMaker import PrimaryDPDHelpers

## Import the module that allows to use named units, e.g. GeV 
import AthenaCommon.SystemOfUnits as Units

class TauFilter( PyAthena.AthFilterAlgorithm ):
    """
    This class is a short algorithm to select events that contain a tau which exceed a certain value.
    The preselection cuts on the current isTau :
    cutTauEtMin      = 30.0*Units.GeV
    cutIdTau         = 0 
    doSelection      = True
    minNumberPassed  = 1
    """



    def __init__ ( self, name = "TauFilter", **kw ):
        ## initialize base class
        kw['name'] = name
        super(TauFilter, self).__init__(**kw)
        
        ## Define the cuts
        self.tauCollectionType = kw.get('tauCollectionType', "Analysis::TauJetContainer")
        self.tauCollectionName = kw.get('tauCollectionName', "TauRecContainer")
        self.goodTauCollectionName = kw.get('goodTauCollectionName', self.name() )
        self.cutTauEtMin       = kw.get('cutTauEtMin', 30.0*Units.GeV)
        self.cutTauId1         = kw.get('cutTauId1', 0)
        self.cutTauId2         = kw.get('cutTauId2', 0)        
        self.cutNTrk           = kw.get('cutNTrk', -1)
        self.cutEMrad1         = kw.get('cutEMrad1', -1 )
        self.cutEMrad3         = kw.get('cutEMrad3', -1 )
        self.cutRtrack1        = kw.get('cutRtrack1', -1  )
        self.cutRtrack3        = kw.get('cutRtrack3', -1  )
        self.cutFtrack1        = kw.get('cutFtrack1', -1  )
        self.cutFtrack3        = kw.get('cutFtrack3', -1  )       
        self.doSelection       = kw.get('doSelection', False)
        self.minNumberPassed   = kw.get('minNumberPassed', 1)
        self.leadTauEtMin      = kw.get('leadTauEtMin', -1)        
        ## Also, declare some counters and initialize them to zero
        self.nProcessed         = 0
        self.nEventPassEt       = 0
        self.nEventPassSelec    = 0
        self.nEventPassOptID    = 0
        self.nEventMinNumPassed = 0
        self.nEventFilterPassed = 0        
        ## Get the storgate handle
        self.storeGateSvc       = None
        
        return


    
    def initialize(self):
        self.msg.info( '************************************' )
        self.msg.info( '==> initialize %s...', self.name() )
        self.msg.info( 'Will apply the following cuts:' )
        self.msg.info( '  TauEtMin            = %r', self.cutTauEtMin )
        self.msg.info( '  TauId1              = %r', self.cutTauId1 )
        self.msg.info( '  TauId2              = %r', self.cutTauId2 )        
        self.msg.info( '  doSelection         = %r', self.doSelection )
        self.msg.info( '  Nb trk max          = %r', self.cutNTrk )
        self.msg.info( '  Cut EMRadius1       = %r', self.cutEMrad1 )
        self.msg.info( '  Cut EMRadius3       = %r', self.cutEMrad3 )        
        self.msg.info( '  Cut Rtrack1         = %r', self.cutRtrack1 )
        self.msg.info( '  Cut Rtrack3         = %r', self.cutRtrack3 )
        self.msg.info( '  Cut Ftrack1         = %r', self.cutFtrack1 )
        self.msg.info( '  Cut Ftrack3         = %r', self.cutFtrack3 )        
        self.msg.info( '  minNumberPassed     = %r', self.minNumberPassed )
        self.msg.info( '  DiTau: LeadTauEtmin = %r', self.leadTauEtMin )        
        self.msg.info( '************************************' )

 
        
        ## Import needed modules
        import PyUtils.RootUtils as ru
        ROOT = ru.import_root()

        #lib = PyAthena.load_library('tauEventDict')

        ## Get the StoreGate service
        self.storeGateSvc = PyAthena.py_svc('StoreGateSvc')
        if self.storeGateSvc is None:
            self.msg.error("Problem retrieving StoreGateSvc pointer !!")
            return StatusCode.Failure
        
        return StatusCode.Success




    def execute(self):
        self.nProcessed+=1
        self.msg.debug( '==> execute %s on %r. event...' % (self.name(), self.nProcessed) )
        
        PassEt      = False
        PassOptID   = False
        PassSelec   = False
        passLeadTau = False
        evtAccepted = False



        # Create a class-member list of all taus that passed all cuts.
        # This can then be used by other filters to do overlap removal, etc.
        VIEW_ELEMENTS = 1
        goodTaus = PyAthena.Analysis.TauJetContainer(VIEW_ELEMENTS)
        
        # Try to access container. Avoid crash if Containers absent in first data 
        try:
            taujet = self.storeGateSvc.retrieve( self.tauCollectionType, self.tauCollectionName )
        except LookupError:
            self.msg.warning( 'Collection %s not found' % self.tauCollectionName)           
            self.setFilterPassed(False)            
            return StatusCode.Success            


        # Now, loop over all taus and do the selection
        for i in range(taujet.size()) :
            if (self.cutTauEtMin<0 or taujet[i].et()>self.cutTauEtMin):
                PassEt = True

                hasDetail = False
                if (taujet[i].nDetails()>=0 and taujet[i].detailName(0).find("TauRecDetailsContainer")>-1):
                    detail = taujet[i].detail(0)
                    emrad = detail.seedCalo_EMRadius()
                    etOverPtLeadTrk = detail.etOverPtLeadTrk()
                    if(etOverPtLeadTrk==0):
                        ftrack = 0.
                    else:
                        ftrack = 1./etOverPtLeadTrk
                        pass
                    rtrack = detail.seedCalo_trkAvgDist()
                    pass
                else:
                    self.msg.warning( 'tau details not found')
                    emrad = -1
                    centfr = -1
                    pass
                
                numtr = taujet[i].numTrack()
                
                if(self.cutNTrk<0 or numtr<=self.cutNTrk):
                    if(((self.cutEMrad1 <0 and self.cutEMrad3 <0) or(numtr==1 and emrad<=self.cutEMrad1)or(numtr>1 and emrad<=self.cutEMrad3))and((self.cutRtrack1 <0 and self.cutRtrack3 <0 )or (numtr==1 and rtrack<= self.cutRtrack1)or(numtr>1 and rtrack<= self.cutRtrack3))and((self.cutFtrack1 <0 and self.cutFtrack3 <0 )or (numtr==1 and ftrack>= self.cutFtrack1)or(numtr>1 and ftrack>= self.cutFtrack3)) ):
                        PassOptID=True
                        if (self.doSelection==False):
                            PassSelec = True
                            goodTaus.push_back( taujet[i] )
                            if (self.leadTauEtMin<0 or taujet[i].et()>self.leadTauEtMin):
                                passLeadTau = True
                            pass
                        else:
                            tauid = taujet[i].tauID()
                            istau = (tauid.isTau(self.cutTauId1) or tauid.isTau(self.cutTauId2))
                            if istau:
                                PassSelec = True
                                goodTaus.push_back( taujet[i] )
                                if (self.leadTauEtMin<0 or taujet[i].et()>self.leadTauEtMin):
                                    passLeadTau = True
                                    pass
                                pass
                            pass
                        pass               
                    pass
                pass
                
        ## Translate the jet pass counters into event pass counters
        if PassEt:       self.nEventPassEt += 1
        if PassOptID:    self.nEventPassOptID += 1
        if PassSelec:   self.nEventPassSelec += 1

        ## Record the good taus into StoreGate so that they can be retrieved by other algorithms
        if self.storeGateSvc.record( goodTaus, self.goodTauCollectionName ) != StatusCode.Success :
            self.msg.error( 'Could not record the goodTaus into StoreGate with the key = ' % self.goodTauCollectionName )
            pass
        
        
        ## Check if the event is accepted
        ## Has to move to general Prescaler
        if (goodTaus.__len__() >= self.minNumberPassed):
            self.nEventMinNumPassed += 1
            if (passLeadTau):
                self.msg.debug( '%s event passed ' % self.name() )
                self.nEventFilterPassed += 1
                self.setFilterPassed(True)
            else:
                self.msg.debug( '%s event failed ' % self.name() )
                self.setFilterPassed(False)
                pass
            pass
        else:
            self.msg.debug( '%s event failed ' % self.name() )
            self.setFilterPassed(False)           
        
        return StatusCode.Success



    def finalize(self):
        ## Import needed modules
        import math

        ## Calculate the efficiencies and their errors
        effPassEt       = 0.0
        effPassOptID    = 0.0
        effSelec       = 0.0
        effMinNumPassed = 0.0
        effFilterPassed = 0.0        
        efferrPassEt       = 0.0
        efferrPassOptID    = 0.0
        efferrSelec       = 0.0
        efferrMinNumPassed = 0.0
        efferrFilterPassed = 0.0
     
        try :
            # Efficiencies
            effPassEt       = self.nEventPassEt / float(self.nProcessed)
            effPassOptID    = self.nEventPassOptID / float(self.nProcessed)
            effSelec       = self.nEventPassSelec / float(self.nProcessed)
            effMinNumPassed = self.nEventMinNumPassed / float(self.nProcessed)
            effFilterPassed = self.nEventFilterPassed / float(self.nProcessed)

            # Error on efficiencies
            efferrPassEt       = 100.0*math.sqrt(effPassEt*(1.-effPassEt)/float(self.nProcessed))
            efferrPassOptID    = 100.0*math.sqrt(effPassOptID*(1.-effPassOptID)/float(self.nProcessed))
            efferrSelec       = 100.0*math.sqrt(effSelec*(1.-effSelec)/float(self.nProcessed))
            efferrMinNumPassed = 100.0*math.sqrt(effMinNumPassed*(1.-effMinNumPassed)/float(self.nProcessed))
            efferrFilterPassed = 100.0*math.sqrt(effFilterPassed*(1.-effFilterPassed)/float(self.nProcessed))

            effPassEt *= 100.
            effPassOptID *= 100.
            effSelec *= 100.
            effMinNumPassed *= 100.
            effFilterPassed *= 100.
            
            
        except ZeroDivisionError :
            self.msg.warning( 'Division by zero error when calculating the uncertainties on the pass efficiencies...' )
            pass
        
        self.msg.info( '==> finalize %s...' % self.name() )
        self.msg.info( '***************************************************************' )
        self.msg.info( 'Cut-flow table of %s skimming algorithm:' % self.name() )
        self.msg.info( '-------------' )
        self.msg.info( ' Number of processed events:     %r' % self.nProcessed )
        self.msg.info( ' Events after E_T cut:           %r, efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPassEt, effPassEt, efferrPassEt ) )
        self.msg.info( ' Events after Optimal ID cut:    %r, efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPassOptID,effPassOptID, efferrPassOptID ) )
        self.msg.info( ' Events after Standard ID:       %r, efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPassSelec, effSelec, efferrSelec ) )
        self.msg.info( ' Events after minNumberPassed:   %r, efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventMinNumPassed, effMinNumPassed, efferrMinNumPassed ) )
        self.msg.info( ' Events which passed the filter: %r, efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventFilterPassed, effFilterPassed, efferrFilterPassed ) )
        self.msg.info( '***************************************************************' )

        return StatusCode.Success

