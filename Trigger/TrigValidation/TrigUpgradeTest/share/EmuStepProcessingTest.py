###############################################################
#
# Job options file
#
# Based on AthExStoreGateExamples
#
#==============================================================

#--------------------------------------------------------------
# ATLAS default Application Configuration options
#--------------------------------------------------------------

# Configure the scheduler
from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.ShowControlFlow( True )
AlgScheduler.ShowDataFlow( True )

# add chain names in Menu/MenuChains.py

# 4 events

data = {'noreco': [';', ';', ';',';']}  # in the lists there are the events

data['emclusters'] = [ ';',
                    'eta:1,phi:1,et:180000; eta:1,phi:-1.2,et:35000;',
                    'eta:0.5,phi:0,et:120000; eta:1,phi:-1.2,et:65000;',
                    'eta:-0.6,phi:1.7,et:9000;']

data['msmu']  = [';',
                 ';',
                 'eta:-1.2,phi:0.7,pt:6500,pt2:8500; eta:-1.1,phi:0.6,pt:8500,pt2:8500;',
                 'eta:-1.7,phi:-0.2,pt:9500,pt2:8500;']

#data['ctp'] = [ 'HLT_e20 HLT_e5_e8 HLT_e5 HLT_e8 HLT_e5v22 HLT_g5',
data['ctp'] = [ 'HLT_e20 HLT_e5_e8 HLT_e5 HLT_e8 HLT_g5',
                'HLT_e20 HLT_e5_e8 HLT_e5 HLT_e8 HLT_g5 HLT_e5_v3',
                'HLT_mu8 HLT_mu8_1step HLT_e20 HLT_e8 HLT_mu8_e8 HLT_e3_e5',
                'HLT_mu20 HLT_mu8 HLT_mu8_1step HLT_2mu8 HLT_e8' ]


data['l1emroi'] = [ ';',
                    '1,1,0,EM3,EM7,EM15,EM20,EM50,EM100,2EM3; 1,-1.2,0,EM3,EM7,2EM3',
                    '-0.6,0.2,0,EM3,EM7,EM15,EM20,EM50,EM100; 1,-1.1,0,EM3,EM7,EM15,EM20,EM50',
                    '-0.6,1.5,0,EM3,EM7,EM7']

data['l1muroi'] = [';',
                   '0,0,0,MU0;',
                   '-1,0.5,0,MU6,MU8; -1,0.5,0,MU6,MU8,MU10',
                   '-1.5,-0.1,0,MU6,MU8']

data['tracks'] = ['eta:1,phi:1,pt:120000; eta:1,phi:-1.2,et:32000;',
                  'eta:1,phi:1,pt:120000; eta:1,phi:-1.2,et:32000;',
                  'eta:0.5,phi:0,pt:130000; eta:1,phi:-1.2,pt:60000;eta:-1.2,phi:0.7,pt:6700; eta:-1.1,phi:0.6,pt:8600;',
                  'eta:-0.6,phi:1.7,et:9000;'] # no MU track for MS candidate 'eta:-1.7,phi:-0.2,pt:9500;'

data['mucomb'] = [';',
                  ';',
                  'eta:-1.2,phi:0.7,pt:6600; eta:-1.1,phi:0.6,pt:8600;',
                  ';']

data['electrons'] = [';',
                     'eta:1,phi:1,pt:120000; eta:1,phi:-1.2,et:32000;',
                     ';',
                     ';']
data['photons'] = [';',
                   'eta:1,phi:1,pt:130000;',
                   ';',
                   ';']



from TrigUpgradeTest.TestUtils import writeEmulationFiles
writeEmulationFiles(data)


from AthenaCommon.CFElements import parOR, seqAND, stepSeq


# signatures
from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig import makeHLTTree
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, Chain, ChainStep


doMuon=True
doElectron=True
doCombo=True

HLTChains = []
EnabledElChains = []
EnabledMuChains = []
EnabledMuComboChains = []
EnabledElComboChains = []


# muon chains
if doMuon:
    from TrigUpgradeTest.HLTSignatureConfig import muStep1MenuSequence, muStep2MenuSequence
    muStep1 = muStep1MenuSequence("v1")
    muStep2 = muStep2MenuSequence("v1")


    MuChains  = [
        Chain(name='HLT_mu20', Seed="L1_MU10",      ChainSteps=[ChainStep("Step1_mu", [muStep1]) , ChainStep("Step2_mu", [muStep2] )]) ,
        Chain(name='HLT_mu8_1step', Seed="L1_MU6",   ChainSteps=[ChainStep("Step1_mu", [muStep1]) ]) ,
        Chain(name='HLT_mu8',  Seed="L1_MU6",       ChainSteps=[ChainStep("Step1_mu", [muStep1]) , ChainStep("Step2_mu",  [muStep2] ) ] )
        ]

    HLTChains += MuChains
    EnabledMuChains= [c.seed.strip().split("_")[1] +" : "+ c.name for c in MuChains]




