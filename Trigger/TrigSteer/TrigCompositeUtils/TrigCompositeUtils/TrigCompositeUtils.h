/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigCompositeUtils_TrigCompositeUtils_h
#define TrigCompositeUtils_TrigCompositeUtils_h

#include <set>
#include <memory>
#include <functional>
#include <iostream>

#include "AthLinks/ElementLink.h"
#include "AthLinks/ElementLinkVector.h"
#include "AsgDataHandles/WriteHandle.h"
#include "AsgDataHandles/WriteHandleKey.h"
#include "AsgDataHandles/ReadHandleKey.h"
#include "AsgTools/CurrentContext.h"
#include "AsgTools/EventStoreType.h"
#include "AsgMessaging/MsgStream.h"
#include "AsgMessaging/MessageCheck.h"


#ifdef XAOD_STANDALONE
#include "AsgTools/SgTEvent.h"
#endif

#include "AthContainers/AuxElement.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"

#include "TrigDecisionInterface/Conditions.h"

#include "HLTIdentifier.h"
#include "NavGraph.h"

namespace TrigCompositeUtils {

  ANA_MSG_HEADER (msgRejected)

  /// alias types, for readability and to simplify future evolution
  typedef SG::WriteHandle<DecisionContainer> DecisionWriteHandle;

  /**
   * @brief Creates and right away records the Container CONT with the key.
   * Returns the WriteHandle. 
   * No Aux store.
   * If possible provide the context that comes via an argument to execute otherwise it will default to looking it up which is slower.
   **/
  template<class CONT>
    SG::WriteHandle<CONT> createAndStoreNoAux( const SG::WriteHandleKey<CONT>& key, const EventContext& ctx = Gaudi::Hive::currentContext());

  /**
   * @brief Creates and right away records the Container CONT with the key.
   * Returns the WriteHandle. 
   * With Aux store.
   * If possible provide the context that comes via an argument to execute otherwise it will default to looking it up which is slower.
   **/
  template<class CONT, class AUX>
    SG::WriteHandle<CONT> createAndStoreWithAux( const SG::WriteHandleKey<CONT>& key, const EventContext& ctx = Gaudi::Hive::currentContext());

  /**
   * @brief Creates and right away records the DecisionContainer with the key.
   * Returns the WriteHandle. 
   * If possible provide the context that comes via an argument to execute otherwise it will default to looking it up which is slower.
   **/
  SG::WriteHandle<DecisionContainer> createAndStore( const SG::WriteHandleKey<DecisionContainer>& key, const EventContext& ctx = Gaudi::Hive::currentContext() );

  /**
   * @brief Creates and right away records the DecisionContainer using the provided WriteHandle.
   **/
  void createAndStore( SG::WriteHandle<DecisionContainer>& handle );

  /**
   * @brief Helper method to create a Decision object, place it in the container and return a pointer to it.
   * This is to make this:
   * auto d = newDecisionIn(output);
   * instead of:
   * auto d = new Decision; 
   * output->push_back(d);    
   * If provided, the name is assigned to the TC object
   * Note that the supplied DecisionContainer must have been recorded in the event store.
   * If possible provide the context that comes via an argument to execute otherwise it will default to looking it up which is slower.
   **/  
  Decision* newDecisionIn ( DecisionContainer* dc, const std::string& name = "" );

  /**
   * @brief Helper method to create a Decision object, place it in the container and return a pointer to it. RoI, view and feature links will be copied from the previous to the new decision and a "seed" link made between them
   * @arg the container in which to place the new Decision
   * @arg the previous decision to which the new one should be connected
   * If provided, the name is assigned to the TC object
   * Note that the supplied DecisionContainer must have been recorded in the event store.
   **/ 
  Decision* newDecisionIn( DecisionContainer* dc, const Decision* dOld, const std::string& name = "", const EventContext& ctx = Gaudi::Hive::currentContext() );

  /**
   * @brief Appends the decision (given as ID) to the decision object
   * @warning Performing two decision insertions with the same ID consecutively results in the ID stored only once
   * This helps solving multiple inserts in the combo hypo tools
   **/
  void addDecisionID( DecisionID id,  Decision* d);

