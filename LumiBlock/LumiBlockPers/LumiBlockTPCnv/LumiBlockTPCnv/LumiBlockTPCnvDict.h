/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUMIBLOCKTPCNVDICT_H
#define LUMIBLOCKTPCNVDICT_H

///////////////////////////////////////////////////////////////
// LumiBlockTPCnvDict.h
// author:Marjorie Shapiro <mdshapiro@lbl.gov>
///////////////////////////////////////////////////////////////

#include <vector>
#include "LumiBlockTPCnv/LumiBlockRange_p1.h"
#include "LumiBlockTPCnv/LumiBlockCollection_p1.h"
#include "LumiBlockTPCnv/LumiBlockRangeCnv_p1.h"
#include "LumiBlockTPCnv/LumiBlockCollectionCnv_p1.h"

#include "LumiBlockTPCnv/LumiBlockRange_p2.h"
#include "LumiBlockTPCnv/LumiBlockCollection_p2.h"
#include "LumiBlockTPCnv/LumiBlockRangeCnv_p2.h"
#include "LumiBlockTPCnv/LumiBlockCollectionCnv_p2.h"

namespace LumiBlockTPCnvDict {

  struct tmp {
    std::vector<LumiBlockRange_p1>                       m_lumiblockcollection_p1;
    T_TPCnv<LumiBlockCollection, LumiBlockCollection_p1> m_lumiblockcollectioncnv_p1;
    T_TPCnv<LB_IOVRange, LumiBlockRange_p1>                 m_lumiblockrangecnv_p1;

    std::vector<LumiBlockRange_p2>                       m_lumiblockcollection_p2;
    T_TPCnv<LumiBlockCollection, LumiBlockCollection_p2> m_lumiblockcollectioncnv_p2;
    T_TPCnv<LB_IOVRange, LumiBlockRange_p2>                 m_lumiblockrangecnv_p2;
  };
}

#endif 
