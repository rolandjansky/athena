##-----------------------------------------------------------------------------
## Name: PrimaryDPD_SCTCommStream.py
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
## Author: Christian Schmitt
## Email:  Christian.Schmitt@cern.ch
##
## Description: This defines the content of the SCT commissioning DPD output stream.
##
## $Id: CommDPD_SCT.py,v 1.1 2009-03-31 12:09:39 kkoeneke Exp $
##-----------------------------------------------------------------------------


## for messaging
from AthenaCommon.Logging import logging
prescaledesdDPDStream_msg = logging.getLogger( 'PrimaryDPD_SCTCommStream' )

## Include the job property flags for this package and from RecExCommon
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from RecExConfig.RecFlags import rec

## This handels multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr 


##====================================================================
## Define the SCTComm DPD output stream
##====================================================================
if primDPD.ApplySkimming():
    from InDetCosmicsFilters.InDetCosmicsFiltersConf import InDet__TrackFilter
    DPD_SCTCOMMSkim = InDet__TrackFilter ( name             = 'DPD_SCTCOMMSkim',
                                           TrackCollections = ["Tracks"],
                                           MinTRTHits       = 0,
                                           MinSCTHits       = 1,
                                           MinPixelHits     = 0,
                                           MinIDHits        = 1,
                                           MinTRTonlyHits   = 2000,
                                           MinSCTonlyHits   = 2000)
    topSequence += DPD_SCTCOMMSkim
    pass

streamName = primDPD.WriteSCTCommStream.StreamName
fileName   = buildFileName( primDPD.WriteSCTCommStream )
if primDPD.isVirtual() == False:
    SCTCommStream=MSMgr.NewPoolStream( streamName, fileName )
    pass
if primDPD.isVirtual() == True:
    SCTCommStream=MSMgr.NewVirtualStream( streamName, fileName )
    pass    

if primDPD.ApplySkimming():
    SCTCommStream.AddAcceptAlgs( "DPD_SCTCOMMSkim" )
    pass


#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput
excludeList=[]
dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,excludeList)

