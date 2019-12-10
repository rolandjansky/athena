//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTPCNV_CALOTOPOTOWERCONTAINERCNV_P1_H
#define CALOTPCNV_CALOTOPOTOWERCONTAINERCNV_P1_H

#include "AthenaKernel/ITPCnvBase.h"
//std::vector<unsigned int> m_caloRegions;
//#include <algorithm>
#include "CaloTPCnv/CaloTowerSegCnv_p1.h"
#include "CaloTPCnv/CaloTopoTowerContainer_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "DataModelAthenaPool/DataLinkCnv_p1.h"
#include "AthLinks/DataLink.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell2ClusterMap.h"
#include "CaloEvent/CaloTowerContainer.h"
#include "CaloEvent/CaloTopoTowerContainer.h"

class CaloTopoTowerContainer;
class CaloTopoTowerContainer_p1;

class CaloTopoTowerContainerCnv_p1
  : public T_AthenaPoolTPCnvConstBase<CaloTopoTowerContainer, CaloTopoTowerContainer_p1>
{
public:
  using base_class::transToPers;
  using base_class::persToTrans;


  virtual
  void persToTrans (const CaloTopoTowerContainer_p1*,
                    CaloTopoTowerContainer*,
                    MsgStream& msg) const override;


  virtual
  void transToPers (const CaloTopoTowerContainer*,
                    CaloTopoTowerContainer_p1*,
                    MsgStream& msg) const override;


private:
  CaloTowerSegCnv_p1 m_caloTowerSegCnv;
  DataLinkCnv_p1<DataLink<CaloCellContainer> > m_cells;
  DataLinkCnv_p1<DataLink<CaloClusterContainer> > m_clusters;
  DataLinkCnv_p1<DataLink<CaloTowerContainer> > m_towers;
};


#endif
