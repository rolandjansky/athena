# Classes to configure the CF graph, via Nodes
from AthenaCommon.CFElements import parOR, seqAND, stepSeq
import sys
import re



allAlgs={}
def useExisting(name):
    global allAlgs
    return allAlgs[name]

def AlgExisting(name):
    global allAlgs
    return name in allAlgs


def remember(name, instance):
    global allAlgs
    allAlgs[name] = instance
    return instance






#### Here functions to create the CF tree from CF configuration objects
def makeSummary(name, decisions):
    from DecisionHandling.DecisionHandlingConf import TriggerSummaryAlg
    summary = TriggerSummaryAlg( name, OutputLevel = 2 )
    summary.InputDecision = "HLTChains"
    summary.FinalDecisions = decisions
    summary.HLTSummary = "MonitoringSummary"+name
    return summary




from AthenaCommon.AlgSequence import dumpSequence
def create_step_reco_node(name, seq_list, dump=False):
    print "Create reco step %s with %d sequences"%(name, len(seq_list))
    stepCF = parOR(name+"_reco")
    for seq in seq_list:        
        step_seq = create_CFSequence(seq)             
        stepCF += step_seq
    
    if dump: dumpSequence (stepCF, indent=0)        
    return stepCF


def create_step_filter_node(name, seq_list, dump=False):
    print "Create filter step %s with %d filters"%(name, len(seq_list))
    stepCF = parOR(name+"_filter")
    for seq in seq_list:
        filterAlg=seq.filter.Alg                    
        print "Add  %s to filter node %s"%(filterAlg.name(),name)
#        print "DUMPFILTER ",filterAlg
        stepCF += filterAlg
    
    if dump: dumpSequence (stepCF, indent=0)        
    return stepCF



def create_step_sequence(name, filterAlg, rest,sublist):
    """ elementary HLT step sequencer, filterAlg is gating, rest is anything that needs to happe within the step """
    toadd=rest
    for k, v in sublist.iteritems():
        seq= seqAND(k, v)
        toadd.append(seq)
        
    stepReco = parOR(name+"_reco", toadd)
    
    stepAnd = seqAND(name, [ filterAlg, stepReco ])
    return stepAnd


def create_CFSequence(CFseq):   
    print "\n * Create CFSequence %s *"%(CFseq.name)
    from TrigUpgradeTest.MenuComponents import ViewNodeSequence
    filterAlg=CFseq.filter.Alg

    subs=[]
    for seq in CFseq.menuSeq.nodeSeqList:
        ath_sequence=seq.sequence.Alg
        subs.append(ath_sequence)

    stepReco = parOR(CFseq.name+"_reco", subs)
    seqAndView = seqAND(CFseq.name+"_view", [stepReco,seq.hypo.Alg ])      
    stepAnd = seqAND(CFseq.name, [ filterAlg, seqAndView ])
    return stepAnd

def find_stepCF_algs(step_list):
    algos = []
    for step in step_list:
        #print "Finding algos of step %s"%step
        step_algs = []
        step_algs.append(step.filter)
        for seq in step.menuSeq.nodeSeqList:
            step_algs.extend(seq.algs )
            step_algs.append(seq.hypo)
        algos.extend(step_algs)
    return algos        
            


def addChainToHypoTool(hypotool, chain):
    prop="Chain"
    setattr(hypotool, prop, chain)
    ## cval = hypotool.getProperties()[prop]
    ## try:
    ##     cval.append(chain)
    ##     setattr(hypotool, prop, cval)
    ## except:
    ##     pass


def addChainToHypoAlg(hypoAlg, chain):
    if "Comb" in hypoAlg.algname:
        prop="Chains"
        cval = hypoAlg.Alg.getProperties()[prop]
        try:
            cval.append(chain)
            setattr(hypoAlg.Alg, prop, cval)
        except:
            pass


