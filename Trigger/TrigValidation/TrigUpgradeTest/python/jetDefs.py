#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#


from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.Constants import VERBOSE,DEBUG,INFO
from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq
from AthenaCommon.Constants import ERROR, DEBUG


def jetAthSequence(ConfigFlags):
    from TrigT2CaloCommon.CaloDef import clusterFSInputMaker
    InputMakerAlg= clusterFSInputMaker()

    dataOrMC = "data"  #FIXME
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

    calibSeq = ""
    dataType = "data" # FIXME
    if calibString:
        calibSeq = calibString.split(":")[1]
        dataType = calibString.split(":")[2]

    cellMakerAlgo = _getHLTCellMakerAlgoForJets("cellMaker", RoIs, OutputLevel=ERROR) 
    topoClusterMakerAlgo = _getHLTTopoClusterMakerAlgoForJets( "topoClusterMaker", inputEDM=cellMakerAlgo.CellsName, OutputLevel=ERROR)

    caloMakerSequence = parOR("TopoClusterRecoSequence"+calibSeq, [cellMakerAlgo, topoClusterMakerAlgo]) 
    caloclusters = topoClusterMakerAlgo.CaloClusters

    from JetRecConfig.JetDefinition import JetConstit, JetDefinition, xAODType, JetModifier
  
    #hardcoded jet collection for now 
    # chosen jet collection
    jetsFullName = "TrigAntiKt4EMTopo"+calibSeq
    trigMinPt = 7e3 # FIXME : two different values for ptminfilter?
    TrigEMTopo = JetConstit( xAODType.CaloCluster, ["EM"])
    TrigEMTopo.ptmin = 2e3
    #TrigEMTopo.ptminfilter = 15e3
    TrigEMTopo.ptminfilter = trigMinPt
    TrigAntiKt4EMTopo = JetDefinition( "AntiKt", 0.4, TrigEMTopo, ptmin=trigMinPt,ptminfilter=trigMinPt)
    #TrigAntiKt4EMTopo.modifiers = ["Sort"] + clustermods 
    #if calibString:
    #    TrigAntiKt4EMTopo.modifiers= [calibString] + TrigAntiKt4EMTopo.modifiers
    TrigAntiKt4EMTopo.isTrigger = True #FIXME : no longer needed since confire CalubTool here?
    jetDefinition = TrigAntiKt4EMTopo

    from JetRecConfig import JetRecConfig
    #deps = JetRecConfig.resolveDependencies( jetDefinition )

    modList = [ (JetModifier("JetCaloEnergies", "jetens"), '') ]
    modList += [ (JetModifier("JetSorter","jetsort"), '') ]
    #deps_mod_tmp = [ modpair for modpair in deps["mods"] if not calibSeq in modpair[1] ]
    #deps["mods"] = deps_mod_tmp
    if calibString:
   	 from JetCalibTools import JetCalibToolsConfig
   	 jetCalibTool = JetCalibToolsConfig.getJetCalibTool( jetDefinition.basename, calibSeq, dataType )
   	 modList += [(JetModifier("JetCalibrationTool", jetCalibTool.name()), calibSeq+':'+dataType)]
    #jetFilterTool = _getJetFilterTool(TrigEMTopo.ptminfilter)
    modList += [ ( JetModifier("JetFilterTool","jetptfilter"), str(int(TrigEMTopo.ptminfilter)))  ]

    print("modList: ", modList)

    constit = TrigEMTopo
    constit.rawname = caloclusters
    print("constit.rawname = ", constit.rawname )
    print("constit.inputname = ", constit.inputname )
    constitAlg = _getConstitAlg( constit )
    jetRecoSequence = parOR( "JetRecSeq_"+jetsFullName, [constitAlg])

    constitPJAlg = _getConstitPJGAlg( constit )
    constitPJKey = constitPJAlg.PJGetter.OutputContainer

    pjs = [constitPJKey]

    jetRecoSequence += constitPJAlg 
        
    eventShapeAlg = _getEventShapeAlg( constit, constitPJKey )
    jetRecoSequence += eventShapeAlg                    

    # Schedule the ghost PseudoJetGetterAlgs # FIXME : not needed.
    ghostList  = []
    for ghostdef in ghostList:
        print("ghostdef = ", ghostdef)
        ghostPJAlg = _getGhostPJGAlg( ghostdef )
        jetRecoSequence += ghostPJAlg
        ghostPJKey = ghostPJAlg.PJGetter.OutputContainer
        pjs.append( ghostPJKey )

    # Generate a JetAlgorithm to run the jet finding and modifiers
    # (via a JetRecTool instance).
    jetRecAlg = JetRecConfig.getJetAlgorithm(jetsFullName, jetDefinition, pjs, modList)

    jetRecoSequence += jetRecAlg

    sequenceOut = jetsFullName

    caloMakerSequence += jetRecoSequence
    #jetRecoSequence += caloMakerSequence

    #jetRecoFullSequence = seqAND("fullJetSeq_"+jetsFullName, [caloMakerSequence, jetRecoSequence]) 
    jetRecoFullSequence = caloMakerSequence
    #jetRecoFullSequence = jetRecoSequence

    return (jetRecoFullSequence,sequenceOut)

