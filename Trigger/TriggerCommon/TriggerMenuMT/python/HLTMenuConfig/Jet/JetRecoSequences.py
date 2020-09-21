#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.CFElements import parOR, seqAND
from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import RecoFragmentsPool
from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
from TriggerMenuMT.HLTMenuConfig.Jet import JetRecoConfiguration
from JetRecConfig.JetRecConfig import getConstitPJGAlg, getJetAlgorithm
from TrigEDMConfig.TriggerEDMRun3 import recordable

# Translate the reco dict to a string for suffixing etc
def jetRecoDictToString(jetRecoDict):
    strtemp = "{recoAlg}_{dataType}_{calib}_{jetCalib}"
    if jetRecoDict["trkopt"] != "notrk":
        strtemp += "_{trkopt}"
    return strtemp.format(**jetRecoDict)

# Configure reco from a dict of options
# Start from a FullScan inputs maker
def jetAthSequence(configFlags, **jetRecoDict):
    from TrigT2CaloCommon.CaloDef import clusterFSInputMaker
    InputMakerAlg= conf2toConfigurable(clusterFSInputMaker())

    (recoSequence, sequenceOut, jetDef) = RecoFragmentsPool.retrieve( jetRecoSequence, configFlags, **jetRecoDict )

    jetDefString = jetRecoDictToString(jetRecoDict)
    JetAthSequence =  seqAND("jetAthSequence_"+jetDefString,[InputMakerAlg, recoSequence ])
    return (JetAthSequence, InputMakerAlg, sequenceOut)

jetNamePrefix = "HLT_"

# Dummy flag arg needed so that each reco sequence is held separately
# in the RecoFragmentsPool -- only the kwargs are used to distinguish
# different sequences. New convention is just to pass "None" for flags
def jetRecoSequence( configFlags, RoIs = 'HLT_FSJETRoI', **jetRecoDict ):

    jetalg, jetradius, extra = JetRecoConfiguration.interpretRecoAlg(jetRecoDict["recoAlg"])
    doGrooming = extra in ["t","sd"]
    doRecluster = extra == "r"
    dataSource = "mc" if configFlags.Input.isMC else "data"

    if doRecluster:
        return RecoFragmentsPool.retrieve( reclusteredJetRecoSequence, configFlags, dataSource=dataSource, RoIs=RoIs, **jetRecoDict)
    elif doGrooming:
        return RecoFragmentsPool.retrieve( groomedJetRecoSequence, configFlags, dataSource=dataSource, RoIs=RoIs, **jetRecoDict)
    else:
        return RecoFragmentsPool.retrieve( standardJetRecoSequence, configFlags, dataSource=dataSource, RoIs=RoIs, **jetRecoDict)