 /**
   * @brief Appends the decision IDs of src to the dest decision object
   * @warning Performing merging of IDs and solving the duples (via set)
   * This helps when making copies of Decision obejcts
   **/
  void insertDecisionIDs( const Decision* src, Decision* dest );

 /**
   * @brief Appends the decision IDs of src to the dest decision object
   * @warning Performing merging of IDs and solving the duples (via set)
   **/
  void insertDecisionIDs( const DecisionIDContainer& src, Decision* dest );

 /**
   * @brief Make unique list of decision IDs of dest Decision object
   * @warning Use vector->set->vector
   * This helps solving multiple inserts of the Decision obejcts
   **/
  void uniqueDecisionIDs( Decision* dest);

      
  /**
   * @brief Extracts DecisionIDs stored in the Decision object 
   **/
  void decisionIDs( const Decision* d, DecisionIDContainer& id );

  /**
   * @brief Another variant of the above method to access DecisionIDs stored in
   * the Decision object, returns const accessor
   * @warning Operates on the underlying xAOD vector form rather than the DecisionContainer set form
   **/
  const std::vector<DecisionID>& decisionIDs( const Decision* d ); 

  /**
   * @brief Another variant of the above method to access DecisionIDs stored in 
   * the Decision object, returns read/write accessor
   * @warning Operates on the underlying xAOD vector form rather than the DecisionContainer set form
   **/ 
  std::vector<DecisionID>& decisionIDs( Decision* d );

  
  /**
   * @brief return true if there is no positive decision stored
   **/
  bool allFailed( const Decision* d );
  
  /**
   * @brief Checks if any of the DecisionIDs passed in arg required is availble in Decision object
   **/
  bool isAnyIDPassing( const Decision* d,  const DecisionIDContainer& required);

  /**
   * @brief checks if required decision ID is in the set of IDs in the container
   **/
  bool passed( DecisionID id, const DecisionIDContainer& );

  /**
   * @brief Takes a raw pointer to a Decision and returns an ElementLink to the Decision. The Decision must already be in a container in SG.
   **/
  ElementLink<DecisionContainer> decisionToElementLink(const Decision* d, const EventContext& ctx = Gaudi::Hive::currentContext());
  
  /**
   * @brief Links to the previous object, location of previous 'seed' decision supplied by hand
   **/
  void linkToPrevious(Decision* d, const std::string& previousCollectionKey, size_t previousIndex);

  /**
   * @brief Links to the previous object, 'seed' decision provided explicitly.
   **/
  void linkToPrevious(Decision* d, const Decision* dOld, const EventContext& ctx = Gaudi::Hive::currentContext());

  /**
   * @brief checks if there is at least one 'seed' link to previous object
   **/
  bool hasLinkToPrevious(const Decision* d);

  /**
   * @brief returns links to previous decision object 'seed'
   **/
  const ElementLinkVector<DecisionContainer> getLinkToPrevious(const Decision*);

  /**
   * @brief copy all links from src to dest TC objects
   * @warning if there are links already in the dest TC, the operation is not performed and false returned
   * @ret true if success
   **/
  bool copyLinks(const Decision* src, Decision* dest);

  /**
   * @brief Generate the HLT::Identifier which corresponds to a specific leg of a given chain. This can be queried for its DecisionID.
   * @param chainIdentifier The HLT::Identifier corresponding to the chain.
   * @param counter The numeral of the leg.
   * @return HLT::Identifier corresponding to the specified leg. Call .numeric() on this to get the DecisionID.
   **/
  HLT::Identifier createLegName(const HLT::Identifier& chainIdentifier, size_t counter);

 /**
   * @brief Generate the HLT::Identifier which corresponds to the chain name from the leg name. This can be queried for its DecisionID.
   * @param chainIdentifier The HLT::Identifier corresponding to the specifci leg.
   * @return HLT::Identifier corresponding to the chain. Call .numeric() on this to get the DecisionID.
   **/
  HLT::Identifier getIDFromLeg(const HLT::Identifier& legIdentifier);
 
/**
   * @brief Recognise whether the chain ID is a leg ID
   * @param chainIdentifier The HLT::Identifier corresponding to the specifci ID.
   * @return True if leg-ID, else false
   **/
  bool isLegId(const HLT::Identifier& legIdentifier);

    
  /**
   * @brief traverses Decision object links for another Decision object fufilling the prerequisite specified by the filter
   * @return matching Decision object or nullptr
   **/
  const Decision* find(const Decision*, const std::function<bool(const Decision*)>& filter);

