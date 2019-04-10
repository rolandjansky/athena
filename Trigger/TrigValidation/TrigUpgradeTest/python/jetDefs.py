#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#


from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.Constants import VERBOSE,DEBUG,INFO
from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq


def jetAthSequence(ConfigFlags):
    from TrigT2CaloCommon.CaloDef import clusterFSInputMaker
    InputMakerAlg= clusterFSInputMaker()
    (recoSequence, sequenceOut) = jetRecoSequence()

    JetAthSequence =  seqAND("jetAthSequence",[InputMakerAlg, recoSequence ])
    return (JetAthSequence, InputMakerAlg, sequenceOut)

    
def jetRecoSequence(RoIs = 'FSJETRoI'):

    from TrigT2CaloCommon.CaloDef import HLTFSTopoRecoSequence
    (caloMakerSequence, caloclusters) = HLTFSTopoRecoSequence(RoIs)

    from JetRecConfig.JetDefinition import JetConstit, JetDefinition, xAODType
  
    #hardcoded jet collection for now 
    # chosen jet collection
    jetsFullName = "TrigAntiKt4EMTopoSubJES"
    clustermods = ["ECPSFrac","ClusterMoments"]
    #clustermods = []
    trigMinPt = 7e3
    TrigEMTopo = JetConstit( xAODType.CaloCluster, ["EM"])
    TrigEMTopo.ptmin = 2e3
    TrigEMTopo.ptminfilter = 15e3
    TrigAntiKt4EMTopoSubJES = JetDefinition( "AntiKt", 0.4, TrigEMTopo, ptmin=trigMinPt,ptminfilter=trigMinPt)
    TrigAntiKt4EMTopoSubJES.modifiers = ["Calib:TrigEMSubJES:mc","Sort"] + clustermods 
    #TrigAntiKt4EMTopoSubJES.isTrigger = True
    jetDefinition = TrigAntiKt4EMTopoSubJES


    from JetRecConfig import JetRecConfig
    deps = JetRecConfig.resolveDependencies( jetDefinition )

    print("resolved dependencies: ", deps)

    inputdeps = deps["inputs"]
    constit = inputdeps[0]
    constit.rawname = caloclusters
    #constit.inputname = caloclusters
    print("constit.rawname = ", constit.rawname )
    print("constit.inputname = ", constit.inputname )
    constitAlg = getConstitAlg( constit )
    jetRecoSequence = seqAND( "JetRecoSeq", [constitAlg])

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
        ghostPJAlg = getGhostPJGAlg( ghostdef )
        jetRecoSequence += ghostPJAlg
        ghostPJKey = ghostPJAlg.PJGetter.OutputContainer
        pjs.append( ghostpjkey )

    # Generate a JetAlgorithm to run the jet finding and modifiers
    # (via a JetRecTool instance).
    jetRecAlg = JetRecConfig.getJetAlgorithm(jetsFullName, jetDefinition, pjs, deps["mods"])

    jetRecoSequence += jetRecAlg

    #sequenceOut = 'HLTJets_EMSubJES'
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

