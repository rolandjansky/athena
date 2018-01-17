from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

#RDO
#athenaCommonFlags.FilesInput = ['root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/phys-rig/pileupSamples/mc11_valid.105200.T1_McAtNlo_Jimmy.digit.RDO.e835_s1310_s1300_d639_tid682867_00/RDO.682867._000001.pool.root.1']

#AOD data
#athenaCommonFlags.FilesInput = ['root://eosatlas.cern.ch//eos/atlas/user/g/gbrooijm/dpd-rtt/data12_8TeV.00206299.physics_JetTauEtmiss.merge.AOD.f450_m1170._lb0078._0001.1']

#AOD MC
athenaCommonFlags.FilesInput = ['/afs/cern.ch/atlas/maxidisk/d33/referencefiles/aod/AOD-17.2.7.2/AOD-17.2.7.2-1.pool.root']




from RecExConfig.RecFlags import rec


rec.readESD=False
rec.readAOD=True
rec.readRDO=False
rec.doWriteESD=False 
rec.doWriteAOD=False 
rec.doAOD=False
rec.doWriteTAG=False 

athenaCommonFlags.EvtMax=5
#athenaCommonFlags.SkipEvents=6
#athenaCommonFlags.OutputLevel = 2

#from ParticleBuilderOptions.AODFlags import AODFlags
#AODFlags.FastSimulation = False

from JetRec.JetRecFlags import jetFlags
#jetFlags.Enable = False

# Setup jet algorithm timing (how long is spent on jet finding/reconstruction)
jetFlags.monitorTime = True

rec.UserExecs = ["myjets()"]

from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
primDPD.WritePhysDPDJetMET.JetCollectionNames = []

from JetRec.JetAlgConfiguration import checkAndUpdateOptions
#include("TestAssoc.py")

