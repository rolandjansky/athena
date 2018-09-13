/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


// Tile includes
#include "TileConditions/TileDCSState.h"

#include <cstring>
#include <algorithm>

TileDCSState::TileDCSState()
  : m_goodDrawer(212222),
    m_unknownValue(-30) // should be negative
{

  std::fill(&m_states[0][0], &m_states[3][64], m_unknownValue);
  std::fill(&m_hv[0][0][0], &m_hv[3][63][48], m_unknownValue);
  std::fill(&m_hvSet[0][0][0], &m_hvSet[3][63][68], m_unknownValue);
  std::fill(&m_status[0][0][0], &m_status[3][63][48], UNKNOWN);
  std::fill(&m_hvStatus[0][0][0], &m_hvStatus[3][63][48], UNKNOWN);

}


//_____________________________________________________________________________
TileDCSState::TileDCSStatus TileDCSState::getDCSStatus(unsigned int ros, unsigned int drawer) const {

  TileDCSStatus status(ALERT_DRAWER);

  int state = m_states[ros - 1][drawer];
  if (state == m_goodDrawer) {
    status = OK_DRAWER;
  } else if (state == m_unknownValue) {
    status = UNKNOWN;
  } else if (std::find(m_warningDrawer.begin(), m_warningDrawer.end(), state) !=
             m_warningDrawer.end()) {

    status = WARNING_DRAWER;
  }

  return status;
}
