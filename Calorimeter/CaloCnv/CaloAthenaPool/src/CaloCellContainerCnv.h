/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CaloCellContainerCnv_H
#define CaloCellContainerCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "CaloTPCnv/CaloCellContainerCnv_p1.h"

#include <string>


typedef T_AthenaPoolCustomCnv<CaloCellContainer, CaloCompactCellContainer> CaloCellContainerCnvBase;
typedef CaloCompactCellContainer CaloCellContainerPERS;

class CaloDetDescrManager;
class CaloCellContainer;
class CaloCompactCellContainerCnv;

/**
 @class CaloCellContainerCnv
 @brief POOL Converter for CaloCellContainer

 **/
class CaloCellContainerCnv : public CaloCellContainerCnvBase
{
   friend class CnvFactory<CaloCellContainerCnv>;
public:
   CaloCellContainerCnv(ISvcLocator* );//svcloc
   virtual ~CaloCellContainerCnv();

   

  virtual CaloCellContainer* createTransient();
  virtual CaloCellContainerPERS* createPersistent(CaloCellContainer*);

private:
   CaloCellContainerCnv_p1 m_converter1;
   pool::Guid   m_p1_guid;
   
};

#endif
