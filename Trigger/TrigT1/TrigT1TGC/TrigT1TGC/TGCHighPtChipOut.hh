/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCHighPtChipOut_hh
#define TGCHighPtChipOut_hh

#include "TrigT1TGC/TGCHighPtBoard.hh"


namespace LVL1TGCTrigger {
const int NHitInTrackSelector = 2;
enum { FirstCandidate, SecondCandidate, NCandidate };

class TGCHighPtChipOut {

public:
  TGCHighPtChipOut(TGCHighPtBoard* hpt, int bidIn);
  TGCHighPtChipOut();
  TGCHighPtChipOut(const TGCHighPtChipOut& );
  ~TGCHighPtChipOut();

  TGCHighPtChipOut& operator=(const TGCHighPtChipOut&);

  int getPt(int chip, int iCandidate) const;
  int getPos(int chip, int iCandidate) const;
  int getHitID(int chip, int iCandidate) const;
  int getDev(int chip, int iCandidate) const;
  int getSel(int chip, int iCandidate) const;

  void setPt(int chip, int iCandidate, int ptIn);
  void setPos(int chip, int iCandidate, int posIn);
  void setHitID(int chip, int iCandidate, int hitIdIn);
  void setDev(int chip, int iCandidate, int devIn);
  void setSel(int chip, int iCandidate, int selIn);

  int getBid() const;
  void setBid(int bidIn){ m_bid=bidIn;};

  void clear();

  void setOrigin(const TGCHighPtBoard* originIn) { m_origin = originIn;};
  const TGCHighPtBoard* getOrigin() const;

private:
  const TGCHighPtBoard* m_origin;
  int m_bid;

  int m_pt[NumberOfChip][NHitInTrackSelector]; //[chip][iCandidate]
  int m_pos[NumberOfChip][NHitInTrackSelector];
  int m_hitID[NumberOfChip][NHitInTrackSelector];
  int m_dev[NumberOfChip][NHitInTrackSelector];
  int m_select[NumberOfChip][NHitInTrackSelector];
};

inline
void TGCHighPtChipOut::setDev(int chip, int iCandidate, int devIn)
{
  m_dev[chip][iCandidate] = devIn;
}

inline
void TGCHighPtChipOut::setPos(int chip, int iCandidate, int posIn)
{
  m_pos[chip][iCandidate] = posIn;
}

inline
void TGCHighPtChipOut::setHitID(int chip, int iCandidate, int hitIdIn)
{
  m_hitID[chip][iCandidate] = hitIdIn;
}

inline
void TGCHighPtChipOut::setPt(int chip, int iCandidate, int ptIn)
{
  m_pt[chip][iCandidate] =ptIn;
}

inline
const TGCHighPtBoard* TGCHighPtChipOut::getOrigin() const
{
  return m_origin;
}

inline
int TGCHighPtChipOut::getDev(int chip, int iCandidate) const
{
  return m_dev[chip][iCandidate];
}

inline
int TGCHighPtChipOut::getPos(int chip, int iCandidate) const
{
  return m_pos[chip][iCandidate];
}

inline
int TGCHighPtChipOut::getHitID(int chip, int iCandidate) const
{
  return m_hitID[chip][iCandidate];
}


inline
int TGCHighPtChipOut::getPt(int chip, int iCandidate) const
{
  return m_pt[chip][iCandidate];
}

inline
int TGCHighPtChipOut::getBid() const
{
  return m_bid;
}

inline
int TGCHighPtChipOut::getSel(int chip, int iCandidate) const
{
  return m_select[chip][iCandidate];
}

inline
void TGCHighPtChipOut::setSel(int chip, int iCandidate, int selIn)
{
  m_select[chip][iCandidate] = selIn;
}


} //end of namespace bracket

#endif // TGCHighPtChipOut_hh