## #electron chains
if doElectron:
    from TrigUpgradeTest.HLTSignatureConfig import elStep1MenuSequence, elStep2MenuSequence, gammStep1MenuSequence
    # electron
    elStep1 = elStep1MenuSequence("v1")
    elStep2 = elStep2MenuSequence("v1","v1")
    elStep2v2 = elStep2MenuSequence("v2","v2")
    elStep2v3 = elStep2MenuSequence("v2","v3")
    # gamma
    gammStep1 = gammStep1MenuSequence("v1")
    
    ElChains  = [
        Chain(name='HLT_e5'   , Seed="L1_EM7", ChainSteps=[ ChainStep("Step1_em",  [elStep1]), ChainStep("Step2_em",  [elStep2]) ] ),
        Chain(name='HLT_e5_v2', Seed="L1_EM7", ChainSteps=[ ChainStep("Step1_em",  [elStep1]), ChainStep("Step2v2_em",  [elStep2v2]) ] ),
        Chain(name='HLT_e5_v3', Seed="L1_EM7", ChainSteps=[ ChainStep("Step1_em",  [elStep1]), ChainStep("Step2v3_em",  [elStep2v3]) ] ),
        Chain(name='HLT_e8'   , Seed="L1_EM7", ChainSteps=[ ChainStep("Step1_em",  [elStep1]), ChainStep("Step2_em",  [elStep2]) ] ),
        Chain(name='HLT_g5'   , Seed="L1_EM7", ChainSteps=[ ChainStep("Step1_gam", [gammStep1]) ] )
        ]

    HLTChains += ElChains
    EnabledElChains= [c.seed.strip().split("_")[1] +" : "+ c.name for c in ElChains]


# combined chain
if doCombo:
    from TrigUpgradeTest.HLTSignatureConfig import elStep1MenuSequence, muStep1MenuSequence, elStep2MenuSequence, muStep2MenuSequence
    elStep1 = elStep1MenuSequence("v1")
    muStep1 = muStep1MenuSequence("v1")
    elStep2 = elStep2MenuSequence("v1","v1")
    muStep2 = muStep2MenuSequence("v1")

    
    CombChains =[
        Chain(name='HLT_mu8_e8',  Seed="L1_MU6_EM7", ChainSteps=[ ChainStep("Step1_mu_em",  [muStep1, elStep1]), ChainStep("Step2_mu_em",  [muStep2, elStep2])] ),
        Chain(name='HLT_e5_e8',   Seed="L1_2EM3",    ChainSteps=[ ChainStep("Step1_2em",[elStep1, elStep1]) ])
        ]

    HLTChains += CombChains
    for c in CombChains:
        seeds=c.seed.split("_")
        seeds.pop(0) #remove first L1 string
        for s in seeds:
            if "MU" in s: EnabledMuComboChains.append(s +" : "+ c.name)
            if "EM" in s: EnabledElComboChains.append(s +" : "+ c.name) 

    print "enabled Combo chains: ", EnabledMuComboChains,EnabledElComboChains


# this is a temporary hack to include new test chains
EnabledChainNamesToCTP = dict([ (c.name, c.seed)  for c in HLTChains])




########################## L1 #################################################

L1UnpackingSeq = parOR("L1UnpackingSeq")
from L1Decoder.L1DecoderConf import CTPUnpackingEmulationTool, RoIsUnpackingEmulationTool, L1Decoder
l1Decoder = L1Decoder( OutputLevel=DEBUG, RoIBResult="" )
l1Decoder.prescaler.EventInfo=""
l1Decoder.ChainToCTPMapping = EnabledChainNamesToCTP
l1Decoder.L1DecoderSummaryKey = "L1DecoderSummary"

ctpUnpacker = CTPUnpackingEmulationTool( OutputLevel =  DEBUG, ForceEnableAllChains=False , InputFilename="ctp.dat" )
l1Decoder.ctpUnpacker = ctpUnpacker

emUnpacker = RoIsUnpackingEmulationTool("EMRoIsUnpackingTool", OutputLevel=DEBUG, InputFilename="l1emroi.dat", OutputTrigRoIs="L1EMRoIs", Decisions="L1EM" )
emUnpacker.ThresholdToChainMapping = EnabledElChains + EnabledElComboChains
emUnpacker.Decisions="L1EM"
print "EMRoIsUnpackingTool enables chians:"
print emUnpacker.ThresholdToChainMapping

muUnpacker = RoIsUnpackingEmulationTool("MURoIsUnpackingTool", OutputLevel=DEBUG, InputFilename="l1muroi.dat",  OutputTrigRoIs="L1MURoIs", Decisions="L1MU" )
muUnpacker.ThresholdToChainMapping = EnabledMuChains + EnabledMuComboChains
muUnpacker.Decisions="L1MU"
print "MURoIsUnpackingTool enables chians:"
print muUnpacker.ThresholdToChainMapping

l1Decoder.roiUnpackers = [emUnpacker, muUnpacker]

#print l1Decoder
L1UnpackingSeq += l1Decoder
print L1UnpackingSeq

########################## L1 #################################################

# steps: sequential AND of 1=Filter 2=Processing
# chainstep=single chain step
# global step=joint for all chains
# filters: one SeqFilter per step, per chain
# inputMakers: one per each first RecoAlg in a step (so one per step), one input per chain that needs that step


from AthenaCommon.AlgSequence import AlgSequence, AthSequencer, dumpSequence
topSequence = AlgSequence()
#topSequence += L1UnpackingSeq
topSequence += l1Decoder
##### Make all HLT #######
makeHLTTree(HLTChains)
   

from AthenaCommon.AlgSequence import dumpMasterSequence
dumpMasterSequence()

theApp.EvtMax = 4

