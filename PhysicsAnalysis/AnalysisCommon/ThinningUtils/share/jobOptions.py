import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections= ["AOD.09897018._000001.pool.root.1"]
theApp.EvtMax=100
algseq = CfgMgr.AthSequencer("AthAlgSeq") #gets a handle to the main athsequencer, for adding things to later!
algseq += CfgMgr.ThinGeantTruthAlg("ThinGeantTruthAlg", StreamName = 'StreamXAOD')

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolRootStream( "StreamXAOD", "xAOD.out.root" ) #suppose this is what your stream definition looked like
xaodStream.AddItem( ["xAOD::TruthParticleContainer#*","xAOD::TruthParticleAuxContainer#*","xAOD::TruthVertexContainer#*","xAOD::TruthVertexAuxContainer#*"] )
