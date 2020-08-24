/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTRADIATORPARAMETERS_H
#define TRTRADIATORPARAMETERS_H

#include "G4LogicalVolume.hh"

enum BEflag { BARREL, ENDCAP };

class TRTRadiatorParameters {
public:
  TRTRadiatorParameters(G4LogicalVolume * p,
                        G4double dFoil,
                        G4double dGas,
                        BEflag beflag) :
    m_aLogicalVolume(p),
    m_FoilThickness(dFoil),
    m_GasThickness(dGas),
    m_BEflg(beflag) {}

  const G4LogicalVolume * GetLogicalVolume() const { return m_aLogicalVolume; }
  G4double          GetFoilThickness() const { return m_FoilThickness; }
  G4double          GetGasThickness()  const { return m_GasThickness; }
  BEflag            GetBEflag()        const { return m_BEflg; }

private:
  G4LogicalVolume * m_aLogicalVolume;
  G4double          m_FoilThickness;
  G4double          m_GasThickness;
  BEflag            m_BEflg;
};

#endif
