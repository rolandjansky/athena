/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CLASS_TRIG_SLIM_VALIDATION_H
#define CLASS_TRIG_SLIM_VALIDATION_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigNavigation/TriggerElement.h"
#include "TrigNavTools/TrigNavigationSlimmingTool.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include <iostream>

/**
  * @brief The TrigSlimValAlg is used in validating the TrigNavigationSlimming
  *        tools.
  * @author Ben Smith <bcsmith@fas.harvard.edu> - Harvard University
  *
  * This algorithm allows validation of the trigger navigation slimming tools
  * in the TrigEvent/TrigNavTools package.  The basic idea is to print many of
  * the aggregate prorperties of the navigation structure, and compare them to
  * the expected results before and after slimming.  For example, when removing
  * a feature, the other features should be remain unchanged.
  *
  * In practice, the log files are parsed by a python script: trigslimval.py which
  * can be found in the TrigValTools package in the bin directory
  */

class TrigSlimValAlg : public AthAlgorithm {

  public:
    TrigSlimValAlg(const std::string& name, ISvcLocator *pSvcLocator);
    ~TrigSlimValAlg() { ; };

    StatusCode initialize();
    StatusCode clear();
    StatusCode execute();
    StatusCode finalize();

    /** 
      *  @brief Abstract base class for TriggerElement function objects.
      *         These are used to count various attributes of the trigger elements
      *         in the navigation structure
      */
    class ElementCounter {
      public:
        ElementCounter(HLT::NavigationCore* nav) { m_nav = nav; };
        virtual ~ElementCounter() {;};
        virtual int count(const HLT::TriggerElement* /* te */) const = 0;
      protected:
        HLT::NavigationCore* m_nav;
    };

    /**
      * @brief Simply counts the number of elements in the tree
      */
    class NumberCounter : public ElementCounter {
      public:
        NumberCounter(HLT::NavigationCore* nav) : ElementCounter(nav) {;};
        virtual int count(const HLT::TriggerElement* /* te */) const { return 1; };
    };

    /**
      * @brief Counts only intermediate (non-RoI, terminal, or inital) elements
      */
    class IntermediateCounter : public ElementCounter {
      public:
        IntermediateCounter(HLT::NavigationCore* nav) : ElementCounter(nav) {;};
        virtual int count(const HLT::TriggerElement *te) const { 
          return ( te && m_nav ? !(m_nav->isInitialNode(te) || 
                m_nav->isRoINode(te) || m_nav->isTerminalNode(te)) : 0 );
        }
      
    };

    /**
      * @brief Counts only elements with no feature links
      */
    class FeaturelessCounter : public ElementCounter {
      public:
        FeaturelessCounter(HLT::NavigationCore* nav) : ElementCounter(nav) {;};
        int count(const HLT::TriggerElement *te) const {
          return ( te ? te->getFeatureAccessHelpers().size() == 0 : 0 );
        }
    };

    /**
      * @brief Counts only elements which are marked as ghosts
      */
    class GhostCounter : public ElementCounter {
      public:
        GhostCounter(HLT::NavigationCore* nav) : ElementCounter(nav) {;};
        int count(const HLT::TriggerElement *te) const {
          return ( te ? te->ghost() : 0 );
        }
    };

    /**
      * @brief Counts only elements which are RoI elements
      */
    class RoICounter : public ElementCounter {
      public:
        RoICounter(HLT::NavigationCore* nav) : ElementCounter(nav) {;};
        int count(const HLT::TriggerElement *te) const {
          return ( te && m_nav ? m_nav->isRoINode(te) : 0 );
        }
    };

    /**
      * @brief Counts the number of seeds relations in all the elements
      */
    class SeedsRelationCounter : public ElementCounter {
      public:
        SeedsRelationCounter(HLT::NavigationCore* nav) : ElementCounter(nav) {;};
        int count(const HLT::TriggerElement *te) const {
          return ( te ? te->getRelated( HLT::TriggerElement::seedsRelation ).size() : 0 );
        }
    };
          
    /**
      * @brief Counts the number of seededBy relations in all the elements
      */
    class SeededByRelationCounter : public ElementCounter {
      public:
        SeededByRelationCounter(HLT::NavigationCore* nav) : ElementCounter(nav) {;};
        int count(const HLT::TriggerElement *te) const {
          return ( te ? te->getRelated( HLT::TriggerElement::seededByRelation ).size() : 0 );
        }
    };
    
