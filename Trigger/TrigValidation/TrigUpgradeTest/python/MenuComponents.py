##########################################################


class NodeSequence():
    def __init__(self, name, Algs, Hypo, Seed, Maker):
        self.name = name
        self.algs = Algs
        self.hypo = Hypo
        self.seed = Seed
        self.maker = Maker
        self.output = Hypo.getOutput()
        self.reuse = 0
        self.connect()
 
    def connect(self):
        self.maker.setInput("")
        alg_input = self.maker.getOutput() 
        for Alg in self.algs:
            Alg.setInput(alg_input) 
            alg_input = Alg.getOutput()
        self.hypo.setInput(Alg.getOutput())

        # add decisions: from InputMaker to Hypo
        sequence_outDecisions= "%s_decisions"%self.maker.algname
        self.addOutputDecision(sequence_outDecisions) #FPP
        self.hypo.setPreviousDecision(sequence_outDecisions) # works for one        
        print "connect NodeSequence %s"%self.name

    def addNode(self,node):
        self.algs.append(node)
        
    def setInput(self,theinput):
        self.maker.setInput(theinput)

    def addInput(self,theinput):
        print "adding input to sequence: %s to %s"%(theinput, self.maker.algname )
        self.maker.addInput(theinput)


    def addOutputDecision(self,input):
        print "adding output decisions (%s) to sequence %s  (to alg %s)"%(input, self.name, self.maker.algname )
        self.maker.setPar("OutputDecisions",input)
        
    def __str__(self):
        return "NodeSequence::%s with \n Seed::%s \n %s \n Hypo::%s"%(self.name, self.seed, ',\n '.join(map(str, self.algs)), self.hypo)

    
class ViewNodeSequence(NodeSequence):
    def __init__(self, name, Algs, Hypo, Seed, Maker, OtherNodes):
        NodeSequence.__init__(self, name, Algs, Hypo, Seed,Maker)
        self.viewNodeName = self.maker.Alg.ViewNodeName
        self.otherNodes=OtherNodes
        
        
    
class MenuSequence():
    def __init__(self, name, nodeSeqList):
        self.name = name
        self.nodeSeqList=nodeSeqList
        self.outputs=[seq.output for seq in nodeSeqList]
        self.seeds=[seq.seed for seq in nodeSeqList]

    def __str__(self):
        return "MenuSequence::%s \n %s"%(self.name,',\n '.join(map(str, self.nodeSeqList)))


class CFSequence():
    def __init__(self, name, FilterAlg, MenuSequence):
        self.name = name        
        self.filter = FilterAlg
        self.menuSeq = MenuSequence
        self.otherNodes = []

    def addNode(self, node):
        self.otherNodes.append(node)
    
    def __str__(self):
        return "--- CFSequence %s ---\n + Filter: %s \n +  %s \n "%(self.name,\
            self.filter, \
            self.menuSeq)

    def getAllNodes(self):
        nodes = []
        for seq in self.menuSeq.nodeSeqList:
            nodes.extend(seq.algs)
            nodes.append(seq.hypo)
        for node in self.otherNodes:
            nodes.append(node)
            
        return nodes
        

class SequenceHypoTool:
    def __init__(self, MenuSequence, HypoTool):
        self.sequence = MenuSequence
        self.hypotool = HypoTool
        

class ChainStep:
     def __init__(self, name,  SequenceHypoTools=[]):
        self.name = name        
        self.sequenceHypoTools = SequenceHypoTools
        self.sequences = [sh.sequence for sh in SequenceHypoTools ] 


class Chain:
     def __init__(self, name, Seed, ChainSteps=[]):
         self.name = name
         self.seed=Seed
         self.steps=ChainSteps
         
         seeds = Seed.strip().split("_")
         seeds.pop(0) #remove first L1
                      #         print "Chians seed: "%seeds
         self.group_seed  = ["L1"+filter(lambda x: x.isalpha(), stri) for stri in seeds]


