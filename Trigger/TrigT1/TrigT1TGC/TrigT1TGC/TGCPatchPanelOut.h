/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCPatchPanelOut_hh
#define TGCPatchPanelOut_hh

#include "TrigT1TGC/TGCHitPattern.h"
#include "MuonDigitContainer/TgcDigit.h"

namespace LVL1TGCTrigger {

const int NumberOfConnectorPerPPOut = 2;

class TGCPatchPanel;

class TGCPatchPanelOut {
 public:
  TGCPatchPanelOut();
  ~TGCPatchPanelOut() = default;
  TGCPatchPanelOut(const TGCPatchPanelOut& right) = default;
  TGCPatchPanelOut& operator=(const TGCPatchPanelOut& right);

  TGCHitPattern* getHitPattern(int connector) const;
  void setHitPattern(int connector, TGCHitPattern* sig);

  const TGCPatchPanel* getOrigin() const;
  void setOrigin(const TGCPatchPanel* pp);

  int getBid() const;
  void setBid(const int bidIn);
  
  void print() const;
  void deleteHitPattern(int i);
  void clear();

 private:
  int m_bid{TgcDigit::BC_UNDEFINED};        ///< bunch ID number
  const TGCPatchPanel* m_origin{nullptr};   ///< pointer to Patch Panel generate this PatchPanelOut
  TGCHitPattern* m_signalPattern[NumberOfConnectorPerPPOut];
};


inline
TGCHitPattern* TGCPatchPanelOut::getHitPattern(int connector) const
{
  return m_signalPattern[connector];
}

inline
const TGCPatchPanel* TGCPatchPanelOut::getOrigin() const
{
  return m_origin;
}

inline
void TGCPatchPanelOut::setBid(const int bidIn) 
{
  m_bid = bidIn; 
}

inline
void TGCPatchPanelOut::setHitPattern(int connector, TGCHitPattern* sig)
{
  m_signalPattern[connector] = sig;
}

inline
int TGCPatchPanelOut::getBid() const
{
  return m_bid;
}

inline
void TGCPatchPanelOut::setOrigin(const TGCPatchPanel* pp)
{
  m_origin = pp;
}


} //end of namespace bracket

#endif // TGCPatchPanelOut_hh





