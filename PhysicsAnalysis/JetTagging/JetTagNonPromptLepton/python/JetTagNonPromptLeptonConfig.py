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
    ReTag(['MV2c10rnn', 'DL1mu', 'RNNIP'], ['AntiKt4PV0TrackJets'], privateSeq)

#------------------------------------------------------------------------------
def GetDecoratePromptLeptonAlgs():

    algs  = []

    algs += [DecoratePromptLepton("PromptLeptonIso",  "Electrons", "AntiKt4PV0TrackJets")]
    algs += [DecoratePromptLepton("PromptLeptonVeto", "Electrons", "AntiKt4PV0TrackJets")]

    algs += [DecoratePromptLepton("PromptLeptonIso",  "Muons", "AntiKt4PV0TrackJets")]
    algs += [DecoratePromptLepton("PromptLeptonVeto", "Muons", "AntiKt4PV0TrackJets")]

    return algs

#------------------------------------------------------------------------------
def GetDecoratePromptTauAlgs():

    algs  = []

    algs += [DecoratePromptTau("PromptTauVeto", "TauJets", "AntiKt4PV0TrackJets")]

    return algs

#------------------------------------------------------------------------------
def GetExtraPromptVariablesForDxAOD():

    prompt_lep_vars = []

    prompt_vars  = "PromptLeptonVeto.PromptLeptonIso."
    prompt_vars += "PromptLeptonInput_TrackJetNTrack.PromptLeptonInput_sv1_jf_ntrkv."
    prompt_vars += "PromptLeptonInput_ip2.PromptLeptonInput_ip3."
    prompt_vars += "PromptLeptonInput_LepJetPtFrac.PromptLeptonInput_DRlj."
    prompt_vars += "PromptLeptonInput_PtFrac.PromptLeptonInput_PtRel."
    prompt_vars += "PromptLeptonInput_DL1mu.PromptLeptonInput_rnnip."
    
    prompt_lep_vars += ["Electrons.%s" %prompt_vars]
    prompt_lep_vars += ["Muons.%s"     %prompt_vars] 

    return prompt_lep_vars

#------------------------------------------------------------------------------
def GetExtraPromptTauVariablesForDxAOD():

    prompt_lep_vars = []

    prompt_vars  = "PromptTauVeto."
    prompt_vars += "PromptLeptonInput_TrackJetNTrack.PromptLeptonInput_rnnip."
    prompt_vars += "PromptLeptonInput_MV2c10rnn."
    prompt_vars += "PromptLeptonInput_JetF.PromptLeptonInput_SV1."
    prompt_vars += "PromptLeptonInput_ip2.PromptLeptonInput_ip3."
    prompt_vars += "PromptLeptonInput_LepJetPtFrac.PromptLeptonInput_DRlj."
    
    prompt_lep_vars += ["TauJets.%s" %prompt_vars]  

    return prompt_lep_vars

