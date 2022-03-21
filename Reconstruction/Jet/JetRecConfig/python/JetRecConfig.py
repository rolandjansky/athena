# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

"""
JetRecConfig: A helper module for configuring jet reconstruction     

The functions defined here turn JetDefinition object into ComponentAccumulator or list of algs fully configured
and ready to be inserted in the framework sequence.

Author: TJ Khoo, P-A Delsart                                                      
"""

########################################################################
import os

from AthenaCommon import Logging
jetlog = Logging.logging.getLogger('JetRecConfig')

from ROOT import xAODType
xAODType.ObjectType

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


from JetRecConfig.JetDefinition import JetDefinition, JetInputConstitSeq, JetInputConstit, JetInputExternal
from JetRecConfig.JetGrooming import GroomingDefinition
from JetRecConfig.DependencyHelper import solveDependencies, solveGroomingDependencies, aliasToModDef


__all__ = ["JetRecCfg", "JetInputCfg"]



########################################################################
#
# Top level functions returning ComponentAccumulator out of JetDefinition 

def JetRecCfg( configFlags, jetdef,  returnConfiguredDef=False):
    """Top-level function for running jet finding or grooming.
    
    This returns a ComponentAccumulator that can be merged with others
    from elsewhere in the job and which provides everything needed to
    reconstruct one jet collection.

    arguments : 
      - jetdef : jet or grooming definition
      - configFlags : the AthenaConfiguration.AllConfigFlags.ConfigFlags, mainly for input file
    peeking such that we don't attempt to reproduce stuff that's already
    in the input file. And also to be able to invoke building of inputs outside of Jet domain during reco from RAW/RDO.
      - returnConfiguredDef : is for debugging. It will also returns the cloned JetDefinition which contains the calculated dependencies.

    """
    
    sequenceName = jetdef.fullname()
    jetlog.info("******************")    
    jetlog.info("Setting up to find {0}".format(sequenceName))

    components = ComponentAccumulator()
    from AthenaCommon.CFElements import parOR
    components.addSequence( parOR(sequenceName) )

    # call the relevant function according to jetdef_i type 
    if isinstance(jetdef, JetDefinition):
        algs, jetdef_i = getJetDefAlgs(configFlags, jetdef , True)
    elif isinstance(jetdef, GroomingDefinition):
        algs, jetdef_i = getJetGroomAlgs(configFlags, jetdef, True)

    # FIXME temporarily reorder for serial running
    if configFlags.Concurrency.NumThreads <= 0:
        jetlog.info("Reordering algorithms in sequence {0}".format(sequenceName))
        algs = reOrderAlgs(algs)

    for a in algs:

        if isinstance(a, ComponentAccumulator):
            components.merge(a )
        else:
            components.addEventAlgo( a , sequenceName = sequenceName )

    if returnConfiguredDef: return components, jetdef_i
    return components


def JetInputCfg(configFlags,jetOrConstitdef , context="default"):    
    """Returns a ComponentAccumulator containing algs needed to build inputs to jet finding as defined by jetOrConstitdef

    jetOrConstitdef can either be 
     * a JetDefinition : this happens when called from JetRecCfg, then the jetdef._prereqDic/Order are used.
     * a JetInputConstit : to allow scheduling the corresponding constituents algs independently of any jet alg. 
    
    context is only used if jetOrConstitdef is not a JetDefinition and must refer to a context in StandardJetContext
    """
    components = ComponentAccumulator()

    algs = getInputAlgs(jetOrConstitdef, configFlags, context)

    for a in algs:

        if isinstance(a, ComponentAccumulator):
            components.merge(a)
        else:
            components.addEventAlgo(a)
    
    return components

def PseudoJetCfg(jetdef):
    """Builds a ComponentAccumulator for creating PseudoJetContainer needed by jetdef.
    THIS updates jetdef._internalAtt['finalPJContainer'] 
    """
    components = ComponentAccumulator()
    pjalglist = getPseudoJetAlgs(jetdef)
    for pjalg in pjalglist:
        components.addEventAlgo(pjalg)
    return components


########################################################################
########################################################################
#
# Mid level functions returning list of algs out of JetDefinition 


