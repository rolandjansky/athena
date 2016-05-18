################################################################################
##
#@file Pi0ClusterMaker_Crakow_jobOptions.py
#
#@brief jobOption to create clusters for the "Crakow" Pi0 Finder.
#
# Use cell container created by TauRecCoreBuilder as an input.
################################################################################
from CaloRec.CaloRecConf import CaloTopoClusterMaker, CaloTopoClusterSplitter, CaloClusterMomentsMaker, CaloClusterMaker, CaloCell2ClusterMapper
from CaloRec.CaloTopoClusterFlags import jobproperties
from AthenaCommon.SystemOfUnits import deg, GeV, MeV
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.GlobalFlags import globalflags


from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
theCaloNoiseTool = CaloNoiseToolDefault()
from AthenaCommon.AppMgr import ToolSvc
ToolSvc += theCaloNoiseTool

TopoClusterForTaus = CaloTopoClusterMaker("TopoClusterForTaus")

TopoClusterForTaus.CellsNames = ["TauCells"]
#TopoClusterForTaus.OutputLevel=2
TopoClusterForTaus.CalorimeterNames=["LAREM"]
TopoClusterForTaus.SeedSamplingNames = [
	  "PreSamplerB", "EMB1", "EMB2",
	  "PreSamplerE", "EME1", "EME2"
          ]
TopoClusterForTaus.CaloNoiseTool=theCaloNoiseTool
TopoClusterForTaus.UseCaloNoiseTool=True
TopoClusterForTaus.UsePileUpNoise=True
TopoClusterForTaus.NeighborOption = "super3D"
TopoClusterForTaus.RestrictHECIWandFCalNeighbors  = False
TopoClusterForTaus.CellThresholdOnEorAbsEinSigma     =    0.0
TopoClusterForTaus.NeighborThresholdOnEorAbsEinSigma =    2.0
TopoClusterForTaus.SeedThresholdOnEorAbsEinSigma  =    4.0
TopoClusterForTaus.SeedCutsInAbsE                 = True
TopoClusterForTaus.ClusterEtorAbsEtCut            = 1*GeV


TopoSplitterForTaus = CaloTopoClusterSplitter("TopoSplitterForTaus")
# cells from the following samplings will be able to form local
# maxima. The excluded samplings are PreSamplerB, EMB1,
# PreSamplerE, EME1, all Tile samplings, all HEC samplings and the
# two rear FCal samplings.
#
#TopoSplitterForTaus.OutputLevel=2
TopoSplitterForTaus.SamplingNames = ["EMB2","EME2"]
# cells from the following samplings will also be able to form
# local maxima but only if they are not overlapping in eta and phi
# with local maxima in previous samplings from the primary list.
#
TopoSplitterForTaus.SecondarySamplingNames = ["EMB1","EME1"]
TopoSplitterForTaus.ShareBorderCells = True
TopoSplitterForTaus.RestrictHECIWandFCalNeighbors  = False

TopoMomentsForTaus = CaloClusterMomentsMaker ("TopoMomentsForTaus")
TopoMomentsForTaus.MaxAxisAngle = 30*deg
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
  ,"CENTER_LAMBDA"
  ,"LATERAL"
  ,"LONGITUDINAL"
  ,"FIRST_ENG_DENS" 
  ,"ENG_FRAC_EM" 
  ,"ENG_FRAC_MAX" 
  ,"ENG_FRAC_CORE" 
  ,"FIRST_ENG_DENS" 
  ,"SECOND_ENG_DENS" 
]


CaloTopoForTausMaker = CaloClusterMaker ("CaloTopoForTausMaker")
CaloTopoForTausMaker.ClustersOutputName="EMTopoForTaus"
CaloTopoForTausMaker.ClusterMakerTools=[
    TopoClusterForTaus.getFullName(),
    TopoSplitterForTaus.getFullName()]
CaloTopoForTausMaker.ClusterCorrectionTools = [
    TopoMomentsForTaus.getFullName()]

CaloTopoForTausMaker += TopoClusterForTaus
CaloTopoForTausMaker += TopoSplitterForTaus
CaloTopoForTausMaker += TopoMomentsForTaus

CaloCell2TopoClusterForTausMapper =   CaloCell2ClusterMapper("CaloCell2TopoClusterForTausMapper")
CaloCell2TopoClusterForTausMapper.ClustersName = "EMTopoForTaus"
CaloCell2TopoClusterForTausMapper.MapOutputName = "CaloCell2TopoClusterForTaus"


topSequence += CaloTopoForTausMaker
topSequence += CaloCell2TopoClusterForTausMapper
    

    

