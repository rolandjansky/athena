##-----------------------------------------------------------------------------
## Name: D2PD_Zee.py
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
##
## Description: This defines the content of the D2PD example
##              Z -> ee output stream.
##
##-----------------------------------------------------------------------------

# Include the job property flags for this package and from RecExCommon
from D2PDMaker.D2PDFlags import D2PDFlags
from RecExConfig.RecFlags import rec

# for messaging
from AthenaCommon.Logging import logging
zeeDPD_msg = logging.getLogger( 'D2PD_ZEE' )
zeeDPD_msg.info( 'Starting configuration of %s' % D2PDFlags.WriteD2AOD_ZEEStream.StreamName )

# Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

# This handels multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr 

# Import the LogicalFilterCombiner
from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner



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
# For this, determine if the input file is of ESD or AOD type:
inputFileStreamType = "AOD"
if inputFileSummary.has_key("stream_names"):
    inputFileSummaryStreamNames = inputFileSummary.get("stream_names")
    if inputFileSummaryStreamNames.__contains__("ESD") :
        inputFileStreamType = "ESD"
        pass
    pass
streamName = "StreamD2" + inputFileStreamType + "M_ZEE"
fileName  = D2PDFlags.OutputDirectoryName() + "MyD2PDZeeStream.pool.root"



#====================================================================
# Define an empty list for bookkeeping
#====================================================================
algsToBookkeep = []


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


# Check for the presence of GEN_AOD, TruthEvent (ESD), GEN_EVENT, and SpclMC:
hasSpclMC = False
mcEventCollectionForMCTruthClassifier = ""
if inputIsSimulation :
    mcEventColls = ""
    if inputFileSummary.has_key("eventdata_itemsDic"):
        eventDic = inputFileSummary.get('eventdata_itemsDic')
        mcEventColls = eventDic['McEventCollection']
        if mcEventColls.__contains__("GEN_AOD") :
            mcEventCollectionForMCTruthClassifier = "GEN_AOD"
            pass
        elif mcEventColls.__contains__("TruthEvent") :
            mcEventCollectionForMCTruthClassifier = "TruthEvent"
            pass
        elif mcEventColls.__contains__("GEN_EVENT") :
            mcEventCollectionForMCTruthClassifier = "GEN_EVENT"
            pass
        truthPartColls = eventDic['TruthParticleContainer']
        if truthPartColls.__contains__("SpclMC") :
            hasSpclMC = True
            pass
        pass
    # Print a error message if no MCEventCollection is found
    if mcEventCollectionForMCTruthClassifier == "" :
        zeeDPD_msg.error( "No MCEventCollection found even though the input file is simulation!" )
        zeeDPD_msg.error( "These are the McEventCollections from the input file: %s" % mcEventColls )
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
    topSequence += GRLTriggerSelectorAlg( "GRLFilterInZeeStream",
                                          OutputLevel       = INFO,
                                          GoodRunsListArray = [ 'WZjets_allchannels' ] ## GRL name from the above xml-file
                                          )

    #theApp.getHandle().Dlls += ['XMLCoreParser'] ## Hack to avoid library mismatch after importing the GRL stuff

    algsToBookkeep.append( "GRLFilterInZeeStream" )
    #--------------------------------------------------------------------
    # Set up the cumulative cut-flow for the standard pre-selection.
    # This will use the Logical Filter Combiners to add individual cuts.
    #--------------------------------------------------------------------
    cmdString_GRL += "GRLFilterInZeeStream and "
    pass # End: if not inputIsSimulation






#====================================================================
# Do the PrimaryVertex selection
#====================================================================
from D2PDMaker.D2PDMakerConf import D2PDVertexSelector
topSequence += D2PDVertexSelector( "VertexSelectorInZeeStream",
                                   OutputLevel          = INFO,
                                   inputCollection      = 'VxPrimaryCandidate',
                                   minNumberVxTracks    = 3
                                   )
algsToBookkeep.append( "VertexSelectorInZeeStream" )

#--------------------------------------------------------------------
# Set up the cumulative cut-flow for the standard pre-selection.
# This will use the Logical Filter Combiners to add individual cuts.
#--------------------------------------------------------------------
cmdString_GRL_PrimVtx = cmdString_GRL + "VertexSelectorInZeeStream"
topSequence += LogicalFilterCombiner( "StreamZeeFilterCombiner_GRL_PrimVtx",
                                      cmdstring = cmdString_GRL_PrimVtx )
algsToBookkeep.append( "StreamZeeFilterCombiner_GRL_PrimVtx" )






#====================================================================
# Do the trigger selection
#====================================================================
include("PrimaryDPDMaker/SetupTrigDecisionTool.py")
from PrimaryDPDMaker.TriggerFilter import TriggerFilter
topSequence += TriggerFilter( "TriggerFilterInZeeStream_L1_EM14",
                              trigger = "L1_EM14"
                              )
algsToBookkeep.append( "TriggerFilterInZeeStream_L1_EM14" )

#--------------------------------------------------------------------
# Set up the cumulative cut-flow for the standard pre-selection.
# This will use the Logical Filter Combiners to add individual cuts.
#--------------------------------------------------------------------
# Create a subsequence: Only when the first algorithm returns isEventAccepted, the rest is executed
from AthenaCommon.AlgSequence import AthSequencer
grlPrimVtxTriggerEM14InZeeStreamSeq = AthSequencer("GRLPrimVtxTriggerEM14InZeeStreamSeq")
topSequence += grlPrimVtxTriggerEM14InZeeStreamSeq

cmdString_GRL_PrimVtx_Trigger = cmdString_GRL_PrimVtx + " and TriggerFilterInZeeStream_L1_EM14"
grlPrimVtxTriggerEM14InZeeStreamSeq += LogicalFilterCombiner( "StreamZeeFilterCombiner_GRL_PrimVtx_Trigger",
                                                              cmdstring = cmdString_GRL_PrimVtx_Trigger )
algsToBookkeep.append( "StreamZeeFilterCombiner_GRL_PrimVtx_Trigger" )

# Make some control plots
if D2PDFlags.WriteD2AOD_ZEEStream.CreateControlPlots :
    from D2PDMaker.D2PDMakerConf import CreateControlPlots
    grlPrimVtxTriggerEM14InZeeStreamSeq += CreateControlPlots( "StreamZeeControlPlots_GRL_PrimVtx_TriggerEM14",
                                                               OutputLevel           = INFO,
                                                               outputStreamName      = streamName,
                                                               THistService          = 'THistSvc/ZeeStreamTHistSvc',
                                                               inputVertexCollection = 'VxPrimaryCandidate',
                                                               minNumberVxTracks     = 3
                                                               )
    pass

# Add other triggers
# EF_e15_medium
topSequence += TriggerFilter( "TriggerFilterInZeeStream_EF_e15_medium",
                              trigger = "EF_e15_medium"
                              )
algsToBookkeep.append( "TriggerFilterInZeeStream_EF_e15_medium" )
# Create a sub-sequence for this trigger filter
grlPrimVtxTriggerE15MedInZeeStreamSeq = AthSequencer("GRLPrimVtxTriggerE15MedInZeeStreamSeq")
topSequence += grlPrimVtxTriggerE15MedInZeeStreamSeq

grlPrimVtxTriggerE15MedInZeeStreamSeq += LogicalFilterCombiner( "StreamZeeFilterCombiner_GRL_PrimVtx_TriggerE15Med",
                                                                cmdstring = cmdString_GRL_PrimVtx + " and TriggerFilterInZeeStream_EF_e15_medium" )
