/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//
//      2014-06-28 Author: Remi Lafaye (Annecy) 
//
/////////////////////////////////////////////////////////////

#ifndef egammaMonToolBase_H
#define egammaMonToolBase_H

#include <vector>
#include "TProfile.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TString.h"

class TH1;
class TH2;
class TString;
class TProfile;
class StoreGateSvc;

class egammaMonToolBase : public ManagedMonitorToolBase
{
 public:
  
  egammaMonToolBase(const std::string& type, const std::string& name, const IInterface* parent); 
  
  virtual ~egammaMonToolBase();
  
  virtual StatusCode initialize() override;
  virtual StatusCode bookHistograms() override;
  virtual StatusCode fillHistograms() override;
  virtual StatusCode procHistograms() override;

 protected:

  enum { BARREL=0, CRACK, ENDCAP, FORWARD, NREGION };

  bool hasBadLar();
  int  GetRegion(float eta);
  int  GetForwardRegion(float eta);
  void bookTH1F(TH1* &h, MonGroup& mygroup, const std::string& hname, const std::string& htitle, int nbins, float low, float high);
  void bookTH2F(TH2* &h, MonGroup& mygroup, const std::string& hname, const std::string& htitle, int nbinsx, float xlow, float xhigh, int nbinsy, float ylow, float yhigh);
  void bookTProfile(TProfile* &h, MonGroup& mygroup, const std::string& hname, const std::string& htitle, int nbins, float xlow, float xhigh, float ylow, float yhigh);
  void bookTH1FperRegion(std::vector<TH1*> &vhist, MonGroup& mygroup, const std::string& hname, const std::string& htitle, int nbins, float low, float high, unsigned int min_region, unsigned int max_region);
  void bookTH2FperRegion(std::vector<TH2*> &vhist, MonGroup& mygroup, const std::string& hname, const std::string& htitle, int nbinsx, float xlow, float xhigh, int nbinsy, float ylow, float yhigh, unsigned int min_region, unsigned int max_region);
  void fillTH1FperRegion(std::vector<TH1*> &vhist, unsigned int ir, float x);
  void fillTH2FperRegion(std::vector<TH2*> &vhist, unsigned int ir, float x, float y);
  void fillEfficiencies(TH1* h, TH1* href);
  bool hasGoodTrigger(const std::string& comment);
  unsigned int getCurrentLB();

  // Data members
  StoreGateSvc * m_storeGate;

  std::vector<std::string> m_Trigger; // generic Trigger Name
  ToolHandle<Trig::TrigDecisionTool> m_trigdec; // Trigger Decision Tool Handle
  bool m_UseTrigger; // Use Trigger ?

  std::string m_GroupExtension;

  unsigned int  m_currentLB;
  SG::ReadHandleKey<xAOD::EventInfo> m_EventInfoKey{this, "EventInfoKey", "EventInfo"};

 private:
  std::vector<std::string> m_region;
};

#endif