#######################################
def decisionTree_From_Chains(HLTAllStepsSeq, chains):
    #from TrigUpgradeTest.MenuHypoTools import *
    print "Run decisionTree_From_Chains on %s"%(HLTAllStepsSeq.name())
    from TrigUpgradeTest.MenuComponents import RoRSequenceFilterNode
    HLTNodeName= HLTAllStepsSeq.name()
    testRoR=False
    # find nsteps
    NSTEPS=0
    for chain in chains:
        steps =len(chain.steps)
        if NSTEPS < steps:
            NSTEPS = steps
    print "Run on %d steps"%NSTEPS
    
    from TrigUpgradeTest.MenuComponents import CFSequence
    #loop over chains to configure
    count_steps=0
    finalDecisions = [] # needed for monitor
    allSeq_list = []
    for nstep in range(0, NSTEPS):
        stepCF_name =  "Step%i"%(nstep+1)
        CFseq_list = []
        step_decisions = []
        for chain in chains:
            #print len(chain.steps), count_steps
            if len(chain.steps) <= count_steps:
                continue
            chain_step=chain.steps[count_steps]
            chain_step_name= "%s:%s"%(stepCF_name, chain.name)
            print "\n*******Filling step %s for chain  %s"%(stepCF_name, chain.name)
      
            sequenceHypoTools=chain_step.sequences
#            sequenceHypoTools=chain_step.sequenceHypoTools
            countseq=0
            otherSubSequence = []
            for st in sequenceHypoTools:
                sequence=st
                hypotool=chain.hypoToolName
                #sequence=st.sequence
                #hypotool=st.hypotool
#                addChainToHypoTool(hypotool, chain.name)
#                print "Going through sequence %s with threshold %s"%(sequence.name, hypotool.getName())
                cfseq_name= sequence.name
                print "Going through sequence %s with threshold %s"%(sequence.name, hypotool)

                seeds= [nodeSeq.seed for nodeSeq in sequence.nodeSeqList]
                print "Found these seeds form the sequence: %s"%(seeds)
                #define sequence input
                if count_steps == 0: # seeding                   
                    filter_input= seeds
                    #[nodeSeq.seed for nodeSeq in sequence.nodeSeqList]
                    print "Seeds from this chain: %s"%(filter_input)
                    previous_sequence="".join(chain.group_seed)
                    previous_seeds=seeds
                    
                    #add check that the sequence seed is foreseen in the chain?
                    
                    # some manipulation for having the same strings as in the menu
                    ## filter_input = []
                    ## for seed in seeds:
                    ##     if seed == "L1EM":
                    ##         filter_input.append("EMRoIDecisions")
                    ##     elif seed == "L1MU":
                    ##         filter_input.append("MURoIDecisions")
                    ##     else:
                    ##        filter_input.append(seed)
                    ## print "Seeds to the filter: %s"%(filter_input)
                    ## previous_seeds=seeds
                else:
                    # from previous step, map the seuqence in the same order?
                    prev=chain.steps[count_steps-1].sequences[countseq]
                    previous_sequence = prev.name #menu sequence
                    filter_input      = prev.outputs
                    print "Connect to previous sequence through these filter inputs:"
                    print filter_input
                    previous_seeds = chain.steps[count_steps-1].sequences[countseq].seeds
                    if len(filter_input) != len(previous_seeds):
                        print "ERROR: found %d filter inputs and %d seeds"%(len(filter_input), len(previous_seeds))
                        sys.exit("ERROR, in size") 

                    
                # add hypotools
                for nodeSeq in sequence.nodeSeqList:
                    hypoAlg= nodeSeq.hypo
                    hypoToolClassName= nodeSeq.hypoToolClassName
                    print "Adding HypoTool::%s with name %s to %s"%(hypoToolClassName,chain.hypoToolName, hypoAlg.algname)
                    hypoAlg.addHypoTool(chain.hypoToolName, hypoToolClassName)
                    #hypoAlg.addHypoTool(hypotoolTool)
                    addChainToHypoAlg(hypoAlg, chain.name) # only for TMP Combo
