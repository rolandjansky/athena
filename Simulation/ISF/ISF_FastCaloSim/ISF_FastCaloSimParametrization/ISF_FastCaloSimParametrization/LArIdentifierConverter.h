/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_LArIdentifierConverter_H
#define ISF_LArIdentifierConverter_H

#include "LArG4Code/LArVHitMerger.h"
#include "StoreGate/StoreGateSvc.h"
#include "LArG4Code/LArG4Identifier.h"
#include "Identifier/Identifier.h"

class LArIdentifierConverter : public LArVHitMerger {
 public:
  LArIdentifierConverter(StoreGateSvc* svc) : LArVHitMerger(svc) {}
  virtual ~LArIdentifierConverter() {}
  
  inline void BeginOfEvent() { ; }
  inline void EndOfEvent() { ; }
  //inline bool process(G4Step* step,    LArG4Identifier ident, G4double time, G4double energy) { return true; }
  inline bool process(G4Step*, const LArG4Identifier&, G4double, G4double) { return true;}
  inline Identifier convertID(LArG4Identifier id) { return ConvertID(id);}

};


#endif