  /**
   * @brief Prerequisite object usable with @see filter allowing to find TC having a link to an object of name
   **/
  class HasObject {
  public:
    /**
     * @brief constructor specifying the link name
     **/
    HasObject(const std::string& name): m_name(name) {}
    /**
     * @brief checks if the arg TC has link of name specified at construction 
     * @warning method for internal consumption within @see find function 
     **/
    bool operator()(const Decision* ) const;
  private:
    std::string m_name;
  };

  /**
   * @brief Prerequisite object usable with @see filter allowing to find TC having a link to an object collection of name
   **/
  class HasObjectCollection {
  public:
    /**
     * @brief constructor specifying the link name
     **/
    HasObjectCollection(const std::string& name): m_name(name) {}
    /**
     * @brief checks if the arg Decision object has link collection of name specified at construction 
     * @warning method for internal consumption within @see find function 
     **/
    bool operator()(const Decision* ) const;
  private:
    std::string m_name;
  };

  /**
   * @brief Query all DecisionCollections in the event store, locate all Decision nodes in the graph where an object failed selection for a given chain.
   * @param[in] eventStore Pointer to event store within current event context
   * @param[in] id ID of chain to located failed decision nodes for. Passing 0 returns all decision nodes which failed at least one chain.
   * @return Vector of Decision nodes whose attached feature failed the trigger chain logic for chain with DecisionID id
   **/
  std::vector<const Decision*> getRejectedDecisionNodes(asg::EventStoreType* eventStore, const DecisionID id = 0);
  



  /**
   * @brief Search back in time from "node" and locate all paths back through Decision objects for a given chain.
   * @param[in] node The Decision object to start the search from. Typically this will be one of the terminus objects from the HLTNav_Summary.
   * @param[inout] navPaths Holds a sub-graph of the full navigation graph, filtered by DecisionID. An already partially populated graph may be provided as input.
   * @param[in] id Optional DecisionID of a Chain to trace through the navigation. If omitted, no chain requirement will be applied.
   * @param[in] enforceDecisionOnStartNode If the check of DecisionID should be carried out on the start node.
   * enforceDecisionOnStartNode should be true if navigating for a trigger which passed (e.g. starting from HLTPassRaw)
   * enforceDecisionOnStartNode should be false if navigating for a trigger which failed but whose failing start node(s) were recovered via getRejectedDecisionNodes
   **/
  void recursiveGetDecisions(const Decision* node, 
    NavGraph& navGraph, 
    const DecisionID id = 0,
    const bool enforceDecisionOnStartNode = true);


  /**
   * @brief Used by recursiveGetDecisions
   * @see recursiveGetDecisions
   * @param comingFrom The parent node which has a link in the navigation to this "node"
   **/
  void recursiveGetDecisionsInternal(const Decision* node, 
    const Decision* comingFrom,
    NavGraph& navGraph,
    const DecisionID id,
    const bool enforceDecisionOnNode);

  /**
   * @brief Additional information returned by the TrigerDecisionTool's feature retrieval, contained within the LinkInfo.
   **/
  enum ActiveState {
    UNSET, //!< Default property of state. Indicates that the creator of the LinkInfo did not supply this information
    ACTIVE, //!< The link was still active for one-or-more of the HLT Chains requested in the TDT
    INACTIVE //!< The link was inactive for all of the HLT Chains requested in the TDT. I.e. the object was rejected by these chains.
  };

  /**
   * @brief Helper to keep a Decision object, ElementLink and ActiveState (with respect to some requested ChainGroup) linked together (for convenience)
   **/
  template<typename T>
  struct LinkInfo {
    LinkInfo()
      : source{0} {}
    LinkInfo(const Decision* s, const ElementLink<T>& l, ActiveState as = ActiveState::UNSET)
      : source{s}, link{l}, state{as} {}

