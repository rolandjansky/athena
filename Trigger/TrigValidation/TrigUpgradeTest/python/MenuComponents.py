class Node():
    def __init__(self, Alg, inputProp='', outputProp=''):
        self.name = ("%s_%s_%s")%( Alg.name(), inputProp, outputProp)
        self.Alg=Alg
        self.algname = Alg.name()
        self.outputProp=outputProp
        self.inputProp=inputProp

    def setOutput(self, name):
         self.outputProp=name

    def getOutput(self):
        return self.outputProp
    
    def getOutputList(self):
        outputs = []
        outputs.append(self.outputProp)
        return outputs
    
    def setInput(self, name):
        self.inputProp=name

    def addInput(self, name):
        self.inputProp=name
    
    def getInput(self):
        return self.inputProp
    
    def getInputList(self):
        inputs = []
        inputs.append(self.inputProp)
        return inputs

    def __str__(self):
        return "Node::%s  [%s] -> [%s]"%(self.algname, self.getInput(), self.getOutput())

    
    
class AlgNode(Node):
    def __init__(self, Alg, inputProp, outputProp):
        Node.__init__(self, Alg, inputProp, outputProp)        

    def addDefaultOutput(self):
        if self.outputProp is not '':
            self.setOutput(("%s_%s_out"%(self.algname,self.outputProp)))
        
    def setPar(self, prop, name):
        cval = self.Alg.getProperties()[prop]
        try:
            if type(cval) == type(list()):                
                cval.append(name)
                return setattr(self.Alg, prop, cval)
            else:
                return setattr(self.Alg, prop, name)
        except:
            pass

    def setParArray(self, prop, name):
        cval = self.Alg.getProperties()[prop]
        try:
            cval.append(name)
            return setattr(self.Alg, prop, cval)
        except:
            pass
 
    def getPar(self, prop):
        try:
            return getattr(self.Alg, prop)
        except:
            return self.Alg.getDefaultProperty(prop)
        raise "Error in reading property " + prop + " from " + self.Alg


    def setOutput(self, name):
        if self.outputProp is not '':
            return self.setPar(self.outputProp,name)

    def addOutput(self, name):
        if self.outputProp is not '':
            return self.setParArray(self.outputProp,name)
 

    def getOutput(self):
        if self.outputProp is not '':
            return self.getPar(self.outputProp)
        return self.outputProp
    
    def getOutputList(self):
        outputs = []
        cval = self.getOutput()
        if cval == '':
            return outputs
        if type(cval) == type(list()):  
            outputs.extend(cval)
        else:
            outputs.append(cval)
        return outputs
    
    def setInput(self, name):
        if self.inputProp is not '':
            return self.setPar(self.inputProp,name)

    def addInput(self, name):
        if self.inputProp is not '':
            return self.setParArray(self.inputProp,name)
    
    def getInput(self):
        if self.inputProp is not '':
            return self.getPar(self.inputProp)
        return self.inputProp
    
    def getInputList(self):
        inputs = []
        cval = self.getInput()
        if cval =='':
            return inputs
        if type(cval) == type(list()):  
            inputs.extend(cval)
        else:
            inputs.append(cval)
        return inputs

    def __str__(self):
        return "Alg::%s  [%s] -> [%s]"%(self.algname, ' '.join(map(str, self.getInputList())), ' '.join(map(str, self.getOutputList())))


 
class HypoAlgNode(AlgNode):
    def __init__(self, Alg):
        assert isHypoBase(Alg), "Error in creating HypoAlgNode from Alg "  + Alg.name()
        AlgNode.__init__(self, Alg, 'HypoInputDecisions', 'HypoOutputDecisions')
        self.addDefaultOutput()
        self.tools = []
        self.previous=[]

    def addHypoTool(self, hypoToolName, hypoToolClassName):
        from TrigUpgradeTest.MenuHypoTools import createHypoTool        

        tools = self.Alg.HypoTools
        ## HypoTools are private, so need to be created when added to the Alg
        try:
            self.Alg.HypoTools = tools+[createHypoTool(hypoToolClassName, hypoToolName)]
        except:
            print "Error in creating HypoTool " + hypoToolName + " of type " + hypoToolClassName

        self.tools.append(hypoToolName)            
        return True
           

    def setPreviousDecision(self,prev):        
        self.previous.append(prev)
        return self.setInput(prev)

    def __str__(self):
        return "HypoAlg::%s  [%s] -> [%s], previous = [%s], HypoTools=[%s]"%(self.Alg.name(),' '.join(map(str, self.getInputList())),
                                                                                 ' '.join(map(str, self.getOutputList())),
                                                                                 ' '.join(map(str, self.previous)),
                                                                                 ' '.join(map(str, self.tools)))



