/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ViewAlgs/TrigCompositeTraversal.h"

namespace ViewAlgs {
  const xAOD::TrigComposite* find(const xAOD::TrigComposite* start, const std::function<bool(const xAOD::TrigComposite*)>& filter) {
    if ( filter( start ) )
      return start;

    if ( start->hasObjectLink("seed") ) 
      return find(start->object<xAOD::TrigComposite>("seed"), filter);
    else 
      return nullptr;	
  }
 
  bool HasObject::operator()(const xAOD::TrigComposite* composite ) const {
    return composite->hasObjectLink(m_name);
  }
}