# Normal jet reconstruction, no reclustering or grooming
def standardJetRecoSequence( configFlags, dataSource, RoIs, **jetRecoDict ):
    jetDefString = jetRecoDictToString(jetRecoDict)
    recoSeq = parOR( "JetRecSeq_"+jetDefString, [])

    # Start by adding the topocluster reco sequence
    from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequenceSetup import (
            caloClusterRecoSequence, LCCaloClusterRecoSequence)
    if jetRecoDict["calib"] == "em":
        topoClusterSequence, clustersKey = RecoFragmentsPool.retrieve(
                caloClusterRecoSequence, flags=None, RoIs=RoIs)
    elif jetRecoDict["calib"] == "lcw":
        topoClusterSequence, clustersKey = RecoFragmentsPool.retrieve(
                LCCaloClusterRecoSequence, flags=None, RoIs=RoIs)
    else:
        raise ValueError("Invalid value for calib: '{}'".format(jetRecoDict["calib"]))
    recoSeq += topoClusterSequence

    # Set up tracking sequence -- may need to reorganise or relocate
    # depending on how we want to handle HLT preselection
    trkcolls = None
    if jetRecoDict["trkopt"] != "notrk":
        from .JetTrackingConfig import JetTrackingSequence
        (jettrkseq, trkcolls) = RecoFragmentsPool.retrieve( JetTrackingSequence, configFlags, trkopt=jetRecoDict["trkopt"], RoIs=RoIs)
        recoSeq += jettrkseq

    # Potentially add particle flow reconstruction
    # Work in progress
    if "pf" in jetRecoDict["dataType"]:
        if jetRecoDict["trkopt"] == "notrk":
            raise RuntimeError("PFlow jet chain requested with no tracking option!")
        from eflowRec.PFHLTSequence import PFHLTSequence
        (pfseq, pfoPrefix) = RecoFragmentsPool.retrieve(PFHLTSequence, configFlags, clustersin=clustersKey, tracktype=jetRecoDict["trkopt"])
        recoSeq += pfseq
        jetDef = JetRecoConfiguration.defineJets(jetRecoDict,pfoPrefix=pfoPrefix)
    else:
        jetDef = JetRecoConfiguration.defineJets(jetRecoDict,clustersKey=clustersKey)
    useConstitMods = ["sktc","cssktc", "pf", "csskpf"]
    doConstitMods = jetRecoDict["dataType"] in useConstitMods

    # chosen jet collection
    jetsFullName = jetNamePrefix+jetDef.basename+"Jets_"+jetRecoDict["jetCalib"]
    if jetRecoDict["trkopt"] != "notrk":
        jetsFullName += "_{}".format(jetRecoDict["trkopt"])
    sequenceOut = recordable(jetsFullName)

    if doConstitMods:
        # Get online monitoring jet rec tool
        from JetRecTools import OnlineMon                                                  
        monJetRecTool = OnlineMon.getMonTool_Algorithm("HLTJets/"+jetsFullName+"/")

        from JetRecConfig.ConstModHelpers import getConstitModAlg
        if jetRecoDict["trkopt"] == "notrk":
            recoSeq += getConstitModAlg(jetDef.inputdef,suffix="HLT",tvaKey="JetTrackVtxAssoc",vtxKey="PrimaryVertices",monTool=monJetRecTool)
        else:
            recoSeq += getConstitModAlg(jetDef.inputdef,suffix="HLT",tvaKey=trkcolls["TVA"],vtxKey=trkcolls["Vertices"],monTool=monJetRecTool)

    # Add the PseudoJetGetter alg to the sequence
    constitPJAlg = getConstitPJGAlg( jetDef.inputdef )
    constitPJKey = constitPJAlg.OutputContainer
    recoSeq += conf2toConfigurable( constitPJAlg )
    # Basic list of PseudoJets is just the constituents
    # Append ghosts (tracks) if desired
    pjs = [constitPJKey]
    if trkcolls:
        pjs.append(trkcolls["GhostTracks"])

    from JetRecConfig import JetRecConfig
    jetModList = []
    if jetRecoDict["trkopt"] != "notrk":
        trkMods = JetRecoConfiguration.defineTrackMods(jetRecoDict["trkopt"])
        jetModList += trkMods

    rhoKey = "auto"
    if "sub" in jetRecoDict["jetCalib"]:
        # Add the event shape alg if needed for area subtraction
        eventShapeAlg = JetRecConfig.getEventShapeAlg( jetDef.inputdef, constitPJKey, "HLT_" )
        recoSeq += conf2toConfigurable(eventShapeAlg)
        # Not currently written because impossible to merge
        # across event views, which is maybe a concern in
        # the case of regional PFlow
        rhoKey = eventShapeAlg.EventDensityTool.OutputContainer

    # Import the standard jet modifiers as defined for offline
    # We can add/configure these differently if desired. In particular,
    # we could define a TriggerJetMods module if settings need to
    # diverge substantially e.g. track/vertex collections
    calibMods = JetRecoConfiguration.defineCalibFilterMods(jetRecoDict,dataSource, rhoKey)
    jetModList += calibMods

    # Get online monitoring tool
    from JetRec import JetOnlineMon
    monTool = JetOnlineMon.getMonTool_TrigJetAlgorithm("HLTJets/"+jetsFullName+"/")

    # Generate a JetAlgorithm to run the jet finding and modifiers
    # (via a JetRecTool instance).
    jetRecAlg = JetRecConfig.getJetAlgorithm(jetsFullName, jetDef, pjs, jetModList, monTool)
    recoSeq += conf2toConfigurable( jetRecAlg )
    # End of basic jet reco
    return recoSeq, sequenceOut, jetDef

