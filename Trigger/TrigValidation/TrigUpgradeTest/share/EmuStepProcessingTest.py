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


# from SGComps.SGCompsConf import SGInputLoader
# topSequence += SGInputLoader( OutputLevel=INFO, ShowEventDump=False )
# topSequence.SGInputLoader.Load = [ ('ROIB::RoIBResult','RoIBResult') ]

#from AthenaCommon.CFElements import stepSeq


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

data['tracks'] = ['eta:1,phi:1,pt:120000; eta:1,phi:-1.2,et:32000;',
                  'eta:0.5,phi:0,pt:130000; eta:1,phi:-1.2,pt:60000;eta:-1.2,phi:0.7,pt:6700; eta:-1.1,phi:0.6,pt:8600;',
                  'eta:-0.6,phi:1.7,et:9000;'] # no MU track for MS candidate 'eta:-1.7,phi:-0.2,pt:9500;'

data['mucomb'] = [';',
                  'eta:-1.2,phi:0.7,pt:6600; eta:-1.1,phi:0.6,pt:8600;',
                  ';']

data['electrons'] = ['eta:1,phi:1,pt:120000; eta:1,phi:-1.2,et:32000;',
                     ';',
                     ';']
data['photons'] = ['eta:1,phi:1,pt:130000;',
                   ';',
                   ';']


from TrigUpgradeTest.TestUtils import writeEmulationFiles
writeEmulationFiles(data)

include("TrigUpgradeTest/L1CF.py")
include("TrigUpgradeTest/HLTCF.py")

# Cati's menu code
#print "=============== MEOW ================"
#include("TrigUpgradeTest/TriggerMenuMT.py")
#genMenu = GenerateMenu()
#genMenu.generate()
#print "=============== WOOF ================"


# steps: sequential AND of 1=Filter 2=Processing
# chainstep=single chain step
# global step=joint for all chains
# filters: one SeqFilter per step, per chain
# inputMakers: one per each first RecoAlg in a step (so one per step), one input per chain that needs that step


from TrigUpgradeTest.HLTCFConfig import *
from TrigUpgradeTest.MenuComponents import *


# muon chains

include("TrigUpgradeTest/HLTSignatureConfig.py")

muStep1 = muStep1Sequence()
MuChains  = [
    Chain(name='HLT_mu20', Seed="L1_MU10",   ChainSteps=[ChainStep("Step1_mu20", [SequenceThreshold(muStep1,"mu20")])]),
    Chain(name='HLT_mu8',  Seed="L1_MU6",    ChainSteps=[ChainStep("Step1_mu8",  [SequenceThreshold(muStep1,"mu8")] )])
    ]


#electron chains
elStep1 = elStep1Sequence()
ElChains  = [
    Chain(name='HLT_e20' , Seed="L1_EM10", ChainSteps=[ChainStep("Step1_e20",  [SequenceThreshold(elStep1,"e20")])])
    ]


# combined chain
muelStep1 = combStep1Sequence()
CombChains =[
    Chain(name='HLT_mu8_e8' , Seed="L1_EM6_MU6", ChainSteps=[ChainStep("Step1_mu8_e8",  [SequenceThreshold(muelStep1,"mu8_e8")])])
    ]


group_of_chains = MuChains + ElChains + CombChains


#decide the number of steps, starting from 1
NSTEPS=1


#loop over chains to configure
count_steps=0
for nstep in range(0, NSTEPS):
    stepCF_name =  "Step%i"%(nstep+1)
    CFseq_list = []

    for chain in group_of_chains:
        chain_step_name= "%s:%s"%(stepCF_name, chain.name)
        print "\n*******Filling step %s for chain  %s"%(stepCF_name, chain.name)
      
        chain_step=chain.steps[count_steps]
        sequenceThresholds=chain_step.sequenceThresholds
        for st in sequenceThresholds:
            sequence=st.sequence
            threshold=st.threshold
            cfseq_name= sequence.name
            hypotool=threshold

            print "Going through sequence %s with threshold %s"%(sequence.name, threshold)
            #define sequence input
            if count_steps == 0: # seeding
                print chain.group_seed
                previous_sequence="".join(chain.group_seed)
                sequence_input = chain.group_seed
            else:
                # from previous step
                previous_sequence=chain.steps[count_steps-1].sequence
                sequence_input=previous_sequence.outputs

            # add hypotools
            for nodeSeq in sequence.nodeSeqList:
                hypoAlg= nodeSeq.hypo                
                print "Adding %s to %s"%(threshold,hypoAlg.name)
                hypoAlg.addHypoTool(hypotool)


            #### FILTER
            # one filter per previous sequence at the start of the sequence: check if it exists or create a new one        
            # if the previous hypo has more than one output, try to get all of them
            # one filter per previous sequence: 1 input/previous seq, 1 output/next seq 
            filter_name="Filter_%s%s"%(stepCF_name,previous_sequence)
            filter_out=["%s_%s_out"%(filter_name,i) for i in sequence_input]
            filter_already_exists=False
            findFilter= [cfseq.filter for cfseq in CFseq_list if filter_name in cfseq.filter.name]        
            if len(findFilter):
                print "Filter %s already exists"%(filter_name)
                filter_already_exists=True
                sfilter=findFilter[0]
                print "Adding chain %s to %s"%(chain.name,sfilter.name)
                sfilter.setChains(chain.name)
                continue
            else:
                sfilter = SequenceFilterNode(name=filter_name)
                for o in filter_out: sfilter.setOutput(o)            
                for i in sequence_input: sfilter.setInput(i)
                sfilter.setChains(chain.name)
                print "Filter Done: %s"%(sfilter)
                    
            #loop over NodeSequences of this sequence to add inputs to InputMaker
            for nodeSeq in sequence.nodeSeqList:
                seed=nodeSeq.seed            
                #TMP: now run InputMaker with sequence input, until InputMaker has inplicit DH
                input_maker_name = nodeSeq.inputMaker.name
                input_maker_input= (inp for inp in sequence_input if seed in inp)
                for i in input_maker_input: nodeSeq.inputMaker.setInput(i)                    
            #end of reco sequence loop
                    
            #make the decision step                        
            CF_seq = CFSeq( cfseq_name, FilterAlg=sfilter, MenuSequence=sequence) 
            print CF_seq
            CFseq_list.append(CF_seq)
           
        #end of sequence/threshold

    #end of this step configuration, now implement CF:
 
    print "\n******** instantiate algorithms in CF"
    stepCF = create_StepCF(stepCF_name, CFseq_list, dump=1)
    stepCF_DataFlow_to_dot(stepCF_name, CFseq_list)
    stepCF_ControlFlow_to_dot(stepCF)


    print "Add stepCF %s to the root"%stepCF_name
    TopHLTSeq += addSteps(stepCF)
    count_steps+=1
    print "************* End of step %s"%stepCF_name
    # end of steps

    #from AthenaCommon.AlgSequence import dumpMasterSequence
    #dumpMasterSequence()

theApp.EvtMax = 3





#  is this needed??
#step +=seqFilter
