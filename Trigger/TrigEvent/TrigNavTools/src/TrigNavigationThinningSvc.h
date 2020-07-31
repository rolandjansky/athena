// -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGNAVTOOLS_TRIGNAVIGATIONTHINNINGSVC_H
#define TRIGNAVTOOLS_TRIGNAVIGATIONTHINNINGSVC_H

#include "GaudiKernel/IIncidentListener.h"
#include "TrigNavigation/TriggerElement.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/ITrigNavigationThinningSvc.h"


namespace HLT {
  class NavigationCore;
}


/**
 * @brief The TrigNavigationThinningSvc is an athena tool used to slim the
 *        trigger Navigation structure in various ways
 * 
 * @author Ben Smith <bcsmith@fas.harvard.edu> - Harvard University
 * major rework Tomasz Bold 
 *
 * This tool allows users to slim the trigger Navigation structure, which is
 * important when creating derived datasets, as the structure can grow very
 * large (> 30k per event).  
 *
 * There are three main slimming operations that are implemented via this tool:
 *
 * (a) TriggerElement squeezing: this removes all intermediate trigger elements
 *     (those which are not the initial node, an RoI node, or a terminal node)
 *
 * (b) Feature Removal: this removes all links to the specified features from
 *     the trigger elements in the structure.  Note that it does not remove the
 *     holder from the navigation structure.
 *
 * (c) Remove the TEs related to certain chains
 *   
 * (d) Complete removal
 *
 * and few other actions
 *
 * All operations on the navigation are configured via the Actions property which specifies set
 * of operations (and the order) with which they are executed in the slimming process.
 * The main method to execute is doSlimming - the vector given as an argument is 
 * filled whti serialized and slimmed content of the navigation.
 * !!! go back !!! we need to remove holders from slimmed navigation as they are becoming now the biggest
 */
