#====================================================================
# HION2.py
# author: Soumya Mohapatra <soumya@cern.ch>
# Application: Flow Analyses
# Triggers: MinBias
# Selection: Offline MB event selection
# Content: in Pb+Pb: All tracks (Minimal information),CaloSums,HIClusters,PrimaryVertices 
# Content: in  p+Pb: All tracks (Minimal information),CaloSums,PrimaryVertices,MBTS,ZDC 
#====================================================================



#====================================================================
# Set up common services and job object.
# This should appear in ALL derivation job options
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkMaster import *



#====================================================================
# SET UP OUTPUT STREAM  
#====================================================================
streamName = derivationFlags.WriteDAOD_HION2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HION2Stream )
HION2Stream = MSMgr.NewPoolRootStream( streamName, fileName )



#====================================================================
#Check to see if it is MC Derivation
#====================================================================
isSimulation = False
if globalflags.DataSource()=='geant4':
    isSimulation = True
print '++++++++++++++++++++++++++++++++++ Start Global Flags +++++++++++++++++++++++++++++++++++'
print globalflags
print '+++++++++++++++++++++++++++++++++++ End Global Flags +++++++++++++++++++++++++++++++++++'

#====================================================================
#Check to see if it is Pb+Pb or p+Pb Derivation
#====================================================================
from PyUtils import AthFile
af = AthFile.fopen(svcMgr.EventSelector.InputCollections[0])
project_tag = af.fileinfos['metadata']['/TagInfo']['project_name']
beam_energy = af.fileinfos['metadata']['/TagInfo']['beam_energy']
print '+++++++++++++++++++++++++++++++ project tag: ',project_tag,' +++++++++++++++++++++++++++++++'
print '+++++++++++++++++++++++++++++++ beam energy: ',beam_energy,' +++++++++++++++++++++++++++++++'
#print af.fileinfos['metadata']['/TagInfo']
is_PbPb_2015_5TeV=False
is_pPb_2016_8TeV =False
is_pPb_2016_5TeV =False
if project_tag=='data15_hi':
  is_PbPb_2015_5TeV=True
  print "Dataset Type: HeavyIon 2015" 
elif project_tag=='data16_hip8TeV':
  is_pPb_2016_8TeV =True
  print "Dataset Type: proton-Ion 8.16TeV 2016" 
elif project_tag=='data16_hip5TeV':
  is_pPb_2016_5TeV =True
  print "Dataset Type: proton-Ion 5.02TeV 2016" 
else:
  if isSimulation:
    print "Dataset Type: Simulation" 
  else:
    print "Unknown Dataset: Quitting" 
    exit()
#====================================================================
#SKIMMING TOOL
#====================================================================
#Triggers
if is_PbPb_2015_5TeV:
    mb_triggers=['HLT_noalg_mb_L1TE50','HLT_mb_sptrk_ion_L1ZDC_A_C_VTE50']
    vn_triggers=['HLT_hi_v23_th14_L1TE50', 'HLT_hi_v23_th15_L1TE50'     , 'HLT_hi_v2_th10_L1TE50',\
                 'HLT_hi_v2_th13_L1TE50' , 'HLT_hi_v2_th13_veto3_L1TE50', 'HLT_hi_v2_th14_L1TE50',\
                 'HLT_hi_v2_th15_L1TE50' , 'HLT_hi_v2_th16_L1TE50'      , 'HLT_hi_v2_th5_L1TE50' ,\
                 'HLT_hi_v3_th10_L1TE50' , 'HLT_hi_v3_th13_L1TE50'      , 'HLT_hi_v3_th13_veto2_L1TE50',\
                 'HLT_hi_v3_th14_L1TE50' , 'HLT_hi_v3_th15_L1TE50'      , 'HLT_hi_v3_th16_L1TE50', 'HLT_hi_v3_th5_L1TE50']
    uc_triggers=['HLT_hi_th1_ucc_L1TE10000','HLT_hi_th2_ucc_L1TE10000','HLT_hi_th3_ucc_L1TE10000',\
                 'HLT_hi_th1_ucc_L1TE12000','HLT_hi_th2_ucc_L1TE12000','HLT_hi_th3_ucc_L1TE12000',\
                 'HLT_hi_th1_ucc_L1TE14000','HLT_hi_th2_ucc_L1TE14000','HLT_hi_th3_ucc_L1TE14000']
    triggers=mb_triggers+vn_triggers+uc_triggers
