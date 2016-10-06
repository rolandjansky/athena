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

  bool setObjectLinks(  xAOD::TrigComposite* start, ElementLinkVector<xAOD::TrigCompositeContainer> linkVector);
  bool makeObjName(std::string& name, int childkey);
  bool getObjectLinks( const xAOD::TrigComposite* start, ElementLinkVector<xAOD::TrigCompositeContainer>& linkVector);
  bool getChildKeys(std::string name, std::vector<int> &childkeys);

  class HasObject {
  public:
    HasObject(const std::string& name): m_name(name) {}
    bool operator()(const xAOD::TrigComposite* ) const;
  private:
    std::string m_name; 
  };
  template<typename Type>
  class HasLinksToObjects {
  public:    
    HasLinksToObjects(const std::string& name) : m_name(name+"Indices") {}

    bool operator()(const xAOD::TrigComposite* c) const {
      
      // hasDetail has to be made const      if ( not c->hasDetail<int>("IsComposite") ) return false;
      
      //      if ( c->hasDetail<std::vector<int> >(m_name) ) return true;
      return false;
    }
  private:
    std::string m_name; 

  };

}

#endif


