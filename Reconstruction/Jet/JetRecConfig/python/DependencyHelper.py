# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""

Functions to solve dependencies of Jet reco components.

Jet reco components are objects described by classes in JetDefinition.py (including JetDefinition, JetModifier,...)
and the dependencies of each instance are set as (list of) string aliases refering to other components.

The functions here are scaning reccursively all the aliases, building the corresponging component objects and
collecting them in a JetDefinition.

"""
from .JetDefinition import JetInputExternal, JetInputConstit, JetModifier, JetInputConstitSeq

class _dummyJetDef:
    def __init__(self):
        self._prereqDic = {}
        self._prereqOrder = [] 


def solveDependencies( jetdef0 ):
    """ Retrieve reccursively all  dependencies described by str aliases (from modifiers, ghosts, etc..) within jetdef0.
    The aliases are converted in to proper config objects (like JetModifier, JetInputConstit,...) and are collected into
    a cloned version of jetdef0.
    The cloned version is returned and contains all the necessary information to build the actual C++ tools and algs.
     (in particular, the _prereqDic and _prereqOrder internal members of the clone are filled).
    """

    jetdef = jetdef0.clone()
    # # start with the inputdef, cloning it so we're not altering a private copy
    # jetdef.inputdef = jetdef.inputdef.clone()
    # 
    solveConstitDependencies(jetdef.inputdef, jetdef, inplace=True)

    jetdef._prereqDic['input:'+jetdef.inputdef.name] = jetdef.inputdef
    jetdef._prereqOrder.append('input:'+jetdef.inputdef.name)

    for g in jetdef.extrainputs:
        gInstance = aliasToInputDef( g , jetdef)
        jetdef._prereqDic['input:'+g] = gInstance
        jetdef._prereqOrder.append('input:'+g)
    
    for g in jetdef.ghostdefs:
        gInstance = aliasToInputDef( g , jetdef)
        jetdef._prereqDic['input:'+g] = gInstance
        jetdef._prereqOrder.append('input:'+g)
        jetdef._prereqDic['ghost:'+g] = gInstance
        jetdef._prereqOrder.append('ghost:'+g)

    for mod in jetdef.modifiers:
        modInstance = aliasToModDef(mod, jetdef)
        jetdef._prereqDic['mod:'+mod] = modInstance
        jetdef._prereqOrder.append('mod:'+mod)

    # Deduplicate the prereq (with python > 3.6 dict is ordered so the trick is guaranteed to work) :
    jetdef._prereqOrder[:] = list(dict.fromkeys(jetdef._prereqOrder) )

    return jetdef


def solveGroomingDependencies( groomdef0 ):
    """Retrieve  all  dependencies described by str aliases in groomdef0.modifiers.
    
    The aliases are converted in to proper config objects (like JetModifier, JetInputConstit,...) and are collected into
    a cloned version of groomdef0.
    The cloned version is returned and contains all the necessary information to build the actual C++ tools and algs.
     (in particular, the _prereqDic and _prereqOrder internal members of the clone are filled).
    """

    groomdef = groomdef0.clone()
    for mod in groomdef.modifiers:
        modInstance = aliasToModDef(mod, groomdef)
        groomdef._prereqDic['mod:'+mod] = modInstance
        groomdef._prereqOrder.append('mod:'+mod)
    return groomdef


def aliasToInputDef(alias, parentjetdef=None):
    """convert a string alias to a full config object, either a JetInputConstitSeq or a JetInputExternal according to the alias.

    This also recursively translate all aliases which are dependencies of this alias. All these dependencies are 
    collected into the 'parentjetdef' (JetDefinition ).
    """
    parentjetdef = parentjetdef or _dummyJetDef()
    # ----------------
    # support the cases where alias is not a string but directly a config object 
    if isinstance(alias, JetInputConstit):
        solveConstitDependencies(alias, parentjetdef, inplace=True)
        return alias
    if isinstance(alias, JetInputExternal):
        solveInputExternalDependencies(alias, parentjetdef, inplace=True)
        return alias

    # else assume it's a string
    
    from .StandardJetConstits import stdConstitDic, stdInputExtDic
    if alias in stdInputExtDic:
        # then it must be a JetInputExternal instance :
        return solveInputExternalDependencies(stdInputExtDic[alias], parentjetdef)
    else:
        # then it must be a JetInputConstit or JetInputConstitSeq instance :
        return solveConstitDependencies(stdConstitDic[alias], parentjetdef)

def solveConstitDependencies(constitseq, parentjetdef, inplace=False):
    """Reccursively translate all aliases appearing in the prereqs of constitseq into proper config objects.
    All are collected into the parentjetdef for which this JetInputConstitSeq is being configured.
    Then instantiates all aliases for JetConstitModifier
    """

    if not inplace:
        constitseq = constitseq.clone()
        
    from .StandardJetConstits import stdInputExtDic, stdContitModifDic
    # we technically need a JetInputExternal for constitseq.inputname : conveniently declare it if not existing :
    stdInputExtDic.setdefault( constitseq.inputname, JetInputExternal( constitseq.inputname, constitseq.basetype) )
    # we re-use the solveInputExternalDependencies to instantiate the prereqs
    constitseq.prereqs += ['input:'+constitseq.inputname] # make sure the external input to these constituents are taken into account.

    if isinstance( constitseq, JetInputConstitSeq):
        # instantiate the JetConstitModifier and add their dependencies to the actual constit sequence
        for mod in constitseq.modifiers:
            modInstance =  stdContitModifDic[ mod ].clone()
            parentjetdef._prereqDic[f'cmod:{mod}'] = modInstance
            solveInputExternalDependencies( modInstance, parentjetdef, inplace=True)
    # the rest of dependencies are handled similarly as JetInputExternal :
    solveInputExternalDependencies( constitseq, parentjetdef)                

    return constitseq

def solveInputExternalDependencies(jetinputext, parentjetdef,  inplace=False):
    """Reccursively translate all aliases appearing in the prereqs of jetinputext into proper config objects.
    All are collected into the parentjetdef for which this JetInputConstitSeq is being configured.
    """
    if not inplace:
        jetinputext = jetinputext.clone()
    if  callable(jetinputext.prereqs):        
        jetinputext.prereqs = jetinputext.prereqs(parentjetdef) 
    for prereq in jetinputext.prereqs :
        reqInstance = parentjetdef._prereqDic.get( prereq, None)
        if reqInstance is None:
            _ , inputkey = prereq.split(':') # always in the form "input:XXX"
            reqInstance = aliasToInputDef(inputkey, parentjetdef)
            parentjetdef._prereqDic[prereq] = reqInstance
            parentjetdef._prereqOrder.append(prereq)
    
    return jetinputext




def prereqToDef(prereq, parentjetdef):
    """translate a prereq string in the form 'type:alias' into a known config object.
    """
    reqtype, reqkey = prereq.split(':',1)
    if reqtype=='mod':
        reqInstance = aliasToModDef(reqkey, parentjetdef)
    else:
        reqInstance = aliasToInputDef(reqkey, parentjetdef)
    return reqInstance

def aliasToModDef(alias, parentjetdef ):
    """return a JetModifier config object corresponding to alias, also recursively translating all aliases in the dependencies of this JetModifier."""
    if isinstance(alias, JetModifier):
        return alias
    # else assume it's a string

    # split it, to extract the optional specifiers 
    modL = alias.split(":")
    modkey = modL[0]
    modspec = ':'.join(modL[1:])

    # retrieve an instance from the known modifiers in StandardJetMods :
    from .StandardJetMods import stdJetModifiers
    moddef = stdJetModifiers[modkey].clone( modspec = modspec)

    
    if callable(moddef.prereqs):
        moddef.prereqs = moddef.prereqs( modspec, parentjetdef )        
    
    for prereq in moddef.prereqs:
        reqInstance = parentjetdef._prereqDic.get( prereq, None)
        if reqInstance is None:
            reqInstance = prereqToDef(prereq, parentjetdef)

        if prereq.startswith('ghost:'):
            # then it is also an input : register this requirement also as an input
            prereqN = prereq.split(':')[1]
            parentjetdef._prereqOrder.append('input:'+prereqN)
            parentjetdef._prereqDic['input:'+prereqN] = reqInstance # the input config instance is identical for input and ghost (only the PseudoJet will differ)
            
        parentjetdef._prereqOrder.append(prereq)
        parentjetdef._prereqDic[prereq] = reqInstance
        
    return moddef
