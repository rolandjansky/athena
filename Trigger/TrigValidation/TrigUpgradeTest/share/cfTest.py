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
from GaudiHive.GaudiHiveConf import ForwardSchedulerSvc
svcMgr += ForwardSchedulerSvc()
svcMgr.ForwardSchedulerSvc.ShowDataFlow=True
svcMgr.ForwardSchedulerSvc.ShowControlFlow=True

# include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
# svcMgr.ByteStreamInputSvc.FullFileName = [ "./input.data" ]

# # This is the list of proxies to set up so that retrieval attempt will trigger the BS conversion
# if not hasattr( svcMgr, "ByteStreamAddressProviderSvc" ):
#     from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc
#     svcMgr += ByteStreamAddressProviderSvc()
# svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "ROIB::RoIBResult/RoIBResult" ]

# Event-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
topSequence = AlgSequence()


# from SGComps.SGCompsConf import SGInputLoader
# topSequence += SGInputLoader( OutputLevel=INFO, ShowEventDump=False )
# topSequence.SGInputLoader.Load = [ ('ROIB::RoIBResult','RoIBResult') ]




data = {'noreco': [';', ';', ';']}  # in the lists there are the events
data['emclusters'] = ['eta:1,phi:1,et:180000; eta:1,phi:-1.2,et:35000;',
                      'eta:0.5,phi:0,et:120000; eta:1,phi:-1.2,et:65000;',
                      'eta:-0.6,phi:1.7,et:9000;']
data['msmu']  = [';',
                 'eta:-1.2,phi:0.7,pt:6500; eta:-1.1,phi:0.6,pt:8500;',
                 'eta:-1.7,phi:-0.2,pt:9500;']

data['ctp'] = [ 'HLT_g100',  'HLT_2g50 HLT_e20', 'HLT_mu20 HLT_mu8 HLT_2mu8 HLT_mu8_e8' ]

data['l1emroi'] = ['1,1,0,EM3,EM7,EM15,EM20,EM50,EM100; 1,-1.2,0,EM3,EM7',
                   '-0.6,0.2,0,EM3,EM7,EM15,EM20,EM50,EM100; 1,-1.1,0,EM3,EM7,EM15,EM20,EM50',
                   '-0.6,1.5,0,EM3,EM7,EM7']

data['l1muroi'] = ['0,0,0,MU0;',
                   '-1,0.5,0,MU6,MU8; -1,0.5,0,MU6,MU8,MU10',
                   '-1.5,-0.1,0,MU6,MU8']




for name, d in data.iteritems():
    with open(name+".dat", "w") as f:
        for event in d:
            f.write(event)
            f.write("\n")
        




def parOR(name, subs=[]):
    seq = AthSequencer( name )
    seq.ModeOR = True
    seq.Sequential = False
    seq.StopOverride = True
    for s in subs:
        seq += s
    return seq
    
def seqAND(name, subs=[]):
    seq = AthSequencer( name )
    seq.ModeOR = False
    seq.Sequential = True
    seq.StopOverride = False
    for s in subs:
        seq += s
    return seq

def stepSeq(name, filterAlg, rest):
    stepReco = parOR(name+"_reco", rest)
    stepAnd = seqAND(name, [ filterAlg, stepReco ])
    return stepAnd



TopHLTSeq = seqAND("TopHLTSeq")
topSequence += TopHLTSeq

L1UnpackingSeq = parOR("L1UnpackingSeq")
from L1Decoder.L1DecoderConf import CTPUnpackingEmulationTool, RoIsUnpackingEmulationTool, L1Decoder
l1Decoder = L1Decoder( OutputLevel=DEBUG, RoIBResult="" )

ctpUnpacker = CTPUnpackingEmulationTool( OutputLevel =  DEBUG, ForceEnableAllChains=False , InputFilename="ctp.dat" )
ctpUnpacker.CTPToChainMapping = [ "0:HLT_g100",  "1:HLT_e20", "2:HLT_mu20", "3:HLT_2mu8", "3:HLT_mu8", "33:HLT_2mu8", "15:HLT_mu8_e8" ]
l1Decoder.ctpUnpacker = ctpUnpacker

emUnpacker = RoIsUnpackingEmulationTool("EMRoIsUnpackingTool", OutputLevel=DEBUG, InputFilename="l1emroi.dat", OutputTrigRoIs="L1EMRoIs", Decisions="L1EMDecisions" )
emUnpacker.ThresholdToChainMapping = ["EM7 : HLT_mu8_e8", "EM20 : HLT_e20", "EM50 : HLT_2g50",   "EM100 : HLT_g100" ]

muUnpacker = RoIsUnpackingEmulationTool("MURoIsUnpackingTool", OutputLevel=DEBUG, InputFilename="l1muroi.dat",  OutputTrigRoIs="L1MURoIs", Decisions="L1MUDecisions" )
muUnpacker.ThresholdToChainMapping = ["MU8 : HLT_mu8", "MU8 : HLT_2mu8", "MU10 : HLT_mu20",   "EM100 : HLT_g100" ]

l1Decoder.roiUnpackers = [emUnpacker, muUnpacker]

L1UnpackingSeq += l1Decoder

TopHLTSeq += L1UnpackingSeq



