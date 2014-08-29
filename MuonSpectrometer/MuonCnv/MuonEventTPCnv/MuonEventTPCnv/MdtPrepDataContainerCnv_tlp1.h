/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTPREPDATACONTAINERCNV_TLP1_H
#define MDTPREPDATACONTAINERCNV_TLP1_H


#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "MdtPrepDataContainer_tlp1.h"

#include "MuonEventTPCnv/MuonPrepRawData/MdtPrepDataContainerCnv_p1.h"
#include "MuonEventTPCnv/MuonPrepRawData/MdtPrepDataCnv_p1.h"
#include "TrkEventTPCnv/TrkPrepRawData/PrepRawDataCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPositionCnv_p1.h"

#include <iostream>

class MdtPrepDataContainerCnv_tlp1
   : public AthenaPoolTopLevelTPConverter<Muon::MdtPrepDataContainerCnv_p1, Muon::MdtPrepDataContainer_tlp1 >
{
public:

  MdtPrepDataContainerCnv_tlp1();
  virtual ~MdtPrepDataContainerCnv_tlp1() {}

  virtual void	setPStorage( Muon::MdtPrepDataContainer_tlp1 *storage );
// return list of tokens to extending persistent objects
// it is stored in m_tokenList member of the Track persistent rep
  virtual TPCnvTokenList_p1* 	getTokenListVar() { return &(getTLPersObject()->m_tokenList); }

protected:

  MdtPrepDataCnv_p1             m_muonPrdCnv;
  PrepRawDataCnv_p1             m_prepDataCnv;
  LocalPositionCnv_p1           m_locPosCnv;
  ErrorMatrixCnv_p1             m_errorMatCnv;
};


#endif


