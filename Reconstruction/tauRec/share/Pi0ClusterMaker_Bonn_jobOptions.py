################################################################################
##
#@file Pi0ClusterMaker_Bonn_jobOptions.py
#
#@brief jobOption to create clusters for the "Bonn" Pi0 Finder.
#
# Use cell container created by TauRecCoreBuilder as an input.
# Most settings copied from /Calorimeter/CaloRec/python/CaloClusterTopoGetter.py
################################################################################

from CaloUtils.CaloUtilsConf import CaloLCClassificationTool, CaloLCWeightTool, CaloLCOutOfClusterTool, CaloLCDeadMaterialTool

from CaloClusterCorrection.CaloClusterCorrectionConf import CaloClusterLocalCalib
#>> new PL May 4, 2009
from CaloClusterCorrection.CaloClusterCorrectionConf import CaloClusterCellWeightCalib
#<<

from CaloRec.CaloRecConf import CaloTopoClusterMaker, CaloTopoClusterSplitter, CaloClusterMomentsMaker, CaloClusterMaker
from CaloRec.CaloTopoClusterFlags import jobproperties
from AthenaCommon.SystemOfUnits import deg, GeV, MeV
from AthenaCommon.GlobalFlags import globalflags


from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
theCaloNoiseTool = CaloNoiseToolDefault()

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
    # H1Weight.CaloNoiseTool       = theCaloNoiseTool
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
    LCClassify.StoreClassificationProbabilityInAOD = True

    LCWeight = CaloLCWeightTool("LCWeight")
    LCWeight.CorrectionKey       = "H1ClusterCellWeights"
    LCWeight.SignalOverNoiseCut  = 2.0
    LCWeight.CaloNoiseTool       = theCaloNoiseTool
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
    #DMTool.SignalOverNoiseCut  = 1.0
    #DMTool.ClusterRecoStatus   = 0
    #DMTool.WeightModeDM        = 2 
    #DMTool.CaloNoiseTool       = theCaloNoiseTool
    
    LCDeadMaterial   = CaloLCDeadMaterialTool("LCDeadMaterial")
    LCDeadMaterial.HadDMCoeffKey       = "HadDMCoeff2"
    LCDeadMaterial.ClusterRecoStatus   = 0
    LCDeadMaterial.WeightModeDM        = 2 
    LCDeadMaterial.UseHadProbability   = True

    # correction tools using tools
    LocalCalib = CaloClusterLocalCalib ("LocalCalibForTaus")
    LocalCalib.ClusterClassificationTool     = [LCClassify]
    #LocalCalib.ClusterRecoStatus             = [2]
    LocalCalib.ClusterRecoStatus             = [1,2]
    LocalCalib.LocalCalibTools               = [LCWeight]
    
    LocalCalib += LCClassify
    LocalCalib += LCWeight

    OOCCalib   = CaloClusterLocalCalib ("OOCCalibForTaus")
    #OOCCalib.ClusterRecoStatus   = [2]
    OOCCalib.ClusterRecoStatus   = [1,2]
    OOCCalib.LocalCalibTools     = [LCOut]

    OOCCalib += LCOut

    OOCPi0Calib   = CaloClusterLocalCalib ("OOCPi0CalibForTaus")
    #OOCPi0Calib.ClusterRecoStatus   = [1]
    OOCPi0Calib.ClusterRecoStatus   = [1,2]
    OOCPi0Calib.LocalCalibTools     = [LCOutPi0]

    OOCPi0Calib += LCOutPi0

    DMCalib    = CaloClusterLocalCalib ("DMCalibForTaus")
    DMCalib.ClusterRecoStatus   = [1,2]
    #DMCalib.LocalCalibToolNames = [DMTool.getFullName()]
    #DMCalib += DMTool
    DMCalib.LocalCalibTools      = [LCDeadMaterial]

    DMCalib += LCDeadMaterial

TopoClusterForTaus = CaloTopoClusterMaker("TauPi0TopoClusterMaker")

TopoClusterForTaus.CellsName = "TauCommonPi0CellContainer"
TopoClusterForTaus.CalorimeterNames=["LAREM"]
TopoClusterForTaus.SeedSamplingNames = [
    "PreSamplerB", "EMB1", "EMB2",
    "PreSamplerE", "EME1", "EME2"
    ]
