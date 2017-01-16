
# check pseudo inputs
import os
assert os.path.exists("roiData.dat"), 'Input file with RoI positions is missing'
assert os.path.exists("menuData.dat"), 'Input file wiht CTP decisions is missing'



from ViewAlgsTest.connectAlgorithmsIO import connectAlgorithmsIO
from ViewAlgsTest.Menu import *

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


from L1Decoder.L1DecoderConf import FakeCTP
fakeCTP = FakeCTP()
fakeCTP.OutputLevel=VERBOSE
fakeCTP.InputFilename="menuData.dat"
fakeCTP.OutputLevel=VERBOSE
topSequence += fakeCTP

from ViewAlgs.ViewAlgsConf import PrescaleDecision
ps = PrescaleDecision("HLTChainsPrescaling")
connectAlgorithmsIO(consumer=(ps, "InputChainDecisions"), producer=(fakeCTP, "OutputDecisions"))
#ps.InputChainDecisions = getOutput(fakeCTP, "OutputDecisions")
setOutput(ps, "OutputChainDecisions", "ChainsPassingAfterPS")
ps.OutputLevel=DEBUG
topSequence += ps


#Run Fake RoI
from L1Decoder.L1DecoderConf import FakeRoI
fakeRoI = FakeRoI("fakeRoI")
fakeRoI.InputFilename="roiData.dat"
#fakeRoI.OutputRoIs="OutputRoIs"
fakeRoI.OutputRoIs="L1RoIs"
setOutput(fakeRoI, "OutputDecisions", "CaloRoIs")
fakeRoI.OutputLevel=DEBUG
topSequence += fakeRoI

# here merge RoIs
from ViewAlgsTest.ViewAlgsTestConf import MergeRoIsAlg
mergeRoIs = MergeRoIsAlg("mergeRoIs")
mergeRoIs.OutputLevel=DEBUG
mergeRoIs.UseSuperRoI=True
connectAlgorithmsIO(consumer=(mergeRoIs, 'InputRoIsContainer'), producer=(fakeRoI, 'OutputRoIs'))
setOutput(mergeRoIs, 'OutputRoIsContainer', 'OutputRoIs')
topSequence += mergeRoIs





theApp.EvtMax = 3
MessageSvc.debugLimit=1000000

