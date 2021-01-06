# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.SystemOfUnits import GeV
from AthenaCommon.Logging import logging

from JetRec.JetRecConf import FastJetInterfaceTool

_fastjetLog = logging.getLogger("FastJetInterfaceConfiguration")

# set up some enumerator values
def enums(name='Enum',**enums):
    return type( name, (), enums)

# recognized keys
fastjet_conf_tags = enums('FastJetConfTags',
                          Strategy=[ 'default', 'Best', 
                                     'N2MinHeapTiled','N2Tiled', 'N2PoorTiled', 'N2Plain',
                                     'N3Dumb', 
                                     'NlnN', 'NlnN3pi', 'NlnN4pi', 'NlnNCam4pi', 'NlnNCam2pi2R', 'NlNCam',
                                     'plugin_strategy' ],
                          RecombScheme = [ 'default', 'E', 'pt', 'pt2', 'Et', 'Et2', 'BIpt', 'BIpt2' ],
                          Algorithm   = [ 'default', 'kt', 'Kt', 'anti-kt', 'AntiKt', 'cambridge', 'CamKt',
                                          'genkt', 'passive cambridge', 'passive genkt',
                                          'CMSCone', 'SISCone'],
                          JetAreaMethod = [ 'default', 'VoronoiArea', 'ActiveArea', 
                                            'ActiveAreaExplicitGhost', 'PassiveArea', '1GhostPassiveArea' ],
                          SISSplitMergeScale = [ 'default', 'pttilde', 'PtTilde', 'Pt', 'Et', 'mt' ], 
                          )

# Ghosted area parameters
fastjet_gas = enums('FastJetGhostAreaSettings',
                    def_ghost_maxrap  = 6.0,   #fastjet::gas::def_ghost_maxrap
                    def_repeat        = 1,     #fastjet::gas::def_repeat
                    def_ghost_area    = 0.01,  #fastjet::gas::def_ghost_area
                    def_grid_scatter  = 1.0,   #fastjet::gas::def_grid_scatter
                    def_kt_scatter    = 0.1,   #fastjet::gas::def_kt_scatter
                    def_mean_ghost_kt = 1e-100,#fastjet::gas::def_mean_ghost_kt
    )

# ignored keys
config_ignored_keys = enums('SetupIgnoredKeys',
                            ControlKeys = ["_alreadyChecked_","_locked_","_ignoreUnknown_" ])

# Default FastJet configuration dictionary: 
#
# Most keys are the same as the corresponding FastJet tags or enumerator names.
# In addition, for backward compatibility, the following tags are recognized:
#
#  
defFastJetInterfaceConfigDict = {
    # -- overall setup and process control
    'Algorithm'               : "anti-kt",
    'JetAreaMethod'           : "VoronoiArea",
    'CalculateJetArea'        : False,
    # -- kt-style parameters
    'Strategy'                : "Best",
    'RecombScheme'            : "E",
    # -- CMS cone parameters
    'CMS_SeedThreshold'       : 15.*GeV,
    # -- SIS cone parameters
    'SIS_OverlapThreshold'    : 0.75,
    'SIS_NumPassMax'          : 0,
    'SIS_ProtojetPtMin'       : 0.0,
    'SIS_DoCaching'           : False,
    'SIS_SplitMergeScale'     : 'PtTilde',
    'SIS_SplitMergeStopScale' : 0.0,
    # -- jet algorithm parameters
    'Radius'                  : 0.4,       # ATLAS default
    'Inclusive'               : True,      # ATLAS default
    'InclusivePtMin'          : 0.*GeV,
    'ExclusiveDcut'           : 0.5,       
    'ExclusiveNjets'          : 3,
    # -- jet area calculation directives and parameters
    'VoronoiEffectiveRfact'   : 1.0,   # Voronoi
    'GhostMaxRapidity'        : fastjet_gas.def_ghost_maxrap,   
    'GhostMinRapidity'        : -fastjet_gas.def_ghost_maxrap,   
    'GhostRepeats'            : fastjet_gas.def_repeat,
    'GhostAreaSize'           : fastjet_gas.def_ghost_area,
    'GhostGridScatter'        : fastjet_gas.def_grid_scatter,
    'GhostKtScatter'          : fastjet_gas.def_kt_scatter,
    'GhostMeanKt'             : fastjet_gas.def_mean_ghost_kt
    }

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
                _fastjetLog.warning("Option %s unknown - ignoring it!",k)
                options.pop(k)
            else :
                _fastjetLog.error("Option %s unknown - abort configuration!",k)
                raise Exception

    checkedOptions = dict(defFastJetInterfaceConfigDict)
    for k,v in defFastJetInterfaceConfigDict.iteritems():
        t = type(v)
        if t in ( list, set, dict ) :
            checkedOptions[k] = t(v)

    checkedOptions['_alreadyChecked_'] = True
    checkedOptions.update(options)

    # check settings for Strategy
    key = "Strategy"
    #    print checkedOptions
    _fastjetLog.info("Test option %s",key)
    if checkedOptions[key] not in fastjet_conf_tags.Strategy :
        _fastjetLog.error("Strategy \042%s\042 not recognized - fatal! Allowed values are: ",checkedOptions['Strategy'])
        for s in fastjet_conf_tags.Strategy :
            _fastjetLog.error("\042%s\042",s)
        raise Exception
    
    # check settings for RecombScheme
    if checkedOptions['RecombScheme'] not in fastjet_conf_tags.RecombScheme :
        _fastjetLog.error("RecombScheme \042%s\042 not recognized - fatal! Allowed values are: ",checkedOptions['RecombScheme'])
        for s in fastjet_conf_tags.RecombScheme :
            _fastjetLog.error("\042%s\042",s)
        raise Exception
    
    # check settings for Algorithm
    if checkedOptions['Algorithm'] not in fastjet_conf_tags.Algorithm :
        _fastjetLog.error("Algorithm \042%s\042 not recognized - fatal! Allowed values are: ",checkedOptions['Algorithm'])
        for s in fastjet_conf_tags.Algorithm :
            _fastjetLog.error("\042%%s\042",s)
        raise Exception

    # check settings for JetAreaMethod
    if checkedOptions['JetAreaMethod'] not in fastjet_conf_tags.JetAreaMethod :
        _fastjetLog.error("JetAreaMethod \042%s\042 not recognized - fatal! Allowed values are: ",checkedOptions['JetAreaMethod'])
        for s in fastjet_conf_tags.JetAreaMethod :
            _fastjetLog.error("\042%s\042",s)
        raise Exception

    # check settings for SIS split merge scale
    if checkedOptions['SIS_SplitMergeScale'] not in fastjet_conf_tags.SISSplitMergeScale :
      _fastjetLog.error("SIS_SplitMergeScale \042%2\042 not recognized - fatal! Allowed values are: ",checkedOptions['SIS_SplitMergeScale'])
      for s in fastjet_conf_tags.SISSplitMergeScale :
        _fastjetLog.error("\042%s\042",s)
      raise Exception

    return checkedOptions
            

def getFastJetInterfaceConfig(name,**options):
    # get tool configuration 
    fjTool = FastJetInterfaceTool(name)
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += fjTool
    # check job options
    options = checkAndUpdate(**options)    
    # set tool properties
    for k,v in options.iteritems():
        if k not in config_ignored_keys.ControlKeys :
            setattr(fjTool,k,v)
    # return tool configuration object
    return fjTool

#def dumpFastJetInterfaceConfig(**options=**defFastJetInterfaceConfigDict):
#    # write out all attributes
#    for k,v in options.iteritems():
#        _fastjetLog.message("Config::%s value %s",%(k),%(v))
    
