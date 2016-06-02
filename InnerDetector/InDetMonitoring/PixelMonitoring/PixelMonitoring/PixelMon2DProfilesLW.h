/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELMON2DPROFILESLW_H_
#define PIXELMON2DPROFILESLW_H_
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include <string.h>

class TProfile2D_LW;
class Identifier;
class PixelID;
class StatusCode;

// A helper class to remove a lot of the code duplication.
// This is a collection of 5 2D histograms which make up the '2D mapsLW' used a lot of in the monitoring.
// Each of the 2D mapsLW correspond to a detector region (B0, B1, B2, ECA, ECC).
// This books and formats the histograms in the constructor. The fill method will take the identifier 
// as the input and fill the correct histogram and bin. The histograms are also public so that they
// can be formated/accessed like any other histograms in the monitoring.

class PixelMon2DProfilesLW
{
public:
  PixelMon2DProfilesLW(std::string name, std::string title, bool doIBL, bool errorHist);
  ~PixelMon2DProfilesLW();
  TProfile2D_LW* IBL;
  TProfile2D_LW* IBL2D;
  TProfile2D_LW* IBL3D;
  TProfile2D_LW* B0;
  TProfile2D_LW* B1;
  TProfile2D_LW* B2;
  TProfile2D_LW* A;
  TProfile2D_LW* C;
  //TProfile2D_LW* DBMA;
  //TProfile2D_LW* DBMC;
  void Fill(Identifier &id, const PixelID* pixID, bool doIBL, bool errorHist, float weight);
  void Fill2DMon(PixelMon2DProfilesLW* oldmap);
  StatusCode regHist(ManagedMonitorToolBase::MonGroup &group, bool doIBL, bool errorHist);
private:
  StatusCode sc;
  void formatHist(bool doIBL, bool errorHist);
};

#endif
