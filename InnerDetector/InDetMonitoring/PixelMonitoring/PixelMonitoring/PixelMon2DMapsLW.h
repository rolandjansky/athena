/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELMON2DMAPSLW_H_
#define PIXELMON2DMAPSLW_H_
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include <string.h>

class TH2F_LW;
class Identifier;
class PixelID;
class StatusCode;

// A helper class to remove a lot of the code duplication.
// This is a collection of 5 2D histograms which make up the '2D mapsLW' used a lot of in the monitoring.
// Each of the 2D mapsLW correspond to a detector region (B0, B1, B2, ECA, ECC).
// This books and formats the histograms in the constructor. The fill method will take the identifier 
// as the input and fill the correct histogram and bin. The histograms are also public so that they
// can be formated/accessed like any other histograms in the monitoring.

class PixelMon2DMapsLW
{
   public:
  PixelMon2DMapsLW(std::string name, std::string title, bool doIBL, bool errorHist);
      ~PixelMon2DMapsLW();
      TH2F_LW* IBL;
      TH2F_LW* IBL2D;
      TH2F_LW* IBL3D;
      TH2F_LW* B0;
      TH2F_LW* B1;
      TH2F_LW* B2;
      TH2F_LW* A;
      TH2F_LW* C;
      TH2F_LW* DBMA;
      TH2F_LW* DBMC;
      void Fill(Identifier &id, const PixelID* pixID, bool doIBL, bool errorHist);
      void FillNormalized(PixelMon2DMapsLW* old, int nevent);
      void WeightingFill(Identifier &id, const PixelID* pixID, bool doIBL, float weight);
      void Scale(double number);
      StatusCode regHist(ManagedMonitorToolBase::MonGroup &group, bool doIBL, bool errorHist);
private:
      StatusCode sc;
      void formatHist(bool doIBL, bool errorHist);
      
};

#endif
