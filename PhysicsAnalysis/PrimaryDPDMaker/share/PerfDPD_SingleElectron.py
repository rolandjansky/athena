##-----------------------------------------------------------------------------
## Name: PrimaryDPD_MissingEtStream.py
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
##
## Description: This defines the content of the Missing Et DPD output stream.
##
##-----------------------------------------------------------------------------

# If this file is already included, don't include it again
include.block("PrimaryDPDMaker/PerfDPD_SingleElectron.py")


## for messaging
from AthenaCommon.Logging import logging
singleelectronDPDStream_msg = logging.getLogger( 'PrimaryDPD_PerfSingleElectronStream' )

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

## Include the job property flags for this package and from RecExCommon
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags_SingleElectronStream import primSingleElectronDPD
from RecExConfig.RecFlags import rec

## This handels multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr


##====================================================================
## Write the used options to the log file
##====================================================================
if rec.OutputLevel() <= INFO:
    singleelectronDPDStream_msg.info('Values of all PrimaryDPDFlags_SingleElectronStream flags:')    
    print primSingleElectronDPD
    pass



# ---- Load the egammaPID and egammaParameters information
# This is needed to always be up-to-date with the egamma
# IsEM selections and also the author selections
import PyUtils.RootUtils as ru
ROOT = ru.import_root()
import PyCintex
PyCintex.loadDictionary('egammaEnumsDict')
from ROOT import egammaPID
from ROOT import egammaParameters




# ============================================================================
# Create a sub-sequence where all needed algorithms will run in
# ============================================================================
import AthenaCommon.CfgMgr as CfgMgr
desdSingleElectronSequence = CfgMgr.AthSequencer( 'DESD_SGLEL_Sequence',
                                                  StopOverride = True )
topSequence += desdSingleElectronSequence



##============================================================================
## Define the skimming (event selection) for the DESD_SGLEL output stream
##============================================================================
from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner
from PrimaryDPDMaker.PrimaryDPDMakerConf   import PrimaryDPDPrescaler
from AssociationComps.AssociationCompsConf import DeltaRAssociationTool
from D2PDMaker.D2PDMakerConf import D2PDElectronSelector
from D2PDMaker.D2PDMakerConf import D2PDMissingEtSelector
from D2PDMaker.D2PDMakerConf import D2PDJetSelector
from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner
from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
from D2PDMaker.D2PDMakerConf import D2PDTauSelector

# Create the command string for the final decision
sglElCMDString = ""
algsToBookkeep = []


# Main single electron filter
if primSingleElectronDPD.ElectronFilter():
    desdSingleElectronSequence += D2PDElectronSelector( "SingleElectronStream_ElectronFilter",
                                                        OutputLevel            = INFO,
                                                        inputCollection        = 'ElectronAODCollection',
                                                        minNumberPassed        = 1,
                                                        electronVetoAuthorList = primSingleElectronDPD.ElectronFilter.VetoElectronAuthor,
                                                        electronID           = primSingleElectronDPD.ElectronFilter.ElectronIsEM,
                                                        egammaTrackIsolationParameter = primSingleElectronDPD.ElectronFilter.ElectronPtCone,
                                                        relativeTrackPtIsolationMax   = primSingleElectronDPD.ElectronFilter.ElectronPtConeMax,
                                                        clusterEtMin                  = primSingleElectronDPD.ElectronFilter.ElectronEt
                                                        )
    
    # Define the prescaled filters; configurable with command line argument
    desdSingleElectronSequence += PrimaryDPDPrescaler( "SingleElectronStream_ElectronPrescaleFilter",
                                                       AcceptAlgs = ["SingleElectronStream_ElectronFilter"],
                                                       Prescale = primSingleElectronDPD.ElectronFilter.Prescale )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("SingleElectronStream_ElectronPrescaleFilter")
    sglElCMDString += "SingleElectronStream_ElectronPrescaleFilter"
    pass




if sglElCMDString != "" and not sglElCMDString.endswith(" or "):
    sglElCMDString += " or "
    pass



