##-----------------------------------------------------------------------------
## Name:  DAOD_MU.py
##
## Author:   Susanne Kuehn (Freiburg) <susanne.kuehn@cern.ch>, Eric Feng (Chicago) <Eric.Feng@cern.ch>
## Created:  August 2009
##
## Description:  This defines the content of the DAOD_MU DPD output stream
##               produced by skimming Monte Carlo.
##
## $Id$
##-----------------------------------------------------------------------------

## for messaging
from AthenaCommon.Logging import logging
DAOD_MUStream_msg = logging.getLogger( 'DAOD_MUStream' )

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
    DAOD_MUStream_msg.info('Values of all PrimaryDPDFlags_DAOD_DESD_Streams flags:')
    print primDAOD_DESD_Streams
    pass

##====================================================================
## Define the skimming (event selection) for the DAOD_MU output stream
##====================================================================
if primDPD.ApplySkimming() and primDAOD_DESD_Streams.ApplySkimming() :
    from PrimaryDPDMaker.MuonFilter import MuonFilter
    DAOD_MU_muonFilter = MuonFilter('DAOD_MU_muonFilter')
    DAOD_MU_muonFilter.cutPtMinMu     = 8.0*Units.GeV
    DAOD_MU_muonFilter.muonAuthors    = "combined"  # Possible values: 'lowpt', 'combined', 'calo', 'standalone', 'all'
    DAOD_MU_muonFilter.muonContainers = "staco"  # Possible values: 'staco', 'muid', 'calo', 'all'
    DAOD_MU_muonFilter.cutEtaMax      = 2.7
    DAOD_MU_muonFilter.usingAOD       = True
    topSequence += DAOD_MU_muonFilter

#     from PrimaryDPDMaker.PrimaryDPDMakerConf import MuonFilterAlg
#     DAOD_MU_muonFilter = MuonFilterAlg("DAOD_MU_muonFilter")
#     DAOD_MU_muonFilter.cutPtMinMu     = 8.0*Units.GeV
#     DAOD_MU_muonFilter.muonContainer  = "StacoMuonCollection" # Possible values: 'StacoMuonCollection' 'StacoESDMuonCollection'
#     DAOD_MU_muonFilter.cutEtaMax      = 2.7
#     DAOD_MU_muonFilter.muonAuthors    = "combined"
#     DAOD_MU_muonFilter.TrackType      = "combined"
#     topSequence += DAOD_MU_muonFilter

    pass


##====================================================================
## Define the DAOD_MU output stream
##====================================================================
streamName = primDPD.WriteDAOD_MUStream.StreamName
fileName   = buildFileName( primDPD.WriteDAOD_MUStream )
DAOD_MUStream = MSMgr.NewPoolStream( streamName, fileName )

if primDPD.ApplySkimming() and primDAOD_DESD_Streams.ApplySkimming() :
    DAOD_MUStream.AcceptAlgs( [DAOD_MU_muonFilter.name()] )
    pass


#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take all items from the input, except for the ones listed in the excludeList
#excludeList = dpdOutput.excludeAODOnlyContainersList()
excludeList = []
dpdOutput.addAllItemsFromInputExceptExcludeList(streamName, excludeList)
