///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOEVENT_CALOCELLCONTAINERVECTOR_H
#define CALOEVENT_CALOCELLCONTAINERVECTOR_H

/**
   @class CaloCellContainerVector
   @brief Container class for CaloCell

   @author Denis Oliveira Damazio

 */

/********************************************************************

NAME:     CaloCellContainerVector.h
PACKAGE:  offline/Calorimeter/CaloRec

AUTHORS:  D. Oliveira Damazio
CREATED:  February 2018

PURPOSE:  Vector Container for CaloCell 
          
********************************************************************/

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "CaloEvent/CaloCellContainer.h"


class CaloCellContainerVector : public DataVector<CaloCellContainer>
{

// only class allowed to use private functions
// this is because CaloCellContainer need to be told by
// someone the maximum hash size. Cannot do it by
// itself to avoid dependencies

 public:

 /** @brief type to be used for the internal lookup table, and to return list of cells */
  typedef std::vector<CaloCellContainer*> CellCollVector;

  /** @brief Main constructor */
  CaloCellContainerVector(SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS ) ;


  /** @brief Sized constructor */
  CaloCellContainerVector(size_t n, SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS );


  /**  @brief destructor */
  virtual ~CaloCellContainerVector() { };

};

CLASS_DEF(CaloCellContainerVector, 1311518038, 1)
SG_BASE(CaloCellContainerVector, DataVector<CaloCellContainer> );


#endif
