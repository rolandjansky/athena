# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import re
import sys

import LeptonTaggers.LeptonTaggersConf as Conf

from AthenaCommon.Logging import logging

log = logging.getLogger('LeptonTaggersConfig.py')

#------------------------------------------------------------------------------
def ConfigureAntiKt4PV0TrackJets(privateSeq, name):

    from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets

    # Run track jet clustering. B-tagging is run at construction
    replaceAODReducedJets(['AntiKt4PV0TrackJets'], privateSeq, name)

#------------------------------------------------------------------------------
def GetDecoratePromptLeptonAlgs(name="", addSpectators=False):

    algs  = []

    if name == "" or name == 'Electrons':
        algs += [DecoratePromptLepton('PromptLeptonIso',  'Electrons', 'AntiKt4PV0TrackJets', addSpectators)]
        algs += [DecoratePromptLepton('PromptLeptonVeto', 'Electrons', 'AntiKt4PV0TrackJets', addSpectators)]
        algs += [DecorateReFitPrimaryVertex('Electrons')]
        algs += [DecorateNonPromptVertex('Electrons')]

    if name == "" or name == 'Muons':
        algs += [DecoratePromptLepton('PromptLeptonIso',  'Muons', 'AntiKt4PV0TrackJets', addSpectators)]
        algs += [DecoratePromptLepton('PromptLeptonVeto', 'Muons', 'AntiKt4PV0TrackJets', addSpectators)]
        algs += [DecorateReFitPrimaryVertex('Muons')]
        algs += [DecorateNonPromptVertex('Muons')]
    
    return algs

#------------------------------------------------------------------------------
def GetDecoratePromptTauAlgs():

    algs  = []

    algs += [DecoratePromptTau('PromptTauIso',  'TauJets', 'AntiKt4PV0TrackJets')]
    
    log.info('GetDecoratePromptTauAlgs - Due to the input variable has changed (missing MV2rmu), PromptTauVeto is not supported')

    return algs

#------------------------------------------------------------------------------
def GetExtraPromptVariablesForDxAOD(name='', addSpectators=False):

    prompt_lep_vars = []

    prompt_vars  = "PromptLeptonVeto.PromptLeptonIso."
    prompt_vars += "PromptLeptonInput_TrackJetNTrack.PromptLeptonInput_sv1_jf_ntrkv."
    prompt_vars += "PromptLeptonInput_ip2.PromptLeptonInput_ip3."
    prompt_vars += "PromptLeptonInput_LepJetPtFrac.PromptLeptonInput_DRlj."
    prompt_vars += "PromptLeptonInput_PtFrac.PromptLeptonInput_PtRel."
    prompt_vars += "PromptLeptonInput_DL1mu.PromptLeptonInput_rnnip."
    prompt_vars += "PromptLeptonInput_TopoEtCone30Rel.PromptLeptonInput_PtVarCone30Rel."

    prompt_vars += "PromptLeptonInput_SecondaryVertexIndexVector.PromptLeptonInput_SecondaryVertexIndexVectorInDet.PromptLeptonInput_SecondaryVertexIndexVectorMerge.PromptLeptonInput_SecondaryVertexIndexVectorDeepMerge."
    prompt_vars += "rhocen.rhofor.SecVtxLinks.RefittedPriVtxLink.RefittedPriVtxWithoutLeptonLink."

    secondaryvertex_vars = "SVType.trackParticleLinks.trackWeights.neutralParticleLinks.neutralWeights.SecondaryVertexIndex.SecondaryVertexIndexVectorInput.chiSquared.numberDoF.x.y.z.covariance.vertexType.energyFraction.mass.normDist.ntrk.distToPriVtx.normDistToPriVtx.distToRefittedPriVtx.normDistToRefittedPriVtx.distToRefittedRmLepPriVtx.normDistToRefittedRmLepPriVtx"
    
    if addSpectators :
        prompt_vars += "PromptLeptonInput_JetPt.PromptLeptonInput_JetEta.PromptLeptonInput_JetPhi.PromptLeptonInput_JetM."
    
    if name == "" or name == "Electrons":
        prompt_vars += "ptvarcone40.topoetcone20.topoetcone20ptCorrection.ptcone20_TightTTVA_pt500.ptcone20_TightTTVA_pt1000.ptvarcone20_TightTTVA_pt1000.ptvarcone30_TightTTVA_pt500.ptvarcone30_TightTTVA_pt1000.ptvarcone40_TightTTVALooseCone_pt500"

        prompt_lep_vars += ["Electrons.%s" %prompt_vars]
        prompt_lep_vars += ["SecVtxContainer_Electrons.%s" %secondaryvertex_vars]
        prompt_lep_vars += ["SecVtx_ConvVtxContainer_Electrons.%s" %secondaryvertex_vars]

    if name == "" or name == "Muons":
        prompt_vars += "ET_Core.ET_EMCore.ET_HECCore.ET_TileCore.EnergyLoss.EnergyLossSigma.MeasEnergyLoss.MeasEnergyLossSigma.ParamEnergyLoss.ParamEnergyLossSigmaMinus.ParamEnergyLossSigmaPlus.neflowisol20.neflowisol30.neflowisol40.ptvarcone20_TightTTVA_pt500.ptvarcone30_TightTTVA_pt500.ptvarcone40_TightTTVA_pt500.ptvarcone20_TightTTVA_pt1000.ptvarcone30_TightTTVA_pt1000.ptvarcone40_TightTTVA_pt1000.caloExt_Decorated.caloExt_eta.caloExt_phi"

        prompt_lep_vars += ["Muons.%s" %prompt_vars]
        prompt_lep_vars += ["SecVtxContainer_Muons.%s" %secondaryvertex_vars]

    return prompt_lep_vars

