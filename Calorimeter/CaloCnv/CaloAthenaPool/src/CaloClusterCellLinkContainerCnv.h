// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloClusterCellLinkContainerCnv.h 781569 2016-11-01 12:16:14Z wlampl $
#ifndef CALOCLUSTERCELLLINKCONTAINERCNV_H
#define CALOCLUSTERCELLLINKCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "CaloEvent/CaloClusterCellLinkContainer.h"
#include "CaloTPCnv/CaloClusterCellLinkContainerCnv_p1.h"


/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv<CaloClusterCellLinkContainer, CaloClusterCellLinkContainer_p1 > CaloClusterCellLinkContainerCnvBase;

class CaloClusterCellLinkContainerCnv : public CaloClusterCellLinkContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< CaloClusterCellLinkContainerCnv >;

public:
   /// Converter constructor
   CaloClusterCellLinkContainerCnv( ISvcLocator* svcLoc );

  /// Function preparing the container to be written out
   virtual CaloClusterCellLinkContainer_p1* createPersistent(CaloClusterCellLinkContainer* trans );
   /// Function reading in the persistent object
   virtual CaloClusterCellLinkContainer* createTransient();

private:
  void toPersistent( CaloClusterCellLink* lnk ) const;

  CaloClusterCellLinkContainerCnv_p1 m_converter;
};

#endif 
