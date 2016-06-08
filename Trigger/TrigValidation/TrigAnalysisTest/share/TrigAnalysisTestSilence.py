from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
if hasattr(topSequence, "T2CaloEgamma_Ringer"):
    alg = getattr(topSequence, "T2CaloEgamma_Ringer")
    alg.OutputLevel = WARNING
if hasattr(topSequence, "TrigL2CaloHypo"):
    alg = getattr(topSequence, "TrigL2CaloHypo")
    alg.OutputLevel = WARNING
if hasattr(topSequence, "T2CaloEgamma_eGamma"):
    alg = getattr(topSequence, "T2CaloEgamma_eGamma")
    alg.OutputLevel = WARNING
if hasattr(topSequence, "TrigCaloCellMaker_eGamma"):
    alg = getattr(topSequence, "TrigCaloCellMaker_eGamma")
    alg.OutputLevel = WARNING
if hasattr(topSequence, "TrigCaloTowerMaker_eGamma"):
    alg = getattr(topSequence, "TrigCaloTowerMaker_eGamma")
    alg.OutputLevel = WARNING
if hasattr(topSequence, "TrigCaloClusterMaker_slw"):
    alg = getattr(topSequence, "TrigCaloClusterMaker_slw")
    alg.OutputLevel = WARNING
if hasattr(topSequence, "TrigL2SiTrackFinder_eGammaC"):
    alg = getattr(topSequence, "TrigL2SiTrackFinder_eGammaC")
    alg.OutputLevel = WARNING
if hasattr(topSequence, "InDetTrigTrackingxAODCnv_Electron_L2ID"):
    alg = getattr(topSequence, "InDetTrigTrackingxAODCnv_Electron_L2ID")
    alg.OutputLevel = WARNING
if hasattr(topSequence, "TrigL2SiTrackFinder_eGammaB"):
    alg = getattr(topSequence, "TrigL2SiTrackFinder_eGammaB")
    alg.OutputLevel = WARNING
if hasattr(topSequence, "TrigL2SiTrackFinder_eGammaA"):
    alg = getattr(topSequence, "TrigL2SiTrackFinder_eGammaA")
    alg.OutputLevel = WARNING
if hasattr(topSequence, "PixelClustering_Electron_IDTrig"):
    alg = getattr(topSequence, "PixelClustering_Electron_IDTrig")
    alg.OutputLevel = WARNING
if hasattr(topSequence, "SCTClustering_Electron_IDTrig"):
    alg = getattr(topSequence, "SCTClustering_Electron_IDTrig")
    alg.OutputLevel = WARNING
if hasattr(topSequence, "TrigFastTrackFinder_eGamma"):
    alg = getattr(topSequence, "TrigFastTrackFinder_eGamma")
    alg.OutputLevel = WARNING
if hasattr(topSequence, "L2PhotonFex_1"):
    alg = getattr(topSequence, "L2PhotonFex_1")
    alg.OutputLevel = WARNING
if hasattr(topSequence, "L2PhotonHypo"):
    alg = getattr(topSequence, "L2PhotonHypo")
    alg.OutputLevel = WARNING
if hasattr(topSequence, "TrigEFCaloCalibFex_Electron"):
    alg = getattr(topSequence, "TrigEFCaloCalibFex_Electron")
    alg.OutputLevel = WARNING
if hasattr(topSequence, "TrigEFCaloHypo"):
    alg = getattr(topSequence, "TrigEFCaloHypo")
    alg.OutputLevel = WARNING
if hasattr(topSequence, "L2ElectronFex_L2StarC"):
    alg = getattr(topSequence, "L2ElectronFex_L2StarC")
    alg.OutputLevel = WARNING
if hasattr(topSequence, "L2ElectronFex_L2StarB"):
    alg = getattr(topSequence, "L2ElectronFex_L2StarB")
    alg.OutputLevel = WARNING
if hasattr(topSequence, "L2ElectronFex_L2StarA"):
    alg = getattr(topSequence, "L2ElectronFex_L2StarA")
    alg.OutputLevel = WARNING
if hasattr(topSequence, "L2ElectronFex_1"):
    alg = getattr(topSequence, "L2ElectronFex_1")
    alg.OutputLevel = WARNING
if hasattr(topSequence, "L2ElectronHypo"):
    alg = getattr(topSequence, "L2ElectronHypo")
    alg.OutputLevel = WARNING
if hasattr(topSequence, "TrigExtProcessor_Electron_EFID"):
    alg = getattr(topSequence, "TrigExtProcessor_Electron_EFID")
    alg.OutputLevel = WARNING
if hasattr(topSequence, "InDetTrigTrackSlimmer_Electron_EFID"):
    alg = getattr(topSequence, "InDetTrigTrackSlimmer_Electron_EFID")
    alg.OutputLevel = WARNING
if hasattr(topSequence, "TrigVxPrimary_Electron_EFID"):
    alg = getattr(topSequence, "TrigVxPrimary_Electron_EFID")
    alg.OutputLevel = WARNING
if hasattr(topSequence, "InDetTrigParticleCreation_Electron_EFID"):
    alg = getattr(topSequence, "InDetTrigParticleCreation_Electron_EFID")
    alg.OutputLevel = WARNING
if hasattr(topSequence, "SiTrigSpacePointFinder_Electron_EFID"):
    alg = getattr(topSequence, "SiTrigSpacePointFinder_Electron_EFID")
    alg.OutputLevel = WARNING
if hasattr(topSequence, "TrigEgammaRec_eGamma"):
    alg = getattr(topSequence, "TrigEgammaRec_eGamma")
    alg.OutputLevel = WARNING
if hasattr(topSequence, "TrigEgammaRec_NoIDEF_eGamma"):
    alg = getattr(topSequence, "TrigEgammaRec_NoIDEF_eGamma")
    alg.OutputLevel = WARNING
if hasattr(topSequence, "TRTTrackExtAlg_Electron_EFID"):
    alg = getattr(topSequence, "TRTTrackExtAlg_Electron_EFID")
    alg.OutputLevel = WARNING
if hasattr(topSequence, "TrigAmbiguitySolver_Electron_EFID"):
    alg = getattr(topSequence, "TrigAmbiguitySolver_Electron_EFID")
    alg.OutputLevel = WARNING
if hasattr(topSequence, "EFTrackHypo"):
    alg = getattr(topSequence, "EFTrackHypo")
    alg.OutputLevel = WARNING
if hasattr(topSequence, "TRTDriftCircleMaker_Electron_EFID"):
    alg = getattr(topSequence, "TRTDriftCircleMaker_Electron_EFID")
    alg.OutputLevel = WARNING
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.enableMonitoring = [ ]