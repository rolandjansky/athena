/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  void setBid(int bidIn){ bid=bidIn;};

  void clear();

  void setOrigin(const TGCHighPtBoard* originIn) { origin = originIn;};
  const TGCHighPtBoard* getOrigin() const;

private:
  const TGCHighPtBoard* origin;
  int bid;

  int pt[NumberOfChip][NHitInTrackSelector]; //[chip][iCandidate]
  int pos[NumberOfChip][NHitInTrackSelector];
  int hitID[NumberOfChip][NHitInTrackSelector];
  int dev[NumberOfChip][NHitInTrackSelector];
  int select[NumberOfChip][NHitInTrackSelector];
};

inline
void TGCHighPtChipOut::setDev(int chip, int iCandidate, int devIn)
{
  dev[chip][iCandidate] = devIn;
}

inline
void TGCHighPtChipOut::setPos(int chip, int iCandidate, int posIn)
{
  pos[chip][iCandidate] = posIn;
}

inline
void TGCHighPtChipOut::setHitID(int chip, int iCandidate, int hitIdIn)
{
  hitID[chip][iCandidate] = hitIdIn;
}

inline
void TGCHighPtChipOut::setPt(int chip, int iCandidate, int ptIn)
{
  pt[chip][iCandidate] =ptIn;
}

inline
const TGCHighPtBoard* TGCHighPtChipOut::getOrigin() const
{
  return origin;
}

inline
int TGCHighPtChipOut::getDev(int chip, int iCandidate) const
{
  return dev[chip][iCandidate];
}

inline
int TGCHighPtChipOut::getPos(int chip, int iCandidate) const
{
  return pos[chip][iCandidate];
}

inline
int TGCHighPtChipOut::getHitID(int chip, int iCandidate) const
{
  return hitID[chip][iCandidate];
}


inline
int TGCHighPtChipOut::getPt(int chip, int iCandidate) const
{
  return pt[chip][iCandidate];
}

inline
int TGCHighPtChipOut::getBid() const
{
  return bid;
}

inline
int TGCHighPtChipOut::getSel(int chip, int iCandidate) const
{
  return select[chip][iCandidate];
}

inline
void TGCHighPtChipOut::setSel(int chip, int iCandidate, int selIn)
{
  select[chip][iCandidate] = selIn;
}


} //end of namespace bracket

#endif // TGCHighPtChipOut_hh

