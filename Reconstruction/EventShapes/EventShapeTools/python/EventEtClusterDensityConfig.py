# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###############################################################################
# Configuration options for EventEtDensityCluster tool                        #
############################################################################### 

# -- Preliminaries

from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Logging import logging

from JetRec.FastJetInterfaceConfig import *

from EventShapeTools.EventShapeToolsConf import EventEtDensityClusterTool

_EventEtDensityLogger = logging.getLogger("EventEtDensityConfiguration")

defaultEtDensityClusterConfigDict = {
    # EventEtDensityProviderBase configuration
    'InputCollectionKey'  : "CaloCalTopoCluster",
    'FastJetTool'         : getFastJetInterfaceConfig("ClusterSequenceArea"),
    'EtaMin'              : -5.,
    'EtaMax'              :  5.,
    'SignalState'         : "CALIBRATED",
    # EventEtDensityClusterTool specials
    'ClusterPtMin'        : 0.,
    'UseAreaFourMomentum' : False
    }

def getDefaultEtDensityClusterDict():
    return defaultEtDensityClusterConfigDict


def configEtDensityCluster(name,**userOptions):
    # copy default options
    options = dict(getDefaultEtDensityClusterDict())
    # update with user options
    options.update(userOptions)
    # update tool configuration
    tool = EventEtDensityClusterTool(name)
    for k,v in options.iteritems():
        setattr(tool,k,v)
    # collect FastJet tool
    tool += options['FastJetTool']
    # ToolSvc
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += tool

    return tool

###############################################################################
# Pileup Suppression in Jets                                                  #
###############################################################################

defaultJetAreaConfigDict = {
    # -- FastJet rho calculation configuration: jet algorithm
    'Algorithm'             : "kt",
    'Radius'                : 0.3,
    'InclusivePtMin'        : 0*GeV,
    # -- FastJet rho calculation configuration: area calculation
    'CalculateJetArea'      : True,
    'JetAreaMethod'         : "VoronoiArea",
    'VoronoiEffectiveRfact' : 1.0,
    # -- EventEtDensity configuration
    'EtaMin'                : getDefaultEtDensityClusterDict()['EtaMin'],
    'EtaMax'                : getDefaultEtDensityClusterDict()['EtaMax'],
    'InputCollectionKey'    : getDefaultEtDensityClusterDict()['InputCollectionKey'],
    'SignalState'           : getDefaultEtDensityClusterDict()['SignalState'],
    'FastJetTool'           : getDefaultEtDensityClusterDict()['FastJetTool'],
    'ClusterPtMin'          : getDefaultEtDensityClusterDict()['ClusterPtMin'],
    'UseAreaFourMomentum'   : getDefaultEtDensityClusterDict()['UseAreaFourMomentum']
}

def configureJetAreaDensity(name,**userOptions):
    # copy default options
    options = dict(defaultJetAreaConfigDict)
    # update with user options
    options.update(userOptions)
    # configure FastJetInterfaceTool
    fjoptions = dict(defFastJetInterfaceConfigDict)
    # configure EventEtDensityClusterTool
    etoptions = dict(getDefaultEtDensityClusterDict())
    # check for updates
    for k,v in options.iteritems():
        # _EventEtDensityLogger.info("analysing JetAreaDensity dict at <"+k+">, user value: ")
        # _EventEtDensityLogger.info(v)
        # - FastJet configuration tag
        if k in fjoptions.keys():
            #            print "updating FastJet Dictionary"
            #            print fjoptions[k]
            #            print options[k]
            #            print v
            fjoptions[k] = v
            #            print fjoptions[k]
        # -- EventEtDensityCluster configuration tag
        if k in etoptions.keys():
            etoptions[k] = v
    # configure tools
    _EventEtDensityLogger.info(fjoptions)
    _EventEtDensityLogger.info(etoptions)
    fjname = name+'FastJetTool'
    etoptions['FastJetTool'] = getFastJetInterfaceConfig(fjname,**fjoptions);
    _EventEtDensityLogger.info("Final FastJet configuration:")
    for k,v in fjoptions.iteritems():
        print "                    ",k,v
    _EventEtDensityLogger.info("Final EventEtDensityCluster configuration:")
    for k,v in etoptions.iteritems():
        print "                    ",k,v
    ettool = configEtDensityCluster(name,**etoptions) 
    # --
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += ettool

    return ettool
            
        
