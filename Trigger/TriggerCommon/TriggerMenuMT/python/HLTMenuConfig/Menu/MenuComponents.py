# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import sys, re, copy
from AthenaCommon.Logging import logging
from AthenaCommon.Constants import VERBOSE,INFO,DEBUG
log = logging.getLogger('MenuComponents')
log.setLevel( VERBOSE )
logLevel=DEBUG

from DecisionHandling.DecisionHandlingConf import RoRSeqFilter
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponentsNaming import CFNaming

class Node():
    """ base class representing one Alg + inputs + outputs, to be used to Draw dot diagrams and connect objects"""
    def __init__(self, Alg):
        self.name = ("%sNode")%( Alg.name())
        self.Alg=Alg
        self.inputs=[]
        self.outputs=[]
        
    def addOutput(self, name):
        self.outputs.append(name)

    def addInput(self, name):
        self.inputs.append(name)

    def getOutputList(self):        
        return self.outputs

    def getInputList(self):        
        return self.inputs

    def __str__(self):
        return "Node::%s  [%s] -> [%s]"%(self.Alg.name(), ' '.join(map(str, self.getInputList())), ' '.join(map(str, self.getOutputList())))


    
class AlgNode(Node):
    """Node class that connects inputs and outputs to basic alg. properties """
    def __init__(self, Alg, inputProp, outputProp):
        Node.__init__(self, Alg)
        self.outputProp=outputProp
        self.inputProp=inputProp

    def addDefaultOutput(self):
        if self.outputProp is not '':
            self.addOutput(("%s_%s"%(self.Alg.name(),self.outputProp)))
        
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

    def getPar(self, prop):
        try:
            return getattr(self.Alg, prop)
        except:
            return self.Alg.getDefaultProperty(prop)
        raise "Error in reading property " + prop + " from " + self.Alg

    def addOutput(self, name):
        outputs = self.readOutputList()
        if name in outputs:
            log.debug("Warning, %s already set in %s, DH not added",name, self.name)
        else:
            if self.outputProp is not '':
                self.setPar(self.outputProp,name)
            else:
                sys.exit("no OutputProp set")
        Node.addOutput(self, name)
       

    def readOutputList(self):
        outputs = []
        cval = self.getPar(self.outputProp)
        if cval == '':
            return outputs
        if type(cval) == type(list()):  
            outputs.extend(cval)
        else:
            outputs.append(cval)
        return outputs
  
    def addInput(self, name):
        inputs = self.readInputList()
        if name in inputs:
            log.debug("Warning, %s already set in %s, DH not added",name, self.name)
        else:
            if self.inputProp is not '':
                self.setPar(self.inputProp,name)
            else:
                sys.exit("no InputProp set")
        Node.addInput(self, name)


    def readInputList(self):
        inputs = []
        cval = self.getPar(self.inputProp)
        if cval =='':
            return inputs
        if type(cval) == type(list()):  
            inputs.extend(cval)
        else:
            inputs.append(cval)
        return inputs

    def __str__(self):
        return "Alg::%s  [%s] -> [%s]"%(self.Alg.name(), ' '.join(map(str, self.getInputList())), ' '.join(map(str, self.getOutputList())))
    

class HypoToolConf():
    """ Class to group info on hypotools"""
    def __init__(self, hypoToolGen):
        self.hypoToolGen = hypoToolGen
        self.name=''
        self.conf=''

    def setName(self, name):
        self.name=name

    def setConf(self, conf):
        self.conf=conf

    def create(self):
        """creates instance of the hypo tool"""
        return self.hypoToolGen( self.name, self.conf )

        
class HypoAlgNode(AlgNode):
    """Node for HypoAlgs"""
    def __init__(self, Alg):
        assert isHypoBase(Alg), "Error in creating HypoAlgNode from Alg "  + Alg.name()
        AlgNode.__init__(self, Alg, 'HypoInputDecisions', 'HypoOutputDecisions')
        self.tools = []
        self.previous=[]

    def addHypoTool (self, hypoToolConf):
        
        if hypoToolConf.name not in self.tools:
            ## HypoTools are private, so need to be created when added to the Alg
            ## this incantation may seem strange, however it is the only one which works
            ## trying tool = hypoToolConf.create() and then assignement does not work! will be no problem in run3 config
            tools = self.Alg.HypoTools
            self.Alg.HypoTools = tools+[hypoToolConf.create()]
            self.tools.append( hypoToolConf.name )
        else:
            raise RuntimeError("The hypo tool of name "+ hypoToolConf.name +" already present")
           

    def setPreviousDecision(self,prev):        
        self.previous.append(prev)
        return self.addInput(prev)

    def __str__(self):
        return "HypoAlg::%s  [%s] -> [%s], previous = [%s], HypoTools=[%s]"%(self.Alg.name(),' '.join(map(str, self.getInputList())),
                                                                                 ' '.join(map(str, self.getOutputList())),
                                                                                 ' '.join(map(str, self.previous)),
                                                                                 ' '.join(map(str, self.tools)))




