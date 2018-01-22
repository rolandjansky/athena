/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELMONMODULES_H_
#define PIXELMONMODULES_H_
#include <string.h>
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

class TH2F;
class TH1F;
class TH1F_LW;
class TProfile;
class TProfile_LW;
class Identifier;
class PixelID;
class StatusCode;

// A helper class to remove a lot of the code duplication.
// This is a collection of 1744 1D histograms which make up one set of module
// histograms. It will book the histograms in the constructor. The fill method
// will take the identifier as the input and fill the correct histogram and bin.

class PixelMonModules {
 public:
  // PixelMonModules();
  virtual void formatHist(std::string) = 0;  // pass the bin labels here if needed
  virtual void reset() = 0;
  virtual StatusCode regHist(ManagedMonitorToolBase* thisptr, std::string path, ManagedMonitorToolBase::Interval_t Run) = 0;

 protected:
  int m_nBins;
  std::string getHistName(int i, bool forPath = false);
  static const bool m_doIBL;
};

class PixelMonModules1D : public PixelMonModules {
 public:
  PixelMonModules1D(std::string name, std::string title, int nbins, double low, double high);
  PixelMonModules1D(std::string name, std::string title, int nbins, double* arr);
  StatusCode regHist(ManagedMonitorToolBase* thisptr, std::string path, ManagedMonitorToolBase::Interval_t Run);
  TH1F* IBL[20][14];
  TH1F* B0[13][22];
  TH1F* B1[13][38];
  TH1F* B2[13][52];
  TH1F* A[3][48];
  TH1F* C[3][48];
  virtual void formatHist(std::string opt = "");  // pass the bin labels here if needed
  void fill(double value, Identifier& id, const PixelID* pixID);
  void setBinLabel(const char* lable, int binN);
  virtual void reset();
  double getBinContent(double value, Identifier& id, const PixelID* pixID);

 private:
  TH1F* m_Dummy;          // shouldn't be used unless messed up;
  TH1F*& getHist(int i);  // make looping over all the histos easier.
};

class PixelMonModulesProf : public PixelMonModules {
 public:
  PixelMonModulesProf(std::string name, std::string title, int nbins, double low, double high);
  PixelMonModulesProf(std::string name, std::string title, int nbins, double* arr);
  StatusCode regHist(ManagedMonitorToolBase* thisptr, std::string path, ManagedMonitorToolBase::Interval_t Run);
  TProfile_LW* IBL[20][14];
  TProfile_LW* B0[13][22];
  TProfile_LW* B1[13][38];
  TProfile_LW* B2[13][52];
  TProfile_LW* A[3][48];
  TProfile_LW* C[3][48];
  virtual void formatHist(std::string opt = "");  // pass the bin labels here if needed
  void fill(double value0, double value1, Identifier& id, const PixelID* pixID);
  void setBinLabel(const char* lable, int binN);
  virtual void reset();

 private:
  TProfile_LW* m_Dummy;          // shouldn't be used unless messed up;
  TProfile_LW*& getHist(int i);  // make looping over all the histos easier.
};

class PixelMonModules2D : public PixelMonModules {
 public:
  PixelMonModules2D(std::string name, std::string title, int nbins0,
                    double low0, double high0, int nbins1, double low1, double high1);
  StatusCode regHist(ManagedMonitorToolBase* thisptr, std::string path, ManagedMonitorToolBase::Interval_t Run);
  TH2F* IBL[20][14];
  TH2F* B0[13][22];
  TH2F* B1[13][38];
  TH2F* B2[13][52];
  TH2F* A[3][48];
  TH2F* C[3][48];
  virtual void formatHist(std::string opt = "");  // pass the bin labels here if needed
  void fill(double value0, double value1, Identifier& id, const PixelID* pixID, double weight = 1.);
  virtual void reset();

 private:
  TH2F* m_Dummy;          // shouldn't be used unless messed up;
  TH2F*& getHist(int i);  // make looping over all the histos easier.
};

#endif
