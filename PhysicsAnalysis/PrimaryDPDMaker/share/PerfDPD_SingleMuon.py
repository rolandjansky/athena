##-----------------------------------------------------------------------------
## Name: PrimaryDPD_SingleMuonStream.py
##       (was PrimaryDPD_TightMuonStream.py)
##
## Author: David Lopez Mateos (Columbia University/Caltech)
## Email:  David.Lopez@cern.ch
##
## Description: This defines the content of the Tight Muon DPD output stream.
##
##-----------------------------------------------------------------------------

# If this file is already included, don't include it again
include.block("PrimaryDPDMaker/PerfDPD_SingleMuon.py")


## for messaging
from AthenaCommon.Logging import logging
muonDPDStream_msg = logging.getLogger( 'PrimaryDPD_SingleMuonStream' )

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

## Include the job property flags for this package
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from RecExConfig.RecFlags import rec
## Include the job property flags for this package
from PrimaryDPDMaker.PrimaryDPDFlags_SingleMuonStream import primDPDsingleMu

## This handels multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr



##====================================================================
## Write the used options to the log file
##====================================================================
if rec.OutputLevel() <= INFO:
    muonDPDStream_msg.info('Values of all PerfDPD_SingleMuon flags:')
    print primDPDsingleMu
    pass



# ============================================================================
# Check which muon collection are available
# ============================================================================
from RecExConfig.ObjKeyStore import objKeyStore
hasMuons = False
hasStacoMuons = False
hasMuidMuons = False
if objKeyStore.isInInput("Analysis::MuonContainer", "Muons") \
       or objKeyStore['transient'].has_item("Analysis::MuonContainer#Muons"):
    hasMuons = True
    pass
if objKeyStore.isInInput("Analysis::MuonContainer", "StacoMuonCollection") \
       or objKeyStore['transient'].has_item("Analysis::MuonContainer#StacoMuonCollection"):
    hasStacoMuons = True
    pass
if objKeyStore.isInInput("Analysis::MuonContainer", "MuidMuonCollection") \
       or objKeyStore['transient'].has_item("Analysis::MuonContainer#MuidMuonCollection"):
    hasMuidMuons = True
    pass


# ============================================================================
# Create a sub-sequence where all needed algorithms will run in
# ============================================================================
import AthenaCommon.CfgMgr as CfgMgr
desdSingleMuonSequence = CfgMgr.AthSequencer( 'DESD_SGLMU_Sequence',
                                              StopOverride = True )
topSequence += desdSingleMuonSequence


##============================================================================
## Define the skimming (event selection) for the DESD_SGLMU output stream
##============================================================================
include( "MuonIsolationTools/MuonIsolationTool_jobOptions.py" )

from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner
from PrimaryDPDMaker.PrimaryDPDMakerConf   import PrimaryDPDPrescaler
from D2PDMaker.D2PDMakerConf import D2PDElectronSelector
from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
from D2PDMaker.D2PDMakerConf import D2PDTauSelector
from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner
from D2PDMaker.D2PDMakerConf import D2PDMissingEtSelector
from D2PDMaker.D2PDMakerConf import D2PDJetSelector

        
# Define the some lists
commandstring = ""
algsToBookkeep = []



from RecExConfig.ObjKeyStore import objKeyStore
muonDPDStream_msg.debug("Items in objKeyStore[transient]: %s" %  objKeyStore['transient'].list() )
#print "Items in objKeyStore[transient]:", objKeyStore['transient'].list()

# ------------------------
# Create the filters for the Z -> tau tau -> muon had analysis
# ------------------------
if primDPDsingleMu.TauForZtautauFilter() :
    # Create a list of accept algs for the tau filters
    zTauTauAcceptAlgsList = []

    if hasStacoMuons:
        # Create a Staco muon filter
        from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_StacoMuonForZtautauFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'StacoMuonCollection',
                                                    outputLinkCollection              = 'DESD_SGLMUStream_StacoMuonForZtautauLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.TauForZtautauFilter.MuonEt,
                                                    ptMax                             = primDPDsingleMu.TauForZtautauFilter.MuonEtMax,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    muonParametersList                = [ 19, 3 ],
                                                    muonParametersMinList             = [ -100000000.0 , -100000000.0 ],
                                                    muonParametersMaxList             = [ primDPDsingleMu.TauForZtautauFilter.RelativeMuonIso,
                                                                                          primDPDsingleMu.TauForZtautauFilter.RelativeMuonEtIso ],
                                                    muonParametersUseRelativeCutsList = [ True, True ],
                                                    muonRequireIsLoose                = True
                                                    )
        # Set up a DeltaR association/matching tool
        from AssociationComps.AssociationCompsConf import DeltaRAssociationTool
        ToolSvc += DeltaRAssociationTool( "SingleMuonStream_TauStacoMuonDeltaRAssociationTool",
                                          OutputLevel                = INFO,
                                          inputAssociateToCollection = "DESD_SGLMUStream_StacoMuonForZtautauLinkCollection",
                                          deltaRMax                  = 0.4,
                                          writeUserData              = False
                                          )
        # Set up the tau filter
        from D2PDMaker.D2PDMakerConf import D2PDTauSelector
        desdSingleMuonSequence += D2PDTauSelector( "SingleMuonStream_TauForStacoMuonFilter",
                                                   OutputLevel                    = INFO,
                                                   inputCollection                = 'TauRecContainer',
                                                   #outputLinkCollection           = 'DESD_SGLMUStream_TauForStacoMuonLinkCollection',
                                                   minNumberPassed                = 1,
                                                   associationToolList            = [ ToolSvc.SingleMuonStream_TauStacoMuonDeltaRAssociationTool ],
                                                   outputAssociationContainerList = [ "DESD_SGLMUStream_TauToStacoMuonAssociations" ],
                                                   numberOfAssociationsMaxCutList = [ 0 ],
                                                   minNumberTracks                = primDPDsingleMu.TauForZtautauFilter.TauNTrackMin,
                                                   maxNumberTracks                = primDPDsingleMu.TauForZtautauFilter.TauNTrackMax,
                                                   etMin                          = primDPDsingleMu.TauForZtautauFilter.TauEt
                                                   )
        # Staco + Tau
        desdSingleMuonSequence += PrimaryDPDPrescaler( "SingleMuonStream_TauStacoMuonPrescaleFilter",
                                                       OutputLevel = INFO,
                                                       RequireAlgs = [ "SingleMuonStream_StacoMuonForZtautauFilter",
                                                                       "SingleMuonStream_TauForStacoMuonFilter" ],
                                                       Prescale    = 1 )
        zTauTauAcceptAlgsList.append("SingleMuonStream_TauStacoMuonPrescaleFilter")
        pass

    
    # Check if we have the Muons chain collection available or not
    if hasMuons:
        muonDPDStream_msg.debug("Using Muons collection for TauForZtautauFilter")
        
        # Create a Muons muon filter
        from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_MuonsForZtautauFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'Muons',
                                                    outputLinkCollection              = 'DESD_SGLMUStream_MuonsForZtautauLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.TauForZtautauFilter.MuonEt,
                                                    ptMax                             = primDPDsingleMu.TauForZtautauFilter.MuonEtMax,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    muonParametersList                = [ 19, 3 ],
                                                    muonParametersMinList             = [ -100000000.0 , -100000000.0 ],
                                                    muonParametersMaxList             = [ primDPDsingleMu.TauForZtautauFilter.RelativeMuonIso,
                                                                                          primDPDsingleMu.TauForZtautauFilter.RelativeMuonEtIso ],
                                                    muonParametersUseRelativeCutsList = [ True, True ],
                                                    muonRequireIsMedium                = True
                                                    )
        
        # Set up a DeltaR association/matching tool
        from AssociationComps.AssociationCompsConf import DeltaRAssociationTool
        ToolSvc += DeltaRAssociationTool( "SingleMuonStream_TauMuonsDeltaRAssociationTool",
                                          OutputLevel                = INFO,
                                          inputAssociateToCollection = "DESD_SGLMUStream_MuonsForZtautauLinkCollection",
                                          deltaRMax                  = 0.4,
                                          writeUserData              = False
                                          )
        # Set up the tau filter
        from D2PDMaker.D2PDMakerConf import D2PDTauSelector
        desdSingleMuonSequence += D2PDTauSelector( "SingleMuonStream_TauForMuonsFilter",
                                                   OutputLevel                    = INFO,
                                                   inputCollection                = 'TauRecContainer',
                                                   #outputLinkCollection           = 'DESD_SGLMUStream_TauForMuonsLinkCollection',
                                                   minNumberPassed                = 1,
                                                   associationToolList            = [ ToolSvc.SingleMuonStream_TauMuonsDeltaRAssociationTool ],
                                                   outputAssociationContainerList = [ "DESD_SGLMUStream_TauToMuonsAssociations" ],
                                                   numberOfAssociationsMaxCutList = [ 0 ],
                                                   minNumberTracks                = primDPDsingleMu.TauForZtautauFilter.TauNTrackMin,
                                                   maxNumberTracks                = primDPDsingleMu.TauForZtautauFilter.TauNTrackMax,
                                                   etMin                          = primDPDsingleMu.TauForZtautauFilter.TauEt
                                                   )
        # Muons + Tau
        desdSingleMuonSequence += PrimaryDPDPrescaler( "SingleMuonStream_TauMuonsPrescaleFilter",
                                                       OutputLevel = INFO,
                                                       RequireAlgs = [ "SingleMuonStream_MuonsForZtautauFilter",
                                                                       "SingleMuonStream_TauForMuonsFilter" ],
                                                       Prescale    = 1 )
        zTauTauAcceptAlgsList.append("SingleMuonStream_TauMuonsPrescaleFilter")
        pass


    if hasMuidMuons:
        # Create a MuID muon filter
        from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_MuidMuonForZtautauFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'MuidMuonCollection',
                                                    outputLinkCollection              = 'DESD_SGLMULStream_MuidMuonForZtautauLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.TauForZtautauFilter.MuonEt,
                                                    ptMax                             = primDPDsingleMu.TauForZtautauFilter.MuonEtMax,
                                                    #muonParametersList                = [ MuonParameters.ptcone40 ],
                                                    muonParametersList                = [ 19, 3 ],
                                                    muonParametersMinList             = [ -100000000.0, -100000000.0 ],
                                                    muonParametersMaxList             = [ primDPDsingleMu.TauForZtautauFilter.RelativeMuonIso,
                                                                                          primDPDsingleMu.TauForZtautauFilter.RelativeMuonEtIso ],
                                                    muonParametersUseRelativeCutsList = [ True, True ],
                                                    muonRequireIsTight                = True
                                                    )
        # Set up a DeltaR association/matching tool
        from AssociationComps.AssociationCompsConf import DeltaRAssociationTool
        ToolSvc += DeltaRAssociationTool( "SingleMuonStream_TauMuidMuonDeltaRAssociationTool",
                                          OutputLevel                = INFO,
                                          inputAssociateToCollection = "DESD_SGLMULStream_MuidMuonForZtautauLinkCollection",
                                          deltaRMax                  = 0.4,
                                          writeUserData              = False
                                          )
        # Set up the tau filter
        from D2PDMaker.D2PDMakerConf import D2PDTauSelector
        desdSingleMuonSequence += D2PDTauSelector( "SingleMuonStream_TauForMuidMuonFilter",
                                                   OutputLevel                    = INFO,
                                                   inputCollection                = 'TauRecContainer',
                                                   #outputLinkCollection           = 'DESD_SGLMUStream_TauForMuidMuonLinkCollection',
                                                   minNumberPassed                = 1,
                                                   associationToolList            = [ ToolSvc.SingleMuonStream_TauMuidMuonDeltaRAssociationTool ],
                                                   outputAssociationContainerList = [ "DESD_SGLMUStream_TauToMuidMuonAssociations" ],
                                                   numberOfAssociationsMaxCutList = [ 0 ],
                                                   minNumberTracks                = primDPDsingleMu.TauForZtautauFilter.TauNTrackMin,
                                                   maxNumberTracks                = primDPDsingleMu.TauForZtautauFilter.TauNTrackMax,
                                                   etMin                          = primDPDsingleMu.TauForZtautauFilter.TauEt
                                                   )
        # Muid + Tau
        desdSingleMuonSequence += PrimaryDPDPrescaler( "SingleMuonStream_TauMuidMuonPrescaleFilter",
                                                       OutputLevel = INFO,
                                                       RequireAlgs = [ "SingleMuonStream_MuidMuonForZtautauFilter",
                                                                       "SingleMuonStream_TauForMuidMuonFilter" ],
                                                       Prescale    = 1 )
        zTauTauAcceptAlgsList.append("SingleMuonStream_TauMuidMuonPrescaleFilter")
        pass
    
    # Create the combined decision for the Z -> tau tau
    desdSingleMuonSequence += PrimaryDPDPrescaler( "SingleMuonStream_TauMuonPrescaleFilter",
                                                   OutputLevel = INFO,
                                                   AcceptAlgs  = zTauTauAcceptAlgsList,
                                                   Prescale    = primDPDsingleMu.TauForZtautauFilter.Prescale )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("SingleMuonStream_TauMuonPrescaleFilter")
    # If the other tau filter is not scheduled, add only this one. Otherwise, add a combination below
    if not primDPDsingleMu.MuonTauLooseFilter() :
        if commandstring=="":
            commandstring = "SingleMuonStream_TauMuonPrescaleFilter"
            pass
        else:
            commandstring = commandstring + " or SingleMuonStream_TauMuonPrescaleFilter"
            pass
        pass
    pass


