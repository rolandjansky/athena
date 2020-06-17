// -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_DECISION_TOOL_DecisionAccess_H
#define TRIGGER_DECISION_TOOL_DecisionAccess_H

/**********************************************************************************
 * @Project: TrigDecisionTool
 * @Package: TrigDecisionTool
 * @class  : DecisionAccess
 *
 * @brief access to information about the trigger decision
 *
 * @author Michael Begel   <michael.begel@cern.ch>  - Brookhaven National Laboratory
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch> - LAPP Annecy
 * @author Tomasz Bold     <Tomasz.Bold@cern.ch>    - UC Irvine - AGH Krakow
 *
 ***********************************************************************************/
#include <string>
#include <sstream>
#include <boost/algorithm/string/predicate.hpp>

#include "TrigNavStructure/TriggerElement.h"
#include "TrigDecisionTool/Logger.h"
#include "TrigDecisionTool/Conditions.h"
#include "TrigDecisionTool/Combination.h"
#include "TrigDecisionTool/Feature.h"
#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/ChainGroup.h"

#include "TrigCompositeUtils/TrigCompositeUtils.h"

namespace HLT {
  class Chain;
}
namespace TrigConf {
  class HLTChain;
  class HLTTriggerElement;
}

class LVL1_ROI;

namespace Trig {

  class CacheGlobalMemory;
   
  class DecisionAccess : public virtual Logger {
  public:      

    DecisionAccess();
    virtual ~DecisionAccess();

    /**
     * @brief true if given group of chains passed
     * @see Trig::ChainGroup
     * @see TrigDefs::Conditions
     **/ 
    bool isPassed(const Trig::ChainGroup* chaingroup, 
                  unsigned int condition = TrigDefs::Physics) const;
    /**
     * @brief true if given chain passed
     * @see Trig::ChainGroup
     * @see TrigDefs::Conditions
     **/ 
    bool isPassed(const std::string& chain, 
                  unsigned int condition) const;

    bool isPassed(const std::string& chain) const;
    /**
     * Return expert-level information about the various trigger flags as a
     * bit mask. Note: this does not accept a conditions flag -- the
     * prescription is always enforceLogicalFlow. The mask can be comprehended
     * using the following constants from TrigDefs::
     * EF_passedRaw
     * EF_passThrough
     * EF_prescaled
     * EF_resurrected
     * L2_passedRaw
     * L2_passThrough
     * L2_prescaled
     * L2_resurrected
     * L1_isPassedAfterPrescale
     * L1_isPassedBeforePrescale
     * L1_isPassedAfterVeto
     **/
    unsigned int isPassedBits(const Trig::ChainGroup* chaingroup) const;
    unsigned int isPassedBits(const std::string& chain) const;
    
    /**
     * @brief get the bunch group code
     * bit X=1: BG X fired (X=0..7)
     * use: bool BGXfired = BGCode & (0x1<<X)
     */
    char getBGCode() const;
    
    /**
     * @brief Runs 1, 2. Returns all features related to given chain group
     **/
    FeatureContainer features(const Trig::ChainGroup* group, 
                              unsigned int condition = TrigDefs::Physics) const;

    /**
     * @brief Runs 1, 2. Returns features related to given chain
     **/
    FeatureContainer features(const std::string& chainName = "EF_.*", 
                              unsigned int condition = TrigDefs::Physics) const;

    /// @name Run 3 functions
    /// @{

    /**
     * @brief Runs 3+. Returns all features related to given chain group
     * @param[in] group Chain group to return features for.
     * @param[in] condition Condition requirement. Only TrigDefs::Physics and TrigDefs::includeFailedDecisions are supported.
     * @param[in] containerSGKey Optional requirement to return only features within the specified container name. Not checked if not specified. 
     * @param[in] featureCollectionMode For lastFeatureOfType, stop exploring each route through the navigation once one matching feature has been found.
     * @param[in] navElementLinkKey Optional name of element link as saved online. The "feature" link is enforced, others may have been added. 
     * @return Vector of LinkInfo, where each entry wraps an ElementLink to the feature, and the Decision object it came from.
     **/
    template<class CONTAINER>
    std::vector< TrigCompositeUtils::LinkInfo<CONTAINER> > 
    features(const Trig::ChainGroup* group,
             const unsigned int condition = TrigDefs::Physics,
             const std::string& containerSGKey = "",
             const unsigned int featureCollectionMode = TrigDefs::lastFeatureOfType,
             const std::string& navElementLinkKey = "feature") const;

    /**
     * @brief Runs 3+. Returns features related to given chain
     * @param[in] group Chain group to return features for.
     * @param[in] condition Condition requirement. Only TrigDefs::Physics and TrigDefs::includeFailedDecisions are supported.
     * @param[in] containerSGKey Optional requirement to return only features within the specified container name. Not checked if not specified. 
     * @param[in] featureCollectionMode For lastFeatureOfType, stop exploring each route through the navigation once one matching feature has been found.
     * @param[in] navElementLinkKey Optional name of element link as saved online. The "feature" link is enforced, others may have been added. 
     * @return Vector of LinkInfo, where each entry wraps an ElementLink to the feature, and the Decision object it came from.
     **/
    template<class CONTAINER>
    std::vector< TrigCompositeUtils::LinkInfo<CONTAINER> >
    features(const std::string& chainGroupName,
             const unsigned int condition = TrigDefs::Physics,
             const std::string& containerSGKey = "",
             const unsigned int featureCollectionMode = TrigDefs::lastFeatureOfType,
             const std::string& navElementLinkKey = "feature") const;

