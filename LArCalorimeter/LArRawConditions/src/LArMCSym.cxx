/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArMCSym.h"
#include <algorithm>
#include <cstdlib>


/**
 * @brief Constructor.
 * @brief onlId Online ID helper.
 * @brief caloId Offline ID helper.
 * @brief oflHashtoSymOnl Symmetric HWID corresponding to each offline hash.
 * @brief onlHashtoSymOnl Symmetric HWID corresponding to each online hash.
 * @brief symIds List of all symmetric HWIDs.  MUST BE SORTED.
 */
LArMCSym::LArMCSym(const LArOnlineID* onlId, 
		   const CaloCell_ID* caloId,
		   std::vector<HWIdentifier>&& oflHashtoSymOnl,
		   std::vector<HWIdentifier>&& onlHashtoSymOnl,
		   std::vector<HWIdentifier>&& symIds) :
  m_onlineID(onlId),
  m_caloCellID(caloId),
  m_oflHashtoSymOnl (initIndices (oflHashtoSymOnl, symIds)),
  m_onlHashtoSymOnl (initIndices (onlHashtoSymOnl, symIds)),
  m_symIds(symIds)
{
}


std::vector<LArMCSym::IdPair_t>
LArMCSym::initIndices (const std::vector<HWIdentifier>& ids,
                       const std::vector<HWIdentifier>& symIds) const
{
  std::vector<IdPair_t> out;
  out.reserve (ids.size());
  HWIdentifier last_id;
  size_t idx = NO_INDEX;
  for (const HWIdentifier& id : ids) {
    if (!id.is_valid()) {
      idx = NO_INDEX;
    }
    else if (id != last_id) {
      if (idx < symIds.size()-1 && symIds[idx+1] == id) {
        ++idx;
      }
      else {
        auto it = std::lower_bound (symIds.begin(), symIds.end(), id);
        if (it == symIds.end() || *it != id) std::abort();
        idx = it - symIds.begin();
      }
    }
    out.emplace_back (id.get_identifier32().get_compact(), idx);
    last_id = id;
  }

  return out;
}

