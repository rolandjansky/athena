/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELMONPROFILES_H_
#define PIXELMONPROFILES_H_
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include <string.h>

//class TProfile2D;
class TProfile2D_LW;
class Identifier;
class PixelID;
class StatusCode;

// A helper class to remove a lot of the code duplication.
// This is a collection of 5 2D PROFILE histograms. It books and formats the histograms in the constructor.
// The fill method will take the identifier as the input and fill the correct histogram and bin.
// The histograms are also public so that they can be formated/accessed just like any other histograms in the monitoring.

class PixelMonProfiles
{
   public:
      PixelMonProfiles(std::string name, std::string title);
      ~PixelMonProfiles();
      TProfile2D_LW* IBL2D;
      TProfile2D_LW* IBL3D;
      TProfile2D_LW* IBL;
      TProfile2D_LW* B0;
      TProfile2D_LW* B1;
      TProfile2D_LW* B2;
      TProfile2D_LW* A;
      TProfile2D_LW* C;
      void Fill(Identifier &id, const PixelID* pixID, float Index, bool doIBL);
      void SetMaxValue(float max);
      StatusCode regHist(ManagedMonitorToolBase::MonGroup &group);
private:
      StatusCode sc;
      void formatHist();
      
};

#endif
