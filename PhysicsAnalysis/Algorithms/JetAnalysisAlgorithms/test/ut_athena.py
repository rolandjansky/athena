import AthenaRootComps.ReadAthenaxAODHybrid

theApp.EvtMax = 500
testFile = os.getenv ('ASG_TEST_FILE_DATA')
svcMgr.EventSelector.InputCollections = [testFile]

algSeq = CfgMgr.AthSequencer("AthAlgSeq")

from AnaAlgorithm.DualUseConfig import setCfgMgr

setCfgMgr (CfgMgr)

# ideally we'd run over all of them, but we don't have a mechanism to
# configure per-sample right now
dataType = "data"
#dataType = "mc"
#dataType = "afii"
jetContainer = "AntiKt4EMTopoJets"

#turning this off, it doesn't seem to work on MacOS
runJvtUpdate = False
runJvtEfficiency = False

# have trouble with nested tools
runJetSmearing = False


from JetAnalysisAlgorithms.JetAnalysisSequence import makeJetAnalysisSequence

sequence = makeJetAnalysisSequence (jetContainer=jetContainer,dataType=dataType,
                                    runJvtUpdate=runJvtUpdate,runJvtEfficiency=runJvtEfficiency,
                                    runJetSmearing=runJetSmearing)


from AsgAnalysisAlgorithms.SequencePostConfiguration import sequencePostConfiguration

sequencePostConfiguration (sequence, jetContainer)

for alg in sequence :
    config = alg["alg"]

    # set everything to debug output
    config.OutputLevel = 1

    algSeq += config
    pass

# create our algorithm with teh given name
#alg = CfgMgr.MyxAODAnalysis()

# later on we'll add some configuration options for our algorithm that go here

#algSeq += alg

# optional include for reducing printout from athena
include("AthAnalysisBaseComps/SuppressLogging.py")
