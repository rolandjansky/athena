#
# jobOptions for the local Dead Material calibration of CaloTopoClusters
#

include.block( "CaloClusterCorrection/CaloTopoLocalCalibDM_jobOptions.py" )

theApp.Dlls += ["CaloClusterCorrection", "CaloUtils"]

CaloTopoClusterMaker.ClusterCorrectionTools += [
"CaloClusterLocalCalib/LocalCalibDM"
]

CaloTopoClusterMaker.TopoMoments.MomentsNames += [
  "FIRST_ETA",
  "FIRST_PHI"
]

CaloTopoClusterMaker.LocalCalibDM.ClusterRecoStatus = 0 # 0 = ANY, 1 = EM, 2 = HAD, 4 = MUON

CaloTopoClusterMaker.LocalCalibDM.LocalCalibToolNames = [ "DeadMaterialCorrectionTool/DMTool" ]

CaloTopoClusterMaker.LocalCalibDM.DMTool.HadDMCoeffKey = "HadDMCoeff"
CaloTopoClusterMaker.LocalCalibDM.DMTool.SignalOverNoiseCut = 2.0
CaloTopoClusterMaker.LocalCalibDM.DMTool.ClusterRecoStatus = 0
# Way of adding DM energy to cluster:
# =0 setting new cluster energy, =1 - changing proportionally weights of all cells
# =2 changing weights of cells which were involved into DM calculations
CaloTopoClusterMaker.LocalCalibDM.DMTool.WeightModeDM = 2
CaloTopoClusterMaker.LocalCalibDM.ClusterRecoStatus = [1, 2]


#
# pool/cool part
#
CaloDBConnection = CondDBCool.CALO
CaloDMCalibTag="000-00"
Tag = "<tag>CaloHadDMCorr-"+CaloDMCalibTag+"</tag>"
IOVDbSvc.Folders+=["/CALO/HadCalibration/CaloDMCorr"+CaloDBConnection+Tag]