class SequenceFilterNode(AlgNode):
    """Node for any kind of sequence filter"""
    def __init__(self, Alg, inputProp, outputProp):
        AlgNode.__init__(self,  Alg, inputProp, outputProp)
              
    def setChains(self, name):
        log.debug("Adding Chain %s to filter %s"%(name, self.Alg.name()))
        return self.setPar("Chains", name)
    
    def getChains(self):
        return self.getPar("Chains")

    def __str__(self):
        return "SequenceFilter::%s  [%s] -> [%s], chains=%s"%(self.Alg.name(),' '.join(map(str, self.getInputList())),' '.join(map(str, self.getOutputList())), self.getChains())


class RoRSequenceFilterNode(SequenceFilterNode):
    def __init__(self, name):
        Alg= RoRSeqFilter(name, OutputLevel = logLevel)
        SequenceFilterNode.__init__(self,  Alg, 'Input', 'Output')



class InputMakerNode(AlgNode):
    def __init__(self, Alg):
        assert isInputMakerBase(Alg), "Error in creating InputMakerNode from Alg "  + Alg.name()
        AlgNode.__init__(self,  Alg, 'InputMakerInputDecisions', 'InputMakerOutputDecisions')
        Alg.OutputLevel = logLevel


from DecisionHandling.DecisionHandlingConf import ComboHypo
class ComboMaker(AlgNode):
    def __init__(self, name):
        Alg = ComboHypo(name)
        log.debug("Making combo Alg %s"%name)
        AlgNode.__init__(self,  Alg, 'HypoInputDecisions', 'HypoOutputDecisions')
        Alg.OutputLevel = logLevel
        self.prop="MultiplicitiesMap"


    def addChain(self, chain):
        log.debug("ComboMaker %s adding chain %s"%(self.Alg.name(),chain))
        from TriggerMenuMT.HLTMenuConfig.Menu.MenuChains import getConfFromChainName
        confs=getConfFromChainName(chain)
        for conf in confs:
            seed=conf.replace("HLT_", "")
            integers = map(int, re.findall(r'^\d+', seed))
            multi=0
            if len(integers)== 0:
                multi=1
            elif len(integers)==1:
                multi=integers[0]
                re.sub('^\d+',"",seed) #remove the multiplicity form the string
            else:
                sys.exit("ERROR in decoding combined chain %s"%(chain))

            newdict={chain:[multi]}
            cval = self.Alg.getProperties()[self.prop]            
            if type(cval) == type(dict()):
                if chain in cval.keys():
                    cval[chain].append(multi)
                else:
                    cval[chain]=[multi]
            else:
                cval=newdict
            setattr(self.Alg, self.prop, cval)
            


#########################################################
# USEFULL TOOLS
#########################################################

def isHypoBase(alg):
    if  'HypoInputDecisions'  in alg.__class__.__dict__:
        return True
    prop = alg.__class__.__dict__.get('_properties')
    return  ('HypoInputDecisions'  in prop)

def isInputMakerBase(alg):
    return  ('InputMakerInputDecisions'  in alg.__class__.__dict__)

def isFilterAlg(alg):
    return isinstance(alg, RoRSeqFilter)


class WrappedList:
    def __init__(self, lst):
        self._lst = lst
    def __getitem__(self, item):
        return self._lst[item]
    def __setitem__(self, item):
        return self._lst.append(item)

    
#class duplicatedHypos(WrappedList):

## allAlgs={}
## def useExisting(name):
##     global allAlgs
##     return allAlgs[name]

## def AlgExisting(name):
##     global allAlgs
##     return name in allAlgs


