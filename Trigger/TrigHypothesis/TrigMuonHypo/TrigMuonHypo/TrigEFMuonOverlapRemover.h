/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
// algorithm for deactivating duplicated TEs at EF
// based on extrapolated muon tracks  
//
// author antonio.policicchio@cern.ch
//
////////////////////////////////////////////////////////////

#ifndef TRIG_TRIGEFMUONOVERLAPREMOVER_H 
#define TRIG_TRIGEFMUONOVERLAPREMOVER_H

#include <vector>
#include "TrigInterfaces/AllTEAlgo.h"


// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

namespace TrigEFMuonOverlapRemoverConsts {
const int  monitorChoice_all       = 0;
const int  monitorChoice_mu6_only = 1;

const int  errorCode_cannot_get_EL         = 1;
const int  errorCode_EL_not_valid          = 2;
const int  errorCode_inconsistent_overlap1 = 3;
const int  errorCode_inconsistent_overlap2 = 4;
const int  errorCode_cannot_get_RoI        = 5;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class TriggerElement;

class TrigEFMuonOverlapRemover : public HLT::AllTEAlgo
{
   public:
      
      TrigEFMuonOverlapRemover(const std::string& name, ISvcLocator* pSvcLocator); //!< std Gaudi Algorithm constructor
      
      HLT::ErrorCode hltInitialize() { return HLT::OK; } //!< hlt initialize, doing nothing here
      HLT::ErrorCode hltFinalize()   { return HLT::OK; } //!< hlt finalize, doing nothing here
      virtual HLT::ErrorCode hltBeginRun();
      HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& input,
				unsigned int output);


      bool isOverlap(std::vector<const xAOD::MuonContainer*> extrMfLink1,
		     std::vector<const xAOD::MuonContainer*> extrMfLink2);

      double invMass(double m1, double pt1, double eta1, double phi1,
		     double m2, double pt2, double eta2, double phi2);
      double dR(double eta1, double phi1, double eta2, double phi2);

   private:
      bool m_doMonitor;
      bool m_doDebug;

      // properties
      // muExtr based
      BooleanProperty     m_doMuExtrBasedRemoval;
      BooleanProperty     m_muExtrRequireDR;
      BooleanProperty     m_muExtrRequireMufastDR;
      BooleanProperty     m_muExtrRequireMass;
      BooleanProperty     m_muExtrRequireSameSign;
      std::vector<double> m_muExtrEtaBins;
      std::vector<double> m_muExtrDRThres;
      std::vector<double> m_muExtrMassThres;

      // monitor choice
      int m_monitorChoice;

      // monitoring variables
      int m_mnt_muextrNrAllTEs;
      int m_mnt_muextrNrOverlapped;
      int m_mnt_muextrNrActiveTEs;
      std::vector<float> m_mnt_muextrOverlappedPt;
      std::vector<float> m_mnt_muextrOverlappedEta;
      std::vector<float> m_mnt_muextrOverlappedPhi;
      std::vector<int>   m_mnt_muextrError;
      std::vector<float> m_mnt_muextrDR;
      std::vector<float> m_mnt_muextrMass;
      std::vector<float> m_mnt_muextrDRLog10;
      std::vector<float> m_mnt_muextrMassLog10;
};

#endif

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