# Grooming needs the ungroomed jets to be built first,
# so call the basic jet reco seq, then add a grooming alg
def groomedJetRecoSequence( configFlags, dataSource, RoIs, **jetRecoDict ):
    jetDefString = jetRecoDictToString(jetRecoDict)
    recoSeq = parOR( "JetGroomSeq_"+jetDefString, [])
    ungroomedJetRecoDict = dict(jetRecoDict)
    ungroomedJetRecoDict["recoAlg"] = ungroomedJetRecoDict["recoAlg"].rstrip("tsd") # Drop grooming spec
    ungroomedJetRecoDict["jetCalib"] = "nojcalib" # No need to calibrate

    (ungroomedJetRecoSequence,ungroomedJetsName, ungroomedDef) = RecoFragmentsPool.retrieve(standardJetRecoSequence,configFlags,dataSource=dataSource,RoIs=RoIs,**ungroomedJetRecoDict)
    recoSeq += conf2toConfigurable( ungroomedJetRecoSequence )
    # Need to forward the pseudojets of the parents to the groomer
    parentpjs = getattr(ungroomedJetRecoSequence,"jetalg_{}".format(ungroomedJetsName)).Tools[0].InputPseudoJets

    groomDef = JetRecoConfiguration.defineGroomedJets(jetRecoDict,ungroomedDef,ungroomedJetsName)
    groomedJetsFullName = jetNamePrefix+groomDef.basename+"Jets_"+jetRecoDict["jetCalib"]
    if jetRecoDict["trkopt"]!="notrk":
        groomedJetsFullName += "_"+jetRecoDict["trkopt"]

    groomedModList = JetRecoConfiguration.defineCalibFilterMods(jetRecoDict,dataSource)
    # Can add substructure mods here

    # Get online monitoring tool
    from JetRec import JetOnlineMon
    monTool = JetOnlineMon.getMonTool_TrigJetAlgorithm("HLTJets/"+groomedJetsFullName+"/")

    from JetRecConfig.JetGroomConfig import getJetGroomAlg
    groomalg = getJetGroomAlg(groomedJetsFullName,groomDef,parentpjs,groomedModList,monTool)
    recoSeq += conf2toConfigurable( groomalg )

    sequenceOut = recordable(groomedJetsFullName)
    jetDef = groomDef
    return recoSeq, sequenceOut, jetDef


# Reclustering -- call the basic jet reco and add this to the sequence,
# then add another jet algorithm to run the reclustering step
def reclusteredJetRecoSequence( configFlags, dataSource, RoIs, **jetRecoDict ):
    jetDefString = jetRecoDictToString(jetRecoDict)
    recoSeq = parOR( "JetReclusterSeq_"+jetDefString, [])

    basicJetRecoDict = dict(jetRecoDict)
    basicJetRecoDict["recoAlg"] = "a4" # Standard size for reclustered
    (basicJetRecoSequence,basicJetsName, basicJetDef) = RecoFragmentsPool.retrieve(standardJetRecoSequence,configFlags,dataSource=dataSource,RoIs=RoIs,**basicJetRecoDict)
    recoSeq += basicJetRecoSequence

    rcJetPtMin = 15e3 # 15 GeV minimum pt for jets to be reclustered
    from JetRec.JetRecConf import JetViewAlg
    filteredJetsName = basicJetsName+"_pt15"
    recoSeq += JetViewAlg("jetview_"+filteredJetsName,
                          InputContainer=basicJetsName,
                          OutputContainer=filteredJetsName,
                          PtMin=rcJetPtMin)

    rcJetDef = JetRecoConfiguration.defineReclusteredJets(jetRecoDict)
    rcJetDef.inputdef.inputname = filteredJetsName
    rcJetsFullName = jetNamePrefix+rcJetDef.basename+"RCJets_"+jetRecoDict["jetCalib"]
    rcModList = [] # Could set substructure mods
    rcJetDef.modifiers = rcModList

    rcConstitPJAlg = getConstitPJGAlg( rcJetDef.inputdef )
    rcConstitPJKey = rcConstitPJAlg.OutputContainer
    recoSeq += conf2toConfigurable( rcConstitPJAlg )

    # Get online monitoring tool
    from JetRec import JetOnlineMon
    monTool = JetOnlineMon.getMonTool_TrigJetAlgorithm("HLTJets/"+rcJetsFullName+"/")

    rcPJs = [rcConstitPJKey]
    rcJetRecAlg = getJetAlgorithm(rcJetsFullName, rcJetDef, rcPJs, rcModList, monTool)

    recoSeq += conf2toConfigurable( rcJetRecAlg )

    sequenceOut = recordable(rcJetsFullName)
    jetDef = rcJetDef
    return recoSeq, sequenceOut, jetDef