## def remember(name, instance):
##     global allAlgs
##     allAlgs[name] = instance
##     return instance

        
##########################################################
# NOW sequences and chains
##########################################################

       
class MenuSequence():
    """ Class to group reco sequences with the Hypo"""
    def __init__(self, Sequence, Maker,  Hypo, HypoToolGen ):
        from AthenaCommon.AlgSequence import AthSequencer
        self.name = CFNaming.menuSequenceName(Hypo.name())
        self.sequence     = Node( Alg=Sequence)
        self.maker        = InputMakerNode( Alg = Maker )
        self.hypoToolConf = HypoToolConf( HypoToolGen )
        self.hypo         = HypoAlgNode( Alg = Hypo )
        self.inputs=[]
        self.outputs=[]
        self.seed=''
        self.reuse = False # flag to draw dot diagrmas

    def replaceHypoForCombo(self, HypoAlg):
        log.debug("set new Hypo %s for combo sequence %s "%(HypoAlg.name(), self.name))
        self.hypo= HypoAlgNode( Alg=HypoAlg )
    
    def connectToFilter(self, outfilter):
        """ Sets the input and output of the hypo, and links to the input maker """

        #### Connect filter to the InputMaker
        self.maker.addInput(outfilter)       
        input_maker_output = CFNaming.inputMakerOutName(self.maker.Alg.name(),outfilter)
        self.maker.addOutput(input_maker_output)       

        #### Add input/output Decision to Hypo
        self.hypo.setPreviousDecision( input_maker_output)
        hypo_output = CFNaming.hypoAlgOutName(self.hypo.Alg.name(), input_maker_output)
        if len(self.hypo.getOutputList()):
            log.error("Hypo " + self.hypo.name() +" has already an output configured: you may want to duplicate the Hypo!")
            sys.exit("ERROR, in chain configuration") 
        self.hypo.addOutput(hypo_output)

        # needed for drawing
        self.inputs.append(outfilter)
        self.outputs.append(hypo_output)

       
        log.debug("MenuSequence.connectToFilter: connecting InputMaker and HypoAlg, adding: \n\
        InputMaker::%s.output=%s, \n\
        HypoAlg::%s.previousDecision=%s, \n\
        HypoAlg::%s.output=%s",\
                self.maker.Alg.name(), input_maker_output, self.hypo.Alg.name(), input_maker_output, self.hypo.Alg.name(), hypo_output )
        
    def __str__(self):
        return "MenuSequence::%s \n Hypo::%s \n Maker::%s \n Sequence::%s"%(self.name, self.hypo, self.maker, self.sequence)


#################################################
#### CONFIGURATION FOR L1DECODER
#################################################
## It might be moved somewhere in the cofiguration later one
# This is amp between the L1 items and the name of teh Decisions in the L1Decoder unpacking tools
def DoMapSeedToL1Decoder(seed):
    mapSeedToL1Decoder = {  "EM" : "L1EM",
                            "MU" : "L1MU",
                            "J"  : "L1J",
                            "TAU": "L1TAU",
                            "XE" : "L1MET",
                            "XS" : "L1MET",
                            "TE" : "L1MET"}

    # remove actual threshold value from L1 seed string
    stripSeed  = filter(lambda x: x.isalpha(), seed)
    if stripSeed not in mapSeedToL1Decoder:
        log.error("Seed "+ seed + " not mapped to any Decision objects! Available are: " + str(mapSeedToL1Decoder.values()))
        sys.exit("ERROR, in chain configuration") 
    return (mapSeedToL1Decoder[stripSeed])   

#################################################

class Chain:
    """Basic class to define the trigger menu """
    def __init__(self, name, Seed, ChainSteps=[]):
        self.name = name
        self.steps=ChainSteps         
        self.seed=Seed
        self.vseeds=[]
        vseeds = Seed.strip().split("_")        
        vseeds.pop(0) #remove first L1 string
        # split multi seeds
        for seed in vseeds:
            split=re.findall(r"(\d+)?([A-Z]+\d+)", seed)
            mult,single = split[0]
            if not mult: mult=1
            else: mult=int(mult)
