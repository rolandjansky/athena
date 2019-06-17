#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#


from AthenaCommon.CFElements import parOR, seqAND
from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import RecoFragmentsPool

# Translate the reco dict to a string for suffixing etc
def jetRecoDictToString(jetRecoDict):
    strtemp = "{recoAlg}_{dataType}_{calib}_{jetCalib}"
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
    radius = float(recoAlg.lstrip("a").rstrip("tr"))/10
    jetNamePrefix = "Trig"

    from JetRecConfig.JetDefinition import JetConstit, xAODType, JetDefinition
    from JetRecConfig.JetRecConfig import getConstitPJGAlg, getJetAlgorithm
    if doRecluster:
        # Reclustering -- recursively call the basic jet reco and add this to the sequence,
        # then add another jet algorithm to run the reclustering step
        basicJetRecoDict = dict(jetRecoDict)
        basicJetRecoDict["recoAlg"] = "a4"
        (basicJetRecoSequence,basicJetsName) = RecoFragmentsPool.retrieve(jetRecoSequence,None,dataSource=dataSource, **basicJetRecoDict)
        recoSeq += basicJetRecoSequence

        rcJetConstit = JetConstit( xAODType.Jet, [])
        rcJetDef = JetDefinition( "AntiKt", 1.0, rcJetConstit)
        rcJetsFullName = jetNamePrefix+rcJetDef.basename+"RCJets_"+jetRecoDict["jetCalib"]

        rcModList= []
        rcJetConstit.inputname = basicJetsName

        rcConstitPJAlg = getConstitPJGAlg( rcJetConstit )
        rcConstitPJKey = rcConstitPJAlg.PJGetter.OutputContainer
        recoSeq += rcConstitPJAlg

        rcPJs = [rcConstitPJKey]
        rcJetRecAlg = getJetAlgorithm(rcJetsFullName, rcJetDef, rcPJs, rcModList)

        recoSeq += rcJetRecAlg

        sequenceOut = rcJetsFullName

    elif doGrooming:
        # Grooming needs to be set up similarly to reclustering
        # --> build ungroomed jets, then add a grooming alg        
        pass
    else:
        # Normal jet reconstruction, no reclustering or grooming

        # Start by adding the topocluster reco sequence
        # We always make LCW topoclusters, then deal with
        # calibration states later
        from TrigT2CaloCommon.CaloDef import HLTFSTopoRecoSequence
        (topoClusterSequence, clustersKey) = RecoFragmentsPool.retrieve(HLTFSTopoRecoSequence,RoIs)
        recoSeq += topoClusterSequence

        # Potentially add particle flow reconstruction
        # Work in progress
        if jetRecoDict["dataType"] == "pf":
            from eflowRec.PFHLTSequence import PFHLTSequence
            pfseq = RecoFragmentsPool.retrieve(PFHLTSequence,clustersKey)
            recoSeq += pfseq

        # Define the jet constituents to be interpreted by JetRecConfig
        def defineJetConstit(jetRecoDict,clustersKey):
            # Get the details of the constituent definition:
            # type, mods and the input container name
            if "tc" in jetRecoDict["dataType"]:
                # apply this scale
                if jetRecoDict["calib"] == "em":
                    constitMods = ["EM"]
                elif jetRecoDict["calib"] == "lcw":
                    constitMods = ["LC"]
                # read from this cluster collection,
                # overriding the standard offline collection
                jetConstit = JetConstit( xAODType.CaloCluster, constitMods )
                jetConstit.rawname = clustersKey
                # apply constituent pileup suppression
                if "sk" in jetRecoDict["dataType"]:
                    constitMods.append("SK")
                else:
                    jetConstit.inputname = clustersKey
            return jetConstit

        jetConstit = defineJetConstit(jetRecoDict,clustersKey)
        doConstitMods = ("sk" in jetRecoDict["dataType"])
        if doConstitMods:
            from JetRecConfig.ConstModHelpers import getConstitModAlg
            recoSeq += getConstitModAlg(jetConstit,"HLT")
        
        # Arbitrary min pt for fastjet, set to be low enough for MHT(?)
        # Could/should adjust higher for large-R
        jetDef = JetDefinition( "AntiKt", radius, jetConstit, ptmin=5000.)
        
        # chosen jet collection
        jetsFullName = jetNamePrefix+jetDef.basename+"Jets_"+jetRecoDict["jetCalib"]
        sequenceOut = jetsFullName

        from JetRecConfig import JetRecConfig
        # Import the standard jet modifiers as defined for offline
        # We can add/configure these differently if desired. In particular,
        # we could define a TriggerJetMods module if settings need to
        # diverge substantially e.g. track/vertex collections
        from JetRecConfig.StandardJetMods import jetmoddict
        # Make generating the list a bit more comprehensible
        def getModSpec(modname,modspec=''):
            return (jetmoddict[modname],str(modspec))
        # Minimum modifier set for calibration w/o track GSC
        # Should eventually build in more mods, depend on track info etc
        calibMods = []

        if jetRecoDict["jetCalib"] != "nojcalib":
            # Translate calib specification into something understood by
            # the calibration config helper
            calibContext,calibSeq = {
                ("a4","subjes"):   ("TrigRun2","JetArea_EtaJES_GSC"),        # Calo GSC only
                ("a4","subjesIS"): ("TrigRun2","JetArea_EtaJES_GSC_Insitu"), # Calo GSC only
                ("a10","subjes"):  ("TrigUngroomed","JetArea_EtaJES"),
                }[(jetRecoDict["recoAlg"],jetRecoDict["jetCalib"])]

            calibSpec = calibContext+":"+dataSource+":"+calibSeq
            calibMods = [getModSpec("ConstitFourMom"),
                         getModSpec("CaloEnergies"),
                         getModSpec("Calib",calibSpec),
                         getModSpec("Sort")]
        jetModList = calibMods + [getModSpec("Filter",5000)]

        # Add the PseudoJetGetter alg to the sequence
        constitPJAlg = getConstitPJGAlg( jetConstit )
        constitPJKey = constitPJAlg.PJGetter.OutputContainer
        recoSeq += constitPJAlg
        # Basic list of PseudoJets is just the constituents
        # Append ghosts (tracks) if desired
        pjs = [constitPJKey]

        if "sub" in jetRecoDict["jetCalib"]:
            # Add the event shape alg if needed for area subtraction
            from JetRecConfig.JetRecConfig import getEventShapeAlg
            eventShapeAlg = getEventShapeAlg( jetConstit, constitPJKey, "TrigJet" )
            recoSeq += eventShapeAlg

        # Generate a JetAlgorithm to run the jet finding and modifiers
        # (via a JetRecTool instance).
        jetRecAlg = JetRecConfig.getJetAlgorithm(jetsFullName, jetDef, pjs, jetModList)
        recoSeq += jetRecAlg
        # End of basic jet reco
        pass

    return (recoSeq,sequenceOut)
