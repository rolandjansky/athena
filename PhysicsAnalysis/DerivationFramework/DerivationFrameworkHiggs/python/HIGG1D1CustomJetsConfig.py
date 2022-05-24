# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#==============================================================================
# Contains the configuration for customs jet reconstruction + decorations
# used in analysis DAODs
#==============================================================================

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def HIGG1D1CustomJetsCfg(ConfigFlags):
    """Jet reconstruction needed for HIGG1D1"""

    acc = ComponentAccumulator()

    # Ideally there would be a nice way to configure the PFlowCustomVtx jets  but at the moment 
    # all tools need to be configured manually to ensure that the track to vertex association is done correctly.
    PrefixForHggCollection   = "Hgg"
    HggVertexContainerName   = PrefixForHggCollection+"PrimaryVertices"
    CustomPFJetContainerName = "AntiKt4EMPFlowCustomVtxJets"

    from JetRecConfig.StandardJetConstits import stdInputExtDic, JetInputExternal,JetInputConstit, JetInputConstitSeq, JetConstitModifier, xAODType
    from JetRecConfig.StandardSmallRJets import AntiKt4EMPFlow
    from JetRecConfig.JetDefinition import JetDefinition 
    from JetRecTools import JetRecToolsConfig as jrtcfg
    from JetMomentTools import JetMomentToolsConfig
    from JetRecConfig.StandardJetConstits import stdConstitDic, stdContitModifDic
    from JetRecConfig.StandardJetContext import propFromContext, inputsFromContext
    from JetRecConfig.JetInputConfig import buildEventShapeAlg

    # Create new jet context
    from JetRecConfig.StandardJetContext import jetContextDic
    jetContextName = 'CustomVtx'
    if jetContextName not in jetContextDic:
      jetContextDic[jetContextName] = jetContextDic['default'].clone(
        Vertices         = HggVertexContainerName,
        GhostTracks      = "PseudoJetGhostTrack"+jetContextName, 
        GhostTracksLabel = "GhostTrack"+jetContextName,
        TVA              = "JetTrackVtxAssoc"+jetContextName,
        JetTracks        = "JetSelectedTracks"+jetContextName,
        JetTracksQualityCuts = "JetSelectedTracks"+jetContextName+"_trackSelOpt"
      )
    context =  jetContextDic[jetContextName] 

    def replaceItems(tup,orgName,newName):
        newList = list(tup)
        for i, item in enumerate(newList):
            if orgName in item:
                newList[i] = item.replace(orgName,newName)
                print( "Updated ", orgName, " to ", newName )
                return tuple(newList)
        print( "Failed to update ", orgName, " to ", newName )
        return tuple(newList)
    
    def updateCalibSequence(tup):
        newList = list(tup)
        for i, item in enumerate(newList):
            if "Calib" in item:
                calibspecs = item.split(":")
                calib, calibcontext, data_type = calibspecs[:3]
                calibseq=""
                if len(calibspecs)>3: 
                  calibseq = calibspecs[3]
                rhoname = "Kt4EMPFlowCustomVtxEventShape"
                pvname = HggVertexContainerName
                finalCalibString = f"CalibCustomVtx:{calibcontext}:{data_type}:{calibseq}:{rhoname}:{pvname}"
                if len(calibspecs)>6: finalCalibString = f"{finalCalibString}:{calibspecs[6]}"
                newList[i] = finalCalibString
                print(finalCalibString)
                return tuple(newList)
        print( "Failed to update calib sequence" )
        return tuple(newList)


    # Create modifier list and JetDefinition 
    modsCustomVtx = AntiKt4EMPFlow.modifiers
    modsCustomVtx = updateCalibSequence(modsCustomVtx)
    modsCustomVtx = replaceItems(modsCustomVtx,"TrackMoments","TrackMomentsCustomVtx")
    modsCustomVtx = replaceItems(modsCustomVtx,"TrackSumMoments","TrackSumMomentsCustomVtx")
    modsCustomVtx = replaceItems(modsCustomVtx,"JVF","JVFCustomVtx")
    modsCustomVtx = replaceItems(modsCustomVtx,"JVT","JVTCustomVtx")
    modsCustomVtx = replaceItems(modsCustomVtx,"Charge","ChargeCustomVtx")

    ghostCustomVtx = AntiKt4EMPFlow.ghostdefs
    ghostCustomVtx = replaceItems(ghostCustomVtx,"Track","TrackCustomVtx")

    # GPFlow are the same than EMPFlow except they have pflow linked to elec or muons filtered out.
    stdConstitDic["TrackCustomVtx"]  = JetInputConstit("TrackCustomVtx", xAODType.TrackParticle,"JetSelectedTracksCustomVtx" )
    
    
    stdConstitDic["GPFlowCustomVtx"] = JetInputConstitSeq("GPFlowCustomVtx", xAODType.FlowElement,["CorrectPFOCustomVtx", "CHSCustomVtx"] , 'GlobalParticleFlowObjects', 'CHSGCustomVtxParticleFlowObjects',
                        label='EMPFlow')

    stdContitModifDic["CorrectPFOCustomVtx"] = JetConstitModifier("CorrectPFOCustomVtx", "CorrectPFOTool",
                                                                  prereqs=[inputsFromContext("Vertices")],
                                                                  properties=dict(VertexContainerKey=propFromContext("Vertices"),
                                                                                  WeightPFOTool= CompFactory.getComp("CP::WeightPFOTool")("weightPFO") )
                                                                  )

    stdContitModifDic["CHSCustomVtx"] = JetConstitModifier("CHSCustomVtx",    "ChargedHadronSubtractionTool",
                                                          prereqs=  [inputsFromContext("Vertices"),inputsFromContext("TVA")],
                                                          properties=dict(VertexContainerKey=propFromContext("Vertices"),                                       
                                                                          TrackVertexAssociation=propFromContext("TVA"),
                                                                          UseTrackToVertexTool=True, 
                                                                          ))



    AntiKt4EMPFlowCustomVtx = JetDefinition("AntiKt",0.4,stdConstitDic.GPFlowCustomVtx,
                                        infix = "CustomVtx",
                                        context = jetContextName,
                                        ghostdefs = ghostCustomVtx,
                                        modifiers = modsCustomVtx+("JetPtAssociation","QGTaggingCustomVtx","fJVTCustomVtx","NNJVTCustomVtx"),
                                        ptmin = 10000,
    )

    def getUsedInVertexFitTrackDecoratorAlgCustomVtx(jetdef, jetmod):
        """ Create the alg  to decorate the used-in-fit information for AMVF """
        from JetRecConfig.StandardJetContext import jetContextDic
        context  = jetContextDic[jetdef.context] 
        
        alg = CompFactory.getComp("InDet::InDetUsedInVertexFitTrackDecorator")(
            "UsedInVertexFitTrackDecoratorAlgCustomVtxAlg" ,
            UsedInFitDecoratorTool=CompFactory.getComp("InDet::InDetUsedInFitTrackDecoratorTool")(
                "UsedInFitTrackDecoratorToolCustomVtx",
                AMVFVerticesDecoName= 'TTVA_AMVFVertices_forReco',
                AMVFWeightsDecoName= 'TTVA_AMVFWeights_forReco',
                TrackContainer=context['Tracks'],
                VertexContainer=context['Vertices'],
            ),
        )
        return alg


    # Define new input variables for jet configuration
    stdInputExtDic[context['Vertices']] = JetInputExternal( context['Vertices'],   xAODType.Vertex )

    stdInputExtDic["JetSelectedTracksCustomVtx"] = JetInputExternal("JetSelectedTracksCustomVtx",     xAODType.TrackParticle,
                                                                                    prereqs= [ f"input:{context['Tracks']}" ], # in std context, this is InDetTrackParticles (see StandardJetContext)
                                                                                    algoBuilder = lambda jdef,_ : jrtcfg.getTrackSelAlg(jdef.context, trackSelOpt=False )
                                                                                 )

    stdInputExtDic["JetTrackUsedInFitDecoCustomVtx"] = JetInputExternal("JetTrackUsedInFitDecoCustomVtx", xAODType.TrackParticle,
                                                                        prereqs= [ f"input:{context['Tracks']}" , # in std context, this is InDetTrackParticles (see StandardJetContext)
                                                                                  f"input:{context['Vertices']}"],
                                                                        algoBuilder = getUsedInVertexFitTrackDecoratorAlgCustomVtx
                                                                        )

    stdInputExtDic["JetTrackVtxAssocCustomVtx"] = JetInputExternal("JetTrackVtxAssocCustomVtx",  xAODType.TrackParticle,
                                              algoBuilder = lambda jdef,_ : jrtcfg.getJetTrackVtxAlg(jdef.context, algname="jetTVACustomVtx", WorkingPoint="Nonprompt_All_MaxWeight"),
                                              prereqs = [ "input:JetTrackUsedInFitDecoCustomVtx", f"input:{context['Vertices']}" ] )

    stdInputExtDic["EventDensityCustomVtx"] =     JetInputExternal("EventDensityCustomVtx", "EventShape", algoBuilder = buildEventShapeAlg,
                      containername = lambda jetdef, _ : "Kt4"+jetdef.inputdef.label+"CustomVtxEventShape",
                      prereqs = lambda jetdef : ["input:"+jetdef.inputdef.name] )

    from JetRecConfig.StandardJetMods import stdJetModifiers
    from JetRecConfig.JetDefinition import JetModifier
    from JetCalibTools import JetCalibToolsConfig

    stdJetModifiers.update(

      CalibCustomVtx = JetModifier("JetCalibrationTool","jetcalib_jetcoll_calibseqCustomVtx",
                                        createfn=JetCalibToolsConfig.getJetCalibToolFromString,
                                        prereqs=lambda mod,jetdef : JetCalibToolsConfig.getJetCalibToolPrereqs(mod,jetdef)+[f"input:{context['Vertices']}"]),


      JVFCustomVtx =             JetModifier("JetVertexFractionTool", "jvfCustomVtx",
                                        createfn= lambda jdef,_ : JetMomentToolsConfig.getJVFTool(jdef.context,"CustomVtx"),
                                        modspec = "CustomVtx",
                                        prereqs = ["mod:TrackMomentsCustomVtx", f"input:{context['Vertices']}"] ,JetContainer = CustomPFJetContainerName),

      JVTCustomVtx =             JetModifier("JetVertexTaggerTool", "jvtCustomVtx",
                                        createfn= lambda jdef,_ : JetMomentToolsConfig.getJVTTool(jdef.context,"CustomVtx"),
                                        modspec = "CustomVtx",
                                        prereqs = [ "mod:JVFCustomVtx" ],JetContainer = CustomPFJetContainerName),

      NNJVTCustomVtx =           JetModifier("JetVertexNNTagger", "nnjvtCustomVtx",
                                              createfn=lambda jdef,_ :JetMomentToolsConfig.getNNJvtTool(jdef.context,"CustomVtx"),
                                              prereqs = [ "mod:JVFCustomVtx" ],JetContainer = CustomPFJetContainerName),

      OriginSetPVCustomVtx =     JetModifier("JetOriginCorrectionTool", "origin_setpvCustomVtx",
                                        modspec = "CustomVtx",
                                        prereqs = [ "mod:JVFCustomVtx" ],JetContainer = CustomPFJetContainerName, OnlyAssignPV=True),

      TrackMomentsCustomVtx =    JetModifier("JetTrackMomentsTool", "trkmomsCustomVtx",
                                        createfn= lambda jdef,_ : JetMomentToolsConfig.getTrackMomentsTool(jdef,"CustomVtx"),
                                        modspec = "CustomVtx",
                                        prereqs = [ "input:JetTrackVtxAssocCustomVtx","ghost:TrackCustomVtx" ],JetContainer = CustomPFJetContainerName),

      TrackSumMomentsCustomVtx = JetModifier("JetTrackSumMomentsTool", "trksummomsCustomVtx",
                                        createfn=lambda jdef,_ :JetMomentToolsConfig.getTrackSumMomentsTool(jdef,"CustomVtx"),
                                        modspec = "CustomVtx",
                                        prereqs = [ "input:JetTrackVtxAssocCustomVtx","ghost:TrackCustomVtx" ],JetContainer = CustomPFJetContainerName),

      ChargeCustomVtx =          JetModifier("JetChargeTool", "jetchargeCustomVtx", 
                                        prereqs = [ "ghost:TrackCustomVtx" ]),


      QGTaggingCustomVtx =       JetModifier("JetQGTaggerVariableTool", "qgtaggingCustomVtx",
                                        createfn=lambda jdef,_ :JetMomentToolsConfig.getQGTaggingTool(jdef,"CustomVtx"),
                                        modspec = "CustomVtx",
                                        prereqs = lambda _,jdef : ["mod:JetPtAssociation", "mod:TrackMomentsCustomVtx"] if not ConfigFlags.Input.isMC else ["mod:TrackMomentsCustomVtx"],
                                        JetContainer = CustomPFJetContainerName),

      fJVTCustomVtx =            JetModifier("JetForwardPFlowJvtTool", "fJVTCustomVtx",
                                        createfn=lambda jdef,_ :JetMomentToolsConfig.getPFlowfJVTTool(jdef,"CustomVtx"),
                                        modspec = "CustomVtx",
                                        prereqs = ["input:EventDensityCustomVtx",f"input:{context['Vertices']}"],
                                        JetContainer = CustomPFJetContainerName),
    )

    from JetRecConfig.JetRecConfig import JetRecCfg
    from JetRecConfig.JetConfigFlags import jetInternalFlags

    jetInternalFlags.isRecoJob = True
    acc.merge(JetRecCfg(ConfigFlags,AntiKt4EMPFlowCustomVtx))
  
    return acc



