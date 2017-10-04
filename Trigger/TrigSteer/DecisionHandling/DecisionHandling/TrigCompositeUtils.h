/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DecisionHandling_TrigCompositeUtils_h
#define DecisionHandling_TrigCompositeUtils_h

#include <set>
#include <memory>
#include <functional>

#include "AthLinks/ElementLink.h"
#include "AthLinks/ElementLinkVector.h"
#include "GaudiKernel/EventContext.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandle.h"

#include "AthContainers/AuxElement.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"

namespace TrigCompositeUtils {

  // alias types, for readability and to simplify future evolution
  typedef xAOD::TrigComposite Decision;
  typedef xAOD::TrigCompositeContainer DecisionContainer;
  typedef xAOD::TrigCompositeAuxContainer DecisionAuxContainer;

  typedef SG::WriteHandle<DecisionContainer> DecisionWriteHandle;
  /**
   * @brief creates and right away stores the DecisionContainer under the key
   **/
  DecisionWriteHandle createAndStore(const SG::WriteHandleKey<DecisionContainer>& key, const EventContext& ctx);

  
  /**
   * @brief helper method to that created the Decision objects, places it in the container and returns
   * This is to make this:
   * auto d = newDecisionIn(output);
   * instead of:
   * auto d = new Decision; 
   * output->push_back(d);    
   **/
  
  Decision* newDecisionIn (DecisionContainer* dc);

  // aliases for the decision IDs, in fact this are just ints
  typedef unsigned int DecisionID;
  typedef std::set<DecisionID> DecisionIDContainer;

  /**
   * @brief Appends the decision (given as ID) to the decision object
   * @warning Performing two decision insertions with the same ID consecutively results in the ID stored only once
   * This helps solving multiple inserts in the combo hypo tools
   **/
  void addDecisionID( DecisionID id,  Decision* d);

      
  /**
   * @brief Extracts DecisionIDs stored in the Decsion object 
   **/
  void decisionIDs( const Decision* d, DecisionIDContainer& id );

  /**
   * @brief Another variant of the above method
   **/
  const std::vector<int>& decisionIDs( const Decision* d ); 
  std::vector<int>& decisionIDs( Decision* d );


  /**
   * @brief return true if thre is no positive decision stored
   **/
  bool allFailed( const Decision* d );
  
  /**
   * @brief Checks if any of the DecisionIDs passed in arg required is availble in Decision object
   **/
  bool passingIDs( const Decision* d,  const DecisionIDContainer& required);

  /**
   * @brief checks if required ID is in the set of the decisions
   **/
  bool passed( DecisionID id, const DecisionIDContainer& );
  
  /**
   * @brief Links to the previous object
   **/
  void linkToPrevious(Decision* d, const std::string& previousCollectionKey, size_t previousIndex);

  /**
   * @brief checks if there is a link to previous object
   **/
  bool hasLinkToPrevious(const Decision* d);

  /**
   * @brief returns link to previous decision object
   **/
  ElementLink<DecisionContainer> linkToPrevious(const Decision*);






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
   * @brief search back the TC links for the object of type T linked to the one of TC
   * @arg start the TC where from where the link back is to be looked for
   * @arg linkName the name with which the Link was added to the source TC
   * @return pair of link and TC with which it was associated, 
   */
  template<typename T>
  LinkInfo<T>
  findLink(const xAOD::TrigComposite* start, const std::string& linkName) {
    auto source = find(start, HasObject(linkName) );
    if ( not source )
      return LinkInfo<T>(); // invalid link
    return LinkInfo<T>( source, source->objectLink<T>( linkName ) );
  }
}


#endif // DecisionHandling_TrigCompositeUtils_h