#------------------------------------------------------------------------------
def DecoratePromptLepton(BDT_name, lepton_name, track_jet_name):

    # Check lepton container is correct
    if lepton_name == 'Electrons':
        part_type = 'Electron'
    elif lepton_name == 'Muons':
        part_type = 'Muon'
    else:
        raise Exception('Decorate%s - unknown lepton type: "%s"' %(BDT_name, lepton_name))  

    # Check track jet container is correct
    if track_jet_name != 'AntiKt4PV0TrackJets':
        raise Exception('Decorate%s - unknown track-jet collection: "%s"' %(BDT_name, track_jet_name))

    # Prepare DecoratePromptLepton alg
    alg = Conf.Prompt__DecoratePromptLepton('%s_decorate%s' %(lepton_name, BDT_name))

    alg.LeptonContainerName   = lepton_name
    alg.TrackJetContainerName = track_jet_name
    alg.ConfigFileVersion     = 'InputData-2017-10-24/%s/%s' %(part_type, BDT_name)
    alg.MethodTitleMVA        = 'BDT_%s_%s' %(part_type, BDT_name)
    alg.BDTName               = '%s' %BDT_name
    alg.AuxVarPrefix          = 'PromptLeptonInput_'
    alg.PrintTime             = False

    alg.StringIntVars   = getStringIntVars  (BDT_name)
    alg.StringFloatVars = getStringFloatVars(BDT_name)

    log.info('Decorate%s - prepared %s algorithm for: %s, %s' %(BDT_name, BDT_name, lepton_name, track_jet_name))

    print alg

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
        raise Exception('Decorate%s - unknown track-jet collection: "%s"' %(BDT_name, track_jet_name))

    # Prepare DecoratePromptLepton alg
    alg = Conf.Prompt__DecoratePromptLepton('%s_decorate%s' %(lepton_name, BDT_name))

    alg.LeptonContainerName         = lepton_name
    alg.TrackJetContainerName       = track_jet_name
    alg.ConfigFileVersionOneTrack   = 'InputData-2017-10-24/%s/%sOneTrack'   %(part_type, BDT_name)
    alg.ConfigFileVersionThreeTrack = 'InputData-2017-10-24/%s/%sThreeTrack' %(part_type, BDT_name)
    alg.MethodTitleMVAOneTrack      = 'BDT_%s_%sOneTrack'   %(part_type, BDT_name)
    alg.MethodTitleMVAThreeTrack    = 'BDT_%s_%sThreeTrack' %(part_type, BDT_name)
    alg.BDTName                     = '%s' %BDT_name
    alg.AuxVarPrefix                = 'PromptLeptonInput_'
    alg.PrintTime                   = False

    alg.StringIntVars   = getStringIntVars  (BDT_name)
    alg.StringFloatVars = getStringFloatVars(BDT_name)

    log.info('Decorate%s - prepared %s algorithm for: %s, %s' %(BDT_name, BDT_name, lepton_name, track_jet_name))

    print alg

    return alg

#------------------------------------------------------------------------------
def DecoratePromptLeptonIso(lepton_name, track_jet_name):

    BDT_name = 'PromptLeptonIso'

    if lepton_name == 'Electrons':
        part_type = 'Electron'
    elif lepton_name == 'Muons':
        part_type = 'Muon'
    else:
        raise Exception('DecoratePromptLeptonIso - unknown lepton type: "%s"' %lepton_name)

    if track_jet_name != 'AntiKt4PV0TrackJets':
        raise Exception('DecoratePromptLeptonIso - unknown track-jet collection: "%s"' %track_jet_name)

    

    alg = Conf.Prompt__DecoratePromptLepton('%s_decorate%s' %(lepton_name, BDT_name))

    alg.LeptonContainerName   = lepton_name
    alg.TrackJetContainerName = track_jet_name
    alg.ConfigFileVersion     = 'InputData-2017-10-24/%s/%s' %(part_type, BDT_name)
    alg.MethodTitleMVA        = 'BDT_%s_%s' %(part_type, BDT_name)
    alg.AuxVarPrefix          = 'PromptLeptonInput_'
    alg.PrintTime             = False

    alg.StringIntVars         = ['TrackJetNTrack',
                                 'sv1_jf_ntrkv']
    alg.StringFloatVars       = ['ip2',
                                 'ip3',
                                 'LepJetPtFrac',
                                 'DRlj',         
                                 'TopoEtCone30Rel',
                                 'PtVarCone30Rel']

    log.info('Decorate%s - prepared %s algorithm for: %s, %s' %(BDT_name, BDT_name, lepton_name, track_jet_name))
    print alg

    return alg

#------------------------------------------------------------------------------
def getStringIntVars(BDT_name):

    int_vars = []

    if BDT_name == "PromptLeptonIso":
        int_vars += ['TrackJetNTrack', 
                     'sv1_jf_ntrkv']

    elif BDT_name == "PromptLeptonVeto":
        int_vars += ['TrackJetNTrack']

    else:
        raise Exception('getStringIntVars - unknown alg: "%s"' %BDT_name)
   
    return int_vars

#------------------------------------------------------------------------------
def getStringFloatVars(BDT_name):

    float_vars = []

    if BDT_name == "PromptLeptonIso":
        float_vars += ['ip2',
                       'ip3',
                       'LepJetPtFrac',
                       'DRlj',         
                       'TopoEtCone30Rel',
                       'PtVarCone30Rel']

    elif BDT_name == "PromptLeptonVeto":
        float_vars += ['rnnip',
                       'DL1mu',
                       'PtRel',
                       'PtFrac',
                       'DRlj',
                       'TopoEtCone30Rel',
                       'PtVarCone30Rel']

    else:
        raise Exception('getStringIntVars - unknown alg: "%s"' %BDT_name)
   
    return float_vars
