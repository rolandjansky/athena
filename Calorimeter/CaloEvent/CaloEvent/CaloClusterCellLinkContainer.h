///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOEVENT_CALOCLUSTERCELLLINKCONTAINER_H
#define CALOEVENT_CALOCLUSTERCELLLINKCONTAINER_H
/** 
   @class CaloClusterCellLinkContainer
   @brief Container class for CaloCellLink

   @author Walter Lampl

 */



#include "CLIDSvc/CLASS_DEF.h"
#include "AthContainers/DataVector.h"
#include "CaloEvent/CaloClusterCellLink.h"

class CaloClusterCellLinkContainer : public DataVector<CaloClusterCellLink>
{
 public:
  /** \brief constructor */ 
  CaloClusterCellLinkContainer();

  /** \brief iterator-based constructor */
  CaloClusterCellLinkContainer(CaloClusterCellLinkContainer::iterator it1,
			       CaloClusterCellLinkContainer::iterator it2,
			       SG::OwnershipPolicy plc);

  /** \brief destructor */ 
  virtual ~CaloClusterCellLinkContainer();

  /**@brief Prepare for writing.
   */
  void toPersistent();
};

CLASS_DEF(CaloClusterCellLinkContainer,1134620868,1)
SG_BASE(CaloClusterCellLinkContainer, DataVector<CaloClusterCellLink> );
#endif
