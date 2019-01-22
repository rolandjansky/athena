/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCHighPtBoardOut_hh
#define TGCHighPtBoardOut_hh

#include "TrigT1TGC/TGCHighPtBoard.hh"

namespace LVL1TGCTrigger {

const int MaxNumberOfHPBData  = 6;
const int SizeOfHitID  = 3;
const int SizeOfDeltaR = 4;

enum TGCHPBBlock { 
  HPBBlockA, HPBBlockB, HPBBlockC, 
  HPBBlockD, HPBBlockE, HPBBlockF, 
  TotalNumTGCHPBBlock };

inline TGCHPBBlock operator++( TGCHPBBlock &rs, int )
{
    return rs = (TGCHPBBlock)(rs + 1);
}

class TGCHighPtBoardOut {

public:
  TGCHighPtBoardOut(TGCHighPtBoard* hpt, int bidIn);
  TGCHighPtBoardOut();
  virtual ~TGCHighPtBoardOut() = default;

  int getPt(int chip, int iData) const;
  int getPos(int chip, int iData) const;
  int getDev(int chip, int iData) const;
  bool getHit(int chip, int iData) const;
  int getSel(int chip, int iData) const;

  void setPt(int chip, int iData, int ptIn);
  void setPos(int chip, int iData, int posIn);
  void setDev(int chip, int iData, int devIn);
  void setHit(int chip, int iData, bool hitIn);
  void setSel(int chip, int iData, int selIn);

  int getBid() const;
  void setBid(int bidIn){ m_bid=bidIn;};

  void clear();

  void setOrigin(const TGCHighPtBoard* originIn) { m_origin = originIn;};
  const TGCHighPtBoard* getOrigin() const;

private:
  const TGCHighPtBoard* m_origin;
  int m_bid;

  int m_pt[NumberOfChip][MaxNumberOfHPBData]; //[chip][block]
  int m_pos[NumberOfChip][MaxNumberOfHPBData];
  int m_dev[NumberOfChip][MaxNumberOfHPBData];
  bool m_hit[NumberOfChip][MaxNumberOfHPBData];
  // enable when the track is selected in trackselector. 
  int m_select[NumberOfChip][MaxNumberOfHPBData];
};

inline
void TGCHighPtBoardOut::setDev(int chip, int iData, int devIn)
{
  if(!m_hit[chip][iData]) m_hit[chip][iData]=true; //[chip][block]
  m_dev[chip][iData] = devIn;
}

inline
void TGCHighPtBoardOut::setPos(int chip, int iData, int posIn)
{
  if(!m_hit[chip][iData]) m_hit[chip][iData]=true;
  m_pos[chip][iData] = posIn;
}

inline
void TGCHighPtBoardOut::setPt(int chip, int iData, int ptIn)
{
  if(!m_hit[chip][iData]) m_hit[chip][iData]=true;
  m_pt[chip][iData] =ptIn;
}

inline
const TGCHighPtBoard* TGCHighPtBoardOut::getOrigin() const
{
  return m_origin;
}

inline
int TGCHighPtBoardOut::getDev(int chip, int iData) const
{
  return m_dev[chip][iData];
}

inline
int TGCHighPtBoardOut::getPos(int chip, int iData) const
{
  return m_pos[chip][iData];
}

inline
int TGCHighPtBoardOut::getPt(int chip, int iData) const
{
  return m_pt[chip][iData];
}

inline
int TGCHighPtBoardOut::getBid() const
{
  return m_bid;
}

inline
bool TGCHighPtBoardOut::getHit(int chip, int iData) const
{
  return m_hit[chip][iData];
}

inline
void TGCHighPtBoardOut::setHit(int chip, int iData, bool hitIn)
{
  m_hit[chip][iData] = hitIn;
}

inline
int TGCHighPtBoardOut::getSel(int chip, int iData) const
{
  return m_select[chip][iData];
}

inline
void TGCHighPtBoardOut::setSel(int chip, int iData, int selIn)
{
  m_select[chip][iData] = selIn;
}

} //end of namespace bracket

#endif // TGCHighPtBoardOut_hh






