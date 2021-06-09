/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPRD_CONTAINER_p2_H
#define MUONPRD_CONTAINER_p2_H

#include <vector>
#include <string>
#include "MuonEventTPCnv/MuonPrepRawData/MuonPRD_Collection_p2.h"
#include "MuonEventTPCnv/MuonPrepRawData/MdtPrepData_p2.h"
#include "MuonEventTPCnv/MuonPrepRawData/CscPrepData_p2.h"
#include "MuonEventTPCnv/MuonPrepRawData/RpcPrepData_p3.h"
#include "MuonEventTPCnv/MuonPrepRawData/TgcPrepData_p2.h"
#include "MuonEventTPCnv/MuonPrepRawData/TgcPrepData_p3.h"
#include "MuonEventTPCnv/MuonPrepRawData/MMPrepData_p1.h"
#include "MuonEventTPCnv/MuonPrepRawData/sTgcPrepData_p1.h"

namespace Muon {
/** Class to contain the Muon Prep Raw Data. */
template <class PRD>
class MuonPRD_Container_p2   
{
 public:
/// Default constructor
  MuonPRD_Container_p2 () {};
  std::vector<MuonPRD_Collection_p2>           m_collections;
  std::vector<PRD>                             m_prds;
   /** The delta identifiers of the PRD i.e. we only store the difference from the last one. This saves space, compared to always writing the full 32-bit word*/
  std::vector<unsigned short>                  m_prdDeltaId;
};

typedef MuonPRD_Container_p2<MdtPrepData_p2> MdtPrepDataContainer_p2;
typedef MuonPRD_Container_p2<CscPrepData_p2> CscPrepDataContainer_p2;
typedef MuonPRD_Container_p2<RpcPrepData_p3> RpcPrepDataContainer_p3;
typedef MuonPRD_Container_p2<TgcPrepData_p2> TgcPrepDataContainer_p2;
typedef MuonPRD_Container_p2<TgcPrepData_p3> TgcPrepDataContainer_p3;
typedef MuonPRD_Container_p2<MMPrepData_p1>  MMPrepDataContainer_p1;
typedef MuonPRD_Container_p2<sTgcPrepData_p1>  sTgcPrepDataContainer_p1;
} 

#endif
