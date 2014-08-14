##-----------------------------------------------------------------------------
## Name:  DAOD_ONIA_MUMU.py
##
## Author:   James Catmore <james.catmore@cern.ch>
## Created:  January 2010
##
## Description:  This defines the content of the DAOD_ONIA_MUMU DAOD stream
##
## $Id$
##-----------------------------------------------------------------------------

## for messaging
from AthenaCommon.Logging import logging
DAOD_ONIA_MUMUStream_msg = logging.getLogger( 'DAOD_ONIA_MUMUStream' )

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
    DAOD_ONIA_MUMUStream_msg.info('Values of all PrimaryDPDFlags_DAOD_DESD_Streams flags:')
    print primDAOD_DESD_Streams
    pass

##====================================================================
## Define the skimming (event selection) for the DAOD_ONIA_MUMU output stream
##====================================================================

if primDPD.ApplySkimming() and primDAOD_DESD_Streams.ApplySkimming() :
    from BPhysDAOD.BPhysDAODConf import BPhysAODtoDAOD 
    DAOD_ONIA_MUMU_filter_FullInvMassRange = BPhysAODtoDAOD('DAOD_ONIA_MUMU_filter_FullInvMassRange')
    DAOD_ONIA_MUMU_filter_FullInvMassRange.muonPtThreshold        = 4.0*Units.GeV
    DAOD_ONIA_MUMU_filter_FullInvMassRange.lowRangeLowLimit       = 2.4*Units.GeV  # Lower invariant mass limit for lower band
    DAOD_ONIA_MUMU_filter_FullInvMassRange.lowRangeHighLimit      = 4.0*Units.GeV  # Upper invariant mass limit for lower band 
    DAOD_ONIA_MUMU_filter_FullInvMassRange.highRangeLowLimit      = 8.0*Units.GeV  # Lower invariant mass limit for upper band
    DAOD_ONIA_MUMU_filter_FullInvMassRange.highRangeHighLimit     = 12.0*Units.GeV # Upper invariant mass limit for upper band
    DAOD_ONIA_MUMU_filter_FullInvMassRange.acceptAllDiMuons       = False # Allow all dimuon events irrespective of invariant mass 
    DAOD_ONIA_MUMU_filter_FullInvMassRange.acceptFullInvMassRange = True # Accept dimuon events in the full invariant mass range 
    DAOD_ONIA_MUMU_filter_FullInvMassRange.acceptHighInvMassRange = False # Accept dimuon events in the upper (upsilon) region 
    DAOD_ONIA_MUMU_filter_FullInvMassRange.acceptLowInvMassRange  = False # Accept dimuon events in the lower (J/psi) region
    DAOD_ONIA_MUMU_filter_FullInvMassRange.muonContainers         = ["StacoMuonCollection"]
    topSequence += DAOD_ONIA_MUMU_filter_FullInvMassRange

    DAOD_ONIA_MUMU_filter_HighInvMassRange = BPhysAODtoDAOD('DAOD_ONIA_MUMU_filter_HighInvMassRange')
    DAOD_ONIA_MUMU_filter_HighInvMassRange.muonPtThreshold        = 4.0*Units.GeV
    DAOD_ONIA_MUMU_filter_HighInvMassRange.lowRangeLowLimit       = 2.4*Units.GeV  # Lower invariant mass limit for lower band
    DAOD_ONIA_MUMU_filter_HighInvMassRange.lowRangeHighLimit      = 4.0*Units.GeV  # Upper invariant mass limit for lower band 
    DAOD_ONIA_MUMU_filter_HighInvMassRange.highRangeLowLimit      = 8.0*Units.GeV  # Lower invariant mass limit for upper band
    DAOD_ONIA_MUMU_filter_HighInvMassRange.highRangeHighLimit     = 12.0*Units.GeV # Upper invariant mass limit for upper band
    DAOD_ONIA_MUMU_filter_HighInvMassRange.acceptAllDiMuons       = False # Allow all dimuon events irrespective of invariant mass 
    DAOD_ONIA_MUMU_filter_HighInvMassRange.acceptFullInvMassRange = False # Accept dimuon events in the full invariant mass range 
    DAOD_ONIA_MUMU_filter_HighInvMassRange.acceptHighInvMassRange = True # Accept dimuon events in the upper (upsilon) region 
    DAOD_ONIA_MUMU_filter_HighInvMassRange.acceptLowInvMassRange  = False # Accept dimuon events in the lower (J/psi) region
    DAOD_ONIA_MUMU_filter_HighInvMassRange.muonContainers         = ["StacoMuonCollection"]
    topSequence += DAOD_ONIA_MUMU_filter_HighInvMassRange

    DAOD_ONIA_MUMU_filter_LowInvMassRange = BPhysAODtoDAOD('DAOD_ONIA_MUMU_filter_LowInvMassRange')
    DAOD_ONIA_MUMU_filter_LowInvMassRange.muonPtThreshold        = 4.0*Units.GeV
    DAOD_ONIA_MUMU_filter_LowInvMassRange.lowRangeLowLimit       = 2.4*Units.GeV  # Lower invariant mass limit for lower band
    DAOD_ONIA_MUMU_filter_LowInvMassRange.lowRangeHighLimit      = 4.0*Units.GeV  # Upper invariant mass limit for lower band 
    DAOD_ONIA_MUMU_filter_LowInvMassRange.highRangeLowLimit      = 8.0*Units.GeV  # Lower invariant mass limit for upper band
    DAOD_ONIA_MUMU_filter_LowInvMassRange.highRangeHighLimit     = 12.0*Units.GeV # Upper invariant mass limit for upper band
    DAOD_ONIA_MUMU_filter_LowInvMassRange.acceptAllDiMuons       = False # Allow all dimuon events irrespective of invariant mass 
    DAOD_ONIA_MUMU_filter_LowInvMassRange.acceptFullInvMassRange = False # Accept dimuon events in the full invariant mass range 
    DAOD_ONIA_MUMU_filter_LowInvMassRange.acceptHighInvMassRange = False # Accept dimuon events in the upper (upsilon) region 
    DAOD_ONIA_MUMU_filter_LowInvMassRange.acceptLowInvMassRange  = True # Accept dimuon events in the lower (J/psi) region
    DAOD_ONIA_MUMU_filter_LowInvMassRange.muonContainers         = ["StacoMuonCollection"]
    topSequence += DAOD_ONIA_MUMU_filter_LowInvMassRange

    pass

##====================================================================
## Define the DAOD_ONIA_MUMU output stream
##====================================================================
streamName = primDPD.WriteDAOD_ONIAMUMUStream.StreamName
fileName   = buildFileName( primDPD.WriteDAOD_ONIAMUMUStream )
if primDPD.isVirtual() == False:
    DAOD_ONIA_MUMUStream = MSMgr.NewPoolStream( streamName, fileName )
    pass
if primDPD.isVirtual() == True:
    DAOD_ONIA_MUMUStream = MSMgr.NewVirtualStream( streamName, fileName )
    pass    

if primDPD.ApplySkimming() and primDAOD_DESD_Streams.ApplySkimming() :
    DAOD_ONIA_MUMUStream.AcceptAlgs( [ DAOD_ONIA_MUMU_filter_FullInvMassRange.name(),
                                       DAOD_ONIA_MUMU_filter_HighInvMassRange.name(),
                                       DAOD_ONIA_MUMU_filter_LowInvMassRange.name() ] )
    pass


#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take all items from the input, except for the ones listed in the excludeList
#excludeList = dpdOutput.excludeAODOnlyContainersList()
excludeList = []
dpdOutput.addAllItemsFromInputExceptExcludeList(streamName, excludeList)
