# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

######################################################
"""
JetAnalysisCommon

This module defines various components to be used in AnalysisBase releases.
These components (classes, submodules...) can be used in place of some Athena components
they are replacing, so athena-style configuration can be used unchanged in AnalysisBase

This module is thus designed ONLY for AnalysisBase and to run EventLoop jobs.

Internally, the athena-style configurations of algorithms are translated to AnaReentrantAlgorithmConfig or 
AnaAlgorithmConfig. The configuration of AsgTools are translated to call to AnaAlgorithmConfig.addPrivateTool() 
or AnaAlgorithmConfig.setPropertyFromString()

This module allows to write python config for EventLoop exactly as one would do for Athena :

alg1 = CompFactory.MyAlg("algname",
                         InputContainer = "SomeJets".
                         Aparameter=4.305,
                         AprivateTool = CompFactory.SomeTool("tool", AToolparam =0.21), )
alg2 = CompFactory.AnOtherAlg( ... )

job = ROOT.EL.Jon()
 ...
job.addManyAlgs([alg1, alg2]) # specific call allowed by this module

driver = ROOT.EL.DirectDriver()
driver.submit( job, "out")

"""

import logging
from types import SimpleNamespace, ModuleType
import ROOT

#*******************************************************************
### configure logger module ----------------------------------------
logging.VERBOSE = logging.DEBUG - 1
logging.ALL     = logging.DEBUG - 2
logging.addLevelName( logging.VERBOSE, 'VERBOSE' )
logging.addLevelName( logging.ALL, 'ALL' )
def _verbose(self, message, *args, **kws):
    if self.isEnabledFor(logging.VERBOSE):
        # Yes, logger takes its '*args' as 'args'.
        self._log(logging.VERBOSE, message, args, **kws) 
logging.Logger.verbose = _verbose


Logging = ModuleType("Logging")
Logging.logging = logging


#*******************************************************************
class SystemOfUnits:
    """Recopied from AthenaCommon.SystemOfUnits. Only energies for now """
    #
    # Energy [E]
    #
    megaelectronvolt = 1. 
    electronvolt     = 1.e-6*megaelectronvolt
    kiloelectronvolt = 1.e-3*megaelectronvolt
    gigaelectronvolt = 1.e+3*megaelectronvolt
    teraelectronvolt = 1.e+6*megaelectronvolt
    petaelectronvolt = 1.e+9*megaelectronvolt

    #joule = electronvolt/e_SI                    # joule = 6.24150 e+12 * MeV

    # symbols
    MeV = megaelectronvolt
    eV  = electronvolt
    keV = kiloelectronvolt
    GeV = gigaelectronvolt
    TeV = teraelectronvolt
    PeV = petaelectronvolt




#*******************************************************************

def stringPropValue( value ):
     """Helper function producing a string property value"""
 
     stringValue = str( value )
     if isinstance( value, bool ):
         stringValue = str( int( value ) )
         pass
     return stringValue

class ConfArray:
    """A simplistic array of Configured (see below) to replace the ToolHandleArray of Athena """
    def __init__(self, key, conflist , parent):
        self.key = key
        self._parent = parent
        self.conflist=[]
        self += conflist

    def __iadd__(self, conflist):
        self.conflist += conflist
        for conf in conflist:
            conf.setparent(self._parent)  
            conf._name = self.key

    def __iter__(self):
        return iter(self.conflist)
            
    def append(self, conf):
        self += [conf]
        
    def assignAllProperties(self, anaAlg):
        self._anaAlg = anaAlg
        for conf in self.conflist:
            actualName = anaAlg.createPrivateToolInArray(conf.fullname(), conf.type)
            conf._name = actualName.split('.')[-1] # because AnaAlgorithmConfig will assign it's own naming scheme
            conf.assignAllProperties(anaAlg)
            
            
