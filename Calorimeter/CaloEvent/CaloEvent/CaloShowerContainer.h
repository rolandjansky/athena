/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOEVENT_CALOSHOWERCONTAINER_H
#define CALOEVENT_CALOSHOWERCONTAINER_H

#include "CLIDSvc/CLASS_DEF.h"

#include "DataModel/DataVector.h"

#include "CaloEvent/CaloShower.h"

class CaloShowerContainer : public DataVector<CaloShower>
{
 public:
  CaloShowerContainer();
  virtual ~CaloShowerContainer();

  bool ownElements() const
    {return (this->m_ownPolicy == SG::OWN_ELEMENTS);}

};

/** @class CaloShowerContainer 
    @brief Container class for CaloShower
*/

CLASS_DEF(CaloShowerContainer, 1278423365, 1)
#endif
