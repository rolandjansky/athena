##-----------------------------------------------------------------------------
## Name: PrimaryDPD_TauMuonStream.py
##       (was PrimaryDPD_TightMuonStream.py)
##
## Author: David Lopez Mateos (Columbia University/Caltech)
## Email:  David.Lopez@cern.ch
##
## Description: This defines the content of the Tight Muon DPD output stream.
##
##-----------------------------------------------------------------------------

# If this file is already included, don't include it again
include.block("PrimaryDPDMaker/PerfDPD_TauMuon.py")


## for messaging
from AthenaCommon.Logging import logging
muonDPDStream_msg = logging.getLogger( 'PrimaryDPD_TauMuonStream' )

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

## Include the job property flags for this package
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from RecExConfig.RecFlags import rec
## Include the job property flags for this package
from PrimaryDPDMaker.PrimaryDPDFlags_TauMuonStream import primDPDtauMu

## This handels multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr



##====================================================================
## Write the used options to the log file
##====================================================================
if rec.OutputLevel() <= INFO:
    muonDPDStream_msg.info('Values of all PerfDPD_TauMuon flags:')
    print primDPDtauMu
    pass


# ============================================================================
# Create a sub-sequence where all needed algorithms will run in
# ============================================================================
import AthenaCommon.CfgMgr as CfgMgr
desdTauMuonSequence = CfgMgr.AthSequencer( 'DESD_TAUMUH_Sequence',
                                              StopOverride = True )
topSequence += desdTauMuonSequence


##============================================================================
## Define the skimming (event selection) for the DESD_TAUMUH output stream
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


from RecExConfig.ObjKeyStore import objKeyStore
muonDPDStream_msg.debug("Items in objKeyStore[transient]: %s" %  objKeyStore['transient'].list() )
#print "Items in objKeyStore[transient]:", objKeyStore['transient'].list()

