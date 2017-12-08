# Classes to configure the CF graph, via Nodes
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


class AlgNode():
    def __init__(self, Alg, inputProp, outputProp):
        self.name = ("%s_%s_%s")%( Alg.name(), inputProp, outputProp)
        self.algname = Alg.name()
        self.outputProp=outputProp
        self.inputProp=inputProp
        self.configureAlg(Alg)
                
    def configureAlg(self, Alg):
        alg_name = Alg.getName()
        if AlgExisting(alg_name):
            print "Found already Alg %s"%alg_name
            self.Alg = useExisting(alg_name)
        else:
            print "Add new Alg %s"%alg_name
            self.Alg=Alg
            remember(alg_name, self.Alg)
            
        if self.Alg is  None:
            return

        self.addDefaultOutput()

    def addDefaultOutput(self):
        self.setOutput(("%s_out"%(self.name)))

    def setPar(self, prop, name):
        cval = self.Alg.getProperties()[prop]
        try:
            if type(cval) == type(list()):                
                cval.append(name)
                setattr(self.Alg, prop, cval)
            else:
                setattr(self.Alg, prop, name)
        except:
            pass
 
    def getPar(self, prop):
        try:
            return getattr(self.Alg, prop)
        except:
            return self.Alg.getDefaultProperty(prop)
        raise "Error in reading property " + prop + " from " + self.Alg


    def setOutput(self, name):
        return self.setPar(self.outputProp,name)

    def getOutput(self):
        return self.getPar(self.outputProp)

    def getOutputList(self):
        outputs = []
        cval = self.getOutput()
        if type(cval) == type(list()):  
            outputs.extend(cval)
        else:
            outputs.append(cval)
        return outputs
    
    def setInput(self, name):
        return self.setPar(self.inputProp,name)
    
    def getInput(self):
        return self.getPar(self.inputProp)

    def getInputList(self):
        inputs = []
        cval = self.getInput()
        if type(cval) == type(list()):  
            inputs.extend(cval)
        else:
            inputs.append(cval)
        return inputs

    def __str__(self):
        return "Alg::%s  [%s] -> [%s]"%(self.name,' '.join(map(str, self.getInputList())), ' '.join(map(str, self.getOutputList())))


 ## class TestRecoAlgNode(AlgNode):
 ##    def __init__(self, Alg, inputProp, outputProp):
 ##        ## self.outputProp='Output'
 ##        ## self.inputProp='Input'
 ##        self.name = "R_"+ name
 ##        AlgNode.__init__(self, name, Alg, inputProp, outputProp)


class HypoAlgNode(AlgNode):
    def __init__(self, Alg, inputProp, outputProp):
        AlgNode.__init__(self, Alg, inputProp, outputProp)
        self.tools = []

    def addHypoTool(self, hypotool):
        if "Comb" in self.name: ###TMP combo, only one threshold
            import re
            thresholds = map(int, re.findall(r'\d+', hypotool))
            self.setPar('Threshold1', thresholds[0])
            self.setPar('Threshold2', thresholds[1])
            self.setPar('DecisionLabel', hypotool)
        else:
            self.tools.append(hypotool)
            return self.setPar('HypoTools',hypotool)        


from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestRoRSeqFilter
class SequenceFilterNode(AlgNode):
    def __init__(self, name):
        Alg= HLTTest__TestRoRSeqFilter(name, OutputLevel = 2)
        inputProp='Inputs'
        outputProp='Outputs'
        AlgNode.__init__(self,  Alg, inputProp, outputProp)
        if "Step1" in self.name: # so that we see events running through, will be gone once L1 emulation is included
            self.Alg.AlwaysPass = True   
  
    def addDefaultOutput(self):
        return #do nothing    
       
    def setChains(self, name):
        return self.setPar("Chains", name)
    
    def getChains(self):
        return self.getPar("Chains")




#### Here functions to create the CF tree from CF configuration objects

#from TrigUpgradeTest.HLTCFConfig import *
#include("TrigUpgradeTest/HLTSignatureConfig.py")

import re
def create_CFSequence(CFseq):   
    print "\n * Create CFSequence %s"%(CFseq.name)
    filterAlg=CFseq.filter.Alg
   
    nodes = CFseq.getAllNodes()
    algos = []
    for node in nodes:
        algos.append(node.Alg)

    algos=list(set(algos))

    step_seq = stepSeq( CFseq.name, filterAlg=filterAlg, rest=algos)       
    return step_seq

  

def find_stepCF_algs(step_list):
    algos = []
    for step in step_list:
        #print "Finding algos of step %s"%step
        step_algs = []
        step_algs.append(step.filter)
        for seq in step.menuSeq.nodeSeqList:
            step_algs.extend(seq.algs )
            step_algs.append(seq.hypo)
        #print step_algs
        algos.extend(step_algs)
    return algos        
            

from AthenaCommon.AlgSequence import dumpSequence
def create_StepCF(name, seq_list, dump=0):
    print "There are %d steps in this CFStep %s"%(len(seq_list), name)
    stepCF = parOR(name)
    for seq in seq_list:        
        step_seq = create_CFSequence(seq)             
        print "Add this step %s to %s"%(seq.name,name)
        stepCF += step_seq
    
    if dump: dumpSequence (stepCF, indent=0)
        
    return stepCF



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
        file.write("  %s[fillcolor=%s style=filled]\n"%(cfseq.filter.algname,algColor(cfseq.filter.algname)))
        for inp in cfseq.filter.getInputList():
            file.write(addConnection(name, cfseq.filter.algname, inp))
        for seq in cfseq.menuSeq.nodeSeqList:
            for inp in seq.inputMaker.getInputList():
                file.write(addConnection(cfseq.filter.algname,seq.inputMaker.algname,inp))

        #        file.write('%s -> %s [label=%s]'%(name,cfseq.filter.algname,cfseq.filter.inputs[0]))
        file.write(  '\n  subgraph cluster_%s {\n'%(cfseq.name)\
                    +'     node [color=white style=filled]\n'\
                    +'     style=filled\n'\
                    +'     color=lightgrey\n'\
                    +'     fontname=Helvetica\n'\
                    +'     label = %s\n'%(cfseq.name))

        cfseq_algs = []
        cfseq_algs.append(cfseq.filter)
        #        cfseq_algs.append(cfseq.hypo)

        for seq in cfseq.menuSeq.nodeSeqList:
            cfseq_algs.append(seq.inputMaker)
            cfseq_algs.extend(seq.algs )
            if seq.reuse==False:
                file.write("    %s[fillcolor=%s]\n"%(seq.inputMaker.algname, algColor(seq.inputMaker.algname)))
                for alg in seq.algs: file.write("    %s[fillcolor=%s]\n"%(alg.algname, algColor(alg.algname)))
                seq.reuse=True
            cfseq_algs.append(seq.hypo)
 
            file.write("    %s[color=%s]\n"%(seq.hypo.algname, algColor(seq.hypo.algname)))
        #file.write("    %s[color=%s]\n"%(cfseq.hypo.algname, algColor(cfseq.hypo.algname)))
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
                lineconnect+=addConnection(nodeA.algname,nodeB.algname, dataIntersection[0])
                print 'Data connections between %s and %s: %s'%(nodeA.algname, nodeB.algname, dataIntersection)
    return lineconnect

def addConnection(nodeA, nodeB, label):
    line = "    %s -> %s [label=%s]\n"%(nodeA,nodeB,label)
    return line

