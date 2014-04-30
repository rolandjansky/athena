/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCoincidence_h
#define TGCCoincidence_h

#include <vector>

namespace LVL1TGCTrigger {

class TGCCoincidence {
public:
  TGCCoincidence(int Bid, int SLid, int Module, int Region, int SSCid, int R, int Phi, int SSid,
	      int ptR, int dR, int ptPhi, int dPhi, int ptMax) :
    m_Bid(Bid), m_SLid(SLid), m_Module(Module), m_Region(Region), m_SSCid(SSCid),
    m_R(R), m_Phi(Phi), m_SSid(SSid), m_ptR(ptR), m_dR(dR), m_ptPhi(ptPhi), m_dPhi(dPhi), m_ptMax(ptMax) {};
  ~TGCCoincidence(){};
  int getBid()    { return m_Bid; }
  int getSLid()   { return m_SLid; }
  int getModule() { return m_Module; }
  int getRegion() { return m_Region; }
  int getSSCid()  { return m_SSCid; }
  int getR()      { return m_R; }
  int getPhi()    { return m_Phi; }
  int getSSid()   { return m_SSid; }
  int getPtR()    { return m_ptR; }
  int getDR()     { return m_dR; }
  int getPtPhi()  { return m_ptPhi; }
  int getDPhi()   { return m_dPhi; }
  int getPtMax()  { return m_ptMax; }
private:
  int m_Bid;
  int m_SLid;
  int m_Module;
  int m_Region;
  int m_SSCid;
  int m_R;
  int m_Phi;
  int m_SSid;
  int m_ptR;
  int m_dR;
  int m_ptPhi;
  int m_dPhi;
  int m_ptMax;
};
  typedef std::vector<TGCCoincidence*> TGCCoincidences;
}
#endif
