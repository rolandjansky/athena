# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import re
import sys

import LeptonTaggers.LeptonTaggersConf as Conf

from AthenaCommon.Logging import logging

log = logging.getLogger('LeptonTaggersConfig.py')

#------------------------------------------------------------------------------
def ConfigureAntiKt4PV0TrackJets(privateSeq, name):

    from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets

    #
    # Run track jet clustering. B-tagging is run at construction
    #
    replaceAODReducedJets(['AntiKt4PV0TrackJets'], privateSeq, name)

#------------------------------------------------------------------------------
def GetDecoratePromptLeptonAlgs(name="", addSpectators=False):

    algs  = []

    if name == "" or name == 'Electrons':
        algs += [DecoratePromptLepton('PromptLeptonVeto', 'Electrons', 'AntiKt4PV0TrackJets', addSpectators)]
        algs += [DecorateLowPtPromptLepton('LowPtPromptLeptonVeto', 'Electrons', 'AntiKt4PV0TrackJets', addSpectators)]

    if name == "" or name == 'Muons':
        algs += [DecoratePromptLepton('PromptLeptonVeto', 'Muons', 'AntiKt4PV0TrackJets', addSpectators)]
        algs += [DecorateLowPtPromptLepton('LowPtPromptLeptonVeto', 'Muons', 'AntiKt4PV0TrackJets', addSpectators)]    

    return algs

#------------------------------------------------------------------------------
def GetDecoratePromptLeptonIsoAlgs(name="", addSpectators=False):

    algs  = []

    if name == "" or name == 'Electrons':
        algs += [DecoratePromptLepton('PromptLeptonIso',  'Electrons', 'AntiKt4PV0TrackJets', addSpectators)]

    if name == "" or name == 'Muons':
        algs += [DecoratePromptLepton('PromptLeptonIso',  'Muons', 'AntiKt4PV0TrackJets', addSpectators)]

    return algs

#------------------------------------------------------------------------------
def GetDecorateImprovedPromptLeptonAlgs(name="", addSpectators=False):

    algs  = []

    if name == "" or name == 'Electrons':
        algs += [DecorateReFitPrimaryVertex  ('Electrons')]
        algs += [DecorateNonPromptVertex     ('Electrons')]
        algs += [DecoratePromptLeptonRNN     ('PromptLeptonRNN',              'Electrons')]
        algs += [DecoratePromptLeptonImproved('PromptLeptonImprovedVetoBARR', 'Electrons', 'AntiKt4PV0TrackJets')]
        algs += [DecoratePromptLeptonImproved('PromptLeptonImprovedVetoECAP', 'Electrons', 'AntiKt4PV0TrackJets')]

    if name == "" or name == 'Muons':
        algs += [DecorateReFitPrimaryVertex  ('Muons')]
        algs += [DecorateNonPromptVertex     ('Muons')]
        algs += [DecoratePromptLeptonRNN     ('PromptLeptonRNN',          'Muons')]
        algs += [DecoratePromptLeptonImproved('PromptLeptonImprovedVeto', 'Muons', 'AntiKt4PV0TrackJets')]
    
    return algs

#------------------------------------------------------------------------------
def GetDecoratePromptTauAlgs():

    algs  = []

    algs += [DecoratePromptTau('PromptTauIso',  'TauJets', 'AntiKt4PV0TrackJets')]
    
    log.info('GetDecoratePromptTauAlgs - Due to the input variable has changed (missing MV2rmu), PromptTauVeto is not supported')

    return algs