    bool isValid() const {
      return source && link.isValid();
    }
    /**
     * @brief helper conversion to make it usable with CHECK macro expecting StatusCode
     */
    operator StatusCode () {
      return (isValid() ? StatusCode::SUCCESS : StatusCode::FAILURE);
    }

    const Decision* source;
    ElementLink<T> link;
    ActiveState state;
  };

  /// @name Constant string literals used within the HLT
  /// @{
  const std::string& initialRoIString();
  const std::string& initialRecRoIString();
  const std::string& roiString();
  const std::string& viewString();
  const std::string& featureString();
  const std::string& seedString();
  /// @}

  /**
   * @brief Removes ElementLinks from the supplied vector if they do not come from the specified collection (sub-string match).
   * @param[in] containerSGKey The StoreGate key of the collection to match against. Performs sub-string matching. Passing "" performs no filtering.
   * @param[in,out] vector Mutible vector of ElementLinks on which to filter.
   **/
  template<class CONTAINER>
  void filterLinkVectorByContainerKey(const std::string& containerSGKey, ElementLinkVector<CONTAINER>& vector);

  /**
   * @brief Extract features from the supplied linkVector (obtained through recursiveGetDecisions).
   * @param[in] navPaths Sub-graph of the trigger navigation which is to be considered.
   * @param[in] lastFeatureOfType True for TrigDefs::lastFeatureOfType. stops at the first feature (of the correct type) found per path through the navigation.
   * @param[in] featureName Optional name of feature link as saved online. The "feature" link is enforced, others may have been added. 
   * @param[in] chains Optional set of Chain IDs which features are being requested for. Used to set the ActiveState of returned LinkInfo objects.
   * @return Typed vector of LinkInfo. Each LinkInfo wraps an ElementLink to a feature and a pointer to the feature's Decision object in the navigation.
   **/
  template<class CONTAINER>
  const std::vector< LinkInfo<CONTAINER> > recursiveGetFeaturesOfType( 
    const NavGraph& navGraph, 
    const std::string containerSGKey = "",
    const bool lastFeatureOfType = true,
    const std::string& navElementLinkKey = featureString(),
    const DecisionIDContainer chainIDs = DecisionIDContainer());

  /**
   * @see recursiveGetFeaturesOfType
   * @brief Internal implimentation called by recursiveGetFeaturesOfType, and by itself
   * @param[inout] features The untimate return vector. New links are to be appended.
   * @param[inout] fullyExploredFrom Cache of graph nodes which have been fully explored, and hence don't need exploring again should they show up.
   * @param[in] navGraphNode The current node in the navGraph which is being explored.
   **/
  template<class CONTAINER>
  void recursiveGetFeaturesOfTypeInternal(
    std::vector< LinkInfo<CONTAINER> >& features, 
    std::set<const NavGraphNode*>& fullyExploredFrom,
    const NavGraphNode* navGraphNode, 
    const std::string containerSGKey,
    const bool lastFeatureOfType,
    const std::string& navElementLinkKey,
    const DecisionIDContainer chainIDs);

  /**
   * @brief Perform a recursive search for ElementLinks of type T and name 'linkName', starting from Decision object 'start' 
   * For the case of multiple links, this function only returns the first one found. @see findLinks
   * @param[in] start the Decision Object from where recursive search should begin
   * @param[in] linkName the name of the ElementLink stored inside one or more DecisionObjects.
   * @param[in] suppressMultipleLinksWarning findLink will print a warning if more than one ElementLink is found, this can be silenced here.
   * @return LinkInfo A wrapper around an ElementLink and the location in the graph 
   */
  template<typename T>
  LinkInfo<T>
  findLink(const Decision* start, 
    const std::string& linkName, 
    const bool suppressMultipleLinksWarning = false);

