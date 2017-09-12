/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCLUSTERCONTAINERCNV_H
#define PIXELCLUSTERCONTAINERCNV_H

#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"



class StoreGateSvc;
class PixelID;

namespace InDetDD {
   class PixelDetectorManager;
}

#include "PixelClusterContainerCnv_p0.h"
#include "InDetEventTPCnv/PixelClusterContainerCnv_tlp1.h"
#include "InDetEventTPCnv/PixelClusterContainerCnv_p2.h"
#include "InDetEventTPCnv/PixelClusterContainerCnv_p3.h"


// the latest persistent representation type of DataCollection:
typedef  InDet::PixelClusterContainer_p3  PixelClusterContainer_PERS;
typedef  T_AthenaPoolCustomCnv<InDet::PixelClusterContainer, PixelClusterContainer_PERS >  PixelClusterContainerCnvBase;

/**
** Create derived converter to customize the saving of identifiable
** container
**/
class PixelClusterContainerCnv :  public PixelClusterContainerCnvBase
{
  friend class CnvFactory<PixelClusterContainerCnv >;

  // Converters need to be initialized (use ID helpers)
  // Thus they can't be local
  PixelClusterContainerCnv_p0   m_converter_p0;
  PixelClusterContainerCnv_p2   m_converter_p2;
  PixelClusterContainerCnv_p3   m_converter_p3;
  PixelClusterContainerCnv_tlp1 m_TPConverter;

  // Should not be needed at some point.
  StoreGateSvc*  m_storeGate;

protected:
  PixelClusterContainerCnv (ISvcLocator* svcloc);
  ~PixelClusterContainerCnv();
  virtual PixelClusterContainer_PERS*   createPersistent (InDet::PixelClusterContainer* transCont);
  virtual InDet::PixelClusterContainer* createTransient ();

  // Must initialize ID helpers
  virtual StatusCode initialize();
  virtual AthenaPoolTopLevelTPCnvBase*  getTopLevelTPCnv() { return &m_TPConverter; }
};


#endif
