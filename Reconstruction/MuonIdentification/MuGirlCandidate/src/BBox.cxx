/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlCandidate/BBox.h"
#include <sstream>

namespace MuGirlNS
{

void BBox::update(double eta, double phi)
{
    if (isNull())
        set(eta, phi);
    else
    {
        if (m_etaMin > eta) m_etaMin = eta;
        if (m_etaMax < eta) m_etaMax = eta;
        if (!inPhiRange(phi))
        {
            if (canonicPhi(m_phiMin - phi) > 0.0) m_phiMin = phi;
            if (canonicPhi(m_phiMax - phi) < 0.0) m_phiMax = phi;
            canonicPhis();
        }
    }
}

std::string BBox::toString() const
{
    if (isNull()) return "NullBBox";

    std::ostringstream oss;
    oss << "etaMin=" << m_etaMin << ",etaMax=" << m_etaMax
    << ",phiMin=" << m_phiMin << ",phiMax=" << m_phiMax
    << ",dPhi=" << m_deletaPhi;

    return oss.str();
}

}
