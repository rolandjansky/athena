/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//
//      2014-06-28 Author: Remi Lafaye (Annecy) 
//
/////////////////////////////////////////////////////////////

#ifndef egammaMonToolBase_H
#define egammaMonToolBase_H

#include <vector>
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

class TH1;
class TH2;
class StoreGateSvc;

class egammaMonToolBase : public ManagedMonitorToolBase
{
 public:
  
  egammaMonToolBase(const std::string& type, const std::string& name, const IInterface* parent); 
  
  virtual ~egammaMonToolBase();
  
  virtual StatusCode initialize();
  virtual StatusCode bookHistograms();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();
     

 protected:

  enum { BARREL=0, CRACK, ENDCAP, FORWARD, NREGION };

  bool hasBadLar();
  int  GetRegion(float eta);
  void bookTH1F(TH1* &h, MonGroup& mygroup, const std::string hname, const std::string htitle, int nbins, float low, float high);
  void bookTH2F(TH2* &h, MonGroup& mygroup, const std::string hname, const std::string htitle, int nbinsx, float xlow, float xhigh, int nbinsy, float ylow, float yhigh);
  void bookTH1FperRegion(std::vector<TH1*> &vhist, MonGroup& mygroup, const std::string hname, const std::string htitle, int nbins, float low, float high, unsigned int max_region=FORWARD);
  void bookTH2FperRegion(std::vector<TH2*> &vhist, MonGroup& mygroup, const std::string hname, const std::string htitle, int nbinsx, float xlow, float xhigh, int nbinsy, float ylow, float yhigh, unsigned int max_region=FORWARD);
  void fillTH1FperRegion(std::vector<TH1*> &vhist, unsigned int ir, float x);
  void fillTH2FperRegion(std::vector<TH2*> &vhist, unsigned int ir, float x, float y);

  // Data members
  StoreGateSvc * m_storeGate;

 private:

  std::vector<std::string> m_region;
};

#endif
