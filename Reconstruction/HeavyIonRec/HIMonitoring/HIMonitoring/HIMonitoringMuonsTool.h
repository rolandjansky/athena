/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIMONITORINGMUONSTOOL_H
#define HIMONITORINGMUONSTOOL_H

#include <vector>
#include <string>

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "TRandom3.h"

#include <xAODHIEvent/HIEventShape.h>
#include <xAODHIEvent/HIEventShapeContainer.h>

//muons
#include <xAODMuon/MuonContainer.h>

//tracks
#include <xAODTracking/TrackParticleContainer.h>

class TH1;
class TGraph;
class TTree;
class TH1D;
class TH1D_LW;
class TH2D_LW;
class TProfile_LW;

class HIMonitoringMuonsTool : public ManagedMonitorToolBase
{
   public:

      HIMonitoringMuonsTool( const std::string & type, const std::string & name,
                             const IInterface* parent ); 

      virtual ~HIMonitoringMuonsTool();

      virtual StatusCode bookHistogramsRecurrent();
      virtual StatusCode bookHistograms();
      virtual StatusCode fillHistograms();
      virtual StatusCode procHistograms();

      
      void book_hist();

	  
	  
   private:
      
      double m_FCalEt; 
      xAOD::Muon::Author auth;
      float chi2ndof,chi2ndofID,chi2ndofMS,chi2ndofME;
      float type;

      /// histograms 
    
      TH1D* h_FCalEt; 
	
      TH1D* h_Auth_fcal; 
      TH1D* h_Type_fcal; 
      TH1D* h_Chi2ndf_fcal; 
   
      TH1D* h_Chi2ndfID_fcal; 
      TH1D* h_Chi2ndfME_fcal; 
      TH1D* h_Chi2ndfMS_fcal; 
};

#endif