algsToBookkeep.append( "StreamZeeFilterCombiner_GRL_PrimVtx_TriggerE15Med" )

# Make some control plots
if D2PDFlags.WriteD2AOD_ZEEStream.CreateControlPlots :
    from D2PDMaker.D2PDMakerConf import CreateControlPlots
    grlPrimVtxTriggerE15MedInZeeStreamSeq += CreateControlPlots( "StreamZeeControlPlots_GRL_PrimVtx_TriggerE15Med",
                                                                 OutputLevel           = INFO,
                                                                 outputStreamName      = streamName,
                                                                 THistService          = 'THistSvc/ZeeStreamTHistSvc',
                                                                 inputVertexCollection = 'VxPrimaryCandidate',
                                                                 minNumberVxTracks     = 3
                                                                 )
    pass

# EF_e20_loose
topSequence += TriggerFilter( "TriggerFilterInZeeStream_EF_e20_loose",
                              trigger = "EF_e20_loose"
                              )
algsToBookkeep.append( "TriggerFilterInZeeStream_EF_e20_loose" )
# Create a sub-sequence for this trigger filter
grlPrimVtxTriggerE20LooseInZeeStreamSeq = AthSequencer("GRLPrimVtxTriggerE20LooseInZeeStreamSeq")
topSequence += grlPrimVtxTriggerE20LooseInZeeStreamSeq

grlPrimVtxTriggerE20LooseInZeeStreamSeq += LogicalFilterCombiner( "StreamZeeFilterCombiner_GRL_PrimVtx_TriggerE20Loose",
                                                                  cmdstring = cmdString_GRL_PrimVtx + " and TriggerFilterInZeeStream_EF_e20_loose" )
algsToBookkeep.append( "StreamZeeFilterCombiner_GRL_PrimVtx_TriggerE20Loose" )

# Make some control plots
if D2PDFlags.WriteD2AOD_ZEEStream.CreateControlPlots :
    from D2PDMaker.D2PDMakerConf import CreateControlPlots
    grlPrimVtxTriggerE20LooseInZeeStreamSeq += CreateControlPlots( "StreamZeeControlPlots_GRL_PrimVtx_TriggerE20Loose",
                                                                   OutputLevel           = INFO,
                                                                   outputStreamName      = streamName,
                                                                   THistService          = 'THistSvc/ZeeStreamTHistSvc',
                                                                   inputVertexCollection = 'VxPrimaryCandidate',
                                                                   minNumberVxTracks     = 3
                                                                   )
    pass

# Make some control plots for all events
if D2PDFlags.WriteD2AOD_ZEEStream.CreateControlPlots :
    from D2PDMaker.D2PDMakerConf import CreateControlPlots
    topSequence += CreateControlPlots( "StreamZeeControlPlots_AllEvents",
                                       OutputLevel           = INFO,
                                       outputStreamName      = streamName,
                                       THistService          = 'THistSvc/ZeeStreamTHistSvc',
                                       inputVertexCollection = 'VxPrimaryCandidate',
                                       minNumberVxTracks     = 3
                                       )
    pass




#====================================================================
# Do the jet/missingEt event cleaning
#====================================================================
# First, create an instance of the tool that will calculate the good,
# the bad, and the ugly for each jet and store the result in UserData.
from UserDataUtils.UserDataUtilsConf import UserDataCalcJetQualityTool
ToolSvc += UserDataCalcJetQualityTool( "UserDataCalcJetQualityToolInZeeStream",
                                       OutputLevel            = INFO,
                                       userDataPrefix         = "jet_",
                                       userDataJetQualityName = "isGoodUglyBad"
                                       )

# Now, use the JetSelector and use the UserDataCalcJetQualityTool with it
from D2PDMaker.D2PDMakerConf import D2PDJetSelector
topSequence += D2PDJetSelector( "JetMetCleaningSelectorInZeeStream",
                                OutputLevel          = INFO,
                                inputCollection      = 'AntiKt4TopoJets',
                                outputLinkCollection = 'EMScalePt10BadJetCollection',
                                #userDataCalcToolList = [ ToolSvc.UserDataCalcJetQualityToolInZeeStream ],
                                minNumberPassed      = 1,
                                useJetSignalState    = PyAthena.P4SignalState.JETEMSCALE,
                                jetSignalStatePtMin  = 10.0*Units.GeV,
                                #userDataNameList     = [ "jet_isGoodUglyBad" ],
                                #userDataMinCutList   = [ 3.5 ] # good=1, ugly=2, bad=4 (combinations add up)
                                )
algsToBookkeep.append( "JetMetCleaningSelectorInZeeStream" )

#--------------------------------------------------------------------
# Set up the cumulative cut-flow for the standard pre-selection.
# This will use the Logical Filter Combiners to add individual cuts.
#--------------------------------------------------------------------
cmdString_GRL_PrimVtx_Trigger_JetMetClean = cmdString_GRL_PrimVtx_Trigger + " and not JetMetCleaningSelectorInZeeStream"
topSequence += LogicalFilterCombiner( "StreamZeeFilterCombiner_GRL_PrimVtx_Trigger_JetMetClean",
                                      cmdstring = cmdString_GRL_PrimVtx_Trigger_JetMetClean )
algsToBookkeep.append( "StreamZeeFilterCombiner_GRL_PrimVtx_Trigger_JetMetClean" )





