/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DBMMON2DMAPS_H_
#define DBMMON2DMAPS_H_
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

class DBMMon2DMaps
{
   public:
      DBMMon2DMaps(std::string name, std::string title);
      ~DBMMon2DMaps();
      TH2F* DBMA;
      TH2F* DBMC;
      void Fill(Identifier &id, const PixelID* pixID);
      void Scale(double number);
      StatusCode regHist(ManagedMonitorToolBase::MonGroup &group);
private:
      void formatHist();
      
};

#endif
