# Classes to configure the CF graph, via Nodes
from AthenaCommon.CFElements import parOR, seqAND, stepSeq
import sys
import re

from AthenaCommon.AlgSequence import dumpSequence

DH_DEBUG = True


#### Here functions to create the CF tree from CF configuration objects
def makeSummary(name, decisions):
    """ Returns a TriggerSummaryAlg connected to given decisions"""
    from DecisionHandling.DecisionHandlingConf import TriggerSummaryAlg
    summary = TriggerSummaryAlg( name, OutputLevel = 2 )
    summary.InputDecision = "HLTChains"
    summary.FinalDecisions = decisions
    summary.HLTSummary = "MonitoringSummary"+name
    return summary



def create_step_reco_node(name, seq_list, dump=False):
    """ elementary HLT reco step, contianing all sequences of the step """
    if DH_DEBUG: print "Create reco step %s with %d sequences"%(name, len(seq_list))
    stepCF = parOR(name+"_reco")
    for seq in seq_list:        
        step_seq = create_CFSequence(seq)             
        stepCF += step_seq
    
    if dump: dumpSequence (stepCF, indent=0)        
    return stepCF


def create_step_filter_node(name, seq_list, dump=False):
    """ elementary HLT filter step: OR node containing all Filters of the sequences. The node gates execution of next reco step """
    if DH_DEBUG: print "Create filter step %s with %d filters"%(name, len(seq_list))
    stepCF = parOR(name+"_filter")
    for seq in seq_list:
        filterAlg=seq.filter.Alg                    
        print "Add  %s to filter node %s"%(filterAlg.name(),name)
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
    """ Creates AthSequencer nodes with sequences attached """
    if DH_DEBUG: print "\n * Create CFSequence %s *"%(CFseq.name)
    filterAlg=CFseq.filter.Alg

    subs=[]
    for seq in CFseq.menuSeq.nodeSeqList:
        ath_sequence=seq.sequence.Alg
        subs.append(ath_sequence)

    stepReco = parOR(CFseq.name+"_reco", subs)
    seqAndView = seqAND(CFseq.name+"_view", [stepReco,seq.hypo.Alg ])      
    stepAnd = seqAND(CFseq.name, [ filterAlg, seqAndView ])
    return stepAnd


def addChainToHypoAlg(hypoAlg, chain):
    """ Tmp functions for Combo Hypo """
    if "Comb" in hypoAlg.algname:
        prop="Chains"
        cval = hypoAlg.Alg.getProperties()[prop]
        try:
            cval.append(chain)
            setattr(hypoAlg.Alg, prop, cval)
        except:
            pass


#######################################
## CORE of Decision Handling
#######################################

def decisionTree_From_Chains(HLTNode, chains):
    """ creates the decision tree, given the starting node and the chains containing the sequences  """
    if DH_DEBUG: print "Run decisionTree_From_Chains on %s"%(HLTNode.name())
    HLTNodeName= HLTNode.name()

    # find nsteps
    NSTEPS=0
    for chain in chains:
        steps =len(chain.steps)
        if NSTEPS < steps:
            NSTEPS = steps
    if DH_DEBUG: print "Run on %d steps"%NSTEPS
    
    from TrigUpgradeTest.MenuComponents import CFSequence, RoRSequenceFilterNode
    #loop over chains to configure
    count_steps=0
