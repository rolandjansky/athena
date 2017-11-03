##########################################################


class NodeSequence():
    def __init__(self, name, InputMaker, Algs, Hypo, Seed):
        self.name = name
        self.inputMaker=InputMaker
        self.algs = Algs
        self.hypo = Hypo
        self.seed = Seed
        self.reuse = 0
        self.connect()

 
    def connect(self):
        alg_input = self.inputMaker.getOutput()
        for Alg in self.algs:
            Alg.setInput(alg_input) 
            alg_input = Alg.getOutput()
        self.hypo.setInput(Alg.getOutput())
        print "connect: %s"%self
            
    def __str__(self):
        return "NodeSequence %s with \n Seed::%s \n InputMaker::%s  \n %s \n Hypo::%s"%(self.name, self.seed, self.inputMaker, ', \n'.join(map(str, self.algs)), self.hypo)

class MenuSequence():
    def __init__(self, name, nodeSeqList):
        self.name = name
        self.nodeSeqList=nodeSeqList

    def __str__(self):
        return "MenuSequence::%s \n  %s"%(self.name,',\n '.join(map(str, self.nodeSeqList)))


class CFSeq:
    def __init__(self, name, FilterAlg, MenuSequence):
        self.name = name        
        self.filter = FilterAlg
        self.menuSeq = MenuSequence

    def __str__(self):
        return "--- CFSeq %s ---\n + Filter: %s \n +  %s \n "%(self.name,\
            self.filter, \
            self.menuSeq)

    def getAllNodes(self):
        algs = []
        for seq in self.menuSeq.nodeSeqList:
            algs.append(seq.inputMaker)
            algs.extend(seq.algs)
            algs.append(seq.hypo)
        return algs
        

class SequenceThreshold:
    def __init__(self, Sequence, Threshold):
        self.sequence = Sequence
        self.threshold = Threshold
        

class ChainStep:
     def __init__(self, name,  SequenceThresholds=[]):
        self.name = name        
        self.sequenceThresholds = SequenceThresholds


class Chain:
     def __init__(self, name, Seed, ChainSteps=[]):
         self.name = name
         self.seed=Seed
         self.steps=ChainSteps
         
         seeds = Seed.strip().split("_")
         seeds.pop(0) #remove first L1
         print seeds
         self.group_seed  = ["L1"+filter(lambda x: x.isalpha(), stri) for stri in seeds]


