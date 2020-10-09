#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.CFElements import parOR, seqAND
from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import RecoFragmentsPool
from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
from TriggerMenuMT.HLTMenuConfig.Jet import JetRecoConfiguration
from JetRecConfig.JetRecConfig import getConstitPJGAlg, getJetAlgorithm
from JetRecConfig import JetInputConfig
from JetRecConfig import JetRecConfig

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
        jetDef = JetRecoConfiguration.defineJets(jetRecoDict,pfoPrefix=pfoPrefix, prefix=jetNamePrefix)
    else:
        jetDef = JetRecoConfiguration.defineJets(jetRecoDict,clustersKey=clustersKey, prefix=jetNamePrefix)

    doConstitMods = jetRecoDict["dataType"] in ["sktc","cssktc", "pf", "csskpf"]

    jetsFullName = jetDef.fullname()
    sequenceOut = recordable(jetsFullName)

    JetRecConfig.instantiateAliases(jetDef)
    if doConstitMods:
        # Get online monitoring jet rec tool
        from JetRecTools import OnlineMon                                                  
        monJetRecTool = OnlineMon.getMonTool_Algorithm("HLTJets/"+jetsFullName+"/")

        from JetRecConfig.ConstModHelpers import getConstitModAlg
        constitModAlg = getConstitModAlg(jetDef.inputdef, monTool=monJetRecTool)
        if constitModAlg:
            recoSeq += constitModAlg

    # Add the PseudoJetGetter alg to the sequence
    constitPJAlg = getConstitPJGAlg( jetDef.inputdef )
    constitPJKey = constitPJAlg.OutputContainer
    recoSeq += conf2toConfigurable( constitPJAlg )
    # Basic list of PseudoJets is just the constituents
    # Append ghosts (tracks) if desired
    pjs = [constitPJKey]
    if trkcolls:
        pjs.append(trkcolls["GhostTracks"])

    jetModList = []
    if jetRecoDict["trkopt"] != "notrk":
        trkMods = JetRecoConfiguration.defineTrackMods(jetRecoDict["trkopt"])
        jetModList += trkMods

    rhoKey = "auto"
    if "sub" in jetRecoDict["jetCalib"]:
        # Add the event shape alg if needed for area subtraction
        eventShapeAlg = JetInputConfig.buildEventShapeAlg( jetDef,  "HLT_" )
        recoSeq += conf2toConfigurable(eventShapeAlg)
        # Not currently written because impossible to merge
        # across event views, which is maybe a concern in
        # the case of regional PFlow
        rhoKey = eventShapeAlg.EventDensityTool.OutputContainer

    # Import the standard jet modifiers as defined for offline
    # We can add/configure these differently if desired. 
    calibMods = JetRecoConfiguration.defineCalibFilterMods(jetRecoDict,dataSource, rhoKey)
    jetModList += calibMods


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

    
    groomDef = JetRecoConfiguration.defineGroomedJets(jetRecoDict,ungroomedDef)#,ungroomedJetsName)
    groomedJetsFullName = groomDef.fullname()+"_"+jetRecoDict["jetCalib"]
    if jetRecoDict["trkopt"]!="notrk":
        groomedJetsFullName += "_"+jetRecoDict["trkopt"]

    groomDef.modifiers = JetRecoConfiguration.defineCalibFilterMods(jetRecoDict,dataSource)
    # Can add substructure mods here

    # Get online monitoring tool
    from JetRec import JetOnlineMon
    monTool = JetOnlineMon.getMonTool_TrigJetAlgorithm("HLTJets/"+groomedJetsFullName+"/")

    from JetRecConfig.JetGroomConfig import getJetGroomAlg, instantiateGroomingAliases
    instantiateGroomingAliases(groomDef)
    groomalg = getJetGroomAlg(groomedJetsFullName,groomDef,parentpjs,monTool)
    recoSeq += conf2toConfigurable( groomalg )

    sequenceOut = recordable(groomedJetsFullName)    
    return recoSeq, sequenceOut, groomDef


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

    rcJetDef = JetRecoConfiguration.defineReclusteredJets(jetRecoDict, filteredJetsName)
    rcJetsFullName = jetNamePrefix+rcJetDef.basename+"Jets_"+jetRecoDict["jetCalib"]
    rcModList = [] # Could set substructure mods
    rcJetDef.modifiers = rcModList

    rcConstitPJAlg = getConstitPJGAlg( rcJetDef.inputdef )
    rcConstitPJKey = rcConstitPJAlg.OutputContainer
    recoSeq += conf2toConfigurable( rcConstitPJAlg )

    # Get online monitoring tool
    from JetRec import JetOnlineMon
    monTool = JetOnlineMon.getMonTool_TrigJetAlgorithm("HLTJets/"+rcJetsFullName+"/")

    rcPJs = [rcConstitPJKey]
    rcJetRecAlg = getJetAlgorithm(rcJetsFullName, rcJetDef, rcPJs,  monTool)

    recoSeq += conf2toConfigurable( rcJetRecAlg )

    sequenceOut = recordable(rcJetsFullName)
    jetDef = rcJetDef
    return recoSeq, sequenceOut, jetDef
