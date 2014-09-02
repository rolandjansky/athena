##=============================================================================
## Name:        BeamSpotRecoPreInclude_standard
##
## Author:      James Walder
## Created:     March 2010
##
## Description: Here, the preInclude for Reco_trf for BeamSpot re-reconstruction is defined
##              the _stanard implies this will be the default setup
##
## $Id$
##=============================================================================

## for messaging
from AthenaCommon.Logging import logging

## import rec ex config
from RecExConfig.RecFlags import rec
#

# load the beamspot flags
from InDetBeamSpotFinder.BeamSpotJobProperties import BeamSpotFlags

if BeamSpotFlags.turnOnIDOnly(): # For testing
    from AthenaCommon.DetFlags import DetFlags
    DetFlags.all_setOff()
    DetFlags.LAr_setOff()
    DetFlags.ID_setOn()
    #DetFlags.Calo_setOff()
    #DetFlags.Muon_setOff()

    rec.doJetMissingETTag.set_Value_and_Lock(False)
    rec.doEgamma.set_Value_and_Lock(False)
    rec.doTau.set_Value_and_Lock(False)
    rec.doTrigger.set_Value_and_Lock(False)
    rec.doCalo.set_Value_and_Lock(False)
    rec.doMuon.set_Value_and_Lock(False)
    rec.doMuonCombined.set_Value_and_Lock(False)
    rec.doLArg.set_Value_and_Lock(False)
    pass

#Set up the ID flags
from InDetRecExample.InDetJobProperties import InDetFlags

if BeamSpotFlags.doIDReco():
    from ParticleBuilderOptions.AODFlags import AODFlags
    AODFlags.egammaTrackSlimmer=False
    AODFlags.TauTrackSlimmer=False
    AODFlags.MuonTrackSlimmer=False

    # Further code algorithms removals 
    rec.doAODCaloCells.set_Value_and_Lock(False)  
    AODFlags.ParticleJet=False
    
    InDetFlags.doSlimming.set_Value_and_Lock(False) # To prevent non-understood errors 

    InDetFlags.Enabled.set_Value_and_Lock(True)
    InDetFlags.disableInDetReco.set_Value_and_Lock(False)
    InDetFlags.disableTracking.set_Value_and_Lock(False)
    InDetFlags.doPRDFormation.set_Value_and_Lock(True)   

#Important to switch off the Beam constraint
InDetFlags.useBeamConstraint.set_Value_and_Lock(False)

