/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCLUSTERCONTAINERCNV_P0_H
#define PIXELCLUSTERCONTAINERCNV_P0_H

#include "DataModel/DataVector.h"
#include "StoreGate/StoreGateSvc.h"

#define private public
#define protected public
#include "InDetPrepRawData/PixelClusterContainer.h"
typedef DataVector<Trk::PrepRawDataCollection<InDet::PixelCluster> > PixelClusterContainer_p0;
#undef private
#undef protected

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class PixelID;
namespace InDetDD{
class PixelDetectorManager;
}

class MsgStream;
class PixelClusterContainerCnv_p0  : public T_AthenaPoolTPCnvBase<InDet::PixelClusterContainer, PixelClusterContainer_p0> {
 private:
   const PixelID*  m_pixId;
   const InDetDD::PixelDetectorManager* m_pixMgr;
   StoreGateSvc*  m_storeGate;
 public:
  virtual void   persToTrans(const PixelClusterContainer_p0*, InDet::PixelClusterContainer*, MsgStream&) {
    // everything is done in createTransient()
  }

  virtual void   transToPers(const InDet::PixelClusterContainer*, PixelClusterContainer_p0*, MsgStream&){
   throw std::runtime_error("Writing Pixel PRDs in the old format is not supported");
  }

  virtual InDet::PixelClusterContainer* createTransient(const PixelClusterContainer_p0* persObj, MsgStream& log);

  // ID helper can't be used in the constructor, need initialize()
  virtual StatusCode initialize( MsgStream &log );
};

#endif

