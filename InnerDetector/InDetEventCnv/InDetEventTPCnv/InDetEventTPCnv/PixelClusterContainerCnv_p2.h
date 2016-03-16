/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCLUSTERCONTAINERCNV_P2_H
#define PIXELCLUSTERCONTAINERCNV_P2_H

// PixelClusterContainerCnv_p2, T/P separation of Pixel PRD
// author D.Costanzo <davide.costanzo@cern.ch>

#include "InDetPrepRawData/PixelClusterContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "InDetEventTPCnv/PixelClusterContainer_p2.h"

class PixelID;
class StoreGateSvc;
namespace InDetDD{ class PixelDetectorManager;}

class PixelClusterContainerCnv_p2 : public T_AthenaPoolTPCnvBase<InDet::PixelClusterContainer, InDet::PixelClusterContainer_p2>
{
 public:
  PixelClusterContainerCnv_p2() : m_isInitialized(0)  {};
  
  virtual void	persToTrans(const InDet::PixelClusterContainer_p2* persCont,
			    InDet::PixelClusterContainer* transCont,
			    MsgStream &log) ;
  virtual void	transToPers(const InDet::PixelClusterContainer* transCont,
			    InDet::PixelClusterContainer_p2* persCont,
			    MsgStream &log) ;

  virtual InDet::PixelClusterContainer* createTransient(const InDet::PixelClusterContainer_p2* persObj, MsgStream& log);
  

 private:
   const PixelID *m_pixId;
   StoreGateSvc *m_storeGate;
   const InDetDD::PixelDetectorManager* m_pixMgr;
   bool m_isInitialized;
   StatusCode initialize(MsgStream &log);


};

#endif


