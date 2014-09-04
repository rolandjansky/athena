# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Logging import logging

from JetRec.FastJetInterfaceConfig import *

# Plain dictionary for event features
defEventFeatureToolDict = {
    'InputCollectionKey'   : 'CaloCalTopoCluster',
    'EventFeatureKey'      : 'TopoClusterFeature',
    'EventFeatureType'     : 'UNKNOWN',
    'EventFeatureStoreKey' : 'CaloTopoClusterFeatures',
    'SignalState'          : 'CALIBRATED' 
    }

# Configure default FastJet event scan jets 
defScanJets = getFastJetInterfaceConfig("LCTopoScanJetsKt4",
                                        Algorithm='kt',
                                        Radius=0.4,
                                        JetPtMin=0.,
                                        JetAreaMethod='ActiveGhost'
                                        GhostRepeats=1);

defEventEtDensityToolDict = {
    InputCollectionKey   : 'NONE',
    EventFeatureKey      : 'EventEtDensity',
    EventFeatureType     : 'ETDENSITY',
    EventFeatureStoreKey : 'CaloTopoClusterEtDensity',
    SignalState          : 'CALIBRATED'
    EtaRangeMin          : -5.0,
    EtaRangeMax          : 5.0,
    JetPtMin             : 20*GeV,
    FastJetTool          : defScanJets 
    } 

_confLog = logging.getLogger("EventFeatureConfig")


# Check whole dictionary or key/value assigments and return dictionary with
# invalid options stripped (if allowed) or exception thrown for invalid options
def checkAndUpdate(**options):
    # already checked
    if options.get("_alreadyChecked_",False) or options.get("_locked_",False):
        return options

    # check what to do with unknowns
    ignoreUnknown = options.pop("_ignoreUnknown_",False)

    # check every entry
    for k in options.keys():
        if k not in defFastJetInterfaceConfigDict :
            if ignoreUnknown :
                _fastjetLog.warning("Option %s unknown - ignoring it!"%(k))
                options.pop(k)
            else :
                _fastjetLog.error("Option %s unknown - abort configuration!"%(k))
                raise Exception

    checkedOptions = dict(defFastJetInterfaceConfigDict)
    for k,v in defFastJetInterfaceConfigDict.iteritems():
        t = type(v)
        if t in ( list, set, dict ) :
            checkedOptions[k] = t(v)

    checkedOptions['_alreadyChecked_'] = True
    checkedOptions.update(options)

    # check settings for Strategy
    if checkOptions[Strategy] not in fastjet_conf_tags.Strategy :
        _fastjetLog.error("Strategy \042%s\042 not recognized - fatal! Allowed values are: ",%(checkOptions[Strategy]))
        for s in fastjet_conf_tags.Strategy :
            _fastjetLog.error("\042%s\042",%(s))
        raise Exception
    
    # check settings for RecombScheme
    if checkOptions[RecombScheme] not in fastjet_conf_tags.RecombScheme :
        _fastjetLog.error("RecombScheme \042%s\042 not recognized - fatal! Allowed values are: ",%(checkOptions[RecombScheme]))
        for s in fastjet_conf_tags.RecombScheme :
            _fastjetLog.error("\042%s\042",%(s))
        raise Exception
    
    # check settings for Algorithm
    if checkOptions[Algorithm] not in fastjet_conf_tags.Algorithm :
        _fastjetLog.error("Algorithm \042%s\042 not recognized - fatal! Allowed values are: ",%(checkOptions[Algorithm]))
        for s in fastjet_conf_tags.Algorithm :
            _fastjetLog.error("\042%%s\042",%(s))
        raise Exception

    # check settings for JetAreaMethod
    if checkOptions[JetAreaMethod] not in fastjet_conf_tags.JetAreaMethod :
        _fastjetLog.error("JetAreaMethod \042%s\042 not recognized - fatal! Allowed values are: ",%(checkOptions[JetAreaMethod]))
        for s in fastjet_conf_tags.JetAreaMethod :
            _fastjetLog.error("\042%s\042",%(s))
        raise Exception

    return checkedOptions

    
# generator
def getEventEtDensityToolConfig(name,**options):
    # 
    etdTool = EventEtDensityTool(name)
    #
    
