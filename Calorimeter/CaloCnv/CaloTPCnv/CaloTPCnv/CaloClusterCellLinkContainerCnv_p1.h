//Dear emacs, this is -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef CALOTPCNV_CALOCLUSTERCELLLINKCNTCNV_P1
#define CALOTPCNV_CALOCLUSTERCELLLINKCNTCNV_P1

#include "CaloEvent/CaloClusterCellLinkContainer.h"
#include "CaloTPCnv/CaloClusterCellLinkContainer_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "DataModelAthenaPool/DataLinkCnv_p2.h"

/**
 * @brief T/P conversions for CaloClusterCellLinkContainerCnv_p1
 */
class CaloClusterCellLinkContainerCnv_p1
  : public T_AthenaPoolTPCnvWithKeyBase<CaloClusterCellLinkContainer, CaloClusterCellLinkContainer_p1>
{
public:
  using base_class::transToPersWithKey;
  using base_class::persToTransWithKey;


  /**
   * @brief Convert from persistent to transient object.
   * @param pers The persistent object to convert.
   * @param trans The transient object to which to convert.
   * @param key SG key of the object being read.
   * @param log Error logging stream.
   */
  virtual
  void persToTransWithKey (const CaloClusterCellLinkContainer_p1* pers,
                           CaloClusterCellLinkContainer* trans,
                           const std::string& key,
                           MsgStream& log) const override;


  /**
   * @brief Convert from transient to persistent object.
   * @param trans The transient object to convert.
   * @param pers The persistent object to which to convert.
   * @param key SG key of the object being written.
   * @param log Error logging stream.
   */
  virtual
  void transToPersWithKey (const CaloClusterCellLinkContainer* trans,
                           CaloClusterCellLinkContainer_p1* pers,
                           const std::string& key,
                           MsgStream& log) const override;


private:
  DataLinkCnv_p2<DataLink<CaloCellContainer> > m_linkCnv;

};


#endif
