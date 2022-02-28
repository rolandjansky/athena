#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#


# List of all possible keys of the Run 3 navigation summary collection
# in order of verbosity. Want to take the most verbose which is available.
from AthenaCommon.Logging import logging

def getTrigDecisionTool(flags):
    msg = logging.getLogger('getTrigDecisionTool')
    msg.warning("This function is becoming obsolete. Use: TrigDecisionToolCfg(flags) - imported from the same package")
    return TrigDecisionToolCfg(flags)


def TrigDecisionToolCfg(flags):
    '''
    @brief Configures and returns the TrigDecisionTool (TDT) for use in Athena-MT. 
    The TDT uses a Tool interface such that it can be used in either Athena or AnalysisBase releases.
    The manages an MT-safe internal state, hence we should only have one instance of it configured in any job.
    It is thus one of the few places where it OK to use a PublicTool in Athena-MT.

    Obtain the configured public tool instance by calling getPrimary()
    on the ComponentAccumulator returned by this function, or by calling
    getPublicTool('TrigDecisionTool') or getPrimaryAndMerge()
    on any downstream merged ComponentAccumulator.


    When running in AnalysisBase, the tdt.TrigConfigTool='TrigConf::xAODConfigTool' should be used 
    in place of the TrigConf::xAODConfigSvc
    '''
    msg = logging.getLogger('TrigDecisionToolCfg')
    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.Enums import Format
    acc = ComponentAccumulator()

    from TrigConfxAOD.TrigConfxAODConfig import getxAODConfigSvc
    cfgsvc = acc.getPrimaryAndMerge(getxAODConfigSvc(flags))

    tdt = CompFactory.Trig.TrigDecisionTool('TrigDecisionTool')
    tdt.TrigConfigSvc = cfgsvc
    use_run3_format = flags.Trigger.EDMVersion == 3 # or flags.Trigger.doEDMVersionConversion (to be added when this is working)
    tdt.NavigationFormat = 'TrigComposite' if use_run3_format else 'TriggerElement'
    tdt.HLTSummary = getRun3NavigationContainerFromInput(flags)

    if flags.Input.Format is Format.BS and flags.Trigger.EDMVersion in [1, 2]:
        tdt.TrigDecisionKey = 'TrigDecision'
        tdt.UseAODDecision = True

    # This pre-loads libraries required to read the run 2 trigger navigation
    from TrigEDMConfig.TriggerEDM import EDMLibraries
    nav = CompFactory.HLT.Navigation('Navigation')
    nav.Dlls = [e for e in  EDMLibraries if 'TPCnv' not in e]
    tdt.Navigation = nav

    acc.addPublicTool(nav)
    acc.addPublicTool(tdt, primary=True)

    msg.info('Configuring the TrigDecisionTool and xAODConfigSvc to use ConfigSource: %s, Run3NavigationFormat: %s, Run3NavigationSummaryCollection: %s',
        'InFileMetadata' if flags.Trigger.InputContainsConfigMetadata else 'ConditionsAndDetStore',
        str(use_run3_format),
        tdt.HLTSummary)

    return acc


possible_keys = [
    'HLTNav_Summary',
    'HLTNav_Summary_OnlineSlimmed',
    'HLTNav_Summary_ESDSlimmed',
    'HLTNav_Summary_AODSlimmed',
    'HLTNav_Summary_DAODSlimmed'
    ]

def getRun3NavigationContainerFromInput(flags):
    # What to return if we cannot look in the file
    default_key = 'HLTNav_Summary_OnlineSlimmed' if flags.Trigger.doOnlineNavigationCompactification else 'HLTNav_Summary'
    to_return = default_key

    for key in possible_keys:
        if key in flags.Input.Collections:
            to_return = key
            break

    from AthenaCommon.Logging import logging
    msg = logging.getLogger('getRun3NavigationContainerFromInput')
    msg.info('Returning {} as the Run 3 trigger navigation colletion to read in this job.'.format(to_return))
    return to_return


if __name__ == '__main__':
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    import sys
    if '--RAWRUN2' in sys.argv:
        flags.Input.Files = defaultTestFiles.RAW
    else:
        flags.Input.Files = defaultTestFiles.AOD
        #TODO expand the test scope Run3 AODs and RAWs

    flags.lock()
    acc = TrigDecisionToolCfg(flags)
    acc.printConfig(withDetails=True, summariseProps=True, prefix='UnitTest')
    acc.wasMerged()
    # TODO possibly add EDM printing alg using the TDT

