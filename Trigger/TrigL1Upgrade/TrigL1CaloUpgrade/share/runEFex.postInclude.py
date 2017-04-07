

if ( "StreamAOD" in dir() ) :

  from TrigL1CaloUpgrade.TrigL1CaloUpgradeConf import TrigT1CaloEFex
  from TrigL1CaloUpgrade.TrigL1CaloUpgradeConf import TrigT1CaloTauFex
  from TrigL1CaloUpgrade.TrigL1CaloUpgradeConf import LArFex

  from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
  theCaloNoiseTool=CaloNoiseToolDefault()
  ToolSvc+=theCaloNoiseTool

  theTrigT1CaloEFex=TrigT1CaloEFex()
  theTrigT1CaloEFexCl=TrigT1CaloEFex(name="TrigT1CaloEFexCl",CleanCellContainer=True,OutputClusterName="SClusterCl")
  theTrigT1CaloTauFex=TrigT1CaloTauFex()
  theLArFex=LArFex(EtInSigma=3.0,CaloNoiseTool=theCaloNoiseTool)
  theLArFex1Sig=LArFex(name="1Sig",EtInSigma=3.0,CaloNoiseTool=theCaloNoiseTool,EtInSigmaSelect=1.0,OutputClusterName="LArLayer1Vars1Sig")
  theLArFex2Sig=LArFex(name="2Sig",EtInSigma=3.0,CaloNoiseTool=theCaloNoiseTool,EtInSigmaSelect=2.0,OutputClusterName="LArLayer1Vars2Sig")
  theLArFex3Sig=LArFex(name="3Sig",EtInSigma=3.0,CaloNoiseTool=theCaloNoiseTool,EtInSigmaSelect=3.0,OutputClusterName="LArLayer1Vars3Sig")

  topSequence+=theTrigT1CaloEFex
  topSequence+=theTrigT1CaloEFexCl
  topSequence+=theTrigT1CaloTauFex
  topSequence+=theLArFex
  topSequence+=theLArFex1Sig
  topSequence+=theLArFex2Sig
  topSequence+=theLArFex3Sig

  StreamAOD.ItemList+=["xAOD::TrigEMClusterContainer#SCluster","xAOD::TrigEMClusterContainer#SClusterCl","xAOD::TrigEMClusterContainer#LArLayer1Vars","xAOD::TrigEMClusterContainer#LArLayer1Vars1Sig","xAOD::TrigEMClusterContainer#LArLayer1Vars2Sig","xAOD::TrigEMClusterContainer#LArLayer1Vars3Sig","xAOD::TrigEMClusterAuxContainer#SClusterAux.","xAOD::TrigEMClusterAuxContainer#SClusterClAux.","xAOD::TrigEMClusterAuxContainer#LArLayer1VarsAux.","xAOD::TrigEMClusterAuxContainer#LArLayer1Vars1SigAux.","xAOD::TrigEMClusterAuxContainer#LArLayer1Vars2SigAux.","xAOD::TrigEMClusterAuxContainer#LArLayer1Vars3SigAux."]

  topSequence.remove(StreamAOD)
  topSequence+=StreamAOD
