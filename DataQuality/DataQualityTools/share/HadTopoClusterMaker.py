# by JAA based on TileMinTopoClusterMaker

#removed by JAA
###from CaloUtils.CaloUtilsConf import H1ClusterCellWeightTool, EMFracClusterClassificationTool, OutOfClusterCorrectionTool, DeadMaterialCorrectionTool2

from CaloClusterCorrection.CaloClusterCorrectionConf import CaloClusterLocalCalib

from CaloRec.CaloRecConf import CaloTopoClusterMaker, CaloTopoClusterSplitter, CaloClusterMomentsMaker, CaloClusterMaker, CaloCell2ClusterMapper, CaloClusterLockVars, CaloClusterPrinter
from CaloRec.CaloTopoClusterFlags import jobproperties
from AthenaCommon.SystemOfUnits import deg, GeV, MeV
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.GlobalFlags import globalflags

#from AthenaCommon.AlgSequence import AlgSequence
#topSequence = AlgSequence()


doTopoClusterLocalCalib=False

from CaloRec.CaloTopoClusterFlags import jobproperties
jobproperties.CaloTopoClusterFlags.doTopoClusterLocalCalib.set_Value(False) 

# correction tools not using tools
HadTopoMoments = CaloClusterMomentsMaker ("HadTopoMoments")
HadTopoMoments.MaxAxisAngle = 30*deg
HadTopoMoments.OutputLevel = INFO
HadTopoMoments.MomentsNames = ["FIRST_PHI" 
                                ,"FIRST_ETA"
                                ,"SECOND_R" 
                                ,"SECOND_LAMBDA"
                                ,"DELTA_PHI"
                                ,"DELTA_THETA"
                                ,"DELTA_ALPHA" 
                                ,"CENTER_X"
                                ,"CENTER_Y"
                                ,"CENTER_Z"
                                ,"CENTER_LAMBDA"
                                ,"LATERAL"
                                ,"LONGITUDINAL"
                                ,"FIRST_ENG_DENS" 
                                ,"ENG_FRAC_EM" 
                                ,"ENG_FRAC_MAX" 
                                ,"ENG_FRAC_CORE" 
                                ,"FIRST_ENG_DENS" 
                                ,"SECOND_ENG_DENS" 
                                ,"ISOLATION"]
HadTopoMoments.AODMomentsNames = ["LATERAL"
                                   ,"LONGITUDINAL"
                                   ,"SECOND_R" 
                                   ,"SECOND_LAMBDA"
                                   ,"CENTER_LAMBDA"
                                   ,"FIRST_ENG_DENS"
                                   ,"ENG_FRAC_MAX" 
                                   ,"ISOLATION"]   

if jobproperties.CaloTopoClusterFlags.lockTopoClusterSamplingEnergies() or jobproperties.CaloTopoClusterFlags.lockTopoClusterSamplingVariables():
    LockVariables = CaloClusterLockVars("LockVariables")
    LockVariables.LockedSamplingVariables = []
    if jobproperties.CaloTopoClusterFlags.lockTopoClusterSamplingEnergies():
        LockVariables.LockedSamplingVariables += [
            "Energy", "Max_Energy"]
    if jobproperties.CaloTopoClusterFlags.lockTopoClusterSamplingVariables():    
        LockVariables.LockedSamplingVariables += [
            "Eta", "Phi", "Delta_Eta",
            "Delta_Phi", "Max_Eta", "Max_Phi"
            ]
        
if jobproperties.CaloTopoClusterFlags.printTopoClusters():
    PrintCaloCluster = CaloClusterPrinter("PrintCaloCluster")
    PrintCaloCluster.PrintFirstOnly = True
    PrintCaloCluster.PrintFrequency = 1
    PrintCaloCluster.EnergyUnit     = 1.0*GeV


# maker tools
HadTopoMaker = CaloTopoClusterMaker("HadTopoMaker")

HadTopoMaker.CellsNames = ["AllCalo"]
HadTopoMaker.CalorimeterNames=["TILE","LARHEC"]
# cells from the following samplings will be able to form
# seeds. By default no sampling is excluded
HadTopoMaker.SeedSamplingNames = ["TileBar0", "TileBar1", "TileBar2",
                                   "TileExt0", "TileExt1", "TileExt2",
                                  "HEC0","HEC1","HEC2","HEC3",
                                   "TileGap1", "TileGap2", "TileGap3"] 
