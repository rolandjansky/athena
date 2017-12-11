##-----------------------------------------------------------------------------
## Name: D2PD_Wenu.py
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
wenuDPD_msg = logging.getLogger( 'D2PD_WENU' )
wenuDPD_msg.info( 'Starting configuration of %s' % D2PDFlags.WriteD2AOD_WENUStream.StreamName )

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

## Include the job property flags for this package and from RecExCommon
from D2PDMaker.D2PDFlags import D2PDFlags
from RecExConfig.RecFlags import rec

## This handels multiple output streams
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
        wenuDPD_msg.error( "No MCEventCollection found even though the input file is simulation!" )
        wenuDPD_msg.error( "These are the McEventCollections from the input file: %s" % mcEventColls )
        pass
    pass





#====================================================================
# Do the good run list (GRL) selection --- only for data
#====================================================================
cmdString_GRL = ""
if not inputIsSimulation and False:
    # use closest DB replica
    from PoolSvc.PoolSvcConf import PoolSvc
    svcMgr += PoolSvc(SortReplicas=True)
    from DBReplicaSvc.DBReplicaSvcConf import DBReplicaSvc
    svcMgr += DBReplicaSvc(UseCOOLSQLite=False)

    # Try to copy the GRL locally
    import socket
    socket.setdefaulttimeout(180)
    import urllib
    remoteGRL = D2PDFlags.GRLFile()
    localGRL  = "data10_7TeV.pro05.merged_LBSUMM_WZjets_allchannels.xml"
    urllib.urlretrieve( remoteGRL, localGRL )

    # Create an instance of the GoodRunsListSelectorTool
    from GoodRunsLists.GoodRunsListsConf import *
    ToolSvc += GoodRunsListSelectorTool( "GoodRunsListSelectorTool",
                                         OutputLevel     = INFO,
                                         GoodRunsListVec = [ 'data10_7TeV.pro05.merged_LBSUMM_WZjets_allchannels.xml' ], ## specify your grl here
                                         PassThrough     = False
                                         )

    # Use the GRLTriggerSelectorAlg
    from GoodRunsListsUser.GoodRunsListsUserConf import *
    topSequence += GRLTriggerSelectorAlg( "GRLFilterInWenuStream",
                                          OutputLevel       = INFO,
                                          GoodRunsListArray = [ 'WZjets_allchannels' ] ## GRL name from the above xml-file
                                          )

    #theApp.getHandle().Dlls += ['XMLCoreParser'] ## Hack to avoid library mismatch after importing the GRL stuff

    algsToBookkeep.append( "GRLFilterInWenuStream" )
    #--------------------------------------------------------------------
    # Set up the cumulative cut-flow for the standard pre-selection.
    # This will use the Logical Filter Combiners to add individual cuts.
    #--------------------------------------------------------------------
    cmdString_GRL += "GRLFilterInWenuStream and "
    pass # End: if not inputIsSimulation






#====================================================================
# Do the PrimaryVertex selection
#====================================================================
from D2PDMaker.D2PDMakerConf import D2PDVertexSelector
topSequence += D2PDVertexSelector( "VertexSelectorInWenuStream",
                                   OutputLevel          = INFO,
                                   inputCollection      = 'VxPrimaryCandidate',
                                   minNumberVxTracks    = 3
                                   )
algsToBookkeep.append( "VertexSelectorInWenuStream" )

#--------------------------------------------------------------------
# Set up the cumulative cut-flow for the standard pre-selection.
# This will use the Logical Filter Combiners to add individual cuts.
#--------------------------------------------------------------------
cmdString_GRL_PrimVtx = cmdString_GRL + "VertexSelectorInWenuStream"
topSequence += LogicalFilterCombiner( "StreamWenuFilterCombiner_GRL_PrimVtx",
                                      cmdstring = cmdString_GRL_PrimVtx )
algsToBookkeep.append( "StreamWenuFilterCombiner_GRL_PrimVtx" )






#====================================================================
# Do the trigger selection
#====================================================================
include("PrimaryDPDMaker/SetupTrigDecisionTool.py")
from PrimaryDPDMaker.TriggerFilter import TriggerFilter
topSequence += TriggerFilter( "TriggerFilterInWenuStream_L1_EM10",
                              trigger = "L1_EM10"
                              )