TopoClusterForTaus.CaloNoiseTool                     = theCaloNoiseTool
TopoClusterForTaus.UseCaloNoiseTool                  = True
TopoClusterForTaus.UsePileUpNoise                    = True
TopoClusterForTaus.NeighborOption                    = "super3D"
TopoClusterForTaus.RestrictHECIWandFCalNeighbors     = False
TopoClusterForTaus.CellThresholdOnEorAbsEinSigma     = 0.0
TopoClusterForTaus.NeighborThresholdOnEorAbsEinSigma = 2.0
TopoClusterForTaus.SeedThresholdOnEorAbsEinSigma     = 4.0
TopoClusterForTaus.SeedCutsInAbsE                    = True
TopoClusterForTaus.ClusterEtorAbsEtCut               = 0.5*GeV
TopoClusterForTaus.TwoGaussianNoise                  = jobproperties.CaloTopoClusterFlags.doTwoGaussianNoise()


TopoSplitterForTaus = CaloTopoClusterSplitter("TauPi0TopoSplitter")
# cells from the following samplings will be able to form local
# maxima. The excluded samplings are PreSamplerB, EMB1,
# PreSamplerE, EME1, all Tile samplings, all HEC samplings and the
# two rear FCal samplings.
TopoSplitterForTaus.SamplingNames = ["EMB2","EME2"]
# cells from the following samplings will also be able to form
# local maxima but only if they are not overlapping in eta and phi
# with local maxima in previous samplings from the primary list.
TopoSplitterForTaus.SecondarySamplingNames = ["EMB1","EME1"]
TopoSplitterForTaus.ShareBorderCells = True
TopoSplitterForTaus.RestrictHECIWandFCalNeighbors  = False

TopoMomentsForTaus = CaloClusterMomentsMaker ("TauPi0TopoMoments")
TopoMomentsForTaus.MaxAxisAngle = 30*deg
TopoMomentsForTaus.OutputLevel = INFO
TopoMomentsForTaus.MomentsNames = [
    "FIRST_PHI" 
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

#TopoMomentsForTaus.AODMomentsNames = [ 
#    "FIRST_ETA"
#    ,"SECOND_R"
#    ,"SECOND_LAMBDA"
#    ,"DELTA_PHI"
#    ,"DELTA_THETA"
#    ,"CENTER_MAG"
#    ,"CENTER_LAMBDA"
#    ,"LATERAL"
#    ,"LONGITUDINAL"
#    ,"ENG_FRAC_EM"
#    ,"ENG_FRAC_MAX"
#    ,"ENG_FRAC_CORE"
#    ,"FIRST_ENG_DENS"
#    ,"SECOND_ENG_DENS"
#    ,"ISOLATION"
#    ]

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
#                
#if jobproperties.CaloTopoClusterFlags.printTopoClusters():
#    PrintCaloCluster = CaloClusterPrinter("PrintCaloCluster")
#    PrintCaloCluster.PrintFirstOnly = True
#    PrintCaloCluster.PrintFrequency = 1
#    PrintCaloCluster.EnergyUnit     = 1.0*GeV

cluster_container = 'TauPi0SubtractedClusterContainer'
CaloTopoForTausMaker = CaloClusterMaker ("TauPi0BonnSubtractedClusterMaker")
CaloTopoForTausMaker.ClustersOutputName=cluster_container
CaloTopoForTausMaker.ClusterMakerTools=[
    TopoClusterForTaus.getFullName(),
    TopoSplitterForTaus.getFullName()]
CaloTopoForTausMaker.ClusterCorrectionTools = [
    TopoMomentsForTaus.getFullName()]

CaloTopoForTausMaker += TopoClusterForTaus
CaloTopoForTausMaker += TopoSplitterForTaus
CaloTopoForTausMaker += TopoMomentsForTaus

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
    CaloTopoForTausMaker.KeepCorrectionToolAndContainerNames += [
        LocalCalib.getFullName(),"CaloTopoForTausMaker"]
    #    CaloTopoForTausMaker.KeepEachCorrection=True
    CaloTopoForTausMaker += LocalCalib
    CaloTopoForTausMaker += OOCCalib
    CaloTopoForTausMaker += OOCPi0Calib
    CaloTopoForTausMaker += DMCalib
                                    
#if jobproperties.CaloTopoClusterFlags.printTopoClusters():
#    CaloTopoForTausMaker.ClusterCorrectionTools += [
#        PrintCaloCluster.getFullName()]
#    CaloTopoForTausMaker += PrintCaloCluster
    
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

#CaloCell2TopoClusterForTausMapper =   CaloCell2ClusterMapper("CaloCell2Pi0ClusterForTausMapper")
#CaloCell2TopoClusterForTausMapper.ClustersName = cluster_container 
#CaloCell2TopoClusterForTausMapper.MapOutputName = "CaloCell2Pi0ClusterForTaus"

topSequence += CaloTopoForTausMaker
#topSequence += CaloCell2TopoClusterForTausMapper

