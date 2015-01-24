/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCSLPreSelectorOut_hh
#define TGCSLPreSelectorOut_hh

#include "TrigT1TGC/TGCRPhiCoincidenceMatrix.hh"

namespace LVL1TGCTrigger {
const int NCandidateInSLPreSelector = 2;

class TGCSLPreSelectorOut {
public:
  void setIdSSC(int ptLevel, int order, int idSSCIn);
  void setR(int ptLevel, int order, int rIn);
  void setPhi(int ptLevel, int order, int phiIn);
  void setDR(int ptLevel, int order, int drIn);
  void setDPhi(int ptLevel, int order, int dphiIn);
  void setInnerVeto(int ptLevel, int order, bool vetoIn);

  int getIdSSC(int ptLevel, int order) const;
  int getR(int ptLevel, int order) const;
  int getPhi(int ptLevel, int order) const;
  int getDR(int ptLevel, int order) const;
  int getDPhi(int ptLevel, int order) const;
  bool getHit(int ptLevel, int order) const;
  bool getInnerVeto(int ptLevel, int order) const;

  void print() const;
  TGCSLPreSelectorOut();
  ~TGCSLPreSelectorOut(){};

private:
  int  idSSC[NumberOfPtLevel+1][NCandidateInSLPreSelector];
  int  r[NumberOfPtLevel+1][NCandidateInSLPreSelector];
  int  phi[NumberOfPtLevel+1][NCandidateInSLPreSelector];
  bool hit[NumberOfPtLevel+1][NCandidateInSLPreSelector];
  int  dr[NumberOfPtLevel+1][NCandidateInSLPreSelector];
  int  dphi[NumberOfPtLevel+1][NCandidateInSLPreSelector];
  bool iVeto[NumberOfPtLevel+1][NCandidateInSLPreSelector];
};


inline
bool TGCSLPreSelectorOut::getHit(int ptLevel, int order) const
{
  return hit[ptLevel][order];
}

inline
void TGCSLPreSelectorOut::setIdSSC(int ptLevel, int order, int idSSCIn)
{
  idSSC[ptLevel][order] = idSSCIn;
  hit[ptLevel][order] = true;
}

inline
void TGCSLPreSelectorOut::setR(int ptLevel, int order, int rIn)
{
  r[ptLevel][order] = rIn;
}

inline
void TGCSLPreSelectorOut::setPhi(int ptLevel, int order, int phiIn)
{
  phi[ptLevel][order] = phiIn;
}

inline
void TGCSLPreSelectorOut::setDR(int ptLevel, int order, int drIn)
{
  dr[ptLevel][order] = drIn;
}

inline
void TGCSLPreSelectorOut::setDPhi(int ptLevel, int order, int dphiIn)
{
  dphi[ptLevel][order] = dphiIn;
}

inline
int TGCSLPreSelectorOut::getIdSSC(int ptLevel, int order) const
{
  return idSSC[ptLevel][order];
}

inline
int TGCSLPreSelectorOut::getR(int ptLevel, int order) const
{
  return r[ptLevel][order];
}

inline
int TGCSLPreSelectorOut::getPhi(int ptLevel, int order) const
{
  return phi[ptLevel][order];
}

inline
int TGCSLPreSelectorOut::getDR(int ptLevel, int order) const
{
  return dr[ptLevel][order];
}

inline
int TGCSLPreSelectorOut::getDPhi(int ptLevel, int order) const
{
  return dphi[ptLevel][order];
}

inline
bool TGCSLPreSelectorOut::getInnerVeto(int ptLevel, int order) const
{
  return iVeto[ptLevel][order];
}

inline
void TGCSLPreSelectorOut::setInnerVeto(int ptLevel, int order, bool vetoIn)
{
  iVeto[ptLevel][order] = vetoIn;
}


} //end of namespace bracket

#endif // TGCSLPreSelectorOut_hh