# High ET single electron filter
if primSingleElectronDPD.HighEtElectronFilter() :
    desdSingleElectronSequence += D2PDElectronSelector( "SingleElectronStream_HighEtElectronFilter",
                                                        OutputLevel            = INFO,
                                                        inputCollection        = 'ElectronAODCollection',
                                                        minNumberPassed        = 1,
                                                        electronVetoAuthorList = primSingleElectronDPD.HighEtElectronFilter.VetoElectronAuthor,
                                                        electronID           = primSingleElectronDPD.HighEtElectronFilter.ElectronIsEM,
                                                        clusterEtMin           = primSingleElectronDPD.HighEtElectronFilter.ElectronEt
                                                        )
    
    # Define the prescaled filters; configurable with command line argument
    desdSingleElectronSequence += PrimaryDPDPrescaler( "SingleElectronStream_HighEtElectronPrescaleFilter",
                                                       AcceptAlgs = ["SingleElectronStream_HighEtElectronFilter"],
                                                       Prescale = primSingleElectronDPD.HighEtElectronFilter.Prescale )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("SingleElectronStream_HighEtElectronPrescaleFilter")
    sglElCMDString += "SingleElectronStream_HighEtElectronPrescaleFilter"
    pass




if sglElCMDString != "" and not sglElCMDString.endswith(" or "):
    sglElCMDString += " or "
    pass




# Disappearing electron filter
# if primSingleElectronDPD.DisappearingElectronFilter():

#     from PrimaryDPDMaker.PrimaryDPDMakerConf import DisappearingEleFilter

#     desdSingleElectronSequence += DisappearingEleFilter("SingleElectronStream_DisappearingEleFilters")

#     desdSingleElectronSequence.SingleElectronStream_DisappearingEleFilters.ElectronEtCut  = primSingleElectronDPD.DisappearingElectronFilter.ElectronEtCut
#     desdSingleElectronSequence.SingleElectronStream_DisappearingEleFilters.electronEtaCut = primSingleElectronDPD.DisappearingElectronFilter.electronEtaCut
#     desdSingleElectronSequence.SingleElectronStream_DisappearingEleFilters.MissingETCut   = primSingleElectronDPD.DisappearingElectronFilter.MissingETCut

#     # Define the prescaled filters; configurable with command line argument
#     desdSingleElectronSequence += PrimaryDPDPrescaler( "SingleElectronStream_DisappearingElectronPrescaleFilter",
#                                                        RequireAlgs = ["SingleElectronStream_DisappearingEleFilters"],
#                                                        Prescale = primSingleElectronDPD.DisappearingElectronFilter.Prescale )

#     # Add the per-event bookkeeping of this filer
#     algsToBookkeep.append("SingleElectronStream_DisappearingElectronPrescaleFilter")
#     sglElCMDString += "SingleElectronStream_DisappearingElectronPrescaleFilter"
#     pass





if sglElCMDString != "" and not sglElCMDString.endswith(" or "):
    sglElCMDString += " or "
    pass





# Define the electron selection for the Zee filter
if primSingleElectronDPD.ZeeFilter():
    desdSingleElectronSequence += D2PDElectronSelector( "SingleElectronStream_ZeeElectron1Filter",
                                         OutputLevel            = INFO,
                                         inputCollection        = 'ElectronAODCollection',
                                         outputLinkCollection   = 'DESD_SGLELStream_ZeeElectron1LinkCollection',
                                         minNumberPassed        = 1,
                                         electronVetoAuthorList = primSingleElectronDPD.ZeeFilter.VetoElectron1Author,
                                         electronID           = primSingleElectronDPD.ZeeFilter.Electron1IsEM,
                                         clusterEtMin           = primSingleElectronDPD.ZeeFilter.Electron1Et
                                         )
    desdSingleElectronSequence += D2PDElectronSelector( "SingleElectronStream_ZeeElectron2Filter",
                                         OutputLevel            = INFO,
                                         inputCollection        = 'ElectronAODCollection',
                                         outputLinkCollection   = 'DESD_SGLELStream_ZeeElectron2LinkCollection',
                                         minNumberPassed        = 1,
                                         electronVetoAuthorList = primSingleElectronDPD.ZeeFilter.VetoElectron2Author,
                                         electronID           = primSingleElectronDPD.ZeeFilter.Electron2IsEM,
                                         clusterEtMin           = primSingleElectronDPD.ZeeFilter.Electron2Et
                                         )
    
    # Define the Z -> ee filter
    desdSingleElectronSequence += D2PDParticleCombiner( "SingleElectronStream_ZeeFilter",
                                                        OutputLevel             = INFO,
                                                        compositeParticlePDG_ID = 23, # This is a Z Boson
                                                        inputCollectionList     = [ 'DESD_SGLELStream_ZeeElectron1LinkCollection',
                                                                                    'DESD_SGLELStream_ZeeElectron2LinkCollection' ],
                                                        outputCollection        = 'DESD_SGLELStream_ZeeBosonCollection',
                                                        minNumberPassed         = 1,
                                                        massMin                 = primSingleElectronDPD.ZeeFilter.MinMass
                                                        )
    
    # Define the prescaled filters; configurable with command line argument
    desdSingleElectronSequence += PrimaryDPDPrescaler( "SingleElectronStream_ZeePrescaleFilter",
                                                       AcceptAlgs = ["SingleElectronStream_ZeeFilter"],
                                                       Prescale = primSingleElectronDPD.ZeeFilter.Prescale )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("SingleElectronStream_ZeePrescaleFilter")
    sglElCMDString += "SingleElectronStream_ZeePrescaleFilter"    
    pass