#------------------------------------------------------------------------------
def GetExtraPromptTauVariablesForDxAOD():

    prompt_lep_vars = []

    prompt_vars  = "PromptTauIso.PromptTauVeto."
    prompt_vars += "PromptTauInput_TrackJetNTrack.PromptTauInput_rnnip."
    prompt_vars += "PromptTauInput_MV2c10."
    prompt_vars += "PromptTauInput_MV2rmu."
    prompt_vars += "PromptTauInput_JetF.PromptTauInput_SV1."
    prompt_vars += "PromptTauInput_ip2.PromptTauInput_ip3."
    prompt_vars += "PromptTauInput_LepJetPtFrac.PromptTauInput_DRlj."
    
    prompt_lep_vars += ["TauJets.%s" %prompt_vars]  

    return prompt_lep_vars

#------------------------------------------------------------------------------
def DecoratePromptLepton(BDT_name, lepton_name, track_jet_name, addSpectators=False):

    # Check lepton container is correct
    if lepton_name == 'Electrons':
        part_type = 'Electron'
    elif lepton_name == 'Muons':
        part_type = 'Muon'
    else:
        raise Exception('Decorate%s - unknown lepton type: "%s"' %(BDT_name, lepton_name))  

    #
    # Check track jet container is correct
    #
    if track_jet_name != 'AntiKt4PV0TrackJets':
        raise Exception('Decorate%s - unknown track jet collection: "%s"' %(BDT_name, track_jet_name))

    #
    # Prepare DecoratePromptLepton alg
    #
    alg = Conf.Prompt__DecoratePromptLepton('%s_decorate%s' %(lepton_name, BDT_name))

    alg.LeptonContainerName   = lepton_name
    alg.TrackJetContainerName = track_jet_name
    alg.ConfigFileVersion     = 'InputData-2017-10-27/%s/%s' %(part_type, BDT_name)
    alg.MethodTitleMVA        = 'BDT_%s_%s' %(part_type, BDT_name)
    alg.BDTName               = '%s' %BDT_name
    alg.AuxVarPrefix          = 'PromptLeptonInput_'
    alg.PrintTime             = False

    alg.StringIntVars   = getStringIntVars  (BDT_name)
    alg.StringFloatVars = getStringFloatVars(BDT_name)

    if addSpectators :
        alg.StringIntSpecVars   = getStringIntSpecVars  (BDT_name)
        alg.StringFloatSpecVars = getStringFloatSpecVars(BDT_name)

    log.info('Decorate%s - prepared %s algorithm for: %s, %s' %(BDT_name, BDT_name, lepton_name, track_jet_name))

    return alg

