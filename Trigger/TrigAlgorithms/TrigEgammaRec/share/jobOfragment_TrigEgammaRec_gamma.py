import EventKernel.ParticleDataType

theApp.DLLs += ["TrigEgammaRec"]
theApp.Dlls += ["egammaRec"]

# switch for Bremfit
if not 'doTrigEgammaBremFit' in dir():
    doTrigEgammaBremFit = False

TrigEgammaRec_gamma_EF = Algorithm( "TrigEgammaRec_gamma_EF" )

#TrigEgammaRec_gamma_EF.EgammaContainerAliasSuffix="egamma" 
TrigEgammaRec_gamma_EF.EgammaContainerAliasSuffix="NoIDEF"
TrigEgammaRec_gamma_EF.EMShowerContainerName = "TrigEMShowerContainer"
TrigEgammaRec_gamma_EF.EMTrackMatchContainerName = "TrigEMTrackMatchContainer"

#From ./ atlas/ Reconstruction/ egammaRec/ share/ egammaRec_jobOptions.py
# jobOption file for egamma 
# dependent on LAr, iPatRec, xKalman, tile 
# 
#######################################
#TrigEgammaRec_gamma_EF.ClusterContainerName="LArClusterEM" 
#TrigEgammaRec_gamma_EF.EgammasOutputName="egammaCollection" 
# Tools to be run
TrigEgammaRec_gamma_EF.ShowerBuilderName= "EMShowerBuilder/EFemshower"
TrigEgammaRec_gamma_EF.TrackMatchBuilderName = "EMTrackMatchBuilder/EFemtrackmatch"
# conversion code has not been adapted to trigger yet
#                      "EMConversionBuilder/emconversion"
TrigEgammaRec_gamma_EF.PID_BuilderName = "EMPIDBuilder/empid"
TrigEgammaRec_gamma_EF.BuilderNames= ["EMFourMomBuilder/egfourmom"]
if doTrigEgammaBremFit:
    TrigEgammaRec_gamma_EF.BuilderNames+= ["EMBremsstrahlungBuilder/EFembremfit"]

ToolSvc = Service( "ToolSvc" )

# SUBCALO enum LAREM LARHEC LARFCAL TILE NSUBCALO NOT_VALID
from CaloIdentifier import SUBCALO

# Calo Cell container
ToolSvc.EFemshower.CellsName="AllCalo"
# List of calo
#ToolSvc.EFemshower.CaloNums=[0,1,3]
ToolSvc.EFemshower.CaloNums=[SUBCALO.LAREM, SUBCALO.LARHEC, SUBCALO.TILE]

theApp.Dlls += [ "egammaUtils" ]
# Instance of egammaShowerShape
ToolSvc.EFemshower.ShowerShapeTool="egammaShowerShape/egammashowershape"
# in case we want some extra print
ToolSvc.EFemshower.Print=FALSE

ToolSvc.EFemshower.egammashowershape.TypeAnalysis ="ClusterSeed"
# calculate quantities base on information in the strips in a region
# around the cluster. 
# Use 5 strips in phi and cover a region of +-1.1875
# times 0.025 in eta (corresponds to 19 strips in em barrel)
# Use 7*7 cluster for middle sampling
# other samplings arew not used for the moment
ToolSvc.EFemshower.egammashowershape.Neta=[0.,2.5,7,0.]
ToolSvc.EFemshower.egammashowershape.Nphi=[0.,2.,7,0.]
# Samplings to execute
ToolSvc.EFemshower.egammashowershape.ExecPreSampler = TRUE
ToolSvc.EFemshower.egammashowershape.ExecEMFirst = TRUE
ToolSvc.EFemshower.egammashowershape.ExecEMSecond = TRUE
ToolSvc.EFemshower.egammashowershape.ExecEMThird = TRUE
ToolSvc.EFemshower.egammashowershape.ExecEMCombined = TRUE

#
# pointing
#
ToolSvc.egammaqpoint.CalorimeterDepth="CaloDepthTool"
ToolSvc.egammaqpoint.CaloDepthTool.DepthChoice = "egparam"
    
ToolSvc.EFemtrackmatch.TrackParticlesName = "TrackParticleCandidate"
ToolSvc.EFemtrackmatch.broadDeltaEta      = 0.05
ToolSvc.EFemtrackmatch.broadDeltaPhi      = 0.10
ToolSvc.EFemtrackmatch.narrowDeltaEta     = 0.025
ToolSvc.EFemtrackmatch.narrowDeltaPhi     = 0.025
ToolSvc.EFemtrackmatch.narrowDeltaPhiBrem = 0.050
ToolSvc.EFemtrackmatch.useCandidateMatch  = True
                                                                                                                                                                                                
#conversions:
#ToolSvc.emconversion.ConversionsName="Conversions"

#bremfit
ToolSvc.EFembremfit.CalorimeterDepth="CaloDepthTool/embremfitcalodepth"
ToolSvc.EFembremfit.embremfitcalodepth.DepthChoice = "egparam"
ToolSvc.EFembremfit.BremFitContainerName = "TrigEMBremFitContainer"
#ToolSvc.EFembremfit.TrackMatchContainerName="TrigEMTrackMatchContainer"
#ToolSvc.EFembremfit.EMTrackMatchContainerName="egDetailContainer"
ToolSvc.EFembremfit.EMTrackMatchContainerName="TrigEMTrackMatchContainer"

# JobOptions to extrapolate a track to Calorimeters
# configure private ExtrapolTrackToCaloTool tool
TTC_ExtrapolatorInstance = 'egammaTTCExtrapolator'
# spcify TrackToCalo instance name
ToolSvc.emtrackmatch.ExtrapolTrackToCaloToolName="extrapolEgammaTrackToCaloTool"
include( "TrackToCalo/ExtrapolToCaloTool_joboptions.py" )
ToolSvc.EFemtrackmatch.extrapolEgammaTrackToCaloTool.ExtrapolatorName = TTC_Extrapolator.name()
ToolSvc.EFemtrackmatch.extrapolEgammaTrackToCaloTool.ExtrapolatorInstanceName = TTC_Extrapolator.instance()

#specify here the extrapolation depth
ToolSvc.EFemtrackmatch.extrapolEgammaTrackToCaloTool.CaloDepthTool.DepthChoice = "egparam"

# do the ParticleID
include ("egammaRec/egammaRec_ParticleID.py")

#4-fourmomentum
#ToolSvc.egfourmom.EMShowerContainerName="egDetailContainer"
ToolSvc.egfourmom.EMShowerContainerName="TrigEMShowerContainer"

