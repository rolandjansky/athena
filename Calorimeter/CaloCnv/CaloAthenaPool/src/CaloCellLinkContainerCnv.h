/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CALOATHENAPOOL_CALOCELLLINKCONTAINERCNV_H
#define CALOATHENAPOOL_CALOCELLLINKCONTAINERCNV_H

#include "CaloTPCnv/CaloCellLinkContainerCnv_p1.h"
#include "CaloTPCnv/CaloCellLinkContainerCnv_p2.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

class CaloCellLinkContainer;
class CaloCellLinkContainer_p1;
class CaloCellLinkContainer_p2;

/**
 @class CaloCellLinkContainerCnv
 @brief POOL Converter for CaloCellLinkContainer

 **/


typedef CaloCellLinkContainer_p2 CaloCellLinkContainerPERS;

typedef T_AthenaPoolCustomCnv<CaloCellLinkContainer,CaloCellLinkContainerPERS> CaloCellLinkContainerCnvBase;

class CaloCellLinkContainerCnv : public CaloCellLinkContainerCnvBase 
{
  friend class CnvFactory<CaloCellLinkContainerCnv>;
 protected:
  CaloCellLinkContainerCnv(ISvcLocator*);
  virtual CaloCellLinkContainer* createTransient();
  virtual CaloCellLinkContainerPERS* createPersistent(CaloCellLinkContainer*);
 private:
  CaloCellLinkContainerCnv_p1 m_converter_p1;
  CaloCellLinkContainerCnv_p2 m_converter_p2;
  pool::Guid   m_p0_guid;
  pool::Guid   m_p1_guid;
  pool::Guid   m_p2_guid;


};

#endif
