##Creation: David Cote, September 2010 
##This file defines the RNDM stream, which simply select 1/100 of events

## Include the job property flags for this package and from RecExCommon
from D2PDMaker.D2PDFlags import D2PDFlags

## This handels multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr 

#====================================================================
# Define the event selection
#====================================================================
from PrimaryDPDMaker.PrimaryDPDMakerConf   import PrimaryDPDPrescaler
topSequence += PrimaryDPDPrescaler( "DAOD_RNDM_Filter", Prescale=100 )



#====================================================================
# Define the test DPD output stream
#====================================================================
from D2PDMaker.D2PDHelpers import buildFileName
# This stream HAS TO start with "StreamD2AODM_"! If the input was an (D)ESD(M), this should start with "StreamD2ESD(M)_".
# See this twiki for more information: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/DPDNamingConvention
fileName   = buildFileName( D2PDFlags.WriteDAOD_RNDMStream )
streamName = D2PDFlags.WriteDAOD_RNDMStream.StreamName
RNDMStream = MSMgr.NewPoolStream( streamName, fileName )

# Only events that pass the filters listed below are written out
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
RNDMStream.AcceptAlgs( ["DAOD_RNDM_Filter"] )



#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take all items from the input, except for the ones listed in the excludeList
# If the excludeList is empty, all containers from the input file (e.g. AOD)
# are copied to the output file.
excludeList = []
dpdOutput.addAllItemsFromInputExceptExcludeList( streamName, excludeList )

# You need to add your newly created output containers from above to the output stream
RNDMStream.AddItem( ['CompositeParticleContainer#*'] )
RNDMStream.AddItem( ['INav4MomLinkContainer#*'] )