# ------------------------
# Create the filters for the Z -> tau tau -> muon had analysis
# ------------------------
if primDPDtauMu.TauForZtautauFilter() :
    # Create a list of accept algs for the tau filters
    zTauTauAcceptAlgsList = []

    # trigger requirement
    include("PrimaryDPDMaker/SetupTrigDecisionTool.py")
    from PrimaryDPDMaker.TriggerFilter import TriggerFilter
    
    TriggerFilterName         = "TauMuonStream_ZtautauTriggerFilter_"+primDPDtauMu.TauForZtautauFilter.TriggerNames
    desdTauMuonSequence   += TriggerFilter( TriggerFilterName,
                                               trigger = primDPDtauMu.TauForZtautauFilter.TriggerNames
                                               )
    desdTauMuonSequence += PrimaryDPDPrescaler( "TauMuonStream_ZtautauTriggerPrescaleFilter",
                                                   AcceptAlgs = [ TriggerFilterName ],
                                                   Prescale   = 1
                                                   )
    
    commandstring = "TauMuonStream_ZtautauTriggerPrescaleFilter"
    
    # Create a Staco muon filter
    from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
    desdTauMuonSequence += D2PDMuonSelector( "TauMuonStream_StacoMuonForZtautauFilter",
                                                OutputLevel                       = INFO,
                                                inputCollection                   = 'StacoMuonCollection',
                                                outputLinkCollection              = 'DESD_TAUMUHStream_StacoMuonForZtautauLinkCollection',
                                                minNumberPassed                   = 1,
                                                ptMin                             = primDPDtauMu.TauForZtautauFilter.MuonEt,
                                                ptMax                             = primDPDtauMu.TauForZtautauFilter.MuonEtMax,
                                                #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                muonParametersList                = [ 19, 1 ],
                                                muonParametersMinList             = [ -100000000.0 , -100000000.0 ],
                                                muonParametersMaxList             = [ primDPDtauMu.TauForZtautauFilter.RelativeMuonIso,
                                                                                      primDPDtauMu.TauForZtautauFilter.RelativeMuonEtIso ],
                                                muonParametersUseRelativeCutsList = [ True, True ],
                                                muonRequireIsLoose                = True
                                                )
    # Set up a DeltaR association/matching tool
    from AssociationComps.AssociationCompsConf import DeltaRAssociationTool
    ToolSvc += DeltaRAssociationTool( "TauMuonStream_TauStacoMuonDeltaRAssociationTool",
                                      OutputLevel                = INFO,
                                      inputAssociateToCollection = "DESD_TAUMUHStream_StacoMuonForZtautauLinkCollection",
                                      deltaRMax                  = 0.4,
                                      writeUserData              = False
                                      )
    # Set up the tau filter
    from D2PDMaker.D2PDMakerConf import D2PDTauSelector
    desdTauMuonSequence += D2PDTauSelector( "TauMuonStream_TauForStacoMuonFilter",
                                               OutputLevel                    = INFO,
                                               inputCollection                = 'TauRecContainer',
                                               #outputLinkCollection           = 'DESD_TAUMUHStream_TauForStacoMuonLinkCollection',
                                               minNumberPassed                = 1,
                                               associationToolList            = [ ToolSvc.TauMuonStream_TauStacoMuonDeltaRAssociationTool ],
                                               outputAssociationContainerList = [ "DESD_TAUMUHStream_TauToStacoMuonAssociations" ],
                                               numberOfAssociationsMaxCutList = [ 0 ],
                                               minNumberTracks                = primDPDtauMu.TauForZtautauFilter.TauNTrackMin,
                                               maxNumberTracks                = primDPDtauMu.TauForZtautauFilter.TauNTrackMax,
                                               tauDiscriminant                = primDPDtauMu.TauForZtautauFilter.TauDiscriminant,
                                               tauDiscriminantMin             = primDPDtauMu.TauForZtautauFilter.TauDiscriminantMin,
                                               etMin                          = primDPDtauMu.TauForZtautauFilter.TauEt
                                               )

    # Staco + Tau
    desdTauMuonSequence += PrimaryDPDPrescaler( "TauMuonStream_TauStacoMuonPrescaleFilter",
                                                   OutputLevel = INFO,
                                                   RequireAlgs = [ "TauMuonStream_StacoMuonForZtautauFilter",
                                                                   "TauMuonStream_TauForStacoMuonFilter" ],
                                                   Prescale    = 1 )
    zTauTauAcceptAlgsList.append("TauMuonStream_TauStacoMuonPrescaleFilter")
    
    # Check if we have the Muons chain collection available or not
    from RecExConfig.ObjKeyStore import objKeyStore
    if objKeyStore.isInInput("Analysis::MuonContainer", "Muons") \
           or objKeyStore['transient'].has_item("Analysis::MuonContainer#Muons"):
        muonDPDStream_msg.debug("Using Muons collection for TauForZtautauFilter")
        
        # Create a Muons muon filter
        from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
        desdTauMuonSequence += D2PDMuonSelector( "TauMuonStream_MuonsForZtautauFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'Muons',
                                                    outputLinkCollection              = 'DESD_TAUMUHStream_MuonsForZtautauLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDtauMu.TauForZtautauFilter.MuonEt,
                                                    ptMax                             = primDPDtauMu.TauForZtautauFilter.MuonEtMax,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    muonParametersList                = [ 19, 1 ],
                                                    muonParametersMinList             = [ -100000000.0 , -100000000.0 ],
                                                    muonParametersMaxList             = [ primDPDtauMu.TauForZtautauFilter.RelativeMuonIso,
                                                                                          primDPDtauMu.TauForZtautauFilter.RelativeMuonEtIso ],
                                                    muonParametersUseRelativeCutsList = [ True, True ],
                                                    muonRequireIsMedium                = True
                                                    )
        
        # Set up a DeltaR association/matching tool
        from AssociationComps.AssociationCompsConf import DeltaRAssociationTool
        ToolSvc += DeltaRAssociationTool( "TauMuonStream_TauMuonsDeltaRAssociationTool",
                                          OutputLevel                = INFO,
                                          inputAssociateToCollection = "DESD_TAUMUHStream_MuonsForZtautauLinkCollection",
                                          deltaRMax                  = 0.4,
                                          writeUserData              = False
                                          )
        # Set up the tau filter
        from D2PDMaker.D2PDMakerConf import D2PDTauSelector
        desdTauMuonSequence += D2PDTauSelector( "TauMuonStream_TauForMuonsFilter",
                                                   OutputLevel                    = INFO,
                                                   inputCollection                = 'TauRecContainer',
                                                   #outputLinkCollection           = 'DESD_TAUMUHStream_TauForMuonsLinkCollection',
                                                   minNumberPassed                = 1,
                                                   associationToolList            = [ ToolSvc.TauMuonStream_TauMuonsDeltaRAssociationTool ],
                                                   outputAssociationContainerList = [ "DESD_TAUMUHStream_TauToMuonsAssociations" ],
                                                   numberOfAssociationsMaxCutList = [ 0 ],
                                                   minNumberTracks                = primDPDtauMu.TauForZtautauFilter.TauNTrackMin,
                                                   maxNumberTracks                = primDPDtauMu.TauForZtautauFilter.TauNTrackMax,
                                                   tauDiscriminant              =primDPDtauMu.TauForZtautauFilter.TauDiscriminant,
                                                   tauDiscriminantMin            =primDPDtauMu.TauForZtautauFilter.TauDiscriminantMin,
                                                   etMin                          = primDPDtauMu.TauForZtautauFilter.TauEt
                                                   )

        # Muons + Tau
        desdTauMuonSequence += PrimaryDPDPrescaler( "TauMuonStream_TauMuonsPrescaleFilter",
                                                       OutputLevel = INFO,
                                                       RequireAlgs = [ "TauMuonStream_MuonsForZtautauFilter",
                                                                       "TauMuonStream_TauForMuonsFilter" ],
                                                       Prescale    = 1 )
        zTauTauAcceptAlgsList.append("TauMuonStream_TauMuonsPrescaleFilter")
        pass
    
    # Create a MuID muon filter
    from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
    desdTauMuonSequence += D2PDMuonSelector( "TauMuonStream_MuidMuonForZtautauFilter",
                                                OutputLevel                       = INFO,
                                                inputCollection                   = 'MuidMuonCollection',
                                                outputLinkCollection              = 'DESD_TAUMUHLStream_MuidMuonForZtautauLinkCollection',
                                                minNumberPassed                   = 1,
                                                ptMin                             = primDPDtauMu.TauForZtautauFilter.MuonEt,
                                                ptMax                             = primDPDtauMu.TauForZtautauFilter.MuonEtMax,
                                                #muonParametersList                = [ MuonParameters.ptcone40 ],
                                                muonParametersList                = [ 19, 1 ],
                                                muonParametersMinList             = [ -100000000.0, -100000000.0 ],
                                                muonParametersMaxList             = [ primDPDtauMu.TauForZtautauFilter.RelativeMuonIso,
                                                                                      primDPDtauMu.TauForZtautauFilter.RelativeMuonEtIso ],
                                                muonParametersUseRelativeCutsList = [ True, True ],
                                                muonRequireIsTight                = True
                                                )
    # Set up a DeltaR association/matching tool
    from AssociationComps.AssociationCompsConf import DeltaRAssociationTool
    ToolSvc += DeltaRAssociationTool( "TauMuonStream_TauMuidMuonDeltaRAssociationTool",
                                      OutputLevel                = INFO,
                                      inputAssociateToCollection = "DESD_TAUMUHLStream_MuidMuonForZtautauLinkCollection",
                                      deltaRMax                  = 0.4,
                                      writeUserData              = False
                                      )
    # Set up the tau filter
    from D2PDMaker.D2PDMakerConf import D2PDTauSelector
    desdTauMuonSequence += D2PDTauSelector( "TauMuonStream_TauForMuidMuonFilter",
                                               OutputLevel                    = INFO,
                                               inputCollection                = 'TauRecContainer',
                                               #outputLinkCollection           = 'DESD_TAUMUHStream_TauForMuidMuonLinkCollection',
                                               minNumberPassed                = 1,
                                               associationToolList            = [ ToolSvc.TauMuonStream_TauMuidMuonDeltaRAssociationTool ],
                                               outputAssociationContainerList = [ "DESD_TAUMUHStream_TauToMuidMuonAssociations" ],
                                               numberOfAssociationsMaxCutList = [ 0 ],
                                               minNumberTracks                = primDPDtauMu.TauForZtautauFilter.TauNTrackMin,
                                               maxNumberTracks                = primDPDtauMu.TauForZtautauFilter.TauNTrackMax,
                                               tauDiscriminant              =primDPDtauMu.TauForZtautauFilter.TauDiscriminant,
                                               tauDiscriminantMin            =primDPDtauMu.TauForZtautauFilter.TauDiscriminantMin,
                                               etMin                          = primDPDtauMu.TauForZtautauFilter.TauEt
                                               )
    # Muid + Tau
    desdTauMuonSequence += PrimaryDPDPrescaler( "TauMuonStream_TauMuidMuonPrescaleFilter",
                                                   OutputLevel = INFO,
                                                   RequireAlgs = [ "TauMuonStream_MuidMuonForZtautauFilter",
                                                                   "TauMuonStream_TauForMuidMuonFilter" ],
                                                   Prescale    = 1 )
    zTauTauAcceptAlgsList.append("TauMuonStream_TauMuidMuonPrescaleFilter")

    # Create the combined decision for the Z -> tau tau
    desdTauMuonSequence += PrimaryDPDPrescaler( "TauMuonStream_TauMuonPrescaleFilter",
                                                   OutputLevel = INFO,
                                                   AcceptAlgs  = zTauTauAcceptAlgsList,
                                                   Prescale    = primDPDtauMu.TauForZtautauFilter.Prescale )
    # If the other tau filter is not scheduled, add only this one. Otherwise, add a combination below
    if not primDPDtauMu.MuonTauLooseFilter() :
        if commandstring=="":
            commandstring = "TauMuonStream_TauMuonPrescaleFilter"
            pass
        else:
            commandstring = commandstring + " and TauMuonStream_TauMuonPrescaleFilter"
            pass
        pass
    pass


