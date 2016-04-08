##=============================================================================
## Name:        DESDM_BEAMSPOTFragment
##
## Author:      James Walder
## Created:     March 2010
##
## Description: Here, the output stream for the BeamSpot is defined.
##
##
## $Id $
##=============================================================================

## for messaging
from AthenaCommon.Logging import logging

## Include the job property flags for this package and from RecExCommon
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from InDetBeamSpotExample import BeamSpotDPDFlags 
from RecExConfig.RecFlags import rec

## This handles multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

#Create a new stream
streamName = primDPD.WriteDESDM_BEAMSPOTStream.StreamName
fileName   = buildFileName( primDPD.WriteDESDM_BEAMSPOTStream )

##====================================================================
## Define this output stream
##====================================================================
if primDPD.isVirtual() == False:
    DESDM_BeamSpotStream = MSMgr.NewPoolStream( streamName, fileName )
    pass
if primDPD.isVirtual() == True:
    DESDM_BeamSpotStream = MSMgr.NewVirtualStream( streamName, fileName )
    pass    




##-----------------------------------------------------------------------------
## Write all IOV meta data containers
## IOV stands for "Interval Of Validity". This means that in one file, there
## can be several blocks of events corresponding to one MetaData block.
##-----------------------------------------------------------------------------
DESDM_BeamSpotStream.AddMetaDataItem( ["IOVMetaDataContainer#*"] )
DESDM_BeamSpotStream.AddMetaDataItem( ["LumiBlockCollection#*"] )
 
##-----------------------------------------------------------------------------
## Write event-based containers 
##-----------------------------------------------------------------------------
DESDM_BeamSpotStream.AddItem( ['EventInfo#*'] )

##-----------------------------------------------------------------------------
## Write Track and Vertex information
##-----------------------------------------------------------------------------

DESDM_BeamSpotStream.AddItem( ["Rec::TrackParticleContainer#TrackParticleCandidate"] )
DESDM_BeamSpotStream.AddItem( ["VxContainer#VxPrimaryCandidate*"] )

