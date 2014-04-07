/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAEVENT_CALORINGSCONTAINER_H
#define EGAMMAEVENT_CALORINGSCONTAINER_H

// INCLUDE HEADER FILES:

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "egammaEvent/CaloRings.h"

class CaloRingsContainer : public DataVector<CaloRings> 
{
 public:
  virtual ~CaloRingsContainer() {};

  /** Main constructor */
  CaloRingsContainer(SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS ) ;

  /** reimplementation of push_back (temporary)*/
  void push_back(const CaloRings *);

  void push_back(CaloRings *);

  /** wether it owns or not the cel SG::OWN_ELEMENTS or VIEW_ELEMENTS */
  SG::OwnershipPolicy  m_ownPolicy ;

};

SG_BASE(CaloRingsContainer, DataVector<CaloRings>);
CLASS_DEF(CaloRingsContainer, 1253285755, 1)

#endif