def jetRecoSequenceLCW(calibString, RoIs = 'FSJETRoI'):

    calibSeq = ""
    dataType = "data"
    if calibString:
        calibSeq = calibString.split(":")[1]
        dataType = calibString.split(":")[2]

    cellMakerAlgo = _getHLTCellMakerAlgoForJets("cellMakerLC", RoIs, outputEDM='CaloCellsLC', OutputLevel=ERROR) 
    topoClusterMakerAlgo = _getHLTTopoClusterMakerAlgoForJets( "topoClusterMakerLC", inputEDM=cellMakerAlgo.CellsName, doLC=True, OutputLevel=ERROR)

    caloMakerSequence = parOR("TopoClusterRecoSequenceLC"+calibSeq, [cellMakerAlgo, topoClusterMakerAlgo]) 
    caloclusters = topoClusterMakerAlgo.CaloClusters

    from JetRecConfig.JetDefinition import JetConstit, JetDefinition, xAODType, JetModifier
  
    #hardcoded jet collection for now 
    # chosen jet collection
    jetsFullName = "TrigAntiKt4LCW"+calibSeq
    trigMinPt = 7e3
    TrigLCW = JetConstit( xAODType.CaloCluster, ["LC"])
    TrigLCW.ptmin = 2e3
    #TrigLCW.ptminfilter = 15e3
    TrigLCW.ptminfilter = trigMinPt
    TrigAntiKt4LCW = JetDefinition( "AntiKt", 0.4, TrigLCW, ptmin=trigMinPt,ptminfilter=trigMinPt)
    #TrigAntiKt4LCW.modifiers = ["Sort"] + clustermods 
    TrigAntiKt4LCW.isTrigger = True
    jetDefinition = TrigAntiKt4LCW

    from JetRecConfig import JetRecConfig

    modList = [ (JetModifier("JetCaloEnergies", "jetens"), '') ]
    modList += [ (JetModifier("JetSorter","jetsort"), '') ]
    if calibString:
   	 from JetCalibTools import JetCalibToolsConfig
   	 jetCalibTool = JetCalibToolsConfig.getJetCalibTool( jetDefinition.basename, calibSeq, dataType )
   	 modList += [(JetModifier("JetCalibrationTool", jetCalibTool.name()), calibSeq+':'+dataType)]
    #jetFilterTool = _getJetFilterTool(TrigLCW.ptminfilter)
    modList += [ ( JetModifier("JetFilterTool","jetptfilter"), str(int(TrigLCW.ptminfilter)))  ]

    print("modList: ", modList)

    constit = TrigLCW
    constit.rawname = caloclusters
    print("constit.rawname = ", constit.rawname )
    print("constit.inputname = ", constit.inputname )
    constitAlg = _getConstitAlg( constit )
    jetRecoSequence = parOR( "JetRecSeq_"+jetsFullName, [constitAlg])

    constitPJAlg = _getConstitPJGAlg( constit )
    constitPJKey = constitPJAlg.PJGetter.OutputContainer

    pjs = [constitPJKey]

    jetRecoSequence += constitPJAlg 
        
    eventShapeAlg = _getEventShapeAlg( constit, constitPJKey )
    jetRecoSequence += eventShapeAlg                    

    # Schedule the ghost PseudoJetGetterAlgs
    ghostList = [] # FIXME
    for ghostdef in ghostList:
        print("ghostdef = ", ghostdef)
        ghostPJAlg = _getGhostPJGAlg( ghostdef )
        jetRecoSequence += ghostPJAlg
        ghostPJKey = ghostPJAlg.PJGetter.OutputContainer
        pjs.append( ghostPJKey )

    # Generate a JetAlgorithm to run the jet finding and modifiers
    # (via a JetRecTool instance).
    jetRecAlg = JetRecConfig.getJetAlgorithm(jetsFullName, jetDefinition, pjs, modList)

    jetRecoSequence += jetRecAlg

    sequenceOut = jetsFullName

    caloMakerSequence += jetRecoSequence

    jetRecoFullSequence = caloMakerSequence

    return (jetRecoFullSequence,sequenceOut)