def getJetDefAlgs(configFlags, jetdef ,  returnConfiguredDef=False, monTool=None):
    """ Create the algorithms necessary to build the jet collection defined by jetdef.
    
    This internally finds all the dependencies declared into jetdef (through input, ghosts & modifiers) 
    and returns a list of all necessary algs.

    if returnConfiguredDef==True, also returns the fully configured clone of jetdef containing solved dependencies (debugging)

    monTool is to allow the trigger config to pass a monitoring tool.

    returns a list containing either algs or ComponentAccumulator 
      (ComponentAccumulator occurs only (?) in reco from RDO/RAW when we need to build externals such as clusters or tracks : in this case we call the main config functions from external packages)
    """

    # Scan the dependencies of this jetdef, also converting all aliases it contains 
    # into config objects and returning a fully configured copy.

    jetdef_i = solveDependencies(jetdef, configFlags=configFlags)
    jetdef_i._cflags = configFlags


    # check if the conditions are compatible with the inputs & modifiers of this jetdef_i.
    # if in standardRecoMode we will remove whatever is incompatible and still try to run
    # if not, we raise an exception
    canrun = removeComponentFailingConditions(jetdef_i, raiseOnFailure= not jetdef_i.standardRecoMode)
    if not canrun :
        if returnConfiguredDef:
            return [], jetdef_i
        return []

    algs = []

    # With jetdef_i, we can now instantiate the proper c++ tools and algs.
    
    # algs needed to build the various inputs (constituents, track selection, event density, ...)
    algs += getInputAlgs(jetdef_i, configFlags , monTool=monTool)

    # algs to create fastjet::PseudoJet objects out of the inputs
    algs+= getPseudoJetAlgs(jetdef_i)
    
    # Generate a JetRecAlg to run the jet finding and modifiers
    jetrecalg = getJetRecAlg( jetdef_i, monTool=monTool)
    algs += [jetrecalg]
    
    jetlog.info("Scheduled JetAlgorithm instance \"jetalg_{0}\"".format(jetdef_i.fullname()))
    
    if returnConfiguredDef:
        return algs, jetdef_i
    return algs

def getJetGroomAlgs(configFlags, groomdef, returnConfiguredDef=False, monTool=None):
    """Instantiate and schedule all the algorithms needed to run the grooming alg 'groomdef' and
    add them in the ComponentAccumulator 'components'

    This function is meant to be called from the top-level JetRecConfig.JetRecCfg
    (groomdef is expected to be non locked and will be modified).

    monTool is to allow the trigger config to pass a monitoring tool.
    """
    
    # Find dependencies from modifier aliases and get a fully configured groomdef 
    #  ( This also detects input dependencies, see below)
    groomdef_i = solveGroomingDependencies(groomdef)
    
    # Transfer the input & ghost dependencies onto the parent jet alg,
    # so they are handled when instatiating the parent jet algs
    for prereq  in groomdef_i._prereqOrder:
        #Protection for some modifiers that have three 'arguments'
        if len(prereq.split(':')) > 2:
            continue
        reqType, reqKey = prereq.split(':')
        if reqType=='ghost':
            groomdef_i.ungroomeddef.ghostdefs.append(reqKey)
        elif reqType.endswith('input:') : # can be extinput or input
            groomdef_i.ungroomeddef.extrainputs.append(reqKey)

    jetlog.info("Scheduling parent alg {} for {} ".format(groomdef.ungroomeddef.fullname(), groomdef.fullname()))

    # Retrieve algs needed to build the parent (ungroomed) jets
    # (we always want it even if the parent jets are already in the input file because
    #  we need to rebuild the pseudoJet)
    algs, ungroomeddef_i = getJetDefAlgs(configFlags, groomdef_i.ungroomeddef , True)
    groomdef_i._ungroomeddef = ungroomeddef_i # set directly the internal members to avoid complication. This is fine, since we've been cloning definitions.

    #Filter the modifiers based on the configFlags
    removeGroomModifFailingConditions(groomdef_i, configFlags, raiseOnFailure = not groomdef_i.ungroomeddef.standardRecoMode)

    algs += [ getJetRecGroomAlg(groomdef_i, monTool=monTool) ]


    jetlog.info("Scheduled JetAlgorithm instance \"jetalg_{0}\"".format(groomdef_i.fullname()))

    if returnConfiguredDef: return algs, groomdef_i
    return algs


