#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.CFElements import parOR
from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import RecoFragmentsPool
from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
from JetRecConfig.JetRecConfig import getConstitPJGAlg, getJetAlgorithm
from JetRecConfig import JetInputConfig
from JetRecConfig import JetRecConfig

from TrigEDMConfig.TriggerEDMRun3 import recordable

from . import JetRecoConfiguration
from .JetRecoConfiguration import jetRecoDictToString

###############################################################################################
# Sequences for input information

# Calo cell unpacking and topocluster reconstruction
def jetClusterSequence(configFlags, RoIs, clusterCalib):

    # Start by adding the topocluster reco sequence
    from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequenceSetup import (
            caloClusterRecoSequence, LCCaloClusterRecoSequence)
    if clusterCalib == "em":
        topoClusterSequence, clustersKey = RecoFragmentsPool.retrieve(
                caloClusterRecoSequence, flags=None, RoIs=RoIs)
    elif clusterCalib == "lcw":
        topoClusterSequence, clustersKey = RecoFragmentsPool.retrieve(
                LCCaloClusterRecoSequence, flags=None, RoIs=RoIs)
    else:
        raise ValueError("Invalid value for calib: '{}'".format(clusterCalib))

    return topoClusterSequence, clustersKey


###############################################################################################
# Sequences that set up the concrete jet finding job
jetNamePrefix = "HLT_"

# Need to do this hacky extraction to get around the inability
# to hash dicts as input to RecoFragmentsPool.retrieve
from .JetTrackingConfig import trkcollskeys
def getTrkColls(jetRecoDict):
    trkcolls = {key:jetRecoDict[key] for key in trkcollskeys}
    return trkcolls

# The top-level sequence, forwards arguments as appropriate to 
# standard jet reco, grooming or reclustering sequences
# If tracking is required, then the trkcolls dict (returned by the
# JetTrackingSequence) must also be passed as kwargs
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

# Normal jet reconstruction, no reclustering or grooming
def standardJetBuildSequence( configFlags, dataSource, clustersKey, **jetRecoDict ):
    jetDefString = jetRecoDictToString(jetRecoDict)
    buildSeq = parOR( "JetBuildSeq_"+jetDefString, [])
    doesTracking = jetRecoDict["trkopt"]!="notrk"
    trkcolls = getTrkColls(jetRecoDict) if doesTracking else {}
    if doesTracking and not trkcolls:
        raise RuntimeError("Failed to retrieve track collections for trkopt '{}'".format(jetRecoDict["trkopt"]))

    isPFlow = "pf" in jetRecoDict["dataType"]

    # Add particle flow reconstruction if needed
    if isPFlow:
        if not doesTracking:
            raise RuntimeError("PFlow jet chain requested with no tracking option!")
        from eflowRec.PFHLTSequence import PFHLTSequence
        (pfseq, pfoPrefix) = RecoFragmentsPool.retrieve(
            PFHLTSequence,
            configFlags, clustersin=clustersKey, tracktype=jetRecoDict["trkopt"])
        buildSeq += pfseq
        jetDef = JetRecoConfiguration.defineJets(jetRecoDict,pfoPrefix=pfoPrefix,prefix=jetNamePrefix)
    else:
        jetDef = JetRecoConfiguration.defineJets(jetRecoDict,clustersKey=clustersKey,prefix=jetNamePrefix)

    # chosen jet collection
    jetsFullName = jetDef.fullname()
    jetsOut = recordable(jetsFullName)
    doConstitMods = jetRecoDict["dataType"] in ["sktc","cssktc", "pf", "csskpf"]
    JetRecConfig.instantiateAliases(jetDef)
    if doConstitMods:
        # Get online monitoring jet rec tool
        from JetRecTools import OnlineMon                                                  
        monJetRecTool = OnlineMon.getMonTool_Algorithm("HLTJets/"+jetsFullName+"/")

        from JetRecConfig.ConstModHelpers import getConstitModAlg
        constitModAlg = getConstitModAlg(jetDef.inputdef, monTool=monJetRecTool)
        if constitModAlg:
            buildSeq += constitModAlg

    # Add the PseudoJetGetter alg to the sequence
    constitPJAlg = getConstitPJGAlg( jetDef.inputdef )
    constitPJKey = str(constitPJAlg.OutputContainer)
    buildSeq += conf2toConfigurable( constitPJAlg )
    # Basic list of PseudoJets is just the constituents
    # Append ghosts (tracks) if desired
    pjs = [constitPJKey]
    # Also compile modifier list
    jetModList = []
    if doesTracking:
        pjs.append(trkcolls["GhostTracks"])
        trkMods = JetRecoConfiguration.defineTrackMods(jetRecoDict["trkopt"])
        jetModList += trkMods

    # Sort and filter
    jetModList += ["Sort", "Filter:"+str(JetRecoConfiguration.getFilterCut(jetRecoDict["recoAlg"]))]

    # Get online monitoring tool
    from JetRec import JetOnlineMon
    monTool = JetOnlineMon.getMonTool_TrigJetAlgorithm("HLTJets/"+jetsFullName+"/")

    jetDef.modifiers = jetModList
    # recall instantiateAliases because we updated the modifiers
    JetRecConfig.instantiateAliases(jetDef)

    # make sure all our JetModifier have their track inputs set up according to trkopt
    from JetRecConfig.JetModConfig import jetModWithAlternateTrk    
    jetModWithAlternateTrk(jetDef, jetRecoDict['trkopt'] )

    # Generate a JetAlgorithm to run the jet finding and modifiers
    # (via a JetRecTool instance).
    jetRecAlg = JetRecConfig.getJetAlgorithm(jetsFullName, jetDef, pjs, monTool)
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
    jetsOut = jetDef.fullname()

    rhoKey = "auto"
    if "sub" in jetRecoDict["jetCalib"]:
        # Add the event shape alg if needed for area subtraction
        eventShapeAlg = JetInputConfig.buildEventShapeAlg( jetDef, jetNamePrefix )
        recoSeq += conf2toConfigurable(eventShapeAlg)
        # Not currently written because impossible to merge
        # across event views, which is maybe a concern in
        # the case of regional PFlow
        rhoKey = str(eventShapeAlg.EventDensityTool.OutputContainer)

    jetDef.modifiers = JetRecoConfiguration.defineCalibMods(jetRecoDict,dataSource,rhoKey)
    # If we need JVT, just rerun the JVT modifier
    doesTracking = jetRecoDict["trkopt"] != "notrk"
    isPFlow = "pf" in jetRecoDict["dataType"]
    if doesTracking:
        jetDef.modifiers.append("JVT:"+jetRecoDict["trkopt"])
    #Configuring jet cleaning mods now
    if not isPFlow and jetRecoDict["cleaning"] != 'noCleaning': #Decorate with jet cleaning info only if not a PFlow chain (no cleaning available for PFlow now)
        jetDef.modifiers.append("Cleaning:"+jetRecoDict["cleaning"])


    decorList = JetRecoConfiguration.getDecorList(doesTracking,isPFlow)
    copyCalibAlg = JetRecConfig.getJetCopyAlg(jetsin=jetsNoCalib,jetsoutdef=jetDef,decorations=decorList)

    recoSeq += copyCalibAlg

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
    # Need to forward the pseudojets of the parents to the groomer
    # Should try to do this in a nicer way...
    parentpjs = getattr(ungroomedJetBuildSequence,"jetalg_{}".format(ungroomedJetsName)).Tools[0].InputPseudoJets

    groomDef = JetRecoConfiguration.defineGroomedJets(jetRecoDict,ungroomedDef)
    groomedJetsFullName = groomDef.fullname()
    groomDef.modifiers = JetRecoConfiguration.defineCalibMods(jetRecoDict,dataSource)
    groomDef.modifiers += ["Sort","Filter:"+str(JetRecoConfiguration.getFilterCut(jetRecoDict["recoAlg"]))]
    # Can add substructure mods here

    # Get online monitoring tool
    from JetRec import JetOnlineMon
    monTool = JetOnlineMon.getMonTool_TrigJetAlgorithm("HLTJets/"+groomedJetsFullName+"/")

    from JetRecConfig.JetGroomConfig import getJetGroomAlg_jetAlg, instantiateGroomingAliases
    instantiateGroomingAliases(groomDef)
    groomalg = getJetGroomAlg_jetAlg(groomDef,parentpjs,monTool)
    recoSeq += conf2toConfigurable( groomalg )

    jetsOut = recordable(groomedJetsFullName)
    jetDef = groomDef
    return recoSeq, jetsOut, jetDef