elif is_pPb_2016_8TeV:
    vn_triggers=[
      "HLT_hi_v2A_th05p_mb_sp3100_trk160_hmt_L1TE120",
      "HLT_hi_v2A_th05p_mb_sp4100_trk200_hmt_L1TE120",
      "HLT_hi_v2A_th05p_mb_sp4100_trk200_hmt_L1TE160",
      "HLT_hi_v2A_th05p_mb_sp4800_trk240_hmt_L1TE120",
      "HLT_hi_v2A_th05p_mb_sp5000_trk260_hmt_L1TE160",
      "HLT_hi_v2A_th05p_mb_sp4100_pusup900_trk200_hmt_L1TE200",
      "HLT_hi_v2A_th05p_mb_sp4500_pusup1000_trk220_hmt_L1TE200",
      "HLT_hi_v2A_th05p_mb_sp4800_pusup1100_trk240_hmt_L1TE200",
      "HLT_hi_v2A_th05p_mb_sp3100_pusup700_trk160_hmt_L1TE120",
      "HLT_hi_v2A_th05p_mb_sp4100_pusup900_trk200_hmt_L1TE120",
      "HLT_hi_v2A_th05p_mb_sp4100_pusup900_trk200_hmt_L1TE160",
      "HLT_hi_v2A_th05p_mb_sp4800_pusup1100_trk240_hmt_L1TE120",
      "HLT_hi_v2A_th05p_mb_sp5000_pusup1200_trk260_hmt_L1TE160",
      "HLT_hi_v2C_th05p_mb_sp3100_trk160_hmt_L1TE120",
      "HLT_hi_v2C_th05p_mb_sp4100_trk200_hmt_L1TE120",
      "HLT_hi_v2C_th05p_mb_sp4100_trk200_hmt_L1TE160",
      "HLT_hi_v2C_th05p_mb_sp4800_trk240_hmt_L1TE120",
      "HLT_hi_v2C_th05p_mb_sp5000_trk260_hmt_L1TE160",
      "HLT_hi_v2C_th05p_mb_sp4100_pusup900_trk200_hmt_L1TE200",
      "HLT_hi_v2C_th05p_mb_sp4500_pusup1000_trk220_hmt_L1TE200",
      "HLT_hi_v2C_th05p_mb_sp4800_pusup1100_trk240_hmt_L1TE200",
      "HLT_hi_v2C_th05p_mb_sp3100_pusup700_trk160_hmt_L1TE120",
      "HLT_hi_v2C_th05p_mb_sp4100_pusup900_trk200_hmt_L1TE120",
      "HLT_hi_v2C_th05p_mb_sp4100_pusup900_trk200_hmt_L1TE160",
      "HLT_hi_v2C_th05p_mb_sp4800_pusup1100_trk240_hmt_L1TE120",
      "HLT_hi_v2C_th05p_mb_sp5000_pusup1200_trk260_hmt_L1TE160"]
    hmt_triggers=[
      "HLT_mb_sptrk_L1MBTS_1",
      "HLT_mb_sp2400_pusup500_trk120_hmt_L1TE20",
      "HLT_mb_sp2800_pusup600_trk140_hmt_L1TE20",
      "HLT_mb_sp2800_pusup600_trk140_hmt_L1TE50",
      "HLT_mb_sp3100_pusup700_trk160_hmt_L1TE50",
      "HLT_mb_sp3100_pusup700_trk160_hmt_L1TE70",
      "HLT_mb_sp3500_pusup800_trk180_hmt_L1TE70",
      "HLT_mb_sp3500_pusup800_trk180_hmt_L1TE90",
      "HLT_mb_sp4100_pusup900_trk200_hmt_L1TE90",
      "HLT_mb_sp4100_pusup900_trk200_hmt_L1TE120",
      "HLT_mb_sp4500_pusup1000_trk220_hmt_L1TE120",
      "HLT_mb_sp4800_pusup1100_trk240_hmt_L1TE160",
      "HLT_mb_sp5000_pusup1100_trk240_hmt_L1TE200",
      "HLT_mb_sp5000_pusup1200_trk260_hmt_L1TE160",
      "HLT_mb_sp5600_pusup1300_trk260_hmt_L1TE200",
      "HLT_mb_sp5200_pusup1300_trk280_hmt_L1TE160",
      "HLT_mb_sp6000_pusup1400_trk280_hmt_L1TE200"]
    muon_triggers=[
      "HLT_mu4_mb_sp1200_pusup200_trk60_hmt_L1MU4",
      "HLT_mu4_mb_sp1200_pusup200_trk60_hmt_L1MU4_TE10.0ETA24",
      "HLT_mu4_mb_sp1200_pusup200_trk60_hmt_L1MU4_TE15.0ETA24",
      "HLT_mu4_mb_sp1200_pusup200_trk60_hmt_L1MU4_TE20.0ETA24",
      "HLT_mu4_mb_sp1200_trk60_hmt_L1MU4",
      "HLT_mu4_mb_sp1200_trk60_hmt_L1MU4_TE10.0ETA24",
      "HLT_mu4_mb_sp1200_trk60_hmt_L1MU4_TE15.0ETA24",
      "HLT_mu4_mb_sp1200_trk60_hmt_L1MU4_TE20.0ETA24",
      "HLT_mu4_mb_sp1600_pusup300_trk80_hmt_L1MU4",
      "HLT_mu4_mb_sp1600_pusup300_trk80_hmt_L1MU4_TE15.0ETA24",
      "HLT_mu4_mb_sp1600_pusup300_trk80_hmt_L1MU4_TE20.0ETA24",
      "HLT_mu4_mb_sp1600_pusup300_trk80_hmt_L1MU4_TE25.0ETA24",
      "HLT_mu4_mb_sp1600_trk80_hmt_L1MU4",
      "HLT_mu4_mb_sp1600_trk80_hmt_L1MU4_TE15.0ETA24",
      "HLT_mu4_mb_sp1600_trk80_hmt_L1MU4_TE20.0ETA24",
      "HLT_mu4_mb_sp1600_trk80_hmt_L1MU4_TE25.0ETA24",
      "HLT_mu4_mb_sp2100_pusup400_trk100_hmt_L1MU4",
      "HLT_mu4_mb_sp2100_pusup400_trk100_hmt_L1MU4_TE15.0ETA24",
      "HLT_mu4_mb_sp2100_pusup400_trk100_hmt_L1MU4_TE20.0ETA24",
      "HLT_mu4_mb_sp2100_pusup400_trk100_hmt_L1MU4_TE25.0ETA24",
      "HLT_mu4_mb_sp2100_pusup400_trk100_hmt_L1MU4_TE50",
      "HLT_mu4_mb_sp2100_trk100_hmt_L1MU4",
      "HLT_mu4_mb_sp2100_trk100_hmt_L1MU4_TE15.0ETA24",
      "HLT_mu4_mb_sp2100_trk100_hmt_L1MU4_TE20.0ETA24",
      "HLT_mu4_mb_sp2100_trk100_hmt_L1MU4_TE25.0ETA24",
      "HLT_mu4_mb_sp2100_trk100_hmt_L1MU4_TE50",
      "HLT_mu4_mb_sp2400_pusup500_trk120_hmt_L1MU4",
      "HLT_mu4_mb_sp2400_pusup500_trk120_hmt_L1MU4_TE15.0ETA24",
      "HLT_mu4_mb_sp2400_pusup500_trk120_hmt_L1MU4_TE20.0ETA24",
      "HLT_mu4_mb_sp2400_pusup500_trk120_hmt_L1MU4_TE25.0ETA24",
      "HLT_mu4_mb_sp2400_pusup500_trk120_hmt_L1MU4_TE50",
      "HLT_mu4_mb_sp2400_trk120_hmt_L1MU4",
      "HLT_mu4_mb_sp2400_trk120_hmt_L1MU4_TE15.0ETA24",
      "HLT_mu4_mb_sp2400_trk120_hmt_L1MU4_TE20.0ETA24",
      "HLT_mu4_mb_sp2400_trk120_hmt_L1MU4_TE25.0ETA24",
      "HLT_mu4_mb_sp2400_trk120_hmt_L1MU4_TE50",
      "HLT_mu4_mb_sp2800_pusup600_trk140_hmt_L1MU4_TE50",
      "HLT_mu4_mb_sp2800_pusup600_trk140_hmt_L1MU4_TE70",
      "HLT_mu4_mb_sp2800_pusup600_trk140_hmt_L1MU4_TE90",
      "HLT_mu4_mb_sp2800_trk140_hmt_L1MU4_TE50",
      "HLT_mu4_mb_sp2800_trk140_hmt_L1MU4_TE70",
      "HLT_mu4_mb_sp2800_trk140_hmt_L1MU4_TE90",
      "HLT_mu4_mb_sp3100_pusup700_trk160_hmt_L1MU4_TE20.0ETA24",
      "HLT_mu4_mb_sp3100_pusup700_trk160_hmt_L1MU4_TE25.0ETA24",
      "HLT_mu4_mb_sp3100_pusup700_trk160_hmt_L1MU4_TE30.0ETA24",
      "HLT_mu4_mb_sp3100_pusup700_trk160_hmt_L1MU4_TE50",
      "HLT_mu4_mb_sp3100_pusup700_trk160_hmt_L1MU4_TE70",
      "HLT_mu4_mb_sp3100_trk160_hmt_L1MU4_TE20.0ETA24",
      "HLT_mu4_mb_sp3100_trk160_hmt_L1MU4_TE25.0ETA24",
      "HLT_mu4_mb_sp3100_trk160_hmt_L1MU4_TE30.0ETA24",
      "HLT_mu4_mb_sp3100_trk160_hmt_L1MU4_TE50",
      "HLT_mu4_mb_sp3100_trk160_hmt_L1MU4_TE70",
      "HLT_mu4_mb_sp3500_pusup800_trk180_hmt_L1MU4_TE120",
      "HLT_mu4_mb_sp3500_pusup800_trk180_hmt_L1MU4_TE70",
      "HLT_mu4_mb_sp3500_pusup800_trk180_hmt_L1MU4_TE90",
      "HLT_mu4_mb_sp3500_trk180_hmt_L1MU4_TE120",
      "HLT_mu4_mb_sp3500_trk180_hmt_L1MU4_TE70",
      "HLT_mu4_mb_sp3500_trk180_hmt_L1MU4_TE90",
      "HLT_mu4_mb_sp4100_pusup900_trk200_hmt_L1MU4_TE120",
      "HLT_mu4_mb_sp4100_pusup900_trk200_hmt_L1MU4_TE160",
      "HLT_mu4_mb_sp4100_pusup900_trk200_hmt_L1MU4_TE90",
      "HLT_mu4_mb_sp4100_trk200_hmt_L1MU4_TE120",
      "HLT_mu4_mb_sp4100_trk200_hmt_L1MU4_TE160",
      "HLT_mu4_mb_sp4100_trk200_hmt_L1MU4_TE90",
      "HLT_mu4_mb_sp4500_pusup1000_trk220_hmt_L1MU4_TE30.0ETA24",
      "HLT_mu4_mb_sp4500_pusup1000_trk220_hmt_L1MU4_TE35.0ETA24",
      "HLT_mu4_mb_sp4500_pusup1000_trk220_hmt_L1MU4_TE40.0ETA24",
      "HLT_mu4_mb_sp4500_trk220_hmt_L1MU4_TE30.0ETA24",
      "HLT_mu4_mb_sp4500_trk220_hmt_L1MU4_TE35.0ETA24",
      "HLT_mu4_mb_sp4500_trk220_hmt_L1MU4_TE40.0ETA24",
      "HLT_mu4_mb_sp4800_pusup1100_trk240_hmt_L1MU4_TE120",
      "HLT_mu4_mb_sp4800_pusup1100_trk240_hmt_L1MU4_TE160",
      "HLT_mu4_mb_sp4800_trk240_hmt_L1MU4_TE120",
      "HLT_mu4_mb_sp4800_trk240_hmt_L1MU4_TE160",
      "HLT_mu4"]
    triggers=vn_triggers+hmt_triggers+muon_triggers
