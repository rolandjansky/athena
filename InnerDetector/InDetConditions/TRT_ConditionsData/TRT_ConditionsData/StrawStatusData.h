/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STRAWSTATUSDATA_H
#define STRAWSTATUSDATA_H

#include "AthenaKernel/CLASS_DEF.h"
#include "Identifier/IdentifierHash.h"

#include <vector>

namespace TRTCond
{

class StrawStatusData
{
public:
  StrawStatusData(size_t nstraws);

  void setStatus(const IdentifierHash& hashID, unsigned int status);
  unsigned int findStatus(const IdentifierHash& hashID) const;
  size_t size() const { return m_status.size(); }

private:
  std::vector<unsigned int> m_status;
};

} // namespace TRTCond

CLASS_DEF( TRTCond::StrawStatusData, 102360873, 1 )
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( TRTCond::StrawStatusData, 178673451 );

#endif // STRAWSTATUSDATA_H

