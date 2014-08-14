##-----------------------------------------------------------------------------
## Name:  DAOD_EGLOOSE.py
##
## Author:   Eric Feng (Chicago)  <Eric.Feng@cern.ch>
## Created:  August 2009
##
## Description:  This defines the content of the DAOD_EGLOOSE DPD output stream
##               produced by skimming Monte Carlo.
##
## $Id$
##-----------------------------------------------------------------------------

## for messaging
from AthenaCommon.Logging import logging
DAOD_EGLOOSEStream_msg = logging.getLogger( 'DAOD_EGLOOSEStream' )

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
    DAOD_EGLOOSEStream_msg.info('Values of all PrimaryDPDFlags_DAOD_DESD_Streams flags:')
    print primDAOD_DESD_Streams
    pass

##====================================================================
## Define the skimming (event selection) for the DAOD_EGLOOSE output stream
##====================================================================
if primDPD.ApplySkimming() and primDAOD_DESD_Streams.ApplySkimming() :
    from PrimaryDPDMaker.PhotonFilter import PhotonFilter
    DAOD_EGLOOSE_photonFilter = PhotonFilter('DAOD_EGLOOSE_photonFilter')
    #E_T cut on samples listed with their generator level cut and appropriate offline cut
    #DAOD_EGLOOSE_photonFilter.cutEtMin  = 5.*Units.GeV   #MinBias
    #DAOD_EGLOOSE_photonFilter.cutEtMin = 20.*Units.GeV   #JF17  (CKIN3 =  15)
    #DAOD_EGLOOSE_photonFilter.cutEtMin = 40.*Units.GeV   #JF35  (CKIN3 =  33)
    #DAOD_EGLOOSE_photonFilter.cutEtMin = 75.*Units.GeV   #JF70  (CKIN3 =  65)
    #DAOD_EGLOOSE_photonFilter.cutEtMin = 145.*Units.GeV  #JF140 (CKIN3 = 135)
    DAOD_EGLOOSE_photonFilter.cutIsEM = "PhotonLoose"
    topSequence += DAOD_EGLOOSE_photonFilter

    from PrimaryDPDMaker.ElectronFilter import ElectronFilter
    DAOD_EGLOOSE_electronFilter = ElectronFilter('DAOD_EGLOOSE_electronFilter')
    #DAOD_EGLOOSE_electronFilter.cutEtMin  = 5.*Units.GeV   #MinBias
    #DAOD_EGLOOSE_electronFilter.cutEtMin = 20.*Units.GeV   #JF17  (CKIN3 =  15)
    #DAOD_EGLOOSE_electronFilter.cutEtMin = 40.*Units.GeV   #JF35  (CKIN3 =  33)
    #DAOD_EGLOOSE_electronFilter.cutEtMin = 75.*Units.GeV   #JF70  (CKIN3 =  65)
    #DAOD_EGLOOSE_electronFilter.cutEtMin = 145.*Units.GeV  #JF140 (CKIN3 = 135)
    DAOD_EGLOOSE_electronFilter.cutIsEM = "ElectronLoose"
    DAOD_EGLOOSE_electronFilter.cutAuthor = "egamma"
    topSequence += DAOD_EGLOOSE_electronFilter

    pass

##====================================================================
## Define the DAOD_EGLOOSE output stream
##====================================================================
streamName = primDPD.WriteDAOD_EGLOOSEStream.StreamName
fileName   = buildFileName( primDPD.WriteDAOD_EGLOOSEStream )
DAOD_EGLOOSEStream = MSMgr.NewPoolStream( streamName, fileName )

if primDPD.ApplySkimming() and primDAOD_DESD_Streams.ApplySkimming() :
    DAOD_EGLOOSEStream.AcceptAlgs( [DAOD_EGLOOSE_photonFilter.name(), DAOD_EGLOOSE_electronFilter.name()] )
    pass

#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take all items from the input, except for the ones listed in the excludeList
#excludeList = dpdOutput.excludeAODOnlyContainersList()
excludeList = []
dpdOutput.addAllItemsFromInputExceptExcludeList(streamName, excludeList)
