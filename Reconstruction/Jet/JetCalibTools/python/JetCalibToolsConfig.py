# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

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

all = ['getJetCalibTool']

# These context definitions could be placed in another package and made
# more robust with a small python class to hold the info.
# The convention is: config file, calibarea, default calibration sequence
pflowcontexts = {
    "T0":("JES_MC15cRecommendation_PFlow_Aug2016_rel21.config","00-04-77","JetArea_Residual_EtaJES"),
    # Omit smearing, to avoid any efficiency loss
    "AnalysisLatest":("JES_data2017_2016_2015_Consolidated_PFlow_2018_Rel21.config","00-04-82","JetArea_Residual_EtaJES_GSC_Insitu"),
    "TrigLS2":("JES_MC16Recommendation_Consolidated_PFlow_Apr2019_Rel21_Trigger.config","00-04-82","JetArea_Residual_EtaJES_GSC_Insitu"),
}

topocontexts = {
    "T0":("JES_MC15cRecommendation_May2016_rel21.config","00-04-77","JetArea_Residual_EtaJES"),
    # Omit smearing, to avoid any efficiency loss
    "AnalysisLatest":("JES_data2017_2016_2015_Consolidated_EMTopo_2018_Rel21.config","00-04-82","JetArea_Residual_EtaJES_GSC_Insitu"),
    "TrigRun2":("JES_MC15cRecommendation_May2016_Trigger.config","00-04-77","JetArea_EtaJES_GSC_Insitu"),
    "TrigRun2GSC":("JES_data2016_data2015_Recommendation_Dec2016_rel21.config","00-04-77","JetArea_EtaJES_GSC_Insitu"),
}

rscanlc2 = {
    "RScanLatest":("JES_MC16Recommendation_Rscan2LC_22Feb2018_rel21.config","00-04-81","JetArea_Residual_EtaJES_GSC_Insitu")
}

rscanlc6 = {
    "RScanLatest":("JES_MC16Recommendation_Rscan6LC_22Feb2018_rel21.config","00-04-81","JetArea_Residual_EtaJES_GSC_Insitu")
}

fatjetcontexts = {
    "CombinedMass":  ("JES_MC16recommendation_FatJet_JMS_comb_19Jan2018.config","00-04-81","EtaJES_JMS"),
    "CaloMass":      ("JES_MC16recommendation_FatJet_JMS_calo_29Nov2017.config","00-04-81","EtaJES_JMS"),
    "TAMass":        ("JES_MC16recommendation_FatJet_JMS_TA_29Nov2017.config","00-04-81","EtaJES_JMS"),
    "TrigUngroomed": ("JES_Full2012dataset_Rscan_June2014.config","00-04-77","JetArea_EtaJES"),
    "TrigTrimmed":   ("JES_MC15recommendation_FatJet_June2015_PtFrac4.config","00-04-82","EtaJES_JMS"),
    "TrigSoftDrop":  ("JES_MC16recommendation_R10_UFO_CSSK_SoftDrop_JMS_01April2020.config","00-04-82","EtaJES_JMS"),
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
    "AntiKt10LCTopo":fatjetcontexts,
    # Standard trimmed
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20":fatjetcontexts,
    "AntiKt10LCTopoTrimmedPtFrac4SmallR20":fatjetcontexts,
    # Large-R PFlow Soft Drop CSSK
    "AntiKt10EMPFlowCSSKSoftDropBeta100Zcut10":fatjetcontexts,
    # R-Scan
    "AntiKt2LCTopo":rscanlc2,
    "AntiKt6LCTopo":rscanlc6,

}

hasInSitu = ["AntiKt4LCTopo", "AntiKt4EMTopo", "AntiKt4EMPFlow", "TrigAntiKt4EMTopo"]

