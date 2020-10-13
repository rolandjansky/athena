################################################################################
##
#@file Pi0ClusterMaker_jobOptions.py
#
#@brief jobOption to create clusters for the Pi0 Finder.
#
# Use cell container created by TauRecCoreBuilder as an input.
# Most settings copied from /Calorimeter/CaloRec/python/CaloClusterTopoGetter.py
################################################################################
from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
from RecExConfig.ObjKeyStore import objKeyStore
from AthenaCommon.JobProperties import jobproperties as jp
import traceback

#from CaloUtils.CaloUtilsConf import H1ClusterCellWeightTool, EMFracClusterClassificationTool, OutOfClusterCorrectionTool, DeadMaterialCorrectionTool2
from CaloUtils.CaloUtilsConf import CaloLCClassificationTool, CaloLCWeightTool, CaloLCOutOfClusterTool, CaloLCDeadMaterialTool

from CaloClusterCorrection.CaloClusterCorrectionConf import CaloClusterLocalCalib
#>> new PL May 4, 2009
from CaloClusterCorrection.CaloClusterCorrectionConf import CaloClusterCellWeightCalib
#<<

from CaloRec.CaloRecConf import CaloTopoClusterMaker, CaloTopoClusterSplitter, CaloClusterMomentsMaker, CaloClusterMaker #, CaloClusterLockVars, CaloClusterPrinter
from CaloRec import CaloRecFlags
from CaloRec.CaloTopoClusterFlags import jobproperties
from AthenaCommon.SystemOfUnits import deg, GeV, MeV
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.GlobalFlags import globalflags
from RecExConfig.RecFlags import rec

from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
CaloNoiseCondAlg()
#For LCWeightsTool needs electronic noise
CaloNoiseCondAlg(noisetype="electronicNoise") 

# configure cell weight calibration
if jobproperties.CaloTopoClusterFlags.doCellWeightCalib():
    from CaloClusterCorrection.CaloClusterCorrectionConf import H1WeightToolCSC12Generic
    from CaloClusterCorrection.StandardCellWeightCalib   import H1Calibration, getCellWeightTool
    CellWeights = CaloClusterCellWeightCalib("CellWeights")
    # -- configure weight tool
    finder = jobproperties.CaloTopoClusterFlags.cellWeightRefFinder.get_Value()
    size   = jobproperties.CaloTopoClusterFlags.cellWeightRefSize.get_Value()
    signal = jobproperties.CaloTopoClusterFlags.cellWeightRefSignal.get_Value()
    WeightTool  = getCellWeightTool(finder,size,signal)
    # -- connect weight tool
    CellWeights.CellSignalWeightTool    = WeightTool
    CellWeights                        += WeightTool
    #-- default properties
    CellWeights.Direction               = "AbsSignal"   #-- use absolute cell energies for eta/phi calculation 
    CellWeights.BelowThresholdLikeAll   = True          #-- treat clusters below thresholds the same as all others
    CellWeights.BelowThresholdDirection = "AbsSignal"   #-- alternative direction calculation for below threshold clusters,
                                                        #   ignored if BelowThresholdLikeAll = True
    CellWeights.EnergyThreshold         = 0.0*MeV       #-- threshold for possible change of direction calculation
    CellWeights.IgnoreGeoWeights        = False         #-- ignore geometrical cell signal weights if True
    