#====================================================================
# Do the Monte Carlo Truth selections, if the input is a simulation file
#====================================================================
if inputIsSimulation :
    # Truth pre-selector (this way, only one loop over the SpclMC container is needed per event -> faster)
    from D2PDMaker.D2PDMakerConf import D2PDTruthParticleSelector
    topSequence += D2PDTruthParticleSelector( "TruthPreSelectorInZeeStream",
                                              OutputLevel             = INFO,
                                              inputCollection         = 'SpclMC',
                                              outputLinkCollection    = 'TruthParticlePreSelectedLinkCollection',
                                              minNumberPassed         = 1,
                                              pdgIDList               = [ 4, 5, 6, 11, 12, 13, 14, 15, 16, 22, 23, 24, 25 ], # c, b, t, e, nu_e, mu, nu_mu, tau, nu_tau, gamma, Z, W, H
                                              removeDocumentationLine = False,
                                              keepOnlyGenerator       = True,
                                              keepOnlyStable          = False,
                                              removeSelfDecay         = True
                                              )
    algsToBookkeep.append( "TruthPreSelectorInZeeStream" )

    # Truth-electron selector:
    topSequence += D2PDTruthParticleSelector( "TruthElectronSelectorInZeeStream",
                                              OutputLevel             = INFO,
                                              inputCollection         = 'TruthParticlePreSelectedLinkCollection',
                                              outputLinkCollection    = 'TruthElectronFromZBosonLinkCollection',
                                              minNumberPassed         = 2,
                                              pdgIDList               = [ 11 ], # e-, e+
                                              pdgIDListOrigin         = [ 23 ], # Z boson
                                              removeDocumentationLine = False,
                                              keepOnlyGenerator       = True,
                                              keepOnlyStable          = False,
                                              removeSelfDecay         = True
                                              )
    algsToBookkeep.append( "TruthElectronSelectorInZeeStream" )

    # Truth-electron selector for status code == 1:
    topSequence += D2PDTruthParticleSelector( "TruthElectronStatusCode1SelectorInZeeStream",
                                              OutputLevel          = INFO,
                                              inputCollection      = 'TruthElectronFromZBosonLinkCollection',
                                              outputLinkCollection = 'TruthElectronStatusCode1FromZBosonLinkCollection',
                                              minNumberPassed      = 2,
                                              acceptStatusCodeList = [ 1 ]
                                              )
    algsToBookkeep.append( "TruthElectronStatusCode1SelectorInZeeStream" )

    # Truth-electron selector for status code == 3:
    topSequence += D2PDTruthParticleSelector( "TruthElectronStatusCode3SelectorInZeeStream",
                                              OutputLevel          = INFO,
                                              inputCollection      = 'TruthElectronFromZBosonLinkCollection',
                                              outputLinkCollection = 'TruthElectronStatusCode3FromZBosonLinkCollection',
                                              minNumberPassed      = 2,
                                              acceptStatusCodeList = [ 3 ]
                                              )
    algsToBookkeep.append( "TruthElectronStatusCode3SelectorInZeeStream" )

    # Truth-photon selector:
    topSequence += D2PDTruthParticleSelector( "TruthPhotonSelectorInZeeStream",
                                              OutputLevel             = INFO,
                                              inputCollection         = 'TruthParticlePreSelectedLinkCollection',
                                              outputLinkCollection    = 'TruthPhotonFromFSRLinkCollection',
                                              minNumberPassed         = 1,
                                              pdgIDList               = [ 22 ], # gamma
                                              pdgIDListOrigin         = [ 11, 23, 24 ], # e-, e+, Z, W boson
                                              removeDocumentationLine = True,
                                              keepOnlyGenerator       = True,
                                              keepOnlyStable          = True,
                                              removeSelfDecay         = True,
                                              etaMax                  = 5.1
                                              )
    algsToBookkeep.append( "TruthPhotonSelectorInZeeStream" )

    # Truth Z0 selector:
    topSequence += D2PDTruthParticleSelector( "TruthZBosonSelectorInZeeStream",
                                              OutputLevel             = INFO,
                                              inputCollection         = 'TruthParticlePreSelectedLinkCollection',
                                              outputLinkCollection    = 'TruthZBosonLinkCollection',
                                              minNumberPassed         = 1,
                                              pdgIDList               = [ 23 ], # Z boson
                                              removeDocumentationLine = True,
                                              keepOnlyGenerator       = False,
                                              keepOnlyStable          = False,
                                              removeSelfDecay         = True
                                              )
    algsToBookkeep.append( "TruthZBosonSelectorInZeeStream" )

    # Truth W selector:
    topSequence += D2PDTruthParticleSelector( "TruthWBosonSelectorInWenuStream",
                                              OutputLevel             = INFO,
                                              inputCollection         = 'TruthParticlePreSelectedLinkCollection',
                                              outputLinkCollection    = 'TruthWBosonLinkCollection',
                                              minNumberPassed         = 1,
                                              pdgIDList               = [ 24 ], # Z boson
                                              removeDocumentationLine = True,
                                              keepOnlyGenerator       = False,
                                              keepOnlyStable          = False,
                                              removeSelfDecay         = True
                                              )
    algsToBookkeep.append( "TruthWBosonSelectorInWenuStream" )

    # Combine truth electron and truth photon in a DeltaR cone of 0.05 (final state radiation recovery)
    from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner
    topSequence += D2PDParticleCombiner( "TruthElectronTruthPhotonCombinerInZeeStream",
                                         OutputLevel             = INFO,
                                         compositeParticlePDG_ID = 11, # This is an electron
                                         inputCollectionList     = [ 'TruthElectronStatusCode1FromZBosonLinkCollection',
                                                                     'TruthPhotonFromFSRLinkCollection' ],
                                         outputCollection        = 'CombinedTruthElectronTruthPhotonCollection',
                                         deltaRMax               = 0.05
                                         )
    algsToBookkeep.append( "TruthElectronTruthPhotonCombinerInZeeStream" )


    # Truth matching/classification using the MCTruthClassifier
    # First, set up the standard MCTruthClassifier
    from MCTruthClassifier.MCTruthClassifierBase import MCTruthClassifier as ElectronMCTruthClassifierInZeeStream
    ElectronMCTruthClassifierInZeeStream.McEventCollection = mcEventCollectionForMCTruthClassifier

    # Now, use the MCTruthClassifier for the association/matching tool
    from AssociationComps.AssociationCompsConf import MCTruthClassifierAssociationTool
    ToolSvc += MCTruthClassifierAssociationTool( "ElectronMCTruthClassifierAssociationToolInZeeStream",
                                                 OutputLevel                = INFO,
                                                 mcTruthClassifier          = ElectronMCTruthClassifierInZeeStream,
                                                 inputAssociateToCollection = "SpclMC",
                                                 userDataPrefix             = "el_mcTruthClassifier_",
                                                 writeUserData              = True
                                                 )
    
    # Set up a DeltaR association/matching tool
    from AssociationComps.AssociationCompsConf import DeltaRAssociationTool
    ToolSvc += DeltaRAssociationTool( "ElectronDeltaRTruthAssociationToolInZeeStream",
                                      OutputLevel                = INFO,
                                      inputAssociateToCollection = "TruthElectronFromZBosonLinkCollection",
                                      deltaRMax                  = 0.1,
                                      userDataPrefix             = "el_deltaRMatchTruth_",
                                      writeUserData              = False
                                      )

    # Now, do a match to status code 1 electrons
    ToolSvc += DeltaRAssociationTool( "ElectronDeltaRTruthSC1AssociationToolInZeeStream",
                                      OutputLevel                = INFO,
                                      inputAssociateToCollection = "TruthElectronStatusCode1FromZBosonLinkCollection",
                                      deltaRMax                  = 0.1,
                                      userDataPrefix             = "el_deltaRMatchTruth_sc1_",
                                      writeUserData              = False
                                      )

    # Now, do a match to status code 3 electrons
    ToolSvc += DeltaRAssociationTool( "ElectronDeltaRTruthSC3AssociationToolInZeeStream",
                                      OutputLevel                = INFO,
                                      inputAssociateToCollection = "TruthElectronStatusCode3FromZBosonLinkCollection",
                                      deltaRMax                  = 0.1,
                                      userDataPrefix             = "el_deltaRMatchTruth_sc3_",
                                      writeUserData              = False
                                      )

    
    pass # End: if inputIsSimulation







#====================================================================
# Define the individual di-cluster selection
# This one is actually used to fitler events later on (very loose pre-selection)
#====================================================================
from D2PDMaker.D2PDMakerConf import D2PDINavigable4MomentumSelector
topSequence += D2PDINavigable4MomentumSelector( "DiClusterInZeeStream",
                                                OutputLevel          = INFO,
                                                inputCollection      = 'egClusterCollection',
                                                outputLinkCollection = 'EGClusterLinkCollection',
                                                minNumberPassed      = 2,
                                                etMin                = 18.0*Units.GeV
                                                )
algsToBookkeep.append( "DiClusterInZeeStream" )






#====================================================================
# Define the individual central electron selections
#====================================================================
# Load the actual filter
from D2PDMaker.D2PDMakerConf import D2PDElectronSelector
# Create an instance of this filter and add it to the AlgSequence
# Also, configure the filter to your needs. This are just some examples
topSequence += D2PDElectronSelector( "ElectronLooseSelectorInZeeStream",
                                     OutputLevel          = INFO,
                                     inputCollection      = 'ElectronAODCollection',
                                     outputLinkCollection = 'CentralLooseElectronLinkCollection',
                                     minNumberPassed      = 1,
                                     electronIsEM         = egammaPID.ElectronLoose,
                                     electronAuthor       = egammaParameters.AuthorElectron,
                                     clusterEtMin         = 18.0*Units.GeV
                                     )