# ------------------------
# Create the filters for the Z -> tau tau (loose, ie low pT) -> muon had analysis
# ------------------------
if primDPDsingleMu.MuonTauLooseFilter() :
    # Create a list of accept algs for the tau filters
    tauAcceptAlgsList = []
    
    # Create a Staco muon filter
    if hasStacoMuons:
        from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_StacoMuonForMuonTauLooseFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'StacoMuonCollection',
                                                    outputLinkCollection              = 'DESD_SGLMUStream_StacoMuonForMuonTauLooseLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.MuonTauLooseFilter.MuonEt,
                                                    ptMax                             = primDPDsingleMu.MuonTauLooseFilter.MuonEtMax,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    muonParametersList                = [ 19, 3 ],
                                                    muonParametersMinList             = [ -100000000.0, -100000000.0 ],
                                                    muonParametersMaxList             = [ primDPDsingleMu.MuonTauLooseFilter.RelativeMuonIso,
                                                                                          primDPDsingleMu.MuonTauLooseFilter.RelativeMuonEtIso],
                                                    muonParametersUseRelativeCutsList = [ True, True ],
                                                    muonRequireIsLoose                = True
                                                    )

        # Set up a DeltaR association/matching tool
        from AssociationComps.AssociationCompsConf import DeltaRAssociationTool
        ToolSvc += DeltaRAssociationTool( "SingleMuonStream_TauLooseStacoMuonDeltaRAssociationTool",
                                          OutputLevel                = INFO,
                                          inputAssociateToCollection = "DESD_SGLMUStream_StacoMuonForMuonTauLooseLinkCollection",
                                          deltaRMax                  = 0.4,
                                          writeUserData              = False
                                          )
        
        # Set up the tau filter
        from D2PDMaker.D2PDMakerConf import D2PDTauSelector
        desdSingleMuonSequence += D2PDTauSelector( "SingleMuonStream_TauLooseForStacoMuonFilter",
                                                   OutputLevel                    = INFO,
                                                   inputCollection                = 'TauRecContainer',
                                                   outputLinkCollection           = 'DESD_SGLMUStream_TauLooseForStacoMuonLinkCollection',
                                                   minNumberPassed                = 1,
                                                   associationToolList            = [ ToolSvc.SingleMuonStream_TauLooseStacoMuonDeltaRAssociationTool ],
                                                   outputAssociationContainerList = [ "DESD_SGLMUStream_TauLooseToStacoMuonAssociations" ],
                                                   numberOfAssociationsMaxCutList = [ 0 ],
                                                   minNumberTracks                = primDPDsingleMu.MuonTauLooseFilter.TauNTrackMin,
                                                   maxNumberTracks                = primDPDsingleMu.MuonTauLooseFilter.TauNTrackMax,
                                                   etMin                          = primDPDsingleMu.MuonTauLooseFilter.TauEt
                                                   #etMax                          = primDPDsingleMu.MuonTauLooseFilter.TauEtBreak
                                                   )
    
        desdSingleMuonSequence += PrimaryDPDPrescaler( "SingleMuonStream_TauLooseStacoMuonPrescaleFilter",
                                                       OutputLevel = INFO,
                                                       RequireAlgs = [ "SingleMuonStream_StacoMuonForMuonTauLooseFilter",
                                                                       "SingleMuonStream_TauLooseForStacoMuonFilter" ],
                                                       Prescale    = 1 )
        # tauAcceptAlgsList.append("SingleMuonStream_TauLooseStacoMuonPrescaleFilter")

        # Define the combination of Staco muon and tau
        from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner
        desdSingleMuonSequence += D2PDParticleCombiner( "SingleMuonStream_ZtauStacomuFilter",
                                                        OutputLevel             = INFO,
                                                        compositeParticlePDG_ID = 23, # This is a Z Boson
                                                        inputCollectionList     = [ 'DESD_SGLMUStream_StacoMuonForMuonTauLooseLinkCollection',
                                                                                    'DESD_SGLMUStream_TauLooseForStacoMuonLinkCollection' ],
                                                        outputCollection        = 'DESD_SGLMUStream_ZtauStacomuCollection',
                                                        chargeMin               = -1,
                                                        chargeMax               = 1,
                                                        deltaPhiMin             = 2.2,
                                                        minNumberPassed         = 1
                                                        )
        tauAcceptAlgsList.append("SingleMuonStream_ZtauStacomuFilter")
        pass
    
    # Check if we have the Muons chain collection available or not
    if hasMuons:
        muonDPDStream_msg.debug("Using Muons collection for MuonTauLooseFilter")
        
        # Create a Muons muon filter
        from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_MuonsForMuonTauLooseFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'Muons',
                                                    outputLinkCollection              = 'DESD_SGLMUStream_MuonsForMuonTauLooseLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.MuonTauLooseFilter.MuonEt,
                                                    ptMax                             = primDPDsingleMu.MuonTauLooseFilter.MuonEtMax,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    muonParametersList                = [ 19, 3 ],
                                                    muonParametersMinList             = [ -100000000.0, -100000000.0 ],
                                                    muonParametersMaxList             = [ primDPDsingleMu.MuonTauLooseFilter.RelativeMuonIso,
                                                                                          primDPDsingleMu.MuonTauLooseFilter.RelativeMuonEtIso],
                                                    muonParametersUseRelativeCutsList = [ True, True ],
                                                    muonRequireIsMedium                = True
                                                    )
        
        # Set up a DeltaR association/matching tool
        from AssociationComps.AssociationCompsConf import DeltaRAssociationTool
        ToolSvc += DeltaRAssociationTool( "SingleMuonStream_TauLooseMuonsDeltaRAssociationTool",
                                          OutputLevel                = INFO,
                                          inputAssociateToCollection = "DESD_SGLMUStream_MuonsForMuonTauLooseLinkCollection",
                                          deltaRMax                  = 0.4,
                                          writeUserData              = False
                                          )

        # Set up the tau filter
        from D2PDMaker.D2PDMakerConf import D2PDTauSelector
        desdSingleMuonSequence += D2PDTauSelector( "SingleMuonStream_TauLooseForMuonsFilter",
                                                   OutputLevel                    = INFO,
                                                   inputCollection                = 'TauRecContainer',
                                                   outputLinkCollection           = 'DESD_SGLMUStream_TauLooseForMuonsLinkCollection',
                                                   minNumberPassed                = 1,
                                                   associationToolList            = [ ToolSvc.SingleMuonStream_TauLooseMuonsDeltaRAssociationTool ],
                                                   outputAssociationContainerList = [ "DESD_SGLMUStream_TauLooseToMuonsAssociations" ],
                                                   numberOfAssociationsMaxCutList = [ 0 ],
                                                   minNumberTracks                = primDPDsingleMu.MuonTauLooseFilter.TauNTrackMin,
                                                   maxNumberTracks                = primDPDsingleMu.MuonTauLooseFilter.TauNTrackMax,
                                                   etMin                          = primDPDsingleMu.MuonTauLooseFilter.TauEt
                                                   #etMax                          = primDPDsingleMu.MuonTauLooseFilter.TauEtBreak
                                                   )
        

        # Define the combination of muon and tau
        from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner
        desdSingleMuonSequence += D2PDParticleCombiner( "SingleMuonStream_ZtaumuFilter",
                                                        OutputLevel             = INFO,
                                                        compositeParticlePDG_ID = 23, # This is a Z Boson
                                                        inputCollectionList     = [ 'DESD_SGLMUStream_TauLooseForMuonsLinkCollection',
                                                                                    'DESD_SGLMUStream_MuonsForMuonTauLooseLinkCollection' ],
                                                        outputCollection        = 'DESD_SGLMUStream_ZtaumuCollection',
                                                        chargeMin               = -1,
                                                        chargeMax               = 1,
                                                        deltaPhiMin             = 2.2,
                                                        minNumberPassed         = 1
                                                        )
        tauAcceptAlgsList.append("SingleMuonStream_ZtaumuFilter")
        pass
    

    # Create a MuID muon filter
    if hasMuidMuons:
        from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_MuidMuonForMuonTauLooseFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'MuidMuonCollection',
                                                    outputLinkCollection              = 'DESD_SGLMULStream_MuidMuonForMuonTauLooseLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.MuonTauLooseFilter.MuonEt,
                                                    ptMax                             = primDPDsingleMu.MuonTauLooseFilter.MuonEtMax,
                                                    #muonParametersList                = [ MuonParameters.ptcone40 ],
                                                    muonParametersList                = [ 19, 3 ],
                                                    muonParametersMinList             = [ -100000000.0, -100000000.0 ],
                                                    muonParametersMaxList             = [ primDPDsingleMu.MuonTauLooseFilter.RelativeMuonIso,
                                                                                          primDPDsingleMu.MuonTauLooseFilter.RelativeMuonEtIso ],
                                                    muonParametersUseRelativeCutsList = [ True, True ],
                                                    muonRequireIsTight                = True
                                                    )

        # Set up a DeltaR association/matching tool
        from AssociationComps.AssociationCompsConf import DeltaRAssociationTool
        ToolSvc += DeltaRAssociationTool( "SingleMuonStream_TauLooseMuidMuonDeltaRAssociationTool",
                                          OutputLevel                = INFO,
                                          inputAssociateToCollection = "DESD_SGLMULStream_MuidMuonForMuonTauLooseLinkCollection",
                                          deltaRMax                  = 0.4,
                                          writeUserData              = False
                                          )
        # Set up the tau filter
        from D2PDMaker.D2PDMakerConf import D2PDTauSelector
        desdSingleMuonSequence += D2PDTauSelector( "SingleMuonStream_TauLooseForMuidMuonFilter",
                                                   OutputLevel                    = INFO,
                                                   inputCollection                = 'TauRecContainer',
                                                   outputLinkCollection           = 'DESD_SGLMUStream_TauLooseForMuidMuonLinkCollection',
                                                   minNumberPassed                = 1,
                                                   associationToolList            = [ ToolSvc.SingleMuonStream_TauLooseMuidMuonDeltaRAssociationTool ],
                                                   outputAssociationContainerList = [ "DESD_SGLMUStream_TauLooseToMuidMuonAssociations" ],
                                                   numberOfAssociationsMaxCutList = [ 0 ],
                                                   minNumberTracks                = primDPDsingleMu.MuonTauLooseFilter.TauNTrackMin,
                                                   maxNumberTracks                = primDPDsingleMu.MuonTauLooseFilter.TauNTrackMax,
                                                   etMin                          = primDPDsingleMu.MuonTauLooseFilter.TauEt
                                                   )
    
        desdSingleMuonSequence += PrimaryDPDPrescaler( "SingleMuonStream_TauLooseMuidMuonPrescaleFilter",
                                                       OutputLevel = INFO,
                                                       RequireAlgs = [ "SingleMuonStream_MuidMuonForMuonTauLooseFilter",
                                                                       "SingleMuonStream_TauLooseForMuidMuonFilter" ],
                                                       Prescale    = 1 )
        #tauAcceptAlgsList.append("SingleMuonStream_TauLooseMuidMuonPrescaleFilter")
        
        # Define the combination of muon and tau
        from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner
        desdSingleMuonSequence += D2PDParticleCombiner( "SingleMuonStream_ZtauMuidmuFilter",
                                                        OutputLevel             = INFO,
                                                        compositeParticlePDG_ID = 23, # This is a Z Boson
                                                        inputCollectionList     = [ 'DESD_SGLMUStream_TauLooseForMuidMuonLinkCollection',
                                                                                    'DESD_SGLMULStream_MuidMuonForMuonTauLooseLinkCollection' ],
                                                        outputCollection        = 'DESD_SGLMUStream_ZtauMuidmuCollection',
                                                        chargeMin               = -1,
                                                        chargeMax               = 1,
                                                        deltaPhiMin             = 2.2,
                                                        minNumberPassed         = 1
                                                        )
        tauAcceptAlgsList.append("SingleMuonStream_ZtauMuidmuFilter")
        pass

    # Create the combined decision for the Z -> tau tau
    desdSingleMuonSequence += PrimaryDPDPrescaler( "SingleMuonStream_TauLooseMuonPrescaleFilter",
                                                   OutputLevel = INFO,
                                                   AcceptAlgs  = tauAcceptAlgsList,
                                                   Prescale    = primDPDsingleMu.MuonTauLooseFilter.Prescale )
    
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("SingleMuonStream_TauLooseMuonPrescaleFilter")
    # If the other tau filter is not scheduled, add only this one. Otherwise, add a combination below
    if not primDPDsingleMu.TauForZtautauFilter() :
        if commandstring=="":
            commandstring = "SingleMuonStream_TauLooseMuonPrescaleFilter"
            pass
        else:
            commandstring = commandstring + " or SingleMuonStream_TauLooseMuonPrescaleFilter"
            pass
        pass
    pass