if sglElCMDString != "" and not sglElCMDString.endswith(" or "):
    sglElCMDString += " or "
    pass





# Define W->e nu filter
if primSingleElectronDPD.WenuFilter():
    desdSingleElectronSequence += D2PDElectronSelector( "SingleElectronStream_WenuElectronFilter",
                                                        OutputLevel                   = INFO,
                                                        inputCollection               = 'ElectronAODCollection',
                                                        minNumberPassed               = 1,
                                                        electronVetoAuthorList        = primSingleElectronDPD.WenuFilter.VetoElectronAuthor,
                                                        electronID                    = primSingleElectronDPD.WenuFilter.ElectronIsEM,
                                                        egammaTrackIsolationParameter = primSingleElectronDPD.WenuFilter.ElectronPtCone,
                                                        relativeTrackPtIsolationMax   = primSingleElectronDPD.WenuFilter.ElectronPtConeMax,
                                                        clusterEtMin                  = primSingleElectronDPD.WenuFilter.ElectronEt
                                                        )
    desdSingleElectronSequence += D2PDMissingEtSelector( "SingleElectronStream_WenuMissingEtFilter",
                                                         OutputLevel      = INFO,
                                                         inputCollection  = 'MET_RefFinal',
                                                         outputCollection = 'DESD_SGLELStream_WenuNeutrinoCollection',
                                                         minNumberPassed  = 1,
                                                         missingEtMin     = primSingleElectronDPD.WenuFilter.MissingEt
                                                         )
    # Define the prescaled filters; configurable with command line argument
    desdSingleElectronSequence += PrimaryDPDPrescaler( "SingleElectronStream_WenuPrescaleFilter",
                                                       RequireAlgs = [ "SingleElectronStream_WenuElectronFilter",
                                                                       "SingleElectronStream_WenuMissingEtFilter" ],
                                                       Prescale    = primSingleElectronDPD.WenuFilter.Prescale )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("SingleElectronStream_WenuPrescaleFilter")
    sglElCMDString += "SingleElectronStream_WenuPrescaleFilter"
    pass




if sglElCMDString != "" and not sglElCMDString.endswith(" or "):
    sglElCMDString += " or "
    pass