class ComboHypoAlgNode(AlgNode):
    # TMP class for curent Combo alg
    def __init__(self, Alg):
        AlgNode.__init__(self, Alg, inputProp='', outputProp='')
        self.tools = []
        self.previous=[]
        self.addDefaultOutput()

    def addDefaultOutput(self):
         self.setPar('Output1', ("%s_Output1"%(self.algname)))
         self.setPar('Output2', ("%s_Output2"%(self.algname)))

    def setInputs(self, inputs):
        self.setPar('previousDecisions1', inputs[0])
        self.setPar('previousDecisions2', inputs[1])

    def setOutputs(self, outputs):
        self.setPar('Output1', outputs[0])
        self.setPar('Output2', outputs[1])


    def addHypoTool(self, hypoToolName, hypoToolClassName):
        self.tools.append(hypoToolName) 
        import re
        thresholds = map(int, re.findall(r'\d+', hypoToolName))
        self.setPar('Threshold1', thresholds[0])
        self.setPar('Threshold2', thresholds[1])
        return self.setPar('DecisionLabel', hypoToolName)        


    def setPreviousDecision(self,prev):
        self.previous.append(prev)
        if  "from_L1MU" in prev:
            if "pt" in self.getPar("Property1"):
                status= self.setPar('previousDecisions1',prev)
            if "pt" in self.getPar("Property2"):
                status= self.setPar('previousDecisions2',prev)
        if  "from_L1EM" in prev:
            if "et" in self.getPar("Property1"):
                status= self.setPar('previousDecisions1',prev)
            if "et" in self.getPar("Property2"):
                status= self.setPar('previousDecisions2',prev)          
        if "MuInputMaker" in prev:
            status= self.setPar('previousDecisions1',prev)
        if "ElInputMaker" in prev:
            status= self.setPar('previousDecisions2',prev)
        return status

    def getOutputList(self):
        outputs = []
        outputs.append(self.getPar('Output1'))
        outputs.append(self.getPar('Output2'))
        return outputs
    
    def getInputList(self):
        inputs = []
        inputs.append(self.getPar('previousDecisions1'))
        inputs.append(self.getPar('previousDecisions2'))
        return inputs
        

    def __str__(self):
        return "ComboHypoAlg::%s  [%s] -> [%s], previous = [%s], HypoTools=[%s]"%(self.Alg.name(),' '.join(map(str, self.getInputList())),
                                                                                 ' '.join(map(str, self.getOutputList())),
                                                                                 ' '.join(map(str, self.previous)),
                                                                                 ' '.join(map(str, self.tools)))

class SequenceFilterNode(AlgNode):
    def __init__(self, Alg, inputProp, outputProp):
        AlgNode.__init__(self,  Alg, inputProp, outputProp)
        self.seeds = []
        
    def addDefaultOutput(self):
        return #do nothing    
       
    def setChains(self, name):
        return self.setPar("Chains", name)
    
    def getChains(self):
        return self.getPar("Chains")

    def addSeed(self, seed):
        self.seeds.append(seed)




from DecisionHandling.DecisionHandlingConf import RoRSeqFilter, DumpDecisions
class RoRSequenceFilterNode(SequenceFilterNode):
    def __init__(self, name):
        Alg= RoRSeqFilter(name, OutputLevel = 2)
        SequenceFilterNode.__init__(self,  Alg, 'Input', 'Output')



