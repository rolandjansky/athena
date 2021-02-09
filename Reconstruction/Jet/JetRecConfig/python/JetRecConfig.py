# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""
JetRecConfig: A helper module for configuring jet reconstruction     

Author: TJ Khoo, P-A Delsart                                                      
"""

########################################################################

from AthenaCommon import Logging
jetlog = Logging.logging.getLogger('JetRecConfig')


from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

import JetRecConfig.ConstModHelpers as constH
import JetRecConfig.JetModConfig as modH

from JetRecConfig.JetDefinition import JetDefinition
from JetRecConfig.JetGrooming import GroomingDefinition




__all__ = ["JetRecCfg", "JetInputCfg"]



########################################################################


def JetRecCfg(jetdef, configFlags):
    """Top-level function for running jet finding or grooming.
    
    This returns a ComponentAccumulator that can be merged with others
    from elsewhere in the job, but will provide everything needed to
    reconstruct one jet collection.
    Receives the jet or grooming definition (jetdef) and input flags (configFlags), mainly for input file
    peeking such that we don't attempt to reproduce stuff that's already
    in the input file.

    """
    # we clone the jetdef, so we're sure we're not using a 'locked' one 
    jetdef_i = jetdef.clone()
    
    sequenceName = jetdef_i.fullname()
    jetlog.info("******************")    
    jetlog.info("Setting up to find {0}".format(sequenceName))

    components = ComponentAccumulator()
    from AthenaCommon.CFElements import parOR
    components.addSequence( parOR(sequenceName) )

    # call the relevant function according to jetdef_i type 
    if isinstance(jetdef_i, JetDefinition):
        addJetClusteringToComponent(components, jetdef_i, configFlags)

    elif isinstance(jetdef_i, GroomingDefinition):
        from JetRecConfig.JetGroomConfig import addGroomToComponent
        addGroomToComponent(components, jetdef_i,  configFlags)

    return components

def addJetClusteringToComponent(components, jetdef_i, configFlags, sequenceName=None):
    """internal function which instantiates the JetRecAlg defined by the JetDefinition 'jetdef_i'  
    into the ComponentAccumulator 'components' 
    """
    sequenceName = sequenceName or jetdef_i.fullname()
    # create proper config instances for each input and ghost aliases in this jetdef_i
    # this implicitely calculates and adds the dependencies.
    instantiateAliases(jetdef_i)
    
    # check if the conditions are compatible with the inputs & modifiers of this jetdef_i.
    # if in standardRecoMode we will remove whatever is incompatible and still try to run
    # if not, we raise an exception
    removeComponentFailingConditions(jetdef_i, configFlags, raiseOnFailure= not jetdef_i.standardRecoMode)
    
    
    # Schedule the various input collections.
    # We don't have to worry about ordering, as the scheduler
    # will handle the details. Just merge the components.
    inputcomps = JetInputCfg(jetdef_i, configFlags)
    components.merge(inputcomps, sequenceName=sequenceName)

    # schedule the algs to create fastjet::PseudoJet objects out of the inputs
    pjCompo= PseudoJetCfg(jetdef_i, configFlags)
    components.merge(pjCompo, sequenceName=sequenceName)
    
    # Generate a JetRecAlg to run the jet finding and modifiers
    jetrecalg = getJetRecAlg( jetdef_i)
    components.addEventAlgo(jetrecalg, sequenceName=sequenceName)
    
    jetlog.info("Scheduled JetAlgorithm instance \"jetalg_{0}\"".format(jetdef_i.fullname()))

    return components


def buildPseudoJetAlgs(jetdef):
    """ Builds the list of configured PseudoJetAlgorithm needed for this jetdef.
    THIS updates jetdef._internalAtt['finalPJContainer'] 
    (this function is factorized out of PseudoJetCfg so it can be used standalone in the trigger config)
    """
    
    constitpjalg = getConstitPJGAlg( jetdef.inputdef )
    finalPJContainer = constitpjalg.OutputContainer
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
            pjContNames.append( ghostpjalg.OutputContainer )

        # .. and merge them together with the input constituents 
        mergeId = mergedPJId( pjContNames )
        finalPJContainer = constitpjalg.OutputContainer+"_merged"+mergeId
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
    
def PseudoJetCfg(jetdef, configFlags):
    """Builds a ComponentAccumulator for creating PseudoJetContainer needed by jetdef.
    THIS updates jetdef._internalAtt['finalPJContainer'] 
    """
    components = ComponentAccumulator()
    pjalglist = buildPseudoJetAlgs(jetdef)
    for pjalg in pjalglist:
        components.addEventAlgo(pjalg)
    return components

_mergedPJContainers = dict()
def mergedPJId(pjList):
    """returns a simple unique ID for the list of PseudoJet container in pjList"""
    t = tuple(pjList)
    currentSize = len(_mergedPJContainers)
    return str(_mergedPJContainers.setdefault(t, currentSize))


########################################################################
def JetInputCfg(jetOrConstitdef, configFlags):
    """Function for setting up inputs to jet finding

    This includes constituent modifications, track selection, copying of
    input truth particles and event density calculations
    
    jetOrConstitdef can either be 
     * a JetDefinition : this happens when called from JetRecCfg, then the jetdef._prereqDic/Order are used.
     * a JetConstitSource : to allow scheduling the corresponding constituents algs independently of any jet alg. 
    """
    components = ComponentAccumulator()

    
    from .JetDefinition import JetConstitSource, JetDefinition
    if isinstance(jetOrConstitdef, JetConstitSource):
        jetlog.info("Setting up jet inputs from JetConstitSource : "+jetOrConstitdef.name)
        jetdef = JetDefinition('Kt', 0., jetOrConstitdef.clone())
        instantiateAliases(jetdef)        
        removeComponentFailingConditions(jetdef, configFlags, raiseOnFailure= not jetdef.standardRecoMode)
    else:
        jetdef = jetOrConstitdef
    
    jetlog.info("Inspecting input file contents")
    filecontents = configFlags.Input.Collections

    inputdeps = [ inputkey for inputkey in jetdef._prereqOrder if inputkey.startswith('input:')]

    for inputfull in inputdeps:
        inputInstance = jetdef._prereqDic[inputfull]
        isprimary = False # actually not using it yet.
        
        if isinstance(inputInstance, JetConstitSource):
            if inputInstance.containername in filecontents:
                jetlog.debug("Input container {0} for label {1} already in input file.".format(inputInstance.containername, inputInstance.name))
            else:
                jetlog.debug("Preparing Constit Mods for label {0} from {1}".format(inputInstance.name,inputInstance.inputname))
                # May need to generate constituent modifier sequences to
                # produce the input collection
                from . import ConstModHelpers
                constitalg = ConstModHelpers.getConstitModAlg(inputInstance)
                if constitalg:
                    components.addEventAlgo(constitalg, primary=isprimary)
        else: # it must be a JetInputDef
            cname = inputInstance.containername(jetdef,inputInstance.specs) # (by defaults this is just inputInstance.name)
            if cname in filecontents:
                jetlog.debug("Input container {0} for prereq {1} already in input file.".format(cname, inputInstance.name))
            else:
                jetlog.debug("Requesting input {} with function {} and specs {}".format(inputInstance.name, inputInstance.algoBuilder, inputInstance.specs) )
                # check if it has something to build an Algorithm
                if inputInstance.algoBuilder:
                    components.addEventAlgo( inputInstance.algoBuilder( jetdef, inputInstance.specs ), primary=isprimary )
                else:
                    # for now just hope the input will be present... 
                    pass
    return components


def getConstitPJGAlg(constitdef):
    """returns a configured PseudoJetAlgorithm which converts the inputs defined by constitdef into fastjet::PseudoJet"""
    
    jetlog.debug("Getting PseudoJetAlg for label {0} from {1}".format(constitdef.name,constitdef.inputname))

    full_label = constitdef.label
    
    pjgalg = CompFactory.PseudoJetAlgorithm(
        "pjgalg_"+constitdef.label,
        InputContainer = constitdef.containername,
        OutputContainer = "PseudoJet"+full_label,
        Label = full_label,
        SkipNegativeEnergy=True
        )
    return pjgalg

def getGhostPJGAlg(ghostdef):
    """returns a configured PseudoJetAlgorithm which converts the inputs defined by constitdef into fastjet::PseudoJet
    
    The difference for the above is this is dedicated to ghosts which need variations for the Label and the muon segment cases.  
    """
    label = "Ghost"+ghostdef.label # IMPORTANT !! "Ghost" in the label will be interpreted by teh C++ side !

    kwargs = dict( 
        InputContainer = ghostdef.containername,
        OutputContainer=    "PseudoJet"+label,
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


def buildJetModifierList( jetdef ):
    """returns the list of configured JetModifier tools needed by this jetdef.
    This is done by instantiating the actual C++ tool as ordered in jetdef._prereqOrder
    """
    modlist = [ key for key in jetdef._prereqOrder if key.startswith('mod:')]
    
    from . import JetModConfig
    mods = []
    for modkey in modlist:
        moddef = jetdef._prereqDic[modkey]
        modkey = modkey[4:] # remove 'mod:'
        modspec = '' if ':' not in modkey else modkey.split(':',1)[1]
        mod = JetModConfig.getModifier(jetdef,moddef,modspec)
        mods.append(mod)

    return mods
        
def getJetAlgorithm(jetname, jetdef, pjContNames, monTool = None):
    """returns a configured JetAlgorithm """
    jetlog.debug("Configuring JetAlgorithm \"jetalg_{0}\"".format(jetname))

    builder = getJetBuilder()

    finder = getJetFinder(jetname, jetdef)
    finder.JetBuilder = builder

    mods = buildJetModifierList(jetdef)
    
    rectool = getJetRecTool(jetname,finder,pjContNames,mods)
    if monTool: rectool.MonTool = monTool

    jetalg = CompFactory.JetAlgorithm("jetalg_"+jetname)
    jetalg.Tools = [rectool]

    return jetalg

########################################################################
# New JetRecAlgorithm to replace JetRecTool
# This call is for a JRA that runs jet-finding
#
def getJetRecAlg( jetdef, monTool = None):
    """ """
    pjContNames = jetdef._internalAtt['finalPJContainer']
    jclust = CompFactory.JetClusterer(
        "builder",
        JetAlgorithm = jetdef.algorithm,
        JetRadius = jetdef.radius,
        PtMin = jetdef.ptmin,
        InputPseudoJets = pjContNames,
        GhostArea = 0.01, 
        JetInputType = int(jetdef.inputdef.jetinputtype),
        RandomOption = 1,
    )

    mods = buildJetModifierList(jetdef)

    jetname = jetdef.fullname()
    jra = CompFactory.JetRecAlg(
        "jetrecalg_"+jetname,
        Provider = jclust,
        Modifiers = mods,
        OutputContainer = jetname,
        MonTool = monTool)

    autoconfigureModifiers(jra.Modifiers, jetname)

    return jra


########################################################################
# Get a JetRecAlg set up to copy a jet collection and apply mods
# In this setup we do not resolve dependencies because typically
# these may be set up already in the original jet collection
# In future we may wish to add a toggle.
#
# The decoration list can be set in order for the decorations
# (jet moments) on the original jets to be propagated to the
# copy collection. Beware of circular dependencies!
def getJetCopyAlg(jetsin, jetsoutdef, decorations=[], shallowcopy=True, shallowIO=True, monTool=None):

    jcopy = CompFactory.JetCopier(
        "copier",
        InputJets = jetsin,
        DecorDeps=decorations,
        ShallowCopy=shallowcopy,
        ShallowIO=shallowIO)

    # Convert mod aliases into concrete tools
    from . import JetModConfig
    mods = []
    for mod in jetsoutdef.modifiers:
        moddef = JetModConfig.aliasToModDef(mod,jetsoutdef)
        mods.append(JetModConfig.getModifier(jetsoutdef,moddef,moddef.modspec))

    jetsoutname = jetsoutdef.fullname()
    jra = CompFactory.JetRecAlg(
        "jetrecalg_copy_"+jetsoutname,
        Provider = jcopy,
        Modifiers = mods,
        OutputContainer = jetsoutname,
        MonTool = monTool)

    autoconfigureModifiers(jra.Modifiers, jetsoutname)

    return jra


########################################################################
# For each modifier in the given list with a configurable input container
# name ("JetContainer"), configure it to containerName.
# Also handle any container-specific configuration needed.
def autoconfigureModifiers(modifiers, containerName):
    for mod in modifiers:
        if "JetContainer" in propertiesOf( mod ):
            mod.JetContainer = containerName
        if "DoPFlowMoments" in propertiesOf( mod ):
            mod.DoPFlowMoments = ("PFlow" in containerName)


def propertiesOf(comp):
    """ Obtain properties irrespectively of the config system"""
    try:
        propNames = comp._descriptors
        return propNames
    except Exception: 
        pass
    return comp.properties()
            
########################################################################
def getJetBuilder(doArea=True):
    """Returns  a jet builder (JetFromPseudojet) , i.e. converter from
    fastjet EDM to xAOD EDM
    """
    # Do we have any reasons for not using the area one?
    # Maybe CPU reduction if we don't need areas for calibration
    builder = CompFactory.JetFromPseudojet("jetbuild")
    if doArea:
        builder.Attributes = ["ActiveArea","ActiveAreaFourVector"]
    return builder

########################################################################
#
def getJetFinder(jetname, jetdef):
    """Creates a jet finder, i.e. interface to fastjet"""
    finder = CompFactory.JetFinder("jetfind_"+jetname,
        JetAlgorithm = jetdef.algorithm,
        JetRadius = jetdef.radius,
        PtMin = jetdef.ptmin,
        GhostArea = 0.01,
        RandomOption = 1,
    )
    return finder

########################################################################
# Function for generating a JetRecTool
#
def getJetRecTool(jetname, finder, pjs, mods):
    # Create the JetRecTool and pass the inputs
    jetrec = CompFactory.JetRecTool("jetrec_"+jetname,
        OutputContainer = jetname,
        InputPseudoJets = pjs,
        JetFinder = finder,
        JetModifiers = mods )
    autoconfigureModifiers(jetrec.JetModifiers, jetname)
    return jetrec



def instantiateAliases( jetdef ):
    """ Instantiate all the aliases contained in this jetdef : modifiers, ghosts and prereqs.
    At the same time fills the internal _prereqDic and _prereqOrder containers.
    
    This functions 
      * assumes jetdef is not 'locked' 
      * implies calls to recursives function constH.aliasToInputDef and modH.aliasToModDef
    """

    # start with the inputdef, cloning it so we're not altering a private copy
    jetdef.inputdef = jetdef.inputdef.clone()
    constH.instantiateJetConstitAliases(jetdef.inputdef, jetdef)

    jetdef._prereqDic['input:'+jetdef.inputdef.name] = jetdef.inputdef
    jetdef._prereqOrder.append('input:'+jetdef.inputdef.name)

    for g in jetdef.extrainputs:
        gInstance = constH.aliasToInputDef( g , jetdef)
        jetdef._prereqDic['input:'+g] = gInstance
        jetdef._prereqOrder.append('input:'+g)
    
    for g in jetdef.ghostdefs:
        gInstance = constH.aliasToInputDef( g , jetdef)
        jetdef._prereqDic['input:'+g] = gInstance
        jetdef._prereqOrder.append('input:'+g)
        jetdef._prereqDic['ghost:'+g] = gInstance
        jetdef._prereqOrder.append('ghost:'+g)

    for mod in jetdef.modifiers:
        modInstance = modH.aliasToModDef(mod, jetdef)
        jetdef._prereqDic['mod:'+mod] = modInstance
        jetdef._prereqOrder.append('mod:'+mod)

    # Deduplicate the prereq (with python > 3.6 dict is ordered so the trick is guaranteed to work) :
    jetdef._prereqOrder[:] = list(dict.fromkeys(jetdef._prereqOrder) )


    
            
def removeComponentFailingConditions(jetdef, configflags, raiseOnFailure=True):
    """Filters the lists jetdef.modifiers and jetdef.ghosts (and jetdef._prereqOrder), so only the components
    comptatible with configflags are selected. 
    The compatibility is ultimately tested using the component 'filterfn' attributes.
    Internally calls the function isComponentPassingConditions() (see below) 
    """
    jetlog.info("Standard Reco mode : filtering components in "+str(jetdef))


    ## TODO :
    ## do not raise an exceptin immediately. Instead collect all failure
    ## then report all of them, then raise
    
    # define a helper function returning a filtered list of components.
    def filterList(inList, compType):
        nOut=0
        outList=[]
        # loop over components in the list to be filtered
        for comp in inList:
            fullkey = compType+':'+comp
            cInstance = jetdef._prereqDic[fullkey]
            ok, reason = isComponentPassingConditions(cInstance, configflags, jetdef._prereqDic)
            if not ok :
                if raiseOnFailure:
                    raise Exception("JetDefinition {} can NOT be scheduled. Failure  of {} {}  reason={}".format(
                        jetdef, compType, comp, reason) )
                
                nOut+=1
                jetlog.info("IMPORTANT : removing {}  {}  reason={} ".format(compType, comp, reason))
                jetdef._prereqOrder.remove(fullkey)
            else:
                outList.append(comp)
        jetlog.info(" *** Number of {} filtered components = {}  final  list={}".format(compType, nOut, outList) )
        return outList
    # ---------
    
    # call the helper function to perform filtering :
    jetdef.ghostdefs = filterList( jetdef.ghostdefs, "ghost")
    jetdef.modifiers = filterList( jetdef.modifiers, "mod")

def isComponentPassingConditions(component, configflags, prereqDic):
    """Test if component is compatible with configflags.
    This is done by calling component.filterfn AND testing all its prereqs.
    """
    from .JetDefinition import JetModifier, JetConstitModifier
    if isinstance(component, (JetModifier, JetConstitModifier)):
        for req in component.prereqs:
            if req not in prereqDic:
                return False, "prereq "+req+" not available"
            reqInstance = prereqDic[req]
            ok, reason = isComponentPassingConditions(reqInstance, configflags, prereqDic)
            if not ok :
                return False, "prereq "+str(component)+" failed."

    ok, reason = component.filterfn(configflags)
    return ok, reason
    
    



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
    from StandardJetDefs import AntiKt4EMTopo
    AntiKt4EMTopo.modifiers = ["Calib:T0:mc","Filter:15000","Sort"] + ["JVT"] + ["PartonTruthLabel"]
    cfg.merge(JetRecCfg(AntiKt4EMTopo,ConfigFlags,jetnameprefix="New"))

    cfg.printConfig(withDetails=False,summariseProps=True)


