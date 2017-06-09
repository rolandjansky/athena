/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELMON2DPROFILESLW_H_
#define PIXELMON2DPROFILESLW_H_
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include <string.h>

class TProfile2D_LW;
class TH2F_LW;
class Identifier;
class PixelID;
class StatusCode;
class PixelMon2DMaps;
class PixelMon2DMapsLW;

// A helper class to facilitate definition of per-layer 2D profile maps.
// It defines a collection of TProfile2D_LW histograms for each pixel layer, which then can be declared or
// filled in a single call.
// This books and formats the histograms in the constructor. The fill method will take the identifier 
// as the input and fill the correct histogram and bin. The histograms are also public so that they
// can be formated/accessed like any other histograms in the monitoring.
//
// N.B. the type of histograms is the same as for PixelMonProfiles class,
// but the filling methods are somewhat different. Candidate for cleanup.

class PixelMon2DProfilesLW
{
public:
  PixelMon2DProfilesLW(std::string name, std::string title, bool doIBL, bool errorHist, bool copy2DFEval);
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
  void Fill(Identifier &id, const PixelID* pixID, float value);
  void Fill2DMon(PixelMon2DProfilesLW* oldmap);
  void FillFromMap(PixelMon2DMaps* inputmap, bool clear_inputmap);
  StatusCode regHist(ManagedMonitorToolBase::MonGroup &group);
private:
  void formatHist();
  bool mDoIBL;
  bool mDoErrorHist;
  bool mCopy2DFEval;
};

#endif