##                    addChainToHypoTool(hypotoolTool, chain.name)                
                    


                #### FILTER
                # one filter per previous sequence at the start of the sequence: check if it exists or create a new one        
                # if the previous hypo has more than one output, try to get all of them
                # one filter per previous sequence: 1 input/previous seq, 1 output/next seq 
                filter_name="Filter%s_on_%s"%(stepCF_name,previous_sequence)
                filter_already_exists=False
                findFilter= [cfseq.filter for cfseq in CFseq_list if filter_name in cfseq.filter.algname]        
                if len(findFilter):
                    print "Filter %s already exists"%(filter_name)
                    filter_already_exists=True
                    sfilter=findFilter[0]
                    print "Adding chain %s to %s"%(chain.name,sfilter.algname)
                    sfilter.setChains(chain.name)
                    print sfilter.getChains()
                    continue
                else:
                    ## if (testRoR):
                    ##     sfilter = TestRoRSequenceFilterNode(name=filter_name)
                    ## else:
                    sfilter = RoRSequenceFilterNode(name=filter_name)

                    print "Adding these seeds to filter: %s"%(previous_seeds)
                    for i in previous_seeds: sfilter.addSeed(i)
                    for i in filter_input: sfilter.addInput(i)                        
                    filter_out=["%s_from_%s_out"%(filter_name,i) for i in filter_input]
                    #filter_out=["%s_from_%s_out"%(filter_name,i) for i in filter_input]                    
                    for o in filter_out: sfilter.setOutput(o)            

                    sfilter.setChains(chain.name)
                    print "Filter Done: %s"%(sfilter.name)
                    
                #loop over NodeSequences of this sequence to add inputs to InputMaker and send decisions to HypoAlg
                for nodeSeq in sequence.nodeSeqList:
                    seed=nodeSeq.seed
#                    out_index=[i for i,fseed in enumerate (sfilter.seeds) if fseed in seed  ]
                    input_maker_input=[sfilter.getOutputList()[i] for i,fseed in enumerate (sfilter.seeds) if fseed in seed  ]
                    #                    input_maker_input= [inp for i,inp in enumerate(sfilter.getOutputList()) if ]
                    #input_maker_input= [inp for inp in sfilter.getOutputList() if ("from_"+seed) in inp]

                    print "Adding %d inputs to sequence::%s from Filter::%s (from seed %s)"%(len(input_maker_input), nodeSeq.name, sfilter.algname, seed)
                    for i in input_maker_input: print i
                    if len(input_maker_input) == 0:
                        sys.exit("ERROR, no inputs to sequence are set!") 
#                        return                    
                    for i in input_maker_input: nodeSeq.addInput(i)

                    print "connecting InputMaker to HypoAlg"
                    input_maker_output=["%s_from_%s_output"%(nodeSeq.maker.algname,i)  for i in input_maker_input  ]
                    #input_maker_output=["%s_output"%i  for i in input_maker_input  ]
                    print "Adding %d output to InputMaker::%s and sending to HypoAlg::%s"%(len(input_maker_output), nodeSeq.maker.algname, nodeSeq.hypo.algname)
                    for i in input_maker_output: print i
                    if len(input_maker_output) == 0:
#                        print "ERROR, no inputs to sequence are set!"
                        sys.exit("ERROR, no outputs to sequence are set!") 
#                        return                    
                    for out in input_maker_output:
                          nodeSeq.addOutputDecision(out) 
                          nodeSeq.hypo.setPreviousDecision(out)
