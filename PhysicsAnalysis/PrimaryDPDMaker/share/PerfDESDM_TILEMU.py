##----------------------------------------------------------------------
## Name: PerfDESDM_TILEMU.py
##
## Author: Tomoe Kishimoto
## Email:  Tomoe.Kishimoto@cern.ch
##
## Description: This defines the content of the commissioning stream for 
##              Tile-Muon trigger
##
##----------------------------------------------------------------------

# If this file is already included, don't include it again
include.block("PrimaryDPDMaker/PerfDESDM_TILEMU.py")

## for messaging
from AthenaCommon.Logging import logging
muonDPDStream_msg = logging.getLogger( 'PerfDESDM_TILEMUStream' )

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

## Include the job property flags for this package
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from RecExConfig.RecFlags import rec

## Include the job property flags for this package
from PrimaryDPDMaker.PrimaryDESDMFlags_TileMu import primDESDMTileMu

## This handels multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

##======================================================================
## Write the used options to the log file
##======================================================================
if rec.OutputLevel() <= DEBUG:
    muonDPDStream_msg.info('Values of all PerfDESDM_TileMu flags:')
    print primDESDMTileMu
    pass

# ======================================================================
# Create a sub-sequence where all needed algorithms will run in
# ======================================================================
import AthenaCommon.CfgMgr as CfgMgr
desdTileMuonSequence = CfgMgr.AthSequencer('DESDM_TILEMU_Sequence',
                                          StopOverride = True )
topSequence += desdTileMuonSequence

##======================================================================
## Define the skimming for the DESDM_TILEMU output stream
##======================================================================
from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner
from PrimaryDPDMaker.PrimaryDPDMakerConf   import PrimaryDPDPrescaler
        
from RecExConfig.ObjKeyStore import objKeyStore
muonDPDStream_msg.debug("Items in objKeyStore[transient]: %s" %  
    objKeyStore['transient'].list() )

# ----------------------------------------------------------------------
# String based reduction
# ----------------------------------------------------------------------
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# ----------------------------------------------------------------------
# Muon HLT triggers:
# ----------------------------------------------------------------------
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
TileMuTriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(name = "TileMuTriggerSkimmingTool",
    TriggerListOR =  ["HLT_mu.*"] )

ToolSvc += TileMuTriggerSkimmingTool
print TileMuTriggerSkimmingTool

# ----------------------------------------------------------------------
# Muon selection
# ----------------------------------------------------------------------
muon_type = '(Muons.muonType == 0)'
muon_pt   = '(Muons.pt > 13*GeV)'
muon_eta  = '((abs(Muons.eta) < 1.5) && (abs(Muons.eta) > 0.8))'

muon_expression = '(count('+muon_type+' && '+muon_pt+' && '+muon_eta+') > 0)'

TileMuEventSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "TileMuEventSkimmingTool",
    expression = muon_expression)

ToolSvc += TileMuEventSkimmingTool
print TileMuEventSkimmingTool

# ----------------------------------------------------------------------
# Final logical selection
# ----------------------------------------------------------------------
TileMuEventFilterTool = DerivationFramework__FilterCombinationAND(name="TileMuEventFilterTool",
    FilterList=[TileMuTriggerSkimmingTool, TileMuEventSkimmingTool])

ToolSvc += TileMuEventFilterTool
print TileMuEventFilterTool


##======================================================================
## Define the thinning for the DESDM_TILEMU output stream
##======================================================================
from PrimaryDPDMaker.PrimaryDPDMakerConf import DerivationFramework__CaloCellThinningTool
TileMuCaloCellThinningTool = DerivationFramework__CaloCellThinningTool(name = "TileMuCaloCellThinningTool",
                                                               ThinningService = "TileMuThinningToolSvc",
                                                               CaloCellId = 3) # Tile Cells
ToolSvc += TileMuCaloCellThinningTool

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
TileMuMuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name = "TileMuMuonTPThinningTool",
                                                                          ThinningService = "TileMuThinningToolSvc",
                                                                          MuonKey = "Muons",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += TileMuMuonTPThinningTool


##======================================================================
##  CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS
##======================================================================
desdTileMuonSequence += CfgMgr.DerivationFramework__DerivationKernel("TileMuKernel", 
    SkimmingTools = [TileMuEventFilterTool],
    ThinningTools = [TileMuCaloCellThinningTool, TileMuMuonTPThinningTool])

