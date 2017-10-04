from AthenaCommon.CFElements import parOR, seqAND, stepSeq


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

from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestRoRSeqFilter
def seqFilter(name, Inputs=[], Outputs=[], Chains=[]):
    global allAlgs
    input_list = list(set(Inputs))
    output_list = list (set(Outputs))
    chain_list = list (set(Chains))
    fname = "F_"+name
    f = HLTTest__TestRoRSeqFilter(fname, OutputLevel = DEBUG, Inputs=input_list, Outputs=output_list, Chains=chain_list)
    if "Step1" in name: # so that we see events running through, will be gone once L1 emulation is included
        f.AlwaysPass = True        
    allAlgs[name] = f
    return f

from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestInputMaker
def seqInputMaker(name, Inputs=[], Outputs=[]):
    global allAlgs
    input_list = list(set(Inputs))
    output_list = list (set(Outputs))
    fname="IM_"+name
    f = HLTTest__TestInputMaker(fname, OutputLevel = DEBUG, Input=Inputs, Output=Outputs, LinkName="initialRoI")
    allAlgs[name] = f
    return f


from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestMerger
def merger(name, Inputs, Output ):
    return remember(name,   HLTTest__TestMerger( "M_"+name, OutputLevel=DEBUG, Inputs=Inputs, Output=Output ) )

def addSteps(s):
    return seqAND("HLTChainsSeq", s)



#### Here functions to create the CF tree from CF configuration objects

from TrigUpgradeTest.HLTCFConfig import *
include("TrigUpgradeTest/HLTSignatureConfig.py")

import re
def create_stepSequence(step):   
    print "\n Create Step %s"%(step.name)
    filt=step.filter
    if AlgExisting(filt.name):
        print "Found already seqFilter %s"%filt
        filterAlg = useExisting(filt.name)
    else:
        print "Create seqFilter %s"%filt
        filterAlg=seqFilter(filt.name, Inputs=filt.inputs, Outputs=filt.outputs, Chains=filt.chains)
    algos = []
    for seq in step.cfRecoSequences:
        input_maker=seq.inputMaker
        if AlgExisting(input_maker.name):
            print "Found already InputMaker %s"%input_maker
            inputMakerAlg = useExisting(input_maker.name)
        else:
            print "Create seqInputMaker %s"%input_maker
            inputMakerAlg= seqInputMaker(input_maker.name, Inputs=input_maker.inputs[0], Outputs=input_maker.outputs[0])
        algos.append(inputMakerAlg)
        #make algos:
        for alg in seq.recoSequence.algs:
            if AlgExisting(alg.name):
                print "Found already RecoAlg %s"%alg
                algAlg = useExisting(alg.name)
            else:
                print "Create Algo %s"%alg                     
                theAlg = globals()[alg.name]
                algAlg=theAlg(alg.name, FileName=alg.filename, Output=alg.outputs[0], Input=alg.inputs[0])
            algos.append(algAlg)

    # make Hypo
    hypo=step.hypo
    if AlgExisting(hypo.name):
        print "Found already HypoAlg %s"%hypo
        hypoAlg = useExisting(hypo.name)
    else:
        print "Crate HypoAlgo %s"%hypo
        theAlg = globals()[hypo.name]


        tools=hypo.tools
        if "Comb" in hypo.name: ###TMP combo, only one threshold
            hypoAlg=theAlg(hypo.name, Input=hypo.inputs, Output=hypo.outputs)
            thresholds = map(int, re.findall(r'\d+', tools[0].name))
            hypoAlg.Threshold1= thresholds[0]
            hypoAlg.Threshold2= thresholds[1]
            hypoAlg.DecisionLabel=tools[0].name
        else:
            hypoAlg=theAlg(hypo.name, Input=hypo.inputs[0], Output=hypo.outputs[0])
            htool_list = []
            for tool in tools:
                print "Crate HypoTool %s"%tool.name
                theAlg = globals()[tool.name]
                hypotoolTool=theAlg(tool.name)
                htool_list.append(hypotoolTool)
            hypoAlg.HypoTools = htool_list

    algos.append(hypoAlg)        

    print "Crate StepSeq %s"%step.name
    step_seq = stepSeq( step.name, filterAlg=filterAlg, rest=algos)       
    return step_seq


def find_stepCF_algs(step_list):
    algos = []
    for step in step_list:
        #print "Finding algos of step %s"%step
        step_algs = []
        step_algs.append(step.filter)
        for seq in step.cfRecoSequences:
            step_algs.extend(seq.recoSequence.algs )
        step_algs.append(step.hypo)
        #print step_algs
        algos.extend(step_algs)
    return algos        
            

