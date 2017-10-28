##-----------------------------------------------------------------------------
## Name: D2PD_ElLoose18.py
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
fileName   = buildFileName( D2PDFlags.WriteDAOD_ElLoose18Stream )
streamName = D2PDFlags.WriteDAOD_ElLoose18Stream.StreamName




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
# Do the good run list (GRL) selection --- only for data
#====================================================================
cmdString_GRL = ""
if not inputIsSimulation :
    # use closest DB replica
    from PoolSvc.PoolSvcConf import PoolSvc
    svcMgr += PoolSvc(SortReplicas=True)
    from DBReplicaSvc.DBReplicaSvcConf import DBReplicaSvc
    svcMgr += DBReplicaSvc(UseCOOLSQLite=False)

    ## # Try to copy the GRL locally
    ## import socket
    ## socket.setdefaulttimeout(180)
    ## remoteGRL = D2PDFlags.GRLFile()
    ## localGRL  = "data10_7TeV.pro05.merged_LBSUMM_WZjets_allchannels.xml"
    ## import urllib
    ## urllib.urlretrieve( remoteGRL, localGRL )

    # Create an instance of the GoodRunsListSelectorTool
    from GoodRunsLists.GoodRunsListsConf import *
    ToolSvc += GoodRunsListSelectorTool( "GoodRunsListSelectorTool",
                                         OutputLevel     = INFO,
                                         #GoodRunsListVec = [ 'data10_7TeV.pro05.merged_LBSUMM_WZjets_allchannels.xml' ], ## specify your grl here
                                         GoodRunsListVec = [ '../xml/data10_7TeV.periodAllYear_DetStatus-v03-pro05_WZjets_allchannels.xml' ],
                                         PassThrough     = False
                                         )

    # Use the GRLTriggerSelectorAlg
    from GoodRunsListsUser.GoodRunsListsUserConf import *
    topSequence += GRLTriggerSelectorAlg( "GRLFilterInElLoose18Stream",
                                          OutputLevel       = INFO,
                                          GoodRunsListArray = [ 'WZjets_allchannels' ] ## GRL name from the above xml-file
                                          )

    #theApp.getHandle().Dlls += ['XMLCoreParser'] ## Hack to avoid library mismatch after importing the GRL stuff

    algsToBookkeep.append( "GRLFilterInElLoose18Stream" )
    #--------------------------------------------------------------------
    # Set up the cumulative cut-flow for the standard pre-selection.
    # This will use the Logical Filter Combiners to add individual cuts.
    #--------------------------------------------------------------------
    cmdString_GRL += "GRLFilterInElLoose18Stream and "
    pass # End: if not inputIsSimulation






#====================================================================
# Do the PrimaryVertex selection
#====================================================================
from D2PDMaker.D2PDMakerConf import D2PDVertexSelector
topSequence += D2PDVertexSelector( "VertexSelectorInElLoose18Stream",
                                   OutputLevel          = INFO,
                                   inputCollection      = 'VxPrimaryCandidate',
                                   minNumberVxTracks    = 3
                                   )
algsToBookkeep.append( "VertexSelectorInElLoose18Stream" )

#--------------------------------------------------------------------
# Set up the cumulative cut-flow for the standard pre-selection.
# This will use the Logical Filter Combiners to add individual cuts.
#--------------------------------------------------------------------
cmdString_GRL_PrimVtx = cmdString_GRL + "VertexSelectorInElLoose18Stream"
topSequence += LogicalFilterCombiner( "StreamElLoose18FilterCombiner_GRL_PrimVtx",
                                      cmdstring = cmdString_GRL_PrimVtx )
algsToBookkeep.append( "StreamElLoose18FilterCombiner_GRL_PrimVtx" )






