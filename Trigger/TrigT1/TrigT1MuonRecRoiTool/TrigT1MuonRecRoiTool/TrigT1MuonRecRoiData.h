/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1MUONRECROIDATA_H
#define TRIGT1MUONRECROIDATA_H

namespace LVL1 {

  class TrigT1MuonRecRoiData{
  public:
    TrigT1MuonRecRoiData();
    TrigT1MuonRecRoiData(const double _eta, const double _phi);
    TrigT1MuonRecRoiData(const double _eta, const double _phi,
			 const double _etaMin, const double _etaMax,
			 const double _phiMin, const double _phiMax );
    ~TrigT1MuonRecRoiData(){}

    unsigned short side(void) const {return m_side;}
    unsigned short sector(void) const {return m_sector;}
    unsigned short roi(void) const {return m_roi;}
    double eta(void) const {return m_eta;}
    double phi(void) const {return m_phi;}
    double etaMin(void) const {return m_etaMin;}
    double etaMax(void) const {return m_etaMax;}
    double phiMin(void) const {return m_phiMin;}
    double phiMax(void) const {return m_phiMax;}

    void set_side(unsigned short _side);
    void set_sector(unsigned short _sector);
    void set_roi(unsigned short _roi);
    void set_eta(double _eta);
    void set_phi(double _phi);
    void set_etaMin(double _etaMin);
    void set_etaMax(double _etaMax);
    void set_phiMin(double _phiMin);
    void set_phiMax(double _phiMax);
    void update(void);

  private:

    unsigned short m_side{0};
    unsigned short m_sector{0};
    unsigned short m_roi{0};
    double m_eta{0};
    double m_phi{0};
    double m_etaMin{0};
    double m_etaMax{0};
    double m_phiMin{0};
    double m_phiMax{0};

  }; // end of TrigT1MuonRecRoiData

} // namespace LVL1

#endif
