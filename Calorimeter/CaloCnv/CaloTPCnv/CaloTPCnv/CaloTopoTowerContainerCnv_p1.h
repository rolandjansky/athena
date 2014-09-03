//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTPCNV_CALOTOPOTOWERCONTAINERCNV_P1_H
#define CALOTPCNV_CALOTOPOTOWERCONTAINERCNV_P1_H

#include "AthenaKernel/ITPCnvBase.h"
//std::vector<unsigned int> m_caloRegions;
//#include <algorithm>
#include "CaloTPCnv/CaloTowerSegCnv_p1.h"
#include "AthenaPoolCnvSvc/ITPConverter.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "DataModelAthenaPool/DataLinkCnv_p1.h"
#include "DataModel/DataLink.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell2ClusterMap.h"
#include "CaloEvent/CaloTowerContainer.h"

class CaloTopoTowerContainer;
class CaloTopoTowerContainer_p1;

class CaloTopoTowerContainerCnv_p1
  : public ITPCnvBase
{
public:
  virtual ~CaloTopoTowerContainerCnv_p1() {}
  virtual void persToTrans(const CaloTopoTowerContainer_p1*, CaloTopoTowerContainer*,MsgStream& msg) ;
  virtual void transToPers(const CaloTopoTowerContainer*, CaloTopoTowerContainer_p1*,MsgStream& msg) ;

  /** Convert persistent object representation to transient
      @param pers [IN] void* pointer to the persistent object
      @param trans [OUT] void* pointer to the empty transient object
      @param log [IN] output message stream
  */
  virtual void persToTransUntyped(const void* pers, void* trans,
                                  MsgStream& log);

  /** Convert transient object representation to persistent
      @param trans [IN] void* pointer to the transient object
      @param pers [OUT] void* pointer to the empty persistent object
      @param log [IN] output message stream
  */  
  virtual void transToPersUntyped(const void* trans, void* pers,
                                  MsgStream& log);

  /** return C++ type id of the transient class this converter is for
      @return std::type_info&
  */
  virtual const std::type_info& transientTInfo() const;

  /** return C++ type id of the persistent class this converter is for
      @return std::type_info&
  */
  virtual const std::type_info& persistentTInfo() const;


private:
  CaloTowerSegCnv_p1 m_caloTowerSegCnv;
  DataLinkCnv_p1<DataLink<CaloCellContainer> > m_cells;
  DataLinkCnv_p1<DataLink<CaloClusterContainer> > m_clusters;
  DataLinkCnv_p1<DataLink<CaloTowerContainer> > m_towers;


};


#endif