elif is_pPb_2016_5TeV:
    triggers=[
      "HLT_noalg_mb_L1MBTS_1",
      "HLT_mb_sp100_trk60_hmt_L1MBTS_1_1",
      "HLT_mb_sp100_trk80_hmt_L1MBTS_1_1",
      "HLT_mb_sp100_trk100_hmt_L1MBTS_1_1",
      "HLT_mb_sp100_trk110_hmt_L1MBTS_1_1"]
else:
    triggers=[]

triggers='(' + ' || '.join(triggers) + ')'
#Other cuts
vertex_cut='count(abs(PrimaryVertices.z)<100)>1'
#All cuts
req_total =triggers+'&&('+vertex_cut+')'
#disable triggers for MC
if isSimulation:
    triggers=""
    req_total='('+vertex_cut+')'
print "******************************************************"
print req_total
print "******************************************************"
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HION2SkimmingTriggerTool = DerivationFramework__xAODStringSkimmingTool( name = "HION2SkimmingTriggerTool",expression = req_total )
ToolSvc+=HION2SkimmingTriggerTool



#====================================================================
#AUGMENTATION TOOLS
#====================================================================
#TrackQuality Augmentation tool
from DerivationFrameworkHI.DerivationFrameworkHIConf import DerivationFramework__HITrackQualityAugmentationTool
HION2HITrackQualityAugmentationTool=DerivationFramework__HITrackQualityAugmentationTool("HION2HITrackQualityAugmentationTool")
ToolSvc+=HION2HITrackQualityAugmentationTool
AugmentationTools_=[HION2HITrackQualityAugmentationTool]
TrkSelTool_pp      =CfgMgr.InDet__InDetTrackSelectionTool("TrackSelectionTool_pp"      ,CutLevel = "MinBias",minPt = 400.)
TrkSelTool_hi_loose=CfgMgr.InDet__InDetTrackSelectionTool("TrackSelectionTool_hi_loose",CutLevel = "HILoose",minPt = 400.)
TrkSelTool_hi_tight=CfgMgr.InDet__InDetTrackSelectionTool("TrackSelectionTool_hi_tight",CutLevel = "HITight",minPt = 400.)
ToolSvc+=TrkSelTool_pp
ToolSvc+=TrkSelTool_hi_loose
ToolSvc+=TrkSelTool_hi_tight
HION2HITrackQualityAugmentationTool.TrackSelectionTool_pp      =TrkSelTool_pp
HION2HITrackQualityAugmentationTool.TrackSelectionTool_hi_loose=TrkSelTool_hi_loose
HION2HITrackQualityAugmentationTool.TrackSelectionTool_hi_tight=TrkSelTool_hi_tight


