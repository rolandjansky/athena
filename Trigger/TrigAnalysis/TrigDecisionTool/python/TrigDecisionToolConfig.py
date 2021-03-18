#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

'''@file TrigDecisionToolConfig.py
@brief Configures and returns the TrigDecisionTool (TDT) for use in Athena-MT. 
The TDT uses a Tool interface such that it can be used in either Athena or AnalysisBase releases.
The manages an MT-safe internal state, hence we should only have one instance of it configured in any job.
It is thus one of the few places where it OK to use a PublicTool in Athena-MT.

Obtain the configured public tool instance by calling getPrimary()
on the ComponentAccumulator returned by this function, or by calling
getPublicTool("TrigDecisionTool") on any downstream merged ComponentAccumulator

When running in AnalysisBase, the tdt.TrigConfigTool="TrigConf::xAODConfigTool" should be used 
in place of the TrigConf::xAODConfigSvc
'''

def getRun3NavigationContainerFromInput(ConfigFlags):
    # List of all possible keys of the Run 3 navigation summary collection
    # in order of verbosity. Want to take the most verbose which is available.
    possible_keys = []
    possible_keys += 'HLTNav_Summary'
    possible_keys += 'HLTNav_Summary_BSSlimmed'
    possible_keys += 'HLTNav_Summary_ESDSlimmed'
    possible_keys += 'HLTNav_Summary_AODSlimmed'
    possible_keys += 'HLTNav_Summary_DAODSlimmed'

    # What to return if we cannot look in the file
    default_key = 'HLTNav_Summary'

    for key in possible_keys:
        if key in ConfigFlags.Input.Collections:
            return key

    return default_key

def getTrigDecisionTool(ConfigFlags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaConfiguration.AutoConfigFlags import GetFileMD
    from AthenaCommon.Logging import logging
    msg = logging.getLogger('getTrigDecisionTool')

    acc = ComponentAccumulator()

    cfgsvc = CompFactory.TrigConf.xAODConfigSvc('xAODConfigSvc')
    # We serve in-file metadata where possible. If it does not exist (e.g. RAWtoESD, RAWtoALL), then it is obtained from the Conditions and Detector stores
    md = GetFileMD(ConfigFlags.Input.Files)
    # This matches against both the R2 and R3 in-file metadata formats. xAODConfigSvc can serve either.
    use_infile_metadata = ("metadata_items" in md and any(('TriggerMenu' in key) for key in md["metadata_items"].keys()))
    cfgsvc.UseInFileMetadata = use_infile_metadata
    acc.addService(cfgsvc)

    tdt = CompFactory.Trig.TrigDecisionTool('TrigDecisionTool')
    tdt.TrigConfigSvc = cfgsvc
    use_run3_format = ConfigFlags.Trigger.EDMVersion == 3 # or ConfigFlags.Trigger.doEDMVersionConversion (to be added when this is working)
    tdt.NavigationFormat = "TrigComposite" if use_run3_format else "TriggerElement"
    tdt.HLTSummary = getRun3NavigationContainerFromInput(ConfigFlags)

    # This pre-loads libraries required to read the run 2 trigger navigation
    from TrigEDMConfig.TriggerEDM import EDMLibraries
    nav = CompFactory.HLT.Navigation('Navigation')
    nav.Dlls = [e for e in  EDMLibraries if 'TPCnv' not in e]
    tdt.Navigation = nav

    acc.addPublicTool(nav)
    acc.addPublicTool(tdt, primary=True)

    if use_infile_metadata:
        from AthenaServices.MetaDataSvcConfig import MetaDataSvcCfg
        acc.merge(MetaDataSvcCfg(ConfigFlags))

    msg.info("Configuring the TrigDecisionTool and xAODConfigSvc to use ConfigSource:{}, Run3NavigationFormat:{}, Run3NavigationSummaryCollection:{}".format(
        "InFileMetadata" if use_infile_metadata else "ConditionsAndDetStore",
        str(use_run3_format),
        tdt.HLTSummary)
    )

    return acc