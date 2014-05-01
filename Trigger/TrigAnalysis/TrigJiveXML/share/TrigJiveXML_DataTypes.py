#Avoid multiple includes
include.block("TrigJiveXML/TrigJiveXML_DataTypes.py")

# Include the base options if the user didn't already do that
if not "theEventData2XML" in dir():
    include ("JiveXML/JiveXML_jobOptionBase.py")

### Include ROI retrievers using low-level decoding flags
#--- EmTau ROI
theEventData2XML.DataTypes += ["JiveXML::EmTauROIRetriever/EmTauROIRetriever"]

#--- Jet ROI
theEventData2XML.DataTypes += ["JiveXML::JetROIRetriever/JetROIRetriever"]

#--- Muon ROI
theEventData2XML.DataTypes += ["JiveXML::MuonROIRetriever/MuonROIRetriever"]

#--- LVL1 trigger towers
theEventData2XML.DataTypes += ["JiveXML::LVL1TrigTowerRetriever/LVL1TrigTowerRetriever"]

#--- LVL1 jet elements
theEventData2XML.DataTypes += ["JiveXML::LVL1JetElementRetriever/LVL1JetElementRetriever"]

#--- LVL1 result from TrigDecision
theEventData2XML.DataTypes += ["JiveXML::LVL1ResultRetriever/LVL1ResultRetriever"]

#--- LVL1 result from CTPDecision (for AOD)
#### obsolete, taken out.
#theEventData2XML.DataTypes += ["JiveXML::CTPDecisionRetriever/CTPDecisionRetriever"]


#--- TriggerInfo (Etmiss, etc)
theEventData2XML.DataTypes += ["JiveXML::TriggerInfoRetriever/TriggerInfoRetriever"]

#--- InDet Trigger info
theEventData2XML.DataTypes += ["JiveXML::TrigSiSpacePointRetriever/TrigSiSpacePointRetriever"]

###--- removed temporarily due to unresolved issues
###--- with multiple collections.   jpt 11Feb11
#theEventData2XML.DataTypes += ["JiveXML::TrigInDetTrackRetriever/TrigInDetTrackRetriever"]

# new xAOD retrievers
theEventData2XML.DataTypes += ["JiveXML::xAODEmTauROIRetriever/xAODEmTauROIRetriever"]
