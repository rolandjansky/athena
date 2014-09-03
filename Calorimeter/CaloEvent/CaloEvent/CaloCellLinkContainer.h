///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOEVENT_CALOCELLLINKCONTAINER_H
#define CALOEVENT_CALOCELLLINKCONTAINER_H
/** 
   @class CaloCellLinkContainer
   @brief Container class for CaloCellLink

   @author Peter Loch

 */



#include "CLIDSvc/CLASS_DEF.h"

#include "DataModel/DataVector.h"

#include "CaloEvent/CaloCellLink.h"

class CaloCellLinkContainer : public DataVector<CaloCellLink>
{
 public:
  /** \brief constructor */ 
  CaloCellLinkContainer();

  /** \brief destructor */ 
  virtual ~CaloCellLinkContainer();

  /** \brief returns true if the container owns the CaloCellLinks */
  bool ownElements() const 
    {return (this->m_ownPolicy == SG::OWN_ELEMENTS);}
};

CLASS_DEF(CaloCellLinkContainer,1327319901,1)
SG_BASE(CaloCellLinkContainer, DataVector<CaloCellLink> );
#endif
