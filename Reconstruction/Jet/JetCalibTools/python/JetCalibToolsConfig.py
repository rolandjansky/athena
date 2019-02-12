# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

########################################################################
#                                                                      #
# JetCalibConfig: A helper module for configuring jet calibration      #
# The intention is to encode a limited set of configurations that      #
# are sane, to ease consistency for various users.                     #
# This module should eventually go in JetCalibTools and be kept in     #
# sync with the latest available recommendations.                      #
# Author: TJ Khoo                                                      #
#                                                                      #
########################################################################

from AthenaCommon import Logging
jetcaliblog = Logging.logging.getLogger('JetCalibToolsConfig')

from JetCalibTools import JetCalibToolsConf

all = ['getJetCalibTool']

# These context definitions could be placed in another package and made
# more robust with a small python class to hold the info.
# The convention is: config file, calibarea, default calibration sequence
pflowcontexts = {
    "T0":("JES_MC15cRecommendation_PFlow_Aug2016_rel21.config","00-04-77","JetArea_Residual_EtaJES"),
    # Omit smearing, to avoid any efficiency loss
    "AnalysisLatest":("JES_data2017_2016_2015_Consolidated_PFlow_2018_Rel21.config","00-04-82","JetArea_Residual_EtaJES_GSC"),
}

topocontexts = {
    "T0":("JES_MC15cRecommendation_May2016_rel21.config","00-04-77","JetArea_Residual_EtaJES"),
    # Placeholder from a vague approximation of the 2017 setup?
    "Trigger":("JES_MC15cRecommendation_May2016_Trigger.config","00-04-77","JetArea_Residual_EtaJES"),
    # Omit smearing, to avoid any efficiency loss
    "AnalysisLatest":("JES_data2017_2016_2015_Consolidated_EMTopo_2018_Rel21.config","00-04-82","JetArea_Residual_EtaJES_GSC"),
}

rscanlc2 = {
    "RScanLatest":("JES_MC16Recommendation_Rscan2LC_22Feb2018_rel21.config","00-04-81","JetArea_Residual_EtaJES_GSC")
}

rscanlc6 = {
    "RScanLatest":("JES_MC16Recommendation_Rscan6LC_22Feb2018_rel21.config","00-04-81","JetArea_Residual_EtaJES_GSC")
}

fatjetcontexts = {
    "CombinedMass": ("JES_MC16recommendation_FatJet_JMS_comb_19Jan2018.config","00-04-81","EtaJES_JMS"),
    "CaloMass":     ("JES_MC16recommendation_FatJet_JMS_calo_29Nov2017.config","00-04-81","EtaJES_JMS"),
    "TAMass":       ("JES_MC16recommendation_FatJet_JMS_TA_29Nov2017.config","00-04-81","EtaJES_JMS"),
}

# List AFII config files separately, to avoid needing to specify a different context
af2configs = {
    "AntiKt4EMPFlow": "JES_MC16Recommendation_AFII_PFlow_April2018_rel21.config",
    "AntiKt4EMTopo":  "JES_MC16Recommendation_AFII_EMTopo_April2018_rel21.config",
    "AntiKt4LCTopo":  "JES_MC16Recommendation_AFII_EMTopo_April2018_rel21.config",
}

calibcontexts = {
    # Standard AntiKt4
    "AntiKt4EMPFlow":pflowcontexts,
    "AntiKt4EMTopo":topocontexts,
    "AntiKt4LCTopo":topocontexts,
    # Standard trimmed
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20":fatjetcontexts,
    # R-Scan
    "AntiKt2LCTopo":rscanlc2,
    "AntiKt6LCTopo":rscanlc6,

}

hasInSitu = ["AntiKt4LCTopo", "AntiKt4EMTopo", "AntiKt4EMPFlow"]

