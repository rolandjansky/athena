##-----------------------------------------------------------------------------
## Name:  DESD_CTLFWJET.py
##
## Author:   Eric Feng (Chicago)  <Eric.Feng@cern.ch>
## Created:  April 2010
##
## Description:  This defines the content of the DESD_CTLFWJET DPD output stream
##               produced by skimming Monte Carlo.
##
## $Id$
##-----------------------------------------------------------------------------

## for messaging
from AthenaCommon.Logging import logging
DESD_CTLFWJETStream_msg = logging.getLogger( 'DESD_CTLFWJETStream' )

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
    DESD_CTLFWJETStream_msg.info('Values of all PrimaryDPDFlags_DAOD_DESD_Streams flags:')
    print primDAOD_DESD_Streams
    pass

##====================================================================
## Define the skimming (event selection) for the DESD_CTLFWJET output stream
##====================================================================
if primDPD.ApplySkimming() and primDAOD_DESD_Streams.ApplySkimming() :
    from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner
    from PrimaryDPDMaker.JetFilter import JetFilter

    # Central jet filter
    DESD_CTLFWJET_jetFilterCentral = JetFilter('DESD_CTLFWJET_jetFilterCentral')
    DESD_CTLFWJET_jetFilterCentral.cutEtMin  = 7.*Units.GeV   # ET cut depends on JX sample
    DESD_CTLFWJET_jetFilterCentral.cutEtMax  = 1E9*Units.GeV
    DESD_CTLFWJET_jetFilterCentral.cutEtaMin = 0.0
    DESD_CTLFWJET_jetFilterCentral.cutEtaMax = 0.3
    DESD_CTLFWJET_jetFilterCentral.jetCollectionName = "AntiKt4TopoEMJets"
    topSequence += DESD_CTLFWJET_jetFilterCentral

    # Forward jet filter
    DESD_CTLFWJET_jetFilterForward = JetFilter('DESD_CTLFWJET_jetFilterForward')
    DESD_CTLFWJET_jetFilterForward.cutEtMin  = 7.*Units.GeV   # ET cut depends on JX sample
    DESD_CTLFWJET_jetFilterForward.cutEtMax  = 1E9*Units.GeV
    DESD_CTLFWJET_jetFilterForward.cutEtaMin = 2.5
    DESD_CTLFWJET_jetFilterForward.cutEtaMax = 5.0
    DESD_CTLFWJET_jetFilterForward.jetCollectionName = "AntiKt4TopoEMJets"
    topSequence += DESD_CTLFWJET_jetFilterForward

    # Require both a central jet and a forward jet
    StreamDESD_CTLFWJET_AcceptEvent = LogicalFilterCombiner("StreamDESD_CTLFWJET_AcceptEvent")
    StreamDESD_CTLFWJET_AcceptEvent.cmdstring = "DESD_CTLFWJET_jetFilterCentral and DESD_CTLFWJET_jetFilterForward"
    topSequence += StreamDESD_CTLFWJET_AcceptEvent
    pass

##====================================================================
## Define the DESD_CTLFWJET output stream
##====================================================================
streamName = primDPD.WriteDESD_CTLFWJETStream.StreamName
fileName   = buildFileName( primDPD.WriteDESD_CTLFWJETStream )
DESD_CTLFWJETStream = MSMgr.NewPoolStream( streamName, fileName )

if primDPD.ApplySkimming() and primDESDStreams.ApplySkimming() :
    #DESD_CTLFWJETStream.AcceptAlgs( [DESD_CTLFWJET_jetFilter.name()] )
    DESD_CTLFWJETStream.AcceptAlgs( [StreamDESD_CTLFWJET_AcceptEvent.name()] )
    pass

#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take all items from the input, except for the ones listed in the excludeList
#excludeList = dpdOutput.excludeAODOnlyContainersList()
excludeList = []
dpdOutput.addAllItemsFromInputExceptExcludeList(streamName, excludeList)
