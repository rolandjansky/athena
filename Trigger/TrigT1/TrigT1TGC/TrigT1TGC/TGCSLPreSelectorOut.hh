/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
  int  m_idSSC[NumberOfPtLevel+1][NCandidateInSLPreSelector];
  int  m_r[NumberOfPtLevel+1][NCandidateInSLPreSelector];
  int  m_phi[NumberOfPtLevel+1][NCandidateInSLPreSelector];
  bool m_hit[NumberOfPtLevel+1][NCandidateInSLPreSelector];
  int  m_dr[NumberOfPtLevel+1][NCandidateInSLPreSelector];
  int  m_dphi[NumberOfPtLevel+1][NCandidateInSLPreSelector];
  bool m_iVeto[NumberOfPtLevel+1][NCandidateInSLPreSelector];
};


inline
bool TGCSLPreSelectorOut::getHit(int ptLevel, int order) const
{
  return m_hit[ptLevel][order];
}

inline
void TGCSLPreSelectorOut::setIdSSC(int ptLevel, int order, int idSSCIn)
{
  m_idSSC[ptLevel][order] = idSSCIn;
  m_hit[ptLevel][order] = true;
}

inline
void TGCSLPreSelectorOut::setR(int ptLevel, int order, int rIn)
{
  m_r[ptLevel][order] = rIn;
}

inline
void TGCSLPreSelectorOut::setPhi(int ptLevel, int order, int phiIn)
{
  m_phi[ptLevel][order] = phiIn;
}

inline
void TGCSLPreSelectorOut::setDR(int ptLevel, int order, int drIn)
{
  m_dr[ptLevel][order] = drIn;
}

inline
void TGCSLPreSelectorOut::setDPhi(int ptLevel, int order, int dphiIn)
{
  m_dphi[ptLevel][order] = dphiIn;
}

inline
int TGCSLPreSelectorOut::getIdSSC(int ptLevel, int order) const
{
  return m_idSSC[ptLevel][order];
}

inline
int TGCSLPreSelectorOut::getR(int ptLevel, int order) const
{
  return m_r[ptLevel][order];
}

inline
int TGCSLPreSelectorOut::getPhi(int ptLevel, int order) const
{
  return m_phi[ptLevel][order];
}

inline
int TGCSLPreSelectorOut::getDR(int ptLevel, int order) const
{
  return m_dr[ptLevel][order];
}

inline
int TGCSLPreSelectorOut::getDPhi(int ptLevel, int order) const
{
  return m_dphi[ptLevel][order];
}

inline
bool TGCSLPreSelectorOut::getInnerVeto(int ptLevel, int order) const
{
  return m_iVeto[ptLevel][order];
}

inline
void TGCSLPreSelectorOut::setInnerVeto(int ptLevel, int order, bool vetoIn)
{
  m_iVeto[ptLevel][order] = vetoIn;
}


} //end of namespace bracket

#endif // TGCSLPreSelectorOut_hh
