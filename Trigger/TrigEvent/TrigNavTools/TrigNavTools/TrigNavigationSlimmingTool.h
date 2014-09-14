/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLT__TRIG_NAVIGATION_SLIMMING_TOOL_H
#define HLT__TRIG_NAVIGATION_SLIMMING_TOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "TrigNavigation/TriggerElement.h"
#include "TrigDecisionTool/TrigDecisionTool.h"


#include <vector>

class IThinningSvc;

namespace HLT {

   class Navigation;

   static const InterfaceID IID_TrigNavigationSlimmingTool("HLT::TrigNavigationSlimmingTool", 1, 0);

  /**
   * @brief The TrigNavigationSlimmingTool is an athena tool used to slim the
   *        trigger Navigation structure in various ways
   * 
   * @author Ben Smith <bcsmith@fas.harvard.edu> - Harvard University
   *
   * This tool allows users to slim the trigger Navigation structure, which is
   * important when creating derived datasets, as the structure can grow very
   * large (> 30k per event).  
   *
   * There are three main slimming operations that are implemented via this tool:
   *
   * (a) TriggerElement squeezing: this removes all intermediate trigger elements
   *     (those which are not the initial node, an RoI node, or a terminal node)
   *     This can be performed via the removeIntermediateTriggerElements function
   *     defined below.
   *
   * (b) Feature Removal: this removes all links to the specified features from
   *     the trigger elements in the structure.  Note that it does not remove the
   *     holder from the navigation structure.
   *     This can be performed via the removeFeatures function defined below.
   *     Features are indicated for removal either through an inclusion list
   *     (only those in the list are included) or an exclusion list (all but those
   *     in the list are included).
   *
   * (c) Branch Removal: this removes all specified RoI nodes from the structure.
   *     This can be performed via the removeBranches function defined below.  As
   *     with feature removal, branches are indicated for removal either through an
   *     inclusion list or an exclusion list.
   *
   * Additionally, if there is a specific trigger element which needs to be removed,
   * this can be done via the removeTriggerElementFromNavigation function.  Note that
   * this will remove all relations with this element, block it from being serialized,
   * and propagate the relations down the tree (so that, for example, the nodes it
   * seeds now are seeded by the node which seeded it).
   */
   
   class TrigNavigationSlimmingTool : public AthAlgTool {

      public:

         TrigNavigationSlimmingTool( const std::string& type,
               const std::string& name,
               const IInterface* parent );
         virtual ~TrigNavigationSlimmingTool();

         static const InterfaceID& interfaceID() {
            return IID_TrigNavigationSlimmingTool;
         }

         virtual StatusCode intialize();
         virtual StatusCode finalize();
	 StatusCode doSlimming();


	 /**
          * @brief Returns a map from feature names to the number of times they appear in
          *        the structure.  Note that this map must be deleted by whatever calls the
          *        function.
          */
         virtual std::map<std::string, int> *getFeatureOccurrences(HLT::NavigationCore* navigation,
                                                                  TriggerElement *te = 0);
	 	 
   private:
	 ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;
	 ServiceHandle<IThinningSvc> m_thinningSvc;
	 
	 // job option configurable
	 int m_seenEvent; // keep track if we've seen an event yet
	 std::string m_resultKey; // key for the HLT result
	 bool m_writeTree; // write the tree back to the HLT result?
	 bool m_reloadNavigation; // refresh the navigation in place 
	 bool m_printTree; // print the tree to output?
	 bool m_doSqueeze; // perform trigger element squeezing?
	 bool m_protectChains; // if true, the final element of each chain will be kept when squeezing
	 bool m_removeGhosts; // remove ghost trigger elements?
	 bool m_removeFeatureless; // remove trigger elements that have no features?
	 bool m_protectOtherStreams; // add all streams not in exclusion to inclusion
	 bool m_removeFailedChains; // remove te's from failed chains
	 bool m_removeEmptyRoIs; // remove RoI's with no te's
	 std::vector<std::string> m_groupInclusionList; // guaranteed to be included
	 std::vector<std::string> m_groupExclusionList; // will try to remove
	 std::vector<std::string> m_streamInclusionList; // guaranteed to be included
	 std::vector<std::string> m_streamExclusionList; // will try to remove
	 std::vector<std::string> m_chainInclusionList; // guaranteed to be included
	 std::vector<std::string> m_chainExclusionList; // will try to remove
	 
