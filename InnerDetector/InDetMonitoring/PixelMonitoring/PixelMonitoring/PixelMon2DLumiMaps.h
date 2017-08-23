/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELMON2DLUMIMAPS_H_
#define PIXELMON2DLUMIMAPS_H_
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include <array>
#include <string.h>

class TH2F_LW;
class Identifier;
class PixelID;
class StatusCode;

namespace PixMon {
enum class HistConf;
}

// A helper class to remove a lot of the code duplication.
// This is a collection of 6 2D histograms which make up the '2D luminosity maps' used a lot of in the monitoring.
// Each of the 2D maps correspond to a detector region (IBL, B0, B1, B2, ECA, ECC).
// This books and formats the histograms in the constructor. The fill method will take the identifier 
// as the input and fill the correct histogram and bin. The histograms are also public so that they
// can be formated/accessed like any other histograms in the monitoring.
//
// N.B. Currently no support for IBL2D/3D histograms as well as DBM monitoring. Should this be added?
// Also, if the histogram configuration contains unsupported histograms, this is completely ignored.
// Should we do anything against it?
 
class PixelMon2DLumiMaps
{
   public:
      PixelMon2DLumiMaps(std::string name, std::string title, std::string zlabel, const PixMon::HistConf& config);
      ~PixelMon2DLumiMaps();
      TH2F_LW* IBLlbm;
      TH2F_LW* B0lbm;
      TH2F_LW* B1lbm;
      TH2F_LW* B2lbm;
      TH2F_LW* Albm;
      TH2F_LW* Clbm;
      TH2F_LW* DBMAlbm;
      TH2F_LW* DBMClbm;
      void Fill(double LB,Identifier &id, const PixelID* pixID, double weight = 1);
      StatusCode regHist(ManagedMonitorToolBase::MonGroup &group);
private:
      void formatHist();
      std::array<TH2F_LW*, 10> m_histograms;
      const PixMon::HistConf m_config;
      static const bool m_doIBL;
};

#endif