def getJetAlgs(configFlags, jetdef, returnConfiguredDef=False, monTool=None):
    # Useful helper function For Run-II config style
    if isinstance(jetdef, JetDefinition):
        func = getJetDefAlgs
    elif isinstance(jetdef, GroomingDefinition):
        func = getJetGroomAlgs

    return func(configFlags, jetdef, returnConfiguredDef, monTool)


########################################################################
#
# Mid level functions returning specific type of algs out of JetDefinition
# functions below assumines the JetDefinition has its dependencies solved by a call to solveDependencies() 
# 


def getPseudoJetAlgs(jetdef):
    """ Builds the list of configured PseudoJetAlgorithm needed for this jetdef.
    THIS updates jetdef._internalAtt['finalPJContainer'] 
    (this function is factorized out of PseudoJetCfg so it can be used standalone in the trigger config)
    """
    
    constitpjalg = getConstitPJGAlg( jetdef.inputdef , suffix=None, flags=jetdef._cflags)
    #print("aaaa" , constitpjalg, constitpjalg.OutputContainer)
    finalPJContainer = str(constitpjalg.OutputContainer)
    pjalglist = [constitpjalg]
    
    # Schedule the ghost PseudoJetAlgs
    ghostlist = [ key for key in jetdef._prereqOrder if key.startswith('ghost:')]
    if ghostlist != []:
        # then we need to schedule a PseudoJetAlg for each ghost collections...
        pjContNames = [finalPJContainer]
        for ghostkey in sorted(ghostlist):
            ghostdef = jetdef._prereqDic[ghostkey]
            ghostpjalg = getGhostPJGAlg( ghostdef )
            pjalglist.append(ghostpjalg)
            pjContNames.append( str(ghostpjalg.OutputContainer) ) #

        # .. and merge them together with the input constituents
        mergeId = mergedPJId( pjContNames )
        finalPJContainer = str(finalPJContainer)+"_merged"+mergeId
        mergerName = "PJMerger_id"+mergeId
        mergeAlg =CompFactory.PseudoJetMerger(
            mergerName,
            InputPJContainers = pjContNames,
            OutputContainer = finalPJContainer,
        )
        pjalglist.append(mergeAlg)
        
    # set the name of the complete,merged input PseudoJets, so it can be re-used downstream
    jetdef._internalAtt['finalPJContainer'] = finalPJContainer
    return pjalglist
    

_mergedPJContainers = dict()
def mergedPJId(pjList):
    """returns a simple unique ID for the list of PseudoJet container in pjList"""
    t = tuple(str(n) for n in pjList) # make sure it is string (it can be DataHandle in old style config)
    currentSize = len(_mergedPJContainers)
    return str(_mergedPJContainers.setdefault(t, currentSize))


def getInputAlgs(jetOrConstitdef, configFlags=None, context="default", monTool=None):
    """Returns the list of configured algs needed to build inputs to jet finding as defined by jetOrConstitdef
    
    jetOrConstitdef can either be 
     * a JetDefinition : this happens when called from JetRecCfg or getJetDefAlgs then the jetdef._prereqDic/Order are used.
     * a JetInputConstit : to allow scheduling the corresponding constituents algs independently of any jet alg. 

    context is only used if jetOrConstitdef is not a JetDefinition and must refer to a context in StandardJetContext.

    The returned list may contain several algs, including constituent modifications algs, track selection, copying of
    input truth particles and event density calculations
    It may also contain ComponentAccumulator, only (?) in reco from RDO/RAW when we need to build externals such as clusters or tracks : in this case we call the main config functions from external packages)

    """

    from .JetDefinition import JetInputConstit, JetDefinition
    if isinstance(jetOrConstitdef, JetInputConstit):
        # technically we need a JetDefinition, so just build an empty one only containing our JetInputConstit
        jetlog.info("Setting up jet inputs from JetInputConstit : "+jetOrConstitdef.name)
        jetdef = solveDependencies( JetDefinition('Kt', 0., jetOrConstitdef, context=context) )
        jetdef._cflags = configFlags
        canrun = removeComponentFailingConditions(jetdef, raiseOnFailure= not jetdef.standardRecoMode)
        if not canrun:
            return []
    else:
        jetdef = jetOrConstitdef
    
    jetlog.info("Inspecting input file contents")

    # We won't prepare an alg if the input already exists in the in input file 
    try:
        filecontents = jetdef._cflags.Input.Collections
    except Exception:
        filecontents = []
    # local function to check if the container of the JetInputXXXX 'c' is already in filecontents :
    def isInInput( c ):
        cname = c.containername if isinstance(c, JetInputConstit) else c.containername(jetdef,c.specs)
        return cname in filecontents

    # Loop over all inputs required by jetdefs and get the corresponding algs
    inputdeps = [ inputkey for inputkey in jetdef._prereqOrder if inputkey.startswith('input:') or inputkey.startswith('extinput:') ]
    algs = []
    for inputfull in inputdeps:
        inputInstance = jetdef._prereqDic[inputfull]
        if isInInput( inputInstance ):
            jetlog.info(f"Input container for {inputInstance} already in input file.")
            continue

        # Get the input or external alg
        if isinstance(inputInstance, JetInputConstit):
            alg = getConstitModAlg(jetdef, inputInstance, monTool=monTool)
        else: # it must be a JetInputExternal
            alg = inputInstance.algoBuilder( jetdef, inputInstance.specs ) 

        if alg is not None:
            if isinstance( alg, list):
                # this can happen when running in runII style atlas config...
                algs+=alg
            else:
                algs.append(alg)

    return algs


