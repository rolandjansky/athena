/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGL2MUONOVERLAPREMOVER_H 
#define TRIG_TRIGL2MUONOVERLAPREMOVER_H

#include <vector>
#include "TrigInterfaces/AllTEAlgo.h"

#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "xAODTrigMuon/L2CombinedMuonContainer.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

namespace TrigL2MuonOverlapRemoverConsts {
const int  monitorChoice_all       = 0;
const int  monitorChoice_mu10_only = 1;

const int  errorCode_cannot_get_EL         = 1;
const int  errorCode_EL_not_valid          = 2;
const int  errorCode_inconsistent_overlap1 = 3;
const int  errorCode_inconsistent_overlap2 = 4;
const int  errorCode_cannot_get_RoI        = 5;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class TriggerElement;

class TrigL2MuonOverlapRemover : public HLT::AllTEAlgo
{
   public:
      
      TrigL2MuonOverlapRemover(const std::string& name, ISvcLocator* pSvcLocator); //!< std Gaudi Algorithm constructor
      
      HLT::ErrorCode hltInitialize() { return HLT::OK; } //!< hlt initialize, doing nothing here
      HLT::ErrorCode hltFinalize()   { return HLT::OK; } //!< hlt finalize, doing nothing here
      virtual HLT::ErrorCode hltBeginRun();
      HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& input,
				unsigned int output);

      bool isOverlap(const ElementLink<xAOD::L2StandAloneMuonContainer>& mfLink1,
		     const ElementLink<xAOD::L2StandAloneMuonContainer>& mfLink2);

      bool isOverlap(const ElementLink<xAOD::L2CombinedMuonContainer>& combMfLink1,
		     const ElementLink<xAOD::L2CombinedMuonContainer>& combMfLink2);

      double invMass(double m1, double pt1, double eta1, double phi1,
		     double m2, double pt2, double eta2, double phi2);
      double dR(double eta1, double phi1, double eta2, double phi2);

   private:
      bool m_doMonitor;
      bool m_doDebug;

      // properties
      // muFast based
      BooleanProperty     m_doMufastBasedRemoval;
      BooleanProperty     m_muFastRequireDR;
      BooleanProperty     m_muFastRequireMass;
      BooleanProperty     m_muFastRequireSameSign;
      // BB
      double              m_muFastDRThresBB;
      double              m_muFastMassThresBB;
      // BE
      double              m_muFastDRThresBE;
      double              m_muFastMassThresBE;
      // EC
      std::vector<double> m_muFastEtaBinsEC;
      std::vector<double> m_muFastDRThresEC;
      std::vector<double> m_muFastMassThresEC;

      // muComb based
      BooleanProperty     m_doMucombBasedRemoval;
      BooleanProperty     m_muCombRequireDR;
      BooleanProperty     m_muCombRequireMufastDR;
      BooleanProperty     m_muCombRequireMass;
      BooleanProperty     m_muCombRequireSameSign;
      std::vector<double> m_muCombEtaBins;
      std::vector<double> m_muCombDRThres;
      std::vector<double> m_muCombMufastDRThres;
      std::vector<double> m_muCombMassThres;

      // monitor choice
      int m_monitorChoice;

      // monitoring variables
      int m_mnt_mufastNrAllTEs;
      int m_mnt_mufastNrOverlapped;
      int m_mnt_mufastNrActiveTEs;
      std::vector<float> m_mnt_mufastOverlappedPt;
      std::vector<float> m_mnt_mufastOverlappedEta;
      std::vector<float> m_mnt_mufastOverlappedPhi;
      std::vector<int>   m_mnt_mufastError;
      std::vector<float> m_mnt_mufastDR;
      std::vector<float> m_mnt_mufastMass;
      std::vector<float> m_mnt_mufastDRLog10;
      std::vector<float> m_mnt_mufastMassLog10;

      int m_mnt_mucombNrAllTEs;
      int m_mnt_mucombNrOverlapped;
      int m_mnt_mucombNrActiveTEs;
      std::vector<float> m_mnt_mucombOverlappedPt;
      std::vector<float> m_mnt_mucombOverlappedEta;
      std::vector<float> m_mnt_mucombOverlappedPhi;
      std::vector<int>   m_mnt_mucombError;
      std::vector<float> m_mnt_mucombDR;
      std::vector<float> m_mnt_mucombMass;
      std::vector<float> m_mnt_mucombDRLog10;
      std::vector<float> m_mnt_mucombMassLog10;
};

#endif

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