# This method extracts the relevant configuration, does some consistency checks,
# then forwards the configuration to defineJetCalibTool, returning the output.
# At present the interface allows for the calibseq to be chosen freely, other
# than checking that the data source is data for the in situ correction.
# The calibarea could be made configurable as well, but then might need to be
# added to the tool name to ensure uniqueness.
# Due to the hackiness of DualUseConfig public tool handling, we need to pass
# an AlgSequence...
def getJetCalibTool(jetcollection, context, data_type, calibseq = ""):
    # In principle we could autoconfigure
    if not data_type in ['data','mc','afii']:
        jetcaliblog.error("JetCalibConfig accepts data_type values: 'data', 'mc', 'afii'")
        raise ValueError("Unsupported data_type provided: '{0}".format(data_type))

    if jetcollection.endswith("Jets"):
        jetcaliblog.error("Jet collection '{0}'should be specified w/o 'Jets' in the name.".format(jetcollection))
        raise ValueError("Bad jet collection formatting in getJetCalibTool.")

    jetcaliblog.debug("Preparing calibration for {0}, in context {1} on sample type {2}".format(jetcollection,context,data_type))

    jetcontexts = calibcontexts[jetcollection]
    try:
        configfile, calibarea, calibseq_def = jetcontexts[context]
        calibseq_tmp = calibseq if calibseq else calibseq_def
        if data_type == 'data' and calibseq_tmp.endswith("GSC") and jetcollection in hasInSitu:
            if calibseq_tmp == calibseq_def:
                calibseq_tmp += "_InSitu"
            # May not be strictly necessary if already checked in JCT
            elif not calibseq_tmp.endswith("InSitu"):
                jetcaliblog.warning("Calibration of jets requested in data without in situ calibration.")
        configfile_tmp = configfile
        if data_type == "afii" and jetcollection in af2configs.keys():
            configfile_tmp = af2configs[jetcollection]
        data_type_tmp = data_type
        # Most likely an oversight, but R20/21 JetRecCalibrationFinder did not set the data flag
        # This affects the residual correction by scaling mu.
        # We should revert this later on, set up now for validation purposes
        if context == "T0":
            data_type_tmp = "data"
        return defineJetCalibTool(jetcollection, configfile_tmp, calibarea, calibseq_tmp, data_type_tmp)
    except KeyError as e:
        jetcaliblog.error("Context '{0}' not found for jet collection '{1}'".format(context,jetcollection))
        jetcaliblog.error("Options are '{0}".format(','.join(jetcontexts.keys())))
        raise e
    return None

# This method actually sets up the tool
def defineJetCalibTool(jetcollection, configfile, calibarea, calibseq, data_type):
    # Abbreviate the calib sequence
    calibseqshort = ''.join([ step[0] for step in calibseq.split('_') ])
    toolname = "jetcalib_{0}_{1}".format(jetcollection,calibseqshort)
    #
    jct = JetCalibToolsConf.JetCalibrationTool(toolname,
        JetCollection = jetcollection,
        ConfigFile = configfile,
        CalibArea = calibarea,
        CalibSequence = calibseq,
        IsData = (data_type == "data")
    )
    return jct

# This method extends the basic config getter to specify the requisite jet
# moments or other inputs
def getJetCalibToolPrereqs(modspec,jetdef):
    calibcontext, data_type, calibseq = getCalibSpecsFromString(modspec)
    if calibseq=="":
        cfg, calibarea, calibseq = calibcontexts[jetdef.basename][calibcontext]
    # For now, only dependent on calibseq -- can ignore InSitu, which is
    # added when getting the concrete tool
    prereqs = []
    prereqs.append("mod:ConstitFourMom")
    if "JetArea" in calibseq:
        prereqs.append("input:EventDensity")
    if "GSC" in calibseq:
        prereqs += ["mod:CaloEnergies",
                    "mod:TrackMoments",
                    "ghost:MuonSegment"]
    jetcaliblog.debug("Prereqs for calibseq '{0}': {1}".format(calibseq,str(prereqs)))
    return prereqs

# This method translates the mod specification string into calibration specifications
def getCalibSpecsFromString(modspec):
    calibseq = ""
    calibspecs = modspec.split(':')
    # Probably want data_type to come from elsewhere
    calibcontext, data_type = calibspecs[:2]
    if len(calibspecs)>2: calibseq = calibspecs[2]
    return calibcontext, data_type, calibseq

# This method instantiates the JetCalibTool given the input mod specification
def getJetCalibToolFromString(modspec,jetdef):
    calibcontext, data_type, calibseq = getCalibSpecsFromString(modspec)
    return getJetCalibTool(jetdef.basename,calibcontext,data_type,calibseq)