########################################################################


def getPJContName( jetOrConstitdef, suffix=None):
    """Construct the name of the PseudoJetContainer defined by the given JetDef or JetInputConstit.
    This name has to be constructed from various places, so we factorize the definition here.
    """
    
    cdef = jetOrConstitdef if isinstance(jetOrConstitdef, JetInputConstit) else jetOrConstitdef.inputdef
    end = '' if suffix is None else f'_{suffix}'
    return f'PseudoJet{cdef.containername}{end}'
    
def getConstitPJGAlg(constitdef,suffix=None, flags=None):
    """returns a configured PseudoJetAlgorithm which converts the inputs defined by constitdef into fastjet::PseudoJet

    IMPORTANT : constitdef must have its dependencies solved (i.e. it must result from a solveDependencies() call)
    
    the flags argument is TEMPORARY and will be removed once further dev on PseudoJetAlgorithm is done (see comment below)
    """

    jetlog.debug("Getting PseudoJetAlg for label {0} from {1}".format(constitdef.name,constitdef.inputname))

    end = '' if suffix is None else f'_{suffix}'
    full_label = constitdef.label + end

    pjgalg = CompFactory.PseudoJetAlgorithm(
        "pjgalg_"+constitdef.containername+end,
        InputContainer = constitdef.containername,
        OutputContainer =getPJContName(constitdef,suffix),
        Label = full_label,
        SkipNegativeEnergy=True
        )

    
    # This is a terrible temporary hack to enable running in cosmic runs.
    # There should not be any Properties setting here in a helper function.
    # This will have to be fixed when all the filtering occuring in PseudoJetAlgorithm
    # is removed and done as part of a JetConstituentModSequence.
    if flags is not None:
        from AthenaConfiguration.Enums import BeamType        
        pjgalg.UseChargedPV = (flags.Beam.Type == BeamType.Collisions)
    
    if suffix == 'EMPFlowPUSB':
        pjgalg.UseChargedPV=False
        pjgalg.UseChargedPUsideband=True
    # end of HAck
        
    return pjgalg

def getGhostPJGAlg(ghostdef):
    """returns a configured PseudoJetAlgorithm which converts the inputs defined by constitdef into fastjet::PseudoJet
    
    The difference for the above is this is dedicated to ghosts which need variations for the Label and the muon segment cases.  

    IMPORTANT : ghostdef must have its dependencies solved (i.e. it must result from a solveDependencies() call)
    """
    label = "Ghost"+ghostdef.label # IMPORTANT !! "Ghost" in the label will be interpreted by the C++ side !

    kwargs = dict( 
        InputContainer = ghostdef.containername,
        OutputContainer= "PseudoJetGhost"+ghostdef.containername,
        Label=              label,
        SkipNegativeEnergy= True,
        #OutputLevel = 3,
    )

    pjaclass = CompFactory.PseudoJetAlgorithm
    if ghostdef.basetype=="MuonSegment":
        # Muon segments have a specialised type
        pjaclass = CompFactory.MuonSegmentPseudoJetAlgorithm
        kwargs.update( Pt =1e-20 ) # ??,)
        kwargs.pop('SkipNegativeEnergy')

    pjgalg = pjaclass( "pjgalg_"+label, **kwargs )
    return pjgalg


