/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ViewAlgs/TrigCompositeTraversal.h"


namespace ViewAlgs {

  const xAOD::TrigComposite* find(const xAOD::TrigComposite* start, const std::function<bool(const xAOD::TrigComposite*)>& filter) {
    if ( filter(start) ) return start;

    if ( start->hasObjectLink("seed") ) {
      return find(start->object<xAOD::TrigComposite>("seed"), filter);
    }
    else return nullptr; 
  }

  bool HasObject::operator()(const xAOD::TrigComposite* composite ) const {
    return composite->hasObjectLink(m_name);
  }





  bool recursiveGetObjectLinks( const xAOD::TrigComposite* start, ElementLinkVector<xAOD::TrigCompositeContainer>& linkVector){    
    //recursively find in the seeds
    if ( start->hasObjectLink("seed") ) {
      return recursiveGetObjectLinks(start->object<xAOD::TrigComposite>("seed"), linkVector);
    }
    
    int isComposite=0;
    start->getDetail( "IsComposite",isComposite);    
    if (!isComposite) {
      std::cout <<"TrigCompositeTraversal ViewAlgs::getObjectLinks    WARNING  Proxy "<< start->name()<< " is not composite!"<<std::endl;
      return false;
    }

    // ElementLink<xAOD::TrigComposite> test;
    // test = start->objectLink("childProxies"); 
    ElementLinkVector<xAOD::TrigCompositeContainer> links = start->objectCollectionLinks<xAOD::TrigCompositeContainer>("childProxies");

    linkVector.reserve(linkVector.size() + links.size());
    std::move(links.begin(), links.end(), std::back_inserter(linkVector));

    return true;
  }

}