# Define the combination of the tau filters for bookkeeping reasons
if primDPDsingleMu.MuonTauLooseFilter() and  primDPDsingleMu.TauForZtautauFilter() :
    desdSingleMuonSequence += PrimaryDPDPrescaler( "SingleMuonStream_TauCombinedPrescaleFilter",
                                                   OutputLevel = INFO,
                                                   AcceptAlgs = [ "SingleMuonStream_TauMuonPrescaleFilter",
                                                                  "SingleMuonStream_TauLooseMuonPrescaleFilter" ],
                                                   Prescale    = 1 )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("SingleMuonStream_TauCombinedPrescaleFilter")
    
    if commandstring=="":
        commandstring = "SingleMuonStream_TauCombinedPrescaleFilter"
        pass
    else:
        commandstring = commandstring + " or SingleMuonStream_TauCombinedPrescaleFilter"
        pass
    pass


# ------------------------
# Create the electron muon filters
# ------------------------
if primDPDsingleMu.ElectronMuonFilter():
    # Create a list of accept algs for the tau filters
    elMuAcceptAlgsList = []

    # Create a Staco muon filter
    if hasStacoMuons:
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_StacoMuonForElectronMuonFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'StacoMuonCollection',
                                                    #outputLinkCollection              = 'DESD_SGLMUStream_StacoMuonForElectronMuonLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.ElectronMuonFilter.MuonEt,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    muonParametersList                = [ 19 ],
                                                    muonParametersMinList             = [ -100000000.0 ],
                                                    muonParametersMaxList             = [ primDPDsingleMu.ElectronMuonFilter.RelativeMuonIso ],
                                                    muonParametersUseRelativeCutsList = [ True ],
                                                    muonRequireIsLoose                = True
                                                    )
        elMuAcceptAlgsList.append("SingleMuonStream_StacoMuonForElectronMuonFilter")
        pass
    
    # Create a MuID muon filter
    if hasMuidMuons:
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_MuidMuonForElectronMuonFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'MuidMuonCollection',
                                                    #outputLinkCollection              = 'DESD_SGLMUStream_MuidMuonForElectronMuonLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.ElectronMuonFilter.MuonEt,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    muonParametersList                = [ 19 ],
                                                    muonParametersMinList             = [ -100000000.0 ],
                                                    muonParametersMaxList             = [ primDPDsingleMu.ElectronMuonFilter.RelativeMuonIso ],
                                                    muonParametersUseRelativeCutsList = [ True ],
                                                    muonRequireIsTight                = True
                                                    )
        elMuAcceptAlgsList.append("SingleMuonStream_MuidMuonForElectronMuonFilter")
        pass

    # Check if we have the Muons chain collection available or not
    if hasMuons:
        muonDPDStream_msg.debug("Using Muons collection for ElectronMuonFilter")
        
        # Create a third chain muon filter
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_MuonsForElectronMuonFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'Muons',
                                                    #outputLinkCollection              = 'DESD_SGLMUStream_MuonsForElectronMuonLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.ElectronMuonFilter.MuonEt,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    muonParametersList                = [ 19 ],
                                                    muonParametersMinList             = [ -100000000.0 ],
                                                    muonParametersMaxList             = [ primDPDsingleMu.ElectronMuonFilter.RelativeMuonIso ],
                                                    muonParametersUseRelativeCutsList = [ True ],
                                                    muonRequireIsMedium             = True
                                                    )
        elMuAcceptAlgsList.append("SingleMuonStream_MuonsForElectronMuonFilter")
        pass
    
    # Create the electron filter
    desdSingleMuonSequence += D2PDElectronSelector( "SingleMuonStream_ElectronForElectronMuonFilter",
                                                    OutputLevel            = INFO,
                                                    inputCollection        = 'ElectronAODCollection',
                                                    #outputLinkCollection   = 'DESD_SGLMUStream_ElectronForElectronMuonLinkCollection',
                                                    minNumberPassed        = 1,
                                                    electronVetoAuthorList = primDPDsingleMu.ElectronMuonFilter.VetoElectronAuthor,
                                                    electronID             = primDPDsingleMu.ElectronMuonFilter.ElectronID,
                                                    clusterEtMin           = primDPDsingleMu.ElectronMuonFilter.ElectronEt
                                                    )
    
    
    # Define the prescaled filters; configurable with command line argument
    desdSingleMuonSequence += PrimaryDPDPrescaler( "SingleMuonStream_ElectronMuonPrescaleFilter",
                                                   AcceptAlgs  = elMuAcceptAlgsList,
                                                   RequireAlgs = [ "SingleMuonStream_ElectronForElectronMuonFilter" ],
                                                   Prescale = primDPDsingleMu.ElectronMuonFilter.Prescale )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("SingleMuonStream_ElectronMuonPrescaleFilter")
    if commandstring=="":
        commandstring = "SingleMuonStream_ElectronMuonPrescaleFilter"
        pass
    else:
        commandstring = commandstring + " or SingleMuonStream_ElectronMuonPrescaleFilter"
        pass
    pass




