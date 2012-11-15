/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4TBECBeamChHit.hh

#ifndef __LArG4TBECBeamChHit_H__
#define __LArG4TBECBeamChHit_H__

class LArG4TBECBeamChHit 
{
public:

  // Constructors and destructors.
  LArG4TBECBeamChHit();
  LArG4TBECBeamChHit(int c = 0, double energy = 0., double time   = 0.);
  virtual ~LArG4TBECBeamChHit();

  void Draw();
  void Print();

private:
  int m_Chamber;
  double m_X;
  double m_Y;

public:
  // Note the lack of "Set" methods.  Once you've defined the position
  // of a hit, you can't change it.  
  // 01-Feb-2001 WGS: Add accessor methods for calorID, zSideID, and region.
  inline int getIdentifier() const { return m_Chamber; }
  inline double X() const { return m_X; }
  inline double Y() const { return m_Y; }
};


#endif // _LArG4TBECBeamChHit_H_