#====================================================================
# Do the trigger selection
#====================================================================
include("PrimaryDPDMaker/SetupTrigDecisionTool.py")
from PrimaryDPDMaker.TriggerFilter import TriggerFilter
topSequence += TriggerFilter( "TriggerFilterInElLoose18Stream_L1_EM14",
                              trigger = "L1_EM14"
                              )
algsToBookkeep.append( "TriggerFilterInElLoose18Stream_L1_EM14" )

#--------------------------------------------------------------------
# Set up the cumulative cut-flow for the standard pre-selection.
# This will use the Logical Filter Combiners to add individual cuts.
#--------------------------------------------------------------------
# Create a subsequence: Only when the first algorithm returns isEventAccepted, the rest is executed
from AthenaCommon.AlgSequence import AthSequencer
grlPrimVtxTriggerEM14InElLoose18StreamSeq = AthSequencer("GRLPrimVtxTriggerEM14InElLoose18StreamSeq")
topSequence += grlPrimVtxTriggerEM14InElLoose18StreamSeq

print "TEST cmdString_GRL_PrimVtx = ", cmdString_GRL_PrimVtx
cmdString_GRL_PrimVtx_Trigger = cmdString_GRL_PrimVtx + " and TriggerFilterInElLoose18Stream_L1_EM14"
print "TEST cmdString_GRL_PrimVtx_Trigger = ", cmdString_GRL_PrimVtx_Trigger
grlPrimVtxTriggerEM14InElLoose18StreamSeq += LogicalFilterCombiner( "StreamElLoose18FilterCombiner_GRL_PrimVtx_Trigger",
                                                                    cmdstring = cmdString_GRL_PrimVtx_Trigger )
algsToBookkeep.append( "StreamElLoose18FilterCombiner_GRL_PrimVtx_Trigger" )

# Make some control plots
from D2PDMaker.D2PDMakerConf import CreateControlPlots
grlPrimVtxTriggerEM14InElLoose18StreamSeq += CreateControlPlots( "StreamElLoose18ControlPlots_GRL_PrimVtx_TriggerEM14",
                                                                 OutputLevel           = INFO,
                                                                 outputStreamName      = streamName,
                                                                 THistService          = 'THistSvc/ElLoose18StreamTHistSvc',
                                                                 inputVertexCollection = 'VxPrimaryCandidate',
                                                                 minNumberVxTracks     = 3
                                                                 )

# Add other triggers
# EF_e15_medium
topSequence += TriggerFilter( "TriggerFilterInElLoose18Stream_EF_e15_medium",
                              trigger = "EF_e15_medium"
                              )
algsToBookkeep.append( "TriggerFilterInElLoose18Stream_EF_e15_medium" )
# Create a sub-sequence for this trigger filter
grlPrimVtxTriggerE15MedInElLoose18StreamSeq = AthSequencer("GRLPrimVtxTriggerE15MedInElLoose18StreamSeq")
topSequence += grlPrimVtxTriggerE15MedInElLoose18StreamSeq

grlPrimVtxTriggerE15MedInElLoose18StreamSeq += LogicalFilterCombiner( "StreamElLoose18FilterCombiner_GRL_PrimVtx_TriggerE15Med",
                                                                      cmdstring = cmdString_GRL_PrimVtx + " and TriggerFilterInElLoose18Stream_EF_e15_medium" )
algsToBookkeep.append( "StreamElLoose18FilterCombiner_GRL_PrimVtx_TriggerE15Med" )

# Make some control plots
from D2PDMaker.D2PDMakerConf import CreateControlPlots
grlPrimVtxTriggerE15MedInElLoose18StreamSeq += CreateControlPlots( "StreamElLoose18ControlPlots_GRL_PrimVtx_TriggerE15Med",
                                                                   OutputLevel           = INFO,
                                                                   outputStreamName      = streamName,
                                                                   THistService          = 'THistSvc/ElLoose18StreamTHistSvc',
                                                                   inputVertexCollection = 'VxPrimaryCandidate',
                                                                   minNumberVxTracks     = 3
                                                                   )

