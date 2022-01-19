# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger('TrigDJHypoTools')

def TrigDJHypoPromptToolFromDict( chainDict ):
    """ Use menu decoded chain dictionary to configure the tool """

    name = chainDict['chainName']
    from AthenaConfiguration.ComponentFactory import CompFactory
    tool = CompFactory.DisplacedJetPromptHypoTool(name)

    tool.min_jet_pt = 50
    tool.min_trk_pt = 2
    tool.trk_d0cut = 4.0
    tool.max_z0st = 3.0
    tool.d0sigcut = 25.0
    tool.max_prompt_trk = 4
    tool.max_jet_rank = 3
    tool.min_njets = 2
    tool.cut_name = "medium"

    if 'DJTrigMon:online' in chainDict['monGroups']:
        #monitoring config
        from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

        monTool = GenericMonitoringTool('MonTool')
        monTool.defineHistogram('pass_jet_pt', path='EXPERT', type='TH1F', title='p_{T} of passing jets;p_{T} [GeV]', xbins=30, xmin=0, xmax=300)
        monTool.defineHistogram('nprompt', path='EXPERT', type='TH1F', title='Number of prompt tracks in jet', xbins=20, xmin=0, xmax=20)
        monTool.defineHistogram('nother', path='EXPERT', type='TH1F', title='Number of other tracks in jet', xbins=20, xmin=0, xmax=20)
        monTool.defineHistogram('pass_nprompt', path='EXPERT', type='TH1F', title='Number of prompt tracks in passing jets', xbins=20, xmin=0, xmax=20)
        monTool.defineHistogram('pass_nother', path='EXPERT', type='TH1F', title='Number of other tracks in passing jets', xbins=20, xmin=0, xmax=20)

        tool.MonTool = monTool

    return tool


def TrigDJHypoPromptToolFromName( name, conf ):
    """ provides configuration of the hypo tool giben the chain name
    The argument will be replaced by "parsed" chain dict. For now it only serves simplest chain HLT_eXYZ.
    """
    from TriggerMenuMT.HLT.Menu.DictFromChainName import dictFromChainName

    decodedDict = dictFromChainName(conf)

    return TrigDJHypoPromptToolFromDict( decodedDict )

def TrigDJPromptHypoAlgMonTool():
    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

    monTool = GenericMonitoringTool('MonTool')

    monTool.defineHistogram('leading_jet_pt', path='EXPERT', type='TH1F', title='p_{T} leading jet in event;p_{T} [GeV]', xbins=30, xmin=0, xmax=300)

    return monTool


def TrigDJHypoDispToolFromDict( chainDict ):
    """ Use menu decoded chain dictionary to configure the tool """

    name = chainDict['chainName']
    from AthenaConfiguration.ComponentFactory import CompFactory
    tool = CompFactory.DisplacedJetDispHypoTool(name)


    tool.min_trk_pt = 2
    tool.trk_d0cut = 4.0

    tool.max_z0st = 3.0
    tool.d0sigcut = 25.0
    tool.cut_name = "medium"

    tool.max_prompt_trk_h = 4
    tool.min_disp_trk_h = 2
    tool.nother_frac_h = 0.75

    tool.max_prompt_trk_l = 3
    tool.min_disp_trk_l = 0
    tool.nother_frac_l = 0.45


    if 'DJTrigMon:online' in chainDict['monGroups']:
        #monitoring config
        from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

        monTool = GenericMonitoringTool('MonTool')
        monTool.defineHistogram('pass_jet_pt', path='EXPERT', type='TH1F', title='p_{T} of passing jets;p_{T} [GeV]', xbins=30, xmin=0, xmax=300)
        monTool.defineHistogram('jet_class', path='EXPERT', type='TH1F', title='Jet class', xbins=3, xmin=0, xmax=3)
        monTool.defineHistogram('nprompt', path='EXPERT', type='TH1F', title='Number of prompt tracks in jet', xbins=20, xmin=0, xmax=20)
        monTool.defineHistogram('ndisp', path='EXPERT', type='TH1F', title='Number of displaced tracks in jet', xbins=20, xmin=0, xmax=20)
        monTool.defineHistogram('frac_other', path='EXPERT', type='TH1F', title='Fraction of other tracks in jet', xbins=20, xmin=0, xmax=1)

        tool.MonTool = monTool

    return tool


def TrigDJHypoDispToolFromName( name, conf ):
    """ provides configuration of the hypo tool giben the chain name
    The argument will be replaced by "parsed" chain dict. For now it only serves simplest chain HLT_eXYZ.
    """
    from TriggerMenuMT.HLT.Menu.DictFromChainName import dictFromChainName

    decodedDict = dictFromChainName(conf)

    return TrigDJHypoDispToolFromDict( decodedDict )

def TrigDJHypoEDToolFromDict( chainDict ):
    """ Use menu decoded chain dictionary to configure the tool """

    name = chainDict['chainName']
    from AthenaConfiguration.ComponentFactory import CompFactory
    tool = CompFactory.DisplacedJetEventDecisionHypoTool(name)


    tool.min_h_jets = 1
    tool.min_l_jets = 1

    tool.cut_name = "medium"

    return tool


def TrigDJHypoEDToolFromName( name, conf ):
    """ provides configuration of the hypo tool giben the chain name
    The argument will be replaced by "parsed" chain dict. For now it only serves simplest chain HLT_eXYZ.
    """
    from TriggerMenuMT.HLT.Menu.DictFromChainName import dictFromChainName

    decodedDict = dictFromChainName(conf)

    return TrigDJHypoEDToolFromDict( decodedDict )


if __name__ == "__main__":
    tool = TrigDJHypoEDToolFromName("HLT_unconvtrk0_dispj_L1J100")
    assert tool, "Not configured simple tool"


    log.info("ALL OK")
