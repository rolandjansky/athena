# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Import a needed helper
from PATCore.HelperUtils import GetTool


def ElectronLikelihoodLooseTriggerConfig2015(theTool):
    '''
    This is for the custom implementation of the Loose, No GSF-variable (trigger-friendly), no d0significance (for now) definitions.
    This uses Online PDFs, and currently has no conversion bit either. Also note that the PDF for trackd0 comes from
    trig_EF_trackd0_physics, but the LH tool requires it to be named el_trackd0pvunbiased. (NOTE: same signal eff as offline Loosepp + 1%)
    '''
    theTool = GetTool(theTool)
    theTool.ConfigFile = "ElectronPhotonSelectorTools/trigger/mc15_20150712/ElectronLikelihoodLooseTriggerConfig2015.conf"


def ElectronLikelihoodMediumTriggerConfig2015(theTool):
    '''
    This is for the custom implementation of the Medium, No GSF-variable (trigger-friendly), no d0significance (for now) definitions.
    This uses Online PDFs, and currently has no conversion bit either. Also note that the PDF for trackd0 comes from
    trig_EF_trackd0_physics, but the LH tool requires it to be named el_trackd0pvunbiased. (NOTE: same signal eff as Offline Mediumpp + 1%)
    '''
    theTool = GetTool(theTool)
    theTool.ConfigFile = "ElectronPhotonSelectorTools/trigger/mc15_20150712/ElectronLikelihoodMediumTriggerConfig2015.conf"


def ElectronLikelihoodTightTriggerConfig2015(theTool):
    '''
    This is for the custom implementation of the Tight, No GSF-variable (trigger-friendly), no d0significance (for now) definitions.
    This uses Online PDFs, and currently has no conversion bit either. Also note that the PDF for trackd0 comes from
    trig_EF_trackd0_physics, but the LH tool requires it to be named el_trackd0pvunbiased. (NOTE: same signal eff as offline Tightpp + 1%)
    '''
    theTool = GetTool(theTool)
    theTool.ConfigFile = "ElectronPhotonSelectorTools/trigger/mc15_20150712/ElectronLikelihoodTightTriggerConfig2015.conf"

def ElectronLikelihoodVeryLooseOfflineConfig2016(theTool):
    '''
    This is for the custom implementation of the VeryLoose offline likelihood for MC16 / Run 2 / Release 21.
    This uses Offline PDFs, but does not yet have the pileup dependent discriminant cut.
    '''
    theTool = GetTool(theTool)
    theTool.ConfigFile = "ElectronPhotonSelectorTools/offline/mc20_20210514/ElectronLikelihoodVeryLooseOfflineConfig2017_Smooth.conf"


def ElectronLikelihoodLooseOfflineConfig2016(theTool):
    '''
    This is for the custom implementation of the Loose offline likelihood for MC16 / Run 2 / Release 21.
    This uses Offline PDFs, but does not yet have the pileup dependent discriminant cut.
    (NOTE: same signal eff as offline Loosepp + 1%)
    '''
    theTool = GetTool(theTool)
    theTool.ConfigFile = "ElectronPhotonSelectorTools/offline/mc20_20210514/ElectronLikelihoodLooseOfflineConfig2017_Smooth.conf"


def ElectronLikelihoodMediumOfflineConfig2016(theTool):
    '''
    This is for the custom implementation of the Medium offline likelihood for MC16 / Run 2 / Release 21.
    This uses Offline PDFs, but does not yet have the pileup dependent discriminant cut.
    (NOTE: same signal eff as offline Mediumpp + 1%)
    '''
    theTool = GetTool(theTool)
    theTool.ConfigFile = "ElectronPhotonSelectorTools/offline/mc20_20210514/ElectronLikelihoodMediumOfflineConfig2017_Smooth.conf"


def ElectronLikelihoodTightOfflineConfig2016(theTool):
    '''
    This is for the custom implementation of the Tight offline likelihood for MC16 / Run 2 / Release 21.
    This uses Offline PDFs, but does not yet have the pileup dependent discriminant cut.
    (NOTE: same signal eff as offline Tightpp + 1%)
    '''
    theTool = GetTool(theTool)
    theTool.ConfigFile = "ElectronPhotonSelectorTools/offline/mc20_20210514/ElectronLikelihoodTightOfflineConfig2017_Smooth.conf"
