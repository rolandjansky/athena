#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.CFElements import parOR
from TriggerMenuMT.HLT.Menu.ChainConfigurationBase import RecoFragmentsPool
from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
from JetRecConfig import JetInputConfig, JetRecConfig
from JetRecConfig.StandardJetContext import jetContextDic
from JetRecConfig.DependencyHelper import solveDependencies, solveGroomingDependencies

from TrigEDMConfig.TriggerEDMRun3 import recordable

from . import JetRecoConfiguration
from .JetRecoConfiguration import jetRecoDictToString

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)


###############################################################################################
# Sequences for input information

# prefix used in naming HLT collections
jetNamePrefix = JetRecoConfiguration.getHLTPrefix() # "HLT_"

# Calo cell unpacking and topocluster reconstruction
def jetClusterSequence(configFlags, RoIs, clusterCalib):

    # Start by adding the topocluster reco sequence
    from TriggerMenuMT.HLT.CommonSequences.CaloSequences import (
            caloClusterRecoSequence, LCCaloClusterRecoSequence)
    if clusterCalib == "em":
        topoClusterSequence, clustersKey = RecoFragmentsPool.retrieve(
                caloClusterRecoSequence, flags=configFlags, RoIs=RoIs)
    elif clusterCalib == "lcw":
        topoClusterSequence, clustersKey = RecoFragmentsPool.retrieve(
                LCCaloClusterRecoSequence, flags=configFlags, RoIs=RoIs)
    else:
        raise ValueError("Invalid value for calib: '{}'".format(clusterCalib))

    return topoClusterSequence, clustersKey


###############################################################################################
# Sequences that set up the concrete jet finding job

# Need to do this hacky extraction to get around the inability
# to hash dicts as input to RecoFragmentsPool.retrieve
def getTrkColls(jetRecoDict):
    trkcolls = {key:jetRecoDict[key] for key in jetContextDic["trackKeys"]}
    return trkcolls

# The top-level sequence, forwards arguments as appropriate to 
# standard jet reco, grooming or reclustering sequences
# If tracking is required, then the trkcolls dict (returned by the
# JetFSTrackingSequence) must also be passed as kwargs
def jetRecoSequence( configFlags, clustersKey, **jetRecoDict ):

    jetalg, jetradius, extra = JetRecoConfiguration.interpretRecoAlg(jetRecoDict["recoAlg"])
    doGrooming = extra in ["t","sd"]
    doRecluster = extra == "r"
    dataSource = "mc" if configFlags.Input.isMC else "data"

    if doRecluster:
        return RecoFragmentsPool.retrieve(
            reclusteredJetRecoSequence, 
            configFlags, dataSource=dataSource,
            clustersKey=clustersKey, **jetRecoDict)
    elif doGrooming:
        return RecoFragmentsPool.retrieve(
            groomedJetRecoSequence,
            configFlags, dataSource=dataSource,
            clustersKey=clustersKey, **jetRecoDict)
    else:
        return RecoFragmentsPool.retrieve(
            standardJetRecoSequence,
            configFlags, dataSource=dataSource,
            clustersKey=clustersKey, **jetRecoDict)

