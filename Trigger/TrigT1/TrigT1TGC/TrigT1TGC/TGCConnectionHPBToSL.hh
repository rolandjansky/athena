/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//  table of connection between High-Pt Board and Sector Logic
#ifndef TGCConnectionHPBToSL_hh
#define TGCConnectionHPBToSL_hh

#include "TrigT1TGC/TGCBoardConnection.hh"

namespace LVL1TGCTrigger {

enum TGCHighPtBoardType { WHPB=0, SHPB, TotalNumHighPtBoardType};
enum{ NumberOfHighPtBoardType = 2 };

class TGCConnectionHPBToSL : public TGCBoardConnection
{
 public:
  TGCConnectionHPBToSL();
  TGCConnectionHPBToSL(const TGCConnectionHPBToSL& right);
  virtual ~TGCConnectionHPBToSL();

  int getSLPortToHPB(int type, int index) const;
  void setSLPortToHPB(int type, int index, int port);

  TGCConnectionHPBToSL& operator=(const TGCConnectionHPBToSL& right);

 private:
  int* m_SLPortToHPB[NumberOfHighPtBoardType];
};

inline
int TGCConnectionHPBToSL::getSLPortToHPB(int type, int index) const
{
  return m_SLPortToHPB[type][index];
}

inline
void TGCConnectionHPBToSL::setSLPortToHPB(int type, int index, int port)
{
  if(m_SLPortToHPB[type]==0) m_SLPortToHPB[type] = new int [m_numberOfBoard[type]];
  m_SLPortToHPB[type][index] = port;
}
} //end of namespace bracket

#endif  // TGCConnectionHPBToSL_hh
