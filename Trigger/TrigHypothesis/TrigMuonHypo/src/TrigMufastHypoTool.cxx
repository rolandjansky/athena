/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>
#include <algorithm>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "AthenaMonitoring/MonitoredScope.h"

#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "TrigMuonHypo/TrigMufastHypoTool.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

using namespace TrigCompositeUtils;
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigMufastHypoTool::TrigMufastHypoTool(const std::string& type, 
				       const std::string & name,
				       const IInterface* parent )
   : AthAlgTool( type, name, parent ),
     m_decisionId( HLT::Identifier::fromToolName( name ) ) 
{
   std::vector<float> def_bins;
   def_bins.push_back(0);
   def_bins.push_back(2.5); 
   std::vector<float> def_thrs;
   def_thrs.push_back(5.49*CLHEP::GeV);

   declareProperty("PtBins",       m_ptBins=def_bins);
   declareProperty("PtThresholds", m_ptThresholds=def_thrs);
   declareProperty("AcceptAll",    m_acceptAll=false);
   declareProperty("SelectPV",     m_selectPV=false);
   declareProperty("Z_PV_Bins",    m_Z_PV=400.);
   declareProperty("R_PV_Bins",    m_R_PV=200.);

   declareProperty("PtThresholdForECWeakBRegionA", m_ptThresholdForECWeakBRegionA=3.);
   declareProperty("PtThresholdForECWeakBRegionB", m_ptThresholdForECWeakBRegionB=3.);

   m_bins = 0;
}

