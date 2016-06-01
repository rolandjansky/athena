#-----------------------------------------------------------------------------
## Name: PerfDPD_EGamma.py
##
## Author : L. Carminati (july 2015) adapted from the original from K. Koeneke
##
## Description: This defines the main EGAMMA DESD output stream.
##               
##-----------------------------------------------------------------------------

include.block("PrimaryDPDMaker/PerfDPD_Egamma.py")

## Include the job property flags for this package
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from RecExConfig.RecFlags import rec

## This handels multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

from AthenaCommon.GlobalFlags import globalflags
from DerivationFrameworkCore.DerivationFrameworkMaster import *

##============================================================================
## Define the skimming (event selection) for the DESDM_EGAMMA output stream
##============================================================================

# Object selection strings

sel_tag_tp1   = 'Electrons.pt > 25*GeV  &&  ( Electrons.Tight || Electrons.LHTight ) '
sel_probe_tp1 = 'Electrons.pt > 15*GeV'

sel_tag_tp2   = 'Electrons.pt > 25*GeV  &&  ( Electrons.Tight || Electrons.LHTight ) '
sel_probe_tp2 = 'ForwardElectrons.pt > 20*GeV'

sel_tag_tp3   = 'Electrons.pt > 25*GeV  &&  ( Electrons.Tight || Electrons.LHTight ) '
sel_probe_tp3 = 'Photons.pt > 15*GeV'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__InvariantMassTool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool

# Tag & Probe with central electrons

DESDM_EGAMMATPMassTool1 = DerivationFramework__InvariantMassTool( name = "DESDM_EGAMMATPMassTool1",
                                                                  ContainerName              = "Electrons",
                                                                  SecondContainerName        = "Electrons",
                                                                  ObjectRequirements         = sel_tag_tp1,
                                                                  SecondObjectRequirements   = sel_probe_tp1,
                                                                  MassHypothesis             = 0.511,
                                                                  SecondMassHypothesis       = 0.511,
                                                                  StoreGateEntryName         = "DESDM_EGAMMATPMass1")
ToolSvc += DESDM_EGAMMATPMassTool1

# Tag & Probe with 1 forward electron

DESDM_EGAMMATPMassTool2 = DerivationFramework__InvariantMassTool( name = "DESDM_EGAMMATPMassTool2",
                                                                  ContainerName              = "Electrons",
                                                                  SecondContainerName        = "ForwardElectrons",
                                                                  ObjectRequirements         = sel_tag_tp2,
                                                                  SecondObjectRequirements   = sel_probe_tp2,
                                                                  MassHypothesis             = 0.511,
                                                                  SecondMassHypothesis       = 0.511,
                                                                  StoreGateEntryName         = "DESDM_EGAMMATPMass2")
ToolSvc += DESDM_EGAMMATPMassTool2

# Tag & Probe with 1 electron and 1 photon

DESDM_EGAMMATPMassTool3 = DerivationFramework__InvariantMassTool( name = "DESDM_EGAMMATPMassTool3",
                                                                  ContainerName              = "Electrons",
                                                                  SecondContainerName        = "Photons",
                                                                  ObjectRequirements         = sel_tag_tp3,
                                                                  SecondObjectRequirements   = sel_probe_tp3,
                                                                  MassHypothesis             = 0.511,
                                                                  SecondMassHypothesis       = 0.0,
                                                                  StoreGateEntryName         = "DESDM_EGAMMATPMass3")
ToolSvc += DESDM_EGAMMATPMassTool3

# Define the skimming

desdm_egamma1 = '( count ( DESDM_EGAMMATPMass1 > 60*GeV)  >= 1 )'
desdm_egamma2 = '( count ( DESDM_EGAMMATPMass2 > 60*GeV)  >= 1 )'
desdm_egamma3 = '( count ( DESDM_EGAMMATPMass3 > 60*GeV)  >= 1 )'

desdm_egam = desdm_egamma1 + '||'+  desdm_egamma2 +'||'+  desdm_egamma3 

# String based event selection tool

DESDM_EGAMMA_SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "DESDM_EGAMMA_SkimmingTool",
                                                                        expression = desdm_egam)

ToolSvc += DESDM_EGAMMA_SkimmingTool

# Trigger based event selection tool

