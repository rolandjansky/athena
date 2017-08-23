/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELMON2DMAPSLW_H_
#define PIXELMON2DMAPSLW_H_
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
// This is a collection of 5 2D histograms which make up the '2D mapsLW' used a lot of in the monitoring.
// Each of the 2D mapsLW correspond to a detector region (B0, B1, B2, ECA, ECC).
// This books and formats the histograms in the constructor. The fill method will take the identifier 
// as the input and fill the correct histogram and bin. The histograms are also public so that they
// can be formated/accessed like any other histograms in the monitoring.

class PixelMon2DMapsLW
{
   public:
      PixelMon2DMapsLW(std::string name, std::string title, const PixMon::HistConf& config, bool copy2DFEval = false);
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
      void Fill(Identifier &id, const PixelID* pixID);
      void WeightingFill(Identifier &id, const PixelID* pixID, float weight);
      void Fill2DMon(PixelMon2DMapsLW* oldmap);
      StatusCode regHist(ManagedMonitorToolBase::MonGroup &group);
private:
      friend class PixelMon2DProfilesLW;
      void formatHist();
      std::array<TH2F_LW*, 10> m_histograms;
      const PixMon::HistConf m_config;
      static const bool m_doIBL;
      const bool m_copy2DFEval;
};

#endif
