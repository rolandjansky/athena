/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrackRecord_H
#define TrackRecord_H

#include "CLHEP/Vector/ThreeVector.h"

class TrackRecord {
public:
  /** @brief Default constructor */ 
  TrackRecord() : m_PDG_code(0), m_Energy(0), m_Momentum(0,0,0), m_Position(0,0,0), m_Time(0), m_barCode(0), m_volName("") {}
  /** @brief Default destructor */ 
  virtual ~TrackRecord() {}
  /** @brief Constructor */ 
  TrackRecord(int pdg, double e, const CLHEP::Hep3Vector& p, const CLHEP::Hep3Vector& x, double t, int bc, const std::string& vn)
    : m_PDG_code(pdg), m_Energy(e), m_Momentum(p), m_Position(x), m_Time(t), m_barCode(bc), m_volName(vn) {}
  /** @brief Constructor */ 
  TrackRecord(const TrackRecord& trc):m_PDG_code(trc.m_PDG_code), m_Energy(trc.m_Energy), 
    m_Momentum(trc.m_Momentum), m_Position(trc.m_Position),
    m_Time(trc.m_Time), m_barCode(trc.m_barCode), m_volName(trc.m_volName){}
  /** @brief Assignement Operator */ 
  TrackRecord &operator=(const TrackRecord& trc) {
    if(this!=&trc) {
      m_PDG_code = trc.m_PDG_code; m_Energy = trc.m_Energy; 
      m_Momentum = trc.m_Momentum; m_Position = trc.m_Position;
      m_Time = trc.m_Time; m_barCode = trc.m_barCode; 
      m_volName = trc.m_volName;
    }
    return *this;
  }
 
  /** @brief Set energy */ 
  void SetEnergy(double e) {m_Energy=e;}
  /** @brief Set position */ 
  void SetPosition(CLHEP::Hep3Vector p) {m_Position=p;}
  /** @brief Set momentum */ 
  void SetMomentum(CLHEP::Hep3Vector e) {m_Momentum=e;}
  /** @brief Set PDG code */ 
  void SetPDGCode(int pcode) {m_PDG_code=pcode;}
  /** @brief Energy */ 
  double GetEnergy() const {return m_Energy;}
  /** @brief Position */ 
  CLHEP::Hep3Vector GetPosition() const {return m_Position;}
  /** @brief Momentum */ 
  CLHEP::Hep3Vector GetMomentum() const {return m_Momentum;}
  /** @brief PDG Code */ 
  int GetPDGCode() const {return m_PDG_code;}
  
  /* Davide 30-6-06 add methods and evolve this class */

  /** @brief Set time */ 
  void SetTime(double time) {m_Time=time;}
  /** @brief  Time */ 
  double GetTime() const {return m_Time;}
  /** @brief Set bar code */ 
  void SetBarCode(int theCode){m_barCode=theCode;}
  /** @brief bar code */ 
  int GetBarCode() const {return m_barCode;}
  /** @brief Set Volume name */ 
  void SetVolName(const std::string& theName){m_volName=theName;}
  /** @brief Volume name */ 
  std::string GetVolName() const {return m_volName;}
  
 private:
  int m_PDG_code;
  double m_Energy;
  CLHEP::Hep3Vector m_Momentum;
  CLHEP::Hep3Vector m_Position;
  /* Davide 30-6-06 add member and evolve this class */
  double m_Time;
  int m_barCode;
  std::string m_volName;
};

#endif