DESDM_EGAMMAJPsiTrigSkimmingTool = DerivationFramework__TriggerSkimmingTool(   name = "DESDM_EGAMMAJPsiTrigSkimmingTool",	
                                                                               TriggerListOR =  ["HLT_e5_tight_e4_etcut",
                                                                                                 "HLT_e5_tight_e4_etcut_Jpsiee", 
                                                                                                 "HLT_e9_tight_e4_etcut_Jpsiee",
                                                                                                 "HLT_e9_etcut_e5_tight_Jpsiee",
                                                                                                 "HLT_e14_tight_e4_etcut_Jpsiee",
                                                                                                 "HLT_e14_etcut_e5_tight_Jpsiee",
                                                                                                 "HLT_e5_lhtight_e4_etcut",
                                                                                                 "HLT_e5_lhtight_e4_etcut_Jpsiee",
                                                                                                 "HLT_e9_lhtight_e4_etcut_Jpsiee",
                                                                                                 "HLT_e9_etcut_e5_lhtight_Jpsiee",            
                                                                                                 "HLT_e14_lhtight_e4_etcut_Jpsiee",
                                                                                                 "HLT_e14_etcut_e5_lhtight_Jpsiee",
                                                                                                 "HLT_e5_tight1_e4_etcut",
                                                                                                 "HLT_e5_tight1_e4_etcut_Jpsiee",
                                                                                                 "HLT_e5_tight_e4_etcut_L1JPSI-1M5",
                                                                                                 "HLT_e5_lhtight_e4_etcut_L1JPSI-1M5", 
                                                                                                 "HLT_e5_tight1_e4_etcut_L1JPSI-1M5",
                                                                                                 "HLT_e5_tight_e4_etcut_Jpsiee_L1JPSI-1M5",
                                                                                                 "HLT_e9_tight_e4_etcut_Jpsiee_L1JPSI-1M5-EM7",                
                                                                                                 "HLT_e9_etcut_e5_tight_Jpsiee_L1JPSI-1M5-EM7",
                                                                                                 "HLT_e14_tight_e4_etcut_Jpsiee_L1JPSI-1M5-EM12",
                                                                                                 "HLT_e14_etcut_e5_tight_Jpsiee_L1JPSI-1M5-EM12",
                                                                                                 "HLT_e5_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5",
                                                                                                 "HLT_e9_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5-EM7",
                                                                                                 "HLT_e9_etcut_e5_lhtight_Jpsiee_L1JPSI-1M5-EM7",
                                                                                                 "HLT_e14_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5-EM12",
                                                                                                 "HLT_e14_etcut_e5_lhtight_Jpsiee_L1JPSI-1M5-EM12",
                                                                                                 "HLT_e5_tight1_e4_etcut_Jpsiee_L1JPSI-1M5" ]) 


ToolSvc += DESDM_EGAMMAJPsiTrigSkimmingTool
print DESDM_EGAMMAJPsiTrigSkimmingTool

# Final event selection filter : 

DESDM_EGAMMAEventFilterTool=DerivationFramework__FilterCombinationOR(name="DESDM_EGAMMAEventFilterTool",FilterList=[DESDM_EGAMMA_SkimmingTool,DESDM_EGAMMAJPsiTrigSkimmingTool])
ToolSvc+=DESDM_EGAMMAEventFilterTool
print DESDM_EGAMMAEventFilterTool

# =================================================================================
# Agmentation starts here
# 
# note : calo cell thinning is tecnically an augmentation. We remove the original      
#        cell container and we add a new cell container with the interesting cells.
#  
# =================================================================================

# ========>>> Calo Cells thinning ===================================

from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CellsInConeThinning
DESDM_EGAMMACellsInConeThinningEl = DerivationFramework__CellsInConeThinning( name = "DESDM_EGAMMACellsInConeThinningEl",
                                                                               InputSGKey = "Electrons",
                                                                               InputCellsSGKey = "AllCalo",
                                                                               OutputClusterSGKey = "EgammaDummyClustersEl",                                                                            
                                                                               deltaR = 0.5                                                            
                                                                               )

ToolSvc += DESDM_EGAMMACellsInConeThinningEl

DESDM_EGAMMACellsInConeThinningPh = DerivationFramework__CellsInConeThinning( name = "DESDM_EGAMMACellsInConeThinningPh", 
                                                                              InputSGKey = "Photons",
                                                                              InputCellsSGKey = "AllCalo",
                                                                              OutputClusterSGKey = "EgammaDummyClustersPh",                                                                            
                                                                              deltaR = 0.5                                                            
                                                                              )

ToolSvc += DESDM_EGAMMACellsInConeThinningPh


# =================================================================================
# Thinning starts here
# =================================================================================

