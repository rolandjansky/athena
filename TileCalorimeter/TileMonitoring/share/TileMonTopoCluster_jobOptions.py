#
# $Id: TileMonTopoCluster_jobOptions.py,v 1.4 2009-04-24 14:03:10 lfiorini Exp $
#
# File: CaloRec/share/CaloTopoCluster_jobOptions.py
# Created: June 2007, S.Menke
# Purpose: Define default calibrated topo cluster algo and corrections
#

Rel17Style=False

#from CaloUtils.CaloUtilsConf import H1ClusterCellWeightTool, EMFracClusterClassificationTool, OutOfClusterCorrectionTool, DeadMaterialCorrectionTool2

from CaloClusterCorrection.CaloClusterCorrectionConf import CaloClusterLocalCalib

from CaloRec.CaloRecConf import CaloTopoClusterMaker, CaloTopoClusterSplitter, CaloClusterMomentsMaker, CaloClusterMaker, CaloCell2ClusterMapper
if Rel17Style:
    from CaloRec.CaloRecConf import CaloClusterLockVars, CaloClusterPrinter
from CaloRec.CaloTopoClusterFlags import jobproperties
from AthenaCommon.SystemOfUnits import deg, GeV, MeV
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.GlobalFlags import globalflags

include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py")
include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
include( "LArDetDescr/LArDetDescr_joboptions.py" )
#include("LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")


from CaloRec.CaloTopoClusterFlags import jobproperties

from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
CaloNoiseCondAlg()

from AthenaCommon.AppMgr import ToolSvc
#ToolSvc += theCaloNoiseTool

doTopoClusterLocalCalib=False

from CaloRec.CaloTopoClusterFlags import jobproperties
jobproperties.CaloTopoClusterFlags.doTopoClusterLocalCalib.set_Value(False) 

