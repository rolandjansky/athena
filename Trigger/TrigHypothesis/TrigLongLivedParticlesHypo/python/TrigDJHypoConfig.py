# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger('TrigDJHypoTools')

#Object cuts sets used to configure the cuts which define prompt and displaced tracks
object_cuts_sets = {"set1": {"min_trk_pt": 1.0, "trk_d0cut": 3.0, "max_z0st": 3.0, "d0_sigcut": 10.0}}

#cuts applied for each config value
config_dict = {
    "3d2p":{
        "object_cuts": "set1",
        "min_disp_trk": 3,
        "max_prompt_trk": 2,
        "other_frac": 0.75,
        "max_jet_rank": 2
    },
    "1p":{
        "object_cuts": "set1",
        "min_disp_trk": 0,
        "max_prompt_trk": 1,
        "other_frac": 0.75,
        "max_jet_rank": 2
    },
    "x3d1p":{
        "object_cuts": "set1",
        "min_disp_trk": 0,
        "max_prompt_trk": 1,
        "other_frac": 0.75,
        "prompt_stage_max_prompt_trk": 2,
        "max_jets": 1,
        "max_jet_rank": 0
    }
}


def TrigDJExtractThreshold(chainDict):
    threshold = [i['threshold'] for i in chainDict['chainParts'] if i['signature'] == 'UnconventionalTracking' and 'dispjet' in i['chainPartName']]
    
    if len(threshold) != 1:
        raise RuntimeError("Cannot support mutiple occurrences of dispjet in chain {}".format(chainDict['chainName']))

    return int(threshold[0])



def TrigDJGetConfigValue(chainDict, key):
    values = [i[key] for i in chainDict['chainParts'] if i['signature'] == 'UnconventionalTracking' and 'dispjet' in i['chainPartName'] and key in i]

    if len(values) != 1:
        raise RuntimeError("Invalid chain dictionary for Displaced Jet Trigger, unable to find config value '{}' in {}".format(key, str(chainDict)))

    return values[0]

def TrigDJEnableMonitoring(chainDict):
    return 'dispjetMon:online' in chainDict['monGroups']

def TrigDJHypoPromptToolFromDict( chainDict ):
    """ Use menu decoded chain dictionary to configure the tool """

    name = chainDict['chainName']
    threshold = TrigDJExtractThreshold(chainDict)

    cfg_name = TrigDJGetConfigValue(chainDict, "dispjetConfig")
    cfg = config_dict[cfg_name]

    if cfg is None:
        raise RuntimeError("Unknown displaced jet config key: '{}'".format(cfg_name))

    obj_cuts = object_cuts_sets[cfg["object_cuts"]]

    if obj_cuts is None:
        raise RuntimeError("Incorrectly configured displaced jet config key: '{}' Object cut set '{}' does not exist".format(cfg_name, cfg["object_cuts"]))

    from AthenaConfiguration.ComponentFactory import CompFactory
    tool = CompFactory.DisplacedJetPromptHypoTool(name)

    #configure from the object defintion set first
    tool.min_trk_pt = obj_cuts["min_trk_pt"]
    tool.trk_d0cut = obj_cuts["trk_d0cut"]
    tool.max_z0st = obj_cuts["max_z0st"]
    tool.d0sigcut = obj_cuts["d0_sigcut"]
    tool.cut_name = cfg["object_cuts"]

    tool.max_prompt_trk = cfg["max_prompt_trk"]
    tool.min_jet_pt = float(threshold)

    if "prompt_stage_max_prompt_trk" in cfg:
        tool.max_prompt_trk = cfg["prompt_stage_max_prompt_trk"]

    #monitoring
    if TrigDJEnableMonitoring(chainDict):
        from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
        monTool = GenericMonitoringTool("DJPrompt_MonTool_"+name)

        monTool.defineHistogram("trk_d0sig", type='TH1F', path='EXPERT', title="Track d0sig", xbins=200, xmin=0.0, xmax=200.0)
        monTool.defineHistogram("trk_z0st", type='TH1F', path='EXPERT', title="Track z0st", xbins=200, xmin=0.0, xmax=20.0)
        monTool.defineHistogram("trk_d0", type='TH1F', path='EXPERT', title="Track d0", xbins=200, xmin=0.0, xmax=20.0)
        monTool.defineHistogram("nprompt", type='TH1F', path='EXPERT', title="nPrompt tracks per Jet", xbins=20, xmin=0, xmax=20)

        tool.MonTool = monTool

    return tool


