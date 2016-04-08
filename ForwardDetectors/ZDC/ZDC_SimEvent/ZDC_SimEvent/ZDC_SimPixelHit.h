/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDC_PIXEL_SIMHIT
#define ZDC_PIXEL_SIMHIT

class ZDC_SimPixelHit
{
  public:
  
  ZDC_SimPixelHit() { //must have a constructor with no argument as otherwise there is some problem from the ".xml" file
    
    m_Side = -1; m_ModNo = -1; m_PixNo = -1; m_Edep = -1; m_Nphotons = -1;
  }
  
  ZDC_SimPixelHit(int side, int modno, int pixno, int nphot, double edep) {
    
    m_Side = side; m_ModNo = modno; m_PixNo = pixno; m_Nphotons = nphot; m_Edep = edep;
  }
  
  int    GetSide()     const { return m_Side; }
  int    GetMod()      const { return m_ModNo; }
  int    GetPix()      const { return m_PixNo; }
  double GetEdep()     const { return m_Edep; }
  int    GetNPhotons() const { return m_Nphotons; }

  int   operator == (const ZDC_SimPixelHit&) const;
  bool  operator <  (const ZDC_SimPixelHit&) const;

 private:
  
  int    m_Side, m_ModNo, m_PixNo;
  int    m_Nphotons;
  double m_Edep;
};

#endif


