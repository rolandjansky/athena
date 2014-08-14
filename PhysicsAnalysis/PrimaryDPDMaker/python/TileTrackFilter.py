# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        TileTrackFilter
##
## Author:      Jose Maneira (Lisboa)
## Created:     November 2008
##
## Based on:    JetFilter.py by Eric Feng (Chicago)
##
## Description: This is a short algorithm to select events that contain at 
##              least one TileMuonFitter track.
##
##
##=============================================================================

__doc__ = """This is a short algorithm to select events that contain at 
least one TileMuonFitter track.
"""
__version__ = "0.0.1"
__author__  = "Jose Maneira <maneira@lip.pt>"


import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

from PrimaryDPDMaker import PrimaryDPDHelpers

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

class TileTrackFilter( PyAthena.AthFilterAlgorithm ):
    """
    This class is a short algorithm to select events that contain at 
    least one TileMuonFitter track.  All energies below
    are uncalibrated (EM scale).  The possible criteria are (with
    default values):
    doEnergyCut          = True
    cutFullEnergyMin     = 1.0*Units.GeV
    doNTracksCut         = True
    cutNtracksMin        = 1
    passAll              = False
    """



    def __init__ ( self, name = "TileTrackFilter", **kw ):
        ## initialize base class
        kw['name'] = name
        super(TileTrackFilter, self).__init__(**kw)
        
        ## Define the cuts
        self.tileContainerType = kw.get('tileContainerType', "TileContainer<TileCosmicMuon>")
        self.tileContainerName = kw.get('tileContainerName', "TileCosmicMuonHT")
        self.doEnergyCut       = kw.get('doEnergyCut', True)
        self.cutFullEnergyMin  = kw.get('cutFullEnergyMin', 1.0)
        self.doNTracksCut      = kw.get('doNTracksCut', True)
        self.cutNTracksMin     = kw.get('cutNTracksMin', 1)
        self.passAll           = kw.get('passAll', False)
        ## Also, declare some counters and initialize them to zero
        self.nProcessed       = 0
        self.nTracks          = 0
        self.nEventPassTracks = 0
        self.nEventPassEnergy = 0
        ## Get the storgate handle
        self.storeGateSvc     = None

        
    def initialize(self):
        self.msg.info( '************************************' )
        self.msg.info( '==> initialize %s...', self.name() )
        self.msg.info( 'Will apply the following cuts:' )
        self.msg.info( '  doEnergyCut      = %r', self.doEnergyCut )
        self.msg.info( '  cutFullEnergyMin = %r', self.cutFullEnergyMin )
        self.msg.info( '  doNTracksCut     = %r', self.doNTracksCut )
        self.msg.info( '  cutNTracksMin    = %r', self.cutNTracksMin )
        self.msg.info( '  passAll          = %r', self.passAll )
        self.msg.info( '************************************' )


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

        return StatusCode.Success



    def execute(self):
        self.nProcessed+=1
        self.msg.debug( '==> execute %s on %r. event...' % (self.name(), self.nProcessed) )
        
        nPassEnergy    = 0

        ## If passAll is selected, accept all events
        if self.passAll :
            self.msg.debug( '%s event passed because passAll is true' % self.name() )
            self.setFilterPassed(True)
            return StatusCode.Success

        #Then... here we go!
        hasTrack  = False
        try:
            tcmContainer = self.storeGateSvc.retrieve( self.tileContainerType, self.tileContainerName )
            pass
        except (NameError,LookupError,KeyError), err:
            # This exception is a hack!! It is needed in software releases PRIOR to release 15!
            # Remove the NameError when you never want to run in a release prior to rel 15 any more!!
            try :
                tcmContainer = self.storeGateSvc.retrieve( 'TileCosmicMuonContainer', self.tileContainerName )
                pass
            except (LookupError,KeyError), err:
                self.msg.warning( 'Collection %s not found' % self.tileContainerName )
                self.setFilterPassed(False)
                return StatusCode.Success
            pass
        
        #tcmContainer = self.storeGateSvc[self.tileContainerName]       
		
        for tileTrack in tcmContainer :
            self.nTracks += 1
            if not hasTrack :
                if tileTrack.GetFitQuality() > 0 :
                    hasTrack  = True
                    if tileTrack.GetFullEnergy() > self.cutFullEnergyMin :
                        nPassEnergy += 1
                        pass
                    pass
                pass
            pass

        ## Translate the tiletrack pass counters into event pass counters
        if nPassEnergy > 0 : self.nEventPassEnergy += 1
        if nPassEnergy >= self.cutNTracksMin : self.nEventPassTracks += 1


        ## Check if the event is accepted
        if nPassEnergy > 0 :
            if nPassEnergy >= self.cutNTracksMin :
                self.msg.debug( '%s event passed.' % self.name() )
                self.setFilterPassed(True)
                pass
            else :
                self.msg.debug( '%s event failed NTracks cut' % self.name() )
                self.setFilterPassed(False)
                pass
            pass
        else:
            self.msg.debug( '%s event failed, no track above cut' % self.name() )
            self.setFilterPassed(False)
            pass
        
        return StatusCode.Success



    def finalize(self):
        ## Import needed modules
        import math

        ## Calculate the efficiencies and their errors
        effiTracks         = 0.0
        effiPassEnergy     = 0.0
        effiPassTracks     = 0.0
        effiErrTracks      = 0.0
        effiErrPassEnergy  = 0.0
        effiErrPassTracks  = 0.0
        try :
            # Efficiencies
            effiTracks     = self.nTracks / float(self.nProcessed)
            effiPassEnergy = self.nEventPassEnergy / float(self.nProcessed)
            effiPassTracks = self.nEventPassTracks / float(self.nProcessed)

            # Error on efficiencies

            effiErrTracks    = math.sqrt(self.nTracks) / float(self.nProcessed)
            effiErrPassEnergy = 100.0*math.sqrt(effiPassEnergy*(1.-effiPassEnergy)/float(self.nProcessed))
            effiErrPassTracks = 100.0*math.sqrt(effiPassTracks*(1.-effiPassTracks)/float(self.nProcessed))

            effiPassEnergy *= 100.
            effiPassTracks *= 100.
            
        except ZeroDivisionError :
            self.msg.warning( 'Division by zero error when calculating the uncertainties on the pass efficiencies...' )


        
        self.msg.info( '==> finalize %s...' % self.name() )
        self.msg.info( '***************************************************************' )
        self.msg.info( 'Cut-flow table of %s skimming algorithm:' % self.name() )
        self.msg.info( '-------------' )
        self.msg.info( ' Number of processed events: %r' % self.nProcessed )
        self.msg.info( ' Number of all tracks:    %r and number of tracks per event: %3.3f +/- %3.3f' % ( self.nTracks, effiTracks, effiErrTracks ) )
        self.msg.info( ' Events after energy cut:     %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPassEnergy,    effiPassEnergy,    effiErrPassEnergy ) )
        self.msg.info( ' Events after NTracks cut:     %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPassTracks,     effiPassTracks,     effiErrPassTracks ) )
        self.msg.info( '***************************************************************' )

        return StatusCode.Success

