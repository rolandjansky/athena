##########################################################


class NodeSequence():
    def __init__(self, name, Algs, Hypo, Seed):
        self.name = name
        self.algs = Algs
        self.hypo = Hypo
        self.seed = Seed
        self.output = Hypo.getOutput()
        self.reuse = 0
        self.connect()

 
    def connect(self):
        alg_input = ""
        #alg_input = self.inputMaker.getOutput()
        for Alg in self.algs:
            Alg.setInput(alg_input) 
            alg_input = Alg.getOutput()
        self.hypo.setInput(Alg.getOutput())
        print "connect NodeSequence %s"%self.name

    def setInput(self,input):
        self.algs[0].setInput(input)

    def addInput(self,input):
        self.algs[0].addInput(input)
        
    def __str__(self):
        return "NodeSequence::%s with \n Seed::%s \n %s \n Hypo::%s"%(self.name, self.seed, ',\n '.join(map(str, self.algs)), self.hypo)

class MenuSequence():
    def __init__(self, name, nodeSeqList):
        self.name = name
        self.nodeSeqList=nodeSeqList
        self.outputs=[seq.output for seq in nodeSeqList]

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


