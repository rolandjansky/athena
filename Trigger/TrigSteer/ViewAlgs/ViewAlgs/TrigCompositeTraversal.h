/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_TrigCompositeTraversal_h
#define ViewAlgs_TrigCompositeTraversal_h
#include <functional>
#include "xAODTrigger/TrigCompositeContainer.h"
namespace ViewAlgs {
  const xAOD::TrigComposite* find(const xAOD::TrigComposite*, const std::function<bool(const xAOD::TrigComposite*)>& filter);

  class HasObject {
  public:
    HasObject(const std::string& name): m_name(name) {}
    bool operator()(const xAOD::TrigComposite* ) const;
  private:
    std::string m_name; 
  };


}

#endif


