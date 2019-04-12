#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#


from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.Constants import VERBOSE,DEBUG,INFO
from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq


def jetAthSequence(ConfigFlags):
    from TrigT2CaloCommon.CaloDef import clusterFSInputMaker
    InputMakerAlg= clusterFSInputMaker()

    dataOrMC = "data"
    #if ConfigFlags.Input.isMC:
    #    dataOrMC = "mc"
    # want to make this automatic.

    jetDefinition = ConfigFlags.jetdefinition

    if jetDefinition=="EMTopoSubJESIS":
    	(recoSequence, sequenceOut) = jetRecoSequenceEMTopo("Calib:TrigSubJESIS:"+dataOrMC)
    elif jetDefinition=="EMTopoSubJES":
    	(recoSequence, sequenceOut) = jetRecoSequenceEMTopo("Calib:TrigSubJES:"+dataOrMC)
    elif jetDefinition=="EMTopoNoCalib":
    	(recoSequence, sequenceOut) = jetRecoSequenceEMTopo("")
    elif jetDefinition=="LCWSubJESIS": # call for LC sequence
    	(recoSequence, sequenceOut) = jetRecoSequenceLCW("Calib:TrigSubJESIS:"+dataOrMC)

    JetAthSequence =  seqAND("jetAthSequence_"+jetDefinition,[InputMakerAlg, recoSequence ])
    return (JetAthSequence, InputMakerAlg, sequenceOut)

    
def jetRecoSequenceEMTopo(calibString, RoIs = 'FSJETRoI'):

    from TrigT2CaloCommon.CaloDef import HLTFSTopoRecoSequence
    #(caloMakerSequence, caloclusters) = HLTFSTopoRecoSequence(RoIs)
    (jetRecoSequence, caloclusters) = HLTFSTopoRecoSequence(RoIs)

    from JetRecConfig.JetDefinition import JetConstit, JetDefinition, xAODType
  
    #hardcoded jet collection for now 
    # chosen jet collection
    calibSeq = ""
    if calibString:
        calibSeq = calibString.split(":")[1]

    #caloMakerSequence.name = "TrigCaloClusterMakerMT_topo_"+calibSeq
    jetsFullName = "TrigAntiKt4EMTopo"+calibSeq
    #clustermods = ["ECPSFrac","ClusterMoments"]
    clustermods = []
    trigMinPt = 7e3
    TrigEMTopo = JetConstit( xAODType.CaloCluster, ["EM"])
    TrigEMTopo.ptmin = 2e3
    TrigEMTopo.ptminfilter = 15e3
    TrigAntiKt4EMTopo = JetDefinition( "AntiKt", 0.4, TrigEMTopo, ptmin=trigMinPt,ptminfilter=trigMinPt)
    TrigAntiKt4EMTopo.modifiers = ["Sort"] + clustermods 
    if calibString:
        TrigAntiKt4EMTopo.modifiers= [calibString] + TrigAntiKt4EMTopo.modifiers
    TrigAntiKt4EMTopo.isTrigger = True
    jetDefinition = TrigAntiKt4EMTopo

    print("printing jet definition info ")
    print(" jet basename = ", jetDefinition.basename)
    print(" jet modifiers = ", jetDefinition.modifiers)
    print(" calibString = ", calibString)
    print(" jets full name = ", jetsFullName)


    from JetRecConfig import JetRecConfig
    deps = JetRecConfig.resolveDependencies( jetDefinition )

    print("TrigAntiKt4EMTopo.modifiers = ", TrigAntiKt4EMTopo.modifiers )
    print("resolved dependencies: ", deps)

    inputdeps = deps["inputs"]
    constit = inputdeps[0]
    constit.rawname = caloclusters
    #constit.inputname = caloclusters
    print("constit.rawname = ", constit.rawname )
    print("constit.inputname = ", constit.inputname )
    constitAlg = getConstitAlg( constit )
    #jetRecoSequence = parOR( "JetRecSeq_"+jetsFullName, [constitAlg])
    jetRecoSequence+= constitAlg

    constitPJAlg = getConstitPJGAlg( constit )
    constitPJKey = constitPJAlg.PJGetter.OutputContainer

    pjs = [constitPJKey]

    jetRecoSequence += constitPJAlg 
        
    if "JetSelectedTracks" in inputdeps or "JetTrackVtxAssoc" in inputdeps:
        jetTrkPrepAlg = getTrackPrepAlg( "trigjetalg_TrackPrep")
        jetRecoSequence += jetTrkPrepAlg
        
    eventShapeAlg = getEventShapeAlg( constit, constitPJKey )
    jetRecoSequence += eventShapeAlg                    

    # Schedule the ghost PseudoJetGetterAlgs
    for ghostdef in deps["ghosts"]:
        print("ghostdef = ", ghostdef)
        ghostPJAlg = getGhostPJGAlg( ghostdef )
        jetRecoSequence += ghostPJAlg
        ghostPJKey = ghostPJAlg.PJGetter.OutputContainer
        pjs.append( ghostPJKey )

    # Generate a JetAlgorithm to run the jet finding and modifiers
    # (via a JetRecTool instance).
    jetRecAlg = JetRecConfig.getJetAlgorithm(jetsFullName, jetDefinition, pjs, deps["mods"])

    jetRecoSequence += jetRecAlg

    sequenceOut = jetsFullName

    #caloMakerSequence += jetRecoSequence

    #jetRecoFullSequence = seqAND("fullJetRecoSequence", [caloMakerSequence, jetRecoSequence]) 
    #jetRecoFullSequence = caloMakerSequence
    jetRecoFullSequence = jetRecoSequence

    return (jetRecoFullSequence,sequenceOut)

