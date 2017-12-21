# Configure the scheduler
from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.ShowControlFlow( False )
AlgScheduler.ShowDataFlow( False)

# Event-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
topSequence = AlgSequence()

# the job purpose is to illustrate the creation of the views from RoIs
# the input is fully emulated in an identical form as in the TrigUpgradeTest
data ={'rois':[
    '-0.6,0.2,1,EM3,EM7,EM15,EM20,EM50,EM100; 1,-1.1,1,EM3,EM7,EM15,EM20,EM50',
    ';', #no RoIs
    '-0.6,1.5,2,EM3,EM7',
    '-1,-1,3,EM3,EM7,EM15,EM20,EM50,EM100;'] }

# actually the HLT chains are here to always make the RoIs as in the first list, thus are not relevant
data['ctp'] = ['HLT_e8',  
               'HLT_e20 HLT_e8',
               'HLT_e8',
               'HLT_e20' ]

from TrigUpgradeTest.TestUtils import writeEmulationFiles
writeEmulationFiles(data)

from AthenaCommon.CFElements import seqAND, parOR, stepSeq
TopHLTSeq = seqAND("TopHLTSeq")
topSequence += TopHLTSeq

L1UnpackingSeq = parOR("L1UnpackingSeq")
from L1Decoder.L1DecoderConf import CTPUnpackingEmulationTool, RoIsUnpackingEmulationTool, L1Decoder
l1Decoder = L1Decoder( OutputLevel=DEBUG, RoIBResult="" )

ctpUnpacker = CTPUnpackingEmulationTool( OutputLevel =  DEBUG, ForceEnableAllChains=False , InputFilename="ctp.dat" )
#ctpUnpacker.CTPToChainMapping = [ "0:HLT_g100",  "1:HLT_e20", "2:HLT_mu20", "3:HLT_2mu8", "3:HLT_mu8", "33:HLT_2mu8", "15:HLT_mu8_e8" ]
l1Decoder.ctpUnpacker = ctpUnpacker

emUnpacker = RoIsUnpackingEmulationTool("EMRoIsUnpackingTool", OutputLevel=DEBUG, InputFilename="rois.dat", OutputTrigRoIs="L1EMRoIs", Decisions="L1EM" )
emUnpacker.ThresholdToChainMapping = ["EM7 : HLT_e8", "EM7 : HLT_mu8_e8", "EM20 : HLT_e20", "EM50 : HLT_2g50",   "EM100 : HLT_g100" ]

l1Decoder.roiUnpackers = [emUnpacker]
L1UnpackingSeq += l1Decoder
TopHLTSeq += L1UnpackingSeq

include( "TrigUpgradeTest/HLTCF.py" )


# definition of a hide corner for view algs
viewAlgsContainer = seqAND( "ViewAlgsContainer" )


steps = [ parOR("step0Filtering"), parOR("step1InViewReco") ]
steps[0] += seqFilter( "Step0EM", Inputs=["L1EM"], Outputs=["step0EM"],
                      Chains=[ x.split(':')[-1].strip() for x in emUnpacker.ThresholdToChainMapping ] ) # all chains


from ViewAlgsTest.ViewAlgsTestConf import SchedulerProxyAlg
viewAlg = SchedulerProxyAlg( "algInView", OutputLevel = DEBUG, RoIsContainer = "InViewRoI", OutputClusterContainer="ViewClusters")

viewAlgsContainer += viewAlg

algs=[ useExisting( "Step0EM" ) ]
from ViewAlgsTest.ViewAlgsTestConf import TestViewDriver
EMViewsMaker = TestViewDriver( "EMViewsMaker", OutputLevel = DEBUG, 
                               RoIsContainer = 'L1EMRoIs', RoIsViewOutput="InViewRoI", 
                               ClustersViewInput="ViewClusters", Views="EMClusterViews", 
                               ViewNodeName = viewAlgsContainer.name(), Scheduler = AlgScheduler.getScheduler() )
algs.append( EMViewsMaker )
algs.append( viewAlgsContainer )

if 'doMerging' in dir() and  doMerging == True:
    from ViewAlgsTest.ViewAlgsTestConf import TestViewMerger    
    EMViewsMerger = TestViewMerger("EMViewsMerger", OutputLevel = DEBUG, 
                                   Views = "EMClusterViews", ClustersViewInput = "ViewClusters", 
                                   ClustersOutput = "EMClusters")
    algs.append( EMViewsMerger )


steps[1] += seqAND("emViewReco", algs )

TopHLTSeq += addSteps( steps )

theApp.EvtMax = 4