#------------------------------------------------------------------------------
def DecoratePromptTau(BDT_name, lepton_name, track_jet_name):

    # Check tau container is correct
    if lepton_name == 'TauJets':
        part_type = 'Tau'
    else:
        raise Exception('Decorate%s - unknown lepton type: "%s"' %(BDT_name, lepton_name))  

    # Check track jet container is correct
    if track_jet_name != 'AntiKt4PV0TrackJets':
        raise Exception('Decorate%s - unknown track jet collection: "%s"' %(BDT_name, track_jet_name))

    # Prepare DecoratePromptLepton alg
    alg = Conf.Prompt__DecoratePromptLepton('%s_decorate%s' %(lepton_name, BDT_name))

    alg.LeptonContainerName         = lepton_name
    alg.TrackJetContainerName       = track_jet_name
    alg.ConfigFileVersionOneTrack   = 'InputData-2018-02-22/%s/%sOneTrack'   %(part_type, BDT_name)
    alg.ConfigFileVersionThreeTrack = 'InputData-2018-02-22/%s/%sThreeTrack' %(part_type, BDT_name)
    alg.MethodTitleMVAOneTrack      = 'BDT_%s_%sOneTrack'   %(part_type, BDT_name)
    alg.MethodTitleMVAThreeTrack    = 'BDT_%s_%sThreeTrack' %(part_type, BDT_name)
    alg.BDTName                     = '%s' %BDT_name
    alg.AuxVarPrefix                = 'PromptTauInput_'
    alg.PrintTime                   = False

    alg.StringIntVars   = getStringIntVars  (BDT_name)
    alg.StringFloatVars = getStringFloatVars(BDT_name)

    log.info('Decorate%s - prepared %s algorithm for: %s, %s' %(BDT_name, BDT_name, lepton_name, track_jet_name))

    return alg

#------------------------------------------------------------------------------
def DecorateNonPromptVertex(lepton_name):

    if lepton_name != 'Electrons' and lepton_name != 'Muons':
        raise Exception('DecorateNonPromptVertex - unknown lepton type: "%s"' %lepton_name)
    
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon.AppMgr import ServiceMgr

    debugAll = False

    if debugAll: outputLevel = 2
    else:        outputLevel = 3

    vtxItrMergingTool = Conf.Prompt__VertexIterativeFitMergingTool('VtxIterFitMergingTool_%s' %lepton_name)

    alg = Conf.Prompt__NonPromptLeptonVertexingAlg('NonPromptVtx_decorate_%s' %(lepton_name))
    alg.PrintTime                         = debugAll
    alg.SelectTracks                      = True
    alg.Debug                             = debugAll
    alg.DebugMerge                        = False
    alg.OutputLevel                       = outputLevel

    alg.LeptonContainerName               = lepton_name
    alg.ReFitPriVtxContainerName          = 'RefittedPriVtx_%s' %lepton_name
    alg.SVContainerName                   = 'SecVtxContainer_%s' %lepton_name

    alg.SecVtxLinksName                   = 'SecVtxLinks'
    alg.NoLeptonPriVtxLinkName            = 'RefittedPriVtxWithoutLepton_%s' %lepton_name
    alg.IndexVectorName                   = 'PromptLeptonInput_SecondaryVertexIndexVector'

    vtxItrMergingTool.minFitProb                  = 0.03
    vtxItrMergingTool.minCandOverSeedFitProbRatio = 0.20
    vtxItrMergingTool.debug                       = False or debugAll
    vtxItrMergingTool.OutputLevel                 = outputLevel

    if debugAll:
        #
        # Example: https://gitlab.cern.ch/atlas/athena/blob/21.2/Reconstruction/RecExample/RecExCommon/share/RecoUtils.py#L220-245
        #
        ServiceMgr.MessageSvc.defaultLimit = 9999999
        ServiceMgr.MessageSvc.Format       = "% F%40W%S%7W%R%T %0W%M" 

        vtxItrMergingTool.outputStream  = 'hists'

        from GaudiSvc.GaudiSvcConf import THistSvc
        ServiceMgr += THistSvc()
        ServiceMgr.THistSvc.Output += ["hists DATAFILE='hists.root' OPT='RECREATE'"]

    alg.VertexMergingTool = vtxItrMergingTool
    alg.VertexFittingSvc  = getVtxFittingService('PromptVertexFittingSvc', ToolSvc, ServiceMgr, debugAll)

    log.info('DecorateNonPromptVertex - prepared decorate second vertexing information algorithm for: %s' %(lepton_name))

    return alg

