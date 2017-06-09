/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELMON2DLUMIPROFILES_H_
#define PIXELMON2DLUMIPROFILES_H_
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include <string.h>

class TProfile2D_LW;
class Identifier;
class PixelID;
class StatusCode;

// A helper class to remove a lot of the code duplication.
// This is a collection of 5 2D histograms which make up the '2D maps' used a lot of in the monitoring.
// Each of the 2D maps correspond to a detector region (B0, B1, B2, ECA, ECC).
// This books and formats the histograms in the constructor. The fill method will take the identifier 
// as the input and fill the correct histogram and bin. The histograms are also public so that they
// can be formated/accessed like any other histograms in the monitoring.

class PixelMon2DLumiProfiles
{
   public:
  PixelMon2DLumiProfiles(std::string name, std::string title, std::string zlabel,bool doIBL=false,bool errorHist=false);
      ~PixelMon2DLumiProfiles();
      TProfile2D_LW* IBLlbp;
      TProfile2D_LW* B0lbp;
      TProfile2D_LW* B1lbp;
      TProfile2D_LW* B2lbp;
      TProfile2D_LW* Albp;
      TProfile2D_LW* Clbp;
      void Fill(double LB, Identifier &id, const PixelID* pixID,double weight=1,bool doIBL=false,bool errorHist=false);
      StatusCode regHist(ManagedMonitorToolBase::MonGroup &group,bool doIBL=false,bool errorHist=false);
private:
      void formatHist(bool doIBL,bool errorHist);
      
};

#endif