# This method extracts the relevant configuration, does some consistency checks,
# then forwards the configuration to defineJetCalibTool, returning the output.
# At present the interface allows for the calibseq to be chosen freely, other
# than checking that the data source is data for the in situ correction.
def getJetCalibTool(jetcollection, context, data_type, calibseq = "", rhoname = "", pvname = "PrimaryVertices", gscdepth = "auto"):
    # In principle we could autoconfigure
    if data_type not in ['data','mc','afii']:
        jetcaliblog.error("JetCalibConfig accepts data_type values: 'data', 'mc', 'afii'")
        raise ValueError("Unsupported data_type provided: '{0}".format(data_type))

    if jetcollection.endswith("Jets"):
        jetcaliblog.error("Jet collection '{0}'should be specified w/o 'Jets' in the name.".format(jetcollection))
        raise ValueError("Bad jet collection formatting in getJetCalibTool.")

    jetcaliblog.debug("Preparing calibration for {0}, in context {1} on sample type {2}".format(jetcollection,context,data_type))

    jetcontexts = calibcontexts[jetcollection]
    try:
        configfile, calibarea, calibseq_def = jetcontexts[context]        
        _calibseq = calibseq if calibseq else calibseq_def
        
        # Check that the calib sequence requests something sensible for the in situ calibration
        # Leave other checks for the tool code.
        # Might need to specialise if we decide MC trigger jets should also have in situ.
        if _calibseq.endswith("Insitu"):
            if data_type == 'data':
                if jetcollection not in hasInSitu:
                    raise ValueError("In situ calibration does not exist for {0}, context {1}".format(jetcollection,context))
            else:
                raise ValueError("In situ calibration requested for MC on {0}, context {1}".format(jetcollection,context))
        _configfile = configfile
        if data_type == "afii" and jetcollection in af2configs.keys():
            _configfile = af2configs[jetcollection]
        _data_type = data_type
        # Most likely an oversight, but R20/21 JetRecCalibrationFinder did not set the data flag
        # This affects the residual correction by scaling mu.
        # We should revert this later on, set up now for validation purposes
        if context == "T0":
            _data_type = "data"
        _pvname = ""
        if "Residual" in _calibseq or "GSC" in _calibseq and gscdepth!="EM3":
            _pvname = pvname
        # HACK: For testing while we don't have finalised calibrations for trigger PF jets
        _jetcollection = jetcollection
        if "PFlow" in jetcollection and context=="TrigSoftDrop":
            _jetcollection = jetcollection.replace("EMPFlow","UFO")
        return defineJetCalibTool(_jetcollection, _configfile, calibarea, _calibseq, _data_type, rhoname, _pvname, gscdepth)
    except KeyError as e:
        jetcaliblog.error("Context '{0}' not found for jet collection '{1}'".format(context,jetcollection))
        jetcaliblog.error("Options are '{0}".format(','.join(jetcontexts.keys())))
        raise e
    return None

# This method actually sets up the tool
def defineJetCalibTool(jetcollection, configfile, calibarea, calibseq, data_type, rhoname, pvname, gscdepth):
    # Abbreviate the calib sequence
    calibseqshort = ''.join([ step[0] for step in calibseq.split('_') ])
    toolname = "jetcalib_{0}_{1}".format(jetcollection,calibseqshort)
    #
    from AthenaConfiguration.ComponentFactory import CompFactory
    jct = CompFactory.JetCalibrationTool(toolname,
                                         JetCollection = jetcollection,
                                         ConfigFile = configfile,
                                         CalibArea = calibarea,
                                         CalibSequence = calibseq,
                                         IsData = (data_type == "data"),
                                         RhoKey = rhoname,
                                         PrimaryVerticesContainerName = pvname,
                                         GSCDepth = gscdepth
    )
    return jct

# This method extends the basic config getter to specify the requisite jet
# moments or other inputs
def getJetCalibToolPrereqs(modspec,jetdef):
    calibcontext, data_type, calibseq, rhoname, pvname, gscdepth = getCalibSpecsFromString(modspec)
    if calibseq=="":
        cfg, calibarea, calibseq = calibcontexts[jetdef.basename][calibcontext]

    # For now, only dependent on calibseq -- can ignore Insitu, which is
    # added when getting the concrete tool
    prereqs = []
    prereqs.append("mod:ConstitFourMom")
    if "JetArea" in calibseq: # Will not insert a prefix here
        if calibcontext.startswith("Trig"): prereqs.append("input:HLT_EventDensity")
        else: prereqs.append("input:EventDensity")
    if "GSC" in calibseq:
        prereqs += ["mod:CaloEnergies"]
        if calibcontext != "TrigRun2": # No track/MS GSC for trigger w/o FTK
            prereqs += ["mod:TrackMoments",
                        "ghost:MuonSegment"]
    jetcaliblog.debug("Prereqs for calibseq '{0}': {1}".format(calibseq,str(prereqs)))
    return prereqs

# This method translates the mod specification string into calibration specifications
def getCalibSpecsFromString(modspec):
    calibseq = ""
    rhoname = "auto"
    pvname = "PrimaryVertices"
    gscdepth = "auto"
    calibspecs = modspec.split(':')
    # Probably want data_type to come from elsewhere
    calibcontext, data_type = calibspecs[:2]
    if len(calibspecs)>2: calibseq = calibspecs[2]
    if len(calibspecs)>3: rhoname = calibspecs[3]
    if len(calibspecs)>4: pvname = calibspecs[4]
    if len(calibspecs)>5: gscdepth = calibspecs[5]
    return calibcontext, data_type, calibseq, rhoname, pvname, gscdepth

# This method instantiates the JetCalibTool given the input mod specification
def getJetCalibToolFromString(modspec,jetdef):
    calibcontext, data_type, calibseq, rhoname, pvname, gscdepth = getCalibSpecsFromString(modspec)
    return getJetCalibTool(jetdef.basename,calibcontext,data_type,calibseq,rhoname,pvname,gscdepth)