# correction tools not using tools
TileTopoMoments = CaloClusterMomentsMaker ("TileTopoMoments")
TileTopoMoments.MaxAxisAngle = 30*deg
TileTopoMoments.OutputLevel = INFO
TileTopoMoments.MomentsNames = ["FIRST_PHI" 
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
if Rel17Style:
    TileTopoMoments.AODMomentsNames = ["LATERAL"
                                   ,"LONGITUDINAL"
                                   ,"SECOND_R"
                                   ,"SECOND_LAMBDA"
                                   ,"CENTER_LAMBDA"
                                   ,"FIRST_ENG_DENS"
                                   ,"ENG_FRAC_MAX"
                                   ,"ISOLATION"]   

if Rel17Style:
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
        
if Rel17Style:
  if jobproperties.CaloTopoClusterFlags.printTopoClusters():
    PrintCaloCluster = CaloClusterPrinter("PrintCaloCluster")
    PrintCaloCluster.PrintFirstOnly = True
    PrintCaloCluster.PrintFrequency = 1
    PrintCaloCluster.EnergyUnit     = 1.0*GeV


# maker tools
TileTopoMaker = CaloTopoClusterMaker("TileTopoMaker")

TileTopoMaker.CellsName = "AllCalo"
TileTopoMaker.CalorimeterNames=["TILE"]
# cells from the following samplings will be able to form
# seeds. By default no sampling is excluded
TileTopoMaker.SeedSamplingNames = ["TileBar0", "TileBar1", "TileBar2",
                                   "TileExt0", "TileExt1", "TileExt2",
                                   "TileGap1", "TileGap2", "TileGap3"] 
#TileTopoMaker.NoiseSigma= 1.68 / 1023 / 64 * 800 * 1.414214 * GeV
TileTopoMaker.NeighborOption = "super3D"
TileTopoMaker.RestrictHECIWandFCalNeighbors  = False
TileTopoMaker.CellThresholdOnEorAbsEinSigma  =    0.0
TileTopoMaker.NeighborThresholdOnEorAbsEinSigma = 2.0
TileTopoMaker.SeedThresholdOnEorAbsEinSigma  =    4.0
# note E or AbsE 
#
# the following cut must be set to TRUE in order to make double
# sided cuts on the seed and the cluster level ( neighbor and cell
# cuts are always double sided)
#
TileTopoMaker.SeedCutsInAbsE                 = True
TileTopoMaker.ClusterEtorAbsEtCut            = 0.0*MeV
# the following Et thresholds are ignored in case UsePileUpNoise
# is TRUE
#
#
#CellThresholdOnEtorAbsEt = 0.0*MeV
#NeighborThresholdOnEtorAbsEt = 100.0*MeV
#SeedThresholdOnEtorAbsEt = 200.0*MeV
# note Et or AbsEt
#Double gaussian noise
#TileTopoMaker.TwoGaussianNoise = True
#TileTopoMaker.OutputLevel=1
#theCaloNoiseTool.OutputLevel=1
TileTopoMaker.TwoGaussianNoise = jobproperties.CaloTopoClusterFlags.doTwoGaussianNoise()


TileTopoSplitter = CaloTopoClusterSplitter("TileTopoSplitter")
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
TileTopoSplitter.SamplingNames = [ "TileBar0", "TileBar1", "TileBar2" ,
                                   "TileExt0", "TileExt1", "TileExt2" ,
                                   "TileGap1", "TileGap2", "TileGap3"]

#TileTopoSplitter.SecondarySamplingNames = ["TileBar0","TileBar1","TileBar2",
#                                           "TileExt0","TileExt1","TileExt2"]
TileTopoSplitter.ShareBorderCells = True
TileTopoSplitter.RestrictHECIWandFCalNeighbors  = False
#
# the following options are not set, since these are the default
# values
#
# NeighborOption                = "super3D",
# NumberOfCellsCut              = 4,
# EnergyCut                     = 500*MeV,

 
# cluster maker
TileTopoCluster = CaloClusterMaker ("TileTopoCluster")

TileTopoCluster.ClustersOutputName="TileTopoCluster"
TileTopoCluster.ClusterMakerTools = [
    TileTopoMaker.getFullName(),
    TileTopoSplitter.getFullName()]
TileTopoCluster.ClusterCorrectionTools = [
    TileTopoMoments.getFullName()]

TileTopoCluster += TileTopoMaker
TileTopoCluster += TileTopoSplitter
TileTopoCluster += TileTopoMoments

if Rel17Style:
  if jobproperties.CaloTopoClusterFlags.lockTopoClusterSamplingEnergies() or jobproperties.CaloTopoClusterFlags.lockTopoClusterSamplingVariables():
    TileTopoCluster.ClusterCorrectionTools += [
        LockVariables.getFullName()]
    TileTopoCluster += LockVariables
    
if jobproperties.CaloTopoClusterFlags.doTopoClusterLocalCalib():
    TileTopoCluster.ClusterCorrectionTools += [
        LocalCalib.getFullName(),
        OOCCalib.getFullName(),
        OOCPi0Calib.getFullName(),
        DMCalib.getFullName()]

    TileTopoCluster += LocalCalib
    TileTopoCluster += OOCCalib
    TileTopoCluster += OOCPi0Calib
    TileTopoCluster += DMCalib
    
if Rel17Style:
  if jobproperties.CaloTopoClusterFlags.printTopoClusters():
    TileTopoCluster.ClusterCorrectionTools += [
    PrintCaloCluster.getFullName()]
    TileTopoCluster += PrintCaloCluster

topSequence += TileTopoCluster
printfunc (TileTopoCluster)

if Rel17Style:
  # cell 2 cluster mapper
  TileCell2TopoClusterMapper =   CaloCell2ClusterMapper("TileCell2TopoClusterMapper")
  TileCell2TopoClusterMapper.ClustersName = "TileTopoCluster"
  TileCell2TopoClusterMapper.MapOutputName = "TileCell2TopoCluster"
  topSequence += TileCell2TopoClusterMapper