# ------------------------
# Create the filters for the Z -> tau tau (loose, ie low pT) -> muon had analysis
# ------------------------
if primDPDtauMu.MuonTauLooseFilter() :
    # Create a list of accept algs for the tau filters
    tauAcceptAlgsList = []
    
    # Create a Staco muon filter
    from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
    desdTauMuonSequence += D2PDMuonSelector( "TauMuonStream_StacoMuonForMuonTauLooseFilter",
                                                OutputLevel                       = INFO,
                                                inputCollection                   = 'StacoMuonCollection',
                                                outputLinkCollection              = 'DESD_TAUMUHStream_StacoMuonForMuonTauLooseLinkCollection',
                                                minNumberPassed                   = 1,
                                                ptMin                             = primDPDtauMu.MuonTauLooseFilter.MuonEt,
                                                ptMax                             = primDPDtauMu.MuonTauLooseFilter.MuonEtMax,
                                                #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                muonParametersList                = [ 19, 1 ],
                                                muonParametersMinList             = [ -100000000.0, -100000000.0 ],
                                                muonParametersMaxList             = [ primDPDtauMu.MuonTauLooseFilter.RelativeMuonIso,
                                                                                      primDPDtauMu.MuonTauLooseFilter.RelativeMuonEtIso],
                                                muonParametersUseRelativeCutsList = [ True, True ],
                                                muonRequireIsLoose                = True
                                                )

    # Set up a DeltaR association/matching tool
    from AssociationComps.AssociationCompsConf import DeltaRAssociationTool
    ToolSvc += DeltaRAssociationTool( "TauMuonStream_TauLooseStacoMuonDeltaRAssociationTool",
                                      OutputLevel                = INFO,
                                      inputAssociateToCollection = "DESD_TAUMUHStream_StacoMuonForMuonTauLooseLinkCollection",
                                      deltaRMax                  = 0.4,
                                      writeUserData              = False
                                      )

    # Set up the tau filter
    from D2PDMaker.D2PDMakerConf import D2PDTauSelector
    desdTauMuonSequence += D2PDTauSelector( "TauMuonStream_TauLooseForStacoMuonFilter",
                                               OutputLevel                    = INFO,
                                               inputCollection                = 'TauRecContainer',
                                               outputLinkCollection           = 'DESD_TAUMUHStream_TauLooseForStacoMuonLinkCollection',
                                               minNumberPassed                = 1,
                                               associationToolList            = [ ToolSvc.TauMuonStream_TauLooseStacoMuonDeltaRAssociationTool ],
                                               outputAssociationContainerList = [ "DESD_TAUMUHStream_TauLooseToStacoMuonAssociations" ],
                                               numberOfAssociationsMaxCutList = [ 0 ],
                                               minNumberTracks                = primDPDtauMu.MuonTauLooseFilter.TauNTrackMin,
                                               maxNumberTracks                = primDPDtauMu.MuonTauLooseFilter.TauNTrackMax,
                                               tauDiscriminant              =primDPDtauMu.TauForZtautauFilter.TauDiscriminant,
                                               tauDiscriminantMin            =primDPDtauMu.TauForZtautauFilter.TauDiscriminantMin,
                                               etMin                          = primDPDtauMu.MuonTauLooseFilter.TauEt
                                               #etMax                          = primDPDtauMu.MuonTauLooseFilter.TauEtBreak
                                               )
    
    desdTauMuonSequence += PrimaryDPDPrescaler( "TauMuonStream_TauLooseStacoMuonPrescaleFilter",
                                                   OutputLevel = INFO,
                                                   RequireAlgs = [ "TauMuonStream_StacoMuonForMuonTauLooseFilter",
                                                                   "TauMuonStream_TauLooseForStacoMuonFilter" ],
                                                   Prescale    = 1 )
    #tauAcceptAlgsList.append("TauMuonStream_TauLooseStacoMuonPrescaleFilter")

    # Define the combination of Staco muon and tau
    from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner
    desdTauMuonSequence += D2PDParticleCombiner( "TauMuonStream_ZtauStacomuFilter",
                                                    OutputLevel             = INFO,
                                                    compositeParticlePDG_ID = 23, # This is a Z Boson
                                                    inputCollectionList     = [ 'DESD_TAUMUHStream_StacoMuonForMuonTauLooseLinkCollection',
                                                                                'DESD_TAUMUHStream_TauLooseForStacoMuonLinkCollection' ],
                                                    outputCollection        = 'DESD_TAUMUHStream_ZtauStacomuCollection',
                                                    chargeMin               = -1,
                                                    chargeMax               = 1,
                                                    deltaPhiMin             = 2.2,
                                                    minNumberPassed         = 1
                                                    )
    tauAcceptAlgsList.append("TauMuonStream_ZtauStacomuFilter")

    # Check if we have the Muons chain collection available or not
    from RecExConfig.ObjKeyStore import objKeyStore
    if objKeyStore.isInInput("Analysis::MuonContainer", "Muons") \
           or objKeyStore['transient'].has_item("Analysis::MuonContainer#Muons"):
        muonDPDStream_msg.debug("Using Muons collection for MuonTauLooseFilter")
        
        # Create a Muons muon filter
        from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
        desdTauMuonSequence += D2PDMuonSelector( "TauMuonStream_MuonsForMuonTauLooseFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'Muons',
                                                    outputLinkCollection              = 'DESD_TAUMUHStream_MuonsForMuonTauLooseLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDtauMu.MuonTauLooseFilter.MuonEt,
                                                    ptMax                             = primDPDtauMu.MuonTauLooseFilter.MuonEtMax,
                                                    #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                    muonParametersList                = [ 19, 1 ],
                                                    muonParametersMinList             = [ -100000000.0, -100000000.0 ],
                                                    muonParametersMaxList             = [ primDPDtauMu.MuonTauLooseFilter.RelativeMuonIso,
                                                                                          primDPDtauMu.MuonTauLooseFilter.RelativeMuonEtIso],
                                                    muonParametersUseRelativeCutsList = [ True, True ],
                                                    muonRequireIsMedium                = True
                                                    )
        
        # Set up a DeltaR association/matching tool
        from AssociationComps.AssociationCompsConf import DeltaRAssociationTool
        ToolSvc += DeltaRAssociationTool( "TauMuonStream_TauLooseMuonsDeltaRAssociationTool",
                                          OutputLevel                = INFO,
                                          inputAssociateToCollection = "DESD_TAUMUHStream_MuonsForMuonTauLooseLinkCollection",
                                          deltaRMax                  = 0.4,
                                          writeUserData              = False
                                          )

        # Set up the tau filter
        from D2PDMaker.D2PDMakerConf import D2PDTauSelector
        desdTauMuonSequence += D2PDTauSelector( "TauMuonStream_TauLooseForMuonsFilter",
                                                   OutputLevel                    = INFO,
                                                   inputCollection                = 'TauRecContainer',
                                                   outputLinkCollection           = 'DESD_TAUMUHStream_TauLooseForMuonsLinkCollection',
                                                   minNumberPassed                = 1,
                                                   associationToolList            = [ ToolSvc.TauMuonStream_TauLooseMuonsDeltaRAssociationTool ],
                                                   outputAssociationContainerList = [ "DESD_TAUMUHStream_TauLooseToMuonsAssociations" ],
                                                   numberOfAssociationsMaxCutList = [ 0 ],
                                                   minNumberTracks                = primDPDtauMu.MuonTauLooseFilter.TauNTrackMin,
                                                   maxNumberTracks                = primDPDtauMu.MuonTauLooseFilter.TauNTrackMax,
                                                   tauDiscriminant              =primDPDtauMu.TauForZtautauFilter.TauDiscriminant,
                                                   tauDiscriminantMin            =primDPDtauMu.TauForZtautauFilter.TauDiscriminantMin,
                                                   etMin                          = primDPDtauMu.MuonTauLooseFilter.TauEt
                                                   #etMax                          = primDPDtauMu.MuonTauLooseFilter.TauEtBreak
                                                   )
        

        # Define the combination of muon and tau
        from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner
        desdTauMuonSequence += D2PDParticleCombiner( "TauMuonStream_ZtaumuFilter",
                                                        OutputLevel             = INFO,
                                                        compositeParticlePDG_ID = 23, # This is a Z Boson
                                                        inputCollectionList     = [ 'DESD_TAUMUHStream_TauLooseForMuonsLinkCollection',
                                                                                    'DESD_TAUMUHStream_MuonsForMuonTauLooseLinkCollection' ],
                                                        outputCollection        = 'DESD_TAUMUHStream_ZtaumuCollection',
                                                        chargeMin               = -1,
                                                        chargeMax               = 1,
                                                        deltaPhiMin             = 2.2,
                                                        minNumberPassed         = 1
                                                        )
        tauAcceptAlgsList.append("TauMuonStream_ZtaumuFilter")

        # Create a MuID muon filter
        from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
        desdTauMuonSequence += D2PDMuonSelector( "TauMuonStream_MuidMuonForMuonTauLooseFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'MuidMuonCollection',
                                                    outputLinkCollection              = 'DESD_TAUMUHLStream_MuidMuonForMuonTauLooseLinkCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDtauMu.MuonTauLooseFilter.MuonEt,
                                                    ptMax                             = primDPDtauMu.MuonTauLooseFilter.MuonEtMax,
                                                    #muonParametersList                = [ MuonParameters.ptcone40 ],
                                                    muonParametersList                = [ 19, 1 ],
                                                    muonParametersMinList             = [ -100000000.0, -100000000.0 ],
                                                    muonParametersMaxList             = [ primDPDtauMu.MuonTauLooseFilter.RelativeMuonIso,
                                                                                          primDPDtauMu.MuonTauLooseFilter.RelativeMuonEtIso ],
                                                    muonParametersUseRelativeCutsList = [ True, True ],
                                                    muonRequireIsTight                = True
                                                    )

        # Set up a DeltaR association/matching tool
        from AssociationComps.AssociationCompsConf import DeltaRAssociationTool
        ToolSvc += DeltaRAssociationTool( "TauMuonStream_TauLooseMuidMuonDeltaRAssociationTool",
                                          OutputLevel                = INFO,
                                          inputAssociateToCollection = "DESD_TAUMUHLStream_MuidMuonForMuonTauLooseLinkCollection",
                                          deltaRMax                  = 0.4,
                                          writeUserData              = False
                                          )
        # Set up the tau filter
        from D2PDMaker.D2PDMakerConf import D2PDTauSelector
        desdTauMuonSequence += D2PDTauSelector( "TauMuonStream_TauLooseForMuidMuonFilter",
                                                   OutputLevel                    = INFO,
                                                   inputCollection                = 'TauRecContainer',
                                                   outputLinkCollection           = 'DESD_TAUMUHStream_TauLooseForMuidMuonLinkCollection',
                                                   minNumberPassed                = 1,
                                                   associationToolList            = [ ToolSvc.TauMuonStream_TauLooseMuidMuonDeltaRAssociationTool ],
                                                   outputAssociationContainerList = [ "DESD_TAUMUHStream_TauLooseToMuidMuonAssociations" ],
                                                   numberOfAssociationsMaxCutList = [ 0 ],
                                                   minNumberTracks                = primDPDtauMu.MuonTauLooseFilter.TauNTrackMin,
                                                   maxNumberTracks                = primDPDtauMu.MuonTauLooseFilter.TauNTrackMax,
                                                   tauDiscriminant                = primDPDtauMu.TauForZtautauFilter.TauDiscriminant,
                                                   tauDiscriminantMin             = primDPDtauMu.TauForZtautauFilter.TauDiscriminantMin,
                                                   etMin                          = primDPDtauMu.MuonTauLooseFilter.TauEt
                                                   )
    
        desdTauMuonSequence += PrimaryDPDPrescaler( "TauMuonStream_TauLooseMuidMuonPrescaleFilter",
                                                       OutputLevel = INFO,
                                                       RequireAlgs = [ "TauMuonStream_MuidMuonForMuonTauLooseFilter",
                                                                       "TauMuonStream_TauLooseForMuidMuonFilter" ],
                                                       Prescale    = 1 )
        #tauAcceptAlgsList.append("TauMuonStream_TauLooseMuidMuonPrescaleFilter")
        
        # Define the combination of muon and tau
        from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner
        desdTauMuonSequence += D2PDParticleCombiner( "TauMuonStream_ZtauMuidmuFilter",
                                                        OutputLevel             = INFO,
                                                        compositeParticlePDG_ID = 23, # This is a Z Boson
                                                        inputCollectionList     = [ 'DESD_TAUMUHStream_TauLooseForMuidMuonLinkCollection',
                                                                                    'DESD_TAUMUHLStream_MuidMuonForMuonTauLooseLinkCollection' ],
                                                        outputCollection        = 'DESD_TAUMUHStream_ZtauMuidmuCollection',
                                                        chargeMin               = -1,
                                                        chargeMax               = 1,
                                                        deltaPhiMin             = 2.2,
                                                        minNumberPassed         = 1
                                                        )
        tauAcceptAlgsList.append("TauMuonStream_ZtauMuidmuFilter")

        # Create the combined decision for the Z -> tau tau
        desdTauMuonSequence += PrimaryDPDPrescaler( "TauMuonStream_TauLooseMuonPrescaleFilter",
                                                       OutputLevel = INFO,
                                                       AcceptAlgs  = tauAcceptAlgsList,
                                                       Prescale    = primDPDtauMu.MuonTauLooseFilter.Prescale )
        
        # If the other tau filter is not scheduled, add only this one. Otherwise, add a combination below
        if not primDPDtauMu.TauForZtautauFilter() :
            if commandstring=="":
                commandstring = "TauMuonStream_TauLooseMuonPrescaleFilter"
                pass
            else:
                commandstring = commandstring + " or TauMuonStream_TauLooseMuonPrescaleFilter"
                pass
        pass
    pass


