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

  int  getPhi() const { return phi;};
  int  getR() const { return r;};
  int  getIdSSC() const { return idSSC;};
  int  getDR() const { return dR; };
  int  getDPhi() const { return dPhi; };
  bool getInnerVeto() const { return innerVeto; };

  bool getHit(int ptIn) const { return hit[ptIn];};
  void setHit(int ptIn){ hit[ptIn]=true;};
  void clearHit(int ptIn) { hit[ptIn] = false;};
  int  getPtLevel() const;

  void setIdSSC(int idSSCIn){ idSSC = idSSCIn;};
  void setR(int rIn){ r=rIn;};
  void setPhi(int phiIn){ phi=phiIn;};
  void setDR(int drIn) { dR = drIn; };
  void setDPhi(int dphiIn) { dPhi = dphiIn; };
  void setInnerVeto(bool vetoIn) { innerVeto = vetoIn; };

  void print() const;
  void clear();
  bool hasHit() const;

  bool isSuperior(const TGCRPhiCoincidenceOut* right) const;

private:
  int idSSC;
  bool hit[NumberOfPtLevel+1];
  int phi;
  int r;
  int dR;
  int dPhi;
  bool innerVeto;
};

inline
 int  TGCRPhiCoincidenceOut::getPtLevel() const
{
  int pt = 0;
  if      (hit[6]) pt =6;
  else if (hit[5]) pt =5;
  else if (hit[4]) pt =4;
  else if (hit[3]) pt =3;
  else if (hit[2]) pt =2;
  else if (hit[1]) pt =1;
  else pt =0;
  return pt;
}

} //end of namespace bracket

#endif // TGCRPhiCoincidenceOut_hh