  /**
   * @brief search back the TC links for the object of type T linked to the one of TC (recursively)
   * Populates provided vector with all located links to T of the corresponding linkName. 
   * @param[in] start the Decision Object from where recursive search should begin
   * @param[in] linkName the name of the ElementLink stored inside one or more DecisionObjects.
   * @param[inout] links Reference to vector, this will be populated with the found links. 
   * @param[in] behaviour TrigDefs::allFeaturesOfType to explore all branches of the navigation graph all the
                          way back to the L1 decoder, or TrigDefs::lastFeatureOfType to exit early from each
                          branch once a link has been located and collected. 
   * @param[inout] visitedCache Optional cache used by the recursive algorithm to avoid exploring each node multiple times. 
   */
  template<typename T>
  void
  findLinks(const Decision* start, 
    const std::string& linkName,
    std::vector<LinkInfo<T>>& links, 
    unsigned int behaviour = TrigDefs::allFeaturesOfType, 
    std::set<const xAOD::TrigComposite*>* visitedCache = nullptr);

  /**
   * @brief search back the TC links for the object of type T linked to the one of TC (recursively)
   * This version returns a vector rather than requiring that one be passed to it. 
   * @param[in] start the Decision Object from where recursive search should begin
   * @param[in] linkName the name of the ElementLink stored inside one or more DecisionObjects.
   * @param[in] behaviour TrigDefs::allFeaturesOfType to explore all branches of the navigation graph all the
                          way back to the L1 decoder, or TrigDefs::lastFeatureOfType to exit early from each
                          branch once a link has been located and collected. 
   * @return Vector with the found links. 
   */
  template<typename T>
  std::vector<LinkInfo<T>>
  findLinks(const Decision* start,
    const std::string& linkName,
    unsigned int behaviour = TrigDefs::allFeaturesOfType);

  /**
   * @brief Perform a recursive search for ElementLinks of any time and name 'linkName', starting from Decision object 'start'
   * Returns the link data in a typeless way, as raw key, index and CLID values. These may be reconstituted into a typed ElementLink.
   * For the case of multiple links, this function only returns the first one found. @see findLinks
   * @param[in] start the Decision Object from where recursive search should begin
   * @param[in] linkName the name of the ElementLink stored inside one or more DecisionObjects.
   * @param[out] key The storegate key (hash) of the located link's collection
   * @param[out] clid The class ID of the link's collection
   * @param[out] index The link's index inside its collection.
   * @param[in] suppressMultipleLinksWarning findLink will print a warning if more than one ElementLink is found, this can be silenced here.
   * @return True if a link was located
   */
  bool typelessFindLink(const Decision* start, 
    const std::string& linkName,
    uint32_t& key,
    uint32_t& clid,
    uint16_t& index,
    const bool suppressMultipleLinksWarning = false);

  /**
   * @brief search back the TC links for the object of type T linked to the one of TC (recursively)
   * Returns the link data in a typeless way, as raw key, index and CLID values. These may be reconstituted into a typed ElementLink.
   * Populates provided vectors with all located links of the corresponding linkName. 
   * @param[in] start the Decision Object from where recursive search should begin
   * @param[in] linkName the name of the ElementLink stored inside one or more DecisionObjects.
   * @param[inout] key The return vector of the storegate key (hash) of the located link's collection
   * @param[inout] clid The return vector of the class ID of the link's collection
   * @param[inout] index The return vector of the link's index inside its collection.
   * @param[in] behaviour TrigDefs::allFeaturesOfType to explore all branches of the navigation graph all the
                          way back to the L1 decoder, or TrigDefs::lastFeatureOfType to exit early from each
                          branch once a link has been located and collected. 
   * @param[inout] visitedCache Optional cache used by the recursive algorithm to avoid exploring each node multiple times. 
   */
  bool typelessfindLinks(const Decision* start, 
    const std::string& linkName,
    std::vector<uint32_t>& key,
    std::vector<uint32_t>& clid,
    std::vector<uint16_t>& index,
    const unsigned int behaviour = TrigDefs::allFeaturesOfType, 
    std::set<const xAOD::TrigComposite*>* visitedCache = nullptr);


  /**
   * Prints the Decision object including the linked seeds
   * @warnign expensive call
   **/  
  std::string dump( const Decision*  tc, std::function< std::string( const Decision* )> printerFnc );

}

#include "TrigCompositeUtils/TrigCompositeUtils.icc"

#endif // TrigCompositeUtils_TrigCompositeUtils_h