class TrigNavigationThinningSvc : public extends<AthService, ITrigNavigationThinningSvc>
{
public:
  TrigNavigationThinningSvc( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  
  virtual StatusCode doSlimming( const EventContext& /*ctx*/,
                                 std::vector<uint32_t>& slimmed_and_serialized ) const override;
  

private:
  ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;
	 
  std::vector<std::string> m_featureInclusionList;
  std::vector<std::string> m_featureExclusionList;
  std::set<std::string>    m_featureKeepSet; //!< computed from above
  std::set<std::string>    m_featureDropSet; //!< computed from above

  struct State
  {
    State (const EventContext& the_ctx,
           HLT::NavigationCore& the_navigation,
           std::vector<uint32_t>& the_destinationNavigation)
      : ctx (the_ctx),
        navigation (the_navigation),
        destinationNavigation (the_destinationNavigation)
    {}
    const EventContext& ctx;
    HLT::NavigationCore& navigation;
    std::vector<uint32_t>& destinationNavigation;
    std::vector<uint32_t> originalNavigation;
    std::set<HLT::te_id_type> tesToProtect;
  };

  std::vector<std::string> m_actions;
  typedef StatusCode (TrigNavigationThinningSvc::*Action)(State& state) const;
  std::map<std::string, Action> m_actionsMap;	 
  std::string m_chainsRegex;
	 
	 
  bool m_report; //!< TE operations verbosity flag

  /**
   * configures at the first event
   */
  StatusCode lateFillConfiguration(State& state) const;


  /**
   * @brief clear the result of the slimming in the doSliming argument (vector<uint32_t>)
   * Makes no sense to combine with other options
   */
  StatusCode drop(State& state) const;
  /**
   * @brief Reload the slimmed navigation in TDT so that all clients of current job see the chage
   */
  StatusCode reload(State& state) const;

  /**
   * @brief Restore the original navigation structure
   */
  StatusCode restore(State& state) const;

  /**
   * @brief Save the result of the slimming in the doSlimming argument (vector<uint32_t>)
   */

  StatusCode save(State& state) const;

  StatusCode print(State& state) const;
  /**
   * @brief Remove intermediate TEs leaving very flat structure with event node, rois and terminals
   */
  StatusCode squeeze(State& state) const;

  /**
   * @brief Removes references to features from the navigation structure.  Features are
   *        specified via the inclusion and exclusion lists.  If the inclusion list is
   *        specified, only elements on that list are included (the exclusion list is
   *        ignored).  If the inclusion list is NULL or empty, then all elements except
   *        those on the exclusion list are included.
   */
  StatusCode dropFeatures(State& state) const;
  /**
   * @brief Removes RoI nodes, rather aggressive option, should be use as one of last actions as it 
   * makes impossible to traverse the tree. Nonetheless the tree can be to some extent usable in the analysis
   * i.e. when no TEs traversing is really needed
   */
  StatusCode dropRoIs(State& state) const;
  /**
   * @brief Removes RoI nodes, which do not seed anything
   */
  StatusCode dropEmptyRoIs(State& state) const;
  /**
   * @brief Removes TEs which have no features (combine wiht squeeze)
   */
  StatusCode dropFeatureless(State& state) const;
  /**
   * @brief Removes TEs which are inactive (rejected by hypothesis)
   */
  StatusCode dropInactive();

  /**
   * @brief reset indexes in the after the thinning
   **/	 
  StatusCode syncThinning(State& state) const;

  /**
   * @brief remove info not related to the specified chains
   */
  StatusCode dropChains(State& state) const;

  /**
   * @brief Removes the passed trigger element from the navigation structure by removing
   *        all references to it in seeded and seeding TEs, 
   * @param propagateFeatures decides if features need to be moved to children TEs
   */
  StatusCode removeTriggerElement(State& state,
                                  HLT::TriggerElement *te,
                                  bool propagateFeatures = true) const;


  /** 
   * @brief Removes all trigger elements with the flag ghost set to true from the
   *        navigation structure
   */
  StatusCode removeGhostTriggerElements(HLT::TriggerElement *te = 0);

  /**
   * @brief Removes all trigger elements with no features from the navigation
   *        structure
   */
  StatusCode removeFeaturelessTriggerElements(State& state, HLT::TriggerElement *te = 0);

  /**
   * @brief Used to compare pointers of TriggerElements.  To establish equality of
   *        trigger elements, we check (a) that their id's match and (b) that they
   *        come from the same RoI.  To check that the RoI is the same, we check
   *        check the RoI features, as these will never be slimmed (unless the
   *        whole branch is removed).
   */
  class TriggerElementFind {
  public:
    TriggerElementFind(const HLT::TriggerElement *te);
    bool operator()( const HLT::TriggerElement* te );

  private:
    unsigned int m_id;
    const HLT::TriggerElement *m_RoI;
  };

  /**
   * @brief Removes the passed te and all children from the navigation structure
   *
   */
  StatusCode recursivelyRemoveNodesFromNavigation(HLT::TriggerElement *te);

  /**
   * @brief This is a helper function for removeFeatures(HLT::NavigationCore*, ...).
   *        Note that that function should be instead, because it not only removes
   *        the features from the tree, but also cleans them up within the navigation
   *        structure.
   */
  StatusCode removeFeatures( State& state,
                             const std::set<std::pair<CLID, uint16_t> >& doDelete) const;

  /**
   * @brief This is a helper function for removeFeatures(HLT::NavigationCore*, ...).
   * Either this or the above is caled depending which of the sets is smaller 
   * @warning this is a bit more agresive as it cleans up the navigation from transient objects
   */
  StatusCode retainFeatures(State& state,
                            const std::set<std::pair<CLID, uint16_t> >& toRetain) const;

  /**
   * @brief Removes all instances of the supplied TriggerElement from the supplied vector.
   * Returns the number of elements removed.
   *
   */
  StatusCode removeTriggerElementFromVector(HLT::TriggerElement *te, std::vector<HLT::TriggerElement*>& v) const;

  /**
   * @brief Returns true if the TriggerElement should be included in the navigation tree
   *        and false if it should not be.
   */
  bool toBeIncluded(State& state, HLT::TriggerElement *te, std::vector<std::string> *inclusionList, 
                    std::vector<std::string> *exclusionList);
  bool toBeIncluded(State& state, HLT::TriggerElement *te, std::vector<HLT::TriggerElement*> *inclusionList, 
                    std::vector<HLT::TriggerElement*> *exclusionList);

  /**
   * @brief Propagates the features on given TE to its children
   */
  StatusCode propagateFeaturesToChildren(const HLT::TriggerElement *te) const;

  // store the CLID and subtype ids of all the of the deleted features so we can
  // remove their holders from the navigation structure.
  //std::vector<std::pair<CLID, uint16_t> > *m_deletedFeatures;

  // internal functions
  //	 virtual std::vector<HLT::TriggerElement*> *getTEsFromFailedChains();
  //	 virtual std::vector<HLT::TriggerElement*> *getTEsFromChainGroup(const Trig::ChainGroup *cg);
  // internal data
};


#endif
