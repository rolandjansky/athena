##===================================
## Name:      DAOD_HSG1
##
## Author:    James Saxon
## Created:   February 2013
##===================================

__doc__     = "Perform the HSG1 preselection."
__author__  = "James Saxon"

#if this file is already inlcuded, don't include it again
include.block("DAOD_HSG1.py")


from AthenaCommon.AlgSequence import AlgSequence
import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

## for messaging
from AthenaCommon.Logging import logging
DAOD_EGammaStream_msg = logging.getLogger( 'DAOD_EGammaStream' )

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

## Include the job property flags for this package and from RecExCommon
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
    
## This handles multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr


##====================================================================
## Define the skimming (event selection) for the ZEEPHO DAOD output stream
##====================================================================
from PrimaryDPDMaker.TriggerFilter import TriggerFilter
from D2PDMaker.D2PDMakerConf import D2PDVertexSelector

topSequence += TriggerFilter("DAODM_HSG1_EF_g35_loose_g25_loose", trigger = "EF_g35_loose_g25_loose")

topSequence += D2PDVertexSelector("DAODM_HSG1_VertexFilter",
                                  inputCollection = 'VxPrimaryCandidate',
                                  minNumberVxTracks = 2)

from HSG1DPDUtils.HSG1Filter import HSG1PreselectionFilter
topSequence += HSG1PreselectionFilter("DAODM_HSG1_PreselectionFilter")

# book-keeping for these three pieces....
hsg1FilterBookkeeping = ["DAODM_HSG1_EF_g35_loose_g25_loose",
                         "DAODM_HSG1_VertexFilter",
                         "DAODM_HSG1_PreselectionFilter"]


##### DO I NEED TO DO THIS???                       
# streamName = primDPD.WriteDAOD_ZEEGAMMAStream.StreamName 
# fileName   = buildFileName( primDPD.WriteDAOD_ZEEGAMMAStream )
# if primDPD.isVirtual() == False:
#     DAOD_EGammaStream = MSMgr.NewPoolStream( streamName, fileName )
#     pass
# if primDPD.isVirtual() == True:
#     DAOD_EGammaStream = MSMgr.NewVirtualStream( streamName, fileName )
#     pass


#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from D2PDMaker.D2PDFlags import D2PDFlags
from D2PDMaker.D2PDHelpers import buildFileName
fileName   = buildFileName( D2PDFlags.WriteDAODM_HSG1Stream )
streamName = D2PDFlags.WriteDAODM_HSG1Stream.StreamName

DAOD_EGammaStream = MSMgr.NewPoolStream( streamName, fileName )

# Only events that pass the filters listed below are written out
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
# SkimmedOutputStream.AcceptAlgs( [""] )
DAOD_EGammaStream.RequireAlgs( [ "DAODM_HSG1_EF_g35_loose_g25_loose",
                                 "DAODM_HSG1_VertexFilter",
                                 "DAODM_HSG1_PreselectionFilter"] )

DAOD_EGammaStream.AddOtherAlgsToBookkeep( hsg1FilterBookkeeping )

excludeList = []
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput
dpdOutput.addAllItemsFromInputExceptExcludeList( streamName, excludeList )