class InputMakerNode(AlgNode):
    def __init__(self, Alg):
        assert isInputMakerBase(Alg), "Error in creating InputMakerNode from Alg "  + Alg.name()
        AlgNode.__init__(self,  Alg, 'InputMakerInputDecisions', 'InputMakerOutputDecisions')

            

#########################################################
# USEFULL TOOLS
#########################################################

def isHypoBase(alg):
    return  ('HypoInputDecisions'  in alg.__class__.__dict__)

def isInputMakerBase(alg):
    return  ('InputMakerInputDecisions'  in alg.__class__.__dict__)

def isFilterAlg(alg):
    return isinstance(alg, RoRSeqFilter)
        
##########################################################
# NOW sequences and chains
##########################################################




class HLTRecoSequence():
    def __init__(self, name, Sequence, Maker, Seed):
        print "Making sequence %s"%(name)
        self.name = name
        self.seed = Seed
        self.sequence =    Node( Alg=Sequence, inputProp='%s_in'%(Sequence.name()), outputProp='%s_out'%(Sequence.name()))
        self.maker =    InputMakerNode( Alg=Maker)
        #self.maker =    AlgNode( Alg=Maker, inputProp='InputMakerInputDecisions',    outputProp='InputMakerOutputDecisions')
        self.reuse = False     

    def getOutputList(self):
        return self.maker.getOutput()

    def getInputList(self):
        return self.maker.getInput()
        
    def addInput(self,theinput):
        print "adding input to sequence: %s to %s"%(theinput, self.maker.algname )
        self.maker.addInput(theinput)

    def addOutputDecision(self,theinput):
        print "adding output decisions (%s) to sequence %s  (output of alg %s)"%(theinput, self.name, self.maker.algname )
        self.maker.addOutput(theinput)
        
    def __str__(self):
        return "HLTRecoSequence::%s with \n Seed::%s \n Maker::%s  \n Sequence::%s"%(self.name, self.seed, self.maker, self.sequence)


class MenuSequence():
    def __init__(self, name, recoSeqList,  Hypo, HypoToolClassName):
        from AthenaCommon.AlgSequence import AthSequencer
        self.name = name
        self.recoSeqList=recoSeqList
        self.hypoToolClassName = HypoToolClassName
        self.seeds=[seq.seed for seq in recoSeqList]
        self.outputs=[]
        if "Combo" in name: #TMP for combo
            self.hypo = ComboHypoAlgNode( Alg=Hypo )
            new_outputs = []
            outputs=self.hypo.getOutputList()
            iseq=0
            for seq in self.recoSeqList:
                new_outputs.append("%s_%s"%(seq.seed, outputs[iseq]))
            
            self.hypo.setOutputs(new_outputs)
            self.outputs=new_outputs
        
        elif len(self.recoSeqList) == 1:
            self.hypo = HypoAlgNode( Alg=Hypo)
            new_output = "%s_%s"%(self.recoSeqList[0].seed, self.hypo.getOutput())
            self.hypo.setOutput(new_output)
            self.outputs.append(new_output)


        
    def __str__(self):
        return "MenuSequence::%s \n Hypo::%s \n %s"%(self.name, self.hypo, ',\n '.join(map(str, self.recoSeqList)))

    
class CFSequence():
    def __init__(self, name, FilterAlg, MenuSequence):
        self.name = name        
        self.filter = FilterAlg
        self.menuSeq = MenuSequence
    
    def __str__(self):
        return "--- CFSequence %s ---\n + Filter: %s \n +  %s \n "%(self.name,\
            self.filter, \
            self.menuSeq)


class SequenceHypoTool:
    def __init__(self, MenuSequence, HypoTool):
        self.sequence = MenuSequence
        self.hypotool = HypoTool
        

class ChainStep:
     def __init__(self, name,  Sequences=[]):
        self.name = name        
        self.sequences = Sequences 

class Chain:
     def __init__(self, name, Seed, ChainSteps=[]):
         self.name = name
         self.seed=Seed
         self.steps=ChainSteps
         self.hypoToolName=name         
         seeds = Seed.strip().split("_")
         seeds.pop(0) #remove first L1
         self.group_seed  = ["L1"+filter(lambda x: x.isalpha(), stri) for stri in seeds]