#------------------------------------------------------------------------------
def GetExtraPromptVariablesForDxAOD(name='', addSpectators=False, onlyBDT=True):

    prompt_lep_vars = []

    #
    # Decorate lepton only with the BDT outputs when the onlyBDT flag is true.
    #
    # NOTE: The output score name for BDTname=LowPtPromptLeptonVeto is "LowPtPLV" instead "LowPtPromptLeptonVeto".
    #       This is to harmonize with the variable augmented in CP::IsolationLowPtPLVTool
    #
    if onlyBDT:
        if name == "" or name == "Electrons":
            prompt_lep_vars += ["Electrons.PromptLeptonVeto.PromptLeptonIso.LowPtPLV."]

        if name == "" or name == "Muons":
            prompt_lep_vars += ["Muons.PromptLeptonVeto.PromptLeptonIso.LowPtPLV."]

        return prompt_lep_vars
 
 
    prompt_vars  = "PromptLeptonVeto.PromptLeptonIso.LowPtPLV."
    prompt_vars += "PromptLeptonInput_TrackJetNTrack.PromptLeptonInput_sv1_jf_ntrkv."
    prompt_vars += "PromptLeptonInput_ip2.PromptLeptonInput_ip3."
    prompt_vars += "PromptLeptonInput_LepJetPtFrac.PromptLeptonInput_DRlj."
    prompt_vars += "PromptLeptonInput_PtFrac.PromptLeptonInput_PtRel."
    prompt_vars += "PromptLeptonInput_DL1mu.PromptLeptonInput_rnnip."
    prompt_vars += "PromptLeptonInput_TopoEtCone20Rel.PromptLeptonInput_PtVarCone20Rel."
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
def GetExtraImprovedPromptVariablesForDxAOD(name='', onlyBDT=False):

    prompt_lep_vars = []

    #
    # Decorate lepton only with the BDT outputs when the onlyBDT flag is true.
    #
    if onlyBDT:
        # Add lepton raw pT and pTBin as default which is needed for the PLIV working points.
        rawpt_vars ="PromptLeptonImprovedInput_MVAXBin.PromptLeptonImprovedInput_RawPt"

        if name == "" or name == "Electrons":
            prompt_lep_vars += ["Electrons.PromptLeptonImprovedVetoBARR.PromptLeptonImprovedVetoECAP.%s"%rawpt_vars]

        if name == "" or name == "Muons":
            prompt_lep_vars += ["Muons.PromptLeptonImprovedVeto.%s"%rawpt_vars]

        return prompt_lep_vars

    prompt_vars  = "PromptLeptonImprovedInput_MVAXBin.PromptLeptonImprovedInput_RawPt."
    prompt_vars += "PromptLeptonImprovedInput_PtFrac.PromptLeptonImprovedInput_DRlj."
    prompt_vars += "PromptLeptonImprovedInput_topoetcone30rel.PromptLeptonImprovedInput_ptvarcone30rel."

    if name == "" or name == "Electrons":
        # Add PromptLeptonTagger electron RNN and new inputs for PromptLeptonImprovedVetoBARR/PromptLeptonImprovedVetoECAP
        prompt_vars += "PromptLeptonRNN_prompt.PromptLeptonRNN_non_prompt_b.PromptLeptonRNN_non_prompt_c.PromptLeptonRNN_conversion."
        prompt_vars += "PromptLeptonImprovedVetoBARR.PromptLeptonImprovedVetoECAP.PromptLeptonImprovedInput_TrackJetNTrack.PromptLeptonImprovedInput_PtRel.PromptLeptonImprovedInput_CaloClusterSumEtRel.PromptLeptonImprovedInput_CandVertex_normDistToPriVtxLongitudinalBest_ThetaCutVtx."

        prompt_lep_vars += ["Electrons.%s" %prompt_vars]

    if name == "" or name == "Muons":
        # Add PromptLeptonTagger muon RNN and new inputs for PromptLeptonImprovedVeto
        prompt_vars += "PromptLeptonRNN_prompt.PromptLeptonRNN_non_prompt_b.PromptLeptonRNN_non_prompt_c."
        prompt_vars += "PromptLeptonImprovedVeto.PromptLeptonImprovedInput_ptvarcone30_TightTTVA_pt500rel.PromptLeptonImprovedInput_CaloClusterERel.PromptLeptonImprovedInput_CandVertex_normDistToPriVtxLongitudinalBest."

        prompt_lep_vars += ["Muons.%s" %prompt_vars]

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
def DecorateLowPtPromptLepton(BDT_name, lepton_name, track_jet_name, addSpectators=False):

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
    alg.ConfigFileVersion     = 'InputData-2019-11-09/%s/%s' %(part_type, BDT_name)
    alg.MethodTitleMVA        = 'BDT_%s_%s' %(part_type, BDT_name)
    alg.BDTName               = '%s' %BDT_name
    alg.AuxVarPrefix          = 'PromptLeptonInput_'
    alg.PrintTime             = False

    alg.StringIntVars   = getStringIntVars  (BDT_name)
    alg.StringFloatVars = getStringFloatVars(BDT_name,part_type)

    if addSpectators :
        alg.StringIntSpecVars   = getStringIntSpecVars  (BDT_name)
        alg.StringFloatSpecVars = getStringFloatSpecVars(BDT_name)

    log.info('Decorate%s - prepared %s algorithm for: %s, %s' %(BDT_name, BDT_name, lepton_name, track_jet_name))

    return alg

