#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.CFElements import parOR, seqAND
from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import RecoFragmentsPool

import JetRecoConfiguration

# Translate the reco dict to a string for suffixing etc
def jetRecoDictToString(jetRecoDict):
    strtemp = "{recoAlg}_{dataType}_{calib}_{jetCalib}"
    if jetRecoDict["trkopt"] != "notrk":
        strtemp += "_{trkopt}"
    return strtemp.format(**jetRecoDict)

# Configure reco from a dict of options
# Start from a FullScan inputs maker
def jetAthSequence(dummyFlags, **jetRecoDict):
    from TrigT2CaloCommon.CaloDef import clusterFSInputMaker
    InputMakerAlg= clusterFSInputMaker()

    # Does nothing now, but may need to use this to toggle Insitu calib step
    # in which case it should be deduced from input flags
    dataSource = "data" # or mc

    (recoSequence, sequenceOut) = RecoFragmentsPool.retrieve( jetRecoSequence, None, dataSource=dataSource, **jetRecoDict )

    jetDefString = jetRecoDictToString(jetRecoDict)
    JetAthSequence =  seqAND("jetAthSequence_"+jetDefString,[InputMakerAlg, recoSequence ])
    return (JetAthSequence, InputMakerAlg, sequenceOut)

# Dummy flag arg needed so that each reco sequence is held separately
# in the RecoFragmentsPool -- only the kwargs are used to distinguish
# different sequences. New convention is just to pass "None" for flags
def jetRecoSequence( dummyFlags, dataSource, RoIs = 'FSJETRoI', **jetRecoDict):

    jetDefString = jetRecoDictToString(jetRecoDict)
    recoSeq = parOR( "JetRecSeq_"+jetDefString, [])

    recoAlg = jetRecoDict["recoAlg"]
    doGrooming = recoAlg.endswith("t") # Maybe other grooming strategies
    doRecluster = recoAlg.endswith("r")
    jetNamePrefix = "HLT_"

    from TrigEDMConfig.TriggerEDMRun3 import recordable
    from JetRecConfig.JetRecConfig import getConstitPJGAlg, getJetAlgorithm
    if doRecluster:
        # Reclustering -- recursively call the basic jet reco and add this to the sequence,
        # then add another jet algorithm to run the reclustering step
        basicJetRecoDict = dict(jetRecoDict)
        basicJetRecoDict["recoAlg"] = "a4" # Standard size for reclustered
        (basicJetRecoSequence,basicJetsName) = RecoFragmentsPool.retrieve(jetRecoSequence,None,dataSource=dataSource, **basicJetRecoDict)
        recoSeq += basicJetRecoSequence

        rcJetDef = JetRecoConfiguration.defineReclusteredJets(jetRecoDict)
        rcJetDef.inputdef.inputname = basicJetsName
        rcJetsFullName = jetNamePrefix+rcJetDef.basename+"RCJets_"+jetRecoDict["jetCalib"]
        rcModList = [] # Could set substructure mods
        rcJetDef.modifiers = rcModList

        rcConstitPJAlg = getConstitPJGAlg( rcJetDef.inputdef )
        rcConstitPJKey = rcConstitPJAlg.PJGetter.OutputContainer
        recoSeq += rcConstitPJAlg

        rcPJs = [rcConstitPJKey]
        rcJetRecAlg = getJetAlgorithm(rcJetsFullName, rcJetDef, rcPJs, rcModList)

        recoSeq += rcJetRecAlg

        sequenceOut = recordable(rcJetsFullName)

    elif doGrooming:
        # Grooming needs to be set up similarly to reclustering
        # --> build ungroomed jets, then add a grooming alg
        # Reclustering -- recursively call the basic jet reco and add this to the sequence,
        # then add another jet algorithm to run the reclustering step
        ungroomedJetRecoDict = dict(jetRecoDict)
        ungroomedJetRecoDict["recoAlg"] = ungroomedJetRecoDict["recoAlg"].rstrip("t") # Drop grooming spec
        ungroomedJetRecoDict["jetCalib"] = "nojcalib" # No need to calibrate
        (ungroomedJetRecoSequence,ungroomedJetsName) = RecoFragmentsPool.retrieve(jetRecoSequence,None,dataSource=dataSource, **ungroomedJetRecoDict)
        recoSeq += ungroomedJetRecoSequence

        ungroomedDef = JetRecoConfiguration.defineJets(ungroomedJetRecoDict)
        ungroomedDef.ptmin = 50e3

        groomDef = JetRecoConfiguration.defineGroomedJets(jetRecoDict,ungroomedDef,ungroomedJetsName)
        groomedJetsFullName = jetNamePrefix+groomDef.basename+"Jets_"+jetRecoDict["jetCalib"]

        groomedModList = JetRecoConfiguration.defineCalibFilterMods(jetRecoDict,dataSource)
        # Can add substructure mods here

        from JetRecConfig.JetGroomConfig import getJetGroomAlg
        groomalg = getJetGroomAlg(groomedJetsFullName,groomDef,groomedModList)
        recoSeq += groomalg

        sequenceOut = recordable(groomedJetsFullName)
    else:
        # Normal jet reconstruction, no reclustering or grooming

        # Start by adding the topocluster reco sequence
        # We always make LCW topoclusters, then deal with
        # calibration states later
        from TrigT2CaloCommon.CaloDef import HLTFSTopoRecoSequence
        (topoClusterSequence, clustersKey) = RecoFragmentsPool.retrieve(HLTFSTopoRecoSequence,RoIs)
        recoSeq += topoClusterSequence

        # Set up tracking sequence -- may need to reorganise or relocate
        # depending on how we want to handle HLT preselection
        trkcolls = None
        if jetRecoDict["trkopt"] != "notrk":
            from JetTrackingConfig import JetTrackingSequence
            (jettrkseq, trkcolls) = RecoFragmentsPool.retrieve( JetTrackingSequence, None, trkopt=jetRecoDict["trkopt"], RoIs=RoIs)
            recoSeq += jettrkseq

        # Potentially add particle flow reconstruction
        # Work in progress
        if jetRecoDict["dataType"] == "pf":
            from eflowRec.PFHLTSequence import PFHLTSequence
            pfseq = RecoFragmentsPool.retrieve(PFHLTSequence,clustersKey)
            recoSeq += pfseq

        jetDef = JetRecoConfiguration.defineJets(jetRecoDict,clustersKey)
        doConstitMods = ("sk" in jetRecoDict["dataType"])
        if doConstitMods:
            from JetRecConfig.ConstModHelpers import getConstitModAlg
            recoSeq += getConstitModAlg(jetDef.inputdef,"HLT")

        # Add the PseudoJetGetter alg to the sequence
        constitPJAlg = getConstitPJGAlg( jetDef.inputdef )
        constitPJKey = constitPJAlg.PJGetter.OutputContainer
        recoSeq += constitPJAlg
        # Basic list of PseudoJets is just the constituents
        # Append ghosts (tracks) if desired
        pjs = [constitPJKey]
        if trkcolls:
            pjs.append(trkcolls["GhostTracks"])
        
        # chosen jet collection
        jetsFullName = jetNamePrefix+jetDef.basename+"Jets_"+jetRecoDict["jetCalib"]
        if jetRecoDict["trkopt"] != "notrk":
            jetsFullName += "_{}".format(jetRecoDict["trkopt"])
        sequenceOut = recordable(jetsFullName)

        from JetRecConfig import JetRecConfig
        jetModList = []
        if jetRecoDict["trkopt"] != "notrk":
            trkMods = JetRecoConfiguration.defineTrackMods(jetRecoDict["trkopt"])
            jetModList += trkMods

        rhoKey = "auto"
        if "sub" in jetRecoDict["jetCalib"]:
            # Add the event shape alg if needed for area subtraction
            eventShapeAlg = JetRecConfig.getEventShapeAlg( jetDef.inputdef, constitPJKey, "HLT_" )
            recoSeq += eventShapeAlg
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

        # Generate a JetAlgorithm to run the jet finding and modifiers
        # (via a JetRecTool instance).
        jetRecAlg = JetRecConfig.getJetAlgorithm(jetsFullName, jetDef, pjs, jetModList)
        recoSeq += jetRecAlg
        # End of basic jet reco
        pass

    return (recoSeq,sequenceOut)