#HijingPars Augmentation tool
if isSimulation:
    from DerivationFrameworkHI.DerivationFrameworkHIConf import DerivationFramework__HIHijingParsAugmentationTool
    HION2HIHijingParsAugmentationTool=DerivationFramework__HIHijingParsAugmentationTool("HION2HIHijingParsAugmentationTool")
    ToolSvc+=HION2HIHijingParsAugmentationTool
    AugmentationTools_+=[HION2HIHijingParsAugmentationTool]





#====================================================================
# THINNING
#====================================================================
thinningTools=[]

## Truth thinning
#truth_cond_finalState = 'TruthParticles.status == 1 && TruthParticles.barcode < 200000'   # stable particles
#truth_expression = '('+truth_cond_finalState+')'
#
#from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
#HION2TruthThinningTool = DerivationFramework__GenericTruthThinning(name                    = "HION2TruthThinningTool",
#                                                                   ThinningService         = "HION2ThinningSvc",
#                                                                   ParticleSelectionString = truth_expression,
#                                                                   PreserveDescendants     = False,
#                                                                   PreserveGeneratorDescendants     = True,
#                                                                   PreserveAncestors       = False)
#if isSimulation:
#    ToolSvc += HION2TruthThinningTool
#    thinningTools.append(HION2TruthThinningTool)
#
##Special lines for thinning
## Thinning service name must match the one passed to the thinning tools
#from AthenaServices.Configurables import ThinningSvc, createThinningSvc
#augStream = MSMgr.GetStream( streamName )
#evtStream = augStream.GetEventStream()
#svcMgr += createThinningSvc( svcName="HION2ThinningSvc", outStreams=[evtStream] )