	 std::vector<std::string> m_featureInclusionList;
	 std::vector<std::string> m_featureExclusionList;
	 std::set<std::string>    m_featureKeepSet; //!< computed from above
	 std::set<std::string>    m_featureDropSet; //!< computed from above
	 
	 std::vector<std::string> m_branchInclusionList;
	 std::vector<std::string> m_branchExclusionList;

	 bool m_dropNavigation;
	 
	 // inclusion and exclusion chain groups
	 const Trig::ChainGroup *m_inclusionChainGroup;
	 const Trig::ChainGroup *m_exclusionChainGroup;
	 std::vector<std::string> m_configuredChainNames;

         /** 
          * @brief Removes all trigger elements between te and the terminal trigger element from
          *        the navigation structure.  The terminal node is never removed, but te is removed 
          *        if it is not an ROI element.
          */
         virtual StatusCode removeIntermediateTriggerElements(HLT::NavigationCore* navigation, 
                                                              TriggerElement *te = 0,
                                                              std::vector<TriggerElement*> *tesToProtect = 0);

         /**
          * @brief Removes references to features from the navigation structure.  Features are
          *        specified via the inclusion and exclusion lists.  If the inclusion list is
          *        specified, only elements on that list are included (the exclusion list is
          *        ignored).  If the inclusion list is NULL or empty, then all elements except
          *        those on the exclusion list are included.
          */
         virtual StatusCode removeFeatures(HLT::NavigationCore* navigation,
                                           const std::set<std::string>& keepSet,
                                           const std::set<std::string>& dropSet);

         /**
          * @brief Removes branches from the navigation structure.  Branches are specified via the
          *        inclusion and exclusion lists.  If the inclusion list is specified, only elements
          *        on that list are included (the exclusion list is ignored).  If the inclusion list
          *        is NULL or empty, then all elements except those on the exclusion list are
          *        included.
          */
         virtual StatusCode removeBranches(HLT::NavigationCore* navigation,
                                           std::vector<std::string> *inclusionList,
                                           std::vector<std::string> *exclusionList = 0);

         /**
          * @brief Removes the passed trigger element from the navigation structure by removing
          *        all references to it
          */
         virtual StatusCode removeTriggerElementFromNavigation(HLT::NavigationCore* navigation,
                                                               TriggerElement *te,
                                                               bool propagateFeatures = true);

         /** 
          * @brief Removes trigger elements from the navigation.  If the inclusion list is specified
          *        only elements on that list are included (the exclusion list is ignored).  If the
          *        inclusion list is NULL or empty, then all elements except those on the exclusion
          *        list are included
          */
         virtual StatusCode removeTriggerElementsFromNavigation(HLT::NavigationCore* navigation,
                                                                std::vector<HLT::TriggerElement*> *inclusionList,
                                                                std::vector<HLT::TriggerElement*> *exclusionList,
                                                                HLT::TriggerElement *te = 0,
                                                                bool propagateFeatures = true);

         /** 
          * @brief Removes all trigger elements with the flag ghost set to true from the
          *        navigation structure
          */
         virtual StatusCode removeGhostTriggerElements(HLT::NavigationCore* navigation,
                                                                TriggerElement *te = 0);

         /**
          * @brief Removes all trigger elements with no features from the navigation
          *        structure
          */
         virtual StatusCode removeFeaturelessTriggerElements(HLT::NavigationCore* navigation,
                                                                 TriggerElement *te = 0);

