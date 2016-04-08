/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAEVENT_EGDETAILCONTAINER_H
#define EGAMMAEVENT_EGDETAILCONTAINER_H
/**
  @class egDetailContainer
          Container for detailed egamma information
   @author 
*/

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "egammaEvent/egDetail.h"

class egDetailContainer : public DataVector<egDetail> 
{
 public:
  /** @brief Destructor*/
  virtual ~egDetailContainer() {};

  /** @brief Main constructor */
  egDetailContainer(SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS ) ;

 private:

  /** wether it owns or not the cel SG::OWN_ELEMENTS or VIEW_ELEMENTS */
  //SG::OwnershipPolicy  m_ownPolicy ;

};

CLASS_DEF(egDetailContainer, 1180687045, 1)

#endif

