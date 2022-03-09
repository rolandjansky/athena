/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TGCRAWDATAMONITORING_TGCCHAMBER_H
#define TGCRAWDATAMONITORING_TGCCHAMBER_H
#include "TgcBase.h"
#include <string>
namespace TGC {
  
  class TgcChamber  {
  public:
    TgcChamber() = default;

    void initChamber(const std::string& chamber_name);
    void initChamber(TGCSIDE side,int sec,int m, int phi, int eta);
    void initChamber(TGCSIDE side,int stationEta ,int stationPhi, int stationName);
    void initChamber(int stationEta ,int stationPhi, int stationName);

    const std::string& cham_name() const;
    const TGCSIDE& iSide() const;
    const TGCDETECTOR& DetId() const;
    const int& iSec() const;
    const int& iM() const;
    const int& iPhi() const;
    const int& iEta() const;
    const int& StationName() const;
    const int& StationEta() const;
    const int& StationPhi() const;
    
  private:
    std::string m_cham_name;
    TGCSIDE m_iSide{};
    TGCDETECTOR m_detId{};
    int m_iSec{};
    int m_iM{};
    int m_iPhi{};
    int m_iEta{};
    int m_stationName{};
    int m_stationEta{};
    int m_stationPhi{};
  };
  
  inline const std::string& TgcChamber::cham_name() const {return m_cham_name;}
  inline const TGCSIDE& TgcChamber::iSide() const {return m_iSide;}
  inline const TGCDETECTOR& TgcChamber::DetId() const {return m_detId;}
  inline const int& TgcChamber::iSec() const {return m_iSec;}
  inline const int& TgcChamber::iM() const {return m_iM;}
  inline const int& TgcChamber::iPhi() const {return m_iPhi;}
  inline const int& TgcChamber::iEta() const {return m_iEta;}
  inline const int& TgcChamber::StationName() const {return m_stationName;}
  inline const int& TgcChamber::StationEta() const {return m_stationEta;}
  inline const int& TgcChamber::StationPhi() const {return m_stationPhi;}
  
}
#endif
