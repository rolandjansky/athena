##-----------------------------------------------------------------------------
## Name: PrimaryDPD_PixelCommStream.py
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
## Author: Christian Schmitt
## Email:  Christian.Schmitt@cern.ch
##
## Description: This defines the content of the Pixel commissioning DPD output stream.
##
## $Id: CommDPD_Pixel.py,v 1.1 2009-03-31 12:09:39 kkoeneke Exp $
##-----------------------------------------------------------------------------


## for messaging
from AthenaCommon.Logging import logging
prescaledesdDPDStream_msg = logging.getLogger( 'PrimaryDPD_PixelCommStream' )

from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from RecExConfig.RecFlags import rec

## This handles multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr 

##====================================================================
## Define the PixelComm DPD output stream
##====================================================================
if primDPD.ApplySkimming() :
    from InDetCosmicsFilters.InDetCosmicsFiltersConf import InDet__TrackFilter
    DPD_PixelCOMMSkim = InDet__TrackFilter ( name             = 'DPD_PixelCOMMSkim',
                                             TrackCollections = ["Tracks"],
                                             MinTRTHits       = 0,
                                             MinSCTHits       = 0,
                                             MinPixelHits     = 1,
                                             MinIDHits        = 1,
                                             MinTRTonlyHits   = 2000,
                                             MinSCTonlyHits   = 2000)
    topSequence += DPD_PixelCOMMSkim
    pass

streamName = primDPD.WritePixelCommStream.StreamName
fileName   = buildFileName( primDPD.WritePixelCommStream )
if primDPD.isVirtual() == False:
    PixelCommStream=MSMgr.NewPoolStream( streamName, fileName )
    pass
if primDPD.isVirtual() == True:
    PixelCommStream=MSMgr.NewVirtualStream( streamName, fileName )
    pass    

if primDPD.ApplySkimming():
    PixelCommStream.AddAcceptAlgs("DPD_PixelCOMMSkim")
    pass


#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput
excludeList=[]
dpdOutput.addAllItemsFromInputExceptExcludeList( streamName,excludeList )