#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HION2Kernel",
                          SkimmingTools     = [HION2SkimmingTriggerTool],
                          AugmentationTools = AugmentationTools_,
                          ThinningTools     = thinningTools
                          )
HION2Stream.AcceptAlgs(["HION2Kernel"])





#containers in DXAOD
#from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
#HION2SlimmingHelper = SlimmingHelper("HION2SlimmingHelper")
#HION2SlimmingHelper.SmartCollections = ["InDetTrackParticles","PrimaryVertices" ]
#HION2SlimmingHelper.AllVariables = ["HIEventShape"]
#HION2SlimmingHelper.AppendContentToStream(HION2Stream)
#HION2Stream.AddItem("xAOD::TrackParticleContainer#InDetTrackParticles")
#HION2Stream.AddItem("xAOD::TrackParticleContainer#InDetTrackParticles.#")


#when not using smart-collection for tracks below, the output becomes larger
#from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
#HION2SlimmingHelper = SlimmingHelper("HION2SlimmingHelper")
#HION2SlimmingHelper.AllVariables = ["HIEventShape","InDetTrackParticles","PrimaryVertices"]
#HION2SlimmingHelper.AppendContentToStream(HION2Stream)


from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HION2SlimmingHelper = SlimmingHelper("HION2SlimmingHelper")
HION2SlimmingHelper.AllVariables = ["CaloSums","PrimaryVertices"]
HION2SlimmingHelper.ExtraVariables=["InDetTrackParticles.qOverP.theta.phi.TrackQuality"]
if isSimulation:
     HION2SlimmingHelper.AllVariables  += ["TruthEvents","TruthParticles","TruthVertices"]
     HION2SlimmingHelper.AllVariables  += ["HIEventShape"]
     HION2SlimmingHelper.ExtraVariables+= ["HIClusters.calE.eta0.phi0"]
     HION2SlimmingHelper.ExtraVariables+= ["InDetTrackParticles.truthMatchProbability.truthParticleLink.truthOrigin.truthType.trackLink"]
if is_PbPb_2015_5TeV:
     HION2SlimmingHelper.AllVariables  += ["HIEventShape"]
     HION2SlimmingHelper.ExtraVariables+= ["HIClusters.calE.eta0.phi0"]
if is_pPb_2016_8TeV or is_pPb_2016_5TeV:
     HION2SlimmingHelper.AllVariables += ["ZdcModules","ZdcSums","ZdcTriggerTowers","MBTSForwardEventInfo","MBTSModules"]
HION2SlimmingHelper.AppendContentToStream(HION2Stream)

