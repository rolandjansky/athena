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

from AthenaCommon.CFElements import stepSeq


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

muStep1 = Sequence("Step1MuHypo", Algs=["muMSRecAlg:msmu.dat"], Hypo="Step1MuHypo")
MuChains  = [ Chain(name='HLT_mu20', Seed="L1MU10",   ChainSteps=[ChainStep("Step1Mu20", Sequence=muStep1, Threshold="pt20")]),
            Chain(name='HLT_mu8',  Seed="L1MU6",    ChainSteps=[ChainStep("Step1Mu8",  Sequence=muStep1, Threshold="pt8")]) ]
                                                                                                                            

elStep1 = Sequence("Step1ElGamHypo", Algs=["CaloClustering:emclusters.dat"], Hypo="Step1ElGamHypo")
ElChains  = [ Chain(name='HLT_e20' , Seed="L1EM10", ChainSteps=[ChainStep("Step1E20",  Sequence=elStep1, Threshold="et20")])  ]


#CombChains =[ Chain(name='HLT_mu8_e8' , Seed="L1EM6MU6", ChainSteps=[ChainStep("Step1E6Mu6",  Sequence=elStep1, Threshold="et20")]) ]



group_of_chains = MuChains + ElChains  


#decide the number of steps, starting from 1
NSTEPS=1
group_of_steps = { "Step%i"%(i+1) :  parOR("Step%i" %(i+1)) for i in range(NSTEPS)}


#loop over chains to configure
count_steps=0
for stepCF_name, stepCF in group_of_steps.iteritems():
    step_list = []

    for chain in group_of_chains:
        chain_step_name= "%s:%s"%(stepCF_name, chain.name)
        print "\n*******Filling stepCF %s for chain  %s"%(stepCF_name, chain.name)
      
        chain_step=chain.steps[count_steps]
        sequence=chain_step.sequence
        step_name= sequence.name

        #define sequence input
        if count_steps == 0: # seeding
            previous_sequence=chain.group_seed
            sequence_input = chain.group_seed
        else:
            # from previous step
            previous_sequence=chain.steps[count_steps-1].sequence
            sequence_input=previous_sequence.outputs

        # one filter per previous sequence at the start of the sequence: check if it exists or create a new one        
        # if the hypo has more than one output, try to get all of them
        filter_name="Filter%s%s"%(stepCF_name,previous_sequence)
        filter_out="%s_out"%filter_name
        filter_already_exists=False
        findFilter= [step.filter for step in step_list if step.filter.name in filter_name]
        
        if len(findFilter):
            print "Filter %s already exists"%(filter_name)
            filter_already_exists=True
            sfilter=findFilter[0]

        else:
            sfilter = SequenceFilter( filter_name, Inputs=[], Outputs=[], Chains=[] )
            sfilter.addOutput(filter_out)            


        sfilter.addChain(chain.name)
        sfilter.addInput(sequence_input)
        print "Filter Done: %s"%(sfilter)        

        #was this sequence already used in other steps? Find if the step exists
        step_already_exists=False        
        findStep= [step for step in step_list if step.name in step_name]
        #findStep= [step.name for seq in seq_list if seq.name in sequence.name]
        if len(findStep):
            step_already_exists=True
        
       
        #make the step
        if  step_already_exists:
            print "  Step %s already exists"%(step_name)
            step=findStep[0]
            #get the hypo
            hypoAlg= step.hypo
            if not hypoAlg:
                print "Error %s not found in existing sequence %s"%(sequence.hypo,sequence.name)
            else:
                print "Found HypoAlg %s"%(hypoAlg.name)
        else:
            print "  Making new step %s from sequence %s"%(step_name,sequence.name)
            # first create the InputMaker
            # imaker_out="IMaker%s_out"%chain_step_name
            # last_output = imaker_out
            #fill the step sequence with algorithms
            algs=sequence.algs_name
            last_output = sequence_input #filter_out #tmp, no InputMaker
            list_of_algs = []
            for alg in algs:
                name_file=alg.split(":")
                input=last_output
                input_list = []
                input_list.append(input)
                output="R%s_out"%(name_file[0])
                output_list = []
                output_list.append(output)

                reco_alg = RecoAlg(name_file[0], Output=output_list,  FileName=name_file[1], Input=input_list)  
                print "   added %s for this sequence %s"%(reco_alg,sequence.name)
                list_of_algs.append(reco_alg)
                last_output= output

            #make the hypo
            hypo=sequence.hypo
            input_list = []
            input_list.append(last_output)            
            sequence_out = "Sequence%s_out"%sequence
            output_list = []
            output_list.append(sequence_out)
            hypoAlg = HypoAlg(hypo, Input=input_list, Output=output_list)
            print "Create HypoAlg %s, and create the Decision Step -> %s"%(hypo,step_name)
            
            #make the decision step                        
            step_seq = Step( step_name, FilterAlg=sfilter, RecoAlgs=list_of_algs, Hypo=hypoAlg)
            step_list.append(step_seq)
           

        # fill hypoTools for each chain       
        threshold=chain_step.threshold
        hypotool_name=chain.name
        print "Adding %s to %s"%(threshold,hypoAlg.name)
        hypoAlg.addHypoTool(threshold)

    #end of this step configuration, now implement CF:
 
    print "\n******** instantiate algorithms in CF"
    print "There are %d steps in this CFStep"%(len(step_list))
    for step in step_list:
        print "\n Create Step %s"%(step.name)
        filt=step.filter
        print "Create seqFilter %s"%filt
        filterAlg=seqFilter(filt.name, Inputs=filt.inputs, Outputs=filt.outputs, Chains=filt.chains)
        #make algos:
        algos = []
        for alg in step.algs:
            print "Crate Algo %s"%alg
            algAlg= TestRecoAlg(alg.name, FileName=alg.filename, Output=alg.outputs[0], Input=alg.inputs[0])
            algos.append(algAlg)

        hypo=step.hypo
        print "Crate HypoAlgo %s"%hypo.name
        hypoAlg=TestHypoAlg(hypo.name, Input=hypo.inputs[0], Output=hypo.outputs[0])
        algos.append(hypoAlg)
        tools=hypo.tools
        htool_list = []
        for tool in tools:
            print "Crate HypoTool %s"%tool
            prop = ''.join(filter(lambda x: x.isalpha(), tool))
            hypotoolTool=TestHypoTool(tool, prop)
            htool_list.append(hypotoolTool)
            
        hypoAlg.HypoTools =htool_list
        print "Crate StepSeq %s"%step.name
        step_seq = stepSeq( step.name, filterAlg=filterAlg, rest=algos)       
        print "Add this step %s to %s"%(step.name,stepCF_name)
        stepCF += step_seq

    print "Add stepCF %s to the root"%stepCF_name
    TopHLTSeq += addSteps(stepCF)
    count_steps+=1
    print "************* End of step %s"%stepCF_name
    # end of steps


theApp.EvtMax = 3




#  is this needed??
#step +=seqFilter
