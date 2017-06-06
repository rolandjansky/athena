/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELMON2DLUMIMAPS_H_
#define PIXELMON2DLUMIMAPS_H_
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include <string.h>

class TH2F_LW;
class Identifier;
class PixelID;
class StatusCode;

// A helper class to remove a lot of the code duplication.
// This is a collection of 5 2D histograms which make up the '2D maps' used a lot of in the monitoring.
// Each of the 2D maps correspond to a detector region (B0, B1, B2, ECA, ECC).
// This books and formats the histograms in the constructor. The fill method will take the identifier 
// as the input and fill the correct histogram and bin. The histograms are also public so that they
// can be formated/accessed like any other histograms in the monitoring.
 
class PixelMon2DLumiMaps
{
   public:
  PixelMon2DLumiMaps(std::string name, std::string title, std::string zlabel,bool doIBL=false,bool errorHist=false);
      ~PixelMon2DLumiMaps();
      TH2F_LW* IBLlbm;
      TH2F_LW* B0lbm;
      TH2F_LW* B1lbm;
      TH2F_LW* B2lbm;
      TH2F_LW* Albm;
      TH2F_LW* Clbm;
      void Fill(double LB,Identifier &id, const PixelID* pixID,double weight=1, bool doIBL=false, bool errorHist=false);
      void Scale(double number,bool doIBL,bool errorHist);
      StatusCode regHist(ManagedMonitorToolBase::MonGroup &group,bool doIBL=false,bool errorHist=false);
private:
      void formatHist(bool doIBL=false,bool errorHist=false);
      
};

#endif
