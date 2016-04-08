/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGMUONEFCOMBINERDIMUONMASSPTIMPACTSHYPO_H 
#define TRIG_TRIGMUONEFCOMBINERDIMUONMASSPTIMPACTSHYPO_H

#include <string>
#include <algorithm>
#include "TrigInterfaces/AllTEAlgo.h"

class StoreGateSvc;
class TriggerElement;

class TrigMuonEFCombinerDiMuonMassPtImpactsHypo: public HLT::AllTEAlgo
{
   public:
      TrigMuonEFCombinerDiMuonMassPtImpactsHypo(const std::string & name, ISvcLocator* pSvcLocator);
      ~TrigMuonEFCombinerDiMuonMassPtImpactsHypo();
      
      HLT::ErrorCode hltInitialize();    
      HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& input, unsigned int output);
      HLT::ErrorCode hltFinalize();
      HLT::ErrorCode hltBeginRun();

   private:
      
      static const double MUMASS;
      static const double MASS_ZERO_LIMIT;
  
      // Properties
      BooleanProperty m_acceptAll;
      double          m_massThresLow;
      double          m_massThresHigh;
      double          m_pairptThresLow;
      double          m_pairptThresHigh;
      double          m_deltaZThres;
      double          m_deltaPhiThresLow;
      double          m_deltaPhiThresHigh;
  
      // Monitoring variables
      std::vector<int>   m_mon_nMuons;
      std::vector<float> m_mon_invMass;
      std::vector<float> m_mon_dimuonPt;                                    //pvn
      std::vector<float> m_mon_muonDeltaZ;                                  //pvn
      std::vector<float> m_mon_muonDeltaPhi;                                //pvn

      // Functions
      double invMass(double pt1, double eta1, double phi1, double pt2, double eta2, double phi2);
      double pairPt(double pt1, double phi1, double pt2, double phi2);      //pvn
      double deltaZ(double muonz1, double muonz2);                          //pvn

      // Other members
      StoreGateSvc* m_storeGate;
};

#endif
