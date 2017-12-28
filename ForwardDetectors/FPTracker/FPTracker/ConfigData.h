/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FPTRACKER_CONDIFGDATA_H
#define  FPTRACKER_CONDIFGDATA_H
#include <ostream>
namespace FPTracker{
  class ConfigData{
  public:
    
    int    IP;
//    float  xinner0;
    bool   useaper;
    double apermb;
    double xcol1;
    double xcol2;
    double pbeam0;
    double brho;
      
    float  endMarker;
    
    float  absZMagMax; // do not read in mags with abs above this value

    static const double speedOfLight;



    ConfigData(); 
    std::string toString() const;
    void setpbeam(double);

  };
  std::ostream& operator<<(std::ostream&, const ConfigData&);
}
#endif