#------------------------------------------------------------------------------
def DecorateReFitPrimaryVertex(lepton_name):

    if lepton_name != 'Electrons' and lepton_name != 'Muons':
        raise Exception('DecorateReFitPrimaryVertex - unknown lepton type: "%s"' %lepton_name)

    from AthenaCommon.AppMgr import ServiceMgr
    from AthenaCommon.AppMgr import ToolSvc

    alg = Conf.Prompt__PrimaryVertexReFitter('PrimaryVertexReFitter_%s_decorate%s' %(lepton_name, 'PriVtx'))

    alg.LeptonContainerName              = lepton_name
    alg.ReFitPriVtxName                  = 'RefittedPriVtx_%s' %lepton_name

    alg.DistToRefittedPriVtxName         = 'distToRefittedPriVtx'
    alg.NormDistToRefittedPriVtxName     = 'normDistToRefittedPriVtx'
    alg.RefittedVtxLinkName              = 'RefittedPriVtxLink'
    alg.RefittedVtxWithoutLeptonLinkName = 'RefittedPriVtxWithoutLepton_%s' %lepton_name
    alg.PrintTime                        = False
    alg.Debug                            = False

    alg.VertexFittingSvc  = getVtxFittingService('PromptVertexFittingSvc', ToolSvc, ServiceMgr, debugAll=False)

    log.info('DecorateReFitPrimaryVertex - prepared re-fitting primary vertex information algorithm for: %s' %(lepton_name))

    return alg

#------------------------------------------------------------------------------
def getStringIntVars(BDT_name):

    int_vars = []

    if BDT_name == 'PromptLeptonIso':
        int_vars += ['TrackJetNTrack', 
                     'sv1_jf_ntrkv']

    elif BDT_name == 'PromptLeptonVeto':
        int_vars += ['TrackJetNTrack']

    elif BDT_name == 'PromptTauIso':
        int_vars += ['TrackJetNTrack']

    else:
        raise Exception('getStringIntVars - unknown alg: "%s"' %BDT_name)
   
    return int_vars

#------------------------------------------------------------------------------
def getStringFloatVars(BDT_name):

    float_vars = []

    if BDT_name == 'PromptLeptonIso':
        float_vars += ['ip2',
                       'ip3',
                       'LepJetPtFrac',
                       'DRlj',         
                       'TopoEtCone30Rel',
                       'PtVarCone30Rel']

    elif BDT_name == 'PromptLeptonVeto':
        float_vars += ['rnnip',
                       'DL1mu',
                       'PtRel',
                       'PtFrac',
                       'DRlj',
                       'TopoEtCone30Rel',
                       'PtVarCone30Rel']

    elif BDT_name == 'PromptTauIso':
        float_vars += ['MV2c10',                      
                       'JetF',
                       'SV1',
                       'ip2',
                       'ip3',
                       'LepJetPtFrac',
                       'DRlj']

    else:
        raise Exception('getStringFloatVars - unknown alg: "%s"' %BDT_name)
   
    return float_vars

#------------------------------------------------------------------------------
def getStringIntSpecVars(BDT_name):

    int_vars = []
   
    return int_vars