# ------------------------
# Create the semi-leptonic ttbar filter
# ------------------------
if primDPDsingleMu.SemiLepTTBarFilter():
    # Create a list of accept algs for the tau filters
    ttbarAcceptAlgsList = []
    
    # Create a Staco muon filter
    if hasStacoMuons:
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_SemiLepTTBarStacoMuonFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'StacoMuonCollection',
                                                    #outputLinkCollection              = 'DESD_SGLMUStream_SemiLepTTBarStacoMuonLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.SemiLepTTBarFilter.MuonEt,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    muonParametersList                = [ 19 ],
                                                    muonParametersMinList             = [ -100000000.0 ],
                                                    muonParametersMaxList             = [ primDPDsingleMu.SemiLepTTBarFilter.RelativeMuonIso ],
                                                    muonParametersUseRelativeCutsList = [ True ],
                                                    muonRequireIsLoose                = True
                                                    )
        ttbarAcceptAlgsList.append("SingleMuonStream_SemiLepTTBarStacoMuonFilter")
        pass
    
    # Check if we have the Muons chain collection available or not
    if hasMuons:
        muonDPDStream_msg.debug("Using Muons collection for SemiLepTTBarFilter")
        
        # Create a Muons muon filter
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_SemiLepTTBarMuonsFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'Muons',
                                                    #outputLinkCollection              = 'DESD_SGLMUStream_SemiLepTTBarMuonsLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.SemiLepTTBarFilter.MuonEt,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    muonParametersList                = [ 19 ],
                                                    muonParametersMinList             = [ -100000000.0 ],
                                                    muonParametersMaxList             = [ primDPDsingleMu.SemiLepTTBarFilter.RelativeMuonIso ],
                                                    muonParametersUseRelativeCutsList = [ True ],
                                                    muonRequireIsMedium                = True
                                                    )
        ttbarAcceptAlgsList.append("SingleMuonStream_SemiLepTTBarMuonsFilter")
        pass
    
    # Create a MuID muon filter
    if hasMuidMuons:
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_SemiLepTTBarMuidMuonFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'MuidMuonCollection',
                                                    #outputLinkCollection              = 'DESD_SGLMUStream_SemiLepTTBarMuidMuonLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.SemiLepTTBarFilter.MuonEt,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    muonParametersList                = [ 19 ],
                                                    muonParametersMinList             = [ -100000000.0 ],
                                                    muonParametersMaxList             = [ primDPDsingleMu.SemiLepTTBarFilter.RelativeMuonIso ],
                                                    muonParametersUseRelativeCutsList = [ True ],
                                                    muonRequireIsTight                = True
                                                    )
        ttbarAcceptAlgsList.append("SingleMuonStream_SemiLepTTBarMuidMuonFilter")
        pass
    
    # Combine the three muon algorithms
    desdSingleMuonSequence += PrimaryDPDPrescaler( "SingleMuonStream_SemiLepTTBarCombinedMuonPrescaleFilter",
                                                   AcceptAlgs  = ttbarAcceptAlgsList,
                                                   Prescale = 1
                                                   )
    desdSingleMuonSequence += D2PDMissingEtSelector( "SingleMuonStream_SemiLepTTBarMissingEtFilter",
                                                     OutputLevel      = INFO,
                                                     inputCollection  = 'MET_RefFinal',
                                                     outputCollection = 'DESD_SGLMUStream_SemiLepTTBarNeutrinoCollection',
                                                     minNumberPassed  = 1,
                                                     missingEtMin     = primDPDsingleMu.SemiLepTTBarFilter.MissingEt
                                                     )
    desdSingleMuonSequence += D2PDJetSelector( "SingleMuonStream_SemiLepTTBarJetFilter",
                                               OutputLevel          = INFO,
                                               inputCollection      = primDPDsingleMu.SemiLepTTBarFilter.JetCollection,
                                               #outputLinkCollection = 'DESD_SGLMUStream_SemiLepTTBarJetLinkCollection',
                                               minNumberPassed      = 4,
                                               ptMin                = primDPDsingleMu.SemiLepTTBarFilter.JetPt,
                                               absEtaMax            = primDPDsingleMu.SemiLepTTBarFilter.JetEta
                                               )
    # Define the prescaled filters; configurable with command line argument
    desdSingleMuonSequence += PrimaryDPDPrescaler( "SingleMuonStream_SemiLepTTBarMuonPrescaleFilter",
                                                   RequireAlgs = [ "SingleMuonStream_SemiLepTTBarCombinedMuonPrescaleFilter",
                                                                   "SingleMuonStream_SemiLepTTBarMissingEtFilter",
                                                                   "SingleMuonStream_SemiLepTTBarJetFilter" ],
                                                   Prescale    = primDPDsingleMu.SemiLepTTBarFilter.Prescale )
    
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("SingleMuonStream_SemiLepTTBarMuonPrescaleFilter")
    if commandstring=="":
        commandstring = "SingleMuonStream_SemiLepTTBarMuonPrescaleFilter"
        pass
    else:
        commandstring = commandstring + " or SingleMuonStream_SemiLepTTBarMuonPrescaleFilter"
        pass
    pass