algsToBookkeep.append( "ElectronLooseSelectorInZeeStream" )

#--------------------------------------------------------------------
# If the input is Monte Carlo, also apply truth matching.
# This will be done for ALL electrons in the ElectronAODCollection
# and not only for the ones surviving the selection cuts!
#--------------------------------------------------------------------
zeeAssociationToolList = []
zeeOutputAssociationContainerList = []
if inputIsSimulation :
    # Schedule the associations with the electron selector, i.e., performed for all electrons in the ElectronAODCollection
    zeeAssociationToolList += [ ToolSvc.ElectronMCTruthClassifierAssociationToolInZeeStream,
                                ToolSvc.ElectronDeltaRTruthAssociationToolInZeeStream,
                                ToolSvc.ElectronDeltaRTruthSC1AssociationToolInZeeStream,
                                ToolSvc.ElectronDeltaRTruthSC3AssociationToolInZeeStream ]
    zeeOutputAssociationContainerList += [ "ElectronMCTruthClassifierMatch",
                                           "ElectronDeltaRTruthMatch",
                                           "ElectronDeltaRTruthStatusCode1Match",
                                           "ElectronDeltaRTruthStatusCode3Match" ]
    
    pass # End: if inputIsSimulation

# Also, add the trigger associations
from AssociationComps.AssociationCompsConf import TriggerAssociationTool
ToolSvc += TriggerAssociationTool( "ElectronTriggerAssociationToolInZeeStream_EF_e15_medium",
                                   OutputLevel               = INFO,
                                   triggerToMatchTo          = "EF_e15_medium",
                                   deltaR                    = 0.1,
                                   matchToPassedTriggersOnly = True
                                   )
zeeAssociationToolList += [ ToolSvc.ElectronTriggerAssociationToolInZeeStream_EF_e15_medium ]
zeeOutputAssociationContainerList += [ "ElectronTriggerMatch_EF_e15_medium" ]

ToolSvc += TriggerAssociationTool( "ElectronTriggerAssociationToolInZeeStream_L1_EM14",
                                   OutputLevel               = INFO,
                                   triggerToMatchTo          = "L1_EM14",
                                   deltaR                    = 0.1,
                                   matchToPassedTriggersOnly = True
                                   )
zeeAssociationToolList            += [ ToolSvc.ElectronTriggerAssociationToolInZeeStream_L1_EM14 ]
zeeOutputAssociationContainerList += [ "ElectronTriggerMatch_L1_EM14" ]

# Now, add all association tools and maps to the algorithm
topSequence.ElectronLooseSelectorInZeeStream.associationToolList            = zeeAssociationToolList
topSequence.ElectronLooseSelectorInZeeStream.outputAssociationContainerList = zeeOutputAssociationContainerList

print "AssociationList = ", topSequence.ElectronLooseSelectorInZeeStream.outputAssociationContainerList

# Define a medium electron selection
topSequence += D2PDElectronSelector( "ElectronMediumSelectorInZeeStream",
                                     OutputLevel          = INFO,
                                     inputCollection      = 'ElectronAODCollection',
                                     outputLinkCollection = 'CentralMediumElectronLinkCollection',
                                     minNumberPassed      = 1,
                                     electronIsEM         = egammaPID.ElectronMedium,
                                     electronAuthor       = egammaParameters.AuthorElectron,
                                     clusterEtMin         = 18.0*Units.GeV
                                     )
algsToBookkeep.append( "ElectronMediumSelectorInZeeStream" )

# Define a tight electron selection
topSequence += D2PDElectronSelector( "ElectronTightSelectorInZeeStream",
                                     OutputLevel          = INFO,
                                     inputCollection      = 'ElectronAODCollection',
                                     outputLinkCollection = 'CentralTightElectronLinkCollection',
                                     minNumberPassed      = 1,
                                     electronIsEM         = egammaPID.ElectronTight,
                                     electronAuthor       = egammaParameters.AuthorElectron,
                                     clusterEtMin         = 18.0*Units.GeV
                                     )
algsToBookkeep.append( "ElectronTightSelectorInZeeStream" )


# Define a di-electron selection
topSequence += D2PDElectronSelector( "DiElectronSelectorInZeeStream",
                                     OutputLevel          = INFO,
                                     inputCollection      = 'ElectronAODCollection',
                                     outputLinkCollection = 'ElectronEt15LinkCollection',
                                     minNumberPassed      = 2,
                                     clusterEtMin         = 18.0*Units.GeV
                                     )
algsToBookkeep.append( "DiElectronSelectorInZeeStream" )

# Make a combination filter
topSequence += LogicalFilterCombiner( "StreamZeeFilterCombiner_MediumElectronPlusDiElectron",
                                      cmdstring = "ElectronMediumSelectorInZeeStream and DiElectronSelectorInZeeStream" )
algsToBookkeep.append( "StreamZeeFilterCombiner_MediumElectronPlusDiElectron" )

# Make a combination filter
topSequence += LogicalFilterCombiner( "StreamZeeFilterCombiner_TightElectronPlusDiCluster",
                                      cmdstring = "ElectronTightSelectorInZeeStream and DiClusterInZeeStream" )
algsToBookkeep.append( "StreamZeeFilterCombiner_TightElectronPlusDiCluster" )



#====================================================================
# Continue with refined offline electron selections
#====================================================================
# Define a medium E_T di-electron selection
topSequence += D2PDElectronSelector( "DiCentralElectronSelectorInZeeStream",
                                     OutputLevel          = INFO,
                                     inputCollection      = 'ElectronAODCollection',
                                     outputLinkCollection = 'CentralElectronLinkCollection',
                                     minNumberPassed      = 2,
                                     electronAuthor       = egammaParameters.AuthorElectron
                                     )
algsToBookkeep.append( "DiCentralElectronSelectorInZeeStream" )

#--------------------------------------------------------------------
# Set up the cumulative cut-flow for the standard pre-selection.
# This will use the Logical Filter Combiners to add individual cuts.
#--------------------------------------------------------------------
cmdString_GRL_PrimVtx_Trigger_DiEl = cmdString_GRL_PrimVtx_Trigger + " and DiCentralElectronSelectorInZeeStream"
topSequence += LogicalFilterCombiner( "StreamZeeFilterCombiner_GRL_PrimVtx_Trigger_DiEl",
                                      cmdstring = cmdString_GRL_PrimVtx_Trigger_DiEl )
algsToBookkeep.append( "StreamZeeFilterCombiner_GRL_PrimVtx_Trigger_DiEl" )






#====================================================================
# Continue with refined offline electron selections
#====================================================================
# central electron list with eta cuts
topSequence += D2PDElectronSelector( "DiElectronEtaSelectorInZeeStream",
                                     OutputLevel          = INFO,
                                     inputCollection      = 'CentralElectronLinkCollection',
                                     outputLinkCollection = 'CentralElectronEtaLinkCollection',
                                     minNumberPassed      = 2,
                                     clusterEtaMin        = -2.47,
                                     clusterEtaMax        = 2.47,
                                     clusterEtaVetoRanges = "[-1.52, -1.37], [1.37, 1.52]"
                                     )
algsToBookkeep.append( "DiElectronEtaSelectorInZeeStream" )