# Define semi-leptonic ttbar filter
if primSingleElectronDPD.SemiLepTTBarFilter():
    desdSingleElectronSequence += D2PDElectronSelector( "SingleElectronStream_SemiLepTTBarElectronFilter",
                                                        OutputLevel                   = INFO,
                                                        inputCollection               = 'ElectronAODCollection',
                                                        minNumberPassed               = 1,
                                                        electronVetoAuthorList        = primSingleElectronDPD.SemiLepTTBarFilter.VetoElectronAuthor,
                                                        electronID                    = primSingleElectronDPD.SemiLepTTBarFilter.ElectronIsEM,
                                                        egammaTrackIsolationParameter = primSingleElectronDPD.SemiLepTTBarFilter.ElectronPtCone,
                                                        relativeTrackPtIsolationMax   = primSingleElectronDPD.SemiLepTTBarFilter.ElectronPtConeMax,
                                                        clusterEtMin                  = primSingleElectronDPD.SemiLepTTBarFilter.ElectronEt
                                                        ) 
    desdSingleElectronSequence += D2PDMissingEtSelector( "SingleElectronStream_SemiLepTTBarMissingEtFilter",
                                                         OutputLevel      = INFO,
                                                         inputCollection  = 'MET_RefFinal',
                                                         minNumberPassed  = 1,
                                                         missingEtMin     = primSingleElectronDPD.SemiLepTTBarFilter.MissingEt
                                                         )
    desdSingleElectronSequence += D2PDJetSelector( "SingleElectronStream_SemiLepTTBarJetFilter",
                                                   OutputLevel          = INFO,
                                                   inputCollection      = primSingleElectronDPD.SemiLepTTBarFilter.JetCollection,
                                                   minNumberPassed      = 4,
                                                   ptMin                = primSingleElectronDPD.SemiLepTTBarFilter.JetPt,
                                                   absEtaMax            = primSingleElectronDPD.SemiLepTTBarFilter.JetEta
                                                   )
    # Define the prescaled filters; configurable with command line argument
    desdSingleElectronSequence += PrimaryDPDPrescaler( "SingleElectronStream_SemiLepTTBarPrescaleFilter",
                                                       RequireAlgs = [ "SingleElectronStream_SemiLepTTBarElectronFilter",
                                                                       "SingleElectronStream_SemiLepTTBarMissingEtFilter",
                                                                       "SingleElectronStream_SemiLepTTBarJetFilter" ],
                                                       Prescale    = primSingleElectronDPD.SemiLepTTBarFilter.Prescale )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("SingleElectronStream_SemiLepTTBarPrescaleFilter")
    sglElCMDString += "SingleElectronStream_SemiLepTTBarPrescaleFilter"
    pass




if sglElCMDString != "" and not sglElCMDString.endswith(" or "):
    sglElCMDString += " or "
    pass





# Create the electron muon filters
if primSingleElectronDPD.ElectronMuonFilter():
    muonAlgList = []

    # Create a Staco muon filter
    # Check if we have the muon chain collection available or not
    from RecExConfig.ObjKeyStore import objKeyStore
    if objKeyStore.isInInput("Analysis::MuonContainer", "StacoMuonCollection") \
           or objKeyStore['transient'].has_item("Analysis::MuonContainer#StacoMuonCollection"):
        singleelectronDPDStream_msg.info("Using StacoMuonCollection chain in addition to staco and muid")

        desdSingleElectronSequence += D2PDMuonSelector( "SingleElectronStream_StacoMuonFilter",
                                                        OutputLevel                       = INFO,
                                                        inputCollection                   = 'StacoMuonCollection',
                                                        minNumberPassed                   = 1,
                                                        ptMin                             = primSingleElectronDPD.ElectronMuonFilter.MuonEt,
                                                        #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                        muonParametersList                = [ 19 ],
                                                        muonParametersMinList             = [ -100000000.0 ],
                                                        muonParametersMaxList             = [ primSingleElectronDPD.ElectronMuonFilter.RelativeMuonIso ],
                                                        muonParametersUseRelativeCutsList = [ True ],
                                                        muonRequireIsCombined             = True
                                                        )
        muonAlgList.append( "SingleElectronStream_StacoMuonFilter" )
        pass
    

    # Create a MuID muon filter
    # Check if we have the muon chain collection available or not
    from RecExConfig.ObjKeyStore import objKeyStore
    if objKeyStore.isInInput("Analysis::MuonContainer", "MuidMuonCollection") \
           or objKeyStore['transient'].has_item("Analysis::MuonContainer#MuidMuonCollection"):
        singleelectronDPDStream_msg.info("Using StacoMuonCollection chain in addition to staco and muid")

        desdSingleElectronSequence += D2PDMuonSelector( "SingleElectronStream_MuidMuonFilter",
                                                        OutputLevel                       = INFO,
                                                        inputCollection                   = 'MuidMuonCollection',
                                                        minNumberPassed                   = 1,
                                                        ptMin                             = primSingleElectronDPD.ElectronMuonFilter.MuonEt,
                                                        #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                        muonParametersList                = [ 19 ],
                                                        muonParametersMinList             = [ -100000000.0 ],
                                                        muonParametersMaxList             = [ primSingleElectronDPD.ElectronMuonFilter.RelativeMuonIso ],
                                                        muonParametersUseRelativeCutsList = [ True ],
                                                        muonRequireIsCombined             = True
                                                        )
        muonAlgList.append( "SingleElectronStream_MuidMuonFilter" )
        pass


    # Check if we have the thirs muon chain collection available or not
    from RecExConfig.ObjKeyStore import objKeyStore
    if objKeyStore.isInInput("Analysis::MuonContainer", "Muons") \
           or objKeyStore['transient'].has_item("Analysis::MuonContainer#Muons"):
        singleelectronDPDStream_msg.info("Using third muon chain in addition to staco and muid")

        # Create a third chain muon filter
        desdSingleElectronSequence += D2PDMuonSelector( "SingleElectronStream_MuonsFilter",
                                                        OutputLevel                       = INFO,
                                                        inputCollection                   = 'Muons',
                                                        minNumberPassed                   = 1,
                                                        ptMin                             = primSingleElectronDPD.ElectronMuonFilter.MuonEt,
                                                        #muonParametersList                = [ ROOT.MuonParameters.ptcone40 ],
                                                        muonParametersList                = [ 19 ],
                                                        muonParametersMinList             = [ -100000000.0 ],
                                                        muonParametersMaxList             = [ primSingleElectronDPD.ElectronMuonFilter.RelativeMuonIso ],
                                                        muonParametersUseRelativeCutsList = [ True ],
                                                        muonRequireIsCombined             = True
                                                        )
        muonAlgList.append( "SingleElectronStream_MuonsFilter" )
        pass

    
    # Create the electron filter
    desdSingleElectronSequence += D2PDElectronSelector( "SingleElectronStream_ElectronForElectronMuonFilter",
                                         OutputLevel            = INFO,
                                         inputCollection        = 'ElectronAODCollection',
                                         minNumberPassed        = 1,
                                         electronVetoAuthorList = primSingleElectronDPD.ElectronMuonFilter.VetoElectronAuthor,
                                         electronID             = primSingleElectronDPD.ElectronMuonFilter.ElectronIsEM,
                                         clusterEtMin           = primSingleElectronDPD.ElectronMuonFilter.ElectronEt
                                         )


    # Define the prescaled filters; configurable with command line argument
    desdSingleElectronSequence += PrimaryDPDPrescaler( "SingleElectronStream_ElectronMuonPrescaleFilter",
                                        AcceptAlgs  = muonAlgList,
                                        RequireAlgs = [ "SingleElectronStream_ElectronForElectronMuonFilter" ],
                                        Prescale    = primSingleElectronDPD.ElectronMuonFilter.Prescale )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("SingleElectronStream_ElectronMuonPrescaleFilter")
    sglElCMDString += "SingleElectronStream_ElectronMuonPrescaleFilter"
    pass