# ------------------------
# Create the modified ttbar filter
# ------------------------
if primDPDsingleMu.ModifiedTTBarFilter():
    # Create a list of accept algs for the tau filters
    ttbarAcceptAlgsList = []
    
    # Create a Staco muon filter
    if hasStacoMuons:
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_ModifiedTTBarStacoMuonFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'StacoMuonCollection',
                                                    #outputLinkCollection              = 'DESD_SGLMUStream_ModifiedTTBarStacoMuonLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.ModifiedTTBarFilter.MuonEt,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    muonParametersList                = [ 19 ],
                                                    muonParametersMinList             = [ -100000000.0 ],
                                                    muonParametersMaxList             = [ primDPDsingleMu.ModifiedTTBarFilter.RelativeMuonIso ],
                                                    muonParametersUseRelativeCutsList = [ True ],
                                                    muonRequireIsLoose                = True
                                                    )
        ttbarAcceptAlgsList.append("SingleMuonStream_ModifiedTTBarStacoMuonFilter")
        pass
    
    # Check if we have the Muons chain collection available or not
    if hasMuons:
        muonDPDStream_msg.debug("Using Muons collection for ModifiedTTBarFilter")
        
        # Create a Muons muon filter
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_ModifiedTTBarMuonsFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'Muons',
                                                    #outputLinkCollection              = 'DESD_SGLMUStream_ModifiedTTBarMuonsLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.ModifiedTTBarFilter.MuonEt,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    muonParametersList                = [ 19 ],
                                                    muonParametersMinList             = [ -100000000.0 ],
                                                    muonParametersMaxList             = [ primDPDsingleMu.ModifiedTTBarFilter.RelativeMuonIso ],
                                                    muonParametersUseRelativeCutsList = [ True ],
                                                    muonRequireIsMedium                = True
                                                    )
        ttbarAcceptAlgsList.append("SingleMuonStream_ModifiedTTBarMuonsFilter")
        pass
    
    # Create a MuID muon filter
    if hasMuidMuons:
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_ModifiedTTBarMuidMuonFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'MuidMuonCollection',
                                                    #outputLinkCollection              = 'DESD_SGLMUStream_ModifiedTTBarMuidMuonLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.ModifiedTTBarFilter.MuonEt,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    muonParametersList                = [ 19 ],
                                                    muonParametersMinList             = [ -100000000.0 ],
                                                    muonParametersMaxList             = [ primDPDsingleMu.ModifiedTTBarFilter.RelativeMuonIso ],
                                                    muonParametersUseRelativeCutsList = [ True ],
                                                    muonRequireIsTight                = True
                                                    )
        ttbarAcceptAlgsList.append("SingleMuonStream_ModifiedTTBarMuidMuonFilter")
        pass
    
    # Combine the three muon algorithms
    desdSingleMuonSequence += PrimaryDPDPrescaler( "SingleMuonStream_ModifiedTTBarCombinedMuonPrescaleFilter",
                                                   AcceptAlgs  = ttbarAcceptAlgsList,
                                                   Prescale = 1
                                                   )
    desdSingleMuonSequence += D2PDMissingEtSelector( "SingleMuonStream_ModifiedTTBarMissingEtFilter",
                                                     OutputLevel      = INFO,
                                                     inputCollection  = 'MET_RefFinal',
                                                     outputCollection = 'DESD_SGLMUStream_ModifiedTTBarNeutrinoCollection',
                                                     minNumberPassed  = 1,
                                                     missingEtMin     = primDPDsingleMu.ModifiedTTBarFilter.MissingEt
                                                     )
    desdSingleMuonSequence += D2PDJetSelector( "SingleMuonStream_ModifiedTTBarJetFilter",
                                               OutputLevel          = INFO,
                                               inputCollection      = primDPDsingleMu.ModifiedTTBarFilter.JetCollection,
                                               #outputLinkCollection = 'DESD_SGLMUStream_ModifiedTTBarJetLinkCollection',
                                               minNumberPassed      = 3,
                                               ptMin                = primDPDsingleMu.ModifiedTTBarFilter.JetPt,
                                               absEtaMax            = primDPDsingleMu.ModifiedTTBarFilter.JetEta
                                               )
    # Define the prescaled filters; configurable with command line argument
    desdSingleMuonSequence += PrimaryDPDPrescaler( "SingleMuonStream_ModifiedTTBarMuonPrescaleFilter",
                                                   RequireAlgs = [ "SingleMuonStream_ModifiedTTBarCombinedMuonPrescaleFilter",
                                                                   "SingleMuonStream_ModifiedTTBarMissingEtFilter",
                                                                   "SingleMuonStream_ModifiedTTBarJetFilter" ],
                                                   Prescale    = primDPDsingleMu.ModifiedTTBarFilter.Prescale )
    
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("SingleMuonStream_ModifiedTTBarMuonPrescaleFilter")
    if commandstring=="":
        commandstring = "SingleMuonStream_ModifiedTTBarMuonPrescaleFilter"
        pass
    else:
        commandstring = commandstring + " or SingleMuonStream_ModifiedTTBarMuonPrescaleFilter"
        pass
    pass


# ------------------------
# Create the Wmunu filter
# ------------------------
if primDPDsingleMu.WmunuFilter():
    # Create a list of accept algs for the tau filters
    wmunuAcceptAlgsList = []

    # Create a Staco muon filter
    if hasStacoMuons:
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_WmunuStacoMuonFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'StacoMuonCollection',
                                                    #outputLinkCollection              = 'DESD_SGLMUStream_WmunuStacoMuonLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.WmunuFilter.MuonEt,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    muonParametersList                = [ 19 ],
                                                    muonParametersMinList             = [ -100000000.0 ],
                                                    muonParametersMaxList             = [ primDPDsingleMu.WmunuFilter.RelativeMuonIso ],
                                                    muonParametersUseRelativeCutsList = [ True ],
                                                    muonRequireIsLoose                = True
                                                    )
        wmunuAcceptAlgsList.append("SingleMuonStream_WmunuStacoMuonFilter")
        pass
    
    # Check if we have the Muons chain collection available or not
    if hasMuons:
        muonDPDStream_msg.debug("Using Muons collection for WmunuFilter")

        # Create a Muons muon filter
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_WmunuMuonsFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'Muons',
                                                    #outputLinkCollection              = 'DESD_SGLMUStream_WmunuMuonsLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.WmunuFilter.MuonEt,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    muonParametersList                = [ 19 ],
                                                    muonParametersMinList             = [ -100000000.0 ],
                                                    muonParametersMaxList             = [ primDPDsingleMu.WmunuFilter.RelativeMuonIso ],
                                                    muonParametersUseRelativeCutsList = [ True ],
                                                    muonRequireIsMedium             = True
                                                    )
        wmunuAcceptAlgsList.append("SingleMuonStream_WmunuMuonsFilter")
        pass
    
    # Create a MuID muon filter
    if hasMuidMuons:
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_WmunuMuidMuonFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'MuidMuonCollection',
                                                    #outputLinkCollection              = 'DESD_SGLMUStream_WmunuMuidMuonLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.WmunuFilter.MuonEt,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    muonParametersList                = [ 19 ],
                                                    muonParametersMinList             = [ -100000000.0 ],
                                                    muonParametersMaxList             = [ primDPDsingleMu.WmunuFilter.RelativeMuonIso ],
                                                    muonParametersUseRelativeCutsList = [ True ],
                                                    muonRequireIsTight                = True
                                                    )
        wmunuAcceptAlgsList.append("SingleMuonStream_WmunuMuidMuonFilter")
        pass
    
    # Combine the two muon algorithms
    desdSingleMuonSequence += PrimaryDPDPrescaler( "SingleMuonStream_WmunuCombinedMuonPrescaleFilter",
                                                   AcceptAlgs  = wmunuAcceptAlgsList,
                                                   Prescale = 1
                                                   )
    desdSingleMuonSequence += D2PDMissingEtSelector( "SingleMuonStream_WmunuMissingEtFilter",
                                                     OutputLevel      = INFO,
                                                     inputCollection  = 'MET_RefFinal',
                                                     outputCollection = 'DESD_SGLMUStream_WmunuNeutrinoCollection',
                                                     minNumberPassed  = 1,
                                                     missingEtMin     = primDPDsingleMu.WmunuFilter.MissingEt
                                                     )
    # Define the prescaled filters; configurable with command line argument
    desdSingleMuonSequence += PrimaryDPDPrescaler( "SingleMuonStream_WmunuMuonPrescaleFilter",
                                                   RequireAlgs = [ "SingleMuonStream_WmunuCombinedMuonPrescaleFilter",
                                                                   "SingleMuonStream_WmunuMissingEtFilter" ],
                                                   Prescale    = primDPDsingleMu.WmunuFilter.Prescale )
    
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("SingleMuonStream_WmunuMuonPrescaleFilter")
    if commandstring=="":
        commandstring = "SingleMuonStream_WmunuMuonPrescaleFilter"
        pass
    else:
        commandstring = commandstring + " or SingleMuonStream_WmunuMuonPrescaleFilter"
        pass
    pass