# EF_e20_loose
topSequence += TriggerFilter( "TriggerFilterInElLoose18Stream_EF_e20_loose",
                              trigger = "EF_e20_loose"
                              )
algsToBookkeep.append( "TriggerFilterInElLoose18Stream_EF_e20_loose" )
# Create a sub-sequence for this trigger filter
grlPrimVtxTriggerE20LooseInElLoose18StreamSeq = AthSequencer("GRLPrimVtxTriggerE20LooseInElLoose18StreamSeq")
topSequence += grlPrimVtxTriggerE20LooseInElLoose18StreamSeq

grlPrimVtxTriggerE20LooseInElLoose18StreamSeq += LogicalFilterCombiner( "StreamElLoose18FilterCombiner_GRL_PrimVtx_TriggerE20Loose",
                                                                        cmdstring = cmdString_GRL_PrimVtx + " and TriggerFilterInElLoose18Stream_EF_e20_loose" )
algsToBookkeep.append( "StreamElLoose18FilterCombiner_GRL_PrimVtx_TriggerE20Loose" )

# Make some control plots
from D2PDMaker.D2PDMakerConf import CreateControlPlots
grlPrimVtxTriggerE20LooseInElLoose18StreamSeq += CreateControlPlots( "StreamElLoose18ControlPlots_GRL_PrimVtx_TriggerE20Loose",
                                                                     OutputLevel           = INFO,
                                                                     outputStreamName      = streamName,
                                                                     THistService          = 'THistSvc/ElLoose18StreamTHistSvc',
                                                                     inputVertexCollection = 'VxPrimaryCandidate',
                                                                     minNumberVxTracks     = 3
                                                                     )


# Make some control plots for all events
from D2PDMaker.D2PDMakerConf import CreateControlPlots
topSequence += CreateControlPlots( "StreamElLoose18ControlPlots_AllEvents",
                                   OutputLevel           = INFO,
                                   outputStreamName      = streamName,
                                   THistService          = 'THistSvc/ElLoose18StreamTHistSvc',
                                   inputVertexCollection = 'VxPrimaryCandidate',
                                   minNumberVxTracks     = 3
                                   )
algsToBookkeep.append( "StreamElLoose18ControlPlots_AllEvents" )



from D2PDMaker.D2PDMakerConf import D2PDElectronSelector
# Create an instance of this filter and add it to the AlgSequence
# Also, configure the filter to your needs. This are just some examples
topSequence += D2PDElectronSelector( "ElectronLooseSelectorInElLoose18Stream",
                                     OutputLevel          = INFO,
                                     inputCollection      = 'ElectronAODCollection',
                                     outputLinkCollection = 'CentralLooseElectronLinkCollectionInElLoose18Stream',
                                     minNumberPassed      = 1,
                                     electronIsEM         = egammaPID.ElectronLoose,
                                     electronAuthor       = egammaParameters.AuthorElectron,
                                     clusterEtMin         = 18.0*Units.GeV
                                     )
algsToBookkeep.append( "ElectronLooseSelectorInElLoose18Stream" )





#====================================================================
# Define the test DPD output stream
#====================================================================
ElLoose18Stream = MSMgr.NewPoolStream( streamName, fileName )

# Only events that pass the filters listed below are written out
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
ElLoose18Stream.AcceptAlgs( ["ElectronLooseSelectorInElLoose18Stream"] )

ElLoose18Stream.AddOtherAlgsToBookkeep( algsToBookkeep )

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
#ElLoose18Stream.AddItem( ['CompositeParticleContainer#*'] )
#ElLoose18Stream.AddItem( ['INav4MomLinkContainer#*'] )



#====================================================================
# Define the THistSvc
#====================================================================
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc( "ElLoose18StreamTHistSvc",
                        Output = [ "%s DATAFILE='%s' OPT='SHARE'" % ( streamName, fileName ) ]
                        )