def jetRecoSequenceLCW(calibString, RoIs = 'FSJETRoI'):

    from TrigT2CaloCommon.CaloDef import HLTLCTopoRecoSequence
    (caloMakerSequence, caloclusters) = HLTLCTopoRecoSequence(RoIs)

    from JetRecConfig.JetDefinition import JetConstit, JetDefinition, xAODType
  
    #hardcoded jet collection for now 
    # chosen jet collection
    calibSeq = ""
    if calibString:
        calibSeq = calibString.split(":")[1]
    jetsFullName = "TrigAntiKt4LCW_"+calibSeq
    #clustermods = ["ECPSFrac","ClusterMoments"]
    clustermods = []
    trigMinPt = 7e3
    TrigLCW = JetConstit( xAODType.CaloCluster, ["LC"])
    TrigLCW.ptmin = 2e3
    TrigLCW.ptminfilter = 15e3
    TrigAntiKt4LCW = JetDefinition( "AntiKt", 0.4, TrigLCW, ptmin=trigMinPt,ptminfilter=trigMinPt)
    TrigAntiKt4LCW.modifiers = ["Sort"] + clustermods 
    if calibString:
        TrigAntiKt4LCW.modifiers+= [calibString]
    TrigAntiKt4LCW.isTrigger = True
    jetDefinition = TrigAntiKt4LCW

    from JetRecConfig import JetRecConfig
    deps = JetRecConfig.resolveDependencies( jetDefinition )

    inputdeps = deps["inputs"]
    constit = inputdeps[0]
    constit.rawname = caloclusters
    #constit.inputname = caloclusters
    print("constit.rawname = ", constit.rawname )
    print("constit.inputname = ", constit.inputname )
    constitAlg = getConstitAlg( constit )
    jetRecoSequence = parOR( "JetRecSeq_"+jetsFullName, [constitAlg])

    constitPJAlg = getConstitPJGAlg( constit )
    constitPJKey = constitPJAlg.PJGetter.OutputContainer

    pjs = [constitPJKey]

    jetRecoSequence += constitPJAlg 
        
    if "JetSelectedTracks" in inputdeps or "JetTrackVtxAssoc" in inputdeps:
        jetTrkPrepAlg = getTrackPrepAlg( "trigjetalg_TrackPrep")
        jetRecoSequence += jetTrkPrepAlg
        
    eventShapeAlg = getEventShapeAlg( constit, constitPJKey )
    jetRecoSequence += eventShapeAlg                    

    # Schedule the ghost PseudoJetGetterAlgs
    for ghostdef in deps["ghosts"]:
        print("ghostdef = ", ghostdef)
        ghostPJAlg = getGhostPJGAlg( ghostdef )
        jetRecoSequence += ghostPJAlg
        ghostPJKey = ghostPJAlg.PJGetter.OutputContainer
        pjs.append( ghostPJKey )

    # Generate a JetAlgorithm to run the jet finding and modifiers
    # (via a JetRecTool instance).
    jetRecAlg = JetRecConfig.getJetAlgorithm(jetsFullName, jetDefinition, pjs, deps["mods"])

    jetRecoSequence += jetRecAlg

    sequenceOut = jetsFullName

    caloMakerSequence += jetRecoSequence

    #jetRecoFullSequence = seqAND("fullJetRecoSequence", [caloMakerSequence, jetRecoSequence]) 
    jetRecoFullSequence = caloMakerSequence

    return (jetRecoFullSequence,sequenceOut)

def getConstitAlg( constit ):

        from JetRecConfig import ConstModHelpers
        constitalg = ConstModHelpers.getConstitModAlg(constit)
        return constitalg

def getConstitPJGAlg(basedef):

    from JetRec import JetRecConf
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

def getGhostPJGAlg(ghostdef):

    label = "Ghost"+ghostdef.inputtype
    kwargs = {
        "OutputContainer":    "PseudoJet"+label,
        "Label":              label,
        "SkipNegativeEnergy": True,
        "GhostScale":         1e-40
        }

    from JetRec import JetRecConf
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

def getTrackPrepAlg( trackPrepAlgName ):

        from JetRecTools import JetRecToolsConfig
        from JetRec import JetRecConf
        # Jet track selection
        jettrackselloose = JetRecToolsConfig.getTrackSelTool()
        jettvassoc = JetRecToolsConfig.getTrackVertexAssocTool()

        jettrkprepalg = JetRecConf.JetAlgorithm(trackPrepAlgName)
        jettrkprepalg.Tools = [ jettrackselloose, jettvassoc ]

        return jettrkprepalg

def getEventShapeAlg( constit, constitpjkey ):

        rhokey = "Kt4"+constit.label+"EventShape"
        rhotoolname = "EventDensity_Kt4"+constit.label
        
        from EventShapeTools import EventShapeToolsConf
        rhotool = EventShapeToolsConf.EventDensityTool(rhotoolname)
        rhotool.InputContainer = constitpjkey
        rhotool.OutputContainer = rhokey
        
        eventshapealg = EventShapeToolsConf.EventDensityAthAlg("{0}Alg".format(rhotoolname))
        eventshapealg.EventDensityTool = rhotool

        return eventshapealg