def getJetRecAlg( jetdef, monTool = None, ftf_suffix = ''):
    """Returns the configured JetRecAlg instance corresponding to jetdef

    IMPORTANT : jetdef must have its dependencies solved (i.e. it must result from solveDependencies() )
    """
    pjContNames = jetdef._internalAtt['finalPJContainer']
    jclust = CompFactory.JetClusterer(
        "builder",
        JetAlgorithm = jetdef.algorithm,#
        JetRadius = jetdef.radius,
        PtMin = jetdef.ptmin,
        InputPseudoJets = pjContNames,#
        GhostArea = 0.01, 
        JetInputType = int(jetdef.inputdef.jetinputtype),
        RandomOption = 1,
        VariableRMinRadius = jetdef.VRMinRadius,
        VariableRMassScale = jetdef.VRMassScale,
    )

    mods = getJetModifierTools(jetdef)

    jetname = jetdef.fullname()
    jra = CompFactory.JetRecAlg(
        "jetrecalg_"+jetname+ftf_suffix,
        Provider = jclust,
        Modifiers = mods,
        OutputContainer = jetname+ftf_suffix,
        )
    if monTool:
        # this option can't be set in AnalysisBase -> set only if explicitly asked :
        jra.MonTool = monTool

    return jra


def getJetRecGroomAlg(groomdef,monTool=None):
    """Returns a configured JetRecAlg set-up to perform the grooming defined by 'groomdef' 
    ('monTool' is a temporary placeholder, it is expected to be used in the trigger in the future) 
    """
    jetlog.debug("Configuring grooming alg \"jetalg_{0}\"".format(groomdef.fullname()))


    # the grooming tool (a IJetProvider instance)
    groomClass = CompFactory.getComp(groomdef.tooltype)
    groomer = groomClass(groomdef.groomalg,
                         UngroomedJets = groomdef.ungroomeddef.fullname(),
                         ParentPseudoJets = groomdef.ungroomeddef._internalAtt['finalPJContainer'],
                         **groomdef.properties)

    # get JetModifier list
    mods = getJetModifierTools(groomdef)

    # put everything together in a JetRecAlg
    jetname = groomdef.fullname()
    jra = CompFactory.JetRecAlg(
        "jetrecalg_"+jetname,
        Provider = groomer,
        Modifiers = mods,
        OutputContainer = jetname,
        MonTool = monTool)
    
    
    
    return jra


########################################################################
def getJetCopyAlg(jetsin, jetsoutdef, decorations=[], shallowcopy=True, shallowIO=True, monTool=None):
    """
    Get a JetRecAlg set up to copy a jet collection and apply mods
    In this setup we do not resolve dependencies because typically
    these may be set up already in the original jet collection
    In future we may wish to add a toggle.

    The decoration list can be set in order for the decorations
    (jet moments) on the original jets to be propagated to the
    copy collection. Beware of circular dependencies!
    """
    jcopy = CompFactory.JetCopier(
        "copier",
        InputJets = jetsin,
        DecorDeps=decorations,
        ShallowCopy=shallowcopy,
        ShallowIO=shallowIO)

    # Convert mod aliases into concrete tools
    mods = []
    for mod in jetsoutdef.modifiers:
        moddef = aliasToModDef(mod,jetsoutdef)
        mods.append(getModifier(jetsoutdef,moddef,moddef.modspec))

    jetsoutname = jetsoutdef.fullname()
    jra = CompFactory.JetRecAlg(
        "jetrecalg_copy_"+jetsoutname,
        Provider = jcopy,
        Modifiers = mods,
        OutputContainer = jetsoutname,
        MonTool = monTool)


    return jra