    /**
     * @brief Runs 3+. Returns a range over a container which are associated with a particular EventView instance from online.
     * Instance mapping done via an ElementLink<TrigRoiDescriptorCollection> obtained from the supplied LinkInfo
     * @param[in] inViewContainer The ReadHandle of the collection which was produced online inside an EventView.
     * @param[in] linkInfo LinkInfo from which a TrigRoiDescriptor can be located.
     * @param[in] roiName Name of the TrigRoiDescriptor inside the Navigation. 
     * @return Pair of iterators spanning a range of indices over the collection accessed through the ReadHandleKey
     **/
    template<class CONTAINER, class FEATURE_CONTAINER>
    std::pair< typename CONTAINER::const_iterator, typename CONTAINER::const_iterator > 
    associateToEventView(SG::ReadHandle<CONTAINER>& inViewContainer,
                         const TrigCompositeUtils::LinkInfo<FEATURE_CONTAINER> linkInfo,
                         const std::string& roiName = TrigCompositeUtils::initialRoIString()) const;

    /**
     * @brief Runs 3+. Returns a range over a container which are associated with a particular EventView instance from online.
     * Instance mapping done via an ElementLink<TrigRoiDescriptorCollection> obtained from the supplied Decision object pointer
     * @param[in] inViewContainer The ReadHandle of the collection which was produced online inside an EventView.
     * @param[in] decisionObject Decision node from which a TrigRoiDescriptor can be located.
     * @param[in] roiName Name of the TrigRoiDescriptor inside the Navigation. 
     * @return Pair of iterators spanning a range of indices over the collection accessed through the ReadHandleKey
     **/
    template<class CONTAINER>
    std::pair< typename CONTAINER::const_iterator, typename CONTAINER::const_iterator > 
    associateToEventView(SG::ReadHandle<CONTAINER>& inViewContainer,
                         const TrigCompositeUtils::Decision* decisionObject,
                         const std::string& roiName = TrigCompositeUtils::initialRoIString()) const;

    /**
     * @brief Runs 3+. Returns a range over a container which are associated with a particular EventView instance from online.
     * Instance mapping done via supplied ElementLink<TrigRoiDescriptorCollection>.
     * @param[in] inViewContainer The ReadHandle of the collection which was produced online inside an EventView.
     * @param[in] roi TrigRoiDescriptor used to seed the desired EventView.
     * @return Pair of iterators spanning a range of indices over the collection accessed through the ReadHandleKey
     **/
    template<class CONTAINER>
    std::pair< typename CONTAINER::const_iterator, typename CONTAINER::const_iterator > 
    associateToEventView(SG::ReadHandle<CONTAINER>& inViewContainer,
                         const ElementLink<TrigRoiDescriptorCollection>& matchROI) const;

    /**
     * @brief Runs 3+. Returns a range over a container which are associated with a particular EventView instance from online.
     * Instance mapping done via matchIndex and optional matchKey (leave matchKey = 0 to not cut on this).
     * @param[in] inViewContainer The ReadHandle of the collection which was produced online inside an EventView.
     * @param[in] matchIndex The index of the desired EventView.
     * @param[in] matchKey Optional. SGKey of the index of the desired EventView (collection hosting the ROI used to span the Event View)
     * @param[in] isFullscan Optional. If true, and inViewContainer has no viewIndex decorations, then return iterators over the full
     *                       span of the inViewContainer instead of throwing an exception.
     *                       This allows the associateToEventView interface to be used also for non-EventView containers, 
     *                       with the FS ROI used to indicate that the lack of decorations is expected rather than being
     *                       indicative of a configuration problem.
     * @return Pair of iterators spanning a range of indices over the collection accessed through the ReadHandleKey
     **/
    template<class CONTAINER>
    std::pair< typename CONTAINER::const_iterator, typename CONTAINER::const_iterator > 
    associateToEventView(SG::ReadHandle<CONTAINER>& inViewContainer,
                         const uint32_t matchIndex,
                         const uint32_t matchKey = 0,
                         const bool isFullscan = false) const;

    /// @}

    /**
     * @brief gives back feature matching (by seeding relation)
     * @param te - is trigger element to start with, not that thanks to conversion operators
     * Feature object can be given here as well
     **/
    template<class T> 
    const Feature<T> ancestor(const HLT::TriggerElement* te, std::string label = "") const;

    /**
     * @brief gives back features matching (by seeding relation) - will also flatten
     *        containers if necessary
     * @param te - is trigger element to start with, not that thanks to conversion operators
     * Feature object can be given here as well
     **/
    template<class T>
    const std::vector<Trig::Feature<T> > ancestors(const HLT::TriggerElement* te, std::string label = "", unsigned int condition=TrigDefs::Physics, const std::string& teName = "") const;

  protected:

    friend class Combination;    


  private:

    virtual Trig::CacheGlobalMemory* cgm() const =0; 

    //    template<class T> 
    //    const T* getL1RoI(const std::vector<T>& rois, unsigned int id) const;


    //unsigned int initRoIWord(const HLT::TriggerElement* te) const;

  };
} // End of namespace

#include "DecisionAccess.icc"

#endif