#                    print nodeSeq.hypo.Alg

                    #sequence_outDecisions= "SequenceDecision_%s"%nodeSeq.name
                    #nodeSeq.addDecision(sequence_outDecisions) #FPP

                    #hypoAlg= nodeSeq.hypo
                    #hypoAlg.setPreviousDecision(sequence_outDecisions) # works for one
                    #for i in input_maker_input: hypoAlg.setPreviousDecision(i) # works for one
                    #for i in input_maker_input: hypoAlg.addPreviousDecision(i)

                    #needed for the summary
                    step_decisions.append(nodeSeq.output)                
                                    
                CF_seq = CFSequence( cfseq_name, FilterAlg=sfilter, MenuSequence=sequence)
                #for seq in otherSubsequence: CF_seq.addSubSequence(seq)
                print CF_seq
                CFseq_list.append(CF_seq)
                countseq+=1
           
            #end of sequence/threshold
                
        #end of loop over chains for this step, now implement CF:
        print "\n******** Create CF Tree %s with AthSequencers"%(stepCF_name)
        
        #first make the filter step
        stepFilter = create_step_filter_node(stepCF_name, CFseq_list, dump=False)
        HLTAllStepsSeq += stepFilter
        allSeq_list.append(CFseq_list)

        # then the reco step
        stepCF = create_step_reco_node("%s_%s"%(HLTNodeName,stepCF_name), CFseq_list, dump=False)
        HLTAllStepsSeq += stepCF


        # then the monitor summary
        summary=makeSummary("TriggerSummary"+ stepCF_name, step_decisions)
        HLTAllStepsSeq += summary
        
        # update final decisions
        finalDecisions=step_decisions

        print "Now Draw..."
        stepCF_DataFlow_to_dot("%s_%s"%(HLTNodeName,stepCF_name), CFseq_list)
        stepCF_ControlFlow_to_dot(stepCF)

       
        print "Added stepCF %s to the root"%stepCF_name     
        count_steps+=1
        print "************* End of step %s"%stepCF_name
        # end of steps



   # summary=makeSummary("TriggerSummaryFinal", finalDecisions)
   # HLTAllStepsSeq += summary
#    dumpSequence (HLTAllStepsSeq, indent=0)
    all_DataFlow_to_dot(HLTNodeName, allSeq_list)
    return
#####




###### Here some graphical methods


from ViewAlgsTest.connectAlgorithmsIO import connectAlgorithmsIO, graph_generator
from AthenaCommon.AlgSequence import AthSequencer


def algColor(alg):
    if "Hypo" in alg: return "darkorchid1"
    if "Filter" in alg: return "chartreuse3"
    if "InputMaker" in alg: return "cyan3"
    return "cadetblue1"
    
def stepCF_ControlFlow_to_dot(stepCF):
    def _dump (seq, indent):
        o = list()
        for c in seq.getChildren():
            if "AthSequencer" in c.getFullName():
                o.append( ("%s[color=%s, shape=circle, width=.5, fixedsize=true ,style=filled]\n"%(c.name(),_seqColor(c)), indent) )
            else:
                o.append( ("%s[fillcolor=%s,style=filled]\n"%(c.name(),algColor(c.name())), indent) )
            o.append( ("%s -> %s\n"%(seq.name(), c.name()), indent))
            o.extend( _dump (c, indent+1) )
        return o

    def _parOR (seq):
        if seq.ModeOR is True:
            if seq.Sequential is False:
                if seq.StopOverride is True:
                    return True
        return False

    def _seqAND(seq):
        if seq.ModeOR is False:
            if seq.Sequential is True:
                if seq.StopOverride is False:
                    return True
        return False

    def _seqColor(seq):
        if _parOR(seq): return "red"
        if _seqAND(seq): return "blue"
        return "black"

   

    # to visualize: dot -T pdf Step1.dot > Step1.pdf
    file = open( '%s.CF.dot'%stepCF.name(), mode="wt" )
    #strict
    file.write( 'digraph step  {\n'\
                +'\n'\
                +'  node [ shape=polygon, fontname=Helvetica ]\n'\
                +'  edge [ fontname=Helvetica ]\n'
                +'  %s   [shape=Mdiamond]\n'%stepCF.name())

    indent=0
