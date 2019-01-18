/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCRPhiCoincidenceOut_hh
#define TGCRPhiCoincidenceOut_hh
#include "TrigT1TGC/TGCRPhiCoincidenceMatrix.hh"

namespace LVL1TGCTrigger {

class TGCRPhiCoincidenceOut {

public:
  TGCRPhiCoincidenceOut();
  ~TGCRPhiCoincidenceOut() = default;

  int  getPhi() const { return m_phi;};
  int  getR() const { return m_r;};
  int  getIdSSC() const { return m_idSSC;};
  int  getDR() const { return m_dR; };
  int  getDPhi() const { return m_dPhi; };
  bool getInnerVeto() const { return m_innerVeto; };

  bool getHit(int ptIn) const { return m_hit[ptIn];};
  void setHit(int ptIn){ m_hit[ptIn]=true;};
  void clearHit(int ptIn) { m_hit[ptIn] = false;};
  int  getPtLevel() const;

  void setIdSSC(int idSSCIn){ m_idSSC = idSSCIn;};
  void setR(int rIn){ m_r=rIn;};
  void setPhi(int phiIn){ m_phi=phiIn;};
  void setDR(int drIn) { m_dR = drIn; };
  void setDPhi(int dphiIn) { m_dPhi = dphiIn; };
  void setInnerVeto(bool vetoIn) { m_innerVeto = vetoIn; };

  void print() const;
  void clear();
  bool hasHit() const;

  bool isSuperior(const TGCRPhiCoincidenceOut* right) const;

private:
  int m_idSSC;
  bool m_hit[NumberOfPtLevel+1];
  int m_phi;
  int m_r;
  int m_dR;
  int m_dPhi;
  bool m_innerVeto;
};

inline
 int  TGCRPhiCoincidenceOut::getPtLevel() const
{
  int pt = 0;
  if      (m_hit[6]) pt =6;
  else if (m_hit[5]) pt =5;
  else if (m_hit[4]) pt =4;
  else if (m_hit[3]) pt =3;
  else if (m_hit[2]) pt =2;
  else if (m_hit[1]) pt =1;
  else pt =0;
  return pt;
}

} //end of namespace bracket

#endif // TGCRPhiCoincidenceOut_hh