#    finalDecisions = [] # needed for monitor
    allSeq_list = []
    for nstep in range(0, NSTEPS):
        stepCF_name =  "Step%i"%(nstep+1)
        CFseq_list = []
        step_decisions = []
        for chain in chains:
            if len(chain.steps) <= count_steps:
                continue
            chain_step=chain.steps[count_steps]
            chain_step_name= "%s:%s"%(stepCF_name, chain.name)
            if DH_DEBUG: print "\n*******Filling step %s for chain  %s"%(stepCF_name, chain.name)
      
            sequenceHypoTools=chain_step.sequences
            hypotool=chain.hypoToolName
            countseq=0
            for sequence in sequenceHypoTools:
                cfseq_name= sequence.name
                if DH_DEBUG: print "Going through sequence %s with threshold %s"%(sequence.name, hypotool)

                seeds= [nodeSeq.seed for nodeSeq in sequence.nodeSeqList]
                if DH_DEBUG: print "Found these seeds form the sequence: %s"%(seeds)
                #define sequence input
                if count_steps == 0: # L1 seeding                   
                    filter_input= seeds
                    if DH_DEBUG: print "Seeds from this chain: %s"%(filter_input)
                    previous_sequence="".join(chain.group_seed)
                    previous_seeds=seeds                                        
                else:
                    # from previous step, map the seuqence in the same order?
                    prev=chain.steps[count_steps-1].sequences[countseq]
                    previous_sequence = prev.name #menu sequence
                    filter_input      = prev.outputs
                    if DH_DEBUG:  print "Connect to previous sequence through these filter inputs:"
                    if DH_DEBUG:  print filter_input
                    previous_seeds = chain.steps[count_steps-1].sequences[countseq].seeds
                    if len(filter_input) != len(previous_seeds):
                        print "ERROR: found %d filter inputs and %d seeds"%(len(filter_input), len(previous_seeds))
                        sys.exit("ERROR, in size") 

                # add hypotools
                for nodeSeq in sequence.nodeSeqList:
                    hypoAlg= nodeSeq.hypo
                    hypoToolClassName= nodeSeq.hypoToolClassName
                    if DH_DEBUG: print "Adding HypoTool::%s with name %s to %s"%(hypoToolClassName,chain.hypoToolName, hypoAlg.algname)
                    hypoAlg.addHypoTool(chain.hypoToolName, hypoToolClassName)
                    addChainToHypoAlg(hypoAlg, chain.name) # only for TMP Combo

                #### Build the FILTER
                # one filter per previous sequence at the start of the sequence: check if it exists or create a new one        
                # if the previous hypo has more than one output, try to get all of them
                # one filter per previous sequence: 1 input/previous seq, 1 output/next seq 
                filter_name="Filter%s_on_%s"%(stepCF_name,previous_sequence)
                filter_already_exists=False
                findFilter= [cfseq.filter for cfseq in CFseq_list if filter_name in cfseq.filter.algname]        
                if len(findFilter):
                    if DH_DEBUG: print "Filter %s already exists"%(filter_name)
                    filter_already_exists=True
                    sfilter=findFilter[0]
                    if DH_DEBUG: print "Adding chain %s to %s"%(chain.name,sfilter.algname)
                    sfilter.setChains(chain.name)
                    if DH_DEBUG: print sfilter.getChains()
                    continue
                else:
                    sfilter = RoRSequenceFilterNode(name=filter_name)
                    if DH_DEBUG: print "Adding these seeds to filter: %s"%(previous_seeds)
                    for i in previous_seeds: sfilter.addSeed(i)
                    for i in filter_input: sfilter.addInput(i)                        
                    filter_out=["%s_from_%s_out"%(filter_name,i) for i in filter_input]
                    for o in filter_out: sfilter.setOutput(o)            

                    sfilter.setChains(chain.name)
                    if DH_DEBUG: print "Filter Done: %s"%(sfilter.name)
                    
                # Connect the InputMaker
                #loop over NodeSequences of this sequence to add inputs to InputMaker and send decisions to HypoAlg
                for nodeSeq in sequence.nodeSeqList:
                    seed=nodeSeq.seed
                    input_maker_input=[sfilter.getOutputList()[i] for i,fseed in enumerate (sfilter.seeds) if fseed in seed  ]
                    if DH_DEBUG: print "Adding %d inputs to sequence::%s from Filter::%s (from seed %s)"%(len(input_maker_input), nodeSeq.name, sfilter.algname, seed)
                    for i in input_maker_input: print i
                    if len(input_maker_input) == 0:
                        sys.exit("ERROR, no inputs to sequence are set!") 
                    for i in input_maker_input: nodeSeq.addInput(i)
                    input_maker_output=["%s_from_%s_output"%(nodeSeq.maker.algname,i)  for i in input_maker_input  ]

                    if len(input_maker_output) == 0:
                        sys.exit("ERROR, no outputs to sequence are set!") 
                    if DH_DEBUG:
                        print "connecting InputMaker to HypoAlg"
                        print "Adding %d output to InputMaker::%s and sending to HypoAlg::%s"%(len(input_maker_output), nodeSeq.maker.algname, nodeSeq.hypo.algname)
                        for i in input_maker_output: print i
                   
                    for out in input_maker_output:
                          nodeSeq.addOutputDecision(out) 
                          nodeSeq.hypo.setPreviousDecision(out)

                    #needed for the summary
                    step_decisions.append(nodeSeq.output)                
                                    
                CF_seq = CFSequence( cfseq_name, FilterAlg=sfilter, MenuSequence=sequence)
                if DH_DEBUG:  print CF_seq
                CFseq_list.append(CF_seq)
                countseq+=1
           
            #end of sequence/threshold
                
        #end of loop over chains for this step, now implement CF:
        if DH_DEBUG: print "\n******** Create CF Tree %s with AthSequencers"%(stepCF_name)        
        #first make the filter step
        stepFilter = create_step_filter_node(stepCF_name, CFseq_list, dump=False)
        HLTNode += stepFilter
        allSeq_list.append(CFseq_list)

        # then the reco step
        stepCF = create_step_reco_node("%s_%s"%(HLTNodeName,stepCF_name), CFseq_list, dump=False)
        HLTNode += stepCF


        # then the monitor summary
        summary=makeSummary("TriggerSummary"+ stepCF_name, step_decisions)
        HLTNode += summary
        
        # update final decisions
