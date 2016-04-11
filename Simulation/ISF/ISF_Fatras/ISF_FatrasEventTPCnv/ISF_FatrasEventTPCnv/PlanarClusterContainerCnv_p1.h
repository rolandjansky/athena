/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PLANARCLUSTERCONTAINERCNV_P1_H
#define PLANARCLUSTERCONTAINERCNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   PlanarClusterContainerCnv_p1.h
//
//-----------------------------------------------------------------------------


#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "ISF_FatrasEvent/PlanarClusterContainer.h"
#include "ISF_FatrasEventTPCnv/PlanarClusterContainer_p1.h"
#include "ISF_FatrasDetDescrModel/IdHashDetElementCollection.h"

class PixelID;
class StoreGateSvc;

class PlanarClusterContainerCnv_p1 : public T_AthenaPoolTPCnvBase< iFatras::PlanarClusterContainer,  iFatras::PlanarClusterContainer_p1>
{
 public:
  PlanarClusterContainerCnv_p1();

  virtual void  persToTrans( const iFatras::PlanarClusterContainer_p1* persCont,
			     iFatras::PlanarClusterContainer* transCont,
			     MsgStream &log) ;
  virtual void  transToPers(const iFatras::PlanarClusterContainer* transCont,
			    iFatras::PlanarClusterContainer_p1* persCont,
			    MsgStream &log) ;
  
  virtual iFatras::PlanarClusterContainer* createTransient(const iFatras::PlanarClusterContainer_p1* persObj, MsgStream& log);
  
 private:
  const PixelID *m_pixId;
  StoreGateSvc *m_storeGate;
  bool m_isInitialized;
  
  iFatras::IdHashDetElementCollection*                    m_detElementMap;
  std::string m_detElementMapName;
  
  StatusCode initialize(MsgStream &log);
  
};

#endif // PLANARCLUSTERCONTAINERCNV_P1_H