#--------------------------------------------------------------------
# Set up the cumulative cut-flow for the standard pre-selection.
# This will use the Logical Filter Combiners to add individual cuts.
#--------------------------------------------------------------------
cmdString_GRL_PrimVtx_Trigger_DiEl_Eta = cmdString_GRL_PrimVtx_Trigger_DiEl + " and DiElectronEtaSelectorInZeeStream"
topSequence += LogicalFilterCombiner( "StreamZeeFilterCombiner_GRL_PrimVtx_Trigger_DiEl_Eta",
                                      cmdstring = cmdString_GRL_PrimVtx_Trigger_DiEl_Eta )
algsToBookkeep.append( "StreamZeeFilterCombiner_GRL_PrimVtx_Trigger_DiEl_Eta" )







# Define a medium E_T di-electron selection
topSequence += D2PDElectronSelector( "DiCentralElectronClusEt15SelectorInZeeStream",
                                     OutputLevel          = INFO,
                                     inputCollection      = 'CentralElectronEtaLinkCollection',
                                     outputLinkCollection = 'CentralElectronEtaEt15LinkCollection',
                                     minNumberPassed      = 2,
                                     clusterEtMin         = 15.0*Units.GeV
                                     )
algsToBookkeep.append( "DiCentralElectronClusEt15SelectorInZeeStream" )

#--------------------------------------------------------------------
# Set up the cumulative cut-flow for the standard pre-selection.
# This will use the Logical Filter Combiners to add individual cuts.
#--------------------------------------------------------------------
cmdString_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15 = cmdString_GRL_PrimVtx_Trigger_DiEl_Eta + " and DiCentralElectronClusEt15SelectorInZeeStream"
topSequence += LogicalFilterCombiner( "StreamZeeFilterCombiner_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15",
                                      cmdstring = cmdString_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15 )
algsToBookkeep.append( "StreamZeeFilterCombiner_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15" )


# Define the standard di-electron selection
topSequence += D2PDElectronSelector( "DiCentralElectronClusEt20SelectorInZeeStream",
                                     OutputLevel          = INFO,
                                     inputCollection      = 'CentralElectronEtaEt15LinkCollection',
                                     outputLinkCollection = 'CentralElectronEtaEt20LinkCollection',
                                     minNumberPassed      = 2,
                                     clusterEtMin         = 20.0*Units.GeV
                                     )
algsToBookkeep.append( "DiCentralElectronClusEt20SelectorInZeeStream" )

#--------------------------------------------------------------------
# Set up the cumulative cut-flow for the standard pre-selection.
# This will use the Logical Filter Combiners to add individual cuts.
#--------------------------------------------------------------------
cmdString_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15_ClusEt20 = cmdString_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15 + " and DiCentralElectronClusEt20SelectorInZeeStream"
topSequence += LogicalFilterCombiner( "StreamZeeFilterCombiner_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15_ClusEt20",
                                      cmdstring = cmdString_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15_ClusEt20 )
algsToBookkeep.append( "StreamZeeFilterCombiner_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15_ClusEt20" )






#====================================================================
# Continue with refined offline electron selections
#====================================================================

# Loose electron with 20 GeV
topSequence += D2PDElectronSelector( "ElectronLooseEt20SelectorInZeeStream",
                                     OutputLevel          = INFO,
                                     inputCollection      = 'CentralElectronEtaEt20LinkCollection',
                                     outputLinkCollection = 'CentralLooseElectronEtaEt20LinkCollection',
                                     minNumberPassed      = 2,
                                     electronIsEM         = egammaPID.ElectronLoose
                                     )
algsToBookkeep.append( "ElectronLooseEt20SelectorInZeeStream" )


# Medium electron with 20 GeV
topSequence += D2PDElectronSelector( "ElectronMediumEt20SelectorInZeeStream",
                                     OutputLevel          = INFO,
                                     inputCollection      = 'CentralLooseElectronEtaEt20LinkCollection',
                                     outputLinkCollection = 'CentralMediumElectronEtaEt20LinkCollection',
                                     minNumberPassed      = 2,
                                     electronIsEM         = egammaPID.ElectronMedium
                                     )
algsToBookkeep.append( "ElectronMediumEt20SelectorInZeeStream" )


# Tight electron with 20 GeV
topSequence += D2PDElectronSelector( "ElectronTightEt20SelectorInZeeStream",
                                     OutputLevel          = INFO,
                                     inputCollection      = 'CentralMediumElectronEtaEt20LinkCollection',
                                     outputLinkCollection = 'CentralTightElectronEtaEt20LinkCollection',
                                     minNumberPassed      = 2,
                                     electronIsEM         = egammaPID.ElectronTight
                                     )
algsToBookkeep.append( "ElectronTightEt20SelectorInZeeStream" )


#====================================================================
# Define the individual forward electron selections
#====================================================================
# forward  electron et > 15
topSequence += D2PDElectronSelector( "ForwardElectronEt15SelectorInZeeStream",
                                     OutputLevel          = INFO,
                                     inputCollection      = 'ElectronAODCollection',
                                     outputLinkCollection = 'ForwardElectronEt15LinkCollection',
                                     minNumberPassed      = 1,
                                     electronAuthor       = egammaParameters.AuthorFrwd,
                                     clusterEtMin         = 15.0*Units.GeV
                                     )
algsToBookkeep.append( "ForwardElectronEt15SelectorInZeeStream" )

# forward  electron et > 20
topSequence += D2PDElectronSelector( "ForwardElectronEt20SelectorInZeeStream",
                                     OutputLevel          = INFO,
                                     inputCollection      = 'ElectronAODCollection',
                                     outputLinkCollection = 'ForwardElectronEt20LinkCollection',
                                     minNumberPassed      = 1,
                                     electronAuthor       = egammaParameters.AuthorFrwd,
                                     clusterEtMin         = 20.0*Units.GeV
                                     )
algsToBookkeep.append( "ForwardElectronEt20SelectorInZeeStream" )









#====================================================================
# Define the Zee combinations for two central electrons
#====================================================================
from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner
# - the Z->ee Boson (on-shell), all combinations with pre-selected electrons with 15 GeV
topSequence += D2PDParticleCombiner( "ZeeCCEt15CombinerInZeeStream",
                                     OutputLevel             = INFO,
                                     compositeParticlePDG_ID = 23, # This is a Z Boson
                                     inputCollectionList     = [ 'CentralElectronEtaEt15LinkCollection',
                                                                 'CentralElectronEtaEt15LinkCollection' ],
                                     outputCollection        = 'ZeeCCEt15BosonCollection',
                                     minNumberPassed         = 1
                                     )
algsToBookkeep.append( "ZeeCCEt15CombinerInZeeStream" )

# - the Z->ee Boson (on-shell), all combinations with pre-selected electrons with 20 GeV
topSequence += D2PDParticleCombiner( "ZeeCCEt20CombinerInZeeStream",
                                     OutputLevel             = INFO,
                                     compositeParticlePDG_ID = 23, # This is a Z Boson
                                     inputCollectionList     = [ 'CentralElectronEtaEt20LinkCollection',
                                                                 'CentralElectronEtaEt20LinkCollection' ],
                                     outputCollection        = 'ZeeCCEt20BosonCollection',
                                     minNumberPassed         = 1
                                     )
algsToBookkeep.append( "ZeeCCEt20CombinerInZeeStream" )





