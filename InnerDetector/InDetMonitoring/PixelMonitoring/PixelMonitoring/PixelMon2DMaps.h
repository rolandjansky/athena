/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELMON2DMAPS_H_
#define PIXELMON2DMAPS_H_
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include <string.h>

class TH2F;
class Identifier;
class PixelID;
class StatusCode;

// A helper class to remove a lot of the code duplication.
// This is a collection of 5 2D histograms which make up the '2D maps' used a lot of in the monitoring.
// Each of the 2D maps correspond to a detector region (B0, B1, B2, ECA, ECC).
// This books and formats the histograms in the constructor. The fill method will take the identifier 
// as the input and fill the correct histogram and bin. The histograms are also public so that they
// can be formated/accessed like any other histograms in the monitoring.

class PixelMon2DMaps
{
   public:
      PixelMon2DMaps(std::string name, std::string title);
      ~PixelMon2DMaps();
      TH2F* IBL2D;
      TH2F* IBL3D;
      TH2F* IBL;
      TH2F* B0;
      TH2F* B1;
      TH2F* B2;
      TH2F* A;
      TH2F* C;
      TH2F* DBMA;
      TH2F* DBMC;
      int m_cnt;
      void Reset();
      void Fill(Identifier &id, const PixelID* pixID, bool doIBL);
      void WeightingFill(Identifier &id, const PixelID* pixID, bool doIBL, float weight);
      //void Scale(double number);
      void Scale(double number, bool doIBL);
      void ScaleByNChannels(int nActive_IBL2D, int nActive_IBL3D, int nActive_B0, int nActive_B1, int nActive_B2, int nActive_ECA, int nActive_ECC, int nActive_DBMA, int nActive_DBMC);
      void ScaleBynPixnEvt(int nevent, bool doIBL);
      //void FillNormalized(PixelMon2DMaps* old, int nevent, int nActive_IBL2D, int nActive_IBL3D, int nActive_B0, int nActive_B1, int nActive_B2, int nActive_ECA, int nActive_ECC, bool doIBL);
      //void FillNormalized(PixelMon2DMaps* old, int nevent, int nActive_IBL2D, int nActive_IBL3D, int nActive_B0, int nActive_B1, int nActive_B2, int nActive_ECA, int nActive_ECC);
      void Fill2DMon(PixelMon2DMaps* oldmap);
      StatusCode regHist(ManagedMonitorToolBase::MonGroup &group);
private:
      void formatHist();
};

#endif