def _getConstitAlg( constit ):

        from JetRecConfig import ConstModHelpers
        constitalg = ConstModHelpers.getConstitModAlg(constit)
        return constitalg

def _getConstitPJGAlg(basedef):

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

def _getGhostPJGAlg(ghostdef):

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

def _getTrackPrepAlg( trackPrepAlgName ):

    from JetRecTools import JetRecToolsConfig
    from JetRec import JetRecConf
    # Jet track selection
    jettrackselloose = JetRecToolsConfig.getTrackSelTool()
    jettvassoc = JetRecToolsConfig.getTrackVertexAssocTool()

    jettrkprepalg = JetRecConf.JetAlgorithm(trackPrepAlgName)
    jettrkprepalg.Tools = [ jettrackselloose, jettvassoc ]

    return jettrkprepalg

def _getEventShapeAlg( constit, constitpjkey ):

    rhokey = "Kt4"+constit.label+"EventShape"
    rhotoolname = "EventDensity_Kt4"+constit.label
    
    from EventShapeTools import EventShapeToolsConf
    rhotool = EventShapeToolsConf.EventDensityTool(rhotoolname)
    rhotool.InputContainer = constitpjkey
    rhotool.OutputContainer = rhokey
    
    eventshapealg = EventShapeToolsConf.EventDensityAthAlg("{0}Alg".format(rhotoolname))
    eventshapealg.EventDensityTool = rhotool

    return eventshapealg

def _getHLTCellMakerAlgoForJets( name, RoIs, outputEDM='CaloCells', OutputLevel=ERROR ):

    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    if not hasattr(svcMgr,'TrigCaloDataAccessSvc'):
      from TrigT2CaloCommon.TrigT2CaloCommonConf import TrigCaloDataAccessSvc
      svcMgr+=TrigCaloDataAccessSvc()
      svcMgr.TrigCaloDataAccessSvc.OutputLevel=ERROR
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    from TrigCaloRec.TrigCaloRecConfig import HLTCaloCellMaker

    inputEDM = RoIs
    algo=HLTCaloCellMaker(name)
    algo.RoIs=inputEDM
    algo.TrigDataAccessMT=svcMgr.TrigCaloDataAccessSvc
    algo.OutputLevel=OutputLevel
    algo.CellsName=outputEDM
   
    return algo

def _getHLTTopoClusterMakerAlgoForJets(name, inputEDM, doLC=False, OutputLevel=ERROR):

    from TrigCaloRec.TrigCaloRecConfig import TrigCaloClusterMakerMT_topo

    algo = TrigCaloClusterMakerMT_topo(name, doMoments=True, doLC=doLC, cells=inputEDM)
    algo.CaloClusters="caloclusters"
    if doLC:
        algo.CaloClusters+="LC"
    algo.OutputLevel=OutputLevel
    return algo

def _getJetFilterTool( threshold ):
	
    from JetRec import JetRecConf
    threshold = int(threshold)
    jetptfilter = JetRecConf.JetFilterTool("jetptfilter_{0}mev".format(threshold))
    jetptfilter.PtMin = threshold     
    return jetptfilter