def standardJetBuildSequence( configFlags, dataSource, clustersKey, **jetRecoDict ):
    """This build the standard jet (not groomed or reclustered).

    This is similar to JetRecConfig.getJetDefAlgs(). However due to how the alg flow is organized in the 
    chain steps, we can't use this function directly.
    Instead we 
      - construct a JetDefinition
      - use lower-level function in JetRecConfig with this JetDefinition to get the necessary algs and build our sequence manually.

    """
    
    jetDefString = jetRecoDictToString(jetRecoDict)
    buildSeq = parOR( "JetBuildSeq_"+jetDefString, [])
    trkopt = jetRecoDict["trkopt"]
    doesTracking = trkopt != "notrk"

    context = jetContextDic.get(trkopt,None)
    
    if doesTracking and not context:
        raise RuntimeError("Failed to retrieve track collections for trkopt '{}'".format(jetRecoDict["trkopt"]))

    # *****************************
    # First part : build a JetDefinition (and a pflow alg if needed)
    isPFlow = jetRecoDict["constitType"] == "pf"

    # Add particle flow reconstruction if needed
    if isPFlow:
        if not doesTracking:
            raise RuntimeError("PFlow jet chain requested with no tracking option!")
        from eflowRec.PFHLTSequence import PFHLTSequence
        (pfseq, pfoPrefix) = RecoFragmentsPool.retrieve(
            PFHLTSequence,
            configFlags, clustersin=clustersKey, tracktype=jetRecoDict["trkopt"], cellsin="CaloCellsFS")
        buildSeq += pfseq
        jetDef = JetRecoConfiguration.defineJets(jetRecoDict,pfoPrefix=pfoPrefix,prefix=jetNamePrefix)
    else:
        jetDef = JetRecoConfiguration.defineJets(jetRecoDict,clustersKey=clustersKey,prefix=jetNamePrefix)
    
    # chosen jet collection
    jetsFullName = jetDef.fullname()
    jetsOut = recordable(jetsFullName)

    # build the list of jetModifiers.
    # Sort and filter
    jetModList = ["Sort", "Filter:"+str(JetRecoConfiguration.getFilterCut(jetRecoDict["recoAlg"])), "ConstitFourMom_copy"]
    if doesTracking:
        jetModList += ["TrackMoments", "JVF", "JVT"]

    if jetRecoDict["recoAlg"] == "a4":
        jetModList += ["CaloEnergies"] # Needed for GSC

    jetDef.modifiers = jetModList    
    # make sure all the modifiers have their dependencies solved 
    jetDef = solveDependencies(jetDef)

    # *****************************
    # Second part : instantiate the actual algs and insert them in the sequence 
    skipConstitMods = (jetRecoDict["constitMod"]=='') and (jetRecoDict["constitType"]=='tc') and (jetRecoDict["clusterCalib"]=="lcw")
    if not skipConstitMods:
        # Then we need a constituent modifier sequence. 
        # Get online monitoring jet rec tool
        from JetRecTools import OnlineMon                                                  
        monJetRecTool = OnlineMon.getMonTool_Algorithm("HLTJets/"+jetsFullName+"/")

        # get the alg from the standard jet config helper :
        constitModAlg = JetRecConfig.getConstitModAlg(jetDef, jetDef.inputdef, monTool=monJetRecTool)
        if constitModAlg:
            buildSeq += constitModAlg

    # Add the PseudoJetGetter alg to the sequence
    constitPJAlg = JetRecConfig.getConstitPJGAlg( jetDef.inputdef , suffix=None)
    buildSeq += conf2toConfigurable( constitPJAlg )
    finalpjs = str(constitPJAlg.OutputContainer)

    if JetRecoConfiguration.jetDefNeedsTracks(jetRecoDict):
        # We need to do ghost association.
        # The ghost tracks pseudoJet are build in other part of the chain : here
        # we just need to merge our constituents with them
        finalpjs = finalpjs+"MergedWithGhostTracks"
        mergerName = "PJMerger_"+finalpjs
        from JetRec import JetRecConf
        mergeAlg = JetRecConf.PseudoJetMerger(
            mergerName,
            InputPJContainers = [str(constitPJAlg.OutputContainer),context["GhostTracks"]],
            OutputContainer = finalpjs)
        buildSeq += mergeAlg

    # set the name of the final PseudoJetContainer to be used as input :
    jetDef._internalAtt['finalPJContainer'] = finalpjs
            
    # Get online monitoring tool
    from JetRec import JetOnlineMon
    monTool = JetOnlineMon.getMonTool_TrigJetAlgorithm("HLTJets/"+jetsFullName+"/")

    # finally get the JetRecAlg :
    jetRecAlg = JetRecConfig.getJetRecAlg(jetDef, monTool=monTool)
    buildSeq += conf2toConfigurable( jetRecAlg )
    
    return buildSeq, jetsOut, jetDef

