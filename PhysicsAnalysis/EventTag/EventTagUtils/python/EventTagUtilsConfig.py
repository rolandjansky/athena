# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from EventTagUtils.EventTagUtilsConf import *


class EventTagUtils_trigAODConfig( GlobalTriggerTagTool ):
    __slots__ = []
    def __init__(self, name="EventTagUtils_trigAODConfig"):
        super( EventTagUtils_trigAODConfig, self ).__init__( name )

    def setDefaults(self, handle):
        pass
        ###   if not hasattr( handle, 'TrigDecisionTool' ) :
        ###       from TrigDecision.TrigDecisionConfig import TrigDecisionTool_AOD
        ###       handle.TrigDecisionTool = TrigDecisionTool_AOD('TrigDecisionTool')


class EventTagUtils_trigXMLConfig( GlobalTriggerTagTool ):
    __slots__ = []
    def __init__(self, name="EventTagUtils_trigXMLConfig"):
        super(  EventTagUtils_trigXMLConfig, self ).__init__( name )

    def setDefaults(self, handle):
        pass
        ###   if not hasattr( handle, 'TrigDecisionTool' ) :
        ###       from TrigDecision.TrigDecisionConfig import TrigDecisionTool_XMLConfig
        ###       handle.TrigDecisionTool = TrigDecisionTool_XMLConfig('TrigDecisionTool_xml')
