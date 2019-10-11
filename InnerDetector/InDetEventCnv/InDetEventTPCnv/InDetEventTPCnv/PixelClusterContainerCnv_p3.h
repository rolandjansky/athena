/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCLUSTERCONTAINERCNV_P3_H
#define PIXELCLUSTERCONTAINERCNV_P3_H

// PixelClusterContainerCnv_p3, T/P separation of Pixel PRD
// author A.Andreazza <attilio.andreazza@cern.ch>

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetEventTPCnv/PixelClusterContainer_p3.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"

class PixelID;
class StoreGateSvc;

class PixelClusterContainerCnv_p3 : public T_AthenaPoolTPCnvBase<InDet::PixelClusterContainer, InDet::PixelClusterContainer_p3>
{
 public:
  PixelClusterContainerCnv_p3() : m_pixelDetEleCollKey{"PixelDetectorElementCollection"}, m_useDetectorElement{true}, m_isInitialized{false} {};
  
  virtual void	persToTrans(const InDet::PixelClusterContainer_p3* persCont,
			    InDet::PixelClusterContainer* transCont,
			    MsgStream &log) ;
  virtual void	transToPers(const InDet::PixelClusterContainer* transCont,
			    InDet::PixelClusterContainer_p3* persCont,
			    MsgStream &log) ;

  virtual InDet::PixelClusterContainer* createTransient(const InDet::PixelClusterContainer_p3* persObj, MsgStream& log);

  // Method for test/PixelClusterContainerCnv_p3_test.cxx
  void setUseDetectorElement(const bool useDetectorElement);

 private:
   const PixelID *m_pixId{};
   StoreGateSvc *m_storeGate{};
   SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey;
   bool m_useDetectorElement;
   bool m_isInitialized;
   StatusCode initialize(MsgStream &log);


};

#endif