if sglElCMDString != "" and not sglElCMDString.endswith(" or "):
    sglElCMDString += " or "
    pass



# Filters for Z -> tau tau -> e had
if primSingleElectronDPD.ElectronTauFilter():
    desdSingleElectronSequence += D2PDElectronSelector( "SingleElectronStream_ElectronForZtautauFilter",
                                                        OutputLevel                   = INFO,
                                                        inputCollection               = 'ElectronAODCollection',
                                                        outputLinkCollection          = 'DESD_SGLELStream_ElectronForZtautauLinkCollection',
                                                        minNumberPassed               = 1,
                                                        electronVetoAuthorList        = primSingleElectronDPD.ElectronTauFilter.VetoElectronAuthor,
                                                        electronID                    = primSingleElectronDPD.ElectronTauFilter.ElectronIsEM,
                                                        clusterEtMin                  = primSingleElectronDPD.ElectronTauFilter.ElectronEt,
                                                        clusterEtMax                  = primSingleElectronDPD.ElectronTauFilter.ElectronEtMax,
                                                        egammaTrackIsolationParameter = primSingleElectronDPD.ElectronTauFilter.ElectronPtCone,
                                                        relativeTrackPtIsolationMax   = primSingleElectronDPD.ElectronTauFilter.ElectronPtConeMax,
                                                        egammaCaloIsolationParameter  = primSingleElectronDPD.ElectronTauFilter.ElectronEtCone,
                                                        relativeCaloEtIsolationMax    = primSingleElectronDPD.ElectronTauFilter.ElectronEtConeMax
                                                        )
    
    # Set up a DeltaR association/matching tool
    ToolSvc += DeltaRAssociationTool( "SingleElectronStream_TauElectronDeltaRAssociationTool",
                                      OutputLevel                = INFO,
                                      inputAssociateToCollection = "DESD_SGLELStream_ElectronForZtautauLinkCollection",
                                      deltaRMax                  = 0.4,
                                      writeUserData              = False
                                      )
    # Set up the low Et tau filter
    desdSingleElectronSequence += D2PDTauSelector( "SingleElectronStream_TauForZtautauFilter",
                                                   OutputLevel                    = INFO,
                                                   inputCollection                = 'TauRecContainer',
                                                   outputLinkCollection           = 'DESD_SGLELStream_TauForZtautauLinkCollection',
                                                   minNumberPassed                = 1,
                                                   associationToolList            = [ ToolSvc.SingleElectronStream_TauElectronDeltaRAssociationTool ],
                                                   outputAssociationContainerList = [ "DESD_SGLELStream_LowEtTauElectronAssociations" ],
                                                   numberOfAssociationsMaxCutList = [ 0 ],
                                                   minNumberTracks                = primSingleElectronDPD.ElectronTauFilter.TauNTrackMin,
                                                   maxNumberTracks                = primSingleElectronDPD.ElectronTauFilter.TauNTrackMax,
                                                   etMin                          = primSingleElectronDPD.ElectronTauFilter.TauEt
                                                   )

    # Define the combination of electron and tau
    from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner
    desdSingleElectronSequence += D2PDParticleCombiner( "SingleElectronStream_ZtauelFilter",
                                                        OutputLevel             = INFO,
                                                        compositeParticlePDG_ID = 23, # This is a Z Boson
                                                        inputCollectionList     = [ 'DESD_SGLELStream_TauForZtautauLinkCollection',
                                                                                    'DESD_SGLELStream_ElectronForZtautauLinkCollection' ],
                                                        outputCollection        = 'DESD_SGLELStream_ZtauelCollection',
                                                        chargeMin               = -1,
                                                        chargeMax               = 1,
                                                        deltaPhiMin             = 2.2,
                                                        minNumberPassed         = 1
                                                        )
    
    # Prescale the combined tau-el filter
    desdSingleElectronSequence += PrimaryDPDPrescaler( "SingleElectronStream_TauElectronPrescaleFilter",
                                                       OutputLevel = INFO,
                                                       AcceptAlgs  = [ "SingleElectronStream_ZtauelFilter" ],
                                                       Prescale    = primSingleElectronDPD.ElectronTauFilter.Prescale )
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("SingleElectronStream_TauElectronPrescaleFilter")
    # Add the prescaler to the bookkeeping
    sglElCMDString += "SingleElectronStream_TauElectronPrescaleFilter"
    pass


