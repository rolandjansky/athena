/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCLUSTERCONTAINERCNV_P0_H
#define PIXELCLUSTERCONTAINERCNV_P0_H

#include "AthContainers/DataVector.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
typedef DataVector<Trk::PrepRawDataCollection<InDet::PixelCluster> > PixelClusterContainer_p0;


class PixelID;
namespace InDetDD{
class PixelDetectorManager;
}

class MsgStream;
class PixelClusterContainerCnv_p0  : public T_AthenaPoolTPCnvBase<InDet::PixelClusterContainer, PixelClusterContainer_p0> {
 private:
   const PixelID*  m_pixId;
   const InDetDD::PixelDetectorManager* m_pixMgr;
 public:
  virtual void   persToTrans(const PixelClusterContainer_p0*, InDet::PixelClusterContainer*, MsgStream&) override {
    // everything is done in createTransient()
  }

  virtual void   transToPers(const InDet::PixelClusterContainer*, PixelClusterContainer_p0*, MsgStream&) override {
   throw std::runtime_error("Writing Pixel PRDs in the old format is not supported");
  }

  virtual InDet::PixelClusterContainer* createTransient(const PixelClusterContainer_p0* /*persObj*/, MsgStream& /*log*/)
  { std::abort(); }
  InDet::PixelClusterContainer* createTransient(PixelClusterContainer_p0* persObj, MsgStream& log);

  // ID helper can't be used in the constructor, need initialize()
  StatusCode initialize( MsgStream &log );
};

#endif