    /**
      * @brief Counts the number of same RoI relations in all the elements
      */
    class SameRoIRelationCounter : public ElementCounter {
      public:
        SameRoIRelationCounter(HLT::NavigationCore* nav) : ElementCounter(nav) {;};
        int count(const HLT::TriggerElement *te) const {
          return ( te ? te->getRelated( HLT::TriggerElement::sameRoIRelation ).size() : 0 );
        }
    };

    /**
      * @brief Counts the number of feature links in all the elements
      */
    class FeatureCounter : public ElementCounter {
      public:
        FeatureCounter(HLT::NavigationCore* nav) : ElementCounter(nav) {;};
        int count(const HLT::TriggerElement *te) const {
          return ( te ? te->getFeatureAccessHelpers().size() : 0 );
        }
    };

    /** 
     * @brief Update function objects - used by the recursive counting program.  By cominging
     *         these with the ElementCounter objects, you can count whatever you want in the tree
     */
    class CountUpdate {
      public:
        CountUpdate() {;};
        virtual ~CountUpdate() {;};
        virtual int update(const int orig, const int val) const = 0;
    };

    /**
      * @brief The SumUpdate class is used when you wish to compile the sum of all occurrences.
      *        For example: when counting all the trigger elements in the structure.
      */
    class SumUpdate : public CountUpdate {
      public:
        SumUpdate() : CountUpdate() {;};
        int update(const int orig, const int val) const {
          return ( orig < 0 ? val : orig + val );
        }
    };

    /**
      * @brief The MinUpdate class is used when you wish to find the minimum value of something
      *        in the tree. For example, when generating the shrtest branch.
      */
    class MinUpdate : public CountUpdate {
      public:
        MinUpdate() : CountUpdate() {;};
        int update(const int orig, const int val) const {
          return ( orig <= 0 || val < orig ? val : orig );
        }
    };

    /**
      * @brief The MaxUpdate class is used when you wish to find the maximum value of something
      *        in the tree. For example, when generating the longest branch.
      */
    class MaxUpdate : public CountUpdate {
      public:
        MaxUpdate() : CountUpdate() {;};
        int update(const int orig, const int val) const {
          return ( val > orig ? val : orig );
        }
    };

    /**
      * @brief Defines equivalence of Trigger Elements.  When they have the same id, they
      *        represent the same threshold in the trigger decision chain (but possibly in
      *        different RoI's, etc.)
      */
    class TriggerElementCompare {
      public:
        TriggerElementCompare(unsigned int id) : m_id(id) { ; };
        bool operator()(const HLT::TriggerElement *te) {
          return te && te->getId() == m_id;
        }
      private:
        unsigned int m_id;
    };

    /**
      * @brief defines ordering of Trigger Elements to be used for set ordering.
      */
    class TriggerElementLessThan {
      public:
        TriggerElementLessThan() {;};
        bool operator()(const HLT::TriggerElement *te1, const HLT::TriggerElement *te2) const {
          return te1 < te2;
        }
    };

    typedef std::set< const HLT::TriggerElement*, TrigSlimValAlg::TriggerElementLessThan > elementSet;

  private:

    // private functions
    /**
      * @brief Basic counting function.  This recursively searches the tree and computes
      *        the value of the Elementcount on each trigger element in the tree.  To combine
      *        two values, it uses the CountUpdate function object.
      */
    int recursiveCount(HLT::NavigationCore *nav, ElementCounter *ec, CountUpdate *cu, HLT::TriggerElement *te = 0);

    /**
      * @brief Returns a set of all the trigger elements in the navigation structure.
      */
    TrigSlimValAlg::elementSet *getAllTriggerElements(HLT::NavigationCore* nav, HLT::TriggerElement *te = 0);

    /**
      * @brief Returns a set of the trigger elements associated with the chain group
      */
    TrigSlimValAlg::elementSet *getTEsFromChainGroup(const Trig::ChainGroup* cg);

    // tools
    ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool; //<! TrigDecisionTool
    ToolHandle<HLT::TrigNavigationSlimmingTool> m_slimmingTool; //<! Navigation Slimming Tool
    
    // internal data
    std::vector<std::string> m_configuredGroups; //<! cache group->chain map
    std::vector<std::string> m_configuredStreams;//<! cache stream->chain map
    int m_eventSeen; //<! used to initialize variables that need to have an event loaded
};

#endif
