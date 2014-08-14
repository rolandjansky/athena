##-----------------------------------------------------------------------------
## Name:  DESD_FWJET.py
##
## Author:   Eric Feng (Chicago)  <Eric.Feng@cern.ch>
## Created:  April 2010
##
## Description:  This defines the content of the DESD_FWJET DPD output stream
##               produced by skimming Monte Carlo.
##
## $Id$
##-----------------------------------------------------------------------------

## for messaging
from AthenaCommon.Logging import logging
DESD_FWJETStream_msg = logging.getLogger( 'DESD_FWJETStream' )

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
    DESD_FWJETStream_msg.info('Values of all PrimaryDPDFlags_DAOD_DESD_Streams flags:')
    print primDAOD_DESD_Streams
    pass

##====================================================================
## Define the skimming (event selection) for the DESD_FWJET output stream
##====================================================================
if primDPD.ApplySkimming() and primDAOD_DESD_Streams.ApplySkimming() :
    from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner
    from PrimaryDPDMaker.JetFilter import JetFilter

    # Forward jet filter
    DESD_FWJET_jetFilterForward = JetFilter('DESD_FWJET_jetFilterForward')
    DESD_FWJET_jetFilterForward.cutEtMax  = 1E9*Units.GeV
    DESD_FWJET_jetFilterForward.cutEtaMin = 3.0
    DESD_FWJET_jetFilterForward.cutEtaMax = 5.0
    DESD_FWJET_jetFilterForward.nLeadingsForCheck = 2
    DESD_FWJET_jetFilterForward.minNumberPassed = 1
    DESD_FWJET_jetFilterForward.jetCollectionName = "AntiKt4TopoEMJets"
    topSequence += DESD_FWJET_jetFilterForward
    
    pass

##====================================================================
## Define the DESD_FWJET output stream
##====================================================================
streamName = primDPD.WriteDESD_FWJETStream.StreamName
fileName   = buildFileName( primDPD.WriteDESD_FWJETStream )
DESD_FWJETStream = MSMgr.NewPoolStream( streamName, fileName )

if primDPD.ApplySkimming() and primDAOD_DESD_Streams.ApplySkimming() :
    DESD_FWJETStream.AcceptAlgs( [DESD_FWJET_jetFilterForward.name()] )
    pass

#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take all items from the input, except for the ones listed in the excludeList
#excludeList = dpdOutput.excludeAODOnlyContainersList()
excludeList = []
dpdOutput.addAllItemsFromInputExceptExcludeList(streamName, excludeList)
