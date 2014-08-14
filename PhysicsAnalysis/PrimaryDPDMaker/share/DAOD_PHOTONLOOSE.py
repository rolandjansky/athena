##-----------------------------------------------------------------------------
## Name:  DAOD_PhotonLOOSE.py
##
## Author:   Mohamed Aharrouche
## Created:  March 2010
##
## Description:  derived AOD to select loose photon based on Eric Feng's JO file
##               
##
## $Id$
##-----------------------------------------------------------------------------

## for messaging
from AthenaCommon.Logging import logging
DAOD_PhotonLOOSEStream_msg = logging.getLogger( 'DAOD_PhotonLOOSEStream' )

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

## Include the job property flags for this package and from RecExCommon
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags_DAOD_DESD_Streams import primDAOD_DESD_Streams
from RecExConfig.RecFlags import rec

## This handles multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

##====================================================================
## Write the used options to the log file
##====================================================================
if rec.OutputLevel() <= INFO:
    DAOD_PhotonLOOSEStream_msg.info('Values of all PrimaryDPDFlags_DAOD_DESD_Streams flags:')
    print primDAOD_DESD_Streams
    pass

##====================================================================
## Define the skimming (event selection) for the DAOD_PhotonLOOSE output stream
##====================================================================
if primDPD.ApplySkimming() and primDAOD_DESD_Streams.ApplySkimming() :
    from PrimaryDPDMaker.PhotonFilter import PhotonFilter
    DAOD_PhotonLOOSE_photonFilter = PhotonFilter('DAOD_PhotonLOOSE_photonFilter')
    DAOD_PhotonLOOSE_photonFilter.cutEtMin = 15.*Units.GeV  
    DAOD_PhotonLOOSE_photonFilter.cutIsEM  = "PhotonLoose"
    topSequence += DAOD_PhotonLOOSE_photonFilter

    pass

##====================================================================
## Define the DAOD_PhotonLOOSE output stream
##====================================================================
streamName = primDPD.WriteDAOD_PHOTONLOOSEStream.StreamName
fileName   = buildFileName( primDPD.WriteDAOD_PHOTONLOOSEStream )
DAOD_PhotonLOOSEStream = MSMgr.NewPoolStream( streamName, fileName )

if primDPD.ApplySkimming() and primDAOD_DESD_Streams.ApplySkimming() :
    DAOD_PhotonLOOSEStream.AcceptAlgs( [DAOD_PhotonLOOSE_photonFilter.name()] )
    pass

#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take all items from the input, except for the ones listed in the excludeList
#excludeList = dpdOutput.excludeAODOnlyContainersList()
excludeList = []
dpdOutput.addAllItemsFromInputExceptExcludeList(streamName, excludeList)
