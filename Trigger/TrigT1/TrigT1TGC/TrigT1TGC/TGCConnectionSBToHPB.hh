/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//  Table of connection between Slave Board and High-Pt Board.
#ifndef TGCConnectionSBToHPB_hh
#define TGCConnectionSBToHPB_hh

#include "TrigT1TGC/TGCBoardConnection.hh"
#include "TrigT1TGC/TGCNumbering.hh"

namespace LVL1TGCTrigger {

class TGCConnectionSBToHPB : public TGCBoardConnection {
public:
  int getHPBPortToSB(int type, int index) const;
  void setHPBPortToSB(int type, int index, int port);

  int getHPBIdToSB(int type, int index) const;
  void setHPBIdToSB(int type, int index, int id);

  TGCConnectionSBToHPB();
  virtual ~TGCConnectionSBToHPB();
  TGCConnectionSBToHPB(const TGCConnectionSBToHPB& right);
  TGCConnectionSBToHPB& operator=(const TGCConnectionSBToHPB& right);

private:
  int* HPBPortToSB[NumberOfSlaveBoardType];
  int* HPBIdToSB[NumberOfSlaveBoardType];
};

inline
int TGCConnectionSBToHPB::getHPBPortToSB(int type, int index) const
{
  return HPBPortToSB[type][index];
}

inline
void TGCConnectionSBToHPB::setHPBPortToSB(int type, int index, int port)
{
  if(HPBPortToSB[type]==0) HPBPortToSB[type] = new int [numberOfBoard[type]];
  HPBPortToSB[type][index] = port;
}

inline
int TGCConnectionSBToHPB::getHPBIdToSB(int type, int index) const
{
  return HPBIdToSB[type][index];
}

inline
void TGCConnectionSBToHPB::setHPBIdToSB(int type, int index, int id)
{
  if(HPBIdToSB[type]==0) HPBIdToSB[type] = new int [numberOfBoard[type]];
  HPBIdToSB[type][index] = id;
}

} //end of namespace bracket

#endif // TGCConnectionSBToHPB_hh