TrigMufastHypoTool::~TrigMufastHypoTool(){
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMufastHypoTool::initialize()
{
  ATH_MSG_DEBUG("Initializing " << name() << " - package version " << PACKAGE_VERSION);

  if(m_acceptAll) {
     ATH_MSG_DEBUG("Accepting all the events with not cut!");
  }
  else {
     m_bins = m_ptBins.size() - 1;
     if (m_bins != m_ptThresholds.size()) {
	ATH_MSG_DEBUG("bad thresholds setup .... exiting!");
	return StatusCode::SUCCESS;
     }

      for (std::vector<float>::size_type i=0; i<m_bins;++i) {
	 char buf1[256];
	 char buf2[256];
	 sprintf(buf1,"%f5.2",m_ptBins[i]);
	 sprintf(buf2,"%f5.2",m_ptBins[i+1]);
	 ATH_MSG_DEBUG("EtaBin " << buf1 << " - " <<  buf2
	              << ": with Pt Threshold of " << (m_ptThresholds[i])/CLHEP::GeV 
	              << " GeV");
      }
      
      ATH_MSG_DEBUG("Endcap WeakBField A: pT threshold of " << m_ptThresholdForECWeakBRegionA / CLHEP::GeV << " GeV");
      ATH_MSG_DEBUG("Endcap WeakBField B: pT threshold of " << m_ptThresholdForECWeakBRegionB / CLHEP::GeV << " GeV");
  }

  ATH_MSG_DEBUG( "Tool configured for chain/id: " << m_decisionId );

  if ( not m_monTool.name().empty() ) {
     ATH_CHECK( m_monTool.retrieve() );
     ATH_MSG_DEBUG("MonTool name: " << m_monTool);
  }

  ATH_MSG_DEBUG("Initialization completed successfully");

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMufastHypoTool::decide(TrigMufastHypoTool::MuonClusterInfo& input) const
{
   using namespace Monitored;

   auto fex_pt 		= MonitoredScalar::declare("Pt", -9999.);
   auto fex_eta 	= MonitoredScalar::declare("Eta", -9999.);
   auto fex_phi 	= MonitoredScalar::declare("Phi", -9999.);
   auto x_at_station	= MonitoredScalar::declare("XatSt", -9999.);
   auto y_at_station	= MonitoredScalar::declare("YatSt", -9999.);
   auto z_at_station	= MonitoredScalar::declare("ZatSt", -9999.);
   auto x_at_beam	= MonitoredScalar::declare("XatBe", -9999.);
   auto z_at_beam	= MonitoredScalar::declare("ZatBe", -9999.);

   auto monitorIt	= MonitoredScope::declare(m_monTool, fex_pt, fex_eta, fex_phi, 
   					          x_at_station, y_at_station, z_at_station, 
					          x_at_beam, z_at_beam);

   bool result = false;
   // if accept All flag is on, just pass it
   if(m_acceptAll) {
      result = true;
      ATH_MSG_DEBUG("Accept property is set: taking all the events");
      return StatusCode::SUCCESS;
   } else {
      result = false;
      ATH_MSG_DEBUG("Accept property not set: applying selection!");
   }

   // Get xAOD::L2StandAloneMuon:
   auto pMuon = input.cluster;
 
   if(!pMuon){
      ATH_MSG_ERROR("Retrieval of L2StandAloneMuon from vector failed");
      return StatusCode::FAILURE;
   }

   // fill Monitoring histos
   fex_pt  = (pMuon->pt())?  pMuon->pt()  : -9999.;
   fex_eta = (pMuon->etaMS())? pMuon->etaMS() : -9999.;
   fex_phi = (pMuon->etaMS())? pMuon->phiMS() : -9999.;

   if( pMuon->etaMS() ) {
      float localPhi = getLocalPhi(pMuon->etaMS(),pMuon->phiMS(),pMuon->rMS());
      float radius = pMuon->rMS()/cos(fabs(localPhi));
      float DirZ = (pMuon->dirZMS())? pMuon->dirZMS() : .000001;
      float DirF = (pMuon->dirPhiMS())?  pMuon->dirPhiMS()  : .000001;
      x_at_station = radius * cos(pMuon->phiMS());
      y_at_station = radius * sin(pMuon->phiMS());
      z_at_station = pMuon->zMS();
      float xb = x_at_station - y_at_station/DirF;
      float de = x_at_station - xb;
      float ds = sqrt(y_at_station*y_at_station+de*de);
      x_at_beam = xb;
      z_at_beam = z_at_station - ds*DirZ;
   } else {
      x_at_station = -9999.;
      y_at_station = -9999.;
      z_at_station = -9999.;
      x_at_beam = -9999.;
      z_at_beam = -9999.;
   }

   //Get the Pt cut for that eta bin
   float threshold = 0;
   float absEta = fabs(fex_eta);
   for (std::vector<float>::size_type i=0; i<m_bins; ++i)
      if ( absEta > m_ptBins[i] && absEta < m_ptBins[i+1] ) threshold = m_ptThresholds[i]; 

   // if in the weak Bfield regions at endcap, set special threshold
   TrigMufastHypoToolConsts::ECRegions ecRegion = whichECRegion( fex_eta, fex_phi );
   if( ecRegion == TrigMufastHypoToolConsts::WeakBFieldA ) {
      ATH_MSG_DEBUG("threshold is set for EC WeakBField A");
      threshold = m_ptThresholdForECWeakBRegionA;
   }
   if( ecRegion == TrigMufastHypoToolConsts::WeakBFieldB ) {
      ATH_MSG_DEBUG("threshold is set for EC WeakBField B");
      threshold = m_ptThresholdForECWeakBRegionB;
   }

   ATH_MSG_DEBUG("threshold value is set as: " << threshold/CLHEP::GeV << " GeV");

   // Check pt threshold for hypothesis, 
   // convert units since Muonfeature is in GeV
   if ( std::abs(pMuon->pt()) > (threshold/CLHEP::GeV)){
      // selects only tracks coming from a region around PV
      if( m_selectPV ){
	 if((fabs(x_at_beam)<m_R_PV) && (fabs(z_at_beam)<m_Z_PV))
	    result = true;
      } else {
	 result = true;
      }
   }

   ATH_MSG_DEBUG("REGTEST muon pt is " << pMuon->pt() << " GeV" 
                 << " and threshold cut is " << threshold/CLHEP::GeV << " GeV" 
                 << " so hypothesis is " << (result?"true":"false"));
  
   return StatusCode(result);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigMufastHypoToolConsts::ECRegions TrigMufastHypoTool::whichECRegion( const float eta, const float phi ) const
{
   float absEta = fabs(eta);

   if(      ( 1.3 <= absEta && absEta < 1.45) &&
            ( (0                 <= fabs(phi) && fabs(phi) < CLHEP::pi/48. )     ||
	      (CLHEP::pi*11./48. <= fabs(phi) && fabs(phi) < CLHEP::pi*13./48. ) ||
	      (CLHEP::pi*23./48. <= fabs(phi) && fabs(phi) < CLHEP::pi*25./48. ) ||
	      (CLHEP::pi*35./48. <= fabs(phi) && fabs(phi) < CLHEP::pi*37./48. ) ||
	      (CLHEP::pi*47./48. <= fabs(phi) && fabs(phi) < CLHEP::pi )
	  )
      ) return TrigMufastHypoToolConsts::WeakBFieldA;
   
   else if( ( 1.5 <= absEta && absEta < 1.65 ) &&
	    ( (CLHEP::pi*3./32.  <= fabs(phi) && fabs(phi) < CLHEP::pi*5./32. ) ||
	      (CLHEP::pi*11./32. <= fabs(phi) && fabs(phi) < CLHEP::pi*13./32.) ||
	      (CLHEP::pi*19./32. <= fabs(phi) && fabs(phi) < CLHEP::pi*21./32.) ||
	      (CLHEP::pi*27./32. <= fabs(phi) && fabs(phi) < CLHEP::pi*29./32.)
	       )
      ) return TrigMufastHypoToolConsts::WeakBFieldB;
   
   else return TrigMufastHypoToolConsts::Bulk;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

float TrigMufastHypoTool::getLocalPhi(float eta, float phi, float rad) const
{
   if(phi<0.) phi += 2*3.14159265;
   float step = 0.78539816;
   float offs = 0.39269908;
   if(fabs(eta) <= 1.1)
   {
      float Dphi = 999999.;
      float sign = 0.;
      const float ZERO_LIMIT = 1e-6;
      if(rad < 800.)
      {
         for(int i=0;i<8;++i) if(fabs(i*step-phi)<=Dphi)
	 { 
	    Dphi=fabs(i*step-phi);
	    sign = (fabs(Dphi) > ZERO_LIMIT) ? (i*step-phi)/fabs(i*step-phi) : 0;
	 }
	 return sign*Dphi;
      }else
      {
         for(int i=1;i<8;++i) if(fabs(i*step+offs-phi)<=Dphi)
	 { 
	    Dphi=fabs(i*step+offs-phi);
	    sign = (fabs(Dphi) > ZERO_LIMIT) ? (i*step+offs-phi)/fabs(i*step+offs-phi) : 0;
	 }     
	 return sign*Dphi;
      }
   }else
   {
      return 0.;
   }
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMufastHypoTool::decide(std::vector<MuonClusterInfo>& toolInput) const {

   ATH_MSG_DEBUG("Applying selection of single << " << m_decisionId.numeric());

   for ( auto& i: toolInput) {
     if (TrigCompositeUtils::passed(m_decisionId.numeric(), i.previousDecisionIDs)) {
       if ( decide(i) ) {
         ATH_MSG_DEBUG("hypothesis is true, so object passed selection");
         TrigCompositeUtils::addDecisionID(m_decisionId, i.decision);
       } else {
         ATH_MSG_DEBUG("hypothesis is false, so no objest passed selection");
       } // End of TrigMufastHypoTool decide()
     } 
   } // End of MuonClusterInfo loops 
   return StatusCode::SUCCESS;
}
