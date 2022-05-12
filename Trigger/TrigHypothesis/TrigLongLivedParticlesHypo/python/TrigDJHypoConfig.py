# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger('TrigDJHypoTools')

def TrigDJExtractVariant(chainDict):
    variants = [i['dispjVariant'] for i in chainDict['chainParts'] if i['signature']=='UnconventionalTracking' and 'dispjVariant' in i]

    if len(variants) != 1:
        raise RuntimeError("Invalid chain dictionary for Displaced Jet Trigger, unable to find variant/multiple variants in "+str(chainDict))

    return variants[0]

def TrigDJHypoPromptToolFromDict( chainDict ):
    """ Use menu decoded chain dictionary to configure the tool """

    name = chainDict['chainName']
    variant = TrigDJExtractVariant(chainDict)
    from AthenaConfiguration.ComponentFactory import CompFactory
    tool = CompFactory.DisplacedJetPromptHypoTool(name)

    tool.min_trk_pt = 1.0
    tool.trk_d0cut = 3.0
    tool.max_z0st = 3.0
    tool.d0sigcut = 10.0
    tool.max_prompt_trk = 2
    tool.max_jet_rank = 2

    if variant == "1jet":
        tool.min_njets = 1
        tool.max_jets = 1
        tool.min_jet_pt = 140
        tool.cut_name = "1jet"
    elif variant == "2jet":
        tool.min_njets = 2
        tool.min_jet_pt = 50
        tool.cut_name = "2jet"
    elif variant == "2jetperf":
        tool.min_njets = 2
        tool.min_jet_pt = 50
        tool.max_prompt_trk = 5
        tool.cut_name = "2jetperf"
    elif variant == "1jetperf":
        tool.min_njets = 1
        tool.min_jet_pt = 140
        tool.max_prompt_trk = 5
        tool.cut_name = "1jetperf"
    else:
        raise RuntimeError("Unknown DJ variant '"+variant+"' chain is '"+name+"'")

    return tool


def TrigDJHypoDispToolFromDict( chainDict ):
    """ Use menu decoded chain dictionary to configure the tool """

    name = chainDict['chainName']
    variant = TrigDJExtractVariant(chainDict)
    from AthenaConfiguration.ComponentFactory import CompFactory
    tool = CompFactory.DisplacedJetDispHypoTool(name)

    tool.min_trk_pt = 1.0
    tool.trk_d0cut = 3.0

    tool.max_z0st = 3.0
    tool.d0sigcut = 10.0

    tool.max_prompt_trk_c2 = 2
    tool.min_disp_trk_c2 = 3
    tool.nother_frac_c2 = 0.75

    tool.max_prompt_trk_c1 = 1
    tool.min_disp_trk_c1 = 0
    tool.nother_frac_c1 = 0.75

    if variant == "1jet":
        tool.max_prompt_trk_c2 = 1
        tool.min_disp_trk_c2 = 3
        tool.nother_frac_c2 = 0.75
        tool.cut_name = "1jet"
    elif variant == "2jet":
        tool.cut_name = "2jet"
    elif variant == "2jetperf":
        tool.max_prompt_trk_c2 = 5
        tool.min_disp_trk_c2 = 0
        tool.nother_frac_c2 = 1.0
        tool.max_prompt_trk_c1 = 5
        tool.min_disp_trk_c1 = 0
        tool.nother_frac_c1 = 1.0
        tool.cut_name = "2jetperf"
    elif variant == "1jetperf":
        tool.max_prompt_trk_c2 = 5
        tool.min_disp_trk_c2 = 0
        tool.nother_frac_c2 = 1.0
        tool.max_prompt_trk_c1 = 5
        tool.min_disp_trk_c1 = 0
        tool.nother_frac_c1 = 1.0
        tool.cut_name = "1jetperf"
    else:
        raise RuntimeError("Unknown DJ variant '"+variant+"' chain is '"+name+"'")

    return tool

def TrigDJHypoEDToolFromDict( chainDict ):
    """ Use menu decoded chain dictionary to configure the tool """

    name = chainDict['chainName']
    variant = TrigDJExtractVariant(chainDict)
    from AthenaConfiguration.ComponentFactory import CompFactory
    tool = CompFactory.DisplacedJetEventDecisionHypoTool(name)


    if variant == "1jet":
        tool.min_h_jets = 1
        tool.min_l_jets = 0
        tool.cut_name = "1jet"
    elif variant == "2jet":
        tool.min_h_jets = 1
        tool.min_l_jets = 1
        tool.cut_name = "2jet"
    elif variant == "2jetperf":
        tool.min_h_jets = 1
        tool.min_l_jets = 1
        tool.cut_name = "2jetperf"
    elif variant == "1jetperf":
        tool.min_h_jets = 1
        tool.min_l_jets = 0
        tool.cut_name = "1jetperf"
    else:
        raise RuntimeError("Unknown DJ variant '"+variant+"' chain is '"+name+"'")

    return tool

