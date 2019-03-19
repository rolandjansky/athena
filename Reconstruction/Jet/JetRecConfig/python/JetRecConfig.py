# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

########################################################################
#                                                                      #
# JetRecConfig: A helper module for configuring jet reconstruction     #
# Author: TJ Khoo                                                      #
#                                                                      #
########################################################################

from AthenaCommon import Logging
jetlog = Logging.logging.getLogger('JetRecConfig')

import cppyy
try:
    cppyy.loadDictionary('xAODBaseObjectTypeDict')
except:
    pass
from ROOT import xAODType
xAODType.ObjectType

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

# CfgMgr is more convenient but it helps to be explicit about where
# things are defined.
# So, import package conf modules rather than a dozen individual classes
from JetRec import JetRecConf

__all__ = ["xAOD", "JetRecCfg", "resolveDependencies"]

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
def JetRecCfg(jetdef, configFlags, jetnameprefix="",jetnamesuffix=""):
    jetsfullname = jetnameprefix+jetdef.basename+jetnamesuffix+"Jets"
    jetlog.info("Setting up to find {0}".format(jetsfullname))

    sequencename = jetsfullname

    components = ComponentAccumulator()
    from AthenaCommon.AlgSequence import AthSequencer
    components.addSequence( AthSequencer(sequencename) )

    deps = resolveDependencies( jetdef )
    
    # Schedule the various input collections.
    # We don't have to worry about ordering, as the scheduler
    # will handle the details. Just merge the components.
    # 
    # To facilitate running in serial mode, we also prepare
    # the constituent PseudoJetGetter here (needed for rho)
    inputcomps, constitpjkey = JetInputCfgAndConstitPJName(deps["inputs"], configFlags, sequenceName=jetsfullname)
    components.merge(inputcomps)
    pjs = [constitpjkey]

    # Schedule the ghost PseudoJetGetterAlgs
    for ghostdef in deps["ghosts"]:
        ghostpjalg = GhostPJGAlg( ghostdef )
        components.addEventAlgo( ghostpjalg, sequencename )
        ghostpjkey = ghostpjalg.PJGetter.OutputContainer
        pjs.append( ghostpjkey )

    # Generate a JetAlgorithm to run the jet finding and modifiers
    # (via a JetRecTool instance).
    jetrecalg = getJetAlgorithm(jetsfullname, jetdef, pjs, deps["mods"])
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
    import JetModConfig
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
    for ghostdef in sorted(list(ghostdefs)):
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
        if not key in prereqdict.keys():
            prereqdict[key] = set()
        prereqdict[key].add(val)
            
    return prereqdict

########################################################################
# Function for expanding prerequisites into definitions
# Only supporting ghosts for now, but could be extended
#
def expandPrereqs(reqtype,prereqs):
    reqdefs = set()
    from JetDefinition import JetGhost
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
# Function for setting up inputs to jet finding
#
# This includes constituent modifications, track selection, copying of
# input truth particles and event density calculations
def JetInputCfgAndConstitPJName(inputdeps, configFlags, sequenceName):
    jetlog.info("Setting up jet inputs.")
    components = ComponentAccumulator(sequenceName)

    jetlog.info("Inspecting first input file")
    # Get the list of SG keys for the first input file
    # I consider it silly to run on a set of mixed file types
    firstinput = configFlags.Input.Files[0]
    import os, pickle
    from AthenaConfiguration.AutoConfigFlags import GetFileMD
    # PeekFiles returns a dict for each input file
    filecontents = GetFileMD([firstinput])["SGKeys"].split(' ')
    
    constit = inputdeps[0]
    # Truth and track particle inputs are handled later
    if constit.basetype not in [xAODType.TruthParticle, xAODType.TrackParticle]:
        # Protection against reproduction of existing containers
        if constit.inputname in filecontents:
            jetlog.debug("Input container {0} for label {1} already in input file.".format(constit.inputname, constit.label))
        else:
            jetlog.debug("Preparing Constit Mods for label {0} from {1}".format(constit.label,constit.inputname))
            # May need to generate constituent modifier sequences to
            # produce the input collection
            import ConstModHelpers
            constitalg = ConstModHelpers.ConstitModAlg(constit.basetype,constit.modifiers)
            components.addEventAlgo(constitalg)

    # Schedule the constituent PseudoJetGetterAlg
    constitpjalg = ConstitPJGAlg( constit )
    constitpjkey = constitpjalg.PJGetter.OutputContainer
    components.addEventAlgo( constitpjalg )

    # Track selection and vertex association kind of go hand in hand, though it's not
    # completely impossible that one might want one and not the other
    if "JetSelectedTracks" in inputdeps or "JetTrackVtxAssoc" in inputdeps:
        jetlog.debug("Setting up input track containers and track-vertex association")
        from JetRecTools import JetRecToolsConfig
        # Jet track selection
        jettrackselloose = JetRecToolsConfig.getTrackSelTool()
        jettvassoc = JetRecToolsConfig.getTrackVertexAssocTool()

        jettrkprepalg = JetRecConf.JetAlgorithm("jetalg_TrackPrep")
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

            tpcalg = JetRecConf.JetAlgorithm("jetalg_{0}".format(tpcname))
            tpcalg.Tools = [tpc]
            components.addEventAlgo(tpcalg)

        # Truth particles specifically for truth labels
        elif dep.startswith("TruthLabel"):
            truthlabel = dep[10:]
            tpcname = "truthpartcopy_"+truthlabel

            jetlog.debug("Setting up input truth particle container TruthLabel{0}".format(truthlabel))
            from ParticleJetTools.ParticleJetToolsConfig import getCopyTruthLabelParticles
            tpc = getCopyTruthLabelParticles(truthlabel)

            tpcalg = JetRecConf.JetAlgorithm("jetalg_{0}".format(tpcname))
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
                jetlog.debug("Event density {0} for label {1} already in input file.".format(rhokey, constit.label))
            else:
                rhotoolname = "EventDensity_Kt4"+constit.label

                jetlog.debug("Setting up event density calculation Kt4{0}".format(constit.label))
                from EventShapeTools import EventShapeToolsConf
                rhotool = EventShapeToolsConf.EventDensityTool(rhotoolname)
                rhotool.InputContainer = constitpjkey
                rhotool.OutputContainer = rhokey

                eventshapealg = EventShapeToolsConf.EventDensityAthAlg("{0}Alg".format(rhotoolname))
                eventshapealg.EventDensityTool = rhotool
                components.addEventAlgo(eventshapealg)

    return components, constitpjkey