def myjets():
    from JetRec.JetRecFlags import jetFlags
    from JetRec.JetGetters import make_StandardJetGetter
    
    # Algorithms to test: AntiKt R = {0.4, 0.6} scheme = {EM, LC}
    #jetAlg4EM  = make_StandardJetGetter('AntiKt',0.4,'Topo',calibName='EM:OFFSET_ORIGIN_ETAJES',outputCollectionName='AntiKt4TopoJetsTest').jetAlgorithmHandle()
    #jetAlg4EM  = make_StandardJetGetter('AntiKt',0.4,'Topo',doCalib=False,minPt=5000,jetMoments=[],outputCollectionName='AntiKt4TopoJetsTest').jetAlgorithmHandle()
    #jetAlg4LC  = make_StandardJetGetter('AntiKt',0.4,'LCTopo',jetMoments=[],outputCollectionName='AntiKt4LCTopoJetsTest').jetAlgorithmHandle()
    jetAlg4EM  = make_StandardJetGetter('AntiKt',0.4,'Topo',doCalib=False,minPt=5000,outputCollectionName='AntiKt4TopoJetsTest').jetAlgorithmHandle()
    jetAlg4LC  = make_StandardJetGetter('AntiKt',0.4,'LCTopo',outputCollectionName='AntiKt4LCTopoJetsTest').jetAlgorithmHandle()
    jetAlg4Tr  = make_StandardJetGetter('AntiKt',0.4,'Truth',outputCollectionName='AntiKt4TruthJetsTest').jetAlgorithmHandle()
    
    #jetAlg6EM  = make_StandardJetGetter('AntiKt',0.6,'Topo',calibName='EM:OFFSET_ORIGIN_ETAJES',outputCollectionName='AntiKt6TopoJetsTest').jetAlgorithmHandle()
    #jetAlg6EM  = make_StandardJetGetter('AntiKt',0.6,'Topo',doCalib=False,minPt=5000,jetMoments=[],outputCollectionName='AntiKt6TopoJetsTest').jetAlgorithmHandle()
    #jetAlg6LC  = make_StandardJetGetter('AntiKt',0.6,'LCTopo',jetMoments=[],outputCollectionName='AntiKt6LCTopoJetsTest').jetAlgorithmHandle()
    jetAlg6EM  = make_StandardJetGetter('AntiKt',0.6,'Topo',doCalib=False,minPt=5000,outputCollectionName='AntiKt6TopoJetsTest').jetAlgorithmHandle()
    jetAlg6LC  = make_StandardJetGetter('AntiKt',0.6,'LCTopo',outputCollectionName='AntiKt6LCTopoJetsTest').jetAlgorithmHandle()
    jetAlg6Tr  = make_StandardJetGetter('AntiKt',0.6,'Truth',outputCollectionName='AntiKt6TruthJetsTest').jetAlgorithmHandle()

    # In devval, areas are set by default
    # However, we need to set second areas
    g = topSequence.AntiKt4TopoJetsTest.AlgTools[0]
    f = g.JetFindingSequence[-1]
    f.CalculateSecondArea = True

    g = topSequence.AntiKt4LCTopoJetsTest.AlgTools[0]
    f = g.JetFindingSequence[-1]
    f.CalculateSecondArea = True

    #g = topSequence.AntiKt4TruthJetsTest.AlgTools[0]
    #f = g.JetFindingSequence[-1]
    g = topSequence.AntiKt4TruthJetsTest.AlgTools[2]
    g.CalculateSecondArea = True
    
    g = topSequence.AntiKt6TopoJetsTest.AlgTools[0]
    f = g.JetFindingSequence[-1]
    f.CalculateSecondArea = True
    
    g = topSequence.AntiKt6LCTopoJetsTest.AlgTools[0]
    f = g.JetFindingSequence[-1]
    f.CalculateSecondArea = True

    #g = topSequence.AntiKt6TruthJetsTest.AlgTools[0]
    #f = g.JetFindingSequence[-1]
    g = topSequence.AntiKt6TruthJetsTest.AlgTools[2]
    g.CalculateSecondArea = True



    from JetValidation.JetTestConfig import scheduleJetTester
    # Now schedule the testers
    scheduleJetTester("CaloCalTopoCluster",JetMoments=False, CollectionType="CaloClusterContainer", JetSigState=False, JetAuthor=False, JetNConstituents=False, JetConstituents=False)
    
    scheduleJetTester("AntiKt4TopoJetsTest",JetMoments_opts = {'moments':['nTrk', 'nTrk_allpv','JVF','WIDTH','trackWIDTH','trackWIDTH_allpv','ActiveArea','ActiveAreaPx','ActiveAreaPy','ActiveAreaPz','ActiveAreaE','VoronoiArea','VoronoiAreaPx','VoronoiAreaPy','VoronoiAreaPz','VoronoiAreaE']})
    scheduleJetTester("AntiKt4LCTopoJetsTest",JetMoments_opts = {'moments':['nTrk', 'nTrk_allpv','JVF','WIDTH','trackWIDTH','trackWIDTH_allpv','ActiveArea','ActiveAreaPx','ActiveAreaPy','ActiveAreaPz','ActiveAreaE','VoronoiArea','VoronoiAreaPx','VoronoiAreaPy','VoronoiAreaPz','VoronoiAreaE']})
    scheduleJetTester("AntiKt4TruthJetsTest",JetMoments_opts = {'moments':['nTrk', 'nTrk_allpv','JVF','WIDTH','trackWIDTH','trackWIDTH_allpv','ActiveArea','ActiveAreaPx','ActiveAreaPy','ActiveAreaPz','ActiveAreaE','VoronoiArea','VoronoiAreaPx','VoronoiAreaPy','VoronoiAreaPz','VoronoiAreaE']})
    
    scheduleJetTester("AntiKt6TopoJetsTest",JetMoments_opts = {'moments':['nTrk', 'nTrk_allpv','JVF','WIDTH','trackWIDTH','trackWIDTH_allpv','ActiveArea','ActiveAreaPx','ActiveAreaPy','ActiveAreaPz','ActiveAreaE','VoronoiArea','VoronoiAreaPx','VoronoiAreaPy','VoronoiAreaPz','VoronoiAreaE']})
    scheduleJetTester("AntiKt6LCTopoJetsTest",JetMoments_opts = {'moments':['nTrk', 'nTrk_allpv','JVF','WIDTH','trackWIDTH','trackWIDTH_allpv','ActiveArea','ActiveAreaPx','ActiveAreaPy','ActiveAreaPz','ActiveAreaE','VoronoiArea','VoronoiAreaPx','VoronoiAreaPy','VoronoiAreaPz','VoronoiAreaE']})
    scheduleJetTester("AntiKt6TruthJetsTest",JetMoments_opts = {'moments':['nTrk', 'nTrk_allpv','JVF','WIDTH','trackWIDTH','trackWIDTH_allpv','ActiveArea','ActiveAreaPx','ActiveAreaPy','ActiveAreaPz','ActiveAreaE','VoronoiArea','VoronoiAreaPx','VoronoiAreaPy','VoronoiAreaPz','VoronoiAreaE']})
    


from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.AllowIgnoreConfigError = False


# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")
#topSequence.AntiKt5TruthJets.OutputLevel = 2
#topSequence.AntiKt4TopoJetsTer.OutputLevel = 2

## from JetTesters import JetTester, JetKinematics, JetMoments

## topSequence += JetTester( CollectionName = "AntiKt4LCTopoJets",
##                           JetTestTools = [JetKinematics(), JetMoments(moments = ["JVF", "nTrk"])]
##                           )


#svcMgr.MessageSvc.setDebug += ["JetCollectionCnv","JetMomentMapCnv"]