#            print mult,single
            for m in range(0,mult): self.vseeds.append(single) 
            
        # group_seed is used to se tthe seed type (EM, MU,JET), removing the actual threshold
        # in practice it is the L1Decoder Decision output
        self.group_seed = [DoMapSeedToL1Decoder(stri) for stri in self.vseeds]
        self.setSeedsToSequences() # save seed of each menuseq
        log.debug("Chain " + name + " with seeds: %s "%str( self.vseeds))

        for step in self.steps:
            if step.isCombo:
                step.combo.addChain(self.name)

        
    def setSeedsToSequences(self):
        # set the seed to the menusequences
        sequences1=self.steps[0].sequences
        tot_seq=len(sequences1)
        tot_seed=len(self.vseeds)
        if tot_seq==tot_seed:
            nseq=0
            for seed in self.vseeds:
                for step in self.steps:
                    seq=step.sequences[nseq]
                    seq.seed ="L1"+filter(lambda x: x.isalpha(), seed)
                    log.debug( "Chain %s adding seed %s to sequence %d in step %s"%(self.name, seq.seed, nseq, step.name))
                nseq+=1
           
        else:
            log.error("found %d sequences in this chain and %d seeds. What to do??", tot_seq, tot_seed)
            sys.exit("ERROR, in chain configuration") 
        
    def decodeHypoToolConfs(self):
        """ This is extrapolating the hypotool configuration from the (combined) chain name"""
        from TriggerMenuMT.HLTMenuConfig.Menu.MenuChains import getConfFromChainName
        signatures = getConfFromChainName(self.name) #currently a lis of chainPart names
        for step in self.steps:
            if len(signatures) != len(step.sequences):
                log.error("Error in step %s: found %d signatures and %d sequences"%(step.name, len(signatures), len(step.sequences)))
                sys.exit("ERROR, in chain configuration")
            nseq=0
            for seq in step.sequences:
                seq.hypoToolConf.setConf(signatures[nseq])
                seq.hypoToolConf.setName(self.name)
                seq.hypo.addHypoTool(seq.hypoToolConf)
                nseq +=1
                

class CFSequence():
    """Class to describe the ChainStep + filter
   
    """
    def __init__(self, ChainStep, FilterAlg):
        self.filter = FilterAlg
        self.step = ChainStep
        self.connect()

    def connect(self):
        """Connect filter to ChainStep (all its sequences)
        if a ChainStep contains the same sequence multiple times (for multi-object chains), 
        the filter is connected only once (to avoid multiple DH links)
        """

        log.debug("CFSequence: Connect Filter %s with menuSequences of step %s"%(self.filter.Alg.name(), self.step.name))
        filter_output = self.filter.getOutputList()
        if len(filter_output) == 0:
            log.error("ERROR, no filter outputs are set!")
            sys.exit("ERROR, no filter outputs are set!")

        # check whether the number of filter outputs are the same as the number of sequences in the step
        if len(filter_output) != len(self.step.sequences):
            log.error("Found %d filter outputs and %d MenuSequences in Step %s"%( len(self.filter.getOutputList()), len(self.step.sequences), self.step.name))
            sys.exit("ERROR: Found %d filter outputs differnt from %d MenuSequences in Step %s"%( len(self.filter.getOutputList()), len(self.step.sequences), self.step.name))
                        
        
        nseq=0
        for seq in self.step.sequences:
            filter_out = filter_output[nseq]
            log.debug("Found input %s to sequence::%s from Filter::%s (from seed %s)", filter_out, seq.name, self.filter.Alg.name(), seq.seed)
            seq.connectToFilter( filter_out )
            #seq.connectToFilter(self.filter, filter_out )
            nseq+=1
            

        if self.step.isCombo: self.connectCombo()

    def connectCombo(self):
        # reset sequence output, they will b ereplaced by new combo outputs
        for seq in self.step.sequences:
            seq.outputs=[]
            
        for seq in self.step.sequences:
            combo_input=seq.hypo.getOutputList()[0]
            self.step.combo.addInput(combo_input)
            log.debug("Adding inputs %s to combo %s"%(combo_input, self.step.combo.Alg.name()))
            # inputs are the output decisions of the hypos of the sequences
            # outputs are the modified name of input deciions that need to be connected to the next filter
            combo_output="combo_%s"%combo_input
            self.step.combo.addOutput(combo_output)
            seq.outputs.append(combo_output)
            log.debug("Adding outputs %s to combo %s"%(combo_output, self.step.combo.Alg.name()))
        
    
    def __str__(self):
        return "--- CFSequence %s ---\n + Filter: %s \n +  %s \n "%(self.name,\
            self.filter, self.step )
          

