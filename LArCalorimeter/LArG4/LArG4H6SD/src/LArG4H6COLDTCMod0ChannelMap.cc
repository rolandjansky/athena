/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4H6COLDTCMod0ChannelMap.h"

#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"

#include <cmath>
#include <vector>

#undef DEBUG_ME

LArG4H6COLDTCMod0ChannelMap::LArG4H6COLDTCMod0ChannelMap()
  : m_phiMax(0), m_phiMin(0),
    m_nPhiBins(0)  { }

LArG4H6COLDTCMod0ChannelMap::LArG4H6COLDTCMod0ChannelMap(G4double rMin,
                                                         G4double rMax,
                                                         G4double area,
                                                         G4double phiMin,
                                                         G4double phiMax,
                                                         G4int    nPhis)
  : m_phiMax(phiMax)
  , m_phiMin(phiMin)
  , m_nPhiBins(nPhis)
{
  // find # phi bins
  const G4double deltaPhi = m_nPhiBins > 0 && phiMax > phiMin
    ? ( phiMax - phiMin ) / 2.
    : 0;
#ifdef DEBUG_ME
  std::cout<<"LArG4H6COLDTCMod0ChannelMap::LArG4H6COLDTCMod0ChannelMap: deltaPhi: "<<deltaPhi<<", area: "<<area<<std::endl;
  std::cout<<phiMax<<"/"<<phiMin<<"/"<<nPhis<<"/"<<m_nPhiBins<<std::endl;
#endif
  // calculate radial bins
  G4double r = deltaPhi > 0
    ? rMin
    : rMax;
  const G4double delta = 2 * area / deltaPhi;
  for ( ; r < rMax ; r = sqrt( delta + r * r ) )
    {
      //       std::cout<<r<<std::endl;
      m_rBins.push_back(r);
    }
  //       std::cout<<rMax<<std::endl;
  m_rBins.push_back(rMax);
}

G4int LArG4H6COLDTCMod0ChannelMap::getRBin(const G4ThreeVector& aPoint) const
{
  size_t    index  = 0;

  G4double radius = sqrt( aPoint.x() * aPoint.x() + aPoint.y() * aPoint.y() );
  while ( index < m_rBins.size() && radius > m_rBins[index] )
    {
      index++;
    }
#ifdef DEBUG_ME
  std::cout<<"LArG4H6COLDTCMod0ChannelMap::getRBin: "<<radius<<" / "<<index-1<<std::endl;
#endif
  return index < m_rBins.size()
    ? static_cast<G4int> (index-1)
    : -1;
}

G4int LArG4H6COLDTCMod0ChannelMap::getPhiBin(const G4ThreeVector& aPoint) const
{
  if ( aPoint.phi() < m_phiMax )
    {
      G4int index = static_cast<int> (floor((aPoint.phi()-m_phiMin)*m_nPhiBins/(m_phiMax-m_phiMin)));
#ifdef DEBUG_ME
      std::cout<<"LArG4H6COLDTCMod0ChannelMap::getPhiBin: "<<aPoint.phi()<<" / "<<index<<std::endl;
#endif
      return index >= 0
        ? index
        : -1;
    }
  else
    {
      return -1;
    }
}