#        finalDecisions=step_decisions

        if DH_DEBUG: print "Now Draw..."
        stepCF_DataFlow_to_dot("%s_%s"%(HLTNodeName,stepCF_name), CFseq_list)
        stepCF_ControlFlow_to_dot(stepCF)
       
        count_steps+=1
        print "************* End of step %s"%stepCF_name
        # end of steps



   # summary=makeSummary("TriggerSummaryFinal", finalDecisions)
   # HLTNode += summary
#    dumpSequence (HLTNode, indent=0)
    all_DataFlow_to_dot(HLTNodeName, allSeq_list)
    return

###########################################################




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
                    file.write("    %s[fillcolor=%s]\n"%(seq.sequence.algname, algColor(seq.sequence.algname)))
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
                file.write("    %s[fillcolor=%s]\n"%(seq.sequence.algname, algColor(seq.sequence.algname)))
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
            dataIntersection = list(set(nodeA.getOutputList()) & set(nodeB.getInputList()))
            if len(dataIntersection) > 0:
                for line in dataIntersection:
                    lineconnect+=addConnection(nodeA.algname,nodeB.algname, line)
                    print 'Data connections between %s and %s: %s'%(nodeA.algname, nodeB.algname, line)
            
    return lineconnect

def findConnections_wihtin_sequence(alg_list):
    lineconnect=''
    import itertools
    for nodeA, nodeB in itertools.combinations(alg_list, 2):
        lineconnect+=findDHconnections(nodeA, nodeB)                                    
    return lineconnect

def findDHconnections(nodeA, nodeB):
    lineconnect=''
    # search connections vis DH
#    print "Find connection between %s and %s"%(nodeA.algname, nodeB.algname)
    outs= getValuesProperties(nodeA)
    ins = getValuesProperties(nodeB)
    dataIntersection = list(set(outs) & set(ins))
    if len(dataIntersection) > 0:
        for line in dataIntersection:
            lineconnect+=addConnection(nodeA.algname,nodeB.algname, line)
            print 'Data DH connections between %s and %s: %s'%(nodeA.algname, nodeB.algname, line)
    return lineconnect
    


def getValuesProperties(node):
    Excluded = ["StoreGateSvc/DetectorStore", "StoreGateSvc"]
    values = []
    algs = []
    if "AthSequencer" in node.Alg.getFullName():
         seq=node.Alg
         algs = seq.getChildren()
         algs.pop(0)
         #.pop(0) # remove inputmaker
    else:
        algs.append(node.Alg)

    
    for alg in algs:        
#        print "prop of alg %s"%(alg.name())
        for k, cval in alg.getValuedProperties().items():
#            print "appending %s"%(cval)
            if type(cval) == type(list()):  
                for val in cval:
                    if type(val) == type(''):
                        if val not in Excluded:
                            values.append(val)            
            elif type(cval) == type(''):
                if cval not in Excluded:
                    values.append(cval)
            else:
                continue
            #getattr(alg, prop))
    return set(values)
        

def addConnection(nodeA, nodeB, label):
    line = "    %s -> %s \n"%(nodeA,nodeB)
    #line = "    %s -> %s [label=%s]\n"%(nodeA,nodeB,label)
    # print line
    return line

