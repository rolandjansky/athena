##-----------------------------------------------------------------------------
## Name: D2PD_HighMDiJet.py
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
##
## Description: This defines the content of the DAOD_ELLOOSE18
##
##-----------------------------------------------------------------------------

## Include the job property flags for this package and from RecExCommon
from D2PDMaker.D2PDFlags import D2PDFlags

## This handels multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr 

#---- Load the egammaPID and egammaParameters information
# This is needed to always be up-to-date with the egamma
# IsEM selections and also the author selections
import PyUtils.RootUtils as ru
ROOT = ru.import_root()
import cppyy
cppyy.loadDictionary('egammaEnumsDict')
from ROOT import egammaPID
from ROOT import egammaParameters

#====================================================================
# Define the test DPD output stream
#====================================================================
from D2PDMaker.D2PDHelpers import buildFileName
# This stream HAS TO start with "StreamD2AODM_"! If the input was an (D)ESD(M), this should start with "StreamD2ESD(M)_".
# See this twiki for more information: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/DPDNamingConvention
fileName   = buildFileName( D2PDFlags.WriteDAOD_HighMDiJetStream )
streamName = D2PDFlags.WriteDAOD_HighMDiJetStream.StreamName




#====================================================================
# Define an empty list for bookkeeping
#====================================================================
algsToBookkeep = []


#====================================================================
# Define the event selection
#====================================================================
# Import the needed filters
from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner


#====================================================================
# Check if we have Monte Carlo or real data, based on the inputFileSummary
#====================================================================
inputIsSimulation = False
if inputFileSummary.has_key("evt_type"):
    eventTypeList = inputFileSummary.get("evt_type")
    if eventTypeList.__contains__("IS_SIMULATION") :
        print "Detected that the input file is a simulated dataset"
        inputIsSimulation = True
        pass
    pass




#====================================================================
# Do the PrimaryVertex selection/flagging
#====================================================================
from D2PDMaker.D2PDMakerConf import D2PDVertexSelector
topSequence += D2PDVertexSelector( "VertexSelectorInHighMDiJetStream",
                                   OutputLevel          = INFO,
                                   inputCollection      = 'VxPrimaryCandidate',
                                   minNumberVxTracks    = 3
                                   )
algsToBookkeep.append( "VertexSelectorInHighMDiJetStream" )





#====================================================================
# Do the trigger selection/flagging
#====================================================================
include("PrimaryDPDMaker/SetupTrigDecisionTool.py")
from PrimaryDPDMaker.TriggerFilter import TriggerFilter
for triggerName in D2PDFlags.WriteDAOD_HighMDiJetStream.TriggerList:
    filterName = "TriggerFilterInHighMDiJetStream_" + triggerName
    topSequence += TriggerFilter( filterName,
                                  trigger = triggerName
                                  )
    algsToBookkeep.append( filterName )
    pass








#====================================================================
# Do the offline selection for all wanted jet collections
#====================================================================
finalFilterNames = []

# Loop over all wanted jet collections:
for jetCollName in D2PDFlags.WriteDAODM_HighMDiJetStream.JetCollections:

    from D2PDMaker.D2PDMakerConf import D2PDJetSelector
    # Create an instance of this filter and add it to the AlgSequence
    # Also, configure the filter to your needs. This are just some examples
    filterName = jetCollName + "SelectorInHighMDiJetStream"
    topSequence += D2PDJetSelector( filterName,
                                    OutputLevel                    = INFO,
                                    inputCollection                = jetCollName,
                                    outputLinkCollection           = jetCollName + 'LinkCollectionInHighMDiJetStream',
                                    associationToolList            = jetAssociationToolList,
                                    outputAssociationContainerList = jetOutputAssociationContainerList,
                                    minNumberPassed                = 2,
                                    absEtaMax                      = 2.8,
                                    ptMin                          = 30.0*Units.GeV
                                    )
    algsToBookkeep.append( filterName )

    filterName = "HighPt" + jetCollName + "SelectorInHighMDiJetStream"
    topSequence += D2PDJetSelector( filterName,
                                    OutputLevel          = INFO,
                                    inputCollection      = jetCollName,
                                    outputLinkCollection = 'HighP' + jetCollName + 'LinkCollectionInHighMDiJetStream',
                                    minNumberPassed      = 1,
                                    absEtaMax            = 2.8,
                                    ptMin                = 280.0*Units.GeV
                                    )
    algsToBookkeep.append( filterName )

    from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner
    # - All combinations with pre-selected electrons with 15 GeV
    filterName = "HighMassDi" + jetCollName + "CombinerInHighMDiJetStream"
    topSequence += D2PDParticleCombiner( filterName,
                                         OutputLevel             = INFO,
                                         compositeParticlePDG_ID = 23, # This is a Z Boson
                                         inputCollectionList     = [ 'HighP' + jetCollName + 'LinkCollectionInHighMDiJetStream',
                                                                     jetCollName + 'LinkCollectionInHighMDiJetStream' ],
                                         outputCollection        = 'HighMassDi' + jetCollName + 'CollectionInHighMDiJetStream',
                                         minNumberPassed         = 1,
                                         massMin                 = 700.0*Units.GeV
                                         )
    algsToBookkeep.append( filterName )
    finalFilterNames.append( filterName )
    pass





#====================================================================
# Define the DAOD output stream
#====================================================================
HighMDiJetStream = MSMgr.NewPoolStream( streamName, fileName )

# Only events that pass the filters listed below are written out
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
HighMDiJetStream.AcceptAlgs( finalFilterNames )

HighMDiJetStream.AddOtherAlgsToBookkeep( algsToBookkeep )

#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take all items from the input, except for the ones listed in the excludeList
# If the excludeList is empty, all containers from the input file (e.g. AOD)
# are copied to the output file.
excludeList = [ "TrigMuonEFContainer#HLT_MuonEF" ]
dpdOutput.addAllItemsFromInputExceptExcludeList( streamName, excludeList )

# You need to add your newly created output containers from above to the output stream
HighMDiJetStream.AddItem( ['CompositeParticleContainer#*'] )
HighMDiJetStream.AddItem( ['INav4MomLinkContainer#*'] )


