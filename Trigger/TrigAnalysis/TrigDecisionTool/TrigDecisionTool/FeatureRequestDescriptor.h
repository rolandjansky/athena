// -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_DECISION_TOOL_FEATUREREQUESTDESCRIPTOR_H
#define TRIGGER_DECISION_TOOL_FEATUREREQUESTDESCRIPTOR_H

/**********************************************************************************
 * @Project: TrigDecisionTool
 * @Package: TrigDecisionTool
 * @class  : FeatureRequestDescriptor
 *
 * @brief Helper class to encapsulate all possible configuration options when making a
 * request for trigger chain features using the TrigDecisionTool's run 3 interface.
 *
 ***********************************************************************************/
#include <string>
#include <sstream>

#include "AsgMessaging/MsgStream.h"
#include "TrigDecisionTool/Conditions.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"


namespace Trig {

  class FeatureRequestDescriptor {

   public:

    /**
     * @brief Default Constructor supplying all properties. See below for individual property descriptions.
     **/
    FeatureRequestDescriptor(const std::string& chainGroupName = "HLT_.*",
      const unsigned int condition = TrigDefs::Physics,
      const std::string& containerSGKey = "",
      const unsigned int featureCollectionMode = TrigDefs::lastFeatureOfType,
      const std::string& navElementLinkKey = TrigCompositeUtils::featureString(),
      const int restrictToLegIndex = -1);

    /**
     * @brief Constructor for a specific Chain or Chain Group, supplied by name. Regex supported.
     **/
    FeatureRequestDescriptor(const std::string& chainGroupName);


    /**
     * @brief Default destructor. No heap allocations.
     **/
    ~FeatureRequestDescriptor() = default;

    /**
     * @brief Default move constructor.
     **/
    FeatureRequestDescriptor(FeatureRequestDescriptor&&) = default; 

    /**
     * @brief Default copy constructor.
     **/
    FeatureRequestDescriptor(const FeatureRequestDescriptor&) = default;  

    /**
     * @brief Reset the FeatureRequestDescriptor to its default configuration.
     **/
    void reset();

    /**
     * @brief Print the configuration of the FeatureRequestDescriptor to the supplied message stream.
     **/
    void print(MsgStream& msg, const MSG::Level level = MSG::INFO);

    /**
     * @brief Set the desired Chain or Chain Group. Regex supported.
     * @return reference to self, allows chaining.
     **/
    FeatureRequestDescriptor& setChainGroup(const std::string& chainGroupName);

    /**
     * @brief Set the Condition:
     * TrigDefs::Physics - (default), only returns features from paths through the navigation which accepted the event 
     *   for at least one of the chains in the ChainGroup.
     * TrigDefs::includeFailedDecisions - follows more paths through the navigation, the paths which were active for at
     *   least one of the chains in the ChainGroup, but which failed for all of these chains by some non-final Step. 
     * @return reference to self, allows chaining.
     **/
    FeatureRequestDescriptor& setCondition(const unsigned int condition);

    /**
     * @brief Set the StoreGate key filter. ElementLinks will have their StoreGate collection key checked against
     * this filter, it must match for the ElementLink to be collected. Regex supported. 
     * Set to an empty string (default) to disable the filter. 
     * @return reference to self, allows chaining.
     **/
    FeatureRequestDescriptor& setRequireSGKey(const std::string& containerSGKey);

    /**
     * @brief Set the Feature Collection Mode:
     * TrigDefs::lastFeatureOfType - (default) stop exploring up each path through the navigation as soon as one ElementLink
     *   has been collected along the path. As all paths are explored from their last step backwards, this equates to collecting
     *   the "final" feature along each path (after accounting for all other filtering options). 
     * TrigDefs::allFeatureOfType - does not stop once an ElementLink is collected when exploring paths through the navigation, instead
     *   keeps exploring always back to the L1 node. Potentially returns many features along each path, corresponding to intermediate
     *   physics objects as well as the "final" physics object.
     * @return reference to self, allows chaining.
     **/
    FeatureRequestDescriptor& setFeatureCollectionMode(const unsigned int featureCollectionMode);

    /**
     * @brief Set the Link Name Key. Sets which named-edges in the navigation graph are returned by the call.
     * By default the TrigDecisionTool looks for edges named "feature", however "roi", "initialRoI" or slice-specific
     * custom-named edges can be alternatively used here.
     * @return reference to self, allows chaining.
     **/
    FeatureRequestDescriptor& setLinkName(const std::string& navElementLinkKey);

    /**
     * @brief Set to -1 by default, indicating that all legs of multi-leg chains are searched.
     * If set to an integer >= 0, the search will be restricted to the single specified leg for all chains in the ChainGroup.
     * @return reference to self, allows chaining.
     **/
    FeatureRequestDescriptor& setRestrictRequestToLeg(const int restrictToLegIndex);

    /**
     * @return String corresponding to the chain group. Will be mapped to an actual ChainGroup by the TrigDecisionTool.
     **/
    const std::string& chainGroup() const;

    /**
     * @return The Condition, TrigDefs::Physics or TrigDefs::includeFailedDecisions
     **/
    unsigned int condition() const;

    /**
     * @return The StoreGate key filter (regex). Or an empty string if no filtering is requested.
     **/
    const std::string& SGKey() const;

    /**
     * @return The feature collection mode, TrigDefs::lastFeatureOfType or TrigDefs::allFeatureOfType
     **/
    unsigned int featureCollectionMode() const;

    /**
     * @return The name of the edges in the navigation graph which will be collected and returned.
     **/
    const std::string& linkName() const;

    /**
     * @return The chain-leg which the request is restricted to follow. -1 indicates no leg-specific filtering.
     **/
    int restrictRequestToLeg() const;

   private:

    std::string m_chainGroupName;
    int m_condition;
    std::string m_containerSGKey;
    int m_featureCollectionMode;
    std::string m_navElementLinkKey;
    int m_restrictToLegIndex;

  };
} // End of namespace

#endif // TRIGGER_DECISION_TOOL_FEATUREREQUESTDESCRIPTOR_H