# ------------------------
# Create the Wmunu+jets filter 
# ------------------------
if primDPDsingleMu.WmunuHighPtFilter():
    # Create a list of accept algs for the tau filters
    wmunuHighPtAcceptAlgsList = []

    # Create a Staco muon filter
    if hasStacoMuons:
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_WmunuHighPtStacoMuonFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'StacoMuonCollection',
                                                    #outputLinkCollection              = 'DESD_SGLMUStream_WmunuHighPtStacoMuonLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.WmunuHighPtFilter.MuonEt,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    muonParametersList                = [ 19 ],
                                                    muonParametersMinList             = [ -100000000.0 ],
                                                    muonParametersMaxList             = [ primDPDsingleMu.WmunuHighPtFilter.RelativeMuonIso ],
                                                    muonParametersUseRelativeCutsList = [ True ],
                                                    muonRequireIsLoose                = True
                                                    )
        wmunuHighPtAcceptAlgsList.append("SingleMuonStream_WmunuHighPtStacoMuonFilter")
        pass
    
    # Check if we have the Muons chain collection available or not
    if hasMuons:
        muonDPDStream_msg.debug("Using Muons collection for WmunuHighPtFilter")
        
        # Create a Muons muon filter
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_WmunuHighPtMuonsFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'Muons',
                                                    #outputLinkCollection              = 'DESD_SGLMUStream_WmunuHighPtMuonsLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.WmunuHighPtFilter.MuonEt,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    muonParametersList                = [ 19 ],
                                                    muonParametersMinList             = [ -100000000.0 ],
                                                    muonParametersMaxList             = [ primDPDsingleMu.WmunuHighPtFilter.RelativeMuonIso ],
                                                    muonParametersUseRelativeCutsList = [ True ],
                                                    muonRequireIsMedium             = True
                                                    )
        wmunuHighPtAcceptAlgsList.append("SingleMuonStream_WmunuHighPtMuonsFilter")
        pass
    
    # Create a MuID muon filter
    if hasMuidMuons:
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_WmunuHighPtMuidMuonFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'MuidMuonCollection',
                                                    #outputLinkCollection              = 'DESD_SGLMUStream_WmunuHighPtMuidMuonLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.WmunuHighPtFilter.MuonEt,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    muonParametersList                = [ 19 ],
                                                    muonParametersMinList             = [ -100000000.0 ],
                                                    muonParametersMaxList             = [ primDPDsingleMu.WmunuHighPtFilter.RelativeMuonIso ],
                                                    muonParametersUseRelativeCutsList = [ True ],
                                                    muonRequireIsTight                = True
                                                    )
        wmunuHighPtAcceptAlgsList.append("SingleMuonStream_WmunuHighPtMuidMuonFilter")
        pass
    
    # Combine the three primary muon algorithms
    desdSingleMuonSequence += PrimaryDPDPrescaler( "SingleMuonStream_WmunuHighPtCombinedMuonPrescaleFilter",
                                                   AcceptAlgs = wmunuHighPtAcceptAlgsList,
                                                   Prescale   = 1
                                                   )
    desdSingleMuonSequence += D2PDMissingEtSelector( "SingleMuonStream_WmunuHighPtMissingEtFilter",
                                                     OutputLevel      = INFO,
                                                     inputCollection  = 'MET_RefFinal',
                                                     outputCollection = 'DESD_SGLMUStream_WmunuHighPtNeutrinoCollection',
                                                     minNumberPassed  = 1,
                                                     missingEtMin     = primDPDsingleMu.WmunuHighPtFilter.MissingEt
                                                     )
    
    # Create a list of accept algs for the filters
    wmunuHighPtExtraAcceptAlgsList = []

    
    # Create a Staco muon filter
    if hasStacoMuons:
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_WmunuHighPtExtraStacoMuonFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'StacoMuonCollection',
                                                    #outputLinkCollection              = 'DESD_SGLMUStream_WmunuHighPtExtraStacoMuonLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.WmunuHighPtFilter.ExtraObjectPt,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    muonParametersList                = [ 19 ],
                                                    muonParametersMinList             = [ -100000000.0 ],
                                                    muonParametersMaxList             = [ primDPDsingleMu.WmunuHighPtFilter.RelativeMuonIso ],
                                                    muonParametersUseRelativeCutsList = [ True ],
                                                    muonRequireIsLoose                = True
                                                    )
        wmunuHighPtExtraAcceptAlgsList.append("SingleMuonStream_WmunuHighPtExtraStacoMuonFilter")
        pass
    
    # Check if we have the Muons chain collection available or not
    if hasMuons:
        muonDPDStream_msg.debug("Using Muons collection for WmunuHighPtFilter 2")
        
        # Create a Muons muon filter
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_WmunuHighPtExtraMuonsFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'Muons',
                                                    #outputLinkCollection              = 'DESD_SGLMUStream_WmunuHighPtExtraMuonsLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.WmunuHighPtFilter.ExtraObjectPt,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    muonParametersList                = [ 19 ],
                                                    muonParametersMinList             = [ -100000000.0 ],
                                                    muonParametersMaxList             = [ primDPDsingleMu.WmunuHighPtFilter.RelativeMuonIso ],
                                                    muonParametersUseRelativeCutsList = [ True ],
                                                    muonRequireIsMedium                = True
                                                    )
        wmunuHighPtExtraAcceptAlgsList.append("SingleMuonStream_WmunuHighPtExtraMuonsFilter")
        pass

    # Create a MuID muon filter
    if hasMuidMuons:
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_WmunuHighPtExtraMuidMuonFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'MuidMuonCollection',
                                                    #outputLinkCollection              = 'DESD_SGLMUStream_WmunuHighPtExtraMuidMuonLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.WmunuHighPtFilter.ExtraObjectPt,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    muonParametersList                = [ 19 ],
                                                    muonParametersMinList             = [ -100000000.0 ],
                                                    muonParametersMaxList             = [ primDPDsingleMu.WmunuHighPtFilter.RelativeMuonIso ],
                                                    muonParametersUseRelativeCutsList = [ True ],
                                                    muonRequireIsTight                = True
                                                    )
        wmunuHighPtExtraAcceptAlgsList.append("SingleMuonStream_WmunuHighPtExtraMuidMuonFilter")
        pass
    
    # Combine the two muon algorithms
    desdSingleMuonSequence += PrimaryDPDPrescaler( "SingleMuonStream_WmunuHighPtCombinedExtraMuonPrescaleFilter",
                                                   AcceptAlgs = wmunuHighPtExtraAcceptAlgsList,
                                                   Prescale   = 1
                                                   )
    desdSingleMuonSequence += D2PDJetSelector( "SingleMuonStream_WmunuHighPtJetFilter",
                                               OutputLevel          = INFO,
                                               inputCollection      = primDPDsingleMu.WmunuHighPtFilter.JetCollection,
                                               #outputLinkCollection = 'DESD_SGLMUStream_WmunuHighPtJetLinkCollection',
                                               minNumberPassed      = 1,
                                               ptMin                = primDPDsingleMu.WmunuHighPtFilter.ExtraObjectPt,
                                               absEtaMax            = primDPDsingleMu.WmunuHighPtFilter.JetEta
                                               )
    
    # Define the prescaled filters; configurable with command line argument
    desdSingleMuonSequence += PrimaryDPDPrescaler( "SingleMuonStream_WmunuHighPtPrescaleFilter",
                                                   RequireAlgs = [ "SingleMuonStream_WmunuHighPtCombinedMuonPrescaleFilter",
                                                                   "SingleMuonStream_WmunuHighPtMissingEtFilter" ],
                                                   #                "SingleMuonStream_WmunuHighPtJetFilter"],
                                                   AcceptAlgs = [ "SingleMuonStream_WmunuHighPtCombinedExtraMuonPrescaleFilter",
                                                                  "SingleMuonStream_WmunuHighPtJetFilter" ],
                                                   Prescale    = primDPDsingleMu.WmunuHighPtFilter.Prescale )
    
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("SingleMuonStream_WmunuHighPtPrescaleFilter")
    if commandstring=="":
        commandstring = "SingleMuonStream_WmunuHighPtPrescaleFilter"
        pass
    else:
        commandstring = commandstring + " or SingleMuonStream_WmunuHighPtPrescaleFilter"
        pass
    pass


