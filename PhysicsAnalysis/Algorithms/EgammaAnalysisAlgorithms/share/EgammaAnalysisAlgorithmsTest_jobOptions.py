# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack


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
electronContainer = "Electrons"
photonContainer = "Photons"



from AsgAnalysisAlgorithms.PileupAnalysisSequence import makePileupAnalysisSequence

sequence = makePileupAnalysisSequence (dataType=dataType)


from AsgAnalysisAlgorithms.SequencePostConfiguration import sequencePostConfiguration

sequencePostConfiguration (sequence, "EventInfo")

for alg in sequence :
    config = alg["alg"]

    # set everything to debug output
    config.OutputLevel = 1

    algSeq += config
    pass


from EgammaAnalysisAlgorithms.ElectronAnalysisSequence import makeElectronAnalysisSequence

sequence = makeElectronAnalysisSequence (electronContainer=electronContainer,dataType=dataType)


sequencePostConfiguration (sequence, electronContainer)

for alg in sequence :
    config = alg["alg"]

    # set everything to debug output
    config.OutputLevel = 1

    algSeq += config
    pass


from EgammaAnalysisAlgorithms.PhotonAnalysisSequence import makePhotonAnalysisSequence

sequence = makePhotonAnalysisSequence (photonContainer=photonContainer,dataType=dataType)


sequencePostConfiguration (sequence, photonContainer)

for alg in sequence :
    config = alg["alg"]

    # set everything to debug output
    config.OutputLevel = 1

    algSeq += config
    pass


# optional include for reducing printout from athena
include("AthAnalysisBaseComps/SuppressLogging.py")