class Configured:
    """A replacement for Athena auto-generated Configured python class.
    Configured has the same interface as its Athena counterpart and can describe both Tools and Algorithms

    This is a base class. The system replacing CompFactory will generate a derived class for each c++ Tool/Algorithm
    to hold the configuration of such tool/alg  (see generateConfigured())
    """
    _properties=set()
    _parent = None
    _anaAlg = None
    # the list of properties and attribute memeber which are allowed to be set.
    # The full list will be generated for each class when actual classes are build in generateConfigured
    _allowed = ['_properties', '_name', '_parent', '_anaAlg']

    def __init__(self, name, **props):
        self._name = name
        self._properties = set()
        for k,v in props.items():
            setattr(self,k,v)

    def __setattr__(self, k, v):
        if k in self._allowed:
            self._setattrImpl(k,v)
        else:
            raise AttributeError("Configuration of Tool {} / {} . can't set attribute : {}".format(self.type, self._name, k) )

    def _setattrImpl(self, k, v) :
        super().__setattr__(k,v)
        if k[0] == '_' :
            # this is not a Property 
            return

        if isinstance(v, Configured):
            if k in self._properties:
                raise RuntimeError( "Configuring {} / {} : Tool for property {} already exists".format(self.type, self._name, k) )
            # it's a tool:
            v.setparent(self)
            v._name = k
        elif isinstance(v, (list, tuple) ):
            if isinstance(v[0], Configured):
                v = ConfArray(k,v, self)
                super().__setattr__(k,v)                
        self._properties.add(k)
        if self._anaAlg:
            self.setPropToAnaAlg( k , v)


    def getName(self):
        return self._name
    
    def setparent(self, parent):
        self._parent = parent

    def prefixed(self, k):
        if self._parent is None: return k
        return self.fullname()+'.'+k

    def ancestors(self):
        if self._parent is None : return [self]
        return self._parent.ancestors()+[self]
    
    def fullname(self):
        parents = self.ancestors()[1:]
        return '.'.join([p._name for p in parents])
        
    def properties(self):
        return [ (k, getattr(self,k)) for k in self._properties]

    def typeAndName(self):
        return self.type+'/'+self._name

    def asAnaAlg(self):
        if issubclass(self._cppclass, ROOT.EL.AnaReentrantAlgorithm):
            alg=ROOT.EL.AnaReentrantAlgorithmConfig()
        else:
            alg=ROOT.EL.AnaAlgorithmConfig()

        alg.setTypeAndName( self.typeAndName() )
        self.assignAllProperties(alg)
        return alg

    def assignAllProperties(self, anaAlg):
        self._anaAlg = anaAlg 
        for (k,v) in self.properties():
            self.setPropToAnaAlg(k,v)
            
    def setPropToAnaAlg(self, k, v):
        alg=self._anaAlg
        if isinstance(v , Configured):
            # it must be a Tool :
            alg.createPrivateTool(v.fullname(), v.type)
            v.assignAllProperties(alg)
        elif isinstance(v, ConfArray ):
            # it is a Tool array 
            v.assignAllProperties(alg)
        else:
            # any other type :
            alg.setPropertyFromString(self.prefixed(k) , stringPropValue( v ) )

            
def generateConfigured(classname, cppclass, prefix=""):
    import cppyy

    # get an instance of the class :
    if issubclass(cppclass, cppyy.gbl.asg.IAsgTool):
        dummy = cppclass('dummy')
    else: # then it's an Algorithm
        dummy = cppclass('dummy', 0)

    # find all the properties of the Tool/Algorithm
    allowedProp = Configured._allowed + [k for k,v in  dummy.getPropertyMgr().getProperties() ]
    # generate the class derived from Configured for this Tool/Alg
    klass=type(classname+'Conf', (Configured,), dict(_allowed=allowedProp,
                                                              type=prefix+classname,_cppclass=cppclass) )

    return klass

class ConfNameSpace:
    """A namespace able to automatically generate Configured when quering attributes :
    Used to replace CompFactory so that expressions like :
      tool = CompFactory.Trk.SomeTrkTool("tname", Prop=2.345) 
      tool = CompFactory.getComp("Trk::SomeTrkTool")("tname", Prop=2.345) 
    works. 
    In the above 2 examples both CompFactory and Trk are ConfNameSpace
    """
    def __init__(self, name=""):
        self.prefix=name+"::" if name !="" else ""

    def __getattr__(self, t):
        return self.getComp(t)

    def getComp(self, classname):
        """generates a new Configured class for the C++ class ROOT.classname .
         This implies there must be a dictionnary for classname.
        """

        # if we already generated the class, return it :
        c = self.__dict__.get(classname, None)
        if c is not None:
            return  c

        # look for the c++ class from ROOT :
        c=getattr(ROOT, self.prefix+classname, None)

        if c is None:
            print("JetAnalysisCommon ERROR : ",classname," is not a known C++ tool, alg, or namespace ")
            raise
            
        if hasattr(c,'getPropertyMgr'):
            conf = generateConfigured(classname,c,self.prefix)
        else:
            # not a configurable. Must be a namespace
            conf = ConfNameSpace(self.prefix+classname)

        # save this new class onto self : 
        setattr(self, classname, conf)
        return conf
    
    def addNameSpaces(self, *nsList):
        for ns in nsList:
            setattr(self, ns, ConfNameSpace(self.prefix+ns))
        

    
# ----------------------------
# A replacement for CompFactory
CompFactory = ConfNameSpace()
# Add known namespaces  : 
CompFactory.addNameSpaces( 'Analysis', 'Trk', 'Jet', 'Sim')

# Make a pseudo-Module :
ComponentFactory = ModuleType("ComponentFactory")
ComponentFactory.CompFactory = CompFactory



#*******************************************************************
# replacements for ConfigFlags
ConfigFlags = SimpleNamespace(
    Input = SimpleNamespace(),    
)
AllConfigFlags = ModuleType("AllConfigFlags")
AllConfigFlags.ConfigFlags= ConfigFlags

