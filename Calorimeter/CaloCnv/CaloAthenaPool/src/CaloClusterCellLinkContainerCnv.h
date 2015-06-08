// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloClusterCellLinkContainerCnv.h 585280 2014-02-26 15:45:07Z wlampl $
#ifndef CALOCLUSTERCELLLINKCONTAINERCNV_H
#define CALOCLUSTERCELLLINKCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "CaloEvent/CaloClusterCellLinkContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv<CaloClusterCellLinkContainer, CaloClusterCellLinkContainer > CaloClusterCellLinkContainerCnvBase;

class CaloClusterCellLinkContainerCnv : public CaloClusterCellLinkContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< CaloClusterCellLinkContainerCnv >;

public:
   /// Converter constructor
   CaloClusterCellLinkContainerCnv( ISvcLocator* svcLoc );

  /// Function preparing the container to be written out
   virtual CaloClusterCellLinkContainer* createPersistent(CaloClusterCellLinkContainer* trans );
   /// Function reading in the persistent object
   virtual CaloClusterCellLinkContainer* createTransient();

private:
   void toPersistent( CaloClusterCellLink* lnk ) const;

};

#endif 