# ========>>> Tracks thinning ===================================

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EGammaTracksThinning
DESDM_EGAMMATracksThinningTool = DerivationFramework__EGammaTracksThinning(  name = "DESDM_EGAMMATracksThinning",
                                                                       ThinningService = "DESDM_EGAMMAThinningSvc",
                                                                       tracksCollectionName   = "Tracks" ,
                                                                       electronContainerName = "Electrons",
                                                                       photonContainerName   = "Photons",
                                                                       deltaR  = 0.5,
                                                                       minEtEg = 0.
                                                                       )
ToolSvc += DESDM_EGAMMATracksThinningTool

# ========>>> Egamma PrepRawData thinning =======================
        
from RegionSelector.RegionSelectorConf import RegSelSvc
svcMgr += RegSelSvc("RegSelSvcInEGammaStream")
svcMgr.RegSelSvcInEGammaStream.enableID    = True
svcMgr.RegSelSvcInEGammaStream.enablePixel = True
svcMgr.RegSelSvcInEGammaStream.enableSCT   = True
svcMgr.RegSelSvcInEGammaStream.enableTRT   = True
    
from InDetRegionSelector.InDetRegionSelectorConf import SiRegionSelectorTable
PixelRegionSelectorTable = SiRegionSelectorTable(name        = "PixelRegionSelectorTable",
                                                 ManagerName = "Pixel",
                                                 DeltaZ      = 168 * Units.mm,  # Z vertex extent = +- this value.
                                                 OutputFile  = "RoITablePixel.txt",
                                                 PrintHashId = True,
                                                 PrintTable  = False)
ToolSvc += PixelRegionSelectorTable
print      PixelRegionSelectorTable
    
from InDetRegionSelector.InDetRegionSelectorConf import SiRegionSelectorTable
SCT_RegionSelectorTable = SiRegionSelectorTable(name        = "SCT_RegionSelectorTable",
                                                ManagerName = "SCT",
                                                DeltaZ      = 168 * Units.mm,  # Z vertex extent = +- this value.
                                                OutputFile  = "RoITableSCT.txt",
                                                PrintHashId = True,
                                                PrintTable  = False)
ToolSvc += SCT_RegionSelectorTable
print      SCT_RegionSelectorTable
    
from InDetRegionSelector.InDetRegionSelectorConf import TRT_RegionSelectorTable
TRT_RegionSelectorTable = TRT_RegionSelectorTable(name        = "TRT_RegionSelectorTable",
                                                  ManagerName = "TRT",
                                                  DeltaZ      = 168 * Units.mm,  # Z vertex extent = +- this value.
                                                  OutputFile  = "RoITableTRT.txt",
                                                  PrintHashId = True,
                                                  PrintTable  = False)
ToolSvc += TRT_RegionSelectorTable
print      TRT_RegionSelectorTable

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EGammaPrepRawDataThinning
DESDM_EGAMMAEGammaPrepRawDataThinningTool = DerivationFramework__EGammaPrepRawDataThinning(  name = "DESDM_EGAMMAEGammaPrepRawDataThinning",                                                                                             
                                                                                             ThinningService = "DESDM_EGAMMAThinningSvc",
                                                                                             RegSelSvc = "RegSelSvcInEGammaStream",
                                                                                             deltaR  = 0.5,
                                                                                             minEtEg = 0.
                                                                                             )
ToolSvc += DESDM_EGAMMAEGammaPrepRawDataThinningTool



# ========>>> Trigger Tower thinning ===================================

from TrigT1CaloCalibTools.TrigT1CaloCalibToolsConf import DerivationFramework__TriggerTowerThinningAlg
DESDM_EGAMMAL1CaloThinning = DerivationFramework__TriggerTowerThinningAlg( name = "DESDM_EGAMMAL1CaloThinning",
                                                                           ThinService = "DESDM_EGAMMAThinningSvc",
                                                                           TriggerTowerLocation = "xAODTriggerTowers",
                                                                           MinCaloCellEnergy = 0.8,
                                                                           MinADC = 36,
                                                                           UseRandom = True,
                                                                           MinRandom = 0.01)

ToolSvc += DESDM_EGAMMAL1CaloThinning

# =================================================================================
# Kernel algorithm
# =================================================================================

augmentationToolsList = [DESDM_EGAMMATPMassTool1,
                         DESDM_EGAMMATPMassTool2,
                         DESDM_EGAMMATPMassTool3,
                         DESDM_EGAMMACellsInConeThinningEl,
                         DESDM_EGAMMACellsInConeThinningPh]

