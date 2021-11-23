
include.block( "CaloClusterCorrection/CaloTopoLocalCalib_jobOptions.py" )

#
# jobOptions for the local calibration of CaloTopoClusters
#

theApp.Dlls += ["CaloClusterCorrection", "CaloUtils" ]

CaloTopoClusterMaker.ClusterCorrectionTools += [
"CaloClusterLocalCalib/LocalCalib",
"CaloClusterLocalCalib/OOCCalib"
]

# use new name for calibrated clusters and old name for em-scale clusters

CaloTopoClusterMaker.ClustersOutputName = "CaloCalTopoClusters"
CaloTopoClusterMaker.KeepCorrectionToolAndContainerNames += ["CaloClusterLocalCalib/LocalCalib","CaloTopoClusters"]
CaloTopoClusterMaker.KeepCorrectionToolAndContainerNames += ["CaloClusterLocalCalib/OOCCalib","CaloWeightTopoCluster"]
CaloTopoClusterMaker.TopoMoments.MomentsNames += [
  "FIRST_ETA",  
  "CENTER_LAMBDA",
  "FIRST_ENG_DENS",
  "ISOLATION"
]

CaloTopoClusterMaker.LocalCalib.ClusterClassificationToolName = "EMFracClusterClassificationTool/EMFrac"
CaloTopoClusterMaker.LocalCalib.ClusterRecoStatus = [2] # 0 = ANY, 1 = EM, 2 = HAD, 4 = MUON, put more than 1 in a comma separated list
CaloTopoClusterMaker.LocalCalib.LocalCalibToolNames = [ "H1ClusterCellWeightTool/H1Weight" ]
CaloTopoClusterMaker.OOCCalib.ClusterRecoStatus = [2] # 0 = ANY, 1 = EM, 2 = HAD, 4 = MUON, put more than 1 in a comma separated list
CaloTopoClusterMaker.OOCCalib.LocalCalibToolNames = [ "OutOfClusterCorrectionTool/OOCC" ]

CaloTopoClusterMaker.LocalCalib.EMFrac.ClassificationKey = "EMFracClassify"
CaloTopoClusterMaker.LocalCalib.H1Weight.CorrectionKey = "H1ClusterCellWeights"
CaloTopoClusterMaker.LocalCalib.EMFrac.UseEMFractionSpread = FALSE
CaloTopoClusterMaker.LocalCalib.H1Weight.SignalOverNoiseCut = 2.0
CaloTopoClusterMaker.OOCCalib.OOCC.CorrectionKey = "OOCCorrection"

#
# pool/cool part
#
CaloDBConnection = CondDBCool.CALO
if DetDescrVersion.startswith("Rome"):
    CaloH1CalibTag="000-00"
    CaloEMFracTag="000-00"
    CaloTopoClusterMaker.LocalCalib.EMFrac.MaxEMFraction = 0.85
else:
    CaloH1CalibTag="001-00"
    CaloEMFracTag="002-00"
    CaloTopoClusterMaker.LocalCalib.EMFrac.MaxEMFraction = 0.5
Tag = "<tag>CaloH1CellWeights-"+CaloH1CalibTag+"</tag>"
IOVDbSvc.Folders+=["/CALO/HadCalibration/H1ClusterCellWeights"+CaloDBConnection+Tag]
Tag = "<tag>CaloEMFrac-"+CaloEMFracTag+"</tag>"
IOVDbSvc.Folders+=["/CALO/HadCalibration/CaloEMFrac"+CaloDBConnection+Tag]
CaloOOCCalibTag="000-00"
Tag = "<tag>CaloHadOOCCorr-"+CaloOOCCalibTag+"</tag>"
IOVDbSvc.Folders+=["/CALO/HadCalibration/CaloOutOfCluster"+CaloDBConnection+Tag]




