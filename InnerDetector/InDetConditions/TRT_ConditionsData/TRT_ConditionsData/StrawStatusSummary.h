/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STRAWSTATUSSUMMARY_H
#define STRAWSTATUSSUMMARY_H

#include "AthenaKernel/CLASS_DEF.h"
#include "Identifier/IdentifierHash.h"

#include <vector>

namespace TRTCond
{

class StrawStatusSummary
{
public:
  StrawStatusSummary(size_t nstraws);

  void setStatus(const IdentifierHash& hashID, bool status);
  bool findStatus(const IdentifierHash& hashID) const;
  size_t size() const { return m_status.size(); }

private:
  std::vector<bool> m_status;
};

} // namespace TRTCond

CLASS_DEF( TRTCond::StrawStatusSummary, 18106044, 1 )
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( TRTCond::StrawStatusSummary, 37831938 );

#endif // STRAWSTATUSSUMMARY_H