HadTopoMaker.NoiseSigma= 1.68 / 1023 / 64 * 800 * 1.414214 * GeV
HadTopoMaker.NeighborOption = "super3D"
HadTopoMaker.RestrictHECIWandFCalNeighbors  = False
HadTopoMaker.SeedThresholdOnEorAbsEinSigma  =    4.0
HadTopoMaker.CellThresholdOnEorAbsEinSigma     =    0.0
HadTopoMaker.NeighborThresholdOnEorAbsEinSigma =    2.0



# note E or AbsE 
#
# the following cut must be set to TRUE in order to make double
# sided cuts on the seed and the cluster level ( neighbor and cell
# cuts are always double sided)
#
HadTopoMaker.SeedCutsInAbsE                 = True
HadTopoMaker.ClusterEtorAbsEtCut            = 0.0*MeV
# the following Et thresholds are ignored in case UsePileUpNoise
# is TRUE
#
#
#CellThresholdOnAbsEt = 0.0*MeV
#NeighborThresholdOnAbsEt = 100.0*MeV
#SeedThresholdOnEtorAbsEt = 200.0*MeV
# note Et or AbsEt

HadTopoSplitter = CaloTopoClusterSplitter("HadTopoSplitter")
# cells from the following samplings will be able to form local
# maxima. The excluded samplings are PreSamplerB, EMB1,
# PreSamplerE, EME1, all Tile samplings, all HEC samplings and the
# two rear FCal samplings.
#
#TopoSplitter.SamplingNames = ["EMB2", "EMB3",
#                              "EME2", "EME3",
#                              "FCAL0"]
# cells from the following samplings will also be able to form
# local maxima but only if they are not overlapping in eta and phi
# with local maxima in previous samplings from the primary list.
#
HadTopoSplitter.SamplingNames = [ "TileBar0", "TileBar1", "TileBar2" ,
                                   "TileExt0", "TileExt1", "TileExt2",
                                  "HEC0","HEC1","HEC2","HEC3",
                                   "TileGap1", "TileGap2", "TileGap3"]

HadTopoSplitter.SecondarySamplingNames = ["TileBar0","TileBar1","TileBar2",
                                           "HEC0","HEC1","HEC2","HEC3",
                                           "TileExt0","TileExt1","TileExt2"]
HadTopoSplitter.ShareBorderCells = True
HadTopoSplitter.RestrictHECIWandFCalNeighbors  = False
#
# the following options are not set, since these are the default
# values
#
# NeighborOption                = "super3D",
# NumberOfCellsCut              = 4,
# EnergyCut                     = 500*MeV,

 
# cluster maker
HadTopoCluster = CaloClusterMaker ("HadTopoCluster")

HadTopoCluster.ClustersOutputName="HadTopoCluster"
HadTopoCluster.ClusterMakerTools = [
    HadTopoMaker.getFullName(),
    HadTopoSplitter.getFullName()]
HadTopoCluster.ClusterCorrectionTools = [
    HadTopoMoments.getFullName()]

HadTopoCluster += HadTopoMaker
HadTopoCluster += HadTopoSplitter
HadTopoCluster += HadTopoMoments

if jobproperties.CaloTopoClusterFlags.lockTopoClusterSamplingEnergies() or jobproperties.CaloTopoClusterFlags.lockTopoClusterSamplingVariables():
    HadTopoCluster.ClusterCorrectionTools += [
        LockVariables.getFullName()]
    HadTopoCluster += LockVariables
    
if jobproperties.CaloTopoClusterFlags.doTopoClusterLocalCalib():
    HadTopoCluster.ClusterCorrectionTools += [
        LocalCalib.getFullName(),
        OOCCalib.getFullName(),
        OOCPi0Calib.getFullName(),
        DMCalib.getFullName()]

    HadTopoCluster += LocalCalib
    HadTopoCluster += OOCCalib
    HadTopoCluster += OOCPi0Calib
    HadTopoCluster += DMCalib
    
if jobproperties.CaloTopoClusterFlags.printTopoClusters():
    HadTopoCluster.ClusterCorrectionTools += [
    PrintCaloCluster.getFullName()]
    HadTopoCluster += PrintCaloCluster


# cell 2 cluster mapper
HadCell2TopoClusterMapper =   CaloCell2ClusterMapper("HadCell2TopoClusterMapper")
HadCell2TopoClusterMapper.ClustersName = "HadTopoCluster"
HadCell2TopoClusterMapper.MapOutputName = "HadCell2TopoCluster"

topSequence += HadTopoCluster
topSequence += HadCell2TopoClusterMapper

print HadTopoCluster


