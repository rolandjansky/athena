
include.block( "CaloClusterCorrection/CaloTopoLocalCalibCone_jobOptions.py" )

#
# jobOptions for the local calibration of CaloTopoClusters with the cone method
#

theApp.Dlls += ["CaloClusterCorrection", "CaloUtils" ]

CaloTopoClusterMaker.ClusterCorrectionTools += [
"CaloClusterLocalCalib/Classification",
"CaloClusterLocalCalibCone/ConeCalibration"
]

# use new name for calibrated clusters and old name for em-scale clusters

CaloTopoClusterMaker.ClustersOutputName = "CaloCalTopoCluster"
CaloTopoClusterMaker.KeepCorrectionToolAndContainerNames += ["CaloClusterLocalCalib/Classification","CaloTopoCluster"]
CaloTopoClusterMaker.TopoMoments.MomentsNames += [
  "FIRST_ETA",  
  "CENTER_LAMBDA",
  "FIRST_ENG_DENS"
]

CaloTopoClusterMaker.Classification.ClusterClassificationToolName = "EMFracClusterClassificationTool/EMFrac"
if DetDescrVersion.startswith("Rome"):
    CaloTopoClusterMaker.Classification.EMFrac.EMFractionFileName = "EMFrac_classify_20060603_RZG.root"
    CaloTopoClusterMaker.Classification.EMFrac.EnergyBins = [ 0.0*GeV, 1.0*GeV,
4.0*GeV, 16.0*GeV, 64.0*GeV, 4.0*TeV ]
    CaloTopoClusterMaker.ConeCalibration.HadWeightFileName = "H1ClusterCellWeights_20060705_RZG.root"

else:
    CaloTopoClusterMaker.Classification.EMFrac.EMFractionFileName = "EMFrac_classify_20061117_RZG.root"
    CaloTopoClusterMaker.Classification.EMFrac.EnergyBins = [ 0.0*GeV, 1.0*GeV,
4.0*GeV, 16.0*GeV, 64.0*GeV, 264.0*GeV, 4.0*TeV ]
    CaloTopoClusterMaker.ConeCalibration.HadWeightFileName = "/afs/cern.ch/user/m/menke/public/postbarcelona_12.0.3_weights_20061118_j0.2_1.0_2sigma_celleta.root"

CaloTopoClusterMaker.Classification.EMFrac.EtaBins = [ 0.0, 0.2, 0.4,
0.6, 0.8, 1.0, 1.2, 1.4, 1.6, 1.8, 2.0, 2.2, 2.4, 2.6, 2.8, 3.0, 3.2,
3.4, 3.6, 3.8, 4.0, 4.2, 4.4, 4.6, 4.8, 5.0 ]
CaloTopoClusterMaker.Classification.EMFrac.UseEMFractionSpread = TRUE
CaloTopoClusterMaker.Classification.EMFrac.MaxEMFraction = 1.0

CaloTopoClusterMaker.ConeCalibration.ClusterRecoStatus = 2 # 0 = ANY, 1 = EM, 2 = HAD, 4 = MUON
CaloTopoClusterMaker.ConeCalibration.EtaBins = [ 0.0, 0.2, 0.4,
0.6, 0.8, 1.0, 1.2, 1.4, 1.6, 1.8, 2.0, 2.2, 2.4, 2.6, 2.8, 3.0, 3.2,
3.4, 3.6, 3.8, 4.0, 4.2, 4.4, 4.6, 4.8, 5.0 ]
CaloTopoClusterMaker.ConeCalibration.CaloIndices = [
    -1,0,0,0
    ,-1,1,1,1
    ,2,2,2,2
    ,4,4,4
    ,-1,-1,-1
    ,4,4,4
    ,3,3,3 ]
CaloTopoClusterMaker.ConeCalibration.SamplingIndices = [
    -1,1,2,3
    ,-1,1,2,3
    ,0,1,2,3
    ,0,1,2
    ,-1,-1,-1
    ,0,1,2
    ,1,2,3 ]
CaloTopoClusterMaker.ConeCalibration.SignalOverNoiseCut = 2.0