class ChainStep:
    """Class to describe one step of a chain; if more than one menuSequence, then the step is combo"""
    def __init__(self, name,  Sequences=[]):
        self.name = name
        self.sequences=[]
        self.isCombo=len(Sequences)>1
        self.combo=None
        if self.isCombo:            
            self.makeCombo(Sequences)
        else:
            self.sequences = Sequences          

    def makeCombo(self, Sequences):        
        # For combo sequences, duplicate the sequence, the Hypo with differnt names and create the ComboHypoAlg
        self.combo = ComboMaker(CFNaming.comboHypoName(self.name))
        duplicatedHypos = []
        for sequence in Sequences:         
            oldhypo=sequence.hypo.Alg
            duplicatedHypos.append(oldhypo.name())
            ncopy=duplicatedHypos.count(oldhypo.name())
               
            new_sequence=copy.deepcopy(sequence)
            new_sequence.name = CFNaming.comboSequenceCopyName(sequence.name,ncopy, self.name)
            
            newHypoAlgName = CFNaming.comboHypoCopyName(oldhypo.name(),ncopy, self.name)
            new_hypoAlg=oldhypo.clone(newHypoAlgName)
            new_sequence.replaceHypoForCombo(new_hypoAlg)
            self.sequences.append(new_sequence)            

    def __str__(self):
        return "--- ChainStep %s ---\n + isCombo: %d \n +  %s \n "%(self.name, self.isCombo, ' '.join(map(str, self.sequences) ))



# this is fragment for New JO

    
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
class InEventReco( ComponentAccumulator ):
    """ Class to handle in-event reco """
    pass


class InViewReco( ComponentAccumulator ):
    """ Class to handle in-view reco, sets up the View maker if not provided and exposes InputMaker so that more inputs to it can be added in the process of assembling the menu """
    def __init__(self, name, viewMaker=None):
        super( InViewReco, self ).__init__()
        self.name = name
        from AthenaCommon.CFElements import parOR, seqAND
        self.mainSeq = seqAND( name )
        self.addSequence( self.mainSeq )

        from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
        if viewMaker:
            self.viewMakerAlg = viewMaker 
        else:
            from AthenaCommon.Constants import DEBUG
            self.viewMakerAlg = EventViewCreatorAlgorithm(name+'ViewsMaker',
                                                          ViewFallThrough = True,
                                                          RoIsLink        = 'initialRoI', # -||-
                                                          InViewRoIs      = name+'RoIs',
                                                          Views           = name+'Views',
                                                          ViewNodeName    = name+"InView",
                                                          OutputLevel=DEBUG)

        self.addEventAlgo( self.viewMakerAlg, self.mainSeq.name() )
        self.viewsSeq = parOR( self.viewMakerAlg.ViewNodeName )        
        self.addSequence( self.viewsSeq, self.mainSeq.name() )
        
    def addInputFromFilter(self, filterAlg ):
        assert len(filterAlg.Output) == 1, "Can only oprate on filter algs with one configured output, use addInput to setup specific inputs"
        self.addInput( filterAlg.Output[0], "Reco_"+( filterAlg.Output[0].replace("Filtered_", "") ) )

    def addInput(self, inKey, outKey ):
        """Adds input (DecisionsContainer) from which the views should be created """
        self.viewMakerAlg.InputMakerInputDecisions += [ inKey ]
        self.viewMakerAlg.InputMakerOutputDecisions += [ outKey ]

    def mergeReco( self, ca ):
        """ Merged CA movnig reconstruction algorithms into the right sequence """
        return self.merge( ca, sequenceName=self.viewsSeq.getName() )

    def addRecoAlg( self, alg ):
        """Reconstruction alg to be run per view"""
        log.warning( "InViewReco.addRecoAlgo: consider using mergeReco that takes care of the CA accumulation and moving algorithms" )
        self.addEventAlgo( alg, self.viewsSeq.name() )

    def sequence( self ):
        return self.mainSeq

    def inputMaker( self ):
        return self.viewMakerAlg 


class RecoFragmentsPool:
    """ Class to host all the reco fragments that need to be reused """
    fragments = {}
    @classmethod
    def retrieve( cls,  creator, flags, **kwargs ):
        """ create, or return created earlier reco fragment

        Reco fragment is uniquelly identified by the function and set og **kwargs. 
        The flags are not part of unique identifier as creation of new reco fragments should not be caused by difference in the unrelated flags.
        TODO, if that code survives migration to New JO we need to handle the case when the creator is an inner function
        """
        from AthenaCommon.Logging import logging
        requestHash = hash( ( creator, tuple(kwargs.keys()), tuple(kwargs.values()) ) )
        if requestHash not in cls.fragments:
            recoFragment = creator( flags, **kwargs )
            cls.fragments[requestHash] = recoFragment
            log.debug( "created reconstruction fragment using function: %s" % creator.func_name )
            return recoFragment
        else:
            log.debug( "reconstruction fragment that would be created from %s is taken from the cache" % creator.func_name )
            return cls.fragments[requestHash]

           
