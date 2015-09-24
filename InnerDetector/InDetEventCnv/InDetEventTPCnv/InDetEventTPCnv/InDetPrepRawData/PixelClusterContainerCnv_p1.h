/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCLUSTERCONTAINERCNV_P1_H
#define PIXELCLUSTERCONTAINERCNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   PixelClusterContainerCnv_p1.h
//
//-----------------------------------------------------------------------------
 
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetEventTPCnv/InDetPrepRawData/InDetPRD_Container_p1.h"
class PixelID;
namespace InDetDD{ class PixelDetectorManager;}

class StoreGateSvc;
class MsgStream;
namespace InDet{
class PixelClusterContainerCnv_p1 : public T_AthenaPoolTPCnvBase< InDet::PixelClusterContainer, InDet::InDetPRD_Container_p1 >
 
{
 public:
   typedef InDet::InDetPRD_Container_p1 PERS; 
   typedef InDet::PixelClusterContainer TRANS;
   PixelClusterContainerCnv_p1(): m_isInitialized(0) {}
   virtual void persToTrans(const PERS* persCont, TRANS* transCont, MsgStream &log); 
   virtual void transToPers(const TRANS* transCont, PERS* persCont, MsgStream &log);
   virtual InDet::PixelClusterContainer* createTransient(const InDet::InDetPRD_Container_p1* persObj, MsgStream& log);
 private:
   const PixelID *m_pixId;
   StoreGateSvc *m_storeGate;
   const InDetDD::PixelDetectorManager* m_pixMgr;
   bool m_isInitialized;
   StatusCode initialize(MsgStream &log);
};
 
}
#endif 
 
