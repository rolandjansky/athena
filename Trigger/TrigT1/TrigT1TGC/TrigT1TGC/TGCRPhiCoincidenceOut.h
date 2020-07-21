/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCRPhiCoincidenceOut_hh
#define TGCRPhiCoincidenceOut_hh
#include "TrigT1TGC/TGCRPhiCoincidenceMatrix.h"

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

  //// New variables for Run3
  int getCharge() const {return m_charge;};
  bool getCoincidenceType() const {return m_coincidenceTypeFlag;};
  bool getGoodMFFlag() const {return m_goodMFFlag;};
  bool getInnerCoincidenceFlag() const {return m_innerCoincidenceFlag;};

  bool getHit(int ptIn) const { return m_hit[ptIn];};
  int  getpT() const {return m_pT;}; // for Run3 Algo
  void setHit(int ptIn){ m_hit[ptIn]=true;};
  void clearHit(int ptIn) { m_hit[ptIn] = false;};
  int  getPtLevel() const; // for Run2 Algo
  int  getRoI() const{ return m_RoI; }

  void setIdSSC(int idSSCIn){ m_idSSC = idSSCIn;};
  void setpT(int pTIn){ m_pT=pTIn;};
  void setR(int rIn){ m_r=rIn;};
  void setPhi(int phiIn){ m_phi=phiIn;};
  void setDR(int drIn) { m_dR = drIn; };
  void setDPhi(int dphiIn) { m_dPhi = dphiIn; };
  void setInnerVeto(bool vetoIn) { m_innerVeto = vetoIn; };
  void setRoI(int RoIIn) {m_RoI=RoIIn; };

  //// New variables for Run3
  void setCharge(int chargeIn){m_charge=chargeIn;};
  void setCoincidenceType(int CoincidenceTypeIn){m_coincidenceTypeFlag=CoincidenceTypeIn;};
  void setGoodMFFlag(bool goodMFFlagIn){m_goodMFFlag=goodMFFlagIn;};
  void setInnerCoincidenceFlag(bool InnerCoincidenceFlagIn){m_innerCoincidenceFlag=InnerCoincidenceFlagIn;};



  void print() const;
  void clear();
  bool hasHit() const;

  bool isSuperior(const TGCRPhiCoincidenceOut* right) const;

private:
  int m_idSSC;
  bool m_hit[NumberOfPtLevel+1]; // for Run2 Algo
  int m_pT; // for Run3 Algo
  int m_phi;
  int m_r;
  int m_dR;
  int m_dPhi;
  bool m_innerVeto;
  int m_RoI;
  int m_charge;
  bool m_coincidenceTypeFlag;
  bool m_goodMFFlag;
  bool m_innerCoincidenceFlag;

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
