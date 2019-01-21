/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_RAWDATACONTAINERCNV_P4_H
#define SCT_RAWDATACONTAINERCNV_P4_H

// SCT_RawDataContainerCnv_p4, T/P separation of SCT Raw data
// author Susumu Oda <Susumu.Oda@cern.ch>

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "InDetEventAthenaPool/SCT_RawDataContainer_p4.h"
#include "InDetRawData/SCT_RDO_Container.h"

class SCT_ID;

// We do NOT use T_AthenaPoolTPCnvIDCont because we do all of the
// conversion in the .cxx. Same as for the LAr Raw Channels
// Note that this is used for a container of SCT Raw Data
// that containes only SCR1_RawData concrete types
// SCT_RDO_ContainerCnv.cxx delegates the conversion to this
// converter if the objects are of the correct type


class SCT_RawDataContainerCnv_p4 : public T_AthenaPoolTPCnvBase<SCT_RDO_Container, SCT_RawDataContainer_p4>
{
 private:
  const SCT_ID* m_sctId;
  int m_type;
 public:
  SCT_RawDataContainerCnv_p4() : m_sctId(nullptr), m_type(0) {};
  
  virtual void	persToTrans(const SCT_RawDataContainer_p4* persCont,
                            SCT_RDO_Container* transCont,
                            MsgStream& log);
  virtual void	transToPers(const SCT_RDO_Container* transCont,
                            SCT_RawDataContainer_p4* persCont,
                            MsgStream& log);
  virtual SCT_RDO_Container* createTransient(const SCT_RawDataContainer_p4* persObj, MsgStream& log);

  // ID helper can't be used in the constructor, need initialize()
  void initialize(const SCT_ID* idhelper) { m_sctId = idhelper; }
  void setType(int type) { m_type = type; }
};

#endif // SCT_RAWDATACONTAINERCNV_P4_H
