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

   class NavigationCore;

   static const InterfaceID IID_TrigNavigationSlimmingTool("HLT::TrigNavigationSlimmingTool", 1, 0);

  /**
   * @brief The TrigNavigationSlimmingTool is an athena tool used to slim the
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
   * of operations (and the order) whit which they are executed in the slimming process.
   * The main method to execute is doSlimming - the vector given as an argument is 
   * filled whit serialized and slimmed content of the navigation.
   * !!! go back !!! we need to remove holders from slimmed navigation as they are becoming now the biggest
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

         virtual StatusCode initialize();
         virtual StatusCode finalize();
	 StatusCode doSlimming( std::vector<uint32_t>& slimmed_and_serialzied );


         std::map<std::string, int>* getFeatureOccurrences(HLT::NavigationCore* navigation);

	 	 
   private:
	 ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;
	 ServiceHandle<IThinningSvc> m_thinningSvc;
	 
	 // job option configurable
	 int m_seenEvent; // keep track if we've seen an event yet

	 std::vector<std::string> m_featureInclusionList;
	 std::vector<std::string> m_featureExclusionList;
	 std::set<std::string>    m_featureKeepSet; //!< computed from above
	 std::set<std::string>    m_featureDropSet; //!< computed from above

	 std::vector<std::string> m_actions;
	 typedef StatusCode (TrigNavigationSlimmingTool::*Action)();
	 std::map<std::string, Action> m_actionsMap;	 
	 std::string m_chainsRegex;
	 
	 
	 bool m_report; //!< TE operations verbosity flag

	 /**
	  * configures at the first event
	  */
	 StatusCode lateFillConfiguration();


         /**
          * @brief clear the result of the sliming in the doSliming argument (vector<uint32_t>)
	  * Makes no sense to combine wiht other options
          */
	 StatusCode drop();
         /**
          * @brief Reload the slimmied navigation in TDT so that all clients of current job see the chage
          */
	 StatusCode reload();

         /**
          * @brief Restore the original navigation structure
          */
	 StatusCode restore();

         /**
          * @brief Save the result of the sliming in the doSliming argument (vector<uint32_t>)
          */

	 StatusCode save();

	 StatusCode print();
         /**
          * @brief Remove intermediate TEs leaving very flat structure wiht event node, rois and terminals
          */
	 StatusCode squeeze();

         /**
          * @brief Removes references to features from the navigation structure.  Features are
          *        specified via the inclusion and exclusion lists.  If the inclusion list is
          *        specified, only elements on that list are included (the exclusion list is
          *        ignored).  If the inclusion list is NULL or empty, then all elements except
          *        those on the exclusion list are included.
          */
	 StatusCode dropFeatures();
         /**
          * @brief Removes RoI nodes, rather agresive option, should be use as one of last actions as it 
	  * makes impossible to traverse the tree. Nonetheless the tree can be to soem extent usable in the analysis
	  * i.e. when no TEs traversing is realy needed
          */
	 StatusCode dropRoIs();
         /**
          * @brief Removes RoI nodes, which do not seed anything
          */
	 StatusCode dropEmptyRoIs();
         /**
          * @brief Removes TEs which have no features (combine wiht squeeze)
          */
	 StatusCode dropFeatureless();
         /**
          * @brief Removes TEs which are inactive (rejected by hypothesis)
          */
	 StatusCode dropInactive();

	 /**
	  * @brief reset indexes in the after the thinning
	  **/	 
	 StatusCode syncThinning();

	 /**
	  * @brief remove info not related to the specified chains
	  */
	 StatusCode dropChains();

         /**
          * @brief Removes the passed trigger element from the navigation structure by removing
          *        all references to it in seeded and seeding TEs, 
	  * @param propagateFeatures decides if features need to be moved to children TEs
          */
         StatusCode removeTriggerElement(TriggerElement *te,
					 bool propagateFeatures = true);


         /** 
          * @brief Removes all trigger elements with the flag ghost set to true from the
          *        navigation structure
          */
         StatusCode removeGhostTriggerElements(TriggerElement *te = 0);

         /**
          * @brief Removes all trigger elements with no features from the navigation
          *        structure
          */
         StatusCode removeFeaturelessTriggerElements(TriggerElement *te = 0);

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
          * @brief Removes the passed te and all children from the navigation structure
          *
          */
         StatusCode recursivelyRemoveNodesFromNavigation(TriggerElement *te);

         /**
          * @brief This is a helper function for removeFeatures(HLT::NavigationCore*, ...).
          *        Note that that function should be instead, because it not only removes
          *        the features from the tree, but also cleans them up within the navigation
          *        structure.
          */
	 StatusCode removeFeatures( const std::set<std::pair<CLID, uint16_t> >& doDelete);

         /**
          * @brief This is a helper function for removeFeatures(HLT::NavigationCore*, ...).
	  * Either this or the above is caled depending which of the sets is smaller 
	  * @warning this is a bit more agresive as it cleans up the navigation from transient objects
          */
	 StatusCode retainFeatures(const std::set<std::pair<CLID, uint16_t> >& toRetain);

         /**
          * @brief Removes all instances of the supplied TriggerElement from the supplied vector.
          * Returns the number of elements removed.
          *
          */
	 StatusCode removeTriggerElementFromVector(TriggerElement *te, std::vector<TriggerElement*>& v);

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
         StatusCode propagateFeaturesToChildren(const TriggerElement *te);

         // store the navigation structure being worked on locally so it doesn't have
         // to be passed around everywhere
         HLT::NavigationCore* m_navigation; 
	 std::vector<uint32_t> m_originalNavigation; 

	 std::vector<uint32_t>* m_destinationNavigation; 
	 std::set<HLT::te_id_type> m_tesToProtect;
	 
         // store the CLID and subtype ids of all the of the deleted features so we can
         // remove their holders from the navigation structure.
         //std::vector<std::pair<CLID, uint16_t> > *m_deletedFeatures;

	 // internal functions
	 //	 virtual std::vector<HLT::TriggerElement*> *getTEsFromFailedChains();
	 //	 virtual std::vector<HLT::TriggerElement*> *getTEsFromChainGroup(const Trig::ChainGroup *cg);
	 // internal data



   }; // end TrigNavigationSlimmingTool declaration

} // end HLT namespace

#endif