#------------------------------------------------------------------------------
def DecoratePromptLeptonImproved(BDT_name, lepton_name, track_jet_name):

    #
    # Check track jet container is correct
    #
    if track_jet_name != 'AntiKt4PV0TrackJets':
        raise Exception('Decorate%s - unknown track jet collection: "%s"' %(BDT_name, track_jet_name))

    #
    # Prepare DecoratePromptLepton alg
    #
    alg = Conf.Prompt__DecoratePromptLeptonImproved('%s_decorate%s' %(lepton_name, BDT_name))

    alg.LeptonContainerName        = lepton_name
    alg.TrackJetContainerName      = track_jet_name
    alg.PrimaryVertexContainerName = 'PrimaryVertices'
    alg.ClusterContainerName       = 'CaloCalTopoClusters'

    alg.ConfigFileVersion          = '' 
    alg.BDTName                    = BDT_name
    alg.InputVarDecoratePrefix     = 'PromptLeptonImprovedInput_'
    alg.PrintTime                  = False
    alg.OutputLevel                = 3

    #
    # Read configuration from AFS for this initial merge request, will switch to cvmfs with second request 
    #
    if lepton_name == 'Electrons':
        alg.MethodTitleMVA     = 'BDT_Electron_%s' %(BDT_name)
        alg.ConfigFileVersion  = 'InputData-2020-02-25/BDT/Electron/%s' %(BDT_name)
        alg.accessorRNNVars    = ['PromptLeptonRNN_prompt']
    elif lepton_name == 'Muons': 
        alg.MethodTitleMVA     = 'BDT_Muon_%s' %(BDT_name)
        alg.ConfigFileVersion  = 'InputData-2020-02-25/BDT/Muon/%s' %(BDT_name)
        alg.accessorRNNVars    = ['PromptLeptonRNN_prompt']
    else:
        raise Exception('Decorate%s - unknown lepton type: "%s"' %(BDT_name, lepton_name))  

    alg.stringIntVars            = getStringIntVars  (BDT_name)
    alg.stringFloatVars          = getStringFloatVars(BDT_name)
    alg.extraDecoratorFloatVars  = ['RawPt']
    alg.extraDecoratorShortVars  = ['CandVertex_NPassVtx']
    alg.vetoDecoratorFloatVars   = ['PromptLeptonRNN_prompt']
    alg.vetoDecoratorShortVars   = []

    alg.leptonPtBinsVector = [10.0e3, 15.0e3, 20.0e3, 25.0e3, 32.0e3, 43.0e3, 100.0e3]

    # 
    # Secondary vertex selection for the PromptLeptonImproved
    #
    alg.VertexLinkName          = 'DeepMergedSecVtxLinks'
    alg.VertexMinChiSquaredProb = 0.03

    #
    # Cut especially for supressing material interaction vertices of the electron
    #
    alg.VertexMinThetaBarrElec  = 0.002
    alg.VertexMinThetaEcapElec  = 0.001
    alg.VertexBarrEcapAbsEtaAt  = 1.37

    log.info('Decorate%s - prepared %s algorithm for: %s, %s' %(BDT_name, BDT_name, lepton_name, track_jet_name))

    return alg

