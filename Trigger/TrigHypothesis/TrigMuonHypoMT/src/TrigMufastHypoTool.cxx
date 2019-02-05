/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>
#include <algorithm>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "AthenaMonitoring/Monitored.h"

#include "DecisionHandling/Combinators.h"

#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "TrigMufastHypoTool.h"

#include "xAODTrigMuon/TrigMuonDefs.h"

using namespace TrigCompositeUtils;
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigMufastHypoTool::TrigMufastHypoTool(const std::string& type, 
				       const std::string & name,
				       const IInterface* parent )
   : AthAlgTool( type, name, parent ),
     m_decisionId( HLT::Identifier::fromToolName( name ) ) 
{
}

TrigMufastHypoTool::~TrigMufastHypoTool(){
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMufastHypoTool::initialize()
{
  ATH_MSG_DEBUG("Initializing " << name() << " - package version " << PACKAGE_VERSION);

  if(m_acceptAll) {
     ATH_MSG_DEBUG("AcceptAll = True");
     ATH_MSG_DEBUG("Accepting all the events with not cut!");
  }
  else {
     ATH_MSG_DEBUG("AcceptAll = False");
     m_bins.resize (m_ptBins.size());
     for ( size_t j=0; j<m_ptBins.size(); ++j) {
        m_bins[j] = m_ptBins[j].size() - 1;
        if (m_bins[j] != m_ptThresholds[j].size()) {
   	   ATH_MSG_DEBUG("bad thresholds setup .... exiting!");
      	   return StatusCode::SUCCESS;
        }

        for (std::vector<float>::size_type i=0; i<m_bins[j];++i) {
   	char buf1[256];
   	char buf2[256];
   	sprintf(buf1,"%f5.2",m_ptBins[j][i]);
   	sprintf(buf2,"%f5.2",m_ptBins[j][i+1]);
   	ATH_MSG_DEBUG("EtaBin[" << j << "] " << buf1 << " - " <<  buf2
   	             << ": with Pt Threshold of " << (m_ptThresholds[j][i])/CLHEP::GeV 
   	             << " GeV");
        }
        
        ATH_MSG_DEBUG("Endcap WeakBField A[" << j << "]: pT threshold of " << m_ptThresholdForECWeakBRegionA[j] / CLHEP::GeV << " GeV");
        ATH_MSG_DEBUG("Endcap WeakBField B[" << j << "]: pT threshold of " << m_ptThresholdForECWeakBRegionB[j] / CLHEP::GeV << " GeV");
     }
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

bool TrigMufastHypoTool::decideOnSingleObject(TrigMufastHypoTool::MuonClusterInfo& input, size_t cutIndex) const
{

   auto fexPt 		= Monitored::Scalar("Pt", -9999.);
   auto fexEta 		= Monitored::Scalar("Eta", -9999.);
   auto fexPhi 		= Monitored::Scalar("Phi", -9999.);
   auto fexPtFL		= Monitored::Scalar("PtFL", -9999.);
   auto xatStation	= Monitored::Scalar("XatSt", -9999.);
   auto yatStation	= Monitored::Scalar("YatSt", -9999.);
   auto zatStation	= Monitored::Scalar("ZatSt", -9999.);
   auto xatBeam		= Monitored::Scalar("XatBe", -9999.);
   auto zatBeam		= Monitored::Scalar("ZatBe", -9999.);

   auto monitorIt	= Monitored::Group(m_monTool, fexPt, fexEta, fexPhi, fexPtFL,
   					          xatStation, yatStation, zatStation, 
					          xatBeam, zatBeam);

   ATH_MSG_VERBOSE( "Cut index " << cutIndex );

   auto roiDescriptor = input.roi;
   ATH_MSG_DEBUG("RoI ID = " << roiDescriptor->roiId() 
                 << ", Eta = " << roiDescriptor->eta()
                 << ", Phi = " << roiDescriptor->phi());

   bool result = false;
   // if accept All flag is on, just pass it
   if(m_acceptAll) {
      result = true;
      ATH_MSG_DEBUG("Accept property is set: taking all the events");
      return result;
   } else {
      result = false;
      ATH_MSG_DEBUG("Accept property not set: applying selection!");
   }

   // Get xAOD::L2StandAloneMuon:
   auto pMuon = input.muFast;
 
   if(!pMuon){
      result = false;
      ATH_MSG_ERROR("Retrieval of L2StandAloneMuon from vector failed");
      return result;
   }

   // fill Monitoring histos
   fexPt    = (pMuon->pt())?  pMuon->pt()  : -9999.;
   fexEta   = (pMuon->etaMS())? pMuon->etaMS() : -9999.;
   fexPhi   = (pMuon->etaMS())? pMuon->phiMS() : -9999.;
   fexPtFL  = (pMuon->pt())?  pMuon->pt()  : -9999.;

   if( pMuon->etaMS() ) {
      float localPhi = getLocalPhi(pMuon->etaMS(),pMuon->phiMS(),pMuon->rMS());
      float radius = pMuon->rMS()/cos(fabs(localPhi));
      float DirZ = (pMuon->dirZMS())? pMuon->dirZMS() : .000001;
      float DirF = (pMuon->dirPhiMS())?  pMuon->dirPhiMS()  : .000001;
      xatStation = radius * cos(pMuon->phiMS());
      yatStation = radius * sin(pMuon->phiMS());
      zatStation = pMuon->zMS();
      float xb = xatStation - yatStation/DirF;
      float de = xatStation - xb;
      float ds = sqrt(yatStation*yatStation+de*de);
      xatBeam = xb;
      zatBeam = zatStation - ds*DirZ;
   } else {
      xatStation = -9999.;
      yatStation = -9999.;
      zatStation = -9999.;
      xatBeam = -9999.;
      zatBeam = -9999.;
   }

   //Get the Pt cut for that eta bin
   double threshold = 0;
   float absEta = fabs(fexEta);
   for (std::vector<float>::size_type i=0; i<m_bins[0]; ++i)
      if ( absEta > m_ptBins[cutIndex][i] && absEta < m_ptBins[cutIndex][i+1] ) threshold = m_ptThresholds[cutIndex][i]; 

   // if in the weak Bfield regions at endcap, set special threshold

   xAOD::L2MuonParameters::ECRegions ecRegion = xAOD::L2MuonParameters::whichECRegion( fexEta , fexPhi );
   if( ecRegion == xAOD::L2MuonParameters::ECRegions::WeakBFieldA ) { 
      ATH_MSG_DEBUG("threshold is set for EC WeakBField A");
      threshold = m_ptThresholdForECWeakBRegionA[cutIndex];
   }

   if( ecRegion == xAOD::L2MuonParameters::ECRegions::WeakBFieldA ) {
      ATH_MSG_DEBUG("threshold is set for EC WeakBField B");
      threshold = m_ptThresholdForECWeakBRegionB[cutIndex];
   }

   ATH_MSG_DEBUG("threshold value is set as: " << threshold/CLHEP::GeV << " GeV");

   // Check pt threshold for hypothesis, 
   // convert units since Muonfeature is in GeV
   if ( std::abs(pMuon->pt()) > (threshold/CLHEP::GeV)){
      // selects only tracks coming from a region around PV
      if( m_selectPV ){
	 if((fabs(xatBeam)<m_RPV) && (fabs(zatBeam)<m_ZPV))
	    result = true;
      } else {
	 result = true;
      }
   }

   if ( result ) fexPtFL = -9999.;

   ATH_MSG_DEBUG("REGTEST: Muon pt is " << pMuon->pt() << " GeV" 
                 << " and threshold cut is " << threshold/CLHEP::GeV << " GeV" 
                 << " so hypothesis is " << (result?"true":"false"));
  
   return result;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
/*
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
*/
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
      const float ZEROLIMIT = 1e-6;
      if(rad < 800.)
      {
         for(int i=0;i<8;++i) if(fabs(i*step-phi)<=Dphi)
	 { 
	    Dphi=fabs(i*step-phi);
	    sign = (fabs(Dphi) > ZEROLIMIT) ? (i*step-phi)/fabs(i*step-phi) : 0;
	 }
	 return sign*Dphi;
      }else
      {
         for(int i=1;i<8;++i) if(fabs(i*step+offs-phi)<=Dphi)
	 { 
	    Dphi=fabs(i*step+offs-phi);
	    sign = (fabs(Dphi) > ZEROLIMIT) ? (i*step+offs-phi)/fabs(i*step+offs-phi) : 0;
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

   size_t numTrigger = m_ptBins.size(); 
   size_t numMuon = toolInput.size();
   ATH_MSG_DEBUG("Retrieved from TrigMufastHypoAlg and Running TrigMufastHypoTool for selections.");

   if ( numTrigger == 1 ) {	// in case of HLT_mu4, HLT_mu6 and so on.
      ATH_MSG_DEBUG("Number of muon event = " << numMuon );
      ATH_MSG_DEBUG("Applying selection of single << " << m_decisionId );
      return inclusiveSelection(toolInput);
   } else {			// in case of HLT_2mu6 and so on.
      if ( numMuon == 1 ) {	// If RoIs have only 1 muon, multipul selection wouldn't run.
         ATH_MSG_DEBUG("Number of muon event = " << numMuon );
         ATH_MSG_DEBUG("Not applying selection " << m_decisionId << " because of " << numMuon << " muon" );
      } else {			// IF RoIs have some muon, multipul selection would run.
         ATH_MSG_DEBUG("Number of muon event = " << numMuon );
         ATH_MSG_DEBUG("Applying selection of multiplicity << " << m_decisionId );
         return multiplicitySelection(toolInput); 
      }
   }

   return StatusCode::SUCCESS;
}


StatusCode TrigMufastHypoTool::inclusiveSelection(std::vector<TrigMufastHypoTool::MuonClusterInfo>& toolInput) const{

   for ( auto& i: toolInput ) {
     // If muon event has defference DecisionID, it shouldn't apply.   
     if ( TrigCompositeUtils::passed( m_decisionId.numeric(), i.previousDecisionIDs ) ) {
         if ( decideOnSingleObject( i, 0 )==true ) {
            ATH_MSG_DEBUG("Pass through selection " << m_decisionId );
            TrigCompositeUtils::addDecisionID(m_decisionId, i.decision);
         } else {
            ATH_MSG_DEBUG("Not pass through selection " << m_decisionId );
         } 
      } else {
         ATH_MSG_DEBUG("Not match DecisionID:" << m_decisionId );
      } 
   }  

   return StatusCode::SUCCESS; 
}


StatusCode TrigMufastHypoTool::multiplicitySelection(std::vector<TrigMufastHypoTool::MuonClusterInfo>& toolInput) const{

   HLT::Index2DVec passingSelection( toolInput.size() );

   for ( size_t cutIndex=0; cutIndex < m_ptBins.size(); ++cutIndex ) {
      size_t elementIndex{ 0 };      
      for ( auto& i: toolInput ) {
         // If muon event has defference DecisionID, it shouldn't apply.   
         if ( TrigCompositeUtils::passed( m_decisionId.numeric(), i.previousDecisionIDs ) ) {
            if ( decideOnSingleObject( i, cutIndex ) == true ) {
               ATH_MSG_DEBUG("Pass through selection " << m_decisionId << " : Event[" << elementIndex << "]" );
               passingSelection[cutIndex].push_back( elementIndex );
            } else {
               ATH_MSG_DEBUG("Not pass through selection " << m_decisionId << " : Event[" << elementIndex << "]" );
            } 
         } else {
            ATH_MSG_DEBUG("Not match DecisionID " << m_decisionId );
         }
         elementIndex++;
      }

      // If no object passes the selection, multipul selection should stop.
      if ( passingSelection[cutIndex].empty() ) {
         ATH_MSG_DEBUG( "No object passed selection " << cutIndex << " rejecting" );
         return StatusCode::SUCCESS;
      }
   }

   std::set<size_t> passingIndices;
   if ( m_decisionPerCluster==true ) {
      auto notFromSameRoI = [&]( const HLT::Index1DVec& comb ) {
         std::set<const xAOD::L2StandAloneMuon*> setOfClusters;
         for ( auto index: comb ) {
            setOfClusters.insert( toolInput[index].muFast );
         }
         return setOfClusters.size() == comb.size();
      };

      HLT::elementsInUniqueCombinations( passingSelection, passingIndices, notFromSameRoI );    

   } else {
      HLT::elementsInUniqueCombinations( passingSelection, passingIndices );
   }

   if ( passingIndices.empty() ) {
      ATH_MSG_DEBUG("No muon event passed through selection " << m_decisionId );
      return StatusCode::SUCCESS;
   }

   for ( auto idx: passingIndices ) {
      ATH_MSG_DEBUG("Muon event[" << idx << "] passes through Chain/ID " << m_decisionId 
                    << " with pT = " << toolInput[idx].muFast->pt() << "GeV" );
      TrigCompositeUtils::addDecisionID( m_decisionId.numeric(), toolInput[idx].decision );
   }

   return StatusCode::SUCCESS;
}
