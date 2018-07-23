/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MsgUtil.h"

// Converter header file
#include "SCT_FlaggedCondDataCnv_p1.h"

// Other stuff
#include "Identifier/IdentifierHash.h"
#include <map>
#include <string>

void
SCT_FlaggedCondDataCnv_p1::persToTrans(const SCT_FlaggedCondData_p1* persObj, SCT_FlaggedCondData* transObj, MsgStream& /*log*/)
{
  for (const auto& it : persObj->m_data) {
    transObj->insert(std::make_pair(IdentifierHash(it.first), it.second));
  }
}

void
SCT_FlaggedCondDataCnv_p1::transToPers(const SCT_FlaggedCondData* transObj, SCT_FlaggedCondData_p1* persObj, MsgStream& /*log*/)
{
  for (const auto& it : *transObj) {
    persObj->m_data.insert(std::make_pair(it.first.value(), it.second));
  }
}
