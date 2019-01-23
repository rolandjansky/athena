/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//$Id: TGCPatchPanelOut.hh,v 1.1.1.10 2003-11-03 10:18:17 mishino Exp $
#ifndef TGCPatchPanelOut_hh
#define TGCPatchPanelOut_hh

#include "TrigT1TGC/TGCPatchPanel.hh"
#include "TrigT1TGC/TGCHitPattern.hh"

namespace LVL1TGCTrigger {

class TGCPatchPanelOut {

public:
  TGCPatchPanelOut();
  ~TGCPatchPanelOut();

  TGCPatchPanelOut(const TGCPatchPanelOut& right);
  TGCPatchPanelOut& operator=(const TGCPatchPanelOut& right);

  TGCHitPattern* getHitPattern(int connector) const;
  void setHitPattern(int connector, TGCHitPattern* sig);

  const TGCPatchPanel* getOrigin() const;
  void setOrigin(const TGCPatchPanel* pp);

  int getBid() const;
  void setBid( int bidIn);
  
  void print() const;
  void deleteHitPattern(int i);
  void clear();

private:
  int m_bid;
  const TGCPatchPanel* m_origin; // pointer to Patch Panel generate this PatchPanelOut.
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
void TGCPatchPanelOut::setBid(int bidIn) 
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