def TrigDJHypoDispToolFromDict( chainDict ):
    """ Use menu decoded chain dictionary to configure the tool """

    name = chainDict['chainName']

    cfg_name = TrigDJGetConfigValue(chainDict, "dispjetConfig")
    cfg = config_dict[cfg_name]

    if cfg is None:
        raise RuntimeError("Unknown displaced jet config key: '{}'".format(cfg_name))

    obj_cuts = object_cuts_sets[cfg["object_cuts"]]

    if obj_cuts is None:
        raise RuntimeError("Incorrectly configured displaced jet config key: '{}' Object cut set '{}' does not exist".format(cfg_name, cfg["object_cuts"]))

    from AthenaConfiguration.ComponentFactory import CompFactory
    tool = CompFactory.DisplacedJetDispHypoTool(name)

    #configure from the object defintion set first
    tool.min_trk_pt = obj_cuts["min_trk_pt"]
    tool.trk_d0cut = obj_cuts["trk_d0cut"]
    tool.max_z0st = obj_cuts["max_z0st"]
    tool.d0sigcut = obj_cuts["d0_sigcut"]
    tool.cut_name = cfg["object_cuts"]

    tool.nother_frac = cfg["other_frac"]
    tool.min_disp_trk = cfg["min_disp_trk"]
    tool.max_prompt_trk = cfg["max_prompt_trk"]

    #monitoring
    if TrigDJEnableMonitoring(chainDict):
        from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
        monTool = GenericMonitoringTool("DJDisp_MonTool_"+name)

        monTool.defineHistogram("trk_d0sig", type='TH1F', path='EXPERT', title="Track d0sig", xbins=200, xmin=0.0, xmax=200.0)
        monTool.defineHistogram("trk_z0st", type='TH1F', path='EXPERT', title="Track z0st", xbins=200, xmin=0.0, xmax=20.0)
        monTool.defineHistogram("trk_d0", type='TH1F', path='EXPERT', title="Track d0", xbins=200, xmin=0.0, xmax=20.0)
        monTool.defineHistogram("nprompt", type='TH1F', path='EXPERT', title="nPrompt tracks per Jet", xbins=20, xmin=0, xmax=20)
        monTool.defineHistogram("ndisp", type='TH1F', path='EXPERT', title="nDisplaced tracks per Jet", xbins=20, xmin=0, xmax=20)
        monTool.defineHistogram("frac_other", type='TH1F', path='EXPERT', title="Fraction of tracks which fail all cuts", xbins=20, xmin=0, xmax=1.1)

        tool.MonTool = monTool

    return tool

def TrigDJComboHypoToolFromDict(chainDict):
    name = chainDict['chainName']

    from AthenaConfiguration.ComponentFactory import CompFactory
    tool = CompFactory.DisplacedJetRankComboHypoTool(name)

    cfg_names = [i["dispjetConfig"] for i in chainDict['chainParts'] if i['signature'] == 'UnconventionalTracking' and 'dispjet' in i['chainPartName'] and "dispjetConfig" in i]
    cfgs = [config_dict[i] for i in cfg_names]
    max_jets = [i["max_jets"] for i in cfgs if "max_jets" in i]
    max_rank = [i["max_jet_rank"] for i in cfgs if "max_jet_rank" in i]

    if len(max_jets) > 0:
        tool.max_jets = max(max_jets)

    if len(max_rank) > 0:
        tool.max_jet_rank = max(max_rank)


    return tool