# now configure local hadronic calibration
if jobproperties.CaloTopoClusterFlags.doTopoClusterLocalCalib():
    # tools used by tools
    # EMFrac   = EMFracClusterClassificationTool("EMFrac")
    # EMFrac.ClassificationKey   = "EMFracClassify"
    # EMFrac.UseEMFractionSpread = False
    # EMFrac.MaxEMFraction       = 0.5
    # 
    # H1Weight = H1ClusterCellWeightTool("H1Weight")
    # H1Weight.CorrectionKey       = "H1ClusterCellWeights"
    # H1Weight.SignalOverNoiseCut  = 2.0
    # 
    # OOCC     = OutOfClusterCorrectionTool("OOCC")
    # OOCC.CorrectionKey       = "OOCCorrection"
    # 
    # OOCCPi0  = OutOfClusterCorrectionTool("OOCCPi0")
    # OOCCPi0.CorrectionKey    = "OOCPi0Correction"
    
    # tools used by tools
    LCClassify   = CaloLCClassificationTool("LCClassify")
    LCClassify.ClassificationKey   = "EMFracClassify"
    LCClassify.UseSpread = False
    LCClassify.MaxProbability = 0.5
    # add the moments EM_PROBABILITY, HAD_WEIGHT, OOC_WEIGHT, DM_WEIGHT to the AOD:
    LCClassify.StoreClassificationProbabilityInAOD = True
    LCClassify.WeightingOfNegClusters = jobproperties.CaloTopoClusterFlags.doTreatEnergyCutAsAbsolute()

    LCWeight = CaloLCWeightTool("LCWeight")
    LCWeight.CorrectionKey       = "H1ClusterCellWeights"
    LCWeight.SignalOverNoiseCut  = 2.0
    LCWeight.UseHadProbability   = True

    LCOut     = CaloLCOutOfClusterTool("LCOut")
    LCOut.CorrectionKey       = "OOCCorrection"
    LCOut.UseEmProbability    = False
    LCOut.UseHadProbability   = True
    
    LCOutPi0  = CaloLCOutOfClusterTool("LCOutPi0")
    LCOutPi0.CorrectionKey    = "OOCPi0Correction"
    LCOutPi0.UseEmProbability  = True
    LCOutPi0.UseHadProbability = False
    
    #DMTool   = DeadMaterialCorrectionTool2("DMTool")
    #DMTool.HadDMCoeffKey       = "HadDMCoeff2"
    #DMTool.ClusterRecoStatus   = 0
    #DMTool.WeightModeDM        = 2 
    #DMTool.CaloNoiseTool       = theCaloNoiseTool
    
    LCDeadMaterial   = CaloLCDeadMaterialTool("LCDeadMaterial")
    LCDeadMaterial.HadDMCoeffKey       = "HadDMCoeff2"
    LCDeadMaterial.ClusterRecoStatus   = 0
    LCDeadMaterial.WeightModeDM        = 2 
    LCDeadMaterial.UseHadProbability   = True
    LCDeadMaterial.WeightingOfNegClusters = jobproperties.CaloTopoClusterFlags.doTreatEnergyCutAsAbsolute()
    # correction tools using tools
    LocalCalib = CaloClusterLocalCalib ("LocalCalibForTaus")
    LocalCalib.ClusterClassificationTool     = [LCClassify]
    #LocalCalib.ClusterRecoStatus             = [2]
    LocalCalib.ClusterRecoStatus             = [1,2]
    LocalCalib.LocalCalibTools               = [LCWeight]
    LocalCalib.WeightingOfNegClusters = jobproperties.CaloTopoClusterFlags.doTreatEnergyCutAsAbsolute()
    
    LocalCalib += LCClassify
    LocalCalib += LCWeight

    OOCCalib   = CaloClusterLocalCalib ("OOCCalibForTaus")
    #OOCCalib.ClusterRecoStatus   = [2]
    OOCCalib.ClusterRecoStatus   = [1,2]
    OOCCalib.LocalCalibTools     = [LCOut]
    OOCCalib.WeightingOfNegClusters = jobproperties.CaloTopoClusterFlags.doTreatEnergyCutAsAbsolute()

    OOCCalib += LCOut

    OOCPi0Calib   = CaloClusterLocalCalib ("OOCPi0CalibForTaus")
    #OOCPi0Calib.ClusterRecoStatus   = [1]
    OOCPi0Calib.ClusterRecoStatus   = [1,2]
    OOCPi0Calib.LocalCalibTools     = [LCOutPi0]

    OOCPi0Calib.WeightingOfNegClusters = jobproperties.CaloTopoClusterFlags.doTreatEnergyCutAsAbsolute()

    OOCPi0Calib += LCOutPi0

    DMCalib    = CaloClusterLocalCalib ("DMCalibForTaus")
    DMCalib.ClusterRecoStatus   = [1,2]
    #DMCalib.LocalCalibToolNames = [DMTool.getFullName()]
    #DMCalib += DMTool
    DMCalib.LocalCalibTools      = [LCDeadMaterial]

    DMCalib.WeightingOfNegClusters = jobproperties.CaloTopoClusterFlags.doTreatEnergyCutAsAbsolute()

    DMCalib += LCDeadMaterial

