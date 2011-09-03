/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDC_PIXEL_SIMHIT
#define ZDC_PIXEL_SIMHIT

class ZDC_SimPixelHit
{
  public:
  
  ZDC_SimPixelHit() { //must have a constructor with no argument as otherwise there is some problem from the ".xml" file
    
    Side = -1; ModNo = -1; PixNo = -1; Edep = -1; Nphotons = -1;
  }
  
  ZDC_SimPixelHit(int side, int modno, int pixno, int nphot, double edep) {
    
    Side = side; ModNo = modno; PixNo = pixno; Nphotons = nphot; Edep = edep;
  }
  
  int    GetSide()     const { return Side; }
  int    GetMod()      const { return ModNo; }
  int    GetPix()      const { return PixNo; }
  double GetEdep()     const { return Edep; }
  int    GetNPhotons() const { return Nphotons; }

  const ZDC_SimPixelHit& operator = (const ZDC_SimPixelHit&);

  int   operator == (const ZDC_SimPixelHit&) const;
  bool  operator <  (const ZDC_SimPixelHit&) const;

 private:
  
  int    Side, ModNo, PixNo;
  int    Nphotons;
  double Edep;
};

#endif


