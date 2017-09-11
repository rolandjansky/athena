/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_CLUSTERCONTAINERCNV_H
#define SCT_CLUSTERCONTAINERCNV_H

#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"



class StoreGateSvc;
class SCT_ID;

namespace InDetDD {
   class SCT_DetectorManager;
}

#include "SCT_ClusterContainerCnv_p0.h"
#include "InDetEventTPCnv/SCT_ClusterContainerCnv_tlp1.h"
#include "InDetEventTPCnv/SCT_ClusterContainerCnv_p3.h"
#include "InDetEventTPCnv/SCT_ClusterContainerCnv_p2.h"


// the latest persistent representation type of DataCollection:
typedef  InDet::SCT_ClusterContainer_p3  SCT_ClusterContainer_PERS;
typedef  T_AthenaPoolCustomCnv<InDet::SCT_ClusterContainer, SCT_ClusterContainer_PERS >  SCT_ClusterContainerCnvBase;

/**
** Create derived converter to customize the saving of identifiable
** container
**/
class SCT_ClusterContainerCnv : public SCT_ClusterContainerCnvBase
{
  friend class CnvFactory<SCT_ClusterContainerCnv >;

  // Converters need to be initialized (use ID helpers)
  // Thus they can't be local
  SCT_ClusterContainerCnv_p0   m_converter_p0;
  SCT_ClusterContainerCnv_tlp1 m_TPConverter;
  SCT_ClusterContainerCnv_p3   m_TPConverter_p3;
  SCT_ClusterContainerCnv_p2   m_TPConverter_p2;

  // Should not be needed at some point.
  StoreGateSvc*  m_storeGate;

protected:
  SCT_ClusterContainerCnv (ISvcLocator* svcloc);
  ~SCT_ClusterContainerCnv();
  virtual SCT_ClusterContainer_PERS*   createPersistent (InDet::SCT_ClusterContainer* transCont);
  virtual InDet::SCT_ClusterContainer* createTransient ();

  // Must initialize ID helpers
  virtual StatusCode initialize();
  virtual AthenaPoolTopLevelTPCnvBase*  getTopLevelTPCnv() { return &m_TPConverter; }
};


#endif
