/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PLANARCLUSTERCONTAINERCNV_H
#define PLANARCLUSTERCONTAINERCNV_H

#include "ISF_FatrasEvent/PlanarClusterCollection.h"
#include "ISF_FatrasEvent/PlanarClusterContainer.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "ISF_FatrasEventTPCnv/PlanarClusterContainerCnv_p1.h"
#include "ISF_FatrasEventTPCnv/PlanarClusterContainerCnv_p2.h"

// the latest persistent representation type of DataCollection:
typedef  iFatras::PlanarClusterContainer_p2  PlanarClusterContainer_PERS;
typedef  T_AthenaPoolCustomCnv<iFatras::PlanarClusterContainer, PlanarClusterContainer_PERS >  PlanarClusterContainerCnvBase;

/**
** Create derived converter to customize the saving of identifiable
** container
**/

class PlanarClusterContainerCnv : public PlanarClusterContainerCnvBase
{
  friend class CnvFactory<PlanarClusterContainerCnv >;

  // Converters need to be initialized (use ID helpers)
  // Thus they can't be local
  
protected:
  PlanarClusterContainerCnv (ISvcLocator* svcloc);
  virtual ~PlanarClusterContainerCnv() override;
  virtual PlanarClusterContainer_PERS*   createPersistent (iFatras::PlanarClusterContainer* transCont)  override;
  virtual iFatras::PlanarClusterContainer* createTransient () override;


private:
  PlanarClusterContainerCnv_p1   m_converter_p1;
  PlanarClusterContainerCnv_p2   m_converter_p2;
};


#endif
