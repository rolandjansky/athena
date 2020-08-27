# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

########################################################################
#                                                                      #
# JetRecConfig: A helper module for configuring jet reconstruction     #
# Author: TJ Khoo                                                      #
#                                                                      #
########################################################################

from AthenaCommon import Logging
jetlog = Logging.logging.getLogger('JetRecConfig')

from xAODBase.xAODType import xAODType

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def propertiesOf(comp):
    """ Obtain properties irrespectively of the config system"""
    try:
        propNames = comp._descriptors
        return propNames
    except Exception: 
        pass
    return comp.properties()


__all__ = ["JetRecCfg", "resolveDependencies", "JetInputCfg"]

########################################################################
# Top-level function for running jet finding
# (i.e. clustering from inputs)
# This returns a ComponentAccumulator that can be merged with others
# from elsewhere in the job, but will provide everything needed to
# reconstruct one jet collection.
# This could still be modularised further into the subcomponents of the
# jet reconstruction job. For now, based on public tools, as private
# tool migration has not been completed.
#
# Receives the jet definition and input flags, mainly for input file
# peeking such that we don't attempt to reproduce stuff that's already
# in the input file
def JetRecCfg(jetdef, configFlags, jetnameprefix="",jetnamesuffix="", evsprefix="", jetnameoverride=None):
    # Ordinarily we want to have jet collection names be descriptive and derived from
    # the configured reconstruction.
    # Nevertheless, we allow an explicit specification when necessary
    # e.g. to ensure that the correct name is used in grooming operations
    if jetnameoverride:
        jetsfullname = jetnameoverride
    else:
        jetsfullname = jetnameprefix+jetdef.basename+"Jets"+jetnamesuffix
    jetlog.info("Setting up to find {0}".format(jetsfullname))

    sequencename = jetsfullname

    components = ComponentAccumulator()
    from AthenaCommon.CFElements import parOR
    components.addSequence( parOR(sequencename) )

    deps = resolveDependencies( jetdef )
    
    # Schedule the various input collections.
    # We don't have to worry about ordering, as the scheduler
    # will handle the details. Just merge the components.
    # 
    # To facilitate running in serial mode, we also prepare
    # the constituent PseudoJetAlgorithm here (needed for rho)
    inputcomps = JetInputCfg(deps["inputs"], configFlags, sequenceName=jetsfullname, evsprefix=evsprefix)
    constitpjalg = inputcomps.getPrimary()
    constitpjkey = constitpjalg.OutputContainer

    components.merge(inputcomps)
    pjs = [constitpjkey]

    # Schedule the ghost PseudoJetAlgs
    for ghostdef in deps["ghosts"]:
        ghostpjalg = getGhostPJGAlg( ghostdef )
        components.addEventAlgo( ghostpjalg, sequencename )
        ghostpjkey = ghostpjalg.OutputContainer
        pjs.append( ghostpjkey )

    # Generate a JetAlgorithm to run the jet finding and modifiers
    # (via a JetRecTool instance).
    mergepjalg = CompFactory.PseudoJetMerger(
        "pjmergealg_"+jetsfullname,
        InputPJContainers = pjs,
        OutputContainer = "PseudoJetMerged_"+jetsfullname)

    components.addEventAlgo(mergepjalg, sequencename)

    jetrecalg = getJetRecAlg(jetsfullname, jetdef, "PseudoJetMerged_"+jetsfullname, deps["mods"])
 
    components.addEventAlgo(jetrecalg, sequencename)

    jetlog.info("Scheduled JetAlgorithm instance \"jetalg_{0}\"".format(jetsfullname))
    return components