algsToBookkeep.append( "TriggerFilterInWenuStream_L1_EM10" )

#--------------------------------------------------------------------
# Set up the cumulative cut-flow for the standard pre-selection.
# This will use the Logical Filter Combiners to add individual cuts.
#--------------------------------------------------------------------
cmdString_GRL_PrimVtx_Trigger = cmdString_GRL_PrimVtx + " and TriggerFilterInWenuStream_L1_EM10"
topSequence += LogicalFilterCombiner( "StreamWenuFilterCombiner_GRL_PrimVtx_Trigger",
                                      cmdstring = cmdString_GRL_PrimVtx_Trigger )
algsToBookkeep.append( "StreamWenuFilterCombiner_GRL_PrimVtx_Trigger" )

# Add other triggers for long term
# EF_g17_etcut
topSequence += TriggerFilter( "TriggerFilterInZeeStream_EF_g17_etcut",
                              trigger = "EF_g17_etcut"
                              )
algsToBookkeep.append( "TriggerFilterInZeeStream_EF_g17_etcut" )

# EF_g15_loose
topSequence += TriggerFilter( "TriggerFilterInZeeStream_EF_g15_loose",
                              trigger = "EF_g15_loose"
                              )
algsToBookkeep.append( "TriggerFilterInZeeStream_EF_g15_loose" )

# EF_g15_medium
topSequence += TriggerFilter( "TriggerFilterInZeeStream_EF_g15_medium",
                              trigger = "EF_g15_medium"
                              )
algsToBookkeep.append( "TriggerFilterInZeeStream_EF_g15_medium" )

# EF_e10_loose
topSequence += TriggerFilter( "TriggerFilterInZeeStream_EF_e10_loose",
                              trigger = "EF_e10_loose"
                              )
algsToBookkeep.append( "TriggerFilterInZeeStream_EF_e10_loose" )

# EF_e15_loose
topSequence += TriggerFilter( "TriggerFilterInZeeStream_EF_e15_loose",
                              trigger = "EF_e15_loose"
                              )
algsToBookkeep.append( "TriggerFilterInZeeStream_EF_e15_loose" )

# EF_e20_loose
topSequence += TriggerFilter( "TriggerFilterInZeeStream_EF_e20_loose",
                              trigger = "EF_e20_loose"
                              )
algsToBookkeep.append( "TriggerFilterInZeeStream_EF_e20_loose" )







#====================================================================
# Do the jet/missingEt event cleaning
#====================================================================
# First, create an instance of the tool that will calculate the good,
# the bad, and the ugly for each jet and store the result in UserData.
from UserDataUtils.UserDataUtilsConf import UserDataCalcJetQualityTool
ToolSvc += UserDataCalcJetQualityTool( "UserDataCalcJetQualityToolInWenuStream",
                                       OutputLevel            = INFO,
                                       userDataPrefix         = "jet_",
                                       userDataJetQualityName = "isGoodUglyBad"
                                       )

# Now, use the JetSelector and use the UserDataCalcJetQualityTool with it
from D2PDMaker.D2PDMakerConf import D2PDJetSelector
topSequence += D2PDJetSelector( "JetMetCleaningSelectorInWenuStream",
                                OutputLevel          = INFO,
                                inputCollection      = 'AntiKt4TopoJets',
                                outputLinkCollection = 'WenuStreamEMScalePt10BadJetCollection',
                                #userDataCalcToolList = [ ToolSvc.UserDataCalcJetQualityToolInWenuStream ],
                                minNumberPassed      = 1,
                                useJetSignalState    = PyAthena.P4SignalState.JETEMSCALE,
                                jetSignalStatePtMin  = 10.0*Units.GeV,
                                #userDataNameList     = [ "jet_isGoodUglyBad" ],
                                #userDataMinCutList   = [ 3.5 ] # good=1, ugly=2, bad=4 (combinations add up)
                                )
algsToBookkeep.append( "JetMetCleaningSelectorInWenuStream" )

