##-----------------------------------------------------------------------------
## Name:  DAOD_ELLOOSE.py
##
## Author:   Susanne Kuehn (Freiburg) <susanne.kuehn@cern.ch>, Eric Feng (Chicago) <Eric.Feng@cern.ch>
## Created:  August 2009
##
## Description:  This defines the content of the DAOD_ELLOOSE DPD output stream
##               produced by skimming Monte Carlo.
##
## $Id$
##-----------------------------------------------------------------------------

## for messaging
from AthenaCommon.Logging import logging
DAOD_ELLOOSEStream_msg = logging.getLogger( 'DAOD_ELLOOSEStream' )

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
    DAOD_ELLOOSEStream_msg.info('Values of all PrimaryDPDFlags_DAOD_DESD_Streams flags:')
    print primDAOD_DESD_Streams
    pass

##====================================================================
## Define the skimming (event selection) for the DAOD_ELLOOSE output stream
##====================================================================
if primDPD.ApplySkimming() and primDAOD_DESD_Streams.ApplySkimming() :
    from PrimaryDPDMaker.ElectronFilter import ElectronFilter
    DAOD_ELLOOSE_electronFilter = ElectronFilter('DAOD_ELLOOSE_electronFilter')
    DAOD_ELLOOSE_electronFilter.cutEtMin   = 8.0*Units.GeV
    DAOD_ELLOOSE_electronFilter.cutIsEM    = "Loose"    # Possible values: "None", "Loose", '?MediumNoIso','Medium', 'Tight', 'TightNoIsolation'
    DAOD_ELLOOSE_electronFilter.cutAuthor = "egamma"    # Possible values: 'softe', 'egamma', 'either'
    DAOD_ELLOOSE_electronFilter.cutEtaMax    = 2.7
    topSequence += DAOD_ELLOOSE_electronFilter

    pass


##====================================================================
## Define the DAOD_ELLOOSE output stream
##====================================================================
streamName = primDPD.WriteDAOD_ELLOOSEStream.StreamName
fileName   = buildFileName( primDPD.WriteDAOD_ELLOOSEStream )
DAOD_ELLOOSEStream = MSMgr.NewPoolStream( streamName, fileName )

if primDPD.ApplySkimming() and primDAOD_DESD_Streams.ApplySkimming() :
    DAOD_ELLOOSEStream.AcceptAlgs( [DAOD_ELLOOSE_electronFilter.name()] )
    pass


#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take all items from the input, except for the ones listed in the excludeList
#excludeList = dpdOutput.excludeAODOnlyContainersList()
excludeList = []
dpdOutput.addAllItemsFromInputExceptExcludeList(streamName, excludeList)
