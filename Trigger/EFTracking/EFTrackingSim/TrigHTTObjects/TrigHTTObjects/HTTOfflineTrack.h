/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHTTOBJECTS_HTTOFFLINETRACK_H
#define TRIGHTTOBJECTS_HTTOFFLINETRACK_H

#include "TrigHTTObjects/HTTOfflineHit.h"
#include <cmath>
#include <TObject.h>

class HTTOfflineTrack : public TObject {
public:
  HTTOfflineTrack();

  void setQOverPt(double v) { m_qoverpt = v; }
  void setEta(double v) { m_eta = v; }
  void setPhi(double v) { m_phi = v; }
  void setD0(double v) { m_d0 = v; }
  void setZ0(double v) { m_z0 = v; }
  void setBarcode(long v) { m_barcode = v; }
  void setBarcodeFrac(float v) { m_barcode_frac = v; }

  double getPt() const { return m_qoverpt != 0 ? std::abs(sin(2 * std::atan(std::exp(-m_eta))) / m_qoverpt) : 99999999.; }
  double getEta() const { return m_eta; }
  double getPhi() const { return m_phi; }
  double getD0() const { return m_d0; }
  double getZ0() const { return m_z0; }
  double getQOverPt() const { return m_qoverpt / sin(2 * std::atan(std::exp(-m_eta))); }
  long   getBarcode() const { return m_barcode; }
  double getBarcodeFrac() const { return m_barcode_frac; }

  //  handling hits
  const std::vector<HTTOfflineHit>& getOfflineHits() const { return m_hits; }
  int   nHits() const { return m_hits.size(); }
  void  addHit(HTTOfflineHit s) { m_hits.push_back(s); }


private:
  // This is actually qoverp instead of qoverpt. (qoverpt = qoverp / sin(theta)),
  // where theta = 2 * atan(exp(-eta)). We can fix this in the next round.
  // The current workaround is to change the get functions.
  double m_qoverpt;
  double m_eta;
  double m_phi;
  double m_d0;
  double m_z0;

  long   m_barcode; // matched geant particle barcode
  double m_barcode_frac;  // largest "matching fraction" with any "good"
                          // geant particle, corresponding to the
                          // particle with m_barcode

  std::vector<HTTOfflineHit> m_hits;


  ClassDef(HTTOfflineTrack, 4)
};

std::ostream& operator<<(std::ostream&, const HTTOfflineTrack&);
#endif // TRIGHTTOBJECTS_HTTOFFLINETRACK_H
