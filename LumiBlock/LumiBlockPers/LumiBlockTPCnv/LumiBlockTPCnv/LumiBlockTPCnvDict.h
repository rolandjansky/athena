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

namespace LumiBlockTPCnvDict {

  struct tmp {
    std::vector<LumiBlockRange_p1>                       m_lumiblockcollection_p1;
    T_TPCnv<LumiBlockCollection, LumiBlockCollection_p1> m_lumiblockcollectioncnv_p1;
    T_TPCnv<IOVRange, LumiBlockRange_p1>                 m_lumiblockrangecnv_p1;
  };
}

#endif 
