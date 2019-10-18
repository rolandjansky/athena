# signatures
from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig import makeHLTTree
from L1Decoder.L1DecoderConf import CTPUnpackingEmulationTool, RoIsUnpackingEmulationTool, L1Decoder
from AthenaCommon.AlgScheduler import AlgScheduler
from AthenaCommon.CFElements import parOR
from AthenaCommon.Logging import logging
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import ChainStep
from TrigUpgradeTest.TestUtils import writeEmulationFiles, makeChain

log = logging.getLogger('EmuScalingCFTest')



def generateInputData(nevents, chain_names):

    data = {'emclusters': [], 'l1emroi': [], 'ctp':[]}  # in the lists there are the events
    passed = ' '.join(chain_names)
    all_passed = []
 
    for ievent in range(nevents):
        data['emclusters'].append('eta:1,phi:1,et:180000; eta:1,phi:-1.2,et:35000;')
        data['l1emroi'].append('1,1,0,EM3,EM7,EM15,EM20,EM50,EM100,2EM3; 1,-1.2,0,EM3,EM7,2EM3')
        all_passed.append(passed)

        #    data['ctp'] = [ 'HLT_e20 HLT_e5_e8 HLT_e5 HLT_e8 HLT_g5',
    data['ctp'] =all_passed

    return data




def generateChains(chain_names):

    from TrigUpgradeTest.HLTSignatureConfig import  genMenuSequence

    nsteps=2
    seed = "EM7"
    chains = []

    for chain in chain_names:
        chainSteps=[]
        for step in range(nsteps):
            # same reco, diff hypo
            seq_sameReco = genMenuSequence(step="SSameR"+str(step),
                                               reconame="v"+str(step),
                                               hyponame="SameR"+str(step)+chain)
            # diff reco, diff hypo
            seq_diffReco = genMenuSequence(step="SDiffR"+str(step),
                                               reconame="v"+str(step)+chain,
                                               hyponame="DiffR"+str(step)+chain) # not really interesting
            # diff reco, same hypo
            ## seq_sameHypo = genMenuSequence(step="SSameH"+str(step),
            ##                                 reconame="v"+str(step)+chain,
            ##                                 hyponame="SameH"+str(step))
            if len(chains)< len(chain_names)/2.:
                seq= seq_sameReco
            else:
                seq= seq_diffReco


            step = ChainStep(seq.name, [seq])
            chainSteps.append(step)
        chainObj=makeChain(name=chain ,L1Thresholds=[seed], ChainSteps=chainSteps )
        log.debug("adding chain %s",chainObj)
        chains.append(chainObj)
           # Chain(name='HLT_e5'   , L1Item="L1_EM7", ChainSteps=[ ChainStep("Step_em11", [el11]), ChainStep("Step_em21",  [el21]) ] ),

           
    log.debug("Produced menu with %d chains",len(chains))
    return chains



def process():
    nevents=2
    nchains=10


    log.debug("Running on %d events, with %d chains",nevents,nchains)

    chain_names=[]
    for chain in range(nchains):
        chain_names+=["HLT_TestChain"+str(chain+1)+"_L1EM7"]



    data=generateInputData(nevents, chain_names)
    HLTChains=generateChains(chain_names)
    
    

    writeEmulationFiles(data)


    ########################## L1 #################################################

    L1UnpackingSeq = parOR("L1UnpackingSeq")

    l1Decoder = L1Decoder( RoIBResult="" )
    l1Decoder.prescaler.EventInfo=""
    l1Decoder.L1DecoderSummaryKey = "L1DecoderSummary"

    ctpUnpacker = CTPUnpackingEmulationTool( ForceEnableAllChains=False , InputFilename="ctp.dat" )
    l1Decoder.ctpUnpacker = ctpUnpacker

    emUnpacker = RoIsUnpackingEmulationTool("EMRoIsUnpackingTool", InputFilename="l1emroi.dat", OutputTrigRoIs="L1EMRoIs", Decisions="L1EM", ThresholdPrefix="EM" )
    from TrigUpgradeTest.EmuStepProcessingConfig import thresholdToChains
    emUnpacker.Decisions="L1EM"

  
    l1Decoder.roiUnpackers = [emUnpacker]

    #print l1Decoder
    L1UnpackingSeq += l1Decoder
    log.debug(L1UnpackingSeq)


    from AthenaCommon.AlgSequence import AlgSequence, AthSequencer, dumpSequence
    topSequence = AlgSequence()
    topSequence += l1Decoder
    ##### Make all HLT #######
    from TriggerMenuMT.HLTMenuConfig.Menu.TriggerConfigHLT import TriggerConfigHLT
    makeHLTTree( triggerConfigHLT=TriggerConfigHLT )
   
    print "EmuScalingCF: dump top Sequence after CF/DF Tree build"
    from AthenaCommon.AlgSequence import dumpMasterSequence, dumpSequence
    dumpSequence( topSequence )
    #dumpMasterSequence()
    
    theApp.EvtMax = nevents

    from TriggerMenuMT.HLTMenuConfig.Menu.HLTMenuJSON import generateJSON
    generateJSON()

    from TrigConfigSvc.TrigConfigSvcCfg import getHLTConfigSvc
    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr += getHLTConfigSvc()


process()
