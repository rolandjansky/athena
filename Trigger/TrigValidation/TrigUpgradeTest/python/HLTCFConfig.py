# Classes to configure the CF graph, via strings


class Alg:
    def __init__(self, name): 
        self.name = name
        self.inputs = []
        self.outputs = []      

    def __str__(self):
        return "%s with inputs [%s] and outputs [%s]"%(self.name,', '.join(map(str, self.inputs)),', '.join(map(str, self.outputs)))

    def addInput(self, Input):
        self.inputs.append(Input)
        self.inputs=list(set(self.inputs))

    def addOutput(self, Output):
        self.outputs.append(Output)
 

class RecoAlg(Alg):
    def __init__(self, name,  FileName="noreco.dat"): 
        Alg.__init__(self,name)
        self.filename=FileName

class HypoAlg(Alg):
    def __init__(self, name):
        Alg.__init__(self, name) 
        self.tools = []

    def addHypoTool(self, hypotool):
        self.tools.append(hypotool)

class HypoTool:
    def __init__(self, name):
        self.name=name
    
class RecoSequence:
    def __init__(self, name, Seed,Reuse=False,Algs=[]):
        self.name = name
        self.algs = Algs
        self.seed=Seed
        self.reuse=Reuse
    def __str__(self):
        return "RecoSequence %s with algs [%s] and seed [%s]"%(self.name,', '.join(map(str, self.algs)), self.seed)

    
class Sequence:
    def __init__(self, name, Hypo, RecoSeq=[]):
        self.name = name
        self.recoSeq=RecoSeq
        self.algs = []
        self.algs.extend(seq.algs for seq in RecoSeq)
        self.hypo = Hypo
       
class CFRecoSequence:
    def __init__(self, name, RecoSequence, InputMaker):
        self.name = name
        self.recoSequence=RecoSequence        
        self.inputMaker=InputMaker
        self.outputs=self.recoSequence.algs[-1].outputs # last alg output
    def __str__(self):
        return "CFRecoSequence %s is %d, with RecoSequence [%s] and inputMaker [%s]"%(self.name,self.recoSequence.reuse,self.recoSequence, self.inputMaker)


class SequenceThreshold:
    def __init__(self, Sequence, Threshold):
        self.sequence = Sequence
        self.threshold = Threshold

class ChainStep:
     def __init__(self, name,  SequenceThresholds=[]):
        self.name = name        
        self.sequenceThresholds = SequenceThresholds



class Step:
    def __init__(self, name, FilterAlg, Hypo, CFRecoSequences=[]):
        self.name = name        
        self.filter = FilterAlg
        self.cfRecoSequences = CFRecoSequences
        self.hypo=Hypo
    def __str__(self):
        return "--- STEP %s ---\n + Filter: %s \n + CFRecoSequence: %s \n + Hypo: %s \n"%(self.name,\
            self.filter, \
            '\n '.join(map(str, self.cfRecoSequences)),\
            self.hypo)

    def getAllAlgs(self):
        algs = []
        algs.append(self.filter)
        algs.extend(seq.recoSequence.algs for seq in self.cfRecoSequences)
        algs.append(self.hypo)
        return algs
        

class Chain:
     def __init__(self, name, Seed, ChainSteps=[]):
         self.name = name
         self.seed=Seed
         self.steps=ChainSteps
         
         seeds = Seed.strip().split("_")
         seeds.pop(0) #remove first L1
         print seeds
         self.group_seed  = ["L1"+filter(lambda x: x.isalpha(), stri) for stri in seeds]


class SequenceFilter(Alg):
    def __init__(self, name): 
        Alg.__init__(self,name)
        self.chains=[] 
               
    def addChain(self, Chain):
        self.chains.append(Chain)

    def __str__(self):
        return "%s with inputs [%s] and outputs [%s], chains = [%s]"%(self.name,', '.join(map(str, self.inputs)),', '.join(map(str, self.outputs)),', '.join(map(str, self.chains)))


class InputMaker(Alg):
    def __init__(self, name): 
        Alg.__init__(self, name) 

        

