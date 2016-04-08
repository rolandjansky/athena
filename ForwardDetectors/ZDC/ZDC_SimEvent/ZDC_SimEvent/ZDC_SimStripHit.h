/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDC_SIMSTRIPHIT
#define ZDC_SIMSTRIPHIT

class ZDC_SimStripHit
{
  
 public:
  
  ZDC_SimStripHit() { // must have a constructor with no argument as otherwise there is some problem from the ".xml" file
    
    m_Side = -1; m_ModNo = -1; m_Edep =-1; m_Nphotons = -1;
  }
  
  ZDC_SimStripHit(int side, int modno, int nphot, double edep) {
    
    m_Side = side; m_ModNo = modno; m_Edep = edep; m_Nphotons = nphot;
  }
  
  int    GetSide()     const { return m_Side; }
  int    GetMod()      const { return m_ModNo; }
  double GetEdep()     const { return m_Edep; }
  int    GetNPhotons() const { return m_Nphotons; }

  int   operator == (const ZDC_SimStripHit&) const;
  bool  operator  < (const ZDC_SimStripHit&) const;

  private:
  
  int    m_Side, m_ModNo; // The side and Module_No to which this entry corresponds to
  int    m_Nphotons;    // The number of detected cherenkov photons in this TimeBin
  double m_Edep;        // The total energy of all the cherenkov photons in this TimeBin
};

#endif


