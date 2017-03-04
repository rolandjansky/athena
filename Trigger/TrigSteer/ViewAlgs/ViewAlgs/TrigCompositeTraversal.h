// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_TrigCompositeTraversal_h
#define ViewAlgs_TrigCompositeTraversal_h
#include <functional>
#include "xAODTrigger/TrigCompositeContainer.h"
#include "AthContainers/AuxElement.h"
#include "AthLinks/ElementLink.h"
#include "AthLinks/ElementLinkVector.h"


namespace ViewAlgs {

  
  const xAOD::TrigComposite* find(const xAOD::TrigComposite*, const std::function<bool(const xAOD::TrigComposite*)>& filter);


  bool recursiveGetObjectLinks( const xAOD::TrigComposite* start, ElementLinkVector<xAOD::TrigCompositeContainer>& linkVector);


  class HasObject {
    public:
    HasObject(const std::string& name): m_name(name) {}
    bool operator()(const xAOD::TrigComposite* ) const;
    private:
    std::string m_name; 
  };

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
   * @retrun pair of link and TC with which it was associated, 
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

#endif