#    out = [("%s[color=%s shape=circle]\n"%(stepCF.name(),_seqColor(stepCF)), indent)]
    out = [("",indent)]
    out.extend( _dump( stepCF, indent=indent+1 ) )
    for n,i in out:
        line= "  "*i+ n
        file.write(line)

    file.write( '}\n')



def all_DataFlow_to_dot(name, step_list):
    file = open( '%s.dot'%(name), mode="wt" )
    file.write( 'digraph step  {\n'\
                    +'\n'\
                    +'  node [ shape=polygon, fontname=Helvetica ]\n'\
                    +'  edge [ fontname=Helvetica ]\n'
                    +'  %s   [shape=Mdiamond]\n'%name)
    
    nstep=1
    last_step_hypoNodes=[]
    for cfseq_list in step_list:
        # loop over steps
        step_connections = []
        step_connections.extend(last_step_hypoNodes)
        # reset the last step
        last_step_hypoNodes =[]
        for cfseq in cfseq_list:
            print cfseq.name
            file.write("  %s[fillcolor=%s style=filled]\n"%(cfseq.filter.algname,algColor(cfseq.filter.algname)))
            
            step_connections.append(cfseq.filter)
            for seq in cfseq.menuSeq.nodeSeqList:
                step_connections.append(seq.maker)
                step_connections.append(seq.hypo)

            #for inp in cfseq.filter.getInputList():
                #file.write(addConnection(name, cfseq.filter.algname, inp))
           
            file.write(  '\n  subgraph cluster_%s {\n'%(cfseq.name)\
                        +'     node [color=white style=filled]\n'\
                        +'     style=filled\n'\
                        +'     color=lightgrey\n'\
                        +'     fontname=Helvetica\n'\
                        +'     label = %s\n'%(cfseq.name))

            cfseq_algs = []
            cfseq_algs.append(cfseq.filter)

            for seq in cfseq.menuSeq.nodeSeqList:
                seq.reuse=False

            for seq in cfseq.menuSeq.nodeSeqList:
                cfseq_algs.append(seq.maker)
                cfseq_algs.append(seq.sequence )
                #cfseq_algs.extend(seq.algs )
                if seq.reuse==False:
                    file.write("    %s[fillcolor=%s]\n"%(seq.maker.algname, algColor(seq.maker.algname)))
                    file.write("    %s[fillcolor=%s]\n"%(seq.sequence.name, algColor(seq.sequence.name)))
                    #for alg in seq.algs: file.write("    %s[fillcolor=%s]\n"%(alg.algname, algColor(alg.algname)))
                    seq.reuse=True
                cfseq_algs.append(seq.hypo)
                last_step_hypoNodes.append(seq.hypo)
                file.write("    %s[color=%s]\n"%(seq.hypo.algname, algColor(seq.hypo.algname)))
            file.write('  }\n')              
            file.write(findConnections(cfseq_algs))
            file.write('\n')

        file.write(findConnections(step_connections))
        nstep+=1
  
    file.write( '}')
    file.close()
    

