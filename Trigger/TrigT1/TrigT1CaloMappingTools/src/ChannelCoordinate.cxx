/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "ChannelCoordinate.h"

namespace LVL1 {

ChannelCoordinate::ChannelCoordinate() : m_layer(NONE), m_eta(0.0), m_phi(0.0),
                                         m_etaGran(0.0), m_phiGran(0.0)
{
}

ChannelCoordinate::ChannelCoordinate(const CaloLayer layer, const double eta,
                                     const double phi, const double etaGran,
				     const double phiGran) :
 m_layer(layer), m_eta(eta), m_phi(phi), m_etaGran(etaGran), m_phiGran(phiGran)
{
}

std::string ChannelCoordinate::layerName(const CaloLayer layer)
{
  std::string name;
  switch (layer) {
    case ChannelCoordinate::NONE:  name = "NONE";  break;
    case ChannelCoordinate::EM:    name = "EM";    break;
    case ChannelCoordinate::HAD:   name = "HAD";   break;
    case ChannelCoordinate::FCAL2: name = "FCAL2"; break;
    case ChannelCoordinate::FCAL3: name = "FCAL3"; break;
    default:                       name = "????";  break;
  }
  return name;
}

} // end namespace