#------------------------------------------------------------------------------
def getStringFloatSpecVars(BDT_name):

    float_vars = []

    if BDT_name == 'PromptLeptonIso':
        float_vars += ['JetPt',
                       'JetEta',
                       'JetPhi',
                       'JetM']

    elif BDT_name == 'PromptLeptonVeto':
        float_vars += ['JetPt',
                       'JetEta',
                       'JetPhi',
                       'JetM']

    elif BDT_name == 'PromptTauIso':
        float_vars += []

    else:
        raise Exception('getStringFloatVars - unknown alg: "%s"' %BDT_name)
   
    return float_vars

#------------------------------------------------------------------------------
def getVtxFittingService(svc_name, ToolSvc, ServiceMgr, debugAll=False):

    if hasattr(ServiceMgr, svc_name):
        return getattr(ServiceMgr, svc_name)

    vertexFittingSvc = Conf.Prompt__VertexFittingSvc(svc_name)

    ServiceMgr += vertexFittingSvc

    if False:
        vertexFittingSvc.vertexFitterTool     = getInDetSequentialVertexFitter(ToolSvc, svc_name)
        vertexFittingSvc.seedVertexFitterTool = getInDetFastVertexFitter      (ToolSvc, svc_name)
        vertexFittingSvc.doSeedVertexFit      = True
    else:
        vertexFittingSvc.vertexFitterTool = getInDetFastVertexFitter(ToolSvc, svc_name)
        vertexFittingSvc.doSeedVertexFit  = False

    vertexFittingSvc.DistToPriVtxName                  = 'distToPriVtx'
    vertexFittingSvc.NormDistToPriVtxName              = 'normDistToPriVtx'
    vertexFittingSvc.DistToRefittedPriVtxName          = 'distToRefittedPriVtx'
    vertexFittingSvc.NormDistToRefittedPriVtxName      = 'normDistToRefittedPriVtx'
    vertexFittingSvc.DistToRefittedRmLepPriVtxName     = 'distToRefittedRmLepPriVtx'
    vertexFittingSvc.NormDistToRefittedRmLepPriVtxName = 'normDistToRefittedRmLepPriVtx'

    if debugAll: outputLevel = 2
    else:        outputLevel = 3

    vertexFittingSvc.debug                        = False
    vertexFittingSvc.vertexFitterTool.OutputLevel = outputLevel
    vertexFittingSvc.OutputLevel                  = outputLevel

    return vertexFittingSvc

#------------------------------------------------------------------------------
def getInDetFastVertexFitter(ToolSvc, key):

    InDetVxFitterName = '%sFastVertexFitterTool' %key

    if hasattr(ToolSvc, InDetVxFitterName):
        raise RuntimeError('getInDetFastVertexFitter - private tool "%s" found in ToolSvc' %InDetVxFitterName)

    from TrkDetDescrSvc           .AtlasTrackingGeometrySvc      import AtlasTrackingGeometrySvc
    from TrkExTools               .TrkExToolsConf                import Trk__Navigator
    from TrkVertexFitterUtils     .TrkVertexFitterUtilsConf      import Trk__FullLinearizedTrackFactory
    from TrkExTools               .TrkExToolsConf                import Trk__MaterialEffectsUpdator
    from TrkExTools               .TrkExToolsConf                import Trk__Extrapolator
    from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator 
    from TrkVertexBilloirTools    .TrkVertexBilloirToolsConf     import Trk__FastVertexFitter

    InDetNavigator = Trk__Navigator(name                = '%sNavigator' %key,
                                    TrackingGeometrySvc = AtlasTrackingGeometrySvc)

    InDetMaterialUpdator = Trk__MaterialEffectsUpdator(name = '%sMaterialEffectsUpdator' %key)

    InDetPropagator = Trk__RungeKuttaPropagator(name = '%sPropagator' %key)

    InDetSubPropagators = [ InDetPropagator.name() ]
    InDetSubUpdators    = [ InDetMaterialUpdator.name() ]

    ToolSvc += InDetNavigator
    ToolSvc += InDetMaterialUpdator
    ToolSvc += InDetPropagator

    InDetExtrapolator = Trk__Extrapolator(name                    = '%sExtrapolator' %key,
                                          Propagators             = [ InDetPropagator ],
                                          MaterialEffectsUpdators = [ InDetMaterialUpdator ],
                                          Navigator               = InDetNavigator,
                                          SubPropagators          = InDetSubPropagators,
                                          SubMEUpdators           = InDetSubUpdators)

    ToolSvc += InDetExtrapolator

    InDetLinFactory = Trk__FullLinearizedTrackFactory(name          = '%sFullLinearizedTrackFactory' %key,
                                                      Extrapolator  = InDetExtrapolator )

    ToolSvc += InDetLinFactory

    InDetVxFitterTool = Trk__FastVertexFitter(name                   = InDetVxFitterName,
                                              LinearizedTrackFactory = InDetLinFactory,
                                              Extrapolator           = InDetExtrapolator)
     
    return InDetVxFitterTool

