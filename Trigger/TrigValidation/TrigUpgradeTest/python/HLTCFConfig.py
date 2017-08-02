# Classes to configure the CF graph, via strings
    
class Sequence:
    def __init__(self, name, Hypo, Algs=[], Inputs=[], Outputs=[]):
        self.name = name
        self.algs_name = Algs
        self.hypo = Hypo
        self.inputs = Inputs
        self.outputs = Outputs
        

    def setInput(self, Inputs):
        self.inputs=Inputs

    def setOutput(self, Outputs):
        self.outputs=Outputs

class ChainStep:
     def __init__(self, name,  Sequence, Threshold):
        self.name = name        
        self.sequence = Sequence
        self.threshold = Threshold


class Step:
    def __init__(self, name, FilterAlg, RecoAlgs, Hypo):
        self.name = name        
        self.filter = FilterAlg
        self.algs = RecoAlgs
        self.hypo=Hypo
    def __str__(self):
        return "%s with filter %s and algs [%s], [%s]"%(self.name, self.filter, ', '.join(map(str, self.algs)), self.hypo)


class Chain:
     def __init__(self, name, Seed, ChainSteps=[]):
         self.name = name
         self.seed=Seed
         self.steps=ChainSteps
         #         self.group_seed=filter(lambda x: x.isalpha(), Seed)#saving only letters
         self.group_seed = Seed[0:4]
                
class Alg:
    def __init__(self, name, Inputs=[],  Outputs=[]):
        self.name = name
        self.inputs = Inputs
        self.outputs = Outputs
        #        print "Constructor%s with inputs [%s] and outputs [%s]"%(self.name,', '.join(map(str, self.inputs)),', '.join(map(str, self.outputs)))

    def __str__(self):
        return "%s with inputs [%s] and outputs [%s]"%(self.name,', '.join(map(str, self.inputs)),', '.join(map(str, self.outputs)))

    def addInput(self, Input):
        self.inputs.append(Input)

    def addOutput(self, Output):
        self.outputs.append(Output)
 

class RecoAlg(Alg):
    def __init__(self, name, Input, Output, FileName="noreco.dat"):
        Alg.__init__(self,name, Inputs=Input, Outputs=Output)
        self.filename=FileName

class HypoAlg(Alg):
    def __init__(self, name, Input, Output):
        Alg.__init__(self, name, Inputs=Input, Outputs=Output)
        self.tools = []

    def addHypoTool(self, name):
        self.tools.append(name)


class SequenceFilter(Alg):
    def __init__(self, name, Inputs=[], Outputs=[], Chains=[]):
        Alg.__init__(self,name, Inputs=Inputs, Outputs=Outputs)
        self.chains=Chains
        
       
    def addChain(self, Chain):
        self.chains.append(Chain)

    def __str__(self):
        return "%s with inputs [%s] and outputs [%s], chains = [%s]"%(self.name,', '.join(map(str, self.inputs)),', '.join(map(str, self.outputs)),', '.join(map(str, self.chains)))


class InputMaker(Alg):
    def __init__(self, name, Input, Output):
        Alg.__init__(self, name, Inputs=Input, Outputs=Output)

        
        