def standardJetRecoSequence( configFlags, dataSource, clustersKey, **jetRecoDict ):
    jetDefString = jetRecoDictToString(jetRecoDict)

    if jetRecoDict["jetCalib"]=="nojcalib":
        return RecoFragmentsPool.retrieve( standardJetBuildSequence, configFlags, dataSource=dataSource,
                                           clustersKey=clustersKey,**jetRecoDict)

    # Schedule reconstruction w/o calibration
    # This is just a starting point -- will change so that
    # the calibration is only ever done at the end for ungroomed
    _jetRecoDictNoJCalib = dict(jetRecoDict)
    _jetRecoDictNoJCalib["jetCalib"] = "nojcalib"

    buildSeq, jetsNoCalib, jetDefNoCalib = RecoFragmentsPool.retrieve( standardJetBuildSequence, configFlags, dataSource=dataSource,
                                                            clustersKey=clustersKey, **_jetRecoDictNoJCalib)

    recoSeq = parOR( "JetRecSeq_"+jetDefString, [buildSeq])
    # Get the calibration tool if desired. 
    jetDef = jetDefNoCalib.clone()
    jetDef.suffix = jetDefNoCalib.suffix.replace("nojcalib",jetRecoDict["jetCalib"])

    rhoKey = "auto"
    if "sub" in jetRecoDict["jetCalib"]:
        # Add the event shape alg if needed for area subtraction
        # WARNING : offline jets use the parameter voronoiRf = 0.9 ! we might want to harmonize this.
        
        eventShapeAlg = JetInputConfig.buildEventShapeAlg( jetDef, jetNamePrefix, voronoiRf = 1.0 )
        recoSeq += conf2toConfigurable(eventShapeAlg)
        # Not currently written because impossible to merge
        # across event views, which is maybe a concern in
        # the case of regional PFlow
        rhoKey = str(eventShapeAlg.EventDensityTool.OutputContainer)

    jetDef.modifiers = JetRecoConfiguration.defineCalibMods(jetRecoDict,dataSource,rhoKey)
    # If we need JVT, just rerun the JVT modifier
    doesTracking = jetRecoDict["trkopt"] != "notrk"
    isPFlow = jetRecoDict["constitType"] == "pf"
    if doesTracking:
        jetDef.modifiers.append("JVT") #+jetRecoDict["trkopt"]
    #Configuring jet cleaning mods now
    if not isPFlow and jetRecoDict['recoAlg']=='a4': #Add jet cleaning decorations only to small-R non-PFlow jets for now
        from TriggerMenuMT.HLT.Jet.JetRecoConfiguration import cleaningDict
        jetDef.modifiers.append("CaloQuality")
        for key,cleanWP in cleaningDict.items(): jetDef.modifiers.append(f"Cleaning:{cleanWP}")

    decorList = JetRecoConfiguration.getDecorList(doesTracking,isPFlow)
    decorList += ["Jvt"]
    
    # Get online monitoring tool
    from JetRec import JetOnlineMon
    monTool = JetOnlineMon.getMonTool_TrigJetAlgorithm("HLTJets/"+jetDef.fullname()+"/")
    copyCalibAlg = JetRecConfig.getJetCopyAlg(jetsin=jetsNoCalib,jetsoutdef=jetDef,decorations=decorList,monTool=monTool)
    recoSeq += copyCalibAlg

    jetPtMin = 10e3 # 10 GeV minimum pt for jets to be seen by hypo
    from JetRec.JetRecConf import JetViewAlg
    filteredJetsName = jetDef.fullname()+"_pt10"
    recoSeq += JetViewAlg("jetview_"+filteredJetsName,
                          InputContainer=jetDef.fullname(),
                          OutputContainer=filteredJetsName,
                          PtMin=jetPtMin,
                          DecorDeps=decorList + (["Jvt"] if doesTracking else [])
    )
    jetsOut = filteredJetsName

    # End of basic jet reco
    return recoSeq, jetsOut, jetDef