# Reclustering -- call the basic jet reco and add this to the sequence,
# then add another jet algorithm to run the reclustering step
def reclusteredJetRecoSequence( configFlags, dataSource, clustersKey, **jetRecoDict ):
    jetDefString = jetRecoDictToString(jetRecoDict)
    recoSeq = parOR( "JetReclusterSeq_"+jetDefString, [])

    basicJetRecoDict = dict(jetRecoDict)
    basicJetRecoDict["recoAlg"] = "a4" # Standard size for reclustered
    (basicJetRecoSequence,basicJetsName, basicJetDef) = RecoFragmentsPool.retrieve(
        standardJetRecoSequence,
        configFlags, dataSource=dataSource, clustersKey=clustersKey,
        **basicJetRecoDict)
    recoSeq += basicJetRecoSequence

    rcJetPtMin = 15e3 # 15 GeV minimum pt for jets to be reclustered
    from JetRec.JetRecConf import JetViewAlg
    filteredJetsName = basicJetsName+"_pt15"
    recoSeq += JetViewAlg("jetview_"+filteredJetsName,
                          InputContainer=basicJetsName,
                          OutputContainer=filteredJetsName,
                          PtMin=rcJetPtMin)

    rcJetDef = JetRecoConfiguration.defineReclusteredJets(jetRecoDict, filteredJetsName)
    rcJetsFullName = jetNamePrefix+rcJetDef.basename+"Jets_"+jetRecoDict["jetCalib"]
    rcModList = [] # Could set substructure mods
    rcJetDef.modifiers = rcModList

    rcConstitPJAlg = getConstitPJGAlg( rcJetDef.inputdef )
    rcConstitPJKey = str(rcConstitPJAlg.OutputContainer)
    recoSeq += conf2toConfigurable( rcConstitPJAlg )

    # Get online monitoring tool
    from JetRec import JetOnlineMon
    monTool = JetOnlineMon.getMonTool_TrigJetAlgorithm("HLTJets/"+rcJetsFullName+"/")

    rcPJs = [rcConstitPJKey]
    rcJetRecAlg = getJetAlgorithm(rcJetsFullName, rcJetDef, rcPJs,  monTool)

    recoSeq += conf2toConfigurable( rcJetRecAlg )

    jetsOut = recordable(rcJetsFullName)
    jetDef = rcJetDef
    return recoSeq, jetsOut, jetDef
