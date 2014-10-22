/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrackRecord_H
#define TrackRecord_H

#include "CLHEP/Vector/ThreeVector.h"

class TrackRecord {
public:
  /** @brief Default constructor */ 
  TrackRecord() : PDG_code(0), Energy(0), Momentum(0,0,0), Position(0,0,0), Time(0), barCode(0), volName("") {}
  /** @brief Default destructor */ 
  virtual ~TrackRecord() {}
  /** @brief Constructor */ 
  TrackRecord(int pdg, double e, CLHEP::Hep3Vector& p, CLHEP::Hep3Vector& x, double t, int bc, std::string vn)
    : PDG_code(pdg), Energy(e), Momentum(p), Position(x), Time(t), barCode(bc), volName(vn) {}
  /** @brief Constructor */ 
  TrackRecord(const TrackRecord& trc):PDG_code(trc.PDG_code), Energy(trc.Energy), 
    Momentum(trc.Momentum), Position(trc.Position),
    Time(trc.Time), barCode(trc.barCode), volName(trc.volName){}
  /** @brief Assignement Operator */ 
  TrackRecord &operator=(const TrackRecord& trc) {
    if(this!=&trc) {
      PDG_code = trc.PDG_code; Energy = trc.Energy; 
      Momentum = trc.Momentum; Position = trc.Position;
      Time = trc.Time; barCode = trc.barCode; 
      volName = trc.volName;
    }
    return *this;
  }
 
  /** @brief Set energy */ 
  void SetEnergy(double e) {Energy=e;}
  /** @brief Set position */ 
  void SetPosition(CLHEP::Hep3Vector p) {Position=p;}
  /** @brief Set momentum */ 
  void SetMomentum(CLHEP::Hep3Vector e) {Momentum=e;}
  /** @brief Set PDG code */ 
  void SetPDGCode(int pcode) {PDG_code=pcode;}
  /** @brief Energy */ 
  double GetEnergy() const {return Energy;}
  /** @brief Position */ 
  CLHEP::Hep3Vector GetPosition() const {return Position;}
  /** @brief Momentum */ 
  CLHEP::Hep3Vector GetMomentum() const {return Momentum;}
  /** @brief PDG Code */ 
  int GetPDGCode() const {return PDG_code;}
  
  /* Davide 30-6-06 add methods and evolve this class */

  /** @brief Set time */ 
  void SetTime(double time) {Time=time;}
  /** @brief  Time */ 
  double GetTime() const {return Time;}
  /** @brief Set bar code */ 
  void SetBarCode(int theCode){barCode=theCode;}
  /** @brief bar code */ 
  int GetBarCode() const {return barCode;}
  /** @brief Set Volume name */ 
  void SetVolName(std::string theName){volName=theName;}
  /** @brief Volume name */ 
  std::string GetVolName() const {return volName;}
  
 private:
  int PDG_code;
  double Energy;
  CLHEP::Hep3Vector Momentum;
  CLHEP::Hep3Vector Position;
  /* Davide 30-6-06 add member and evolve this class */
  double Time;
  int barCode;
  std::string volName;
};

#endif