#------------------------------------------------------------------------------
def getInDetSequentialVertexFitter(ToolSvc, key):

    vxFitterName = '%sSequentialVertexFitterTool' %key

    if hasattr(ToolSvc, vxFitterName):
        raise RuntimeError('getInDetSequentialVertexFitter - private tool "%s" found in ToolSvc' %vxFitterName)

    from TrkDetDescrSvc           .AtlasTrackingGeometrySvc      import AtlasTrackingGeometrySvc
    from TrkExTools               .TrkExToolsConf                import Trk__Navigator
    from TrkExTools               .TrkExToolsConf                import Trk__MaterialEffectsUpdator
    from TrkExTools               .TrkExToolsConf                import Trk__Extrapolator
    from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator 
    from TrkVertexFitterUtils     .TrkVertexFitterUtilsConf      import Trk__FullLinearizedTrackFactory
    from TrkVertexFitters         .TrkVertexFittersConf          import Trk__SequentialVertexFitter
    from TrkVxEdmCnv              .TrkVxEdmCnvConf               import Trk__VxCandidateXAODVertex

    InDetNavigator = Trk__Navigator(name                = '%sDetNavigator' %key,
                                    TrackingGeometrySvc = AtlasTrackingGeometrySvc)

    InDetMaterialUpdator = Trk__MaterialEffectsUpdator(name = '%sMaterialEffectsUpdator' %key)

    InDetPropagator = Trk__RungeKuttaPropagator(name = '%sPropagator' %key)

    InDetSubPropagators = [ InDetPropagator.name() ]
    InDetSubUpdators    = [ InDetMaterialUpdator.name() ]

    ToolSvc += InDetNavigator
    ToolSvc += InDetMaterialUpdator
    ToolSvc += InDetPropagator

    InDetExtrapolator = Trk__Extrapolator(name                    = '%sExtrapolator' %key,
                                          Propagators             = [ InDetPropagator ],
                                          MaterialEffectsUpdators = [ InDetMaterialUpdator ],
                                          Navigator               = InDetNavigator,
                                          SubPropagators          = InDetSubPropagators,
                                          SubMEUpdators           = InDetSubUpdators)

    ToolSvc += InDetExtrapolator

    InDetLinFactory = Trk__FullLinearizedTrackFactory(name         = '%sFullLinearizedTrackFactory' %key,
                                                      Extrapolator = InDetExtrapolator )
    ToolSvc += InDetLinFactory

    InDetVxEdmCnv = Trk__VxCandidateXAODVertex(name='%sVertexInternalEdmFactory' %key)
    ToolSvc += InDetVxEdmCnv

    InDetVxFitterTool = Trk__SequentialVertexFitter(name                   = vxFitterName,
                                                    LinearizedTrackFactory = InDetLinFactory,
                                                    XAODConverter          = InDetVxEdmCnv,
                                                    DoSmoothing            = False)

    InDetVxFitterTool.MaxIterations = 200
    InDetVxFitterTool.maxDeltaChi2  = 0.005
    InDetVxFitterTool.OutputLevel   = 2

    log.info('%s - current number of max iterations: %s' %(vxFitterName, InDetVxFitterTool.MaxIterations))

    return InDetVxFitterTool