########################################################################
# Functions for generating PseudoJetGetters, including determining
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

def ConstitPJGAlg(basedef):
    jetlog.debug("Getting PseudoJetAlg for label {0} from {1}".format(basedef.label,basedef.inputname))
    # 
    getter = JetRecConf.PseudoJetGetter("pjg_"+basedef.label,
        InputContainer = basedef.inputname,
        OutputContainer = "PseudoJet"+basedef.label,
        Label = basedef.label,
        SkipNegativeEnergy=True,
        GhostScale=0.
        )

    pjgalg = JetRecConf.PseudoJetAlgorithm(
        "pjgalg_"+basedef.label,
        PJGetter = getter
        )
    return pjgalg

def GhostPJGAlg(ghostdef):
    label = "Ghost"+ghostdef.inputtype
    kwargs = {
        "OutputContainer":    "PseudoJet"+label,
        "Label":              label,
        "SkipNegativeEnergy": True,
        "GhostScale":         1e-40
        }

    pjgclass = JetRecConf.PseudoJetGetter
    if ghostdef.inputtype=="MuonSegment":
        # Muon segments have a specialised type
        pjgclass = JetRecConf.MuonSegmentPseudoJetGetter
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

    getter = pjgclass("pjg_"+label, **kwargs)

    pjgalg = JetRecConf.PseudoJetAlgorithm(
        "pjgalg_"+label,
        PJGetter = getter
        )
    return pjgalg

########################################################################
# Function for configuring the jet algorithm and builders, given the
# set of dependencies
#
def getJetAlgorithm(jetname, jetdef, pjs, modlist):
    jetlog.debug("Configuring JetAlgorithm \"jetalg_{0}\"".format(jetname))

    builder = getJetBuilder()

    finder = getJetFinder(jetname, jetdef)
    finder.JetBuilder = builder

    import JetModConfig
    mods = []
    for moddef,modspec in modlist:
        mod = JetModConfig.getModifier(jetdef,moddef,modspec)
        mods.append(mod)

    rectool = getJetRecTool(jetname,finder,pjs,mods)

    jetalg = JetRecConf.JetAlgorithm("jetalg_"+jetname)
    jetalg.Tools = [rectool]

    return jetalg
    

########################################################################
# Function for generating a jet builder, i.e. converter from
# fastjet EDM to xAOD EDM
#
def getJetBuilder(doArea=True):
    # Do we have any reasons for not using the area one?
    # Maybe CPU reduction if we don't need areas for calibration
    builder = JetRecConf.JetFromPseudojet("jetbuild")
    if doArea:
        builder.Attributes = ["ActiveArea","ActiveAreaFourVector"]
    return builder

########################################################################
# Function for generating a jet finder, i.e. interface to fastjet
#
def getJetFinder(jetname, jetdef):
    finder = JetRecConf.JetFinder("jetfind_"+jetname,
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
    jetrec = JetRecConf.JetRecTool("jetrec_"+jetname,
        OutputContainer = jetname,
        InputPseudoJets = pjs,
        JetFinder = finder,
        JetModifiers = mods
    )
    return jetrec