         /**
          * @brief Returns a list of all feature labels in the structure.  Note that this list
          *        must be deleted by whatever calls the function.
          */
         virtual std::vector<std::string> *getAllFeatureNames(HLT::NavigationCore* navigation,
                                                              TriggerElement *te = 0);

         
         /**
          * @brief Returns a list of all branch labels in the structure.  Note that this list
          *        must be deleted by whatever calls the function.
          */
         virtual std::vector<std::string> *getAllBranchNames(HLT::NavigationCore* navigation);

         /**
          * @brief Returns the number of times the passed feature appears in the structure.
          */
         virtual int countFeature(HLT::NavigationCore* navigation, std::string *name, TriggerElement *te = 0);

         /**
          * @brief Returns a vector of strings corresponding to the labels of the features
          *        associated with the passed trigger element
          */
         virtual std::vector<std::string> *featureLabels(const HLT::TriggerElement *te);

         /**
          * @brief Used to compare pointers of TriggerElements.  To establish equality of
          *        trigger elements, we check (a) that their id's match and (b) that they
          *        come from the same RoI.  To check that the RoI is the same, we check
          *        check the RoI features, as these will never be slimmed (unless the
          *        whole branch is removed).
          */
         class TriggerElementFind {
           public:
            TriggerElementFind(const TriggerElement *te);
            bool operator()( const HLT::TriggerElement* te );

           private:
            unsigned int m_id;
            const TriggerElement *m_RoI;
         };

	 /**
	  * @brief reset indexes in the after the thinning
	  **/
	 
	 StatusCode adjustIndicesAfterThinning(HLT::NavigationCore* navigation, 
					       ServiceHandle<IThinningSvc>& service );







         /**
          * @brief Removes the passed te and all children from the navigation structure
          *
          */
         virtual StatusCode recursivelyRemoveNodesFromNavigation(HLT::NavigationCore* navigation,
                                                                 TriggerElement *te);

         /**
          * @brief This is a helper function for removeFeatures(HLT::NavigationCore*, ...).
          *        Note that that function should be instead, because it not only removes
          *        the features from the tree, but also cleans them up within the navigation
          *        structure.
          */
         virtual StatusCode removeFeatures( HLT::NavigationCore* navigation,
					    const std::set<std::pair<CLID, uint16_t> >& doDelete);
         /**
          * @brief Removes all instances of the supplied TriggerElement from the supplied vector.
          * Returns the number of elements removed.
          *
          */
         virtual int removeTriggerElementFromVector(TriggerElement *te, std::vector<TriggerElement*> *v);

         /**
          * @brief Returns true if the TriggerElement should be included in the navigation tree
          *        and false if it should not be.
          */
         bool toBeIncluded(TriggerElement *te, std::vector<std::string> *inclusionList, 
             std::vector<std::string> *exclusionList);
         bool toBeIncluded(TriggerElement *te, std::vector<TriggerElement*> *inclusionList, 
             std::vector<TriggerElement*> *exclusionList);

         /**
          * @brief Returns the label for the given feature
          */
         std::string getLabel(const TriggerElement::FeatureAccessHelper &fah) const;

         /**
          * @brief Propagates the features on given TE to its children
          */
         void propagateFeaturesToChildren(const TriggerElement *te);

         // store the navigation structure being worked on locally so it doesn't have
         // to be passed around everywhere
         HLT::NavigationCore* m_navigation; 

         // store the CLID and subtype ids of all the of the deleted features so we can
         // remove their holders from the navigation structure.
         std::vector<std::pair<CLID, uint16_t> > *m_deletedFeatures;

	 // internal functions
	 virtual std::vector<HLT::TriggerElement*> *getTEsFromFailedChains();
	 virtual std::vector<HLT::TriggerElement*> *getTEsFromChainGroup(const Trig::ChainGroup *cg);
	 // internal data


   }; // end TrigNavigationSlimmingTool declaration

} // end HLT namespace

#endif