# ------------------------
# Create the isolated di-muon filters
# ------------------------
if primDPDsingleMu.DiIsolatedMuonFilter():
    # Create a list of accept algs for the filters
    diIsoAcceptAlgsList = []

    # Create a di-staco muon filter
    if hasStacoMuons:
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_DiIsolatedStacoMuonFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'StacoMuonCollection',
                                                    #outputLinkCollection              = 'DESD_SGLMUStream_DiIsolatedStacoMuonLinkCollection',
                                                    minNumberPassed                   = 2,
                                                    ptMin                             = primDPDsingleMu.DiIsolatedMuonFilter.MuonEt,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    # muonParametersList                = [ 19 ],
                                                    # muonParametersMinList             = [ -100000000.0 ],
                                                    # muonParametersMaxList             = [ primDPDsingleMu.DiIsolatedMuonFilter.RelativeMuonIso ],
                                                    # muonParametersUseRelativeCutsList = [ True ],
                                                    muonRequireIsLoose                = True
                                                    )
        diIsoAcceptAlgsList.append("SingleMuonStream_DiIsolatedStacoMuonFilter")
        pass

    # Check if we have the Muons chain collection available or not
    if hasMuons:
        muonDPDStream_msg.debug("Using Muons collection for DiIsolatedMuonFilter")
        
        # Create a di-Muons muon filter
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_DiIsolatedMuonsFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'Muons',
                                                    #outputLinkCollection              = 'DESD_SGLMUStream_DiIsolatedMuonsLinkCollection',
                                                    minNumberPassed                   = 2,
                                                    ptMin                             = primDPDsingleMu.DiIsolatedMuonFilter.MuonEt,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    # muonParametersList                = [ 19 ],
                                                    # muonParametersMinList             = [ -100000000.0 ],
                                                    # muonParametersMaxList             = [ primDPDsingleMu.DiIsolatedMuonFilter.RelativeMuonIso ],
                                                    # muonParametersUseRelativeCutsList = [ True ],
                                                    muonRequireIsMedium                = True
                                                    )
        diIsoAcceptAlgsList.append("SingleMuonStream_DiIsolatedMuonsFilter")
        pass
    
    # Create a di-MuID muon filter
    if hasMuidMuons:
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_DiIsolatedMuidMuonFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'MuidMuonCollection',
                                                    #outputLinkCollection              = 'DESD_SGLMUStream_DiIsolatedMuidMuonLinkCollection',
                                                    minNumberPassed                   = 2,
                                                    ptMin                             = primDPDsingleMu.DiIsolatedMuonFilter.MuonEt,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    # muonParametersList                = [ 19 ],
                                                    # muonParametersMinList             = [ -100000000.0 ],
                                                    # muonParametersMaxList             = [ primDPDsingleMu.DiIsolatedMuonFilter.RelativeMuonIso ],
                                                    # muonParametersUseRelativeCutsList = [ True ],
                                                    muonRequireIsTight                = True
                                                    )
        diIsoAcceptAlgsList.append("SingleMuonStream_DiIsolatedMuidMuonFilter")
        pass
    
    
    # Define the prescaled filters; configurable with command line argument
    desdSingleMuonSequence += PrimaryDPDPrescaler( "SingleMuonStream_DiIsolatedMuonPrescaleFilter",
                                                   AcceptAlgs = diIsoAcceptAlgsList,
                                                   Prescale   = primDPDsingleMu.DiIsolatedMuonFilter.Prescale )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("SingleMuonStream_DiIsolatedMuonPrescaleFilter")
    if commandstring=="":
        commandstring = "SingleMuonStream_DiIsolatedMuonPrescaleFilter"
        pass
    else:
        commandstring = commandstring + " or SingleMuonStream_DiIsolatedMuonPrescaleFilter"
        pass
    pass





# ------------------------
# Create the Zmumu filters
# ------------------------
if primDPDsingleMu.ZmumuFilter():
    # Create a list of accept algs for the filters
    zmumuAcceptAlgsList = []

    # Create a di-staco muon filter
    if hasStacoMuons:
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_HighPtStacoMuonForZmumuFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'StacoMuonCollection',
                                                    outputLinkCollection              = 'DESD_SGLMUStream_ZmumuHighPtStacoMuonLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.ZmumuFilter.LeadingMuonPt,
                                                    muonRequireIsCombined             = True,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    # muonParametersList                = [ 19 ],
                                                    # muonParametersMinList             = [ -100000000.0 ],
                                                    # muonParametersMaxList             = [ primDPDsingleMu.ZmumuFilter.RelativeMuonIso ],
                                                    # muonParametersUseRelativeCutsList = [ True ],
                                                    muonRequireIsLoose                = True
                                                    )
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_LowPtStacoMuonForZmumuFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'StacoMuonCollection',
                                                    outputLinkCollection              = 'DESD_SGLMUStream_ZmumuLowPtStacoMuonLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.ZmumuFilter.SubleadingMuonPt,
                                                    muonRequireIsCombined             = True,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    # muonParametersList                = [ 19 ],
                                                    # muonParametersMinList             = [ -100000000.0 ],
                                                    # muonParametersMaxList             = [ primDPDsingleMu.ZmumuFilter.RelativeMuonIso ],
                                                    # muonParametersUseRelativeCutsList = [ True ],
                                                    muonRequireIsLoose                = True
                                                    )
        desdSingleMuonSequence += D2PDParticleCombiner( "SingleMuonStream_ZmumuStacoMuonFilter",
                                                        OutputLevel             = INFO,
                                                        compositeParticlePDG_ID = 23, # This is a Z Boson
                                                        inputCollectionList     = [ 'DESD_SGLMUStream_ZmumuHighPtStacoMuonLinkCollection',
                                                                                    'DESD_SGLMUStream_ZmumuLowPtStacoMuonLinkCollection' ],
                                                        outputCollection        = 'DESD_SGLMUStream_ZmumuWithStacoCollection',
                                                        chargeMin               = 0,
                                                        chargeMax               = 0,
                                                        massMin                 = primDPDsingleMu.ZmumuFilter.MassMin,
                                                        minNumberPassed         = 1
                                                        )
        zmumuAcceptAlgsList.append("SingleMuonStream_ZmumuStacoMuonFilter")
        pass

    # Check if we have the Muons chain collection available or not
    if hasMuons:
        muonDPDStream_msg.debug("Using Muons collection for ZmumuIsolatedMuonFilter")
        
        # Create a di-Muons muon filter
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_HighPtMuonsForZmumuFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'Muons',
                                                    outputLinkCollection              = 'DESD_SGLMUStream_ZmumuHighPtMuonsLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.ZmumuFilter.LeadingMuonPt,
                                                    muonRequireIsCombined             = True,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    # muonParametersList                = [ 19 ],
                                                    # muonParametersMinList             = [ -100000000.0 ],
                                                    # muonParametersMaxList             = [ primDPDsingleMu.ZmumuFilter.RelativeMuonIso ],
                                                    # muonParametersUseRelativeCutsList = [ True ],
                                                    muonRequireIsMedium                = True
                                                    )
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_LowPtMuonsForZmumuFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'Muons',
                                                    outputLinkCollection              = 'DESD_SGLMUStream_ZmumuLowPtMuonsLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.ZmumuFilter.SubleadingMuonPt,
                                                    muonRequireIsCombined             = True,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    # muonParametersList                = [ 19 ],
                                                    # muonParametersMinList             = [ -100000000.0 ],
                                                    # muonParametersMaxList             = [ primDPDsingleMu.ZmumuFilter.RelativeMuonIso ],
                                                    # muonParametersUseRelativeCutsList = [ True ],
                                                    muonRequireIsMedium                = True
                                                    )
        desdSingleMuonSequence += D2PDParticleCombiner( "SingleMuonStream_ZmumuMuonsFilter",
                                                        OutputLevel             = INFO,
                                                        compositeParticlePDG_ID = 23, # This is a Z Boson
                                                        inputCollectionList     = [ 'DESD_SGLMUStream_ZmumuHighPtMuonsLinkCollection',
                                                                                    'DESD_SGLMUStream_ZmumuLowPtMuonsLinkCollection' ],
                                                        outputCollection        = 'DESD_SGLMUStream_ZmumuWithMuonsCollection',
                                                        chargeMin               = 0,
                                                        chargeMax               = 0,
                                                        massMin                 = primDPDsingleMu.ZmumuFilter.MassMin,
                                                        minNumberPassed         = 1
                                                        )
        zmumuAcceptAlgsList.append("SingleMuonStream_ZmumuMuonsFilter")
        pass
    
    # Create a di-MuID muon filter
    if hasMuidMuons:
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_HighPtMuidMuonForZmumuFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'MuidMuonCollection',
                                                    outputLinkCollection              = 'DESD_SGLMUStream_ZmumuHighPtMuidMuonLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.ZmumuFilter.LeadingMuonPt,
                                                    muonRequireIsCombined             = True,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    # muonParametersList                = [ 19 ],
                                                    # muonParametersMinList             = [ -100000000.0 ],
                                                    # muonParametersMaxList             = [ primDPDsingleMu.ZmumuFilter.RelativeMuonIso ],
                                                    # muonParametersUseRelativeCutsList = [ True ],
                                                    muonRequireIsTight                = True
                                                    )
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_LowPtMuidMuonForZmumuFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'MuidMuonCollection',
                                                    outputLinkCollection              = 'DESD_SGLMUStream_ZmumuLowPtMuidMuonLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.ZmumuFilter.SubleadingMuonPt,
                                                    muonRequireIsCombined             = True,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    # muonParametersList                = [ 19 ],
                                                    # muonParametersMinList             = [ -100000000.0 ],
                                                    # muonParametersMaxList             = [ primDPDsingleMu.ZmumuFilter.RelativeMuonIso ],
                                                    # muonParametersUseRelativeCutsList = [ True ],
                                                    muonRequireIsTight                = True
                                                    )
        desdSingleMuonSequence += D2PDParticleCombiner( "SingleMuonStream_ZmumuMuidMuonFilter",
                                                        OutputLevel             = INFO,
                                                        compositeParticlePDG_ID = 23, # This is a Z Boson
                                                        inputCollectionList     = [ 'DESD_SGLMUStream_ZmumuHighPtMuidMuonLinkCollection',
                                                                                    'DESD_SGLMUStream_ZmumuLowPtMuidMuonLinkCollection' ],
                                                        outputCollection        = 'DESD_SGLMUStream_ZmumuWithMuidCollection',
                                                        chargeMin               = 0,
                                                        chargeMax               = 0,
                                                        massMin                 = primDPDsingleMu.ZmumuFilter.MassMin,
                                                        minNumberPassed         = 1
                                                        )
        zmumuAcceptAlgsList.append("SingleMuonStream_ZmumuMuidMuonFilter")
        pass
    
    
    # Define the prescaled filters; configurable with command line argument
    desdSingleMuonSequence += PrimaryDPDPrescaler( "SingleMuonStream_ZmumuPrescaleFilter",
                                                   AcceptAlgs = zmumuAcceptAlgsList,
                                                   Prescale   = primDPDsingleMu.ZmumuFilter.Prescale )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("SingleMuonStream_ZmumuPrescaleFilter")
    if commandstring=="":
        commandstring = "SingleMuonStream_ZmumuPrescaleFilter"
        pass
    else:
        commandstring = commandstring + " or SingleMuonStream_ZmumuPrescaleFilter"
        pass
    pass