class NJMenuSequence( ComponentAccumulator ):
    def __init__( self, name ):
        super( NJMenuSequence, self ).__init__()
        from AthenaCommon.CFElements import seqAND
        self.name = name
        self.mainSeq = seqAND( name )
        self.addSequence( self.mainSeq )
        self.filterAlg =  None
        self.reco = None
        self.hypoAlg = None

    def addReco( self, reco ):
        """ Adds reconstruction sequence, has to be one of InViewReco or InEventReco objects """
        assert self.filterAlg, "The filter alg has to be setup first"
        self.reco = reco
        reco.addInputFromFilter( self.filterAlg )
        self.addSequence( reco.sequence(), self.mainSeq.name() )
        self.merge( reco )

    def addFilter( self, chains, inKey ):
        """ adds filter alg of type RoRSeqFilter, output key is predefined by the sequence name as: Filtered + the name"""
        from DecisionHandling.DecisionHandlingConf import RoRSeqFilter
        self.filterAlg = RoRSeqFilter( self.name+'Filter' )
        self.filterAlg.Input       = [ inKey ]
        self.filterAlg.Output      = [ 'Filtered_'+self.name ]
        self.filterAlg.Chains      = chains
        from AthenaCommon.Constants import DEBUG
        self.filterAlg.OutputLevel = DEBUG
        self.addEventAlgo( self.filterAlg, self.mainSeq.name() )

    def hypoDecisions( self ):
        return "Decisions_"+self.name

    def addHypo( self, hypo ):
        self.hypoAlg = hypo
        self.hypoAlg.HypoInputDecisions = self.reco.inputMaker().InputMakerOutputDecisions[-1]
        self.hypoAlg.HypoOutputDecisions = self.hypoDecisions()
        self.addEventAlgo( hypo, self.mainSeq.name() )

    def sequence( self ):
        return self.mainSeq

    def filter( self ):
        return self.filterAlg

    def check( self ):
        assert self.filterAlg, "Filter not configured"
        assert self.hypoAlg, "Hypo alg is not configured"


class HLTMenuAccumulator( ComponentAccumulator ):
    def __init__( self ):
        super( HLTMenuAccumulator, self ).__init__()
        from AthenaCommon.CFElements import seqAND
        HLTSteps =  seqAND( "HLTSteps" )
        self.addSequence( HLTSteps )

    def __getOrMakeStepSequence(self, step, isFilter = False ):
       """ Constructs sequence for the step, the filtering step or, reco step will be created depending on isFilter flags
       
       The function assures that all previous steps are aready in place i.e. HLTStep_1_filter, HLTStep_1 ... until HLTStep_N-1 are in place.
       Therefore the steps can be added in any order (not yet sure if that will but better assure robustness now)
       """
       from AthenaCommon.CFElements import parOR
       name = "HLTStep_%d%s" % (step, "_filters" if isFilter else "" )
        
       s  = self.getSequence( name )
       if s:
           return s
       # make sure that sequences for previous steps are in place
       for p in range( 1, step ):
           self.__getOrMakeStepSequence( p, isFilter = True )
           self.__getOrMakeStepSequence( p, isFilter = False )
       # make sure that filtering steps is placed before reco steps
       if not isFilter:
           self.__getOrMakeStepSequence( step, isFilter = True )
       s = parOR( name )
       self.addSequence( s, parentName="HLTSteps")
       return s


    def setupSteps( self, steps ):        
        """ The main menu function, it is responsible for crateion of step sequences and placing slice specific sequencers there.
                
        It would rely on the MenuSeq object API in two aspects, to obtain filter alg and to obtain the reco sequence
        The MenuSeq should already contain all the chains that are needed for the menu setup (chains info can be accessed via flags passed when steps were created)
        """
        for stepNo, fhSeq in enumerate( steps, 1 ):
            filterStep = self.__getOrMakeStepSequence( stepNo, isFilter = True )
            filterStep += fhSeq.filter() # FilterHypoSequence API

            recoStep = self.__getOrMakeStepSequence( stepNo, isFilter = False )            
            self.addSequence( fhSeq.sequence(), recoStep.name() )

    
    def steps( self ):
        """ returns step seqeuncers """
        return self.getSequence("HLTSteps")


def getChainStepName(chainName, stepNumber):
    return '{}_step{}'.format(chainName, stepNumber)