# Opposite charge Zee
topSequence += D2PDParticleCombiner( "ZeeCCEt20OSCombinerInZeeStream",
                                     OutputLevel             = INFO,
                                     compositeParticlePDG_ID = 23, # This is a Z Boson
                                     inputCollectionList     = [ 'CentralElectronEtaEt20LinkCollection',
                                                                 'CentralElectronEtaEt20LinkCollection' ],
                                     outputCollection        = 'ZeeCCEt20OSBosonCollection',
                                     minNumberPassed         = 1,
                                     chargeMax               = 0
                                     )
algsToBookkeep.append( "ZeeCCEt20OSCombinerInZeeStream" )

#--------------------------------------------------------------------
# Set up the cumulative cut-flow for the standard pre-selection.
# This will use the Logical Filter Combiners to add individual cuts.
#--------------------------------------------------------------------
cmdString_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15_ClusEt20_Charge = cmdString_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15_ClusEt20 + " and ZeeCCEt20OSCombinerInZeeStream"
topSequence += LogicalFilterCombiner( "StreamZeeFilterCombiner_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15_ClusEt20_Charge",
                                      cmdstring = cmdString_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15_ClusEt20_Charge )
algsToBookkeep.append( "StreamZeeFilterCombiner_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15_ClusEt20_Charge" )







# Opposite charge loose Zee
topSequence += D2PDParticleCombiner( "ZeeLooseCCEt20OSCombinerInZeeStream",
                                     OutputLevel             = INFO,
                                     compositeParticlePDG_ID = 23, # This is a Z Boson
                                     inputCollectionList     = [ 'CentralLooseElectronEtaEt20LinkCollection',
                                                                 'CentralLooseElectronEtaEt20LinkCollection' ],
                                     outputCollection        = 'ZeeLooseCCEt20OSBosonCollection',
                                     minNumberPassed         = 1,
                                     chargeMax               = 0
                                     )
algsToBookkeep.append( "ZeeLooseCCEt20OSCombinerInZeeStream" )


#--------------------------------------------------------------------
# Set up the cumulative cut-flow for the standard pre-selection.
# This will use the Logical Filter Combiners to add individual cuts.
#--------------------------------------------------------------------
cmdString_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15_ClusEt20_Charge_Loose = cmdString_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15_ClusEt20_Charge + " and ZeeLooseCCEt20OSCombinerInZeeStream"
topSequence += LogicalFilterCombiner( "StreamZeeFilterCombiner_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15_ClusEt20_Charge_Loose",
                                      cmdstring = cmdString_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15_ClusEt20_Charge_Loose )
algsToBookkeep.append( "StreamZeeFilterCombiner_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15_ClusEt20_Charge_Loose" )







# Opposite charge medium Zee
topSequence += D2PDParticleCombiner( "ZeeMediumCCEt20OSCombinerInZeeStream",
                                     OutputLevel             = INFO,
                                     compositeParticlePDG_ID = 23, # This is a Z Boson
                                     inputCollectionList     = [ 'CentralMediumElectronEtaEt20LinkCollection',
                                                                 'CentralMediumElectronEtaEt20LinkCollection' ],
                                     outputCollection        = 'ZeeMediumCCEt20OSBosonCollection',
                                     minNumberPassed         = 1,
                                     chargeMax               = 0
                                     )
algsToBookkeep.append( "ZeeMediumCCEt20OSCombinerInZeeStream" )


#--------------------------------------------------------------------
# Set up the cumulative cut-flow for the standard pre-selection.
# This will use the Logical Filter Combiners to add individual cuts.
#--------------------------------------------------------------------
cmdString_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15_ClusEt20_Charge_Loose_Medium = cmdString_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15_ClusEt20_Charge_Loose + " and ZeeMediumCCEt20OSCombinerInZeeStream"
topSequence += LogicalFilterCombiner( "StreamZeeFilterCombiner_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15_ClusEt20_Charge_Loose_Medium",
                                      cmdstring = cmdString_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15_ClusEt20_Charge_Loose_Medium )
algsToBookkeep.append( "StreamZeeFilterCombiner_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15_ClusEt20_Charge_Loose_Medium" )







# Opposite charge tight Zee
topSequence += D2PDParticleCombiner( "ZeeTightCCEt20OSCombinerInZeeStream",
                                     OutputLevel             = INFO,
                                     compositeParticlePDG_ID = 23, # This is a Z Boson
                                     inputCollectionList     = [ 'CentralTightElectronEtaEt20LinkCollection',
                                                                 'CentralTightElectronEtaEt20LinkCollection' ],
                                     outputCollection        = 'ZeeTightCCEt20OSBosonCollection',
                                     minNumberPassed         = 1,
                                     chargeMax               = 0
                                     )
algsToBookkeep.append( "ZeeTightCCEt20OSCombinerInZeeStream" )


#--------------------------------------------------------------------
# Set up the cumulative cut-flow for the standard pre-selection.
# This will use the Logical Filter Combiners to add individual cuts.
#--------------------------------------------------------------------
cmdString_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15_ClusEt20_Charge_Loose_Medium_Tight = cmdString_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15_ClusEt20_Charge_Loose_Medium + " and ZeeTightCCEt20OSCombinerInZeeStream"
topSequence += LogicalFilterCombiner( "StreamZeeFilterCombiner_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15_ClusEt20_Charge_Loose_Medium_Tight",
                                      cmdstring = cmdString_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15_ClusEt20_Charge_Loose_Medium_Tight )
algsToBookkeep.append( "StreamZeeFilterCombiner_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15_ClusEt20_Charge_Loose_Medium_Tight" )







# Zee plus invarian mass cut
topSequence += D2PDINavigable4MomentumSelector( "ZeeMediumCCEt20OSInvMassCombinerInZeeStream",
                                                OutputLevel          = INFO,
                                                inputCollection      = 'ZeeMediumCCEt20OSBosonCollection',
                                                outputLinkCollection = 'ZeeMediumCCEt20OSInvMassBosonCollection',
                                                minNumberPassed      = 1,
                                                massMin              = 66.0*Units.GeV,
                                                massMax              = 116.0*Units.GeV
                                                )
algsToBookkeep.append( "ZeeMediumCCEt20OSInvMassCombinerInZeeStream" )


#--------------------------------------------------------------------
# Set up the cumulative cut-flow for the standard pre-selection.
# This will use the Logical Filter Combiners to add individual cuts.
#--------------------------------------------------------------------
cmdString_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15_ClusEt20_Charge_Loose_Medium_InvMass = cmdString_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15_ClusEt20_Charge_Loose_Medium + " and ZeeMediumCCEt20OSInvMassCombinerInZeeStream"
topSequence += LogicalFilterCombiner( "StreamZeeFilterCombiner_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15_ClusEt20_Charge_Loose_Medium_InvMass",
                                      cmdstring = cmdString_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15_ClusEt20_Charge_Loose_Medium_InvMass )
algsToBookkeep.append( "StreamZeeFilterCombiner_GRL_PrimVtx_Trigger_DiEl_Eta_ClusEt15_ClusEt20_Charge_Loose_Medium_InvMass" )





#====================================================================
# Define the Zee combinations for one central electron plus one forward electron
#====================================================================
# - the Z->ee Boson (on-shell), all combinations with pre-selected electrons (central-forward) with 15 GeV
topSequence += D2PDParticleCombiner( "ZeeCFEt15CombinerInZeeStream",
                                     OutputLevel             = INFO,
                                     compositeParticlePDG_ID = 23, # This is a Z Boson
                                     inputCollectionList     = [ 'CentralElectronEtaEt15LinkCollection',
                                                                 'ForwardElectronEt15LinkCollection' ],
                                     outputCollection        = 'ZeeCFEt15BosonCollection',
                                     minNumberPassed         = 1
                                     )
