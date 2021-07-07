/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigDecisionTool/FeatureRequestDescriptor.h"

namespace Trig {


  FeatureRequestDescriptor::FeatureRequestDescriptor(const std::string& chainGroupName,
      const unsigned int condition,
      const std::string& containerSGKey,
      const unsigned int featureCollectionMode,
      const std::string& navElementLinkKey,
      const int restrictToLegIndex) {
    setChainGroup(chainGroupName);
    setCondition(condition);
    setRequireSGKey(containerSGKey);
    setFeatureCollectionMode(featureCollectionMode);
    setLinkName(navElementLinkKey);
    setRestrictRequestToLeg(restrictToLegIndex);
  }


  FeatureRequestDescriptor::FeatureRequestDescriptor(const std::string& chainGroupName) {
    reset();
    setChainGroup(chainGroupName);
  }


  void FeatureRequestDescriptor::reset() {
    m_chainGroupName = "HLT_.*";
    m_condition = TrigDefs::Physics;
    m_containerSGKey = "";
    m_featureCollectionMode = TrigDefs::lastFeatureOfType;
    m_navElementLinkKey = TrigCompositeUtils::featureString();
    m_restrictToLegIndex = -1;
  }


  void FeatureRequestDescriptor::print(MsgStream& msg, const MSG::Level level) {
    msg << level << "Feature Request Descriptor configuration:" << endmsg;
    msg << level << "-- Chain Group name:'" << chainGroup() << "'." << endmsg;
    /////////////////////////
    if (condition() == TrigDefs::Physics) {
      msg << level << "-- Condition: TrigDefs::Physics, only paths which accepted chains in the ChainGroup will be explored." << endmsg;
    } else if (condition() == TrigDefs::includeFailedDecisions) {
      msg << level << "-- Condition: TrigDefs::includeFailedDecisions, passed and failed paths for chains in the ChainGroup will be explored." << endmsg;
    } else {
      msg << MSG::ERROR << "-- Unknown condition: " << condition() << endmsg;
    }
    /////////////////////////
    if (SGKey().empty()) {
      msg << level << "-- No filtering on Container StoreGate Key" << endmsg;
    } else {
      msg << level << "-- Container StoreGate Key: ElementLinks must regex match the container name '" << SGKey() << "'." << endmsg;
    }
    /////////////////////////
    if (featureCollectionMode() == TrigDefs::lastFeatureOfType) {
      msg << level << "-- Feature Collection Mode: TrigDefs::lastFeatureOfType, only one (final) matching ElementLink will be returned on each path through the navigation." << endmsg;
    } else if (featureCollectionMode() == TrigDefs::allFeaturesOfType) {
      msg << level << "-- Condition: TrigDefs::includeFailedDecisions, all matching ElementLinks will be returned on each path through the navigation." << endmsg;
    } else {
      msg << MSG::ERROR << "-- Unknown feature collection mode: " << featureCollectionMode() << "." << endmsg;
    }
    /////////////////////////
    msg << level << "-- Link Name: ElementLinks referenced by graph edges named '" << linkName() << "' will be returned." << endmsg;
    /////////////////////////
    if (restrictRequestToLeg() == -1) {
      msg << level << "-- Restrict Request To Leg: ElementLinks will be collected from all legs of any multi-leg chains in the ChainGroup." << endmsg;
    } else {
      msg << level << "-- Restrict Request To Leg: ElementLinks will be collected only from leg #" << restrictRequestToLeg() << " of all chains in the ChainGroup." << endmsg;
    }
  }


  FeatureRequestDescriptor& FeatureRequestDescriptor::setChainGroup(const std::string& chainGroupName) {
    m_chainGroupName = chainGroupName;
    return *this;
  }


  FeatureRequestDescriptor& FeatureRequestDescriptor::setCondition(const unsigned int condition) {
    if (condition != TrigDefs::Physics and condition != TrigDefs::includeFailedDecisions) {
      throw std::runtime_error("Invalid option supplied to FeatureRequestDescriptor::setCondition. Must be Physics or includeFailedDecisions.");
    }
    m_condition = condition;
    return *this;
  }


  FeatureRequestDescriptor& FeatureRequestDescriptor::setRequireSGKey(const std::string& containerSGKey) {
    m_containerSGKey = containerSGKey;
    return *this;
  }


  FeatureRequestDescriptor& FeatureRequestDescriptor::setFeatureCollectionMode(const unsigned int featureCollectionMode) {
    if (featureCollectionMode != TrigDefs::lastFeatureOfType and featureCollectionMode != TrigDefs::allFeaturesOfType) {
      throw std::runtime_error("Invalid option supplied to FeatureRequestDescriptor::featureCollectionMode. Must be lastFeatureOfType or allFeaturesOfType.");
    }
    m_featureCollectionMode = featureCollectionMode;
    return *this;
  }


  FeatureRequestDescriptor& FeatureRequestDescriptor::setLinkName(const std::string& navElementLinkKey) {
    m_navElementLinkKey = navElementLinkKey;
    return *this;
  }


  FeatureRequestDescriptor& FeatureRequestDescriptor::setRestrictRequestToLeg(const int restrictToLegIndex) {
    m_restrictToLegIndex = restrictToLegIndex;
    return *this;
  }


  const std::string& FeatureRequestDescriptor::chainGroup() const {
    return m_chainGroupName;
  }


  unsigned int FeatureRequestDescriptor::condition() const {
    return m_condition;
  }


  const std::string& FeatureRequestDescriptor::SGKey() const {
    return m_containerSGKey;
  }


  unsigned int FeatureRequestDescriptor::featureCollectionMode() const {
    return m_featureCollectionMode;
  }


  const std::string& FeatureRequestDescriptor::linkName() const {
    return m_navElementLinkKey;
  }


  int FeatureRequestDescriptor::restrictRequestToLeg() const {
    return m_restrictToLegIndex;
  }


}
