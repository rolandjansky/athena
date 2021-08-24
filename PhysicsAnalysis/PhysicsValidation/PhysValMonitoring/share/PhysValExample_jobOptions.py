from PhysValMonitoring.PhysValMonitoringConf import PhysVal__PhysValExample
tool1 = PhysVal__PhysValExample()
tool1.EnableLumi = False
tool1.OutputLevel = WARNING
tool1.DetailLevel = 10

tool1.TauContainerName = "TauJets"
tool1.PhotonContainerName = "Photons"
tool1.ElectronContainerName = "Electrons"
tool1.METContainerName = "MET_Reference_AntiKt4LCTopo"



from RecExConfig.AutoConfiguration import IsInInputFile
tool1.DoExMET=IsInInputFile('xAOD::MissingETContainer',"MET_Reference_AntiKt4LCTopo")
tool1.DoExJet=IsInInputFile('xAOD::JetContainer','AntiKt4EMTopoJets')
tool1.DoExBtag=IsInInputFile('xAOD::JetContainer','AntiKt4EMTopoJets')


monMan = CfgMgr.AthenaMonManager("PhysValMonManager")
monMan.AthenaMonTools += [ tool1 ]