#------------------------------------------------------------------------------
def DecoratePromptLeptonRNN(RNN_name, lepton_name):

    #
    # Prepare DecoratePromptLepton alg
    #
    alg = Conf.Prompt__DecoratePromptLeptonRNN('%s_decorate_RNN_%s' %(lepton_name, RNN_name))
    alg.inputContainerLepton          = lepton_name
    alg.inputContainerTrack           = 'InDetTrackParticles'
    alg.inputContainerTrackJet        = 'AntiKt4PV0TrackJets'
    alg.inputContainerPrimaryVertices = 'PrimaryVertices'
    
    alg.decorationPrefixRNN           = 'PromptLeptonRNN_'

    alg.debug                         = False
    alg.outputStream                  = 'out'

    alg.toolRNN = Conf.Prompt__RNNTool('%s_%s_RNNTool' %(RNN_name, lepton_name))

    #
    # Read configuration from AFS for this initial merge request, will switch to cvmfs with second request 
    #
    if lepton_name == 'Electrons':
        alg.toolRNN.configRNNVersion  = 'InputData-2020-02-25/RNN/Electron'
        alg.toolRNN.configRNNJsonFile = 'elecs_feb20_fullrun2_linear_ptraw_ntk5_model_ndense10_nhidden50_nepoch10_nbatch256_use_weights_nn-config.json'

    elif lepton_name == 'Muons':
        alg.toolRNN.configRNNVersion  = 'InputData-2020-02-25/RNN/Muon'
        alg.toolRNN.configRNNJsonFile = 'muons_feb19_fullrun2_linear_ptraw_ntk5_model_ndense10_nhidden50_nepoch10_nbatch256_use_weights_nn-config.json'
    else:
        raise Exception('DecorateNonPromptVertex - unknown lepton type: "%s"' %lepton_name)

    log.info('Decorate%s - prepared %s algorithm for: %s' %(RNN_name, RNN_name, lepton_name))

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
    alg.DeepMergedSecVtxLinksName         = 'DeepMergedSecVtxLinks'
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

    elif BDT_name == 'LowPtPromptLeptonVeto':
        int_vars += ['TrackJetNTrack']

    elif BDT_name == 'PromptTauIso':
        int_vars += ['TrackJetNTrack']

    elif BDT_name == 'PromptLeptonImprovedVeto':
        int_vars += ['MVAXBin']

    elif BDT_name == 'PromptLeptonImprovedVetoBARR' or \
         BDT_name == 'PromptLeptonImprovedVetoECAP':
        int_vars += ['MVAXBin',
                     'TrackJetNTrack']
    
    else:
        raise Exception('getStringIntVars - unknown alg: "%s"' %BDT_name)
   
    return int_vars

#------------------------------------------------------------------------------
def getStringFloatVars(BDT_name, part_type=''):

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

    elif BDT_name == 'LowPtPromptLeptonVeto':
        float_vars += ['PtRel',
                       'PtFrac',
                       'DRlj',                       
                       'TopoEtCone20Rel']
        if part_type == "Electron" : 
            float_vars += ['PtVarCone20Rel']
        else :
            float_vars += ['PtVarCone30Rel']

    elif BDT_name == 'PromptTauIso':
        float_vars += ['MV2c10',                      
                       'JetF',
                       'SV1',
                       'ip2',
                       'ip3',
                       'LepJetPtFrac',
                       'DRlj']

    elif BDT_name == 'PromptLeptonImprovedVeto':
        float_vars += ['topoetcone30rel',
                       'ptvarcone30_TightTTVA_pt500rel',
                       'PromptLeptonRNN_prompt',
                       'PtFrac',
                       'DRlj',
                       'CaloClusterERel',
                       'CandVertex_normDistToPriVtxLongitudinalBest']

    elif BDT_name == 'PromptLeptonImprovedVetoBARR' or \
         BDT_name == 'PromptLeptonImprovedVetoECAP':
        float_vars += ['topoetcone30rel',
                       'ptvarcone30rel',
                       'PromptLeptonRNN_prompt',
                       'PtFrac',
                       'DRlj',
                       'CaloClusterSumEtRel',
                       'PtRel',
                       'CandVertex_normDistToPriVtxLongitudinalBest_ThetaCutVtx']

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

    elif BDT_name == 'PromptLeptonVeto' or BDT_name == 'LowPtPromptLeptonVeto':
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
    InDetVxFitterTool.OutputLevel   = 3

    log.info('%s - current number of max iterations: %s' %(vxFitterName, InDetVxFitterTool.MaxIterations))

    return InDetVxFitterTool