##======================================================================
## Define this Muon DPD output stream
##======================================================================
streamName = primDPD.WriteDESDM_TILEMUStream.StreamName
fileName   = buildFileName( primDPD.WriteDESDM_TILEMUStream )
algsToBookkeep = []

if primDPD.WriteDESDM_TILEMUStream.isVirtual or primDPD.isVirtual() :
    TileMuStream = MSMgr.NewVirtualStream( streamName, fileName )
    pass
else:
    TileMuStream = MSMgr.NewPoolStream( streamName, fileName )
    # Add the per-event bookkeeping 
    TileMuStream.AddOtherAlgsToBookkeep( algsToBookkeep )
    pass

if primDESDMTileMu.ApplySkimming():
    TileMuStream.AcceptAlgs(["TileMuKernel"])
    pass

from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream(streamName)
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc(svcName="TileMuThinningToolSvc", outStreams=[evtStream])

#-----------------------------------------------------------------------
# Add the containers to the output stream
#-----------------------------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput
ExcludeList=[]

#dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,ExcludeList)

trackParticleAuxExclusions="-caloExtension.-cellAssociation.-clusterAssociation.-trackParameterCovarianceMatrices.-parameterX.-parameterY.-parameterZ.-parameterPX.-parameterPY.-parameterPZ.-parameterPosition"


TileMuStream.AddItem(["xAOD::EventInfo#*"])
TileMuStream.AddItem(["xAOD::EventAuxInfo#*"])

TileMuStream.AddItem(["xAOD::MuonContainer#Muons"])
TileMuStream.AddItem(["xAOD::MuonAuxContainer#MuonsAux."])

TileMuStream.AddItem(["xAOD::TrigNavigation#TrigNavigation"])
TileMuStream.AddItem(["xAOD::TrigNavigationAuxInfo#TrigNavigationAux."])
TileMuStream.AddItem(["xAOD::TrigDecision#xTrigDecision"])
TileMuStream.AddItem(["xAOD::TrigDecisionAuxInfo#xTrigDecisionAux."])

TileMuStream.AddItem(["xAOD::MuonRoIContainer#LVL1MuonRoIs"])
TileMuStream.AddItem(["xAOD::MuonRoIAuxContainer#LVL1MuonRoIsAux."])
TileMuStream.AddItem(["MuCTPI_RDO#MUCTPI_RDO"])

TileMuStream.AddItem(["Muon::TgcCoinDataContainer#TrigT1CoinDataCollectionPriorBC"])
TileMuStream.AddItem(["Muon::TgcCoinDataContainer#TrigT1CoinDataCollectionNextBC"])
TileMuStream.AddItem(["Muon::TgcCoinDataContainer#TrigT1CoinDataCollection"])

TileMuStream.AddItem(["TileDigitsContainer#MuRcvDigitsCnt"])
TileMuStream.AddItem(["TileRawChannelContainer#MuRcvRawChCnt"])
TileMuStream.AddItem(["TileMuonReceiverContainer#TileMuRcvCnt"])
TileMuStream.AddItem(["CaloCellContainer#AllCalo"])

TileMuStream.AddItem(["xAOD::MuonContainer#HLT_xAOD__MuonContainer_MuonEFInfo"])
TileMuStream.AddItem(["xAOD::MuonAuxContainer#HLT_xAOD__MuonContainer_MuonEFInfoAux."])

TileMuStream.AddItem(["xAOD::TrackParticleContainer#MuonSpectrometerTrackParticles"])
TileMuStream.AddItem(["xAOD::TrackParticleAuxContainer#MuonSpectrometerTrackParticlesAux."+trackParticleAuxExclusions])
TileMuStream.AddItem(["xAOD::TrackParticleContainer#CombinedMuonTrackParticles"])
TileMuStream.AddItem(["xAOD::TrackParticleAuxContainer#CombinedMuonTrackParticlesAux."+trackParticleAuxExclusions])
TileMuStream.AddItem(["xAOD::TrackParticleContainer#ExtrapolatedMuonTrackParticles"])
TileMuStream.AddItem(["xAOD::TrackParticleAuxContainer#ExtrapolatedMuonTrackParticlesAux."+trackParticleAuxExclusions])
TileMuStream.AddItem(["xAOD::TrackParticleContainer#InDetTrackParticles"]) 
TileMuStream.AddItem(["xAOD::TrackParticleAuxContainer#InDetTrackParticlesAux."+trackParticleAuxExclusions])