########################################################################
# The real workhorse -- establishes the full sequence of jet reco,
# recursively expanding the prerequisites
#
# Avoids constructing any configurables at this stage, the goal being
# to produce a human-readable job description.
def resolveDependencies(jetdef):

    jetlog.info("Resolving dependencies for {0} definition".format(jetdef.basename))

    # Accumulate prerequisites of the base constituent type
    # We just collect everything and sort out the types later
    prereqs = set() # Resolve duplication as we go
    prereqs.update( getConstitPrereqs( jetdef.inputdef ) )
    prereqs.update( set( ["input:"+dep for dep in jetdef.extrainputs] ) )

    # Add the Filter modifier if desired (usually it is)
    # It might be simpler to just eliminate ptminfilter
    # and always make this an explicit modifier
    mods_initial = list(jetdef.modifiers)
    if jetdef.ptminfilter>1e-9:
        filtstr = "Filter:{0:.0f}".format(jetdef.ptminfilter)
        # Insert pt filter after calibration if present
        idx=-1
        for imod, mod in enumerate(mods_initial):
            if mod.startswith("Calib"):
                idx = imod+1
                break
        mods_initial.insert(idx,filtstr)

    # Accumulate prerequisites of the modifiers, as these are
    # the most extensive. Internally resolves modifier chains,
    # returning an updated modifiers list
    # Need to use a list, as the order matters.
    # The elements of the "final" list are tuples extracting
    # the modifier specification.
    from . import JetModConfig
    mods_final, modprereqs = JetModConfig.getFinalModifierListAndPrereqs( mods_initial, jetdef )

    # Remove the duplicates in the mod list -- just do this
    # once at the end and preserve ordering.
    def dedupe(mylist):
        outlist = []
        usedset = set()
        for item in mylist:
            if not (item in usedset):
                outlist.append(item)
                usedset.add(item)
        return outlist
    mods_final = dedupe( mods_final )

    prereqs.update( modprereqs )

    # Ghost prerequisites are only of type input, so we can
    # afford to sort now.
    prereqdict = {"ghost":set(), "input":set()}
    prereqdict.update( classifyPrereqs(prereqs) )

    # Copy the explicitly requested ghost defs and add to
    # these those required by modifiers.
    ghostdefs = set(jetdef.ghostdefs).union(prereqdict["ghost"])
    # Expand from strings to JetGhost objects where needed.
    ghostdefs = expandPrereqs( "ghost",ghostdefs )
    
    # Accumulate prerequisites of the ghost-associated types
    jetlog.info("  Full list of ghosts: ")
    for ghostdef in sorted(list(ghostdefs), key=lambda g: g.inputtype):
        jetlog.info("    " + str(ghostdef))
        gprereqs = getGhostPrereqs(ghostdef)
        prereqdict["input"].update( [req.split(':',1)[1] for req in gprereqs] )

    jetlog.info("  Full list of mods: ")
    for mod, modspec in mods_final:
        jetlog.info("    " + str(mod) + ("" if not modspec else ": \"{0}\"".format(modspec)))

    # Return a dict of the dependencies, converting sets to lists.
    # May want to further separate input deps.
    dependencies = {
        "inputs":  [jetdef.inputdef] + sorted(list( prereqdict["input"] )),
        "ghosts":  list( ghostdefs ),
        "mods":    mods_final
        }

    # We don't expand the inputs at this stage, as they are diverse
    # and don't have a dedicated config class.
    # Doing so may trigger another level of expansion if the inputs
    # include a jet collection.
    return dependencies
    
########################################################################
# Function for classifying prerequisites
#
def classifyPrereqs(prereqs):
    prereqdict = {}
    for req in prereqs:
        key,val = req.split(":",1)
        jetlog.verbose( "Interpreted prereqs: {0} --> {1}".format(key,val) )
        if key not in prereqdict.keys():
            prereqdict[key] = set()
        prereqdict[key].add(val)
            
    return prereqdict

########################################################################
# Function for expanding prerequisites into definitions
# Only supporting ghosts for now, but could be extended
#
def expandPrereqs(reqtype,prereqs):
    reqdefs = set()
    from .JetDefinition import JetGhost
    for req in prereqs:
        if reqtype=="ghost":
            if req.__class__ == JetGhost:
                reqdefs.add( req )
            else:
                ghostdef = JetGhost(req)
                reqdefs.add( ghostdef )
                jetlog.debug("Expanded prereq {0} to {1}".format(req,ghostdef))
        else:
            jetlog.error("Prereqs \"{0}\" unsupported!".format(reqtype))
            return None              
    return reqdefs

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


########################################################################
# Function producing an EventShapeAlg to calculate
# medaian energy density for pileup correction
#
def getEventShapeAlg( constit, constitpjkey, evsprefix="" ):

    rhokey = evsprefix+"Kt4"+constit.label+"EventShape"
    rhotoolname = "EventDensity_{}Kt4{}".format(evsprefix,constit.label)
    
    rhotool = CompFactory.EventDensityTool(rhotoolname)
    rhotool.InputContainer = constitpjkey
    rhotool.OutputContainer = rhokey
    
    eventshapealg = CompFactory.EventDensityAthAlg("{0}{1}Alg".format(evsprefix,rhotoolname))
    eventshapealg.EventDensityTool = rhotool

    return eventshapealg

