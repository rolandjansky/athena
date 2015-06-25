/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PLANARCLUSTERCONTAINERCNV_P2_H
#define PLANARCLUSTERCONTAINERCNV_P2_H

//-----------------------------------------------------------------------------
//
// file:   PlanarClusterContainerCnv_p2.h
//
//-----------------------------------------------------------------------------


#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "ISF_FatrasEvent/PlanarClusterContainer.h"
#include "ISF_FatrasEventTPCnv/PlanarClusterContainer_p2.h"
#include "ISF_FatrasDetDescrModel/IdHashDetElementCollection.h"

class PixelID;
class SCT_ID;
class StoreGateSvc;

class PlanarClusterContainerCnv_p2 : public T_AthenaPoolTPCnvBase< iFatras::PlanarClusterContainer,  iFatras::PlanarClusterContainer_p2>
{
 public:
  PlanarClusterContainerCnv_p2();
  
  virtual void  persToTrans( const iFatras::PlanarClusterContainer_p2* persCont,
			     iFatras::PlanarClusterContainer* transCont,
			     MsgStream &log) ;
  virtual void  transToPers(const iFatras::PlanarClusterContainer* transCont,
			    iFatras::PlanarClusterContainer_p2* persCont,
			    MsgStream &log) ;
  
  virtual iFatras::PlanarClusterContainer* createTransient(const iFatras::PlanarClusterContainer_p2* persObj, MsgStream& log);
  
 private:
  const PixelID *m_pixId;
  const SCT_ID *m_sctId;
  StoreGateSvc *m_storeGate;
  bool m_isInitialized;
  
  iFatras::IdHashDetElementCollection*                    m_pixelDetElementMap;
  std::string m_pixelDetElementMapName;
  iFatras::IdHashDetElementCollection*                    m_sctDetElementMap;
  std::string m_sctDetElementMapName;
  
  StatusCode initialize(MsgStream &log);
  
};

#endif // PLANARCLUSTERCONTAINERCNV_P1_H
