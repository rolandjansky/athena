/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELMON2DLUMIPROFILES_H_
#define PIXELMON2DLUMIPROFILES_H_
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include <array>
#include <string.h>

class TProfile2D_LW;
class Identifier;
class PixelID;
class StatusCode;

namespace PixMon {
enum class HistConf;
}

// A helper class to remove a lot of the code duplication.
// This is a collection of 6 2D histograms which make up the '2D lumi profiles' used a lot of in the monitoring.
// Each of the 2D maps correspond to a detector region (IBL, B0, B1, B2, ECA, ECC).
// This books and formats the histograms in the constructor. The fill method will take the identifier 
// as the input and fill the correct histogram and bin. The histograms are also public so that they
// can be formated/accessed like any other histograms in the monitoring.
//
// N.B. Currently no support for IBL2D/3D histograms as well as DBM monitoring. Should this be added?
// Also, if the histogram configuration contains unsupported histograms, this is completely ignored.
// Should we do anything against it?

class PixelMon2DLumiProfiles
{
   public:
      PixelMon2DLumiProfiles(std::string name, std::string title, std::string zlabel, const PixMon::HistConf& config);
      ~PixelMon2DLumiProfiles();
      TProfile2D_LW* IBLlbp;
      TProfile2D_LW* B0lbp;
      TProfile2D_LW* B1lbp;
      TProfile2D_LW* B2lbp;
      TProfile2D_LW* Albp;
      TProfile2D_LW* Clbp;
      TProfile2D_LW* DBMAlbp;
      TProfile2D_LW* DBMClbp;
      void Fill(double LB, Identifier &id, const PixelID* pixID, double weight = 1);
      StatusCode regHist(ManagedMonitorToolBase::MonGroup &group);
private:
      void formatHist();
      std::array<TProfile2D_LW*, 10> m_histograms;
      const PixMon::HistConf m_config;
      static const bool m_doIBL;
};

#endif