def getConstitModAlg(parentjetdef, constitSeq, monTool=None):
    """returns a configured JetConstituentModSequence or None if constit.modifiers == [] 
    
    The JetConstituentModSequence is determined by the JetInputConstitSeq constitSeq . 
    However, details of the configuration of the JetConstituentModSequence may depends on which JetDefinition
    this JetConstituentModSequence is intended for. Thus the function also requires a parentjetdef JetDefinition input
  
    IMPORTANT : parentjetdef & constitSeq must have their dependencies solved (i.e. they must result from solveDependencies() )
    
    See also getConstitModAlg_nojetdef
    """
    
    # JetInputConstit do not need any JetConstituentModSequence
    # (they are only needed to trigger the building of the source container and a PJ algo)
    if not isinstance(constitSeq, JetInputConstitSeq): return

    
    inputtype = constitSeq.basetype

    sequence = constitSeq.modifiers
    
    modlist = []

    #if modlist == []: return
    if constitSeq.inputname == constitSeq.containername: return
    
    for step in sequence:
        modInstance = parentjetdef._prereqDic[ f'cmod:{step}' ]
        if not modInstance.tooltype: continue

        toolclass = getattr( CompFactory, modInstance.tooltype)

        # update the properties : if some of them are function, just replace by calling this func :
        for k,v in modInstance.properties.items():
            if callable(v) :
                modInstance.properties[k ] = v(parentjetdef, constitSeq )
        
        tool =  toolclass(modInstance.name,**modInstance.properties)
        
        if (inputtype == xAODType.FlowElement or inputtype == xAODType.ParticleFlow) and modInstance.tooltype not in ["CorrectPFOTool","ChargedHadronSubtractionTool"]:
            tool.IgnoreChargedPFO=True
            tool.ApplyToChargedPFO=False
        tool.InputType = inputtype
        modlist.append(tool)

    sequenceshort = "".join(sequence)
    seqname = "ConstitMod{0}_{1}".format(sequenceshort,constitSeq.name)
    inputcontainer = str(constitSeq.inputname)
    outputcontainer = str(constitSeq.containername)
    if (inputtype == xAODType.FlowElement or inputtype == xAODType.ParticleFlow):
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
    )
    if monTool:
        modseq.MonTool = monTool

    constitmodalg = CompFactory.JetAlgorithm("jetalg_{0}".format(modseq.getName()))
    constitmodalg.Tools = [modseq]

    return constitmodalg

def getConstitModAlg_nojetdef( constitSeq, context="default", monTool=None):
    """Same as getConstitModAlg. 
    This is a convenient function to obtain a JetConstituentModSequence when it is certain, no JetDef is needed.
    This function just builds a dummy JetDefinition then calls getConstitModAlg
    Needed in the trigger config.
    """
    jetdef = solveDependencies( JetDefinition('Kt', 0., constitSeq, context=context) )
    constitSeq = jetdef._prereqDic['input:'+constitSeq.name] # retrieve the fully configured version of constitSeq
    return getConstitModAlg(jetdef, constitSeq, monTool=monTool)


def getJetModifierTools( jetdef ):
    """returns the list of configured JetModifier tools needed by this jetdef.
    This is done by instantiating the actual C++ tool as ordered in jetdef._prereqOrder
    """
    modlist = [ key for key in jetdef._prereqOrder if key.startswith('mod:')]
    
    mods = []
    for modkey in modlist:
        moddef = jetdef._prereqDic[modkey]
        modkey = modkey[4:] # remove 'mod:'
        modspec = '' if ':' not in modkey else modkey.split(':',1)[1]
        mod = getModifier(jetdef,moddef,modspec)
        mods.append(mod)

    return mods


def getModifier(jetdef, moddef, modspec):
    """Translate JetModifier into a concrete tool"""
    jetlog.verbose("Retrieving modifier {0}".format(str(moddef)))

    # Get the modifier tool
    try:
        modtool = moddef.createfn(jetdef, modspec)
    except Exception as e:
        jetlog.error( "Unhandled modifier specification {0} for mod {1} acting on jet def {2}!".format(modspec,moddef,jetdef.basename) )
        jetlog.error( "Received exception \"{0}\"".format(e) )
        jetlog.error( "Helper function is \"{0}\"".format(moddef.createfn) )
        raise ValueError( "JetModConfig unable to handle mod {0} with spec \"{1}\"".format(moddef,modspec) )


    # now we overwrite the default properties of the tool, by those
    # set in the moddef :
    for k,v in moddef.properties.items():
        if callable(v) :
            # The value we got is a function : we call it to get the actual value we want to set on the tool
            v = v(jetdef, modspec)
        setattr(modtool, k, v)
    
    return modtool


    
            
