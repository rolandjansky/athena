/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTATHENAPOOL_CSCRAWDATACONTAINERCNV_H
#define MUONEVENTATHENAPOOL_CSCRAWDATACONTAINERCNV_H

#include "MuonEventAthenaPool/CscRawDataContainer_p1.h"
#include "MuonRDO/CscRawDataContainer.h"
#include "MuonRdoContainerTPCnv.h"
#include "CscRawDataContainerCnv_p1.h"
#include "CscRawDataContainerCnv_p2.h"
#include "CscRawDataContainerCnv_p3.h"
#include "CscRawDataContainerCnv_p4.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

/*
  custom POOL TP converter for CSC RDO
  @author Marcin Nowak
*/
typedef MuonRdoContainerTPCnv<CscRawDataContainer, CscRawDataContainer_p4, CscRawDataContainerCnv_p4 > CscRawDataContainerCnvBase;

// override here the createTransient() of MuonRdoContainerTPCnv.icc since it cause compiler error
// TODO: need to properly address compiler error and get rid of this override
template < > inline CscRawDataContainer* CscRawDataContainerCnvBase::createTransient(){
  return nullptr;
}

class CscRawDataContainerCnv : public CscRawDataContainerCnvBase {
  public:
    CscRawDataContainerCnv (ISvcLocator* svcloc) : CscRawDataContainerCnvBase(svcloc) {}
    ~CscRawDataContainerCnv() {}
  protected:
    virtual CscRawDataContainer* createTransient();
    virtual StatusCode initialize(); // pass the MuonIdHelperSvc to the CscRawDataCnv_p...
  private:
    CscRawDataContainerCnv_p1   m_converter_p1;
    CscRawDataContainerCnv_p2   m_converter_p2;
    CscRawDataContainerCnv_p3   m_converter_p3;
};

#endif