########################################################################
# Function for setting up inputs to jet finding
#
# This includes constituent modifications, track selection, copying of
# input truth particles and event density calculations
def JetInputCfg(inputdeps, configFlags, sequenceName, evsprefix=""):
    jetlog.info("Setting up jet inputs.")
    components = ComponentAccumulator(sequenceName)

    jetlog.info("Inspecting input file contents")
    filecontents = [i for i in configFlags.Input.Collections]

    constit = inputdeps[0]
    # Truth and track particle inputs are handled later
    if constit.basetype not in [xAODType.TruthParticle, xAODType.TrackParticle] and constit.inputname!=constit.rawname:
        # Protection against reproduction of existing containers
        if constit.inputname in filecontents:
            jetlog.debug("Input container {0} for label {1} already in input file.".format(constit.inputname, constit.label))
        else:
            jetlog.debug("Preparing Constit Mods for label {0} from {1}".format(constit.label,constit.inputname))
            # May need to generate constituent modifier sequences to
            # produce the input collection
            from . import ConstModHelpers
            constitalg = ConstModHelpers.getConstitModAlg(constit)
            if constitalg:
                components.addEventAlgo(constitalg)

    # Schedule the constituent PseudoJetAlg
    constitpjalg = getConstitPJGAlg( constit )
    constitpjkey = constitpjalg.OutputContainer
    # Mark the constit PJGAlg as the primary so that the caller
    # can access the output container name
    components.addEventAlgo( constitpjalg, primary=True )

    # Track selection and vertex association kind of go hand in hand, though it's not
    # completely impossible that one might want one and not the other
    if "JetSelectedTracks" in inputdeps or "JetTrackVtxAssoc" in inputdeps:
        jetlog.debug("Setting up input track containers and track-vertex association")
        from JetRecTools import JetRecToolsConfig
        # Jet track selection
        jettrackselloose = JetRecToolsConfig.getTrackSelTool(doWriteTracks=True)
        jettvassoc = JetRecToolsConfig.getTrackVertexAssocTool()

        jettrkprepalg = CompFactory.JetAlgorithm("jetalg_TrackPrep")
        jettrkprepalg.Tools = [ jettrackselloose, jettvassoc ]
        components.addEventAlgo( jettrkprepalg )

    # Resolve the rest of the input dependencies
    for dep in inputdeps[1:]:
        # Generate prequisite truth particle collections
        # There may be more than one.
        if dep.startswith("JetInputTruthParticles"):
            # Special conditions e.g. "WZ" are set as a suffix preceded by ":"
            truthmod = ''
            if ":" in dep:
                truthmod = dep.split(':')[1]
            tpcname = "truthpartcopy"+truthmod
            jetlog.debug("Setting up input truth particle container JetInputTruthParticles{0}".format(truthmod))

            from ParticleJetTools.ParticleJetToolsConfig import getCopyTruthJetParticles
            tpc = getCopyTruthJetParticles(truthmod)

            tpcalg = CompFactory.JetAlgorithm("jetalg_{0}".format(tpcname))
            tpcalg.Tools = [tpc]
            components.addEventAlgo(tpcalg)

        # Truth particles specifically for truth labels
        elif dep.startswith("TruthLabel"):
            truthlabel = dep[10:]
            tpcname = "truthpartcopy_"+truthlabel

            jetlog.debug("Setting up input truth particle container TruthLabel{0}".format(truthlabel))
            from ParticleJetTools.ParticleJetToolsConfig import getCopyTruthLabelParticles
            tpc = getCopyTruthLabelParticles(truthlabel)

            tpcalg = CompFactory.JetAlgorithm("jetalg_{0}".format(tpcname))
            tpcalg.Tools = [tpc]
            components.addEventAlgo(tpcalg)

        # Calculate the event density for jet area subtraction taking the
        # jet constituents as input
        # Possibly not needed if constituent suppression has been applied.
        # Will want to update the standalone ED python for other uses,
        # e.g. isolation or rho from constituents that are not used to
        # build a particular jet collection (e.g. neutral PFOs)
        #
        # Needs protection against reproduction of existing containers
        elif dep == "EventDensity":
            rhokey = "Kt4"+constit.label+"EventShape"
            if rhokey in filecontents:
                jetlog.info("Event density {0} for label {1} already in input file.".format(rhokey, constit.label))
            else:
                components.addEventAlgo( getEventShapeAlg(constit,constitpjkey,evsprefix) )

    return components

########################################################################
# Functions for generating PseudoJetAlgorithms, including determining
# the prerequisites for their operation
#
def getConstitPrereqs(basedef):
    prereqs = []
    if basedef.basetype==xAODType.TrackParticle:
        prereqs = ["input:JetSelectedTracks","input:JetTrackVtxAssoc"]
    elif basedef.basetype==xAODType.TruthParticle:
        prereqs = ["input:JetInputTruthParticles:"+basedef.inputname[22:]]
    return prereqs

def getGhostPrereqs(ghostdef):
    jetlog.verbose("Getting ghost PseudoJets of type {0}".format(ghostdef.inputtype))

    prereqs = []
    if ghostdef.inputtype=="Track":
        prereqs = ["input:JetSelectedTracks","input:JetTrackVtxAssoc"]
    elif ghostdef.inputtype.startswith("TruthLabel"):
        truthsuffix = ghostdef.inputtype[5:]
        prereqs = ["input:TruthLabel"+truthsuffix]
    elif ghostdef.inputtype == "Truth":
        prereqs = ["input:JetInputTruthParticles"]
    return prereqs