#--------------------------------------------------------------------
# Set up the cumulative cut-flow for the standard pre-selection.
# This will use the Logical Filter Combiners to add individual cuts.
#--------------------------------------------------------------------
cmdString_GRL_PrimVtx_Trigger_JetMetClean = cmdString_GRL_PrimVtx_Trigger + " and not JetMetCleaningSelectorInWenuStream"
topSequence += LogicalFilterCombiner( "StreamWenuFilterCombiner_GRL_PrimVtx_Trigger_JetMetClean",
                                      cmdstring = cmdString_GRL_PrimVtx_Trigger_JetMetClean )
algsToBookkeep.append( "StreamWenuFilterCombiner_GRL_PrimVtx_Trigger_JetMetClean" )





#====================================================================
# Do the Monte Carlo Truth selections, if the input is a simulation file
#====================================================================
if inputIsSimulation :
    # Truth pre-selector (this way, only one loop over the SpclMC container is needed per event -> faster)
    from D2PDMaker.D2PDMakerConf import D2PDTruthParticleSelector
    topSequence += D2PDTruthParticleSelector( "TruthPreSelectorInWenuStream",
                                              OutputLevel             = INFO,
                                              inputCollection         = 'SpclMC',
                                              outputLinkCollection    = 'WenuStreamTruthParticlePreSelectedLinkCollection',
                                              minNumberPassed         = 1,
                                              pdgIDList               = [ 4, 5, 6, 11, 12, 13, 14, 15, 16, 22, 23, 24, 25 ], # c, b, t, e, nu_e, mu, nu_mu, tau, nu_tau, gamma, Z, W, H
                                              removeDocumentationLine = False,
                                              keepOnlyGenerator       = True,
                                              keepOnlyStable          = False,
                                              removeSelfDecay         = True
                                              )
    algsToBookkeep.append( "TruthPreSelectorInWenuStream" )

    # Truth-electron selector:
    topSequence += D2PDTruthParticleSelector( "TruthElectronSelectorInWenuStream",
                                              OutputLevel             = INFO,
                                              inputCollection         = 'TruthParticlePreSelectedLinkCollection',
                                              outputLinkCollection    = 'WenuStreamTruthElectronFromWBosonLinkCollection',
                                              minNumberPassed         = 2,
                                              pdgIDList               = [ 11 ], # e-, e+
                                              pdgIDListOrigin         = [ 24 ], # W boson
                                              removeDocumentationLine = False,
                                              keepOnlyGenerator       = True,
                                              keepOnlyStable          = False,
                                              removeSelfDecay         = True
                                              )
    algsToBookkeep.append( "TruthElectronSelectorInWenuStream" )

    # Truth-electron selector for status code == 1:
    topSequence += D2PDTruthParticleSelector( "TruthElectronStatusCode1SelectorInWenuStream",
                                              OutputLevel          = INFO,
                                              inputCollection      = 'TruthElectronFromWBosonLinkCollection',
                                              outputLinkCollection = 'WenuStreamTruthElectronStatusCode1FromWBosonLinkCollection',
                                              minNumberPassed      = 2,
                                              acceptStatusCodeList = [ 1 ]
                                              )
    algsToBookkeep.append( "TruthElectronStatusCode1SelectorInWenuStream" )

    # Truth-electron selector for status code == 3:
    topSequence += D2PDTruthParticleSelector( "TruthElectronStatusCode3SelectorInWenuStream",
                                              OutputLevel          = INFO,
                                              inputCollection      = 'TruthElectronFromWBosonLinkCollection',
                                              outputLinkCollection = 'WenuStreamTruthElectronStatusCode3FromWBosonLinkCollection',
                                              minNumberPassed      = 2,
                                              acceptStatusCodeList = [ 3 ]
                                              )
    algsToBookkeep.append( "TruthElectronStatusCode3SelectorInWenuStream" )

    # Truth-photon selector:
    topSequence += D2PDTruthParticleSelector( "TruthPhotonSelectorInWenuStream",
                                              OutputLevel             = INFO,
                                              inputCollection         = 'TruthParticlePreSelectedLinkCollection',
                                              outputLinkCollection    = 'WenuStreamTruthPhotonFromFSRLinkCollection',
                                              minNumberPassed         = 1,
                                              pdgIDList               = [ 22 ], # gamma
                                              pdgIDListOrigin         = [ 11, 23, 24 ], # e-, e+, Z, W boson
                                              removeDocumentationLine = True,
                                              keepOnlyGenerator       = True,
                                              keepOnlyStable          = True,
                                              removeSelfDecay         = True,
                                              etaMax                  = 5.1
                                              )
    algsToBookkeep.append( "TruthPhotonSelectorInWenuStream" )

    # Truth Z0 selector:
    topSequence += D2PDTruthParticleSelector( "TruthZBosonSelectorInZeeStream",
                                              OutputLevel             = INFO,
                                              inputCollection         = 'TruthParticlePreSelectedLinkCollection',
                                              outputLinkCollection    = 'WenuStreamTruthZBosonLinkCollection',
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
                                              outputLinkCollection    = 'WenuStreamTruthWBosonLinkCollection',
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
    topSequence += D2PDParticleCombiner( "TruthElectronTruthPhotonCombinerInWenuStream",
                                         OutputLevel             = INFO,
                                         compositeParticlePDG_ID = 11, # This is an electron
                                         inputCollectionList     = [ 'TruthElectronStatusCode1FromWBosonLinkCollection',
                                                                     'TruthPhotonFromFSRLinkCollection' ],
                                         outputCollection        = 'WenuStreamCombinedTruthElectronTruthPhotonCollection',
                                         deltaRMax               = 0.05
                                         )
    algsToBookkeep.append( "TruthElectronTruthPhotonCombinerInWenuStream" )


    # Truth matching/classification using the MCTruthClassifier
    # First, set up the standard MCTruthClassifier
    from MCTruthClassifier.MCTruthClassifierBase import MCTruthClassifier as ElectronMCTruthClassifierInWenuStream
    ElectronMCTruthClassifierInWenuStream.McEventCollection = mcEventCollectionForMCTruthClassifier

    # Now, use the MCTruthClassifier for the association/matching tool
    from AssociationComps.AssociationCompsConf import MCTruthClassifierAssociationTool
    ToolSvc += MCTruthClassifierAssociationTool( "ElectronMCTruthClassifierAssociationToolInWenuStream",
                                                 OutputLevel                = INFO,
                                                 mcTruthClassifier          = ElectronMCTruthClassifierInWenuStream,
                                                 inputAssociateToCollection = "SpclMC",
                                                 userDataPrefix             = "el_mcTruthClassifier_",
                                                 writeUserData              = True
                                                 )
    
    # Set up a DeltaR association/matching tool
    from AssociationComps.AssociationCompsConf import DeltaRAssociationTool
    ToolSvc += DeltaRAssociationTool( "ElectronDeltaRTruthAssociationToolInWenuStream",
                                      OutputLevel                = INFO,
                                      inputAssociateToCollection = "TruthElectronFromWBosonLinkCollection",
                                      deltaRMax                  = 0.1,
                                      userDataPrefix             = "el_deltaRMatchTruth_",
                                      writeUserData              = False
                                      )

    # Now, do a match to status code 1 electrons
    ToolSvc += DeltaRAssociationTool( "ElectronDeltaRTruthSC1AssociationToolInWenuStream",
                                      OutputLevel                = INFO,
                                      inputAssociateToCollection = "TruthElectronStatusCode1FromWBosonLinkCollection",
                                      deltaRMax                  = 0.1,
                                      userDataPrefix             = "el_deltaRMatchTruth_sc1_",
                                      writeUserData              = False
                                      )

    # Now, do a match to status code 3 electrons
    ToolSvc += DeltaRAssociationTool( "ElectronDeltaRTruthSC3AssociationToolInWenuStream",
                                      OutputLevel                = INFO,
                                      inputAssociateToCollection = "TruthElectronStatusCode3FromWBosonLinkCollection",
                                      deltaRMax                  = 0.1,
                                      userDataPrefix             = "el_deltaRMatchTruth_sc3_",
                                      writeUserData              = False
                                      )

    
    pass # End: if inputIsSimulation







#====================================================================
# Define the individual missing et selection
#====================================================================
from D2PDMaker.D2PDMakerConf import D2PDMissingEtSelector
# MET_RefFinal selection
topSequence += D2PDMissingEtSelector( "MissingEtRefFinalSelectorInWenuStream",
                                      OutputLevel      = DEBUG,
                                      inputCollection  = "MET_RefFinal",
                                      outputCollection = "WenuStreamNeutrinoRefFinal",
                                      missingEtMin     = 20.0*Units.GeV,
                                      minNumberPassed  = 1
                                      )
algsToBookkeep.append( "MissingEtRefFinalSelectorInWenuStream" )


# MET_LocHadTopoObj selection
topSequence += D2PDMissingEtSelector( "MissingEtLocHadTopoObjSelectorInWenuStream",
                                      OutputLevel      = DEBUG,
                                      inputCollection  = "MET_LocHadTopoObj",
                                      outputCollection = "WenuStreamNeutrinoLocHadTopoObj",
                                      missingEtMin     = 20.0*Units.GeV,
                                      minNumberPassed  = 1
                                      )
algsToBookkeep.append( "MissingEtLocHadTopoObjSelectorInWenuStream" )




#====================================================================
# Define the individual central electron selections
#====================================================================
# Load the actual filter
from D2PDMaker.D2PDMakerConf import D2PDElectronSelector
# Create an instance of this filter and add it to the AlgSequence
# Also, configure the filter to your needs. This are just some examples
topSequence += D2PDElectronSelector( "ElectronLooseSelectorInWenuStream",
                                     OutputLevel          = INFO,
                                     inputCollection      = 'ElectronAODCollection',
                                     outputLinkCollection = 'WenuStreamCentralLooseElectronLinkCollection',
                                     minNumberPassed      = 1,
                                     electronIsEM         = egammaPID.ElectronLoose,
                                     electronAuthor       = egammaParameters.AuthorElectron,
                                     clusterEtMin         = 20.0*Units.GeV
                                     )
algsToBookkeep.append( "ElectronLooseSelectorInWenuStream" )

#--------------------------------------------------------------------
# If the input is Monte Carlo, also apply truth matching.
# This will be done for ALL electrons in the ElectronAODCollection
# and not only for the ones surviving the selection cuts!
#--------------------------------------------------------------------
if inputIsSimulation :
    # Schedule the associations with the electron selector, i.e., performed for all electrons in the ElectronAODCollection
    topSequence.ElectronLooseSelectorInWenuStream.associationToolList = [ ToolSvc.ElectronMCTruthClassifierAssociationToolInWenuStream,
                                                                         ToolSvc.ElectronDeltaRTruthAssociationToolInWenuStream,
                                                                         ToolSvc.ElectronDeltaRTruthSC1AssociationToolInWenuStream,
                                                                         ToolSvc.ElectronDeltaRTruthSC3AssociationToolInWenuStream ]
    topSequence.ElectronLooseSelectorInWenuStream.outputAssociationContainerList = [ "WenuStreamElectronMCTruthClassifierMatch",
                                                                                     "WenuStreamElectronDeltaRTruthMatch",
                                                                                     "WenuStreamElectronDeltaRTruthStatusCode1Match",
                                                                                     "WenuStreamElectronDeltaRTruthStatusCode3Match" ]
    
    pass # End: if inputIsSimulation


# Define a medium electron selection
topSequence += D2PDElectronSelector( "ElectronMediumSelectorInWenuStream",
                                     OutputLevel          = INFO,
                                     inputCollection      = 'ElectronAODCollection',
                                     outputLinkCollection = 'WenuStreamCentralMediumElectronLinkCollection',
                                     minNumberPassed      = 1,
                                     electronIsEM         = egammaPID.ElectronMedium,
                                     electronAuthor       = egammaParameters.AuthorElectron,
                                     clusterEtMin         = 15.0*Units.GeV
                                     )
algsToBookkeep.append( "ElectronMediumSelectorInWenuStream" )






#--------------------------------------------------------------------
# Build the W candidates
#--------------------------------------------------------------------
# Use MET_LocHadTopoObj
# - the W->mu nu Boson
from D2PDMaker.D2PDMakerConf import D2PDLeptonNeutrinoCombiner
topSequence += D2PDLeptonNeutrinoCombiner( "WenuMediumLocHadTopoObjCombinerInWenuStream",
                                           OutputLevel                  = INFO,
                                           compositeParticlePDG_ID      = 24, # This is a W Boson
                                           inputCollection              = 'MET_LocHadTopoObj',
                                           inputLeptonCollection        = 'WenuStreamCentralMediumElectronLinkCollection',
                                           outputNeutrinoCollection     = 'WenuMediumLocHadTopoObjNeutrinoCollection',
                                           outputNeutrinoLinkCollection = 'WenuMediumLocHadTopoObjNeutrinoLinkCollection',
                                           outputCollection             = 'WenuMediumLocHadTopoObjWBosonCollection',
                                           minNumberPassed              = 1,
                                           missingEtMin                 = 20.0*Units.GeV
                                           )
algsToBookkeep.append( "WenuMediumLocHadTopoObjCombinerInWenuStream" )

	

# Wenu plus invarian mass cut
from D2PDMaker.D2PDMakerConf import D2PDINavigable4MomentumSelector
topSequence += D2PDINavigable4MomentumSelector( "WenuMediumLocHadTopoObjInvMassCombinerInWenuStream",
                                                OutputLevel          = INFO,
                                                inputCollection      = 'WenuMediumLocHadTopoObjWBosonCollection',
                                                outputLinkCollection = 'WenuMediumLocHadTopoObjInvMassWBosonCollection',
                                                minNumberPassed      = 1,
                                                massMin              = 40.0*Units.GeV,
                                                )
algsToBookkeep.append( "WenuMediumLocHadTopoObjInvMassCombinerInWenuStream" )



# ---------------------------
# Use MET_RefFinal
# - the W->mu nu Boson
from D2PDMaker.D2PDMakerConf import D2PDLeptonNeutrinoCombiner
topSequence += D2PDLeptonNeutrinoCombiner( "WenuMediumRefFinalCombinerInWenuStream",
                                           OutputLevel                  = INFO,
                                           compositeParticlePDG_ID      = 24, # This is a W Boson
                                           inputCollection              = 'MET_RefFinal',
                                           inputLeptonCollection        = 'WenuStreamCentralMediumElectronLinkCollection',
                                           outputNeutrinoCollection     = 'WenuMediumRefFinalNeutrinoCollection',
                                           outputNeutrinoLinkCollection = 'WenuMediumRefFinalNeutrinoLinkCollection',
                                           outputCollection             = 'WenuMediumRefFinalWBosonCollection',
                                           minNumberPassed              = 1,
                                           missingEtMin                 = 20.0*Units.GeV
                                           )
algsToBookkeep.append( "WenuMediumRefFinalCombinerInWenuStream" )

	

# Wenu plus invarian mass cut
from D2PDMaker.D2PDMakerConf import D2PDINavigable4MomentumSelector
topSequence += D2PDINavigable4MomentumSelector( "WenuMediumRefFinalInvMassCombinerInWenuStream",
                                                OutputLevel          = INFO,
                                                inputCollection      = 'WenuMediumRefFinalWBosonCollection',
                                                outputLinkCollection = 'WenuMediumRefFinalInvMassWBosonCollection',
                                                minNumberPassed      = 1,
                                                massMin              = 40.0*Units.GeV,
                                                )
algsToBookkeep.append( "WenuMediumRefFinalInvMassCombinerInWenuStream" )










#====================================================================
# Build logical combination of filter decisions
#====================================================================
from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner
topSequence += LogicalFilterCombiner( "LooseElectronMetRefFinal_AcceptEvent",
                                      cmdstring = "ElectronLooseSelectorInWenuStream and MissingEtRefFinalSelectorInWenuStream"
                                      )
algsToBookkeep.append( "LooseElectronMetRefFinal_AcceptEvent" )


topSequence += LogicalFilterCombiner( "LooseElectronLocHadTopoObj_AcceptEvent",
                                      cmdstring = "ElectronLooseSelectorInWenuStream and MissingEtLocHadTopoObjSelectorInWenuStream"
                                      )
algsToBookkeep.append( "LooseElectronLocHadTopoObj_AcceptEvent" )





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
streamName = "StreamD2" + inputFileStreamType + "M_WENU"
print "Configuring the output stream with the name =", streamName
fileName  = D2PDFlags.OutputDirectoryName() + "MyD2PDWenuStream.pool.root"
WenuStream = MSMgr.NewPoolStream( streamName, fileName )

# Only events that pass the filters listed below are written out
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
WenuStream.AcceptAlgs( [ "LooseElectronMetRefFinal_AcceptEvent",
                         "LooseElectronLocHadTopoObj_AcceptEvent" ] )

# Require that the events pass the wanted trigger
#WenuStream.RequireAlgs( [ "TriggerFilterInWenuStream_L1_EM10" ] )

# Veto the events selected by the jet/missingEt event cleaning
#WenuStream.VetoAlgs( [ "JetMetCleaningSelectorInWenuStream" ] )

# Also record the MetaData for all other algorithms
WenuStream.AddOtherAlgsToBookkeep( algsToBookkeep )


#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take all items from the input, except for the ones listed in the excludeList
# If the excludeList is empty, all containers from the input file (e.g. AOD)
# are copied to the output file.
excludeList = [ "TrigMuonEFContainer#HLT_MuonEF" ]
excludeList = list(set(excludeList)) # This removes dublicates from the list
dpdOutput.addAllItemsFromInputExceptExcludeList( streamName, excludeList )

# Just copy a few containers by hand. This way is rather dangerous because
# if you have a typo in the string or if a container name in the AOD changes,
# you will loose this one in the output container!
## WenuStream.AddItem( ['EventInfo#*'] )
## WenuStream.AddItem( ["egammaContainer#ElectronAODCollection"] )
## WenuStream.AddItem( ["egammaContainer#PhotonAODCollection"] )
## WenuStream.AddItem( ["egDetailContainer#egDetailAOD"] )
## WenuStream.AddItem( ["egDetailContainer#egDetailContainer"] )
## WenuStream.AddItem( ["egDetailContainer#SofteDetailContainer"] )
## WenuStream.AddItem( ["CaloCompactCellContainer#AODCellContainer"] )
## WenuStream.AddItem( ["CaloClusterContainer#egClusterCollection"] )
## WenuStream.AddItem( ["CaloCellLinkContainer#egClusterCollection_Link"] )
## WenuStream.AddItem( ["Analysis::MuonContainer#StacoMuonCollection"] )
## WenuStream.AddItem( ["Analysis::MuonContainer#MuidMuonCollection"] )
## WenuStream.AddItem( ["Analysis::TauJetContainer#TauRecContainer"] )
## WenuStream.AddItem( ["JetCollection#Cone4TopoJets"] )
## WenuStream.AddItem( ["JetCollection#AntiKt4TowerJets"] )
## WenuStream.AddItem( ["JetCollection#AntiKt4TopoJets"] )
## WenuStream.AddItem( ["JetCollection#AntiKt6TopoJets"] )
## WenuStream.AddItem( ["JetCollection#AntiKt6LCTopoJets"] )
## WenuStream.AddItem( ["JetKeyDescriptor#JetKeyMap"] )
## WenuStream.AddItem( ["Rec::TrackParticleContainer#TrackParticleCandidate"] )
## WenuStream.AddItem( ["CaloClusterContainer#CaloCalTopoCluster"] )
## WenuStream.AddItem( ["VxContainer#VxPrimaryCandidate"] )
## WenuStream.AddItem( ["MissingET#*"] )
## WenuStream.AddItem( ["LArNoisyROSummary#LArNoisyROSummary"] )
## WenuStream.AddItem( ["SkimDecisionCollection#*"] )

## # Add the needed trigger containers
## WenuStream.AddItem( ["HLT::HLTResult#HLTResult_L2"] )
## WenuStream.AddItem( ["HLT::HLTResult#HLTResult_EF"] )
## WenuStream.AddItem( ["LVL1_ROI#LVL1_ROI"] )
## WenuStream.AddItem( ["TrigDec::TrigDecision#TrigDecision"] )
## WenuStream.AddItem( ["CTP_Decision#CTP_Decision"] )
## WenuStream.AddItem( ["TrigRoiDescriptorCollection#HLT"] )
## WenuStream.AddItem( ["TrigRoiDescriptorCollection#HLT_TrigT2CaloEgamma"] )
## WenuStream.AddItem( ["TrigRoiDescriptorCollection#HLT_forID"] )
## WenuStream.AddItem( ["TrigRoiDescriptorCollection#HLT_initialRoi"] )
## WenuStream.AddItem( ["TrigRoiDescriptorCollection#HLT_secondaryRoI_L2"] )
## WenuStream.AddItem( ["TrigRoiDescriptorCollection#HLT_secondaryRoI_EF"] )
## WenuStream.AddItem( ["CaloCellContainer#HLT_TrigT2CaloEgammaCells"] )
## WenuStream.AddItem( ["egammaContainer#HLT_egamma"] )
## WenuStream.AddItem( ["egammaContainer#HLT_egamma_Electrons"] )
## WenuStream.AddItem( ["egammaContainer#HLT_egamma_Photons"] )
## WenuStream.AddItem( ["CaloClusterContainer#HLT"] )
## WenuStream.AddItem( ["CaloClusterContainer#HLT_TrigCaloClusterMaker"] )
## WenuStream.AddItem( ["CaloClusterContainer#HLT_TrigCaloClusterMaker_slw"] )
## WenuStream.AddItem( ["TrigElectronContainer#HLT_L2ElectronFex"] )
## WenuStream.AddItem( ["TrigElectronContainer#HLT_L2IDCaloFex"] )
## WenuStream.AddItem( ["TrigInDetTrackCollection#HLT"] )
## WenuStream.AddItem( ["TrigInDetTrackCollection#HLT_TrigIDSCAN_FullScan"] )
## WenuStream.AddItem( ["TrigInDetTrackCollection#HLT_TrigIDSCAN_eGamma"] )
## WenuStream.AddItem( ["TrigInDetTrackCollection#HLT_TrigIDSCAN_eGamma_Brem"] )
## WenuStream.AddItem( ["TrigInDetTrackCollection#HLT_TrigSiTrack_FullScan"] )
## WenuStream.AddItem( ["TrigInDetTrackCollection#HLT_TrigSiTrack_eGamma"] )
## WenuStream.AddItem( ["TrigInDetTrackCollection#HLT_TrigSiTrack_eGamma_robust"] )
## WenuStream.AddItem( ["egDetailContainer#HLT_egamma"] )
## WenuStream.AddItem( ["egDetailContainer#HLT_egamma_Electrons"] )
## WenuStream.AddItem( ["egDetailContainer#HLT_egamma_Photons"] )
## WenuStream.AddItem( ["TrigPhotonContainer#HLT_L2PhotonFex"] )
## WenuStream.AddItem( ["TrigEMClusterContainer#HLT"] )
## WenuStream.AddItem( ["TrigEMClusterContainer#HLT_T2CaloSwCluster"] )
## WenuStream.AddItem( ["TrigEMClusterContainer#HLT_TrigT2CaloEgamma"] )
## WenuStream.AddItem( ["TrigEMClusterContainer#HLT_TrigcaloSwCluster"] )
## WenuStream.AddItem( ["Rec::TrackParticleContainer#HLT_InDetTrigParticleCreationCombined_Electron_EFID"] )
## WenuStream.AddItem( ["Rec::TrackParticleContainer#HLT_InDetTrigParticleCreationTRTOnly_Electron_EFID"] )
## WenuStream.AddItem( ["Rec::TrackParticleContainer#HLT_InDetTrigParticleCreationTRTOnly_FullScan_EFID"] )
## WenuStream.AddItem( ["Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Electron_EFID"] )
## WenuStream.AddItem( ["Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_FullScan_EFID"] )
## WenuStream.AddItem( ["Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Photon_EFID"] )

## # Add the Monte Carlo Truth
## WenuStream.AddItem( ["TruthParticleContainer#SpclMC"] )
## WenuStream.AddItem( ["McEventCollection#GEN_AOD"] )
## WenuStream.AddItem( ["TrackParticleTruthCollection#TrackParticleTruthCollection"] )


# You need to add your newly created output containers from above to the output stream
WenuStream.AddItem( ['CompositeParticleContainer#*'] )
WenuStream.AddItem( ['INav4MomLinkContainer#*'] )
WenuStream.AddItem( ['INav4MomAssocs#*'] )



# Final message to log
wenuDPD_msg.info( 'Finished configuration of %s' % D2PDFlags.WriteD2AOD_WENUStream.StreamName )
