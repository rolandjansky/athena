/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDC_SIMSTRIPHIT
#define ZDC_SIMSTRIPHIT

class ZDC_SimStripHit
{
  
 public:
  
  ZDC_SimStripHit() { // must have a constructor with no argument as otherwise there is some problem from the ".xml" file
    
    Side = -1; ModNo = -1; Edep =-1; Nphotons = -1;
  }
  
  ZDC_SimStripHit(int side, int modno, int nphot, double edep) {
    
    Side = side; ModNo = modno; Edep = edep; Nphotons = nphot;
  }
  
  int    GetSide()     const { return Side; }
  int    GetMod()      const { return ModNo; }
  double GetEdep()     const { return Edep; }
  int    GetNPhotons() const { return Nphotons; }

  const ZDC_SimStripHit& operator = (const ZDC_SimStripHit&);

  int   operator == (const ZDC_SimStripHit&) const;
  bool  operator  < (const ZDC_SimStripHit&) const;

  private:
  
  int    Side, ModNo; // The side and Module_No to which this entry corresponds to
  int    Nphotons;    // The number of detected cherenkov photons in this TimeBin
  double Edep;        // The total energy of all the cherenkov photons in this TimeBin
};

#endif


