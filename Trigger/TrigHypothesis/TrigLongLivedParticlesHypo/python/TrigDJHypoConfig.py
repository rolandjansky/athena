# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger('TrigDJHypoTools')


def TrigDJExtractThreshold(chainDict):
    threshold = [i['threshold'] for i in chainDict['chainParts'] if i['signature'] == 'UnconventionalTracking' and 'dispjet' in i['chainPartName']]
    
    if len(threshold) != 1:
        raise RuntimeError("Cannot support mutiple occurrences of dispjet in chain {}".format(chainDict['chainName']))

    return int(threshold[0])


def TrigDJExtractMultiplicity(chainDict):
    multiplicity = [i['multiplicity'] for i in chainDict['chainParts'] if i['signature'] == 'UnconventionalTracking' and 'dispjet' in i['chainPartName']]
    
    if len(multiplicity) != 1:
        raise RuntimeError("Cannot support mutiple occurrences of dispjet in chain {}".format(chainDict['chainName']))

    return int(multiplicity[0])


def TrigDJHypoPromptToolFromDict( chainDict ):
    """ Use menu decoded chain dictionary to configure the tool """

    name = chainDict['chainName']
    threshold = TrigDJExtractThreshold(chainDict)
    multiplicity = TrigDJExtractMultiplicity(chainDict)
    addInfo = chainDict['chainParts'][0]['addInfo']

    from AthenaConfiguration.ComponentFactory import CompFactory
    tool = CompFactory.DisplacedJetPromptHypoTool(name)

    tool.min_trk_pt = 1.0
    tool.trk_d0cut = 3.0
    tool.max_z0st = 3.0
    tool.d0sigcut = 10.0
    tool.max_prompt_trk = 2
    tool.max_jet_rank = 2
    tool.cut_name = str(multiplicity) + 'jet'
    if addInfo == 'perf':
        tool.max_prompt_trk = 5
        tool.cut_name += 'perf'

    if multiplicity == 1:
        tool.min_njets = 1
        if threshold != 140:
            raise RuntimeError("Cannot support a prompt jet pT threshold of {} [GeV] for dispjet with multiplicity {} in chain {}. Supported value is 140 [GeV]".format(threshold, multiplicity, name))
        tool.min_jet_pt = 140
        if addInfo != 'perf':
            tool.max_jets = 1
    elif multiplicity == 2:
        tool.min_njets = 2
        if threshold != 50:
            raise RuntimeError("Cannot support a prompt jet pT threshold of {} [GeV] for dispjet with multiplicity {} in chain {}. Supported value is 50 [GeV]".format(threshold, multiplicity, name))
        tool.min_jet_pt = 50
    else:
        raise RuntimeError("Cannot support a multiplicity of {} for dispjet in chain {}".format(multiplicity, name))

    return tool


def TrigDJHypoDispToolFromDict( chainDict ):
    """ Use menu decoded chain dictionary to configure the tool """

    name = chainDict['chainName']
    addInfo = chainDict['chainParts'][0]['addInfo']
    multiplicity = TrigDJExtractMultiplicity(chainDict)

    from AthenaConfiguration.ComponentFactory import CompFactory
    tool = CompFactory.DisplacedJetDispHypoTool(name)

    tool.min_trk_pt = 1.0
    tool.trk_d0cut = 3.0
    tool.max_z0st = 3.0
    tool.d0sigcut = 10.0
    tool.min_disp_trk_c2 = 3
    tool.nother_frac_c2 = 0.75
    tool.max_prompt_trk_c1 = 1
    tool.min_disp_trk_c1 = 0
    tool.nother_frac_c1 = 0.75
    tool.cut_name = str(multiplicity) + 'jet'

    if multiplicity == 1:
        tool.max_prompt_trk_c2 = 1
    elif multiplicity == 2:
        tool.max_prompt_trk_c2 = 2
    else:
        raise RuntimeError("Cannot support a multiplicity of {} for dispjet in chain {}".format(multiplicity, name))

    if addInfo == 'perf':
        tool.max_prompt_trk_c2 = 5
        tool.min_disp_trk_c2 = 0
        tool.nother_frac_c2 = 1.0
        tool.max_prompt_trk_c1 = 5
        tool.nother_frac_c1 = 1.0
        tool.cut_name += 'perf'

    return tool


def TrigDJHypoEDToolFromDict( chainDict ):
    """ Use menu decoded chain dictionary to configure the tool """

    name = chainDict['chainName']
    addInfo = chainDict['chainParts'][0]['addInfo']
    multiplicity = TrigDJExtractMultiplicity(chainDict)

    from AthenaConfiguration.ComponentFactory import CompFactory
    tool = CompFactory.DisplacedJetEventDecisionHypoTool(name)

    tool.min_h_jets = 1
    tool.cut_name = str(multiplicity) + 'jet'
    if addInfo == 'perf':
        tool.cut_name += 'perf'

    if multiplicity == 1:
        tool.min_l_jets = 0
    elif multiplicity == 2:
        tool.min_l_jets = 1
    else:
        raise RuntimeError("Cannot support a multiplicity of {} for dispjet in chain {}".format(multiplicity, name))

    return tool