# ------------------------
# Create the isolated single muon filters
# ------------------------
if primDPDsingleMu.IsolatedMuonFilter():
    # Create a list of accept algs for the filters
    isoAcceptAlgsList = []

    # Create a staco muon filter
    if hasStacoMuons:
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_IsolatedStacoMuonFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'StacoMuonCollection',
                                                    #outputLinkCollection              = 'DESD_SGLMUStream_IsolatedStacoMuonLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.IsolatedMuonFilter.MuonEt,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    muonParametersList                = [ 19 ],
                                                    muonParametersMinList             = [ -100000000.0 ],
                                                    muonParametersMaxList             = [ primDPDsingleMu.IsolatedMuonFilter.RelativeMuonIso ],
                                                    muonParametersUseRelativeCutsList = [ True ],
                                                    muonRequireIsLoose                = True
                                                    )
        isoAcceptAlgsList.append("SingleMuonStream_IsolatedStacoMuonFilter")
        pass
    
    # Check if we have the Muons chain collection available or not
    if hasMuons:
        muonDPDStream_msg.debug("Using Muons collection for IsolatedMuonFilter")
        
        # Create a Muons muon filter
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_IsolatedMuonFsilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'Muons',
                                                    #outputLinkCollection              = 'DESD_SGLMUStream_IsolatedMuonsLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.IsolatedMuonFilter.MuonEt,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    muonParametersList                = [ 19 ],
                                                    muonParametersMinList             = [ -100000000.0 ],
                                                    muonParametersMaxList             = [ primDPDsingleMu.IsolatedMuonFilter.RelativeMuonIso ],
                                                    muonParametersUseRelativeCutsList = [ True ],
                                                    muonRequireIsMedium              = True
                                                    )
        isoAcceptAlgsList.append("SingleMuonStream_IsolatedMuonFsilter")
        pass

    # Create a MuID muon filter
    if hasMuidMuons:
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_IsolatedMuidMuonFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'MuidMuonCollection',
                                                    #outputLinkCollection              = 'DESD_SGLMUStream_IsolatedMuidMuonLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.IsolatedMuonFilter.MuonEt,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    muonParametersList                = [ 19 ],
                                                    muonParametersMinList             = [ -100000000.0 ],
                                                    muonParametersMaxList             = [ primDPDsingleMu.IsolatedMuonFilter.RelativeMuonIso ],
                                                    muonParametersUseRelativeCutsList = [ True ],
                                                    muonRequireIsTight                = True
                                                    )
        isoAcceptAlgsList.append("SingleMuonStream_IsolatedMuidMuonFilter")
        pass
    
    # Define the prescaled filters; configurable with command line argument
    desdSingleMuonSequence += PrimaryDPDPrescaler( "SingleMuonStream_IsolatedMuonPrescaleFilter",
                                                   AcceptAlgs = isoAcceptAlgsList,
                                                   Prescale   = primDPDsingleMu.IsolatedMuonFilter.Prescale )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("SingleMuonStream_IsolatedMuonPrescaleFilter")
    if commandstring=="":
        commandstring = "SingleMuonStream_IsolatedMuonPrescaleFilter"
        pass
    else:
        commandstring = commandstring + " or SingleMuonStream_IsolatedMuonPrescaleFilter"
        pass
    pass

# ------------------------
# Create the high pT single muon filter
# ------------------------
if primDPDsingleMu.HighPtMuonFilter():
    # Create a list of accept algs for the filters
    highPtAcceptAlgsList = []

    # Create a staco muon filter
    if hasStacoMuons:
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_HighPtStacoMuonFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'StacoMuonCollection',
                                                    #outputLinkCollection              = 'DESD_SGLMUStream_HighPtStacoMuonLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.HighPtMuonFilter.MuonEt,
                                                    muonRequireIsLoose                = True
                                                    )
        highPtAcceptAlgsList.append("SingleMuonStream_HighPtStacoMuonFilter")
        pass
    
    # Check if we have the Muons chain collection available or not
    if hasMuons:
        muonDPDStream_msg.debug("Using Muons collection for HighPtMuonFilter")
        
        # Create a Muons muon filter
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_HighPtMuonsFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'Muons',
                                                    #outputLinkCollection              = 'DESD_SGLMUStream_HighPtMuonsLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.HighPtMuonFilter.MuonEt,
                                                    muonRequireIsMedium               = True
                                                    )
        highPtAcceptAlgsList.append("SingleMuonStream_HighPtMuonsFilter")
        pass
    
    # Create a MuID muon filter
    if hasMuidMuons:
        desdSingleMuonSequence += D2PDMuonSelector( "SingleMuonStream_HighPtMuidMuonFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'MuidMuonCollection',
                                                    #outputLinkCollection              = 'DESD_SGLMUStream_HighPtMuidMuonLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDsingleMu.HighPtMuonFilter.MuonEt,
                                                    muonRequireIsTight                = True
                                                    )
        highPtAcceptAlgsList.append("SingleMuonStream_HighPtMuidMuonFilter")
        pass
    
    # Define the prescaled filters; configurable with command line argument
    desdSingleMuonSequence += PrimaryDPDPrescaler( "SingleMuonStream_HighPtMuonPrescaleFilter",
                                                   AcceptAlgs  = highPtAcceptAlgsList,
                                                   Prescale = primDPDsingleMu.HighPtMuonFilter.Prescale )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("SingleMuonStream_HighPtMuonPrescaleFilter")
    if commandstring=="":
        commandstring = "SingleMuonStream_HighPtMuonPrescaleFilter"
        pass
    else:
        commandstring = commandstring + " or SingleMuonStream_HighPtMuonPrescaleFilter"
        pass
    pass

# ------------------------
# Create the J/psi trigger filters
# ------------------------
include("PrimaryDPDMaker/SetupTrigDecisionTool.py")
from PrimaryDPDMaker.TriggerFilter import TriggerFilter
if primDPDsingleMu.JpsiTriggerFilter():
    prescalefilternames = []
    for trigNamePrescale in primDPDsingleMu.JpsiTriggerFilter.TriggerNames :
        TriggerFilterName         = "SingleMuonStream_JpsiTriggerFilter_"+trigNamePrescale[0]
        TriggerPrescaleFilterName = "SingleMuonStream_JpsiTriggerPrescaleFilter_"+trigNamePrescale[0]
        prescalefilternames.append(TriggerPrescaleFilterName)
        desdSingleMuonSequence += TriggerFilter( TriggerFilterName,
                                                 trigger = trigNamePrescale[0]
                                                 )
        desdSingleMuonSequence += PrimaryDPDPrescaler( TriggerPrescaleFilterName,
                                                       AcceptAlgs = [ TriggerFilterName ],
                                                       Prescale   = trigNamePrescale[1]
                                                       )
        pass
    desdSingleMuonSequence += PrimaryDPDPrescaler( "SingleMuonStream_JpsiTriggerPrescaleFilter",
                                                   AcceptAlgs = prescalefilternames,
                                                   Prescale   = primDPDsingleMu.JpsiTriggerFilter.Prescale
                                                   )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("SingleMuonStream_JpsiTriggerPrescaleFilter")
    if commandstring=="":
        commandstring = "SingleMuonStream_JpsiTriggerPrescaleFilter"
        pass
    else:
        commandstring = commandstring + " or " + "SingleMuonStream_JpsiTriggerPrescaleFilter"
        pass
    pass





# Create the combined decision
desdSingleMuonSequence += LogicalFilterCombiner( "StreamDESD_DESD_SGLMU_AcceptEvent",
                                                 cmdstring = commandstring )
from PrimaryDPDMaker.PrimaryDPDFlags import desdEventSkimmingFilterNamesList
desdEventSkimmingFilterNamesList.append( "StreamDESD_DESD_SGLMU_AcceptEvent" )



##====================================================================
## Define the Single Muon DPD output stream
##====================================================================
streamName = primDPD.WriteSingleMuonStream.StreamName
fileName   = buildFileName( primDPD.WriteSingleMuonStream )
if primDPD.WriteSingleMuonStream.isVirtual or primDPD.isVirtual() :
    SingleMuonStream = MSMgr.NewVirtualStream( streamName, fileName )
    pass
else:
    SingleMuonStream = MSMgr.NewPoolStream( streamName, fileName )
    # Add the per-event bookkeeping 
    SingleMuonStream.AddOtherAlgsToBookkeep( algsToBookkeep )
    pass

if primDPD.ApplySkimming() and primDPDsingleMu.ApplySkimming() :
    SingleMuonStream.AcceptAlgs( [ "StreamDESD_DESD_SGLMU_AcceptEvent"] )
    pass



#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
#old way
#SingleMuonStream.Stream.TakeItemsFromInput = True
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take everything from the input
ExcludeList=[]

dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,ExcludeList)

#SingleMuonStream.AddItem( ["egammaContainer#ElectronAODCollection"] )


