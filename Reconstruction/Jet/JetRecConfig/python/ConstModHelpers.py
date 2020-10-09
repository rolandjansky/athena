# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""
                                                                     #
ConstModUtils: A module for configuring constituent modifiers        #
Author: TJ Khoo                                                      #
                                                                     #
"""

import cppyy
try:
    cppyy.load_library('libxAODBaseObjectTypeDict')
except Exception:
    pass
from ROOT import xAODType
xAODType.ObjectType

from AthenaCommon import Logging
constmodlog = Logging.logging.getLogger('ConstModHelpers')

from AthenaConfiguration.ComponentFactory import CompFactory


from .StandardJetConstits import jetconstitdic, jetinputdefdic, jetconstitmoddic
from .JetDefinition import JetConstitSeq, JetInputDef, JetConstitSource

class _dummyJetDef:
    def __init__(self):
        self._prereqDic = {}
        self._prereqOrder = [] 
    

def aliasToInputDef(alias, parentjetdef=None):
    """convert a string alias to a full config object, either a JetConstitSeq or a JetInputDef according to the alias.

    This also recursively translate all aliases which are dependencies of this aliases. All these dependencies are 
    collected into the 'parentjetdef' (JetDefinition ).
    """
    parentjetdef = parentjetdef or _dummyJetDef()
    # ----------------
    # support the cases where alias is not a string but directly a config object 
    if isinstance(alias, JetConstitSource):
        instantiateJetConstitAliases(alias, parentjetdef)
        return alias
    if isinstance(alias, JetInputDef):
        instantiateJetInputDefAliases(alias, parentjetdef)
        return alias

    # else assume it's a string
        
    if alias in jetinputdefdic:
        inputdef = jetinputdefdic[alias].clone()
        instantiateJetInputDefAliases(inputdef, parentjetdef)
        return inputdef
    else:
        constitseq = jetconstitdic[alias].clone( )
        instantiateJetConstitAliases(constitseq, parentjetdef)
        return constitseq

def instantiateJetConstitAliases(constitseq, parentjetdef):
    """Reccursively translate all aliases appearing in the prereqs of constitseq into proper config objects.
    All are collected into the parentjetdef for which this JetConstitSeq is being configured.
    Then instantiates all aliases for JetConstitModifier
    """

    # we technically need a JetInputDef for constitseq.inputname : conveniently declare it if not existing :
    jetinputdefdic.setdefault( constitseq.inputname, JetInputDef( constitseq.inputname, constitseq.basetype) )
    # we re-use the instantiateJetInputDefAliases to instantiate the prereqs
    instantiateJetInputDefAliases( constitseq, parentjetdef,pre_prereqs=['input:'+constitseq.inputname])

    # JetConstitSource don't have modifiers, we can return immediately
    if not hasattr( constitseq, "modifiers") : return
    
    # instantiate the JetConstitModifier (those don't have dependencies)
    for mod in constitseq.modifiers:
        modInstance =  jetconstitmoddic[ mod ].clone()
        constitseq._instanceMap[mod] = modInstance

def instantiateJetInputDefAliases(jetinputdef, parentjetdef, pre_prereqs=[]):
    """Reccursively translate all aliases appearing in the prereqs of jetinputdef into proper config objects.
    All are collected into the parentjetdef for which this JetConstitSeq is being configured.
    """
    prereqs = jetinputdef.prereqs(parentjetdef) if  callable(jetinputdef.prereqs) else jetinputdef.prereqs
    prereqs = pre_prereqs + prereqs
    for prereq in prereqs   :
        reqInstance = parentjetdef._prereqDic.get( prereq, None)
        if reqInstance is None:
            _ , inputkey = prereq.split(':') # always in the form "input:XXX"
            reqInstance = aliasToInputDef(inputkey, parentjetdef)
            parentjetdef._prereqDic[prereq] = reqInstance
            parentjetdef._prereqOrder.append(prereq)

        




def getConstitModAlg(constit, monTool=None):
    """returns a configured JetConstituentModSequence or None if constit.modifiers == [] """
    
    # JetConstitSource do not need any JetConstituentModSequence
    # (they are only needed to trigger the building of the source container and a PJ algo)
    if not isinstance(constit, JetConstitSeq): return

    
    inputtype = constit.basetype

    sequence = constit.modifiers
    
    modlist = []

    #if modlist == []: return
    if constit.inputname == constit.containername: return
    
    for step in sequence:
        modInstance = constit._instanceMap[ step ]
        if not modInstance.tooltype: continue

        toolclass = getattr( CompFactory, modInstance.tooltype)

        # update the properties : if some of them are function, just replace by calling this func :
        for k,v in modInstance.properties.items():
            if callable(v) :
                modInstance.properties[k ] = v( constit )
        
        tool =  toolclass(modInstance.name,**modInstance.properties)
        
        if inputtype == xAODType.ParticleFlow and modInstance.tooltype not in ["CorrectPFOTool","ChargedHadronSubtractionTool"]:
            tool.IgnoreChargedPFO=True
            tool.ApplyToChargedPFO=False
        tool.InputType = inputtype
        modlist.append(tool)

    sequenceshort = "".join(sequence)
    seqname = "ConstitMod{0}_{1}".format(sequenceshort,constit.name)
    inputcontainer = str(constit.inputname)
    outputcontainer = str(constit.containername)
    if inputtype==xAODType.ParticleFlow:
        # Tweak PF names because ConstModSequence needs to work with
        # up to 4 containers
        def chopPFO(thestring):
            pfostr = "ParticleFlowObjects"
            if thestring.endswith(pfostr):
                return thestring[:-len(pfostr)]
            return thestring
        inputcontainer = chopPFO(inputcontainer)
        outputcontainer = chopPFO(outputcontainer)

    modseq = CompFactory.JetConstituentModSequence(seqname,
                                                   InputType=inputtype,
                                                   OutputContainer = outputcontainer,
                                                   InputContainer= inputcontainer,
                                                   Modifiers = modlist,
                                                   MonTool = monTool,
    )

    constitmodalg = CompFactory.JetAlgorithm("jetalg_{0}".format(modseq.getName()))
    constitmodalg.Tools = [modseq]

    return constitmodalg





def constitModWithAlternateTrk(mod, trkopt):
    """Generates a new JetConstitModifier cloned from mod and stores it in the standard jetconstitmoddic.
    The cloned JetConstitModifier has all its track related properties changed according to trackcollectionmap[trkopt]
    This is mainly useful to configure trigger algs.
    """
    from JetRecTools.JetRecToolsConfig import trackcollectionmap
    from JetRecConfig.StandardJetConstits import jetconstitmoddic
    newmod = mod+trkopt

    if newmod in jetconstitmoddic:
        return jetconstitmoddic[newmod]

    newmodInstance  = jetconstitmoddic[mod].clone()

    trackproperties = trackcollectionmap[trkopt]
    
    if mod == "CorrectPFO":
        newmodInstance.properties.update(VertexContainerKey=trackproperties["Vertices"])
    elif mod=="CHS":
        newmodInstance.properties.update(VertexContainerKey=trackproperties["Vertices"],
                                         TrackVertexAssociation=trackproperties["TVA"])
    jetconstitmoddic[newmod] = newmodInstance
    
    return newmodInstance


