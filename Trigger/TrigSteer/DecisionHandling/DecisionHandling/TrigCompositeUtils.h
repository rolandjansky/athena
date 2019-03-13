/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DecisionHandling_TrigCompositeUtils_h
#define DecisionHandling_TrigCompositeUtils_h

#include <set>
#include <memory>
#include <functional>
#include <iostream>

#include "AthLinks/ElementLink.h"
#include "AthLinks/ElementLinkVector.h"
#include "GaudiKernel/EventContext.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandle.h"
#include "GaudiKernel/ThreadLocalContext.h"

#include "AthContainers/AuxElement.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"

namespace TrigCompositeUtils {

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
  Decision* newDecisionIn ( DecisionContainer* dc, const std::string& name = "", const EventContext& ctx = Gaudi::Hive::currentContext() );

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
   * @brief Links to the previous object, location of previous 'seed' decision supplied by hand
   **/
  void linkToPrevious(Decision* d, const std::string& previousCollectionKey, size_t previousIndex);

  /**
   * @brief Links to the previous object, 'seed' decision provided explicitly.
   **/
  void linkToPrevious(Decision* d, const Decision* dOld);

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
   * @brief traverses TC links for another TC fufilling the prerequisite specified by the filter
   * @return matching TC or nullptr
   **/
  const xAOD::TrigComposite* find(const xAOD::TrigComposite*, const std::function<bool(const xAOD::TrigComposite*)>& filter);

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
    bool operator()(const xAOD::TrigComposite* ) const;
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
     * @brief checks if the arg TC has link collection of name specified at construction 
     * @warning method for internal consumption within @see find function 
     **/
    bool operator()(const xAOD::TrigComposite* ) const;
  private:
    std::string m_name;
  };

  
  /**
   * @brief collects all TC linked back to the start TC
   * @return true if search worked, false if a composite TC was found but not links back were found there
   **/
  bool recursiveGetObjectLinks( const xAOD::TrigComposite* start, ElementLinkVector<xAOD::TrigCompositeContainer>& linkVector);

  /**
   * @brief Helper to keep the TC & object it has linked together (for convenience)
   **/
  template<typename T>
  struct LinkInfo {
    LinkInfo()
      : source{0} {}
    LinkInfo(const xAOD::TrigComposite *s, const ElementLink<T>& l)
      : source{s}, link{l} {}

    bool isValid() const {
      return source && link.isValid(); }
    /**
     * @brief helper conversion to make it usable with CHECK macro expecting StatusCode
     */
    operator StatusCode () {
      return (isValid() ? StatusCode::SUCCESS : StatusCode::FAILURE); }

    const xAOD::TrigComposite *source;
    ElementLink<T> link;
  };

  /**
   * @brief search back the TC links for the object of type T linked to the one of TC (recursively)
   * @arg start the TC  from where the link back is to be looked for
   * @arg linkName the name with which the Link was added to the source TC
   * @return pair of link and TC with which it was associated, 
   */
  template<typename T>
  LinkInfo<T>
  findLink(const xAOD::TrigComposite* start, const std::string& linkName);

  /**
   * Prints the TC including the linked seeds
   * @warnign expensive call
   **/  
  std::string dump( const xAOD::TrigComposite*  tc, std::function< std::string( const xAOD::TrigComposite* )> printerFnc );

}

#include "DecisionHandling/TrigCompositeUtils.icc"

#endif // DecisionHandling_TrigCompositeUtils_h