if sglElCMDString != "" and not sglElCMDString.endswith(" or "):
    sglElCMDString += " or "
    pass






if sglElCMDString.endswith(" or "):
    sglElCMDString = sglElCMDString[:-len(" or ")]
    pass




# Create the combined decission
desdSingleElectronSequence += LogicalFilterCombiner( "StreamDESD_SGLEL_AcceptEvent",
                                                     cmdstring = sglElCMDString )
from PrimaryDPDMaker.PrimaryDPDFlags import desdEventSkimmingFilterNamesList
desdEventSkimmingFilterNamesList.append( "StreamDESD_SGLEL_AcceptEvent" )





##====================================================================
## Define the Single Electron DPD output stream
##====================================================================
streamName = primDPD.WriteSingleElectronStream.StreamName
fileName   = buildFileName( primDPD.WriteSingleElectronStream )
if primDPD.WriteSingleElectronStream.isVirtual or primDPD.isVirtual() :
    SingleElectronStream = MSMgr.NewVirtualStream( streamName, fileName )
    pass
else:
    SingleElectronStream = MSMgr.NewPoolStream( streamName, fileName )
    SingleElectronStream.AddOtherAlgsToBookkeep( algsToBookkeep )
    pass

if primDPD.ApplySkimming() and primSingleElectronDPD.ApplySkimming() :
    # Schedule these prescaled algorithms as filters in OR
    SingleElectronStream.AcceptAlgs( [ "StreamDESD_SGLEL_AcceptEvent" ] )
    pass

#SingleElectronStream.AddOtherAlgsToBookkeep( [ "SingleElectronStream_ElectronForZtautauFilter", "SingleElectronStream_TauForZtautauFilter" ] ) 


#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take everything from the input
ExcludeList=[ "TrigMuonEFContainer#HLT_MuonEF" ]

dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,ExcludeList)
#SingleElectronStream.AddItem( ["egammaContainer#ElectronAODCollection"] )