def setupFlags(inputFiles, ):
    """Setup the ConfigFlags according to the input files content. 
    This is required for some part of the Athena-style config to work.
    """
    ConfigFlags.Input.Files = inputFiles
    f = ROOT.TFile(ConfigFlags.Input.Files[0]) 
    tree = f.CollectionTree
    ConfigFlags.Input.Collections = [br.GetName() for br in tree.GetListOfBranches() if '.' not in br.GetName()]

    return ConfigFlags


#*******************************************************************
# replacements for CFElements
def parOR(name):
    pass
CFElements = ModuleType("CFElements")
CFElements.parOR = parOR


#*******************************************************************

class ComponentAccumulatorMockUp:
    """Provdide similar interface than AthenaConfiguration.ComponentAccumulator and also very simplistic
    merging of list of algorithms 
    """
    def __init__(self, name="ca"):
        self.name = name
        self.algs = []

    def __iter__(self):
        return iter(self.algs)
    
    def addSequence(self, seqName):
        pass
    def addEventAlgo(self, alg, primary=False, sequenceName=""):
        self.algs +=[alg]
        setattr(self, alg._name, alg)
        
    def merge(self, ca, sequenceName=""):
        myTNs = set( alg.typeAndName() for alg in self.algs)
        for alg in ca.algs:
            tn = alg.typeAndName()
            if tn not in myTNs:
                self.algs.append(alg)
                setattr(self, alg._name, alg)
        

ComponentAccumulator = ModuleType("ComponentAccumulator")
ComponentAccumulator.ComponentAccumulator = ComponentAccumulatorMockUp





#*******************************************************************
#
def addManyAlgs(job, algList):
    """a little configuration function added from the python module JetAnalysisCommon.py to easily schedule
    # a list of Configured algs as defined by this module."""
    for alg in algList:
        job.algsAdd( alg.asAnaAlg() )

ROOT.EL.Job.addManyAlgs = addManyAlgs        


#*******************************************************************
# hack the list of modules in sys so that the Athena config modules are found and redirected
# to what we have defined in this module 
import sys
JetAnalysisCommon = sys.modules[__name__]
sys.modules['AthenaCommon'] = JetAnalysisCommon
sys.modules['AthenaCommon.Logging'] = JetAnalysisCommon.Logging
sys.modules['AthenaCommon.SystemOfUnits'] = JetAnalysisCommon.SystemOfUnits
sys.modules['AthenaConfiguration'] = JetAnalysisCommon
sys.modules['AthenaConfiguration.ComponentFactory'] = JetAnalysisCommon.ComponentFactory
sys.modules['AthenaConfiguration.ComponentAccumulator'] = JetAnalysisCommon.ComponentAccumulator
sys.modules['AthenaCommon.CFElements'] = JetAnalysisCommon.CFElements
sys.modules['AthenaConfiguration.AllConfigFlags'] = JetAnalysisCommon.AllConfigFlags



#*******************************************************************
# hack specific to jets
import JetRecConfig.JetRecConfig as JetRecConfig

# In Athena the jet config relies on the automatic scheduling of algorithms
# which relies on Read/WriteHandles to correctly order them.
# There's no such automatic ordering in EventLoop.
# We thus need to add a step in AnalysisBase to make sure inputs algs to jet finder are
# correctly ordered.
# For this we redefine the original JetRecConfig.JetRecCfg function. 

JetRecConfig.JetRecCfg_original = JetRecConfig.JetRecCfg
def JetRecCfg_reorder(jetdef, configFlags, returnFinalJetDef=False):
    """Builds the algs with JetRecConfig.JetRecCfg and then make sure
    they are in proper order.
    Re-ordering is done manually, according to various input alg type.
    """
    res = JetRecConfig.JetRecCfg_original(configFlags, jetdef , returnFinalJetDef)

    acc , _ = res if returnFinalJetDef else (res,None)
    algs = acc.algs

    # ************
    # reorder EventDensity and PseudoJetAlg 
    evtDensityAlgs = [ (i,alg) for (i,alg) in enumerate(algs) if alg._cppclass == ROOT.EventDensityAthAlg ]
    pjAlgs = [ (i,alg) for (i,alg) in enumerate(algs) if alg._cppclass == ROOT.PseudoJetAlgorithm ]
    pairsToswap = []
    for i,edalg in evtDensityAlgs:
        edInput = edalg.EventDensityTool.InputContainer
        for j,pjalg in pjAlgs:
            if j<i: continue 
            if edInput == pjalg.OutputContainer:
                pairsToswap.append( (i,j) )
    for (i,j) in pairsToswap:
        algs[i], algs[j] = algs[j], algs[i]

    # ************
    # if there were other types of alg which need re-rordering
    # we could add the specific re-ordering code below ...
    
    return res

JetRecConfig.JetRecCfg = JetRecCfg_reorder
