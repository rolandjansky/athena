##-----------------------------------------------------------------------------
## Name: PrimaryDPD_IDCommStream.py
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
## Author: Christian Schmitt
## Email:  Christian.Schmitt@cern.ch
##
## Description: This defines the content of the ID commissioning DPD output stream.
##
## $Id: CommDPD_ID.py,v 1.1 2009-03-31 12:09:39 kkoeneke Exp $
##-----------------------------------------------------------------------------


## for messaging
from AthenaCommon.Logging import logging
prescaledesdDPDStream_msg = logging.getLogger( 'PrimaryDPD_IDCommStream' )

## Include the job property flags for this package and from RecExCommon
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from RecExConfig.RecFlags import rec

## This handels multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr 


##====================================================================
## Define the IDComm DPD output stream
##====================================================================
if primDPD.ApplySkimming():
    from InDetCosmicsFilters.InDetCosmicsFiltersConf import InDet__TrackFilter
    DPD_IDCOMMSkim = InDet__TrackFilter ( name             = 'DPD_IDCOMMSkim',
                                          TrackCollections = ["Tracks"],
                                          MinTRTHits       = 0,
                                          MinSCTHits       = 0,
                                          MinPixelHits     = 0,
                                          MinIDHits        = 1,
                                          MinTRTonlyHits   = 2000,
                                          MinSCTonlyHits   = 2000)
    topSequence += DPD_IDCOMMSkim
    pass

streamName = primDPD.WriteIDCommStream.StreamName
fileName   = buildFileName( primDPD.WriteIDCommStream )
if primDPD.isVirtual() == False:
    IDCommStream=MSMgr.NewPoolStream( streamName, fileName )
    pass
if primDPD.isVirtual() == True:
    IDCommStream=MSMgr.NewVirtualStream( streamName, fileName )
    pass    

if primDPD.ApplySkimming():
    IDCommStream.AddAcceptAlgs( "DPD_IDCOMMSkim" )
    pass


#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput
excludeList=[]
dpdOutput.addAllItemsFromInputExceptExcludeList( streamName,excludeList )

   
