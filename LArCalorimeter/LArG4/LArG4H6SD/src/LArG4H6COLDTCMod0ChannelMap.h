/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArG4H6COLDTCMod0ChannelMap_H
#define LArG4H6COLDTCMod0ChannelMap_H

#include <vector>
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"

class LArG4H6COLDTCMod0ChannelMap
{
 public:

  LArG4H6COLDTCMod0ChannelMap();
  LArG4H6COLDTCMod0ChannelMap(G4double rMin, G4double rMax, G4double m_area,
                          G4double phiMin, G4double phiMax, G4int nPhis);
  ~LArG4H6COLDTCMod0ChannelMap() = default;

  G4int getRBin(const G4ThreeVector& aPoint) const;
  G4int getNoRBins() const { return m_rBins.size(); }
  const std::vector<G4double>& getRBinning() { return m_rBins; }
  G4int getPhiBin(const G4ThreeVector& aPoint) const;
  G4int getNoPhiBins() const { return m_nPhiBins; }

 private:

  G4double m_phiMax, m_phiMin;

  std::vector<G4double> m_rBins;
  G4int    m_nPhiBins;

};
#endif