# correction tools not using tools
TopoMomentsForTaus = CaloClusterMomentsMaker ("TauPi0TopoMoments")
TopoMomentsForTaus.WeightingOfNegClusters = jobproperties.CaloTopoClusterFlags.doTreatEnergyCutAsAbsolute()
TopoMomentsForTaus.MaxAxisAngle = 20*deg
TopoMomentsForTaus.TwoGaussianNoise = jobproperties.CaloTopoClusterFlags.doTwoGaussianNoise()
TopoMomentsForTaus.MinBadLArQuality = 4000
TopoMomentsForTaus.MomentsNames = ["FIRST_PHI"
                            ,"FIRST_ETA"
                            ,"SECOND_R" 
                            ,"SECOND_LAMBDA"
                            ,"DELTA_PHI"
                            ,"DELTA_THETA"
                            ,"DELTA_ALPHA" 
                            ,"CENTER_X"
                            ,"CENTER_Y"
                            ,"CENTER_Z"
                            ,"CENTER_MAG"
                            ,"CENTER_LAMBDA"
                            ,"LATERAL"
                            ,"LONGITUDINAL"
                            ,"ENG_FRAC_EM" 
                            ,"ENG_FRAC_MAX" 
                            ,"ENG_FRAC_CORE" 
                            ,"FIRST_ENG_DENS" 
                            ,"SECOND_ENG_DENS" 
                            ,"ISOLATION"
                            ]
#TopoMomentsForTaus.AODMomentsNames = ["LATERAL"
#                                ,"LONGITUDINAL"
#                                ,"SECOND_R" 
#                                ,"SECOND_LAMBDA"
#                                ,"CENTER_MAG"
#                                ,"CENTER_LAMBDA"
#                                ,"FIRST_ENG_DENS"
#                                ,"ENG_FRAC_MAX" 
#                                ,"ISOLATION"
#                                ,"ENG_BAD_CELLS"
#                                ,"N_BAD_CELLS"
#                                ,"BADLARQ_FRAC"
#                                ,"ENG_POS"
#                                ,"SIGNIFICANCE"
#                                ,"CELL_SIGNIFICANCE"
#                                ,"CELL_SIG_SAMPLING"
#                                ,"AVG_LAR_Q"
#                                ,"AVG_TILE_Q"
#                                ] 

#if jobproperties.CaloTopoClusterFlags.lockTopoClusterSamplingEnergies() or jobproperties.CaloTopoClusterFlags.lockTopoClusterSamplingVariables():
#    LockVariables = CaloClusterLockVars("LockVariables")
#    LockVariables.FixBasicEnergy = True
#    LockVariables.LockedSamplingVariables = []
#    if jobproperties.CaloTopoClusterFlags.lockTopoClusterSamplingEnergies():
#        LockVariables.LockedSamplingVariables += [
#            "Energy", "Max_Energy"]
#    if jobproperties.CaloTopoClusterFlags.lockTopoClusterSamplingVariables():    
#        LockVariables.LockedSamplingVariables += [
#            "Eta", "Phi", "Delta_Eta",
#            "Delta_Phi", "Max_Eta", "Max_Phi"
#            ]
                
#if jobproperties.CaloTopoClusterFlags.printTopoClusters():
#    PrintCaloCluster = CaloClusterPrinter("PrintCaloCluster")
#    PrintCaloCluster.PrintFirstOnly = True
#    PrintCaloCluster.PrintFrequency = 1
#    PrintCaloCluster.EnergyUnit     = 1.0*GeV

    
# maker tools
TopoClusterForTaus = CaloTopoClusterMaker("TauPi0TopoClusterMaker")

TopoClusterForTaus.CellsName = "TauCommonPi0Cells"
TopoClusterForTaus.CalorimeterNames=["LAREM"]
TopoClusterForTaus.SeedSamplingNames = [
    "PreSamplerB", "EMB1", "EMB2", # Do we want to use EMB3?
    "PreSamplerE", "EME1", "EME2"  # Do we want to use EME3?
    ]
TopoClusterForTaus.NeighborOption                    = "super3D"
TopoClusterForTaus.RestrictHECIWandFCalNeighbors     = False
TopoClusterForTaus.RestrictPSNeighbors               = True
TopoClusterForTaus.CellThresholdOnEorAbsEinSigma     = 0.0
TopoClusterForTaus.NeighborThresholdOnEorAbsEinSigma = 2.0
TopoClusterForTaus.SeedThresholdOnEorAbsEinSigma     = 4.0
TopoClusterForTaus.SeedCutsInAbsE                    = True
TopoClusterForTaus.ClusterEtorAbsEtCut               = 0.5*GeV # 0.0*MeV in standard CaloCalTopoCluster JobOptions!
TopoClusterForTaus.TwoGaussianNoise                  = jobproperties.CaloTopoClusterFlags.doTwoGaussianNoise()