algsToBookkeep.append( "ZeeCFEt15CombinerInZeeStream" )

# - the Z->ee Boson (on-shell), all combinations with pre-selected electrons (central-forward) with 20 GeV
topSequence += D2PDParticleCombiner( "ZeeCFEt20CombinerInZeeStream",
                                     OutputLevel             = INFO,
                                     compositeParticlePDG_ID = 23, # This is a Z Boson
                                     inputCollectionList     = [ 'CentralElectronEtaEt20LinkCollection',
                                                                 'ForwardElectronEt20LinkCollection' ],
                                     outputCollection        = 'ZeeCFEt20BosonCollection',
                                     minNumberPassed         = 1
                                     )
algsToBookkeep.append( "ZeeCFEt20CombinerInZeeStream" )







# Create a subsequence: Only when the first algorithm returns isEventAccepted, the rest is executed
from AthenaCommon.AlgSequence import AthSequencer
thinSeqZeeStream = AthSequencer("ZeeStreamThinningSeq")
topSequence += thinSeqZeeStream

# Create the combined decission.
# The rest of the thinSeqZeeStream will only be executed if this first algorithm accepts the event
#thinSeqZeeStream += LogicalFilterCombiner( "StreamZee_AcceptEvent",
#                                           OutputLevel = INFO,
#                                           cmdstring = '' )

if D2PDFlags.WriteD2AOD_ZEEStream.ApplyThinning :
    # Apply the trigger navigation slimming
    include("PrimaryDPDMaker/SetupTrigDecisionTool.py")
    from TrigNavTools.TrigNavToolsConfig import HLTStreamTrigNavSlimming
    thinSeqZeeStream += HLTStreamTrigNavSlimming( "StreamZee_HLTStreamTrigNavSlimming" )
    thinSeqZeeStream.StreamZee_HLTStreamTrigNavSlimming.OutputLevel         = INFO
    thinSeqZeeStream.StreamZee_HLTStreamTrigNavSlimming.ThinningSvc         = 'ThinningSvc/ZeeStreamThinning'
    thinSeqZeeStream.StreamZee_HLTStreamTrigNavSlimming.PrintTree           = 0
    thinSeqZeeStream.StreamZee_HLTStreamTrigNavSlimming.ProtectOtherStreams = 1
    thinSeqZeeStream.StreamZee_HLTStreamTrigNavSlimming.Squeeze             = 1
    thinSeqZeeStream.StreamZee_HLTStreamTrigNavSlimming.RemoveFeatureless   = 1
    thinSeqZeeStream.StreamZee_HLTStreamTrigNavSlimming.RemoveGhosts        = 1
    thinSeqZeeStream.StreamZee_HLTStreamTrigNavSlimming.WriteTree           = 1
    thinSeqZeeStream.StreamZee_HLTStreamTrigNavSlimming.ChainInclusionList  = [ 'L1_EM14', 'EF_e15_medium', 'EF_e20_loose' ]
    

    # Apply the CaloCalTopoCluster thinning
    from ThinningUtils.ThinningUtilsConf import ThinCaloClusters
    thinSeqZeeStream += ThinCaloClusters( "ThinElectronCaloClustersInZeeStream",
                                          OutputLevel           = INFO,
                                          thinSvc               = 'ThinningSvc/ZeeStreamThinning',
                                          caloClusterCollection = 'CaloCalTopoCluster',
                                          inputCollection       = 'ElectronAODCollection'
                                          )

    # Apply the TrackParticle thinning
    from ThinningUtils.ThinningUtilsConf import ThinTrackParticles
    thinSeqZeeStream += ThinTrackParticles( "ThinElectronTrackParticlesInZeeStream",
                                            OutputLevel             = INFO,
                                            thinSvc                 = 'ThinningSvc/ZeeStreamThinning',
                                            trackParticleCollection = 'TrackParticleCandidate',
                                            inputCollection         = 'ElectronAODCollection'
                                            #inputCollection         = 'CentralLooseElectronLinkCollection'
                                            )
##     thinSeqZeeStream += ThinTrackParticles( "ThinPhotonTrackParticlesInZeeStream",
##                                             OutputLevel             = INFO,
##                                             thinSvc                 = 'ThinningSvc/ZeeStreamThinning',
##                                             trackParticleCollection = 'TrackParticleCandidate',
##                                             inputCollection         = 'PhotonAODCollection'
##                                             )
    pass


if D2PDFlags.WriteD2AOD_ZEEStream.ApplySlimming :
    from PrimaryDPDMaker.PrimaryDPDMakerConf import SlimTrackInfo
    thinSeqZeeStream += SlimTrackInfo( "SlimTrackParticlesInZeeStream",
                                       thinSvc             = 'ThinningSvc/ZeeStreamThinning',
                                       TrackPartContName   = 'TrackParticleCandidate'
                                       )
    pass




#====================================================================
# Define the test DPD output stream
#====================================================================
ZeeStream = MSMgr.NewPoolStream( streamName, fileName )

# Only events that pass the filters listed below are written out
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
ZeeStream.AcceptAlgs( [ "ElectronLooseSelectorInZeeStream"
                        #"StreamZeeFilterCombiner_MediumElectronPlusDiElectron",
                        #"StreamZeeFilterCombiner_TightElectronPlusDiCluster"
                        ] )

# Require that the events pass the wanted trigger
#ZeeStream.RequireAlgs( [ "TriggerFilterInZeeStream_L1_EM14" ] )

# Veto the events selected by the jet/missingEt event cleaning
#ZeeStream.VetoAlgs( [ "JetMetCleaningSelectorInZeeStream" ] )

# Also record the MetaData for all other algorithms
ZeeStream.AddOtherAlgsToBookkeep( algsToBookkeep )




#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take all items from the input, except for the ones listed in the excludeList
# If the excludeList is empty, all containers from the input file (e.g. AOD)
# are copied to the output file.
excludeList = [ "TrigMuonEFContainer#HLT_MuonEF" ]
excludeList = list(set(excludeList)) # This removes dublicates from the list
#dpdOutput.addAllItemsFromInputExceptExcludeList( streamName, excludeList )

