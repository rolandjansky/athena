//Dear emacs, this is -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef CALOTPCNV_CALOCLUSTERCELLLINKCNTCNV_P1
#define CALOTPCNV_CALOCLUSTERCELLLINKCNTCNV_P1

#include "AthenaKernel/ITPCnvBase.h"
#include "CaloEvent/CaloClusterCellLinkContainer.h"
#include "CaloTPCnv/CaloClusterCellLinkContainer_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "DataModelAthenaPool/DataLinkCnv_p2.h"

/**
 * @brief T/P conversions for CaloClusterCellLinkContainerCnv_p1
 */
class CaloClusterCellLinkContainerCnv_p1
  : public ITPCnvBase
{
public:
  /**
   * @brief Convert from persistent to transient object.
   * @param pers The persistent object to convert.
   * @param trans The transient object to which to convert.
   * @param log Error logging stream.
   */
  virtual void persToTrans(const CaloClusterCellLinkContainer_p1* pers,
			   CaloClusterCellLinkContainer* trans,
			   MsgStream& log);


  /**
   * @brief Convert from transient to persistent object.
   * @param trans The transient object to convert.
   * @param pers The persistent object to which to convert.
   * @param log Error logging stream.
   */
  virtual void transToPers(const CaloClusterCellLinkContainer* trans,
			   CaloClusterCellLinkContainer_p1* pers,
			   MsgStream &log);



  // Methods for invoking conversions on objects given by generic pointers.
  virtual void persToTransUntyped(const void* pers,
                                  void* trans,
                                  MsgStream& log);
  virtual void transToPersUntyped(const void* trans,
                                  void* pers,
                                  MsgStream& log);


  virtual const std::type_info& transientTInfo() const;

  /** return C++ type id of the persistent class this converter is for
      @return std::type_info&
  */
  virtual const std::type_info& persistentTInfo() const;




private:
  DataLinkCnv_p2<DataLink<CaloCellContainer> > m_linkCnv;

};

#endif
