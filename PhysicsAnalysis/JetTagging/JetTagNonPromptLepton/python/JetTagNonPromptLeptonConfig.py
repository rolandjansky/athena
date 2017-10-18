# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import re
import sys

import JetTagNonPromptLepton.JetTagNonPromptLeptonConf as Conf

from AthenaCommon.Logging import logging

log = logging.getLogger('JetTagNonPromptLeptonConfig.py')

#------------------------------------------------------------------------------
def ConfigureAntiKt4PV0TrackJets(privateSeq, name):

    from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
    from DerivationFrameworkFlavourTag.FlavourTagCommon import ReTag

    # Run track jet clustering 
    replaceAODReducedJets(["AntiKt4PV0TrackJets"], privateSeq, name)

    # B-tag algs to be run on new track jet collection
    ReTag(['DL1mu', 'RNNIP'], ['AntiKt4PV0TrackJets'], privateSeq)

#------------------------------------------------------------------------------
def GetDecoratePromptLeptonAlgs():

    algs  = []

    algs += [DecoratePromptLeptonIso  ("Electrons", "AntiKt4PV0TrackJets")]
    algs += [DecoratePromptLeptonNoIso("Electrons", "AntiKt4PV0TrackJets")]
    algs += [DecoratePromptLepton     ("Electrons", "AntiKt4PV0TrackJets")]

    algs += [DecoratePromptLeptonIso  ("Muons", "AntiKt4PV0TrackJets")]
    algs += [DecoratePromptLeptonNoIso("Muons", "AntiKt4PV0TrackJets")]
    algs += [DecoratePromptLepton     ("Muons", "AntiKt4PV0TrackJets")]

    return algs

#------------------------------------------------------------------------------
def GetExtraPromptVariablesForDxAOD():

    prompt_lep_vars = []

    prompt_vars  = ".PromptLeptonIso_TagWeight.PromptLeptonIso_TrackJetNTrack."
    prompt_vars += "PromptLeptonIso_sv1_jf_ntrkv."
    prompt_vars += "PromptLeptonIso_ip2.PromptLeptonIso_ip3."
    prompt_vars += "PromptLeptonIso_LepJetPtFrac.PromptLeptonIso_DRlj."
    prompt_vars += "PromptLepton_TagWeight."
    prompt_vars += "PromptLeptonNoIso_TagWeight"
    
    prompt_lep_vars += ["Electrons.%s" %prompt_vars]
    prompt_lep_vars += ["Muons.%s" %prompt_vars]    

    return prompt_lep_vars

#------------------------------------------------------------------------------
def GetPromptVariablesForDxAOD():

    prompt_lep_vars = []

    prompt_vars = "PromptLeptonVeto.PromptLeptonInput_TrackJetNTrack."
    
    prompt_lep_vars += ["Electrons.%s" %prompt_vars]
    prompt_lep_vars += ["Muons.%s" %prompt_vars]    

    return prompt_lep_vars

#------------------------------------------------------------------------------
def DecoratePromptLeptonVeto(lepton_name, track_jet_name):

    if lepton_name == 'Electrons':
        part_type = 'Electron'
    elif lepton_name == 'Muons':
        part_type = 'Muon'
    else:
        raise Exception('DecoratePromptLeptonVeto - unknown lepton type: "%s"' %lepton_name)

    BDT_name = 'PromptLeptonVeto'

    if track_jet_name != 'AntiKt4PV0TrackJets':
        raise Exception('Decorate%s - unknown track-jet collection: "%s"' %(BDT_name, track_jet_name))

    alg = Conf.Prompt__DecoratePromptLepton('%s_decorate%s' %(lepton_name, BDT_name))

    alg.LeptonContainerName   = lepton_name
    alg.TrackJetContainerName = track_jet_name
    #alg.ConfigFileVersion     = 'InputData-2016-11-02/%s/%s' %(part_type, BDT_name)
    alg.ConfigPathOverride    = '%s/%s' %(part_type, BDT_name)
    alg.MethodTitleMVA        = 'BDT_%s_%s' %(part_type, BDT_name)
    alg.BDTName               = '%s' %BDT_name
    alg.AuxVarPrefix          = 'PromptLeptonInput_'
    alg.PrintTime             = False

    alg.StringIntVars         = ['TrackJetNTrack']
    alg.StringFloatVars       = ['rnnip',
                                 'DL1mu',
                                 'PtRel',
                                 'PtFrac',
                                 'DRlj',
                                 'TopoEtCone30Rel',
                                 'PtVarCone30Rel']

    log.info('Decorate%s - prepared %s algorithm for: %s, %s' %(BDT_name, BDT_name, lepton_name, track_jet_name))
    print alg

    return alg