def getConstitPJGAlg(basedef):
    jetlog.debug("Getting PseudoJetAlg for label {0} from {1}".format(basedef.label,basedef.inputname))
    # 
    full_label = basedef.label
    if basedef.basetype == xAODType.Jet:
        full_label += "_"+basedef.inputname

    pjgalg = CompFactory.PseudoJetAlgorithm(
        "pjgalg_"+basedef.label,
        InputContainer = basedef.inputname,
        OutputContainer = "PseudoJet"+full_label,
        Label = full_label,
        SkipNegativeEnergy=True
        )
    return pjgalg

def getGhostPJGAlg(ghostdef):
    label = "Ghost"+ghostdef.inputtype
    kwargs = {
        "OutputContainer":    "PseudoJet"+label,
        "Label":              label,
        "SkipNegativeEnergy": True
        }

    pjaclass = CompFactory.PseudoJetAlgorithm
    if ghostdef.inputtype=="MuonSegment":
        # Muon segments have a specialised type
        pjaclass = CompFactory.MuonSegmentPseudoJetAlgorithm
        kwargs = {
            "InputContainer":"MuonSegments",
            "OutputContainer":"PseudoJet"+label,
            "Label":label,
            "Pt":1e-20
            }
    elif ghostdef.inputtype=="Track":
        kwargs["InputContainer"] = "JetSelectedTracks"
    elif ghostdef.inputtype.startswith("TruthLabel"):
        truthsuffix = ghostdef.inputtype[5:]
        kwargs["InputContainer"] = "TruthLabel"+truthsuffix
    elif ghostdef.inputtype == "Truth":
        kwargs["InputContainer"] = "JetInputTruthParticles"
    else:
        raise ValueError("Unhandled ghost type {0} received!".format(ghostdef.inputtype))

    pjgalg = pjaclass(
        "pjgalg_"+label,
        **kwargs
        )
    return pjgalg

########################################################################
# Function for configuring the jet algorithm and builders, given the
# set of dependencies
#
def getJetAlgorithm(jetname, jetdef, pjs, modlist, monTool = None):
    jetlog.debug("Configuring JetAlgorithm \"jetalg_{0}\"".format(jetname))

    builder = getJetBuilder()

    finder = getJetFinder(jetname, jetdef)
    finder.JetBuilder = builder

    from . import JetModConfig
    mods = []
    for moddef,modspec in modlist:
        mod = JetModConfig.getModifier(jetdef,moddef,modspec)
        mods.append(mod)

    rectool = getJetRecTool(jetname,finder,pjs,mods)
    if monTool: rectool.MonTool = monTool

    jetalg = CompFactory.JetAlgorithm("jetalg_"+jetname)
    jetalg.Tools = [rectool]

    return jetalg

########################################################################
# Function for generating a jet builder, i.e. converter from
# fastjet EDM to xAOD EDM
#
def getJetBuilder(doArea=True):
    # Do we have any reasons for not using the area one?
    # Maybe CPU reduction if we don't need areas for calibration
    builder = CompFactory.JetFromPseudojet("jetbuild")
    if doArea:
        builder.Attributes = ["ActiveArea","ActiveAreaFourVector"]
    return builder

########################################################################
# Function for generating a jet finder, i.e. interface to fastjet
#
def getJetFinder(jetname, jetdef):
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
    AntiKt4EMTopo.ptminfilter = 15e3
    AntiKt4EMTopo.modifiers = ["Calib:T0:mc","Sort"] + ["JVT"] + ["PartonTruthLabel"]
    cfg.merge(JetRecCfg(AntiKt4EMTopo,ConfigFlags,jetnameprefix="New"))

    cfg.printConfig(withDetails=False,summariseProps=True)

    cfg.run(maxEvents=10)

    import sys
    sys.exit(0)

########################################################################
# Function that substitues JetRecTool + JetAlgorithm
#
def getJetRecAlg(jetname, jetdef, pjs, modlist):

    jclust = CompFactory.JetClusterer("builder")
    jclust.JetAlgorithm = jetdef.algorithm
    jclust.JetRadius = jetdef.radius
    jclust.PtMin = jetdef.ptmin
    jclust.InputPseudoJets = pjs
    jclust.GhostArea = 0.01 # In which cases do we not want areas?
    jclust.JetInputType = jetdef.inputdef.basetype

    from . import JetModConfig
    mods = []
    for moddef,modspec in modlist:
        mod = JetModConfig.getModifier(jetdef,moddef,modspec)
        mods.append(mod)

    jra = CompFactory.JetRecAlg(
        "jetrecalg_"+jetname,
        Provider = jclust,
        Modifiers = mods,
        OutputContainer = jetname)

    autoconfigureModifiers(jra.Modifiers, jetname)

    return jra