# Grooming needs the ungroomed jets to be built first,
# so call the basic jet reco seq, then add a grooming alg
def groomedJetRecoSequence( configFlags, dataSource, clustersKey, **jetRecoDict ):
    jetDefString = jetRecoDictToString(jetRecoDict)
    recoSeq = parOR( "JetGroomSeq_"+jetDefString, [])

    ungroomedJetRecoDict = dict(jetRecoDict)
    ungroomedJetRecoDict["recoAlg"] = ungroomedJetRecoDict["recoAlg"].rstrip("tsd") # Drop grooming spec
    ungroomedJetRecoDict["jetCalib"] = "nojcalib" # No need to calibrate

    # Only jet building -- we do jet calib in a larger sequence via copy+calib
    (ungroomedJetBuildSequence,ungroomedJetsName,ungroomedDef) = RecoFragmentsPool.retrieve(
        standardJetBuildSequence,
        configFlags, dataSource=dataSource, clustersKey=clustersKey,
        **ungroomedJetRecoDict)
    recoSeq += ungroomedJetBuildSequence

    groomDef = JetRecoConfiguration.defineGroomedJets(jetRecoDict,ungroomedDef)
    groomedJetsFullName = groomDef.fullname()
    groomDef.modifiers = JetRecoConfiguration.defineCalibMods(jetRecoDict,dataSource)
    groomDef.modifiers += ["Sort","Filter:"+str(JetRecoConfiguration.getFilterCut(jetRecoDict["recoAlg"]))]
    # Can add substructure mods here

    # Get online monitoring tool
    from JetRec import JetOnlineMon
    monTool = JetOnlineMon.getMonTool_TrigJetAlgorithm("HLTJets/"+groomedJetsFullName+"/")

    groomDef = solveGroomingDependencies(groomDef)
    groomalg = JetRecConfig.getJetRecGroomAlg(groomDef,monTool)
    recoSeq += conf2toConfigurable( groomalg )

    jetsOut = recordable(groomedJetsFullName)
    return recoSeq, jetsOut, groomDef


# Reclustering -- call the basic jet reco and add this to the sequence,
# then add another jet algorithm to run the reclustering step
def reclusteredJetRecoSequence( configFlags, dataSource, clustersKey, **jetRecoDict ):
    jetDefString = jetRecoDictToString(jetRecoDict)
    recoSeq = parOR( "JetReclusterSeq_"+jetDefString, [])

    basicJetRecoDict = dict(jetRecoDict)
    basicJetRecoDict["recoAlg"] = "a4" # Standard size for reclustered
    (basicJetRecoSequence,basicJetsFiltered, basicJetDef) = RecoFragmentsPool.retrieve(
        standardJetRecoSequence,
        configFlags, dataSource=dataSource, clustersKey=clustersKey,
        **basicJetRecoDict)
    recoSeq += basicJetRecoSequence

    rcJetPtMin = 15e3 # 15 GeV minimum pt for jets to be reclustered
    from JetRec.JetRecConf import JetViewAlg
    filteredJetsName = basicJetDef.fullname()+"_pt15"
    recoSeq += JetViewAlg("jetview_"+filteredJetsName,
                          InputContainer=basicJetDef.fullname(),
                          OutputContainer=filteredJetsName,
                          PtMin=rcJetPtMin)

    rcJetDef = JetRecoConfiguration.defineReclusteredJets(jetRecoDict, filteredJetsName, basicJetDef.inputdef.label, jetNamePrefix, '_'+jetRecoDict["jetCalib"])
    rcModList = [] # Could set substructure mods
    rcJetDef.modifiers = rcModList

    rcConstitPJAlg = JetRecConfig.getConstitPJGAlg( rcJetDef.inputdef, suffix=jetDefString)
    rcConstitPJKey = str(rcConstitPJAlg.OutputContainer)
    recoSeq += conf2toConfigurable( rcConstitPJAlg )

    # Get online monitoring tool
    from JetRec import JetOnlineMon
    monTool = JetOnlineMon.getMonTool_TrigJetAlgorithm("HLTJets/"+rcJetDef.fullname()+"/")

    rcJetDef._internalAtt['finalPJContainer'] = rcConstitPJKey
    # Depending on whether running the trackings step
    ftf_suffix = "" if jetRecoDict["trkopt"] == "notrk" else "_ftf" 
    rcJetRecAlg = JetRecConfig.getJetRecAlg(rcJetDef, monTool, ftf_suffix)
    recoSeq += conf2toConfigurable( rcJetRecAlg )

    jetsOut = recordable(rcJetDef.fullname())
    jetDef = rcJetDef
    return recoSeq, jetsOut, jetDef