#------------------------------------------------------------------------------
def DecoratePromptLeptonNoIso(lepton_name, track_jet_name):

    if lepton_name == 'Electrons':
        part_type = 'Electron'
    elif lepton_name == 'Muons':
        part_type = 'Muon'
    else:
        raise Exception('DecoratePromptLeptonNoIso - unknown lepton type: "%s"' %lepton_name)

    if track_jet_name != 'AntiKt4PV0TrackJets':
        raise Exception('DecoratePromptLeptonNoIso - unknown track-jet collection: "%s"' %track_jet_name)

    alg = Conf.Prompt__DecoratePromptLepton('%s_decoratePromptLeptonNoIso' %lepton_name)

    alg.LeptonContainerName   = lepton_name
    alg.TrackJetContainerName = track_jet_name
    alg.ConfigFileVersion     = 'InputData-2016-11-02/%s/PromptLepton' %part_type
    alg.MethodTitleMVA        = 'BDT_%s_PromptLepton' %part_type
    alg.AuxVarPrefix          = 'PromptLeptonNoIso_'
    alg.PrintTime             = False

    alg.StringIntVars         = ['TrackJetNTrack',
                                 'sv1_jf_ntrkv']
    alg.StringFloatVars       = ['ip2',
                                 'ip3',
                                 'LepJetPtFrac',
                                 'DRlj']

    log.info('DecoratePromptLeptonNoIso - prepared PromptLepton algorithm for: %s, %s' %(lepton_name, track_jet_name))
    print alg

    return alg

#------------------------------------------------------------------------------
def DecoratePromptLeptonIso(lepton_name, track_jet_name):

    if lepton_name == 'Electrons':
        part_type = 'Electron'
    elif lepton_name == 'Muons':
        part_type = 'Muon'
    else:
        raise Exception('DecoratePromptLeptonIso - unknown lepton type: "%s"' %lepton_name)

    if track_jet_name != 'AntiKt4PV0TrackJets':
        raise Exception('DecoratePromptLeptonIso - unknown track-jet collection: "%s"' %track_jet_name)

    alg = Conf.Prompt__DecoratePromptLepton('%s_decoratePromptLeptonIso' %lepton_name)

    alg.LeptonContainerName   = lepton_name
    alg.TrackJetContainerName = track_jet_name
    alg.ConfigFileVersion     = 'InputData-2016-11-02/%s/PromptLeptonIso' %part_type
    alg.MethodTitleMVA        = 'BDT_%s_PromptLeptonIso' %part_type
    alg.AuxVarPrefix          = 'PromptLeptonIso_'
    alg.PrintTime             = False

    alg.StringIntVars         = ['TrackJetNTrack',
                                 'sv1_jf_ntrkv']
    alg.StringFloatVars       = ['ip2',
                                 'ip3',
                                 'LepJetPtFrac',
                                 'DRlj',         
                                 'EtTopoCone30Rel',
                                 'PtVarCone30Rel']

    log.info('DecoratePromptLeptonIso - prepared PromptLeptonIso algorithm for: %s, %s' %(lepton_name, track_jet_name))
    print alg

    return alg

#------------------------------------------------------------------------------
def DecoratePromptLepton(lepton_name, track_jet_name):

    if lepton_name == 'Electrons':
        part_type = 'Electron'
    elif lepton_name == 'Muons':
        part_type = 'Muon'
    else:
        raise Exception('DecoratePromptLepton - unknown lepton type: "%s"' %lepton_name)

    if track_jet_name != 'AntiKt4PV0TrackJets':
        raise Exception('DecoratePromptLepton - unknown track-jet collection: "%s"' %track_jet_name)

    alg = Conf.Prompt__DecoratePromptLepton('%s_decoratePromptLepton' %lepton_name)
    alg.LeptonContainerName   = lepton_name
    alg.TrackJetContainerName = track_jet_name
    alg.ConfigFileVersion     = 'InputData-2016-05-22/%s/BTagBDTEtIso' %part_type
    alg.MethodTitleMVA        = 'BDT_%s_BTagBDTEtIso' %part_type
    alg.AuxVarPrefix          = 'PromptLepton_'
    alg.PrintTime             = False

    alg.StringIntVars         = ['TrackJetNTrack',
                                 'sv1_ntkv',
                                 'jf_ntrkv']
    alg.StringFloatVars       = ['ip2',
                                 'ip2_cu',
                                 'ip3',
                                 'ip3_cu',
                                 'EtTopoCone20Rel']

    log.info('DecoratePromptLepton - prepared algorithm for: %s, %s' %(lepton_name, track_jet_name))
    print alg

    return alg

#------------------------------------------------------------------------------
def DecoratePromptLepton_Version0512(lepton_name, track_jet_name):

    alg = Conf.Prompt__DecoratePromptLepton('%s_decoratePromptLepton' %lepton_name)

    if lepton_name == 'Electrons':
        part_type = 'Electron'
    elif lepton_name == 'Muons':
        part_type = 'Muon'
    else:
        raise Exception('DecoratePromptLepton - unknown lepton type: "%s"' %lepton_name)

    if track_jet_name != 'AntiKt4PV0TrackJets':
        raise Exception('DecoratePromptLepton - unknown track-jet collection: "%s"' %track_jet_name)

    alg.LeptonContainerName   = lepton_name
    alg.TrackJetContainerName = track_jet_name
    alg.ConfigFileVersion     = 'InputData-2016-05-12/%s' %part_type
    alg.AuxVarPrefix          = 'PromptLepton_'
    alg.PrintTime             = False

    alg.StringIntVars         = ['TrackJetNTrack',
                                 'sv1_ntkv',
                                 'jf_ntrkv']
    alg.StringFloatVars       = ['ip2',
                                 'ip2_cu',
                                 'ip3',
                                 'ip3_cu',
                                 'EtTopoCone20Rel']    

    return alg