# Just copy a few containers by hand. This way is rather dangerous because
# if you have a typo in the string or if a container name in the AOD changes,
# you will loose this one in the output container!
ZeeStream.AddItem( ['EventInfo#*'] )
ZeeStream.AddItem( ["egammaContainer#ElectronAODCollection"] )
ZeeStream.AddItem( ["egammaContainer#PhotonAODCollection"] )
ZeeStream.AddItem( ["egDetailContainer#egDetailAOD"] )
ZeeStream.AddItem( ["egDetailContainer#egDetailContainer"] )
ZeeStream.AddItem( ["egDetailContainer#SofteDetailContainer"] )
ZeeStream.AddItem( ["CaloCompactCellContainer#AODCellContainer"] )
ZeeStream.AddItem( ["CaloClusterContainer#egClusterCollection"] )
ZeeStream.AddItem( ["CaloCellLinkContainer#egClusterCollection_Link"] )
ZeeStream.AddItem( ["Analysis::MuonContainer#StacoMuonCollection"] )
ZeeStream.AddItem( ["Analysis::MuonContainer#MuidMuonCollection"] )
ZeeStream.AddItem( ["Analysis::TauJetContainer#TauRecContainer"] )
ZeeStream.AddItem( ["JetCollection#AntiKt4TopoEMJets"] )
#ZeeStream.AddItem( ["JetMomentMap#AntiKt4TopoEMJetsMomentMap"] )
#ZeeStream.AddItem( ["JetCollection#AntiKt6TopoEMJets"] )
#ZeeStream.AddItem( ["JetMomentMap#AntiKt6TopoEMJetsMomentMap"] )
#ZeeStream.AddItem( ["JetCollection#AntiKt6LCTopoJets"] )
#ZeeStream.AddItem( ["JetKeyDescriptor#JetKeyMap"] )
ZeeStream.AddItem( ["Rec::TrackParticleContainer#TrackParticleCandidate"] )
ZeeStream.AddItem( ["CaloClusterContainer#CaloCalTopoCluster"] )
ZeeStream.AddItem( ["VxContainer#VxPrimaryCandidate"] )
#ZeeStream.AddItem( ["MissingET#*"] )
ZeeStream.AddItem( ["MissingET#MET_RefFinal"] )
ZeeStream.AddItem( ["MissingET#MET_LocHadTopo"] )
ZeeStream.AddItem( ["MissingET#MET_MuonBoy"] )
ZeeStream.AddItem( ["MissingET#MET_RefMuon_Track"] )
ZeeStream.AddItem( ["LArNoisyROSummary#LArNoisyROSummary"] )
#ZeeStream.AddItem( ["SkimDecisionCollection#*"] )
ZeeStream.AddItem( ["SkimDecisionCollection#DAOD_ELLOOSE18_SkimDecisionsContainer"] )
ZeeStream.AddItem( ["SkimDecisionCollection#StreamD2AODM_ZEE_SkimDecisionsContainer"] )

# Add the needed trigger containers
#ZeeStream.AddItem( ["HLT::HLTResult#HLTResult_L2"] )
ZeeStream.AddItem( ["HLT::HLTResult#HLTResult_EF"] )
ZeeStream.AddItem( ["LVL1_ROI#LVL1_ROI"] )
ZeeStream.AddItem( ["TrigDec::TrigDecision#TrigDecision"] )
#ZeeStream.AddItem( ["CTP_Decision#CTP_Decision"] )
## ZeeStream.AddItem( ["TrigRoiDescriptorCollection#HLT"] )
## ZeeStream.AddItem( ["TrigRoiDescriptorCollection#HLT_TrigT2CaloEgamma"] )
## ZeeStream.AddItem( ["TrigRoiDescriptorCollection#HLT_forID"] )
## ZeeStream.AddItem( ["TrigRoiDescriptorCollection#HLT_initialRoI"] )
## ZeeStream.AddItem( ["TrigRoiDescriptorCollection#HLT_secondaryRoI_L2"] )
## ZeeStream.AddItem( ["TrigRoiDescriptorCollection#HLT_secondaryRoI_EF"] )
## ZeeStream.AddItem( ["CaloCellContainer#HLT_TrigT2CaloEgammaCells"] )
## ZeeStream.AddItem( ["egammaContainer#HLT_egamma"] )
## ZeeStream.AddItem( ["egammaContainer#HLT_egamma_Electrons"] )
## ZeeStream.AddItem( ["egammaContainer#HLT_egamma_Photons"] )
## ZeeStream.AddItem( ["CaloClusterContainer#HLT"] )
## ZeeStream.AddItem( ["CaloClusterContainer#HLT_TrigCaloClusterMaker"] )
## ZeeStream.AddItem( ["CaloClusterContainer#HLT_TrigCaloClusterMaker_slw"] )
## ZeeStream.AddItem( ["TrigElectronContainer#HLT_L2ElectronFex"] )
## ZeeStream.AddItem( ["TrigElectronContainer#HLT_L2IDCaloFex"] )
## ZeeStream.AddItem( ["TrigInDetTrackCollection#HLT"] )
## ZeeStream.AddItem( ["TrigInDetTrackCollection#HLT_TrigIDSCAN_FullScan"] )
## ZeeStream.AddItem( ["TrigInDetTrackCollection#HLT_TrigIDSCAN_eGamma"] )
## ZeeStream.AddItem( ["TrigInDetTrackCollection#HLT_TrigIDSCAN_eGamma_Brem"] )
## ZeeStream.AddItem( ["TrigInDetTrackCollection#HLT_TrigSiTrack_FullScan"] )
## ZeeStream.AddItem( ["TrigInDetTrackCollection#HLT_TrigSiTrack_eGamma"] )
## ZeeStream.AddItem( ["TrigInDetTrackCollection#HLT_TrigSiTrack_eGamma_robust"] )
## ZeeStream.AddItem( ["egDetailContainer#HLT_egamma"] )
## ZeeStream.AddItem( ["egDetailContainer#HLT_egamma_Electrons"] )
## ZeeStream.AddItem( ["egDetailContainer#HLT_egamma_Photons"] )
## ZeeStream.AddItem( ["TrigPhotonContainer#HLT_L2PhotonFex"] )
## ZeeStream.AddItem( ["TrigEMClusterContainer#HLT"] )
## ZeeStream.AddItem( ["TrigEMClusterContainer#HLT_T2CaloSwCluster"] )
## ZeeStream.AddItem( ["TrigEMClusterContainer#HLT_TrigT2CaloEgamma"] )
## ZeeStream.AddItem( ["TrigEMClusterContainer#HLT_TrigcaloSwCluster"] )
## ZeeStream.AddItem( ["Rec::TrackParticleContainer#HLT_InDetTrigParticleCreationCombined_Electron_EFID"] )
## ZeeStream.AddItem( ["Rec::TrackParticleContainer#HLT_InDetTrigParticleCreationTRTOnly_Electron_EFID"] )
## ZeeStream.AddItem( ["Rec::TrackParticleContainer#HLT_InDetTrigParticleCreationTRTOnly_FullScan_EFID"] )
## ZeeStream.AddItem( ["Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Electron_EFID"] )
## ZeeStream.AddItem( ["Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_FullScan_EFID"] )
## ZeeStream.AddItem( ["Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Photon_EFID"] )

# Add the Monte Carlo Truth
ZeeStream.AddItem( ["TruthParticleContainer#SpclMC"] )
ZeeStream.AddItem( ["McEventCollection#GEN_AOD"] )
ZeeStream.AddItem( ["TrackParticleTruthCollection#TrackParticleTruthCollection"] )


# You need to add your newly created output containers from above to the output stream
ZeeStream.AddItem( ['CompositeParticleContainer#*'] )
ZeeStream.AddItem( ['INav4MomLinkContainer#*'] )
ZeeStream.AddItem( ['INav4MomAssocs#*'] )




#====================================================================
# Define the THistSvc
#====================================================================
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc( "ZeeStreamTHistSvc",
                        Output = [ "%s DATAFILE='%s' OPT='SHARE'" % ( streamName, fileName ) ]
                        )



#====================================================================
# Define the ThinningSvc
#====================================================================
if D2PDFlags.WriteD2AOD_ZEEStream.ApplyThinning or D2PDFlags.WriteD2AOD_ZEEStream.ApplySlimming :
    from AthenaServices.Configurables import ThinningSvc, createThinningSvc
    augStream = MSMgr.GetStream( streamName )
    evtStream = augStream.GetEventStream()
    svcMgr += createThinningSvc( svcName="ZeeStreamThinning", outStreams=[evtStream] )
    pass




# Final message to log
zeeDPD_msg.info( 'Finished configuration of %s' % D2PDFlags.WriteD2AOD_ZEEStream.StreamName )



