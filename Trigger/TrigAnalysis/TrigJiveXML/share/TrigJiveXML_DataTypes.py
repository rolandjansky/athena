#Avoid multiple includes
include.block("TrigJiveXML/TrigJiveXML_DataTypes.py")

# Include the base options if the user didn't already do that
if not "theEventData2XML" in dir():
    include ("JiveXML/JiveXML_jobOptionBase.py")

### Include ROI retrievers using low-level decoding flags
#--- EmTau ROI
# old pre-xAOD format, obsolete in rel.20
##theEventData2XML.DataTypes += ["JiveXML::EmTauROIRetriever/EmTauROIRetriever"]

#--- Jet ROI
# old pre-xAOD format, obsolete in rel.20
##theEventData2XML.DataTypes += ["JiveXML::JetROIRetriever/JetROIRetriever"]

#--- Muon ROI
# old pre-xAOD format, obsolete in rel.20
##theEventData2XML.DataTypes += ["JiveXML::MuonROIRetriever/MuonROIRetriever"]

#--- LVL1 trigger towers
# old pre-xAOD format, obsolete in rel.20
##theEventData2XML.DataTypes += ["JiveXML::LVL1TrigTowerRetriever/LVL1TrigTowerRetriever"]

#--- LVL1 jet elements
# old pre-xAOD format, obsolete in rel.20
##theEventData2XML.DataTypes += ["JiveXML::LVL1JetElementRetriever/LVL1JetElementRetriever"]

#--- LVL1 result from TrigDecision
theEventData2XML.DataTypes += ["JiveXML::LVL1ResultRetriever/LVL1ResultRetriever"]
from TrigJiveXML.TrigJiveXMLConf import JiveXML__LVL1ResultRetriever
theLVL1ResultRetriever = JiveXML__LVL1ResultRetriever (name = "LVL1ResultRetriever")
##theLVL1ResultRetriever.OutputLevel = DEBUG
ToolSvc += theLVL1ResultRetriever 

#--- LVL1 result from CTPDecision (for AOD)
#### obsolete, taken out.
#theEventData2XML.DataTypes += ["JiveXML::CTPDecisionRetriever/CTPDecisionRetriever"]

#--- TriggerInfo (Etmiss, etc)
theEventData2XML.DataTypes += ["JiveXML::TriggerInfoRetriever/TriggerInfoRetriever"]
from TrigJiveXML.TrigJiveXMLConf import JiveXML__TriggerInfoRetriever
theTriggerInfoRetriever = JiveXML__TriggerInfoRetriever (name = "TriggerInfoRetriever")
##theTriggerInfoRetriever.OutputLevel = DEBUG
ToolSvc += theTriggerInfoRetriever 

#--- InDet Trigger info
##theEventData2XML.DataTypes += ["JiveXML::TrigSiSpacePointRetriever/TrigSiSpacePointRetriever"]
#
###--- removed temporarily due to unresolved issues
###--- with multiple collections.   jpt 11Feb11
#theEventData2XML.DataTypes += ["JiveXML::TrigInDetTrackRetriever/TrigInDetTrackRetriever"]

# new xAOD retrievers
theEventData2XML.DataTypes += ["JiveXML::xAODEmTauROIRetriever/xAODEmTauROIRetriever"]
from TrigJiveXML.TrigJiveXMLConf import JiveXML__xAODEmTauROIRetriever
thexAODEmTauROIRetriever = JiveXML__xAODEmTauROIRetriever (name = "xAODEmTauROIRetriever")
##thexAODEmTauROIRetriever.OutputLevel = DEBUG
ToolSvc += thexAODEmTauROIRetriever 

theEventData2XML.DataTypes += ["JiveXML::xAODJetROIRetriever/xAODJetROIRetriever"]
from TrigJiveXML.TrigJiveXMLConf import JiveXML__xAODJetROIRetriever
thexAODJetROIRetriever = JiveXML__xAODJetROIRetriever (name = "xAODJetROIRetriever")
##thexAODJetROIRetriever.OutputLevel = DEBUG
ToolSvc += thexAODJetROIRetriever 

theEventData2XML.DataTypes += ["JiveXML::xAODMuonROIRetriever/xAODMuonROIRetriever"]
from TrigJiveXML.TrigJiveXMLConf import JiveXML__xAODMuonROIRetriever
thexAODMuonROIRetriever = JiveXML__xAODMuonROIRetriever (name = "xAODMuonROIRetriever")
##thexAODMuonROIRetriever.OutputLevel = DEBUG
ToolSvc += thexAODMuonROIRetriever 

theEventData2XML.DataTypes += ["JiveXML::xAODTriggerTowerRetriever/xAODTriggerTowerRetriever"]
from TrigJiveXML.TrigJiveXMLConf import JiveXML__xAODTriggerTowerRetriever
thexAODTriggerTowerRetriever = JiveXML__xAODTriggerTowerRetriever (name = "xAODTriggerTowerRetriever")
thexAODTriggerTowerRetriever.OutputLevel = DEBUG                                     
ToolSvc += thexAODTriggerTowerRetriever

### Not enabled by default, still skeleton without valid outputs !
#theEventData2XML.DataTypes += ["JiveXML::xAODTrigDecisionRetriever/xAODTrigDecisionRetriever"]
#from TrigJiveXML.TrigJiveXMLConf import JiveXML__xAODTrigDecisionRetriever
#thexAODTrigDecisionRetriever = JiveXML__xAODTrigDecisionRetriever (name = "xAODTrigDecisionRetriever")
###thexAODTrigDecisionRetriever.OutputLevel = DEBUG
#ToolSvc += thexAODTrigDecisionRetriever 