def removeComponentFailingConditions(jetdef, configflags=None, raiseOnFailure=True):
    """Filters the lists jetdef.modifiers and jetdef.ghosts (and jetdef._prereqOrder), so only the components
    comptatible with configflags are selected. 
    If configflags==None : assume jetdef._cflags is properly set (this is done by higher-level functions)
    The compatibility is ultimately tested using the component 'filterfn' attributes.
    Internally calls the function isComponentPassingConditions() (see below) 
    """
    jetlog.info("Standard Reco mode : filtering components in "+str(jetdef))

    if jetdef._cflags is None:
        jetdef._cflags = configflags

    ## TODO :
    ## do not raise an exceptin immediately. Instead collect all failure
    ## then report all of them, then raise

    fullname = jetdef.fullname()
    
    # define a helper function returning a filtered list of components.
    def filterList(inList, compType):
        nOut=0
        outList=[]
        basekey= compType+':' if compType!="" else ""
        # loop over components in the list to be filtered
        for comp in inList:
            fullkey = basekey+comp
            cInstance = jetdef._prereqDic[fullkey]
            ok, reason = isComponentPassingConditions(cInstance, jetdef._cflags, jetdef._prereqDic)
            if not ok :
                if raiseOnFailure:
                    raise Exception("JetDefinition {} can NOT be scheduled. Failure  of {} {}  reason={}".format(
                        jetdef, compType, comp, reason) )
                
                nOut+=1
                jetlog.info(f"{fullname} : removing {compType}  {comp}  reason={reason}")
                if fullkey in jetdef._prereqOrder: 
                    jetdef._prereqOrder.remove(fullkey)
                if compType=='ghost':
                    removeFromList(jetdef._prereqOrder, 'input:'+comp)
                    removeFromList(jetdef._prereqOrder, 'extinput:'+comp)
            else:
                outList.append(comp)
        jetlog.info(" *** Number of {} filtered components = {}  final  list={}".format(compType, nOut, outList) )
            
        return outList
    # ---------

    # ---------
    # first check if the input can be obtained. If not return.
    ok,reason = isComponentPassingConditions( jetdef.inputdef, jetdef._cflags, jetdef._prereqDic)
    if not ok:
        if raiseOnFailure:
            raise Exception(f"JetDefinition {jetdef} can NOT be scheduled. Failure  of input {jetdef.inputdef.name}  reason={reason}" )
        jetlog.info(f"IMPORTANT : removing {jetdef} because input incompatible with job conditions. Reason={reason} ")
        return False

    if isinstance( jetdef.inputdef, JetInputConstitSeq):
        # remove ConstitModifiers failing conditions.
        jetdef.inputdef.modifiers = filterList( jetdef.inputdef.modifiers, 'cmod' )
        
            
    
    # call the helper function to perform filtering :
    jetdef.ghostdefs = filterList( jetdef.ghostdefs, "ghost")
    jetdef.modifiers = filterList( jetdef.modifiers, "mod")
    # finally filter all possible intermediate dependency :
    filterList( list(jetdef._prereqOrder), "")
    return True



def removeGroomModifFailingConditions(groomdef, configflags, raiseOnFailure=True):

    mods_filtered = []

    for mod in groomdef.modifiers:
        fullkey = 'mod:'+mod
        cInstance = groomdef._prereqDic[fullkey]
        ok, reason = isComponentPassingConditions(cInstance, configflags, groomdef._prereqDic)

        if not ok :
            if raiseOnFailure:
                raise Exception("JetGrooming {} can NOT be scheduled. Failure  of {} {}  reason={}".format(
                groomdef, 'mod', mod, reason) )

            jetlog.info(f"{groomdef.fullname()} : removing modifier {mod}  reason={reason}")

            if fullkey in groomdef._prereqOrder:
                groomdef._prereqOrder.remove(fullkey)
        else:
            mods_filtered.append(mod)

    groomdef.modifiers = mods_filtered