from AthenaCommon.AlgSequence import dumpSequence
def create_StepCF(name, step_list, dump=0):
    print "There are %d steps in this CFStep %s"%(len(step_list), name)
    stepCF = parOR(name)
    for step in step_list:        
        step_seq = create_stepSequence(step)             
        print "Add this step %s to %s"%(step.name,name)
        stepCF += step_seq

    
    if dump: dumpSequence (stepCF, indent=0)
        
    return stepCF



###### Here some graphical methods


from ViewAlgsTest.connectAlgorithmsIO import connectAlgorithmsIO, graph_generator
from AthenaCommon.AlgSequence import AthSequencer


def algColor(alg):
    if "Hypo" in alg: return "darkorchid1"
    if "F_"  in alg: return "chartreuse3"
    if "Filter" in alg: return "chartreuse3"
    if "IM_" in alg: return "cyan3"
    return "cadetblue1"
    
def stepCF_ControlFlow_to_dot(stepCF):
    def _dump (seq, indent):
        o = list()
        for c in seq.getChildren():
            if "AthSequencer" in c.getFullName():
                o.append( ("%s[color=%s, shape=circle, width=.5, fixedsize=true ,style=filled]\n"%(c.name(),_seqColor(c)), indent) )
            else:
                o.append( ("%s[fillcolor=%s,style=filled]\n"%(c.name(),algColor(c.name())), indent) )
            o.append( ("%s -> %s"%(seq.name(), c.name()), indent))
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
    out = [("%s[color=%s shape=circle]\n"%(stepCF.name(),_seqColor(stepCF)), indent)]
  
    out.extend( _dump( stepCF, indent=indent+1 ) )
    for n,i in out:
        line= "  "*i+ n
        file.write(line)

    file.write( '}\n')



    

def stepCF_DataFlow_to_dot(name, step_list):
    # to visualize: dot -T pdf Step1.dot > Step1.pdf
    file = open( '%s.DF.dot'%name, mode="wt" )
    #strict
    file.write( 'digraph step  {\n'\
                +'\n'\
                +'  node [ shape=polygon, fontname=Helvetica ]\n'\
                +'  edge [ fontname=Helvetica ]\n'
                +'  %s   [shape=Mdiamond]\n'%name)


    for step in step_list:
        file.write("  %s[fillcolor=%s style=filled]\n"%(step.filter.name,algColor(step.filter.name)))
        for inp in step.filter.inputs:
            file.write(addConnection(name, step.filter.name, inp))
        for seq in step.cfRecoSequences:
            for inp in seq.inputMaker.inputs:
                file.write(addConnection(step.filter.name,seq.inputMaker.name,inp))

        #        file.write('%s -> %s [label=%s]'%(name,step.filter.name,step.filter.inputs[0]))
        file.write(  '\n  subgraph cluster_%s {\n'%(step.name)\
                    +'     node [color=white style=filled]\n'\
                    +'     style=filled\n'\
                    +'     color=lightgrey\n'\
                    +'     fontname=Helvetica\n'\
                    +'     label = %s\n'%(step.name))

        step_algs = []
        step_algs.append(step.filter)
        step_algs.append(step.hypo)

        for seq in step.cfRecoSequences:
            step_algs.append(seq.inputMaker)
            step_algs.extend(seq.recoSequence.algs )
            if seq.recoSequence.reuse==False:
                file.write("    %s[fillcolor=%s]\n"%(seq.inputMaker.name, algColor(seq.inputMaker.name)))
                for alg in seq.recoSequence.algs: file.write("    %s[fillcolor=%s]\n"%(alg.name, algColor(alg.name)))
                seq.recoSequence.reuse=True
 
        file.write("    %s[color=%s]\n"%(step.hypo.name, algColor(step.hypo.name)))
        file.write('  }\n')              
        file.write(findConnections(step_algs))
        file.write('\n')    
  
    file.write( '}')
    file.close()


def findConnections(alg_list):
    lineconnect=''
    for nodeA in alg_list:
        for nodeB in alg_list:
            if nodeA is nodeB:
                continue
            dataIntersection = list(set(nodeA.outputs) & set(nodeB.inputs))
            if len(dataIntersection) > 0:
                lineconnect+=addConnection(nodeA.name,nodeB.name, dataIntersection[0])
                print 'Data connections between %s and %s: %s'%(nodeA.name, nodeB.name, dataIntersection)
    return lineconnect

def addConnection(nodeA, nodeB, label):
    line = "    %s -> %s [label=%s]\n"%(nodeA,nodeB,label)
    return line