thinningToolsList = [DESDM_EGAMMATracksThinningTool,
                     DESDM_EGAMMAEGammaPrepRawDataThinningTool,
                     DESDM_EGAMMAL1CaloThinning ]

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("DESDM_EGAMMAKernel",
                                                                       AugmentationTools = augmentationToolsList ,
                                                                       SkimmingTools = [DESDM_EGAMMAEventFilterTool],
                                                                       ThinningTools = thinningToolsList
                                                                       )

# =================================================================================
# Define stream
# =================================================================================

streamName = primDPD.WriteDESDM_EGAMMAStream.StreamName
fileName   = buildFileName( primDPD.WriteDESDM_EGAMMAStream )

if primDPD.WriteDESDM_EGAMMAStream.isVirtual or primDPD.isVirtual() :
    StreamDESDM_EGAMMA = MSMgr.NewVirtualStream( streamName, fileName )
    pass
else:
    StreamDESDM_EGAMMA = MSMgr.NewPoolStream( streamName, fileName )
    pass

StreamDESDM_EGAMMA.AcceptAlgs(["DESDM_EGAMMAKernel"])
	
# Thinning service name must match the one passed to the thinning tools

from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="DESDM_EGAMMAThinningSvc", outStreams=[evtStream] )

from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# build a new container with cells in big-clusters around egamma candidates and 
# add it to the output

from DerivationFrameworkCalo.CaloCellDFGetter import CaloCellDFGetter
DESDM_EGAMMACaloCellDFGetter = CaloCellDFGetter(inputClusterKeys=["EgammaDummyClustersEl","EgammaDummyClustersPh","egammaClusters"],
                                                inputCellKey="AllCalo",
                                                outputCellKey="DESDMEGAMMACellContainer")

StreamDESDM_EGAMMA.AddItem("CaloClusterCellLinkContainer#egammaClusters_links")
StreamDESDM_EGAMMA.AddItem("CaloCellContainer#DESDMEGAMMACellContainer")
   
# remove un-necessary containers from the output ( note that I remove the original calo
# cells container

#exclude list
# First non xAOD containers
# Then xAOD interface containers
# Then the corresponding Aux containers

excludeList=['RpcPadContainer#RPCPAD',
             'CaloCellContainer#AllCalo',
             'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_MuonEFInfo_CombTrackParticles',
             'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_MuTagIMO_EF_CombTrackParticles',
             'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_eMuonEFInfo_CombTrackParticles',
             'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_MuonEFInfo_ExtrapTrackParticles',
             'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTF',
             'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_MuTagIMO_EF_ExtrapTrackParticles',
             'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_eMuonEFInfo_ExtrapTrackParticles',
             'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_FTF',
             'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_FTF',
             'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_EFID',
             'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_EFID',
             'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_EFID',
             'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_TauIso_FTF',
             'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_IDTrig',
             'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_IDTrig',
             'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_IDTrig',
             'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_TauCore_FTF',
             'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bphysics_FTF',
             'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_MuonIso_EFID',
             'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_minBias_EFID',
             'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bphysics_EFID',
             'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_CosmicsN_EFID',
             'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_FullScan_EFID',
             'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF',
             'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bphysics_IDTrig',
             'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnvIOTRT_CosmicsN_EFID',             
             'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_MuonEFInfo_CombTrackParticlesAux.',
             'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_MuTagIMO_EF_CombTrackParticlesAux.',
             'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_eMuonEFInfo_CombTrackParticlesAux.',
             'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_MuonEFInfo_ExtrapTrackParticlesAux.',
             'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTFAux.',
             'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_MuTagIMO_EF_ExtrapTrackParticlesAux.',
             'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_eMuonEFInfo_ExtrapTrackParticlesAux.',
             'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_FTFAux.',
             'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_FTFAux.',
             'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_EFIDAux.',
             'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_EFIDAux.',
             'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_EFIDAux.',
             'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_TauIso_FTFAux.',
             'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_IDTrigAux.',
             'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_IDTrigAux.',
             'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_IDTrigAux.',
             'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_TauCore_FTFAux.',
             'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bphysics_FTFAux.',
             'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_MuonIso_EFIDAux.',
             'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_minBias_EFIDAux.',
             'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bphysics_EFIDAux.',
             'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_CosmicsN_EFIDAux.',
             'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_FullScan_EFIDAux.',
             'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_BjetPrmVtx_FTFAux.',
             'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bphysics_IDTrigAux.',
             'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnvIOTRT_CosmicsN_EFIDAux.'             
             ]
   
dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,excludeList)