def stepCF_DataFlow_to_dot(name, cfseq_list):
    # to visualize: dot -T pdf Step1.dot > Step1.pdf
    file = open( '%s.DF.dot'%name, mode="wt" )
    #strict
    file.write( 'digraph step  {\n'\
                +'\n'\
                +'  node [ shape=polygon, fontname=Helvetica ]\n'\
                +'  edge [ fontname=Helvetica ]\n'
                +'  %s   [shape=Mdiamond]\n'%name)


    for cfseq in cfseq_list:
        print cfseq.name
        file.write("  %s[fillcolor=%s style=filled]\n"%(cfseq.filter.algname,algColor(cfseq.filter.algname)))
        for inp in cfseq.filter.getInputList():
            file.write(addConnection(name, cfseq.filter.algname, inp))
           
        file.write(  '\n  subgraph cluster_%s {\n'%(cfseq.name)\
                    +'     node [color=white style=filled]\n'\
                    +'     style=filled\n'\
                    +'     color=lightgrey\n'\
                    +'     fontname=Helvetica\n'\
                    +'     label = %s\n'%(cfseq.name))

        cfseq_algs = []
        cfseq_algs.append(cfseq.filter)       

        for seq in cfseq.menuSeq.nodeSeqList:
            cfseq_algs.append(seq.maker)
            cfseq_algs.append(seq.sequence )
            if seq.reuse==False:
                file.write("    %s[fillcolor=%s]\n"%(seq.maker.algname, algColor(seq.maker.algname)))
                file.write("    %s[fillcolor=%s]\n"%(seq.sequence.name, algColor(seq.sequence.name)))
                seq.reuse=True
            cfseq_algs.append(seq.hypo)
 
            file.write("    %s[color=%s]\n"%(seq.hypo.algname, algColor(seq.hypo.algname)))
        file.write('  }\n')              
        file.write(findConnections(cfseq_algs))
        file.write('\n')    
  
    file.write( '}')
    file.close()

def old_stepCF_DataFlow_to_dot(name, cfseq_list):
    # to visualize: dot -T pdf Step1.dot > Step1.pdf
    file = open( '%s.DF.dot'%name, mode="wt" )
    #strict
    file.write( 'digraph step  {\n'\
                +'\n'\
                +'  node [ shape=polygon, fontname=Helvetica ]\n'\
                +'  edge [ fontname=Helvetica ]\n'
                +'  %s   [shape=Mdiamond]\n'%name)


    for cfseq in cfseq_list:
        print cfseq.name
        file.write("  %s[fillcolor=%s style=filled]\n"%(cfseq.filter.algname,algColor(cfseq.filter.algname)))
        for inp in cfseq.filter.getInputList():
            file.write(addConnection(name, cfseq.filter.algname, inp))
           
        file.write(  '\n  subgraph cluster_%s {\n'%(cfseq.name)\
                    +'     node [color=white style=filled]\n'\
                    +'     style=filled\n'\
                    +'     color=lightgrey\n'\
                    +'     fontname=Helvetica\n'\
                    +'     label = %s\n'%(cfseq.name))

        cfseq_algs = []
        cfseq_algs.append(cfseq.filter)       

        for seq in cfseq.menuSeq.nodeSeqList:
            cfseq_algs.append(seq.maker)
            cfseq_algs.extend(seq.algs )
            if seq.reuse==False:
                file.write("    %s[fillcolor=%s]\n"%(seq.maker.algname, algColor(seq.maker.algname)))
                for alg in seq.algs: file.write("    %s[fillcolor=%s]\n"%(alg.algname, algColor(alg.algname)))
                seq.reuse=True
            cfseq_algs.append(seq.hypo)
 
            file.write("    %s[color=%s]\n"%(seq.hypo.algname, algColor(seq.hypo.algname)))
        file.write('  }\n')              
        file.write(findConnections(cfseq_algs))
        file.write('\n')    
  
    file.write( '}')
    file.close()


def findConnections(alg_list):
    lineconnect=''
    for nodeA in alg_list:
        for nodeB in alg_list:
            if nodeA is nodeB:
                continue
#            print "Find connection between %s and %s"%(nodeA.name, nodeB.name)
            dataIntersection = list(set(nodeA.getOutputList()) & set(nodeB.getInputList()))
            if len(dataIntersection) > 0:
                for line in dataIntersection:
                    lineconnect+=addConnection(nodeA.algname,nodeB.algname, line)
                    print 'Data connections between %s and %s: %s'%(nodeA.algname, nodeB.algname, line)
    return lineconnect

def addConnection(nodeA, nodeB, label):
    line = "    %s -> %s \n"%(nodeA,nodeB)
    #line = "    %s -> %s [label=%s]\n"%(nodeA,nodeB,label)
    # print line
    return line

