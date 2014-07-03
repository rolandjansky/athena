# Configure athena common flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags

athenaCommonFlags.EvtMax = 1
athenaCommonFlags.SkipEvents = 0
#athenaCommonFlags.OutputLevel = 2
athenaCommonFlags.AllowIgnoreConfigError = False

globalflags.ConditionsTag.set_Value_and_Lock('COMCOND-BLKPA-RUN1-01')
#globalflags.ConditionsTag.set_Value_and_Lock('OFLCOND-MC12-SDR-06')
globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-20-00-01')

#athenaCommonFlags.FilesInput = ['/afs/cern.ch/atlas/groups/JetEtmiss/ReferenceFiles/RTT/DATA/RAW/data12_8TeV.CurrentRef.RAW']




# Configure execution flags
from RecExConfig.RecFlags import rec

#rec.readESD=False
#rec.readAOD=False
#rec.readRDO=False
#rec.doWriteESD=True
#rec.doWriteAOD=False
#rec.doAOD=False

#from ParticleBuilderOptions.AODFlags import AODFlags
#AODFlags.FastSimulation = False

rec.UserExecs = ["myjets()"]


from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
primDPD.WritePhysDPDJetMET.JetCollectionNames = []

from JetRec.JetAlgConfiguration import checkAndUpdateOptions



# Function to run within athena
def myjets():
    from JetRec.JetRecFlags import jetFlags
    from JetRec.JetGetters import make_StandardJetGetter
    
    # Algorithms to test: AntiKt R = {0.4, 0.6} scheme = {EM, LC}
    #jetAlg4EM  = make_StandardJetGetter('AntiKt',0.4,'Topo',calibName='EM:OFFSET_ORIGIN_ETAJES',outputCollectionName='AntiKt4TopoJetsTest').jetAlgorithmHandle()
    jetAlg4EM  = make_StandardJetGetter('AntiKt',0.4,'Topo',doCalib=False,minPt=5000,outputCollectionName='AntiKt4TopoEMJetsTest').jetAlgorithmHandle()
    jetAlg4LC  = make_StandardJetGetter('AntiKt',0.4,'LCTopo',outputCollectionName='AntiKt4LCTopoJetsTest').jetAlgorithmHandle()
    
    #jetAlg6EM  = make_StandardJetGetter('AntiKt',0.6,'Topo',calibName='EM:OFFSET_ORIGIN_ETAJES',outputCollectionName='AntiKt6TopoJetsTest').jetAlgorithmHandle()
    jetAlg6EM  = make_StandardJetGetter('AntiKt',0.6,'Topo',doCalib=False,minPt=5000,outputCollectionName='AntiKt6TopoEMJetsTest').jetAlgorithmHandle()
    jetAlg6LC  = make_StandardJetGetter('AntiKt',0.6,'LCTopo',outputCollectionName='AntiKt6LCTopoJetsTest').jetAlgorithmHandle()
   
    # Prepare to dump relevant jet collections and associated moments
    from JetValidation.JetTestConfig import scheduleJetTester
    moments = {'moments':['nTrk', 'nTrk_allpv','JVF','WIDTH','trackWIDTH','trackWIDTH_allpv','ActiveArea','ActiveAreaPx','ActiveAreaPy','ActiveAreaPz','ActiveAreaE','VoronoiArea','VoronoiAreaPx','VoronoiAreaPy','VoronoiAreaPz','VoronoiAreaE']}
    
    # Schedule the jet testers for the existing collections
    scheduleJetTester("CaloCalTopoCluster",JetMoments=False, CollectionType="CaloClusterContainer", JetSigState=False, JetAuthor=False, JetNConstituents=False, JetConstituents=False)
    scheduleJetTester("AntiKt4TopoEMJets",JetMoments_opts = moments)
    scheduleJetTester("AntiKt4LCTopoJets",JetMoments_opts = moments)
    scheduleJetTester("AntiKt6TopoEMJets",JetMoments_opts = moments)
    scheduleJetTester("AntiKt6LCTopoJets",JetMoments_opts = moments)

    # Schedule the jet testers for the recalculated collections
    scheduleJetTester("AntiKt4TopoEMJetsTest",JetMoments_opts = moments)
    scheduleJetTester("AntiKt4LCTopoJetsTest",JetMoments_opts = moments)
    scheduleJetTester("AntiKt6TopoEMJetsTest",JetMoments_opts = moments)
    scheduleJetTester("AntiKt6LCTopoJetsTest",JetMoments_opts = moments)
    



# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

