##-----------------------------------------------------------------------------
## Name:  DESD_HIRARE.py
##
## Author:   Karsten Koeneke <karsten.koeneke_at_cern.ch>
## Created:  October 2010
##
## Description:  DESD for heavy-ion events which are rare.
##
## $Id$
##-----------------------------------------------------------------------------

## for messaging
from AthenaCommon.Logging import logging

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

## Include the job property flags for this package and from RecExCommon
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from RecExConfig.RecFlags import rec

## This handles multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner



##====================================================================
## Define the event selection (skimming)
##====================================================================

# -------------------
# Trigger selection
# -------------------

# Have to set this flag to True to set up the trigger info!!!
include("PrimaryDPDMaker/SetupTrigDecisionTool.py")
hiRareCMDString = ""
# Now, define the trigger fitlers
from PrimaryDPDMaker.TriggerFilter import TriggerFilter
for trigName in primDPD.WriteDESD_HIRAREStream.TriggerNames :
    TriggerFilterName = "DESD_HIRAREStream_TriggerFilter_"+trigName
    topSequence += TriggerFilter( TriggerFilterName,
                                  trigger = trigName )
    hiRareCMDString += TriggerFilterName
    if hiRareCMDString != "" and not hiRareCMDString.endswith(" or "):
        hiRareCMDString += " or "
        pass
    pass

# Remove the trailing "or" from the string
if hiRareCMDString.endswith(" or "):
    hiRareCMDString = hiRareCMDString[:-len(" or ")]
    pass

# Make the combination of the trigger filters
topSequence += LogicalFilterCombiner( "DESD_HIRAREStream_CombinedTriggerFilter",
                                      OutputLevel = INFO,
                                      cmdstring = hiRareCMDString )


# -------------------
# Offline selection
# -------------------

# The list of filter algorithm instances for event selection
filterList = []

# Photon selection
from D2PDMaker.D2PDMakerConf import D2PDPhotonSelector
topSequence += D2PDPhotonSelector( "DESD_HIRAREStream_PhotonFilter",
                                   OutputLevel          = INFO,
                                   inputCollection      = primDPD.WriteDESD_HIRAREStream.photonCollection,
                                   minNumberPassed      = 1,
                                   clusterEtMin         = primDPD.WriteDESD_HIRAREStream.photonMinEt
                                   )
filterList.append( "DESD_HIRAREStream_PhotonFilter" )

# Electron selection
from D2PDMaker.D2PDMakerConf import D2PDElectronSelector
topSequence += D2PDElectronSelector( "DESD_HIRAREStream_ElectronFilter",
                                     OutputLevel         = INFO,
                                     inputCollection     = primDPD.WriteDESD_HIRAREStream.electronCollection,
                                     minNumberPassed     = 1,
                                     clusterEtMin        = primDPD.WriteDESD_HIRAREStream.electronMinEt
                                     )
filterList.append( "DESD_HIRAREStream_ElectronFilter" )



##====================================================================
## Define this output stream
##====================================================================
streamName = primDPD.WriteDESD_HIRAREStream.StreamName
fileName   = buildFileName( primDPD.WriteDESD_HIRAREStream )
if primDPD.isVirtual() == False:
    DESD_HiRareStream = MSMgr.NewPoolStream( streamName, fileName )
    pass
if primDPD.isVirtual() == True:
    DESD_HiRareStream = MSMgr.NewVirtualStream( streamName, fileName )
    pass    

if primDPD.ApplySkimming() :
    DESD_HiRareStream.AcceptAlgs( filterList )
    DESD_HiRareStream.RequireAlgs( ["DESD_HIRAREStream_CombinedTriggerFilter"] )
    pass


#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput
# Take all items from the input, except for the ones listed in the excludeList
excludeList = []

if primDPD.WriteDESD_HIRAREStream.trimContainers :
    excludeList.append( "TrackCollection#CombinedInDetTracks" )
    excludeList.append( "TrackCollection#ResolvedPixelTracks" )
    pass

dpdOutput.addAllItemsFromInputExceptExcludeList( streamName, excludeList )