steps = [ parOR("step%i" % i) for i in range(5)]
HLTChainsSeq  = seqAND("HLTChainsSeq", steps)
TopHLTSeq += HLTChainsSeq # in principle we do not need the HLTChainsSeq 



from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestRecoAlg
def reco(name, Output, FileName="noreco.dat"):
    return HLTTest__TestRecoAlg("R_"+name, OutputLevel = DEBUG, FileName=FileName, Output=Output)

from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestHypoAlg
def hypo(name, Input, Output):
    return HLTTest__TestHypoAlg("H_"+name, OutputLevel = DEBUG, Input=Input, Output=Output)

from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestRoRSeqFilter
allFilters={}
def seqFilter(name, Inputs=[], Outputs=[]):
    global allFilters
    f = HLTTest__TestRoRSeqFilter("F_"+name, OutputLevel = DEBUG, Inputs=Inputs, Outputs=Outputs)
    if "Step1" in name: # so that we see events running through, will be gone once L1 emulation is included
        f.AlwaysPass = True        
    allFilters[name] = f
    return f

def pick(name):
    global allFilters
    return allFilters[name]

from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestHypoTool
def emHTool(name):
    v = int(name[5:])*1000
    return HLTTest__TestHypoTool(name, OutputLevel=DEBUG, Threshold=v, Property="et")

stepNo = 0
steps[stepNo] += seqFilter( "Step1MU", Inputs=["L1MU"], Outputs=["step0MU"])
steps[stepNo] += seqFilter( "Step1MU_E", Inputs=["L1MU", "L1EM"], Outputs=["step0MU","step0EM"])
steps[stepNo] += seqFilter( "Step1EM", Inputs=["L1EM"], Outputs=["step0EM"])

emHypo = hypo("Step1ElGamHypo", Input="EMClusters", Output="EMDecisions")
emHypoTools = [ emHTool("HLT_e2"), emHTool("HLT_e3"), emHTool("HLT_e5"),
                emHTool("HLT_g5"), emHTool("HLT_g7"), emHTool("HLT_g15") ]
emHypo.HypoTools = emHypoTools 
emHypo += emHypoTools

def msMuHTool(name):
    v = int(name[6:])*1000
    return HLTTest__TestHypoTool(name, OutputLevel=DEBUG, Threshold=v, Property="pt")

msMuHypo =  hypo("Step1MuHypo", Input="MSMuons", Output="MSMUonDecisions")
msMuHypoTools = [ msMuHTool("HLT_mu6"), msMuHTool("HLT_mu8"),  msMuHTool("HLT_mu10")  ]
msMuHypo.HypoTools = msMuHypoTools
msMuHypo += msMuHypoTools


stepNo += 1 
em1 = stepSeq( "em1", pick("Step1EM"), [ reco("EMRoIs", Output="EMRoIs"),
                                         reco("CaloClustering", FileName="emclusters.dat", Output="EMClusters"),
                                         emHypo ] )
steps[stepNo] += em1
mu1 = stepSeq("mu1", pick("Step1MU"), [ reco("MURoIs", Output="MURoIs"),
                                        reco("muMSRecAlg", FileName="msmu.dat", Output="MSMuons"),
                                        msMuHypo ] )
steps[stepNo] += mu1

# mue1 = stepSeq("mue1", pick("Step1MU_E"), [ pick("CaloClustering"), reco("muMSRecAlg", "msmu.dat"), hypo("Step1MuEHypo") ])
# steps[stepNo] += mue1


# stepNo += 1
# steps[stepNo] += seqFilter("Step2MU", Inputs=["step1MUDecisions"], Outputs=["step2MU"] )
# steps[stepNo] += seqFilter("Step2MU_E", Inputs=["step1MUEDecisions"], Outputs=["step2MUE"] )
# steps[stepNo] += seqFilter("Step2E", Inputs=["step1EDecisions"], Outputs=["step2E"] )
# steps[stepNo] += seqFilter("Step2G", Inputs=["step1GDecisions"], Outputs=["step2G"])

# stepNo += 1
# mu2 = stepSeq("mu2", pick("Step2MU"), [ reco("TrkRoIs", "noreco.dat"), reco("TrigFastTrackFinder", "tracks.dat"),  reco("MuonRecAlg", "mucomb.dat"), hypo("Step2MuHypo") ] )
# steps[stepNo] += mu2

# mue2 = stepSeq("mue2", pick("Step2MU_E"), [ pick("TrkRoIs", "noreco.dat"), pick("TrigFastTrackFinder"), reco("MuonRecAlg", "mucomb.dat"),  reco("ElectronRecAlg"), hypo("Step2MuEHypo") ])
# steps[stepNo] += mue2

# e2 = stepSeq( "e2" , pick("Step2E"), [ pick("TrkRoIs"), pick("TrigFastTrackFinder"), reco("ElectronRecAlg", "electrons.dat"),hypo("Step2EHypo") ] )
# steps[stepNo] += e2

# g2 = stepSeq("g2", pick("Step2G"), [ reco("GRoIs", "noreco.dat"), reco("PhotonRecAlg", "photons.dat"), hypo("Step2PhotonHypo") ])
# steps[stepNo] += g2
# stepNo += 1

theApp.EvtMax = 3