def isComponentPassingConditions(component, configflags, prereqDic):
    """Test if component is compatible with configflags.
    This is done by calling component.filterfn AND testing all its prereqs.
    """
    for req in component.prereqs:
        if req not in prereqDic:
            return False, "prereq "+req+" not available"
        reqInstance = prereqDic[req]
        ok, reason = isComponentPassingConditions(reqInstance, configflags, prereqDic)
        if not ok :
            return False, "prereq "+str(reqInstance)+" failed because : "+reason

    ok, reason = component.filterfn(configflags)
    return ok, reason
    
    





def isAthenaRelease():
    return 'Ath' in os.environ.get("AtlasProject", "")

def isAnalysisRelease():
    return 'Analysis' in os.environ.get("AtlasProject", "")


def reOrderAlgs(algs):
    """In runIII the scheduler automatically orders algs, so the JetRecConfig helpers do not try to enforce the correct ordering.
    This is not the case in runII config for which this jobO is intended --> This function makes sure some jet-related algs are well ordered.
    """
    algs = [ a for a in algs if not isinstance(a, ComponentAccumulator)] 
    evtDensityAlgs = [(i, alg) for (i, alg) in enumerate(algs) if alg and alg.getType() == 'EventDensityAthAlg' ]
    pjAlgs = [(i, alg) for (i, alg) in enumerate(algs) if alg and alg.getType() == 'PseudoJetAlgorithm' ]
    pairsToswap = []
    for i, edalg in evtDensityAlgs:
        edInput = edalg.EventDensityTool.InputContainer
        for j, pjalg in pjAlgs:
            if j < i:
                continue 
            if edInput == str(pjalg.OutputContainer):
                pairsToswap.append((i, j))
    for i, j in pairsToswap:
        algs[i], algs[j] = algs[j], algs[i]
    return algs



def registerAsInputConstit( jetdef ):
    """Make the jet collection described by jetdef available as constituents to other jet finding 
   
    Technically : create  JetInputExternal and JetInputConstit and register them in the relevant look-up dictionnaries.
    the JetInputConstit will have a algoBuilder to generate the JetContainer described by jetdef
    """
    from .StandardJetConstits import stdConstitDic, stdInputExtDic
    jetname = jetdef.fullname()

    # define a function to generate the CA for this jetdef
    def jetBuilder(largejetdef,spec):
        from AthenaCommon.Configurable import Configurable
        if Configurable.configurableRun3Behavior :
            return JetRecCfg(largejetdef._cflags, jetdef)
        else:
            # Compatibility with runII style : we can't use ComponentAccumulator and must return the list of algs.
            #  When this is not needed anymore we can remove here and simplify inside getInputAlgs()
            algs, jetdef_i = getJetAlgs(largejetdef._cflags, jetdef, True)
            algs = reOrderAlgs( [a for a in algs if a is not None])
            return algs
            
    stdInputExtDic[jetname]  = JetInputExternal( jetname, jetname, algoBuilder=jetBuilder)
    stdConstitDic[jetname] = JetInputConstit(jetname, xAODType.Jet, jetname )
    

def removeFromList(l, o):
    if o in l:
        l.remove(o)

    
if __name__=="__main__":

    # Setting needed for the ComponentAccumulator to do its thing
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=True
    
    # Config flags steer the job at various levels
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ASG/mc16_13TeV.410501.PowhegPythia8EvtGen_A14_ttbar_hdamp258p75_nonallhad.merge.AOD.e5458_s3126_r9364_r9315/AOD.11182705._000001.pool.root.1"]
    ConfigFlags.Concurrency.NumThreads = 1
    ConfigFlags.Concurrency.NumConcurrentEvents = 1
    ConfigFlags.lock()

    # Get a ComponentAccumulator setting up the fundamental Athena job
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    cfg=MainServicesCfg(ConfigFlags) 

    # Add the components for reading in pool files
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(ConfigFlags))

    # Add the components from our jet reconstruction job
    from StandardSmallRJets import AntiKt4EMTopo
    AntiKt4EMTopo.modifiers = ["Calib:T0:mc","Filter:15000","Sort"] + ["JVT"] + ["PartonTruthLabel"]
    cfg.merge(JetRecCfg(AntiKt4EMTopo,ConfigFlags,jetnameprefix="New"))

    cfg.printConfig(withDetails=False,summariseProps=True)


