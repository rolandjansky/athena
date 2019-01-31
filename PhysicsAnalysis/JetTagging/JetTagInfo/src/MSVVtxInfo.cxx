/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagInfo/MSVVtxInfo.h"

namespace Analysis
{

  MSVVtxInfo::MSVVtxInfo() :
    m_recsvx(Trk::RecVertex()),
    m_masssvx(-1.),
    m_ptsvx(-1.),
    m_etasvx(-1.),
    m_phisvx(-1.),
    m_efracsvx(-1.),
    m_normdist(-1.),
    m_trackinfo(std::vector<SVTrackInfo>()){
  /////...
  }

} //// namespace

