# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""
                                                                     
JetDefinition: A module for classes encoding definitions of jets and 
related objects for configuring jet reconstruction                   

Various classes encode definitions of different types of components used in Jet Reco.
They are : 

 - JetInputExternal : describes how to build a source container, typically external to the jet domain. This includes input to jet finding  (ex: CaloCluster, Track) but also other sources like EventDensity...

 - JetInputConstit :  describes specifically an input constituents container (an input to a PseudoJetAlgorithm), thus referring to a JetInputExternal as the primary source.
 - JetInputConstitSeq : a subclass of JetInputConstit, describing how a constituents container is build from a JetConstituentModSequence (ex: PU or Origin correction).
 - JetConstitModifier : describes a constituent modifier tool to be used in a JetConstituentModSequence

 - JetDefinition : describes a full jet reco sequence. Uses a JetInputConstit and a list of JetModifier
 - JetModifier : describes a JetModifier c++ tool. 

Author: TJ Khoo, P-A Delsart                                         
                                                                     
"""

__all__ =  [  "JetDefinition","xAODType", "JetModifier", "JetConstitModifier" , "JetInputConstitSeq", "JetInputExternal"] 

from AthenaCommon import Logging
jetlog = Logging.logging.getLogger('JetDefinition')

from xAODBase.xAODType import xAODType
from .Utilities import make_lproperty, onlyAttributesAreProperties, clonable, make_alias

def formatRvalue(parameter):
    """Define the convention that we write R truncating the decimal point
    if R>=1, then we write R*10.
    (Code from JetRecUtils )
    """
    # impose precision limits where there could be ambiguity
    if int(10*parameter)>=1 and int(100*parameter % 10):
        #jetlog.warning('Radius parameter {0} exceeds allowable precision of 0.1'.format(parameter))
        raise ValueError('Bad radius parameter')
    if int(parameter)>=1:
        return "{0:.0f}".format(10*parameter)
    else:
        return "{0:.1g}".format(10*parameter).replace('.','')


# Could also split off a VR name builder
def buildJetAlgName(finder, mainParam,
                    variableRMassScale= -1.0, variableRMinRadius=-1.0):
    """variableRMassScale (Rho) in MeV """
    if ( variableRMassScale >= 0.0 and variableRMinRadius >= 0.0):
        rmaxstr = formatRvalue(mainParam)
        rminstr = formatRvalue(variableRMinRadius)
        return f"{finder}VR{str(int(variableRMassScale/1000))}Rmax{rmaxstr}Rmin{rminstr}"
    return finder + formatRvalue(mainParam)



def _condAlwaysPass(condflags):
    return True,""

from AthenaCommon.SystemOfUnits import MeV

@clonable
@onlyAttributesAreProperties
class JetDefinition(object):
    _allowedattributes = ['_cflags'] # onlyAttributesAreProperties will add all properties to this list.
    def __init__(self,
                 algorithm,           # The fastjet clustering algorithm
                 radius,              # The jet radius specifier (clustering cutoff)
                 inputdef,            # The input JetConstit
                 ptmin=5e3*MeV,       # The pt cutoff for fastjet in MeV
                 ghostdefs=[],        # The list of alias to JetGhosts to ghost-associate
                 modifiers=[],        # The list of alias to JetModifiers to execute after jet finding
                 extrainputs=[],      # The list of additional input types needed for jet finding
                 standardRecoMode = False, # 
                 prefix = "",         # allows to tune the full JetContainer name
                 suffix = "",         # allows to tune the full JetContainer name
                 context = "default", # describe a context for which this definition will be used. See StandardJetContext
                 VRMinR = -1.0, # Minimum radius for VR jet finding
                 VRMassSc = -1.0, # Mass scale for VR jet finding, in MeV
                 lock = False,        # lock the properties of this instance to avoid accidental overwrite after __init__
    ):     

        self._locked = False # unlock during init
        # Should add some type checking here
        # Could use JetContainerInfo conversion
        if algorithm not in ["Kt","AntiKt","CamKt"]:
            jetlog.error("FastJet algorithm specification was not one of Kt, AntiKt, CamKt!")
            raise KeyError("Invalid fastjet algorithm choice: {0}".format(self.algorithm))
        self._algorithm = algorithm

        self._radius = radius
        self._inputdef = inputdef
        self._prefix = prefix
        self._suffix = suffix
        self._context = context
        self._VRMinRadius = VRMinR
        self._VRMassScale = VRMassSc
        self._defineName()
        
        self.ptmin = ptmin # The pt down to which FastJet is run

        self.ghostdefs = ghostdefs     # Objects to ghost-associate
        self.modifiers = modifiers     # Tools to modify the jet
        self.extrainputs = extrainputs # Any extra input dependencies

        self.standardRecoMode = standardRecoMode
        
        # used internally to resolve dependencies
        self._prereqDic = {}
        self._prereqOrder = [] 
        self._internalAtt = {}
        self._cflags = None # pointer to AthenaConfiguration.ConfigFlags. Mainly to allow to invoke building of input dependencies which are outside Jet domain during std reco
        self._locked = lock

            
    def __hash__(self):
        return hash((self._radius,self._inputdef,self.ptmin,str(self.ghostdefs),str(self.modifiers),str(self.extrainputs)))

    def __eq__(self,rhs):
        return self.__hash__() == rhs.__hash__()

    def __ne__(self,rhs):
        return (not self.__eq__(rhs))

    # Define core attributes as properties, with
    # custom setter/getter such that if changed, these
    # force resetting of the jet name
    @make_lproperty
    def algorithm(self): pass

    @algorithm.lsetter
    def algorithm(self,algorithm):
        self._algorithm = algorithm
        self._defineName()

    @make_lproperty
    def radius(self): pass

    @radius.lsetter
    def radius(self,radius):
        self._radius = radius
        self._defineName()

    @make_lproperty
    def inputdef(self): pass
    
    @inputdef.lsetter
    def inputdef(self,inputdef):
        self._inputdef = inputdef
        self._defineName()

    @make_lproperty
    def prefix(self): pass
    
    @make_lproperty
    def suffix(self): pass
    
    @make_lproperty
    def basename(self): pass

    @basename.lsetter
    def basename(self,v):
        raise Exception("Can NOT set property basename of JetDefinition ",self," Change prefix or suffix instead.")
        

    @make_lproperty
    def ghostdefs(self): pass
    @make_lproperty
    def modifiers(self): pass
    @make_lproperty
    def extrainputs(self): pass
    @make_lproperty
    def standardRecoMode(self): pass

    @make_lproperty
    def VRMinRadius(self): pass
    @make_lproperty
    def VRMassScale(self): pass

    @make_lproperty
    def context(self): pass
    

    def fullname(self):
        return self.prefix+self.basename+"Jets"+self.suffix
        
    def _defineName(self):
        self._basename = buildJetAlgName(self.algorithm,self.radius,self.VRMassScale,self.VRMinRadius)+self.inputdef.label # .label
        if self.inputdef.basetype == xAODType.CaloCluster:
            # Omit cluster origin correction from jet name
            # Keep the origin correction explicit because sometimes we may not
            # wish to apply it, whereas PFlow corrections are applied implicitly
            self._basename = self.basename.replace("Origin","")
        pass

    # Define a string conversion for printing
    def __str__(self):
        return f"JetDefinition({self.fullname()})"
    # Need to override __repr__ for printing in lists etc
    __repr__ = __str__


########################################################################

@clonable
@onlyAttributesAreProperties
class JetModifier(object):
    """Helper to define the config of a IJetModifier tool.
    Tools that typically have more complex properties set should have
    their own dedicated helper 'createfn' functions defined"""

    def __init__(self,tooltype,toolname,
                 createfn=None,
                 filterfn=_condAlwaysPass,                 
                 prereqs=[],modspec=None,
                 **properties
                 ):
        # For the easy cases where no helper function is needed.
        # They will be ignored in the case of a helper,
        # but are still required such that it's obvious what
        # the tool name and type are when defining the config.
        self.tooltype = tooltype
        self.toolname = toolname

        # The helper function may take 2 parameters:
        # a "modifier specification" string and the jet
        # definition

        # The helper function always returns the desired
        # modifier, and a ComponentAccumulator instance, 
        # in case additional supporting tools/services
        # need to be set up.
        if createfn is None:
            self.createfn = self.getGenericModifier
        else:
            self.createfn = createfn
        self.modspec = modspec

        # Prereqs is normally a list.
        # However, in special cases, the prereqs may
        # depend on either or both of modspec and jetdef,
        # in which case a helper function can be defined.
        self.prereqs = prereqs

        # a function taking a CondFlags as argument and deciding if this JetModifier is compatible
        # with the conditions.
        #  The function must return a tuple : (bool, "reason of failure")
        self.filterfn = filterfn 

        # These will be set as the Gaudi properties of the C++ tool
        self.properties = properties
        
                

        
    @make_lproperty
    def tooltype(self):pass
    @make_lproperty
    def toolname(self):pass
    @make_lproperty
    def createfn(self):pass
    @make_lproperty
    def modspec(self):pass
    @make_lproperty
    def prereqs(self):pass
    @make_lproperty
    def filterfn(self):pass
    @make_lproperty
    def properties(self):pass

    
    
    def __hash__(self):
        return hash((self.toolname,self.tooltype,self.createfn.__name__,self.modspec,str(self.prereqs)))

    def __eq__(self,rhs):
        return self.__hash__() == rhs.__hash__()

    def __ne__(self,rhs):
        return (not self.__eq__(rhs))

    # Define a string conversion for printing
    def __str__(self):
        return "JetModifier({0}/{1})".format(self.tooltype,self.toolname)
    # Need to override __repr__ for printing in lists etc
    __repr__ = __str__

    def getGenericModifier(self,jetdef, modspec):
        """returns a real tool instance accoding to this definition : simply instantiating from
        class self.tooltype and with name self.toolname ( actually : self.toolname.format(modspec) )
        Since this function will be called as a callback from JetRecConfig as 'func(jetdef, modspec)', it must accept
        the jetdef argument, even if unused in this case.
        """
        from AthenaConfiguration.ComponentFactory import CompFactory
        name = self.toolname.format(modspec=modspec)
        tool = CompFactory.getComp(self.tooltype)(name)
        return tool




########################################################################
    
@clonable
@onlyAttributesAreProperties
class JetInputExternal(object):
    """This class allows to declare primary data sources to jet finding which are typically outside of jet domain.
    Such sources can be container of particles (ex: clusters, selection of tracks,...) but also
    other object needed by some JetModifier (ex: EventDensity or track-vertex association map).

    The class is mainly here to hold a helper function (algoBuilder) in charge of configuring the proper algorithm to build the source. 
    If this function is None, then we expect the container pre-exists in the evt store. 

    Arguments to the constructor :
      - name : container name in event store 
      - objtype  : the xAODType (ex: xAODType.TruthParticle, xAODType.CaloCluster, ...)
      - algoBuilder [optional] : a function returning a configured algorithm which build the container
                                 the function is called as algoBuilder(parentjetdef, specs) where 
                                     parentjetdef is the JetDefinition for which this input building is called.
                                     specs is self.specs
                                 If omitted, it is assumed the container pre-exists in the event store.
     - specs [optional] : a string (or anything) which specifies some options, and passed to the algoBuilder function
     - filterfn : a function taking a CondFlags as argument and deciding if this JetModifier is compatible
                  with the conditions (same as JetModifier.filterfn )
                  The function must return a tuple : (bool, "reason of failure")
     - prereqs : a list of prerequisites (str) for this input definition. If any, these str must match the name of other existing JetInputExternal instances. 
    """
    def __init__(self, name, objtype, algoBuilder=None, specs=None, containername=None, filterfn= _condAlwaysPass, prereqs=[]):
        self.name = name
        self.basetype = objtype
        
        self.algoBuilder = algoBuilder if algoBuilder is not None else buildNothing # buildNothing returns None (see below)

        # In certain cases (EventShape) we need to configure the concrete
        # output container name based on the jetdef and specs, so can
        # pass in a (lambda) function to define this.
        if containername:
            self.containername = containername
        else:
            # Ordinarily we just return the name
            self.containername = lambda dummyjetdef,dummyspecs : self.name

        self.specs = specs
        self.filterfn = filterfn 
        self.prereqs = prereqs

        

    @make_lproperty
    def name(self): pass
    @make_lproperty
    def algoBuilder(self): pass
    @make_lproperty
    def basetype(self): pass
    @make_lproperty
    def specs(self): pass
    @make_lproperty
    def filterfn(self):pass
    @make_lproperty
    def prereqs(self):pass

    # make outputname an alias of name so JetInputExternal shares an interface with JetInputConstitSeq.
    outputname = make_alias("name")

    # Define a string conversion for printing
    def __str__(self):
        return f"JetInputExternal({self.name},type={str(self.basetype)})"
    

########################################################################    


from enum import IntEnum, auto
class JetInputType(IntEnum):
    """We reproduce the Enum from in xAODJet/​JetContainerInfo.h, xAOD::JetInput : loading the C++ library
    can slow down a lot the configuration. 
    Note : this is different from the xAODType which describes *only* c++ types whereas JetInputType describes
    categories of inputs to jets.
    """
    LCTopo=0
    EMTopo=auto()
    TopoTower=auto()
    Tower=auto()
    Truth=auto()  
    TruthWZ=auto()
    Track=auto()
    PFlow=auto()      
    LCPFlow=auto()      # LC PFlow
    EMPFlow=auto()      # EM Pflow at EM scale
    EMCPFlow=auto()     # EM Pflow calibrated to LC scale
    Jet=auto()
    LCTopoOrigin=auto()
    EMTopoOrigin=auto()
    TrackCaloCluster=auto()
    TruthDressedWZ=auto() # Truth jets without prompt e/mu (or dressed photons) or prompt gammas
    EMTopoOriginSK=auto()
    EMTopoOriginCS=auto()
    EMTopoOriginVor=auto()
    EMTopoOriginCSSK=auto()
    EMTopoOriginVorSK=auto()
    LCTopoOriginSK=auto()
    LCTopoOriginCS=auto()
    LCTopoOriginVor=auto()
    LCTopoOriginCSSK=auto()
    LCTopoOriginVorSK=auto()
    EMPFlowSK=auto()
    EMPFlowCS=auto()
    EMPFlowVor=auto()
    EMPFlowCSSK=auto()
    EMPFlowVorSK=auto()
    TruthCharged=auto() # Truth jets with only charged particles
    EMTopoOriginTime=auto()
    EMTopoOriginSKTime=auto()
    EMTopoOriginCSSKTime=auto()
    EMTopoOriginVorSKTime=auto()
    EMPFlowTime=auto()
    EMPFlowSKTime=auto()
    EMPFlowCSSKTime=auto()
    EMPFlowVorSKTime=auto()
    HI=auto()
    HIClusters=auto()
    Other = 100
    Uncategorized= 1000

    def fromxAODType(xt):
        """Returns a default JetInputType for a given xAODType """
        _xaodTojetinputMap = {
            xAODType.CaloCluster   : JetInputType.LCTopo,
            xAODType.ParticleFlow  : JetInputType.EMPFlow,
            xAODType.FlowElement   : JetInputType.EMPFlow,
            xAODType.TrackParticle : JetInputType.Track,
            xAODType.TruthParticle : JetInputType.Truth,    
        }
        return _xaodTojetinputMap.get(xt, JetInputType.Other) 
        
@clonable
@onlyAttributesAreProperties
class JetInputConstit(object):
    """Configuration for simplest constituents (or ghost constituents) to jets.
    This describes what can be the input to a PseudoJetAlgorithm.
    The containername attribute must correspond to an existing JetInputExternal so the system knows how to build this
    source container (if necessary).
    """

    def __init__(
        self,
        name,               # identifies this constit source, must be unique.
        objtype,            # The type of xAOD object from which to build the jets
        containername,      # The key of the source container in the event store.
        prereqs=[],         # will contain references to JetInputExternal
        label=None,         # used to describe a category for these constits. if None, will default to name
        jetinputtype=None,  # The JetInputType category. Can be passed as a string.
                            #    if None, set according to objtype.
        filterfn=_condAlwaysPass,
        lock=False,  # lock all properties of this instance
    ):

        self.name = name
        self.containername = containername
        self.prereqs = prereqs
        self.label = label or name

        self.basetype = objtype
        self.filterfn = filterfn

        jetinputtype = jetinputtype or JetInputType.fromxAODType(objtype)
        if isinstance(jetinputtype, str):
            jetinputtype = JetInputType[jetinputtype]
        self.jetinputtype = jetinputtype

        self._locked = lock

    @make_lproperty
    def basetype(self): pass
        
    @make_lproperty
    def name(self): pass

    @make_lproperty
    def containername(self): pass
    
    @make_lproperty
    def prereqs(self): pass

    @make_lproperty
    def filterfn(self):pass

    @make_lproperty
    def jetinputtype(self): pass
    
    # make an alias on containername so JetInputConstit and JetInputConstitSeq share an interface
    inputname = make_alias("containername")

    # Define a string conversion for printing
    def __str__(self):
        return f"JetInputConstit({self.name},type={str(self.basetype)})"
    

    
@clonable
@onlyAttributesAreProperties
class JetInputConstitSeq(JetInputConstit):
    """Configuration for JetConstituentModSequence. 
    Describes the constituents which need to be build with a JetConstituentModSequence.
    Uses a list of aliases to JetConstitModifier to describe the modif steps.
    """
    def __init__(self,
                 name,
                 objtype,         # The type of xAOD object from which to build the jets
                 modifiers=[],    # Modifications to be applied to constituents prior to jet finding
                 inputname=None,    # input collection which will be transformed into the source constituents
                 outputname=None,  #  output collection, will be set to self.containername
                 prereqs = [],     # will contain references to JetInputExternal 
                 label = None,
                 jetinputtype=None,
                 filterfn=_condAlwaysPass,                 
                 lock = False,    # lock all properties of this instance
    ):    
        
        JetInputConstit.__init__(self,name, objtype, outputname, prereqs=prereqs, jetinputtype=jetinputtype, filterfn=filterfn,label=label,lock=False, finalinit=False, )
        self.inputname  = inputname or name
        self.modifiers = modifiers

        
        self._locked = lock

    @make_lproperty
    def modifiers(self): pass

    @make_lproperty
    def inputname(self): pass
    @make_lproperty
    def label(self): pass
    
    
        
    def __hash__(self):
        return hash((self._basetype,str(self._modifiers)))

    def __eq__(self,rhs):
        return self.__hash__() == rhs.__hash__()

    def __ne__(self,rhs):
        return (not self.__eq__(rhs))

    
    # Define a string conversion for printing
    def __str__(self):
        return f"JetInputConstitSeq({self.name}, {self.inputname} , {self.containername})"
    # Need to override __repr__ for printing in lists etc
    __repr__ = __str__
    


@clonable
@onlyAttributesAreProperties
class JetConstitModifier(object):
    """Configuration for  a constituent modifier tool to be used in a JetConstituentModSequence.
    See StandardJetConstits.py for usage of this class.
    
    the properties argument in __init__ defines directly the properties of the final tool :
    if the tool has the property "PtMin" then passing 'dict(PtMin=10*GeV)' will result in 'tool.PtMin = 10*GeV'
    IMPORTANT : If a property is itself an other tool, we can pass a function returning the tool like in 'dict(TheSubTool = mySubToolFunc)'
    The function will be called only when appropriate in the form 'tool.TheSubTool = mySubToolFunc(constitseq)'
    """
    def __init__(self,
                 name,
                 tooltype,
                 prereqs= [],                 
                 properties={},
                 ):
        self.name = name
        self.tooltype = tooltype
        self.properties = properties
        self.prereqs = prereqs
        self.filterfn = _condAlwaysPass # we might want to make this a proper attribute in the future
        
    @make_lproperty
    def name(self): pass
    @make_lproperty
    def tooltype(self): pass
    @make_lproperty
    def properties(self): pass
    @make_lproperty
    def prereqs(self): pass



    
def buildNothing(*l):
    return None
