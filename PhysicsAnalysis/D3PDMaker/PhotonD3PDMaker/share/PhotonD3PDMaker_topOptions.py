# $Id $
#
# @file PhotonD3PDMaker/share/PhotonD3PDMaker_topOptions.py
# @author Mike Hance <mike.hance@cern.ch>
# @date Aug, 2010
# @brief Example for building a photon D3PD from an AOD, based on other D3PDMaker tools
#

###################################################################
# Define the input file here.
#

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput= ["AOD.pool.root"]

athenaCommonFlags.EvtMax =100 

###################################################################
# Define the output file here.
#

if not globals().get('tupleFileOutput'):
    tupleFileOutput = 'test.NTUP_PHOTON.root'

from D3PDMakerConfig.D3PDProdFlags import prodFlags
prodFlags.WritePhotonD3PD = True
prodFlags.WritePhotonD3PD.FileName = tupleFileOutput
prodFlags.WritePhotonD3PD.lock()


###################################################################
# Some options to increase the output
#

# These are some examples of how to set some options.
# All of these specific choices are already set in PhotonD3PD.py, so
# don't bother to re-set them here.
#
# Note: options set in this file will only get picked up if running
# athena directly on this file - standard Reco_trf jobs will NOT
# see any options set here!  To have your options propagated into
# official production jobs, set the options in PhotonD3PD.py
# or in PhotonD3PD_prodJobOFragment.py.
#
from PhotonD3PDMaker.PhotonD3PDMakerFlags import PhotonD3PDMakerFlags
PhotonD3PDMakerFlags.AdditionalMETOutputLevel .set_Value_and_Lock(0)
PhotonD3PDMakerFlags.DumpAllTracks            .set_Value_and_Lock(False)
PhotonD3PDMakerFlags.FilterTracksForMET       .set_Value_and_Lock(False)
PhotonD3PDMakerFlags.FillFullMET              .set_Value_and_Lock(False)
PhotonD3PDMakerFlags.JetCollections           =['AntiKt4TopoEMJets','AntiKt4LCTopoJets']
PhotonD3PDMakerFlags.JetCollections           .lock()
# PhotonD3PDMakerFlags.DumpNearbyTracks     .set_Value_and_Lock(True )
# PhotonD3PDMakerFlags.DumpAllTracks        .set_Value_and_Lock(True ) # if you want all the tracks!
# PhotonD3PDMakerFlags.DumpLotsOfTriggerInfo.set_Value_and_Lock(False)
    
###################################################################
# Make the D3PD.
#

from RecExConfig.RecFlags import rec
rec.DPDMakerScripts.append("PhotonD3PDMaker/PhotonD3PD_prodJobOFragment.py")
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(False)
rec.doESD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doForwardDet.set_Value_and_Lock(False)
include ("RecExCommon/RecExCommon_topOptions.py")


