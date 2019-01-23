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
                                                                               OutputClusterSGKey ="EgammaDummyClustersEl",
                                                                               OutputCellLinksSGKey="EgammaDummyClustersEl_links",
                                                                               deltaR = 0.5
                                                                               )

ToolSvc += DESDM_EGAMMACellsInConeThinningEl

DESDM_EGAMMACellsInConeThinningPh = DerivationFramework__CellsInConeThinning( name = "DESDM_EGAMMACellsInConeThinningPh", 
                                                                              InputSGKey = "Photons",
                                                                              InputCellsSGKey = "AllCalo",
                                                                              OutputClusterSGKey = "EgammaDummyClustersPh",
                                                                              OutputCellLinksSGKey="EgammaDummyClustersPh_links",
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
        
# from RegionSelector.RegionSelectorConf import RegSelSvc
# svcMgr += RegSelSvc("RegSelSvcInEGammaStream")
# svcMgr.RegSelSvcInEGammaStream.enableID    = True
# svcMgr.RegSelSvcInEGammaStream.enablePixel = True
# svcMgr.RegSelSvcInEGammaStream.enableSCT   = True
# svcMgr.RegSelSvcInEGammaStream.enableTRT   = True
    
# from InDetRegionSelector.InDetRegionSelectorConf import SiRegionSelectorTable
# PixelRegionSelectorTable = SiRegionSelectorTable(name        = "PixRegSelectorTableForEgamma",
#                                                  ManagerName = "Pixel",
#                                                  DeltaZ      = 225 * Units.mm,  # Z vertex extent = +- this value.
#                                                  OutputFile  = "RoITablePixel.txt",
#                                                  PrintHashId = True,
#                                                  PrintTable  = False)
# ToolSvc += PixelRegionSelectorTable
# print      PixelRegionSelectorTable
    
# from InDetRegionSelector.InDetRegionSelectorConf import SiRegionSelectorTable
# SCT_RegionSelectorTable = SiRegionSelectorTable(name        = "SCT_RegSelectorTableForEgamma",
#                                                 ManagerName = "SCT",
#                                                 DeltaZ      = 225 * Units.mm,  # Z vertex extent = +- this value.
#                                                 OutputFile  = "RoITableSCT.txt",
#                                                 PrintHashId = True,
#                                                 PrintTable  = False)
# ToolSvc += SCT_RegionSelectorTable
# print      SCT_RegionSelectorTable
    
# from InDetRegionSelector.InDetRegionSelectorConf import TRT_RegionSelectorTable
# TRT_RegionSelectorTable = TRT_RegionSelectorTable(name        = "TRT_RegSelectorTableForEgamma",
#                                                   ManagerName = "TRT",
#                                                   DeltaZ      = 225 * Units.mm,  # Z vertex extent = +- this value.
#                                                   OutputFile  = "RoITableTRT.txt",
#                                                   PrintHashId = True,
#                                                   PrintTable  = False)
# ToolSvc += TRT_RegionSelectorTable
# print      TRT_RegionSelectorTable

# from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EGammaPrepRawDataThinning
# DESDM_EGAMMAEGammaPrepRawDataThinningTool = DerivationFramework__EGammaPrepRawDataThinning(  name = "DESDM_EGAMMAEGammaPrepRawDataThinning",                                                                                             
#                                                                                              ThinningService = "DESDM_EGAMMAThinningSvc",
#                                                                                              RegSelSvc = "RegSelSvcInEGammaStream",
#                                                                                              deltaR  = 0.5,
#                                                                                              minEtEg = 0.
#                                                                                              )
# ToolSvc += DESDM_EGAMMAEGammaPrepRawDataThinningTool


# ========>>> Trigger Tower thinning ===================================

from TrigT1CaloCalibTools.TrigT1CaloCalibToolsConf import DerivationFramework__TriggerTowerThinningAlg
DESDM_EGAMMAL1CaloThinning = DerivationFramework__TriggerTowerThinningAlg( name = "DESDM_EGAMMAL1CaloThinning",
                                                                           ThinService = "DESDM_EGAMMAThinningSvc",
                                                                           TriggerTowerLocation = "xAODTriggerTowers",
                                                                           MinCaloCellET = 0.8,
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
#                     DESDM_EGAMMAEGammaPrepRawDataThinningTool,
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

excludeList=[#'RpcPadContainer#RPCPAD',
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
   
#dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,excludeList)

trackParticleAuxExclusions="-caloExtension.-cellAssociation.-clusterAssociation.-trackParameterCovarianceMatrices.-parameterX.-parameterY.-parameterZ.-parameterPX.-parameterPY.-parameterPZ.-parameterPosition"


from AthenaCommon.KeyStore import CfgKeyStore,CfgItemList
outList = CfgItemList( 'EGAMMA',
                       items = ['EventInfo#*',
    'EventInfo#ByteStreamEventInfo',
    'InDet::PixelClusterContainer#PixelClusters',
    'InDet::SCT_ClusterContainer#SCT_Clusters',
    'BCM_RDO_Container#BCM_CompactDOs',
    'BCM_RDO_Container#BCM_RDOs',
    'LUCID_RawDataContainer#Lucid_RawData',
    'LArDigitContainer#LArDigitContainer_EMClust',
    'LArDigitContainer#LArDigitContainer_Thinned',
    'CaloCellContainer#HLT_CaloCellContainer_TrigCaloCellLoF',
    'CaloCellContainer#HLT_CaloCellContainer_TrigCaloCellMaker_eGamma',
    'CaloCellContainer#HLT_CaloCellContainer_TrigCaloCellMaker_tau',
    'CaloCellContainer#HLT_CaloCellContainer_TrigT2CaloEgammaCells',
    'CaloCellContainer#HLT_CaloCellContainer_TrigT2CaloTauCells',
    'CaloCellContainer#TauCommonPi0Cells',
    'TileDigitsContainer#MuRcvDigitsCnt',
    'TileDigitsContainer#TileDigitsFlt',
    'TileRawChannelContainer#MuRcvRawChCnt',
    'TileRawChannelContainer#TileRawChannelFlt',
    'TileCellContainer#E4prContainer',
    'TileCellContainer#MBTSContainer',
    'TileL2Container#TileL2Cnt',
    'TileMuContainer#TileMuObj',
    'TileMuonReceiverContainer#TileMuRcvCnt',
    'ROIB::RoIBResult#RoIBResult',
    'CTP_RDO#CTP_RDO',
    'MuCTPI_RDO#MUCTPI_RDO',
    'xAOD::TrigMissingETContainer#HLT_xAOD__TrigMissingETContainer_EFJetEtSum',
    'xAOD::TrigMissingETContainer#HLT_xAOD__TrigMissingETContainer_EFMissingET_Fex_2sidednoiseSupp_PUC',
    'xAOD::TrigMissingETContainer#HLT_xAOD__TrigMissingETContainer_T2MissingET',
    'xAOD::TrigMissingETContainer#HLT_xAOD__TrigMissingETContainer_TrigEFMissingET',
    'xAOD::TrigMissingETContainer#HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_FEB',
    'xAOD::TrigMissingETContainer#HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_mht',
    'xAOD::TrigMissingETContainer#HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl',
    'xAOD::TrigMissingETContainer#HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl_PS',
    'xAOD::TrigMissingETContainer#HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl_PUC',
    'xAOD::TrigMissingETContainer#HLT_xAOD__TrigMissingETContainer_TrigL2MissingET_FEB',
    'xAOD::TrigConfKeys#TrigConfKeys',
    'xAOD::EventAuxInfo#EventInfoAux.',
    'xAOD::TrigDecisionAuxInfo#xTrigDecisionAux.',
    'xAOD::EventInfo#EventInfo',
    'xAOD::EnergySumRoI#LVL1EnergySumRoI',
    'RecoTimingObj#RAWtoESD_mems',
    'RecoTimingObj#RAWtoESD_timings',
    'xAOD::TrigNavigationAuxInfo#TrigNavigationAux.',
    'xAOD::TrigDecision#xTrigDecision',
    'xAOD::JetEtRoI#LVL1JetEtRoI',
    'Trk::SegmentCollection#MuonSegments',
    'Trk::SegmentCollection#NCB_MuonSegments',
    'xAOD::MissingETAuxAssociationMap#METAssoc_AntiKt4EMPFlowAux.',
    'xAOD::MissingETAuxAssociationMap#METAssoc_AntiKt4EMTopoAux.',
    'xAOD::MissingETAuxAssociationMap#METAssoc_AntiKt4LCTopoAux.',
    'HLT::HLTResult#HLTResult_HLT',
    'LArNoisyROSummary#LArNoisyROSummary',
    'TrackCollection#CombinedMuonTracks',
    'TrackCollection#ExtrapolatedMuonTracks',
    'TrackCollection#GSFTracks',
    'TrackCollection#HLT_TrackCollection_InDetTrigTrackSlimmer_Bjet_EFID',
    'TrackCollection#HLT_TrackCollection_InDetTrigTrackSlimmer_Bphysics_EFID',
    'TrackCollection#HLT_TrackCollection_InDetTrigTrackSlimmer_CosmicsN_EFID',
    'TrackCollection#HLT_TrackCollection_InDetTrigTrackSlimmer_Electron_EFID',
    'TrackCollection#HLT_TrackCollection_InDetTrigTrackSlimmer_FullScan_EFID',
    'TrackCollection#HLT_TrackCollection_InDetTrigTrackSlimmer_Muon_EFID',
    'TrackCollection#HLT_TrackCollection_InDetTrigTrackSlimmer_Photon_EFID',
    'TrackCollection#HLT_TrackCollection_InDetTrigTrackSlimmer_Tau_EFID',
    'TrackCollection#MuonSpectrometerTracks',
    'TrackCollection#PixelPrdAssociationTracks',
    'TrackCollection#ResolvedForwardTracks',
    'TrackCollection#Tracks',
    'xAOD::EventShapeAuxInfo#Kt4EMPFlowEventShapeAux.',
    'xAOD::EventShapeAuxInfo#Kt4EMTopoEventShapeAux.',
    'xAOD::EventShapeAuxInfo#Kt4LCTopoEventShapeAux.',
    'xAOD::EventShapeAuxInfo#NeutralParticleFlowIsoCentralEventShapeAux.',
    'xAOD::EventShapeAuxInfo#NeutralParticleFlowIsoForwardEventShapeAux.',
    'xAOD::EventShapeAuxInfo#ParticleFlowIsoCentralEventShapeAux.',
    'xAOD::EventShapeAuxInfo#ParticleFlowIsoForwardEventShapeAux.',
    'xAOD::EventShapeAuxInfo#TopoClusterIsoCentralEventShapeAux.',
    'xAOD::EventShapeAuxInfo#TopoClusterIsoForwardEventShapeAux.',
    'InDet::PixelGangedClusterAmbiguities#PixelClusterAmbiguitiesMap',
    'InDet::PixelGangedClusterAmbiguities#SplitClusterAmbiguityMap',
    'xAOD::MissingETAssociationMap#METAssoc_AntiKt4EMPFlow',
    'xAOD::MissingETAssociationMap#METAssoc_AntiKt4EMTopo',
    'xAOD::MissingETAssociationMap#METAssoc_AntiKt4LCTopo',
    'xAOD::TrigNavigation#TrigNavigation',
    'LArFebErrorSummary#LArFebErrorSummary',
    'ComTime#TRT_Phase',
    'xAOD::EventShape#Kt4EMPFlowEventShape',
    'xAOD::EventShape#Kt4EMTopoEventShape',
    'xAOD::EventShape#Kt4LCTopoEventShape',
    'xAOD::EventShape#NeutralParticleFlowIsoCentralEventShape',
    'xAOD::EventShape#NeutralParticleFlowIsoForwardEventShape',
    'xAOD::EventShape#ParticleFlowIsoCentralEventShape',
    'xAOD::EventShape#ParticleFlowIsoForwardEventShape',
    'xAOD::EventShape#TopoClusterIsoCentralEventShape',
    'xAOD::EventShape#TopoClusterIsoForwardEventShape',
    'RawInfoSummaryForTag#RawInfoSummaryForTag',
    'xAOD::JetEtRoIAuxInfo#LVL1JetEtRoIAux.',
    'xAOD::EnergySumRoIAuxInfo#LVL1EnergySumRoIAux.',
    'Muon::CscPrepDataContainer#CSC_Clusters',
    'CombinedMuonFeatureContainer#HLT_CombinedMuonFeatureContainer',
    'xAOD::EmTauRoIContainer#HLT_xAOD__EmTauRoIContainer_L1TopoEM',
    'xAOD::EmTauRoIContainer#HLT_xAOD__EmTauRoIContainer_L1TopoTau',
    'xAOD::EmTauRoIContainer#LVL1EmTauRoIs',
    'xAOD::BTagVertexAuxContainer#BTagging_AntiKt2TrackJFVtxAux.',
    'xAOD::BTagVertexAuxContainer#BTagging_AntiKt3TrackJFVtxAux.',
    'xAOD::BTagVertexAuxContainer#BTagging_AntiKt4EMTopoJFVtxAux.',
    'xAOD::BTagVertexAuxContainer#BTagging_AntiKt4LCTopoJFVtxAux.',
    'xAOD::BTagVertexAuxContainer#BTagging_AntiKt4TrackJFVtxAux.',
    'xAOD::BTagVertexAuxContainer#HLT_xAOD__BTagVertexContainer_BjetVertexFexAux.',
    'xAOD::TrigRingerRingsAuxContainer#HLT_xAOD__TrigRingerRingsContainer_TrigT2CaloEgammaAux.',
    'xAOD::TriggerTowerAuxContainer#ZdcTriggerTowersAux.',
    'xAOD::TriggerTowerAuxContainer#xAODTriggerTowersAux.',
    'xAOD::TriggerTowerAuxContainer#xAODTriggerTowersMuonAux.',
    'xAOD::ElectronContainer#Electrons',
    'xAOD::ElectronContainer#ForwardElectrons',
    'xAOD::ElectronContainer#HLT_xAOD__ElectronContainer_egamma_Electrons',
    'xAOD::BTaggingTrigAuxContainer#HLT_xAOD__BTaggingContainer_HLTBjetFexAux.',
    'xAOD::TrigSpacePointCountsAuxContainer#HLT_xAOD__TrigSpacePointCountsContainer_spacepointsAux.',
    'TrigVertexCountsCollection#HLT_TrigVertexCountsCollection_vertexcounts',
    'xAOD::TrigT2ZdcSignalsContainer#HLT_xAOD__TrigT2ZdcSignalsContainer_zdcsignals',
    'xAOD::VertexContainer#BTagging_AntiKt2TrackSecVtx',
    'xAOD::VertexContainer#BTagging_AntiKt3TrackSecVtx',
    'xAOD::VertexContainer#BTagging_AntiKt4EMTopoSecVtx',
    'xAOD::VertexContainer#BTagging_AntiKt4LCTopoSecVtx',
    'xAOD::VertexContainer#BTagging_AntiKt4TrackSecVtx',
    'xAOD::VertexContainer#GSFConversionVertices',
    'xAOD::VertexContainer#HLT_xAOD__VertexContainer_BjetSecondaryVertexFex',
    'xAOD::VertexContainer#HLT_xAOD__VertexContainer_EFHistoPrmVtx',
    'xAOD::VertexContainer#HLT_xAOD__VertexContainer_PrimVertexFTK',
    'xAOD::VertexContainer#HLT_xAOD__VertexContainer_PrimVertexFTKRaw',
    'xAOD::VertexContainer#HLT_xAOD__VertexContainer_PrimVertexFTKRefit',
    'xAOD::VertexContainer#HLT_xAOD__VertexContainer_SecondaryVertex',
    'xAOD::VertexContainer#HLT_xAOD__VertexContainer_xPrimVx',
    'xAOD::VertexContainer#MSDisplacedVertex',
    'xAOD::VertexContainer#PrimaryVertices',
    'xAOD::VertexContainer#TauSecondaryVertices',
    'xAOD::TrigBphysAuxContainer#HLT_xAOD__TrigBphysContainer_EFBMuMuFexAux.',
    'xAOD::TrigBphysAuxContainer#HLT_xAOD__TrigBphysContainer_EFBMuMuXFexAux.',
    'xAOD::TrigBphysAuxContainer#HLT_xAOD__TrigBphysContainer_EFBMuMuXFex_XAux.',
    'xAOD::TrigBphysAuxContainer#HLT_xAOD__TrigBphysContainer_EFDsPhiPiFexAux.',
    'xAOD::TrigBphysAuxContainer#HLT_xAOD__TrigBphysContainer_EFMuPairsAux.',
    'xAOD::TrigBphysAuxContainer#HLT_xAOD__TrigBphysContainer_EFMultiMuFexAux.',
    'xAOD::TrigBphysAuxContainer#HLT_xAOD__TrigBphysContainer_EFTrackMassAux.',
    'xAOD::TrigBphysAuxContainer#HLT_xAOD__TrigBphysContainer_L2BMuMuFexAux.',
    'xAOD::TrigBphysAuxContainer#HLT_xAOD__TrigBphysContainer_L2BMuMuXFexAux.',
    'xAOD::TrigBphysAuxContainer#HLT_xAOD__TrigBphysContainer_L2DiMuXFexAux.',
    'xAOD::TrigBphysAuxContainer#HLT_xAOD__TrigBphysContainer_L2DsPhiPiFexDsAux.',
    'xAOD::TrigBphysAuxContainer#HLT_xAOD__TrigBphysContainer_L2DsPhiPiFexPhiAux.',
    'xAOD::TrigBphysAuxContainer#HLT_xAOD__TrigBphysContainer_L2JpsieeFexAux.',
    'xAOD::TrigBphysAuxContainer#HLT_xAOD__TrigBphysContainer_L2MultiMuFexAux.',
    'xAOD::TrigBphysAuxContainer#HLT_xAOD__TrigBphysContainer_L2TrackMassAux.',
    'xAOD::CPMTowerAuxContainer#CPMTowersAux.',
    'xAOD::CPMTowerAuxContainer#CPMTowersOverlapAux.',
    'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection',
    'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_SplitJet',
    'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_SuperRoi',
    'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_TrigJetRec',
    'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_TrigT2CaloJet',
    'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_TrigT2CosmicJet',
    'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_forID',
    'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_forID1',
    'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_forID2',
    'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_forID3',
    'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_forMS',
    'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_initialRoI',
    'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_secondaryRoI_EF',
    'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_secondaryRoI_HLT',
    'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_secondaryRoI_L2',
    'xAOD::CMXRoIContainer#CMXRoIs',
    'xAOD::CMXCPTobAuxContainer#CMXCPTobsAux.',
    'xAOD::MissingETContainer#MET_Calo',
    'xAOD::MissingETContainer#MET_Core_AntiKt4EMPFlow',
    'xAOD::MissingETContainer#MET_Core_AntiKt4EMTopo',
    'xAOD::MissingETContainer#MET_Core_AntiKt4LCTopo',
    'xAOD::MissingETContainer#MET_EMTopo',
    'xAOD::MissingETContainer#MET_EMTopoRegions',
    'xAOD::MissingETContainer#MET_LocHadTopo',
    'xAOD::MissingETContainer#MET_LocHadTopoRegions',
    'xAOD::MissingETContainer#MET_Reference_AntiKt4EMPFlow',
    'xAOD::MissingETContainer#MET_Reference_AntiKt4EMTopo',
    'xAOD::MissingETContainer#MET_Reference_AntiKt4LCTopo',
    'xAOD::MissingETContainer#MET_Track',
    'xAOD::L2IsoMuonContainer#HLT_xAOD__L2IsoMuonContainer_MuonL2ISInfo',
    'xAOD::PhotonContainer#HLT_xAOD__PhotonContainer_egamma_Photons',
    'xAOD::PhotonContainer#Photons',
    'xAOD::TrigRNNOutputContainer#HLT_xAOD__TrigRNNOutputContainer_TrigRingerNeuralFex',
    'xAOD::TrigRNNOutputContainer#HLT_xAOD__TrigRNNOutputContainer_TrigTRTHTCounts',
    'xAOD::TrackParticleClusterAssociationAuxContainer#InDetTrackParticlesClusterAssociationsAux.',
    'xAOD::RODHeaderAuxContainer#RODHeadersAux.',
    'xAOD::ZdcModuleContainer#ZdcModules',
    'xAOD::TrigEMClusterAuxContainer#HLT_xAOD__TrigEMClusterContainer_TrigT2CaloEgammaAux.',
    'xAOD::ElectronAuxContainer#ElectronsAux.',
    'xAOD::ElectronAuxContainer#ForwardElectronsAux.-CellLink',
    'TrigTrackCountsCollection#HLT_TrigTrackCountsCollection_trackcounts',
    'xAOD::TrigPassBitsContainer#HLT_xAOD__TrigPassBitsContainer_passbits',
    'xAOD::TrigElectronContainer#HLT_xAOD__TrigElectronContainer_L2ElectronFex',
    'xAOD::TrigElectronContainer#HLT_xAOD__TrigElectronContainer_L2IDCaloFex',
    'TRT_BSIdErrContainer#TRT_ByteStreamIdErrs',
    'xAOD::TrigT2MbtsBitsContainer#HLT_xAOD__TrigT2MbtsBitsContainer_T2Mbts',
    'TrigSpacePointCountsCollection#HLT_TrigSpacePointCountsCollection_spacepoints',
    'xAOD::TrigVertexCountsContainer#HLT_xAOD__TrigVertexCountsContainer_vertexcounts',
    'xAOD::L2IsoMuonAuxContainer#HLT_xAOD__L2IsoMuonContainer_MuonL2ISInfoAux.',
    'InDet::TRT_DriftCircleContainer#TRT_DriftCircles',
    'xAOD::MuonSegmentContainer#MuonSegments',
    'xAOD::MuonSegmentContainer#NCB_MuonSegments',
    'xAOD::JetRoIContainer#HLT_xAOD__JetRoIContainer_L1TopoJet',
    'xAOD::JetRoIContainer#LVL1JetRoIs',
    'xAOD::JetElementAuxContainer#JetElementsAux.',
    'xAOD::JetElementAuxContainer#JetElementsOverlapAux.',
    'CaloClusterCellLinkContainer#CaloCalTopoClusters_links',
    'CaloClusterCellLinkContainer#InDetTrackParticlesAssociatedClusters_links',
    'CaloClusterCellLinkContainer#LArClusterEM7_11Nocorr_links',
    'CaloClusterCellLinkContainer#LArClusterEM_links',
    'CaloClusterCellLinkContainer#MuonClusterCollection_links',
    'CaloClusterCellLinkContainer#egammaClusters_links',
    'CaloClusterCellLinkContainer#egammaTopoSeededClusters_links',
    'xAOD::TrigPhotonContainer#HLT_xAOD__TrigPhotonContainer_L2PhotonFex',
    'xAOD::CMXCPHitsAuxContainer#CMXCPHitsAux.',
    'TrigEFBjetContainer#HLT_TrigEFBjetContainer_EFBjetFex',
    'xAOD::SlowMuonContainer#SlowMuons',
    'xAOD::CMXJetHitsAuxContainer#CMXJetHitsAux.',
    'xAOD::RODHeaderContainer#RODHeaders',
    'xAOD::MuonAuxContainer#HLT_xAOD__MuonContainer_MuTagIMO_EFAux.',
    'xAOD::MuonAuxContainer#HLT_xAOD__MuonContainer_MuonEFInfoAux.',
    'xAOD::MuonAuxContainer#HLT_xAOD__MuonContainer_MuonEFInfo_FullScanAux.',
    'xAOD::MuonAuxContainer#HLT_xAOD__MuonContainer_MuonEFInfo_MSonlyTrackParticles_FullScanAux.',
    'xAOD::MuonAuxContainer#HLT_xAOD__MuonContainer_eMuonEFInfoAux.',
    'xAOD::MuonAuxContainer#MuonsAux.',
    'xAOD::MuonAuxContainer#StausAux.-caloExtension.-cellAssociation.-clusterAssociation',
    'Muon::MdtPrepDataContainer#MDT_DriftCircles',
    'xAOD::HIEventShapeContainer#HLT_xAOD__HIEventShapeContainer_HIUE',
    'xAOD::BTagVertexContainer#BTagging_AntiKt2TrackJFVtx',
    'xAOD::BTagVertexContainer#BTagging_AntiKt3TrackJFVtx',
    'xAOD::BTagVertexContainer#BTagging_AntiKt4EMTopoJFVtx',
    'xAOD::BTagVertexContainer#BTagging_AntiKt4LCTopoJFVtx',
    'xAOD::BTagVertexContainer#BTagging_AntiKt4TrackJFVtx',
    'xAOD::BTagVertexContainer#HLT_xAOD__BTagVertexContainer_BjetVertexFex',
    'xAOD::CaloClusterTrigAuxContainer#HLT_xAOD__CaloClusterContainer_TrigCaloClusterMakerAux.',
    'xAOD::CaloClusterTrigAuxContainer#HLT_xAOD__CaloClusterContainer_TrigCaloClusterMaker_slwAux.',
    'xAOD::CaloClusterTrigAuxContainer#HLT_xAOD__CaloClusterContainer_TrigCaloClusterMaker_topoAux.',
    'xAOD::CaloClusterTrigAuxContainer#HLT_xAOD__CaloClusterContainer_TrigEFCaloCalibFexAux.',
    'xAOD::SlowMuonAuxContainer#SlowMuonsAux.',
    'xAOD::HIEventShapeAuxContainer#HLT_xAOD__HIEventShapeContainer_HIUEAux.',
    'TrigMuonEFInfoContainer#HLT_TrigMuonEFInfoContainer_MuTagIMO_EF',
    'TrigMuonEFInfoContainer#HLT_TrigMuonEFInfoContainer_MuonEFInfo',
    'TrigMuonEFInfoContainer#HLT_TrigMuonEFInfoContainer_eMuonEFInfo',
    'xAOD::TriggerTowerContainer#ZdcTriggerTowers',
    'xAOD::TriggerTowerContainer#xAODTriggerTowers',
    'xAOD::TriggerTowerContainer#xAODTriggerTowersMuon',
    'TrigMuonEFIsolationContainer#HLT_TrigMuonEFIsolationContainer_MuonEFIsolation',
    'xAOD::TrigCompositeAuxContainer#HLT_xAOD__TrigCompositeContainer_L1TopoCompositeAux.',
    'xAOD::TrigCompositeAuxContainer#HLT_xAOD__TrigCompositeContainer_L1TopoMETAux.',
    'xAOD::TrigCompositeAuxContainer#HLT_xAOD__TrigCompositeContainer_MuonRoIClusterAux.',
    'xAOD::CMXJetTobAuxContainer#CMXJetTobsAux.',
    'xAOD::ZdcModuleAuxContainer#ZdcModulesAux.',
    'xAOD::TauJetContainer#HLT_xAOD__TauJetContainer_TrigTauRecMerged',
    'xAOD::TauJetContainer#HLT_xAOD__TauJetContainer_TrigTauRecPreselection',
    'xAOD::TauJetContainer#TauJets',
    'xAOD::PFOContainer#JetETMissChargedParticleFlowObjects',
    'xAOD::PFOContainer#JetETMissNeutralParticleFlowObjects',
    'xAOD::PFOContainer#TauChargedParticleFlowObjects',
    'xAOD::PFOContainer#TauHadronicParticleFlowObjects',
    'xAOD::PFOContainer#TauNeutralParticleFlowObjects',
    'xAOD::PFOContainer#TauShotParticleFlowObjects',
    'xAOD::MuonContainer#HLT_xAOD__MuonContainer_MuTagIMO_EF',
    'xAOD::MuonContainer#HLT_xAOD__MuonContainer_MuonEFInfo',
    'xAOD::MuonContainer#HLT_xAOD__MuonContainer_MuonEFInfo_FullScan',
    'xAOD::MuonContainer#HLT_xAOD__MuonContainer_MuonEFInfo_MSonlyTrackParticles_FullScan',
    'xAOD::MuonContainer#HLT_xAOD__MuonContainer_eMuonEFInfo',
    'xAOD::MuonContainer#Muons',
    'xAOD::MuonContainer#Staus',
    'xAOD::CMXEtSumsContainer#CMXEtSums',
    'xAOD::TrigElectronAuxContainer#HLT_xAOD__TrigElectronContainer_L2ElectronFexAux.',
    'xAOD::TrigElectronAuxContainer#HLT_xAOD__TrigElectronContainer_L2IDCaloFexAux.',
    'TrigT2MbtsBitsContainer#HLT_TrigT2MbtsBitsContainer_T2Mbts',
    'xAOD::L1TopoRawDataContainer#L1TopoRawData',
    'xAOD::CPMTobRoIAuxContainer#CPMTobRoIsAux.',
    'xAOD::CPMTobRoIAuxContainer#CPMTobRoIsRoIBAux.',
    'Muon::TgcCoinDataContainer#TrigT1CoinDataCollection',
    'Muon::TgcCoinDataContainer#TrigT1CoinDataCollectionNextBC',
    'Muon::TgcCoinDataContainer#TrigT1CoinDataCollectionPriorBC',
    'Muon::RpcCoinDataContainer#RPC_triggerHits',
    'IsoMuonFeatureContainer#HLT_IsoMuonFeatureContainer',
    'xAOD::CMXEtSumsAuxContainer#CMXEtSumsAux.',
    'MuonFeatureContainer#HLT_MuonFeatureContainer',
    'xAOD::L2CombinedMuonContainer#HLT_xAOD__L2CombinedMuonContainer_MuonL2CBInfo',
    'xAOD::L2CombinedMuonAuxContainer#HLT_xAOD__L2CombinedMuonContainer_MuonL2CBInfoAux.',
    'xAOD::JEMTobRoIContainer#JEMTobRoIs',
    'xAOD::JEMTobRoIContainer#JEMTobRoIsRoIB',
    'xAOD::L2StandAloneMuonAuxContainer#HLT_xAOD__L2StandAloneMuonContainer_MuonL2SAInfoAux.',
    'xAOD::TrigSpacePointCountsContainer#HLT_xAOD__TrigSpacePointCountsContainer_spacepoints',
    'xAOD::TrackParticleAuxContainer#CombinedMuonTrackParticlesAux.'+trackParticleAuxExclusions,
    'xAOD::TrackParticleAuxContainer#CombinedStauTrackParticlesAux.'+trackParticleAuxExclusions,
    'xAOD::TrackParticleAuxContainer#ExtrapolatedMuonTrackParticlesAux'+trackParticleAuxExclusions,
    'xAOD::TrackParticleAuxContainer#ExtrapolatedStauTrackParticlesAux.'+trackParticleAuxExclusions,
    'xAOD::TrackParticleAuxContainer#GSFTrackParticlesAux.'+trackParticleAuxExclusions,
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnvIOTRT_CosmicsN_EFIDAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_BjetPrmVtx_FTFAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_EFIDAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_FTFAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_FTKAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_FTKRefitAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_FTKRefit_IDTrigAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_FTK_IDTrigAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_IDTrigAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bphysics_EFIDAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bphysics_FTFAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bphysics_IDTrigAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_CosmicsN_EFIDAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Electron_EFIDAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Electron_FTFAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Electron_IDTrigAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Electron_L2IDAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_FullScan_EFIDAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_MuonIso_EFIDAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_EFIDAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_FTFAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_FTKAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_FTKRefitAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_FTKRefit_IDTrigAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_FTK_IDTrigAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_IDTrigAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Photon_EFIDAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_TauCore_FTFAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_TauIso_FTFAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_EFIDAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTFAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTKAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTKRefitAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTKRefit_IDTrigAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTK_IDTrigAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_IDTrigAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_minBias_EFIDAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_MuTagIMO_EF_CombTrackParticlesAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_MuTagIMO_EF_ExtrapTrackParticlesAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_MuonEFInfo_CombTrackParticlesAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_MuonEFInfo_CombTrackParticles_FullScanAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_MuonEFInfo_ExtrapTrackParticlesAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_MuonEFInfo_ExtrapTrackParticles_FullScanAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_eMuonEFInfo_CombTrackParticlesAux.',
    'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_eMuonEFInfo_ExtrapTrackParticlesAux.',
    'xAOD::TrackParticleAuxContainer#InDetForwardTrackParticlesAux.'+trackParticleAuxExclusions,
    'xAOD::TrackParticleAuxContainer#InDetPixelPrdAssociationTrackParticlesAux.'+trackParticleAuxExclusions,
    'xAOD::TrackParticleAuxContainer#InDetTrackParticlesAux.'+trackParticleAuxExclusions,
    'xAOD::TrackParticleAuxContainer#MSonlyTrackletsAux.'+trackParticleAuxExclusions,
    'xAOD::TrackParticleAuxContainer#MuonSpectrometerTrackParticlesAux.'+trackParticleAuxExclusions,
    'TrigPassFlagsCollection#HLT_TrigPassFlagsCollection_isEM',
    'TrigPassFlagsCollection#HLT_TrigPassFlagsCollection_passflags',
    'TrigEMClusterContainer#HLT_TrigEMClusterContainer_TrigT2CaloCosmic',
    'xAOD::CaloClusterContainer#CaloCalTopoClusters',
    'xAOD::CaloClusterContainer#ForwardElectronClusters',
    'xAOD::CaloClusterContainer#HLT_xAOD__CaloClusterContainer_TrigCaloClusterMaker',
    'xAOD::CaloClusterContainer#HLT_xAOD__CaloClusterContainer_TrigCaloClusterMaker_slw',
    'xAOD::CaloClusterContainer#HLT_xAOD__CaloClusterContainer_TrigCaloClusterMaker_topo',
    'xAOD::CaloClusterContainer#HLT_xAOD__CaloClusterContainer_TrigEFCaloCalibFex',
    'xAOD::CaloClusterContainer#InDetTrackParticlesAssociatedClusters',
    'xAOD::CaloClusterContainer#LArClusterEM',
    'xAOD::CaloClusterContainer#LArClusterEM7_11Nocorr',
    'xAOD::CaloClusterContainer#MuonClusterCollection',
    'xAOD::CaloClusterContainer#TauPi0Clusters',
    'xAOD::CaloClusterContainer#egammaClusters',
    'xAOD::CaloClusterContainer#egammaTopoSeededClusters',
    'xAOD::PFOAuxContainer#JetETMissChargedParticleFlowObjectsAux.',
    'xAOD::PFOAuxContainer#JetETMissNeutralParticleFlowObjectsAux.',
    'xAOD::PFOAuxContainer#TauChargedParticleFlowObjectsAux.',
    'xAOD::PFOAuxContainer#TauHadronicParticleFlowObjectsAux.',
    'xAOD::PFOAuxContainer#TauNeutralParticleFlowObjectsAux.',
    'xAOD::PFOAuxContainer#TauShotParticleFlowObjectsAux.',
    'MuonFeatureDetailsContainer#HLT_MuonFeatureDetailsContainer',
    'xAOD::CMXRoIAuxContainer#CMXRoIsAux.',
    'xAOD::MissingETAuxContainer#MET_CaloAux.',
    'xAOD::MissingETAuxContainer#MET_Core_AntiKt4EMPFlowAux.',
    'xAOD::MissingETAuxContainer#MET_Core_AntiKt4EMTopoAux.',
    'xAOD::MissingETAuxContainer#MET_Core_AntiKt4LCTopoAux.',
    'xAOD::MissingETAuxContainer#MET_EMTopoAux.',
    'xAOD::MissingETAuxContainer#MET_EMTopoRegionsAux.',
    'xAOD::MissingETAuxContainer#MET_LocHadTopoAux.',
    'xAOD::MissingETAuxContainer#MET_LocHadTopoRegionsAux.',
    'xAOD::MissingETAuxContainer#MET_Reference_AntiKt4EMPFlowAux.',
    'xAOD::MissingETAuxContainer#MET_Reference_AntiKt4EMTopoAux.',
    'xAOD::MissingETAuxContainer#MET_Reference_AntiKt4LCTopoAux.',
    'xAOD::MissingETAuxContainer#MET_TrackAux.',
    'xAOD::CMXJetTobContainer#CMXJetTobs',
    'xAOD::TrigPassBitsAuxContainer#HLT_xAOD__TrigPassBitsContainer_passbitsAux.',
    'xAOD::JEMEtSumsContainer#JEMEtSums',
    'xAOD::CPMTobRoIContainer#CPMTobRoIs',
    'xAOD::CPMTobRoIContainer#CPMTobRoIsRoIB',
    'Muon::CscStripPrepDataContainer#CSC_Measurements',
    'xAOD::BTaggingAuxContainer#BTagging_AntiKt2TrackAux.',
    'xAOD::BTaggingAuxContainer#BTagging_AntiKt3TrackAux.',
    'xAOD::BTaggingAuxContainer#BTagging_AntiKt4EMTopoAux.',
    'xAOD::BTaggingAuxContainer#BTagging_AntiKt4LCTopoAux.',
    'xAOD::BTaggingAuxContainer#BTagging_AntiKt4TrackAux.',
    'xAOD::JetContainer#AntiKt10LCTopoJets',
    'xAOD::JetContainer#AntiKt2PV0TrackJets',
    'xAOD::JetContainer#AntiKt3PV0TrackJets',
    'xAOD::JetContainer#AntiKt4EMPFlowJets',
    'xAOD::JetContainer#AntiKt4EMTopoJets',
    'xAOD::JetContainer#AntiKt4LCTopoJets',
    'xAOD::JetContainer#AntiKt4PV0TrackJets',
    'xAOD::JetContainer#CamKt12LCTopoJets',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_EFJet',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_FarawayJet',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_SplitJet',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_SuperRoi',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_TrigHLTJetDSSelectorCollection',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_TrigTauJet',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_a10TTemnojcalibFS',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_a10TThadnojcalibFS',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_a10r_tcemsubjesFS',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_a10tcemjesFS',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_a10tcemjesPS',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_a10tcemnojcalibFS',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_a10tcemnojcalibPS',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_a10tcemsubFS',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_a10tcemsubjesFS',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_a10tclcwjesFS',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_a10tclcwjesPS',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_a10tclcwnojcalibFS',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_a10tclcwnojcalibPS',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_a10tclcwsubFS',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_a10tclcwsubjesFS',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_a3ionemsubjesFS',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_a4TTemnojcalibFS',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_a4TThadnojcalibFS',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_a4ionemsubjesFS',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_a4tcemjesFS',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_a4tcemjesPS',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_a4tcemnojcalibFS',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_a4tcemnojcalibPS',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_a4tcemsubFS',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_a4tcemsubjesFS',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_a4tclcwjesFS',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_a4tclcwjesPS',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_a4tclcwnojcalibFS',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_a4tclcwnojcalibPS',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_a4tclcwsubFS',
    'xAOD::JetContainer#HLT_xAOD__JetContainer_a4tclcwsubjesFS',
    'xAOD::TrackParticleClusterAssociationContainer#InDetTrackParticlesClusterAssociations',
    'xAOD::CMXCPTobContainer#CMXCPTobs',
    'xAOD::PhotonAuxContainer#PhotonsAux.',
    'xAOD::EmTauRoIAuxContainer#HLT_xAOD__EmTauRoIContainer_L1TopoEMAux.',
    'xAOD::EmTauRoIAuxContainer#HLT_xAOD__EmTauRoIContainer_L1TopoTauAux.',
    'xAOD::EmTauRoIAuxContainer#LVL1EmTauRoIsAux.',
    'xAOD::TrigTrackCountsAuxContainer#HLT_xAOD__TrigTrackCountsContainer_trackcountsAux.',
    'Muon::RpcPrepDataContainer#RPC_Measurements',
    'xAOD::TrigT2MbtsBitsAuxContainer#HLT_xAOD__TrigT2MbtsBitsContainer_T2MbtsAux.',
    'xAOD::JetElementContainer#JetElements',
    'xAOD::JetElementContainer#JetElementsOverlap',
    'xAOD::BTaggingContainer#BTagging_AntiKt2Track',
    'xAOD::BTaggingContainer#BTagging_AntiKt3Track',
    'xAOD::BTaggingContainer#BTagging_AntiKt4EMTopo',
    'xAOD::BTaggingContainer#BTagging_AntiKt4LCTopo',
    'xAOD::BTaggingContainer#BTagging_AntiKt4Track',
    'xAOD::BTaggingContainer#HLT_xAOD__BTaggingContainer_HLTBjetFex',
    'xAOD::TrigEMClusterContainer#HLT_xAOD__TrigEMClusterContainer_TrigT2CaloEgamma',
    'xAOD::CPMTowerContainer#CPMTowers',
    'xAOD::CPMTowerContainer#CPMTowersOverlap',
    'xAOD::TrigRNNOutputAuxContainer#HLT_xAOD__TrigRNNOutputContainer_TrigRingerNeuralFexAux.',
    'xAOD::TrigRNNOutputAuxContainer#HLT_xAOD__TrigRNNOutputContainer_TrigTRTHTCountsAux.',
    'xAOD::PhotonTrigAuxContainer#HLT_xAOD__PhotonContainer_egamma_PhotonsAux.',
    'TrigMissingETContainer#HLT_TrigMissingETContainer_EFJetEtSum',
    'TrigMissingETContainer#HLT_TrigMissingETContainer_L2JetEtSum',
    'TrigMissingETContainer#HLT_TrigMissingETContainer_L2MissingET_FEB',
    'TrigMissingETContainer#HLT_TrigMissingETContainer_T2MissingET',
    'TrigMissingETContainer#HLT_TrigMissingETContainer_TrigEFMissingET',
    'TrigMissingETContainer#HLT_TrigMissingETContainer_TrigEFMissingET_FEB',
    'TrigMissingETContainer#HLT_TrigMissingETContainer_TrigEFMissingET_topocl',
    'TrigMissingETContainer#HLT_TrigMissingETContainer_TrigEFMissingET_topocl_PS',
    'TrigMuonClusterFeatureContainer#HLT_TrigMuonClusterFeatureContainer_MuonCluster',
    'xAOD::CMXJetHitsContainer#CMXJetHits',
    'xAOD::DiTauJetContainer#DiTauJets',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_EFJetAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_FarawayJetAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_SplitJetAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_SuperRoiAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_TrigHLTJetDSSelectorCollectionAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_TrigTauJetAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_a10TTemnojcalibFSAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_a10TThadnojcalibFSAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_a10r_tcemsubjesFSAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_a10tcemjesFSAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_a10tcemjesPSAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_a10tcemnojcalibFSAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_a10tcemnojcalibPSAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_a10tcemsubFSAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_a10tcemsubjesFSAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_a10tclcwjesFSAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_a10tclcwjesPSAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_a10tclcwnojcalibFSAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_a10tclcwnojcalibPSAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_a10tclcwsubFSAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_a10tclcwsubjesFSAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_a3ionemsubjesFSAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_a4TTemnojcalibFSAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_a4TThadnojcalibFSAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_a4ionemsubjesFSAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_a4tcemjesFSAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_a4tcemjesPSAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_a4tcemnojcalibFSAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_a4tcemnojcalibPSAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_a4tcemsubFSAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_a4tcemsubjesFSAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_a4tclcwjesFSAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_a4tclcwjesPSAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_a4tclcwnojcalibFSAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_a4tclcwnojcalibPSAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_a4tclcwsubFSAux.',
    'xAOD::JetTrigAuxContainer#HLT_xAOD__JetContainer_a4tclcwsubjesFSAux.',
    'xAOD::TrackParticleContainer#CombinedMuonTrackParticles',
    'xAOD::TrackParticleContainer#CombinedStauTrackParticles',
    'xAOD::TrackParticleContainer#ExtrapolatedMuonTrackParticles',
    'xAOD::TrackParticleContainer#ExtrapolatedStauTrackParticles',
    'xAOD::TrackParticleContainer#GSFTrackParticles',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnvIOTRT_CosmicsN_EFID',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_EFID',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_FTF',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_FTK',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_FTKRefit',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_FTKRefit_IDTrig',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_FTK_IDTrig',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_IDTrig',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bphysics_EFID',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bphysics_FTF',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bphysics_IDTrig',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_CosmicsN_EFID',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Electron_EFID',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Electron_FTF',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Electron_IDTrig',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Electron_L2ID',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_FullScan_EFID',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_MuonIso_EFID',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_EFID',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_FTF',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_FTK',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_FTKRefit',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_FTKRefit_IDTrig',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_FTK_IDTrig',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_IDTrig',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Photon_EFID',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_TauCore_FTF',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_TauIso_FTF',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_EFID',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTF',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTK',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTKRefit',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTKRefit_IDTrig',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTK_IDTrig',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_IDTrig',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_minBias_EFID',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_MuTagIMO_EF_CombTrackParticles',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_MuTagIMO_EF_ExtrapTrackParticles',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_MuonEFInfo_CombTrackParticles',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_MuonEFInfo_CombTrackParticles_FullScan',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_MuonEFInfo_ExtrapTrackParticles',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_MuonEFInfo_ExtrapTrackParticles_FullScan',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_eMuonEFInfo_CombTrackParticles',
    'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_eMuonEFInfo_ExtrapTrackParticles',
    'xAOD::TrackParticleContainer#InDetForwardTrackParticles',
    'xAOD::TrackParticleContainer#InDetPixelPrdAssociationTrackParticles',
    'xAOD::TrackParticleContainer#InDetTrackParticles',
    'xAOD::TrackParticleContainer#MSonlyTracklets',
    'xAOD::TrackParticleContainer#MuonSpectrometerTrackParticles',
    'xAOD::JetRoIAuxContainer#HLT_xAOD__JetRoIContainer_L1TopoJetAux.',
    'xAOD::JetRoIAuxContainer#LVL1JetRoIsAux.',
    'xAOD::TrigMissingETAuxContainer#HLT_xAOD__TrigMissingETContainer_EFJetEtSumAux.',
    'xAOD::TrigMissingETAuxContainer#HLT_xAOD__TrigMissingETContainer_EFMissingET_Fex_2sidednoiseSupp_PUCAux.',
    'xAOD::TrigMissingETAuxContainer#HLT_xAOD__TrigMissingETContainer_T2MissingETAux.',
    'xAOD::TrigMissingETAuxContainer#HLT_xAOD__TrigMissingETContainer_TrigEFMissingETAux.',
    'xAOD::TrigMissingETAuxContainer#HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_FEBAux.',
    'xAOD::TrigMissingETAuxContainer#HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_mhtAux.',
    'xAOD::TrigMissingETAuxContainer#HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topoclAux.',
    'xAOD::TrigMissingETAuxContainer#HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl_PSAux.',
    'xAOD::TrigMissingETAuxContainer#HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl_PUCAux.',
    'xAOD::TrigMissingETAuxContainer#HLT_xAOD__TrigMissingETContainer_TrigL2MissingET_FEBAux.',
    'xAOD::MuonRoIAuxContainer#HLT_xAOD__MuonRoIContainer_L1TopoMuonAux.',
    'xAOD::MuonRoIAuxContainer#LVL1MuonRoIsAux.',
    'TileTrackMuFeatureContainer#HLT_TileTrackMuFeatureContainer',
    'xAOD::JetAuxContainer#AntiKt10LCTopoJetsAux.',
    'xAOD::JetAuxContainer#AntiKt2PV0TrackJetsAux.',
    'xAOD::JetAuxContainer#AntiKt3PV0TrackJetsAux.',
    'xAOD::JetAuxContainer#AntiKt4EMPFlowJetsAux.',
    'xAOD::JetAuxContainer#AntiKt4EMTopoJetsAux.',
    'xAOD::JetAuxContainer#AntiKt4LCTopoJetsAux.',
    'xAOD::JetAuxContainer#AntiKt4PV0TrackJetsAux.',
    'xAOD::JetAuxContainer#CamKt12LCTopoJetsAux.',
    'TileMuFeatureContainer#HLT_TileMuFeatureContainer',
    'RpcSectorLogicContainer#RPC_SECTORLOGIC',
    'TrigVertexCollection#HLT_TrigVertexCollection_TrigBeamSpotVertex',
    'TrigVertexCollection#HLT_TrigVertexCollection_TrigL2SiTrackFinder_FullScan_ZF_Only',
    'TrigInDetTrackCollection#HLT_TrigInDetTrackCollection',
    'xAOD::TrigRingerRingsContainer#HLT_xAOD__TrigRingerRingsContainer_TrigT2CaloEgamma',
    'xAOD::CaloClusterAuxContainer#CaloCalTopoClustersAux.',
    'xAOD::CaloClusterAuxContainer#ForwardElectronClustersAux.',
    'xAOD::CaloClusterAuxContainer#InDetTrackParticlesAssociatedClustersAux.',
    'xAOD::CaloClusterAuxContainer#LArClusterEM7_11NocorrAux.',
    'xAOD::CaloClusterAuxContainer#LArClusterEMAux.',
    'xAOD::CaloClusterAuxContainer#MuonClusterCollectionAux.',
    'xAOD::CaloClusterAuxContainer#TauPi0ClustersAux.',
    'xAOD::CaloClusterAuxContainer#egammaClustersAux.',
    'xAOD::CaloClusterAuxContainer#egammaTopoSeededClustersAux.',
    'xAOD::CMXCPHitsContainer#CMXCPHits',
    'xAOD::TrigT2ZdcSignalsAuxContainer#HLT_xAOD__TrigT2ZdcSignalsContainer_zdcsignalsAux.',
    'xAOD::DiTauJetAuxContainer#DiTauJetsAux.',
    'xAOD::JEMTobRoIAuxContainer#JEMTobRoIsAux.',
    'xAOD::JEMTobRoIAuxContainer#JEMTobRoIsRoIBAux.',
    'xAOD::L2StandAloneMuonContainer#HLT_xAOD__L2StandAloneMuonContainer_MuonL2SAInfo',
    'xAOD::TauJetAuxContainer#HLT_xAOD__TauJetContainer_TrigTauRecMergedAux.',
    'xAOD::TauJetAuxContainer#HLT_xAOD__TauJetContainer_TrigTauRecPreselectionAux.',
    'xAOD::TauJetAuxContainer#TauJetsAux.',
    'TrigPassBitsCollection#HLT_TrigPassBitsCollection_passbits',
    'xAOD::MuonRoIContainer#HLT_xAOD__MuonRoIContainer_L1TopoMuon',
    'xAOD::MuonRoIContainer#LVL1MuonRoIs',
    'xAOD::JEMEtSumsAuxContainer#JEMEtSumsAux.',
    'xAOD::TrigPhotonAuxContainer#HLT_xAOD__TrigPhotonContainer_L2PhotonFexAux.',
    'InDetBSErrContainer#PixelByteStreamErrs',
    'InDetBSErrContainer#SCT_ByteStreamErrs',
    'xAOD::TrigVertexCountsAuxContainer#HLT_xAOD__TrigVertexCountsContainer_vertexcountsAux.',
    'xAOD::VertexAuxContainer#BTagging_AntiKt2TrackSecVtxAux.-vxTrackAtVertex',
    'xAOD::VertexAuxContainer#BTagging_AntiKt3TrackSecVtxAux.-vxTrackAtVertex',
    'xAOD::VertexAuxContainer#BTagging_AntiKt4EMTopoSecVtxAux.-vxTrackAtVertex',
    'xAOD::VertexAuxContainer#BTagging_AntiKt4LCTopoSecVtxAux.-vxTrackAtVertex',
    'xAOD::VertexAuxContainer#BTagging_AntiKt4TrackSecVtxAux.-vxTrackAtVertex',
    'xAOD::VertexAuxContainer#GSFConversionVerticesAux.-vxTrackAtVertex',
    'xAOD::VertexAuxContainer#HLT_xAOD__VertexContainer_BjetSecondaryVertexFexAux.',
    'xAOD::VertexAuxContainer#HLT_xAOD__VertexContainer_EFHistoPrmVtxAux.',
    'xAOD::VertexAuxContainer#HLT_xAOD__VertexContainer_PrimVertexFTKAux.',
    'xAOD::VertexAuxContainer#HLT_xAOD__VertexContainer_PrimVertexFTKRawAux.',
    'xAOD::VertexAuxContainer#HLT_xAOD__VertexContainer_PrimVertexFTKRefitAux.',
    'xAOD::VertexAuxContainer#HLT_xAOD__VertexContainer_SecondaryVertexAux.',
    'xAOD::VertexAuxContainer#HLT_xAOD__VertexContainer_xPrimVxAux.',
    'xAOD::VertexAuxContainer#MSDisplacedVertexAux.',
    'xAOD::VertexAuxContainer#PrimaryVerticesAux.-vxTrackAtVertex',
    'xAOD::VertexAuxContainer#TauSecondaryVerticesAux.-vxTrackAtVertex',
    'TrigOperationalInfoCollection#HLT_TrigOperationalInfoCollection_EXPRESS_OPI_HLT',
    'TRT_BSErrContainer#TRT_ByteStreamErrs',
    'xAOD::TrigTrackCountsContainer#HLT_xAOD__TrigTrackCountsContainer_trackcounts',
    'xAOD::TrigBphysContainer#HLT_xAOD__TrigBphysContainer_EFBMuMuFex',
    'xAOD::TrigBphysContainer#HLT_xAOD__TrigBphysContainer_EFBMuMuXFex',
    'xAOD::TrigBphysContainer#HLT_xAOD__TrigBphysContainer_EFBMuMuXFex_X',
    'xAOD::TrigBphysContainer#HLT_xAOD__TrigBphysContainer_EFDsPhiPiFex',
    'xAOD::TrigBphysContainer#HLT_xAOD__TrigBphysContainer_EFMuPairs',
    'xAOD::TrigBphysContainer#HLT_xAOD__TrigBphysContainer_EFMultiMuFex',
    'xAOD::TrigBphysContainer#HLT_xAOD__TrigBphysContainer_EFTrackMass',
    'xAOD::TrigBphysContainer#HLT_xAOD__TrigBphysContainer_L2BMuMuFex',
    'xAOD::TrigBphysContainer#HLT_xAOD__TrigBphysContainer_L2BMuMuXFex',
    'xAOD::TrigBphysContainer#HLT_xAOD__TrigBphysContainer_L2DiMuXFex',
    'xAOD::TrigBphysContainer#HLT_xAOD__TrigBphysContainer_L2DsPhiPiFexDs',
    'xAOD::TrigBphysContainer#HLT_xAOD__TrigBphysContainer_L2DsPhiPiFexPhi',
    'xAOD::TrigBphysContainer#HLT_xAOD__TrigBphysContainer_L2JpsieeFex',
    'xAOD::TrigBphysContainer#HLT_xAOD__TrigBphysContainer_L2MultiMuFex',
    'xAOD::TrigBphysContainer#HLT_xAOD__TrigBphysContainer_L2TrackMass',
    'xAOD::TrigCompositeContainer#HLT_xAOD__TrigCompositeContainer_L1TopoComposite',
    'xAOD::TrigCompositeContainer#HLT_xAOD__TrigCompositeContainer_L1TopoMET',
    'xAOD::TrigCompositeContainer#HLT_xAOD__TrigCompositeContainer_MuonRoICluster',
    'xAOD::L1TopoRawDataAuxContainer#L1TopoRawDataAux.',
    'xAOD::ElectronTrigAuxContainer#HLT_xAOD__ElectronContainer_egamma_ElectronsAux.',
    'xAOD::MuonSegmentAuxContainer#MuonSegmentsAux.',
    'xAOD::MuonSegmentAuxContainer#NCB_MuonSegmentsAux.',
    'Muon::TgcPrepDataContainer#TGC_MeasurementsAllBCs',
    'MuonCaloEnergyContainer#MuonCaloEnergyCollection',
    'DataHeader#StreamESD']
)

outList.removeAllItems(excludeList)
StreamDESDM_EGAMMA.AddItem(outList())