TopoSplitterForTaus = CaloTopoClusterSplitter("TauPi0TopoSplitter")
# cells from the following samplings will be able to form local
# maxima. The excluded samplings are PreSamplerB, EMB1,
# PreSamplerE, EME1, all Tile samplings, all HEC samplings and the
# two rear FCal samplings.
TopoSplitterForTaus.SamplingNames = ["EMB2","EME2"] # Do we want to use EMB3 and EME3?
# cells from the following samplings will also be able to form
# local maxima but only if they are not overlapping in eta and phi
# with local maxima in previous samplings from the primary list.
TopoSplitterForTaus.SecondarySamplingNames = ["EMB1","EME1"]
TopoSplitterForTaus.ShareBorderCells = True
TopoSplitterForTaus.RestrictHECIWandFCalNeighbors  = False
TopoSplitterForTaus.WeightingOfNegClusters = jobproperties.CaloTopoClusterFlags.doTreatEnergyCutAsAbsolute()

# cluster maker
cluster_container = 'TauInitialPi0Clusters'
CaloTopoForTausMaker = CaloClusterMaker ("TauInitialPi0ClusterMaker")
CaloTopoForTausMaker.ClustersOutputName=cluster_container
CaloTopoForTausMaker.ClusterMakerTools=[
    TopoClusterForTaus.getFullName(),
    TopoSplitterForTaus.getFullName()]

from CaloClusterCorrection.CaloClusterBadChannelListCorr import CaloClusterBadChannelListCorr
BadChannelListCorrForTaus = CaloClusterBadChannelListCorr()
CaloTopoForTausMaker.ClusterCorrectionTools += [BadChannelListCorrForTaus.getFullName()]

CaloTopoForTausMaker.ClusterCorrectionTools += [TopoMomentsForTaus.getFullName()]

CaloTopoForTausMaker += TopoClusterForTaus
CaloTopoForTausMaker += TopoSplitterForTaus
CaloTopoForTausMaker += BadChannelListCorrForTaus
CaloTopoForTausMaker += TopoMomentsForTaus

# Don't add CaloClusterVertexFractionMaker for now
# Don't add CaloCalibClusterMomentsMaker2 for now

#if jobproperties.CaloTopoClusterFlags.lockTopoClusterSamplingEnergies() or jobproperties.CaloTopoClusterFlags.lockTopoClusterSamplingVariables():
#    CaloTopoForTausMaker.ClusterCorrectionTools += [
#        LockVariables.getFullName()]
#    CaloTopoForTausMaker += LockVariables

if jobproperties.CaloTopoClusterFlags.doCellWeightCalib():
    CaloTopoForTausMaker.ClusterCorrectionTools += [
        CellWeights.getFullName() ]
    CaloTopoForTausMaker += CellWeights
    
if jobproperties.CaloTopoClusterFlags.doTopoClusterLocalCalib():
    CaloTopoForTausMaker.ClusterCorrectionTools += [
        LocalCalib.getFullName(),
        OOCCalib.getFullName(),
        OOCPi0Calib.getFullName(),
        DMCalib.getFullName()]

    CaloTopoForTausMaker += LocalCalib
    CaloTopoForTausMaker += OOCCalib
    CaloTopoForTausMaker += OOCPi0Calib
    CaloTopoForTausMaker += DMCalib
                                    
    
#
# pool/cool part
#
if jobproperties.CaloTopoClusterFlags.doTopoClusterLocalCalib():
    from CaloRec import CaloClusterTopoCoolFolder
    if globalflags.DetDescrVersion().startswith("Rome"):
        CaloTopoForTausMaker.LocalCalibForTaus.LCClassify.MaxProbability = 0.85
        CaloTopoForTausMaker.LocalCalibForTaus.LCClassify.UseNormalizedEnergyDensity = False 
    else:   
        CaloTopoForTausMaker.LocalCalibForTaus.LCClassify.MaxProbability = 0.50
        CaloTopoForTausMaker.LocalCalibForTaus.LCClassify.UseNormalizedEnergyDensity = True

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += CaloTopoForTausMaker