# Define the combination of the tau filters for bookkeeping reasons
if primDPDtauMu.MuonTauLooseFilter() and  primDPDtauMu.TauForZtautauFilter() :
    desdTauMuonSequence += PrimaryDPDPrescaler( "TauMuonStream_TauCombinedPrescaleFilter",
                                                   OutputLevel = INFO,
                                                   AcceptAlgs = [ "TauMuonStream_TauMuonPrescaleFilter",
                                                                  "TauMuonStream_TauLooseMuonPrescaleFilter" ],
                                                   Prescale    = 1 )
    
    if commandstring=="":
        commandstring = "TauMuonStream_TauCombinedPrescaleFilter"
        pass
    else:
        commandstring = commandstring + " or TauMuonStream_TauCombinedPrescaleFilter"
        pass
    pass


# Create the combined decision
desdTauMuonSequence += LogicalFilterCombiner( "StreamDESD_DESD_TAUMUH_AcceptEvent",
                                                 cmdstring = commandstring )
from PrimaryDPDMaker.PrimaryDPDFlags import desdEventSkimmingFilterNamesList
desdEventSkimmingFilterNamesList.append( "StreamDESD_DESD_TAUMUH_AcceptEvent" )



##====================================================================
## Define the Tau Muon DPD output stream
##====================================================================
streamName = primDPD.WriteTauMuonStream.StreamName
fileName   = buildFileName( primDPD.WriteTauMuonStream )
if primDPD.WriteTauMuonStream.isVirtual or primDPD.isVirtual() :
    TauMuonStream=MSMgr.NewVirtualStream( streamName, fileName )
    pass
else:
    TauMuonStream=MSMgr.NewPoolStream( streamName, fileName )
    pass

if primDPD.ApplySkimming() and primDPDtauMu.ApplySkimming() :
    TauMuonStream.AcceptAlgs( [ "StreamDESD_DESD_TAUMUH_AcceptEvent"] )
    pass



#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
#old way
#TauMuonStream.Stream.TakeItemsFromInput = True
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take everything from the input
ExcludeList=[]

dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,ExcludeList)
#TauMuonStream.AddItem( ["egammaContainer#ElectronAODCollection"] )

