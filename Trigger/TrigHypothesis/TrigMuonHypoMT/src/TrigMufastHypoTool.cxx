/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/SystemOfUnits.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "TrigCompositeUtils/Combinators.h"
#include "TrigMufastHypoTool.h"
#include "CxxUtils/phihelper.h"

#include "xAODTrigMuon/TrigMuonDefs.h"

#include <cmath>

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
  ATH_MSG_DEBUG("Initializing " << name());

  if(m_acceptAll) {
     ATH_MSG_DEBUG("AcceptAll = True");
     ATH_MSG_DEBUG("Accepting all the events!");
  }
  else if(m_doCalib){
     ATH_MSG_DEBUG("This is muon calibration chain.");
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
   	             << ": with Pt Threshold of " << (m_ptThresholds[j][i])/Gaudi::Units::GeV
   	             << " GeV");
        }
        
        ATH_MSG_DEBUG("Endcap WeakBField A[" << j << "]: pT threshold of " << m_ptThresholdForECWeakBRegionA[j] / Gaudi::Units::GeV << " GeV");
        ATH_MSG_DEBUG("Endcap WeakBField B[" << j << "]: pT threshold of " << m_ptThresholdForECWeakBRegionB[j] / Gaudi::Units::GeV << " GeV");
     }
  }

  ATH_MSG_DEBUG( "Tool configured for chain/id: " << m_decisionId );

  if ( not m_monTool.name().empty() ) {
     ATH_CHECK( m_monTool.retrieve() );
     ATH_MSG_DEBUG("MonTool name: " << m_monTool);
  }


  // Overlap Removal
  if( m_applyOR ) {
    ATH_MSG_DEBUG( "--- overlap removal as: ---"     );
    if( m_requireDR ) {
      ATH_MSG_DEBUG( "+ dR cut:" );
      if( (m_etaBinsEC.size()-1) != m_dRThresEC.size() ) {
	ATH_MSG_ERROR( "bad thresholds setup .... exiting!" );
	return StatusCode::FAILURE;
      }
      ATH_MSG_DEBUG( "     B-B : dR < " << m_dRThresBB );
      ATH_MSG_DEBUG( "     B-E : dR < " << m_dRThresBE );
      ATH_MSG_DEBUG( "     E-E : " );
      for(unsigned int i=0; i<m_dRThresEC.size(); i++) {
	ATH_MSG_DEBUG( "        EtaBin " << m_etaBinsEC[i] << " - " << m_etaBinsEC[i+1]
		       << " : dR < " << m_dRThresEC[i] );
      }
    }
    if( m_requireMass ) {
      ATH_MSG_DEBUG( "+ Mass cut:" );
      if( (m_etaBinsEC.size()-1) != m_massThresEC.size() ) {
	ATH_MSG_ERROR( "bad thresholds setup .... exiting!" );
	return StatusCode::FAILURE;
      }
      ATH_MSG_DEBUG( "     B-B : Mass < " << m_massThresBB );
      ATH_MSG_DEBUG( "     B-E : Mass < " << m_massThresBE );
      ATH_MSG_DEBUG( "     E-E : " );
      for(unsigned int i=0; i<m_massThresEC.size(); i++) {
	ATH_MSG_DEBUG( "        EtaBin " << m_etaBinsEC[i] << " - " << m_etaBinsEC[i+1]
		       << " : Mass < " << m_massThresEC[i] );
      }
    }
    if( m_requireSameSign ) ATH_MSG_DEBUG( "+ Same charge sign" );
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

   if(m_doCalib){
      result = false;
      ATH_MSG_DEBUG("This muoncalib chain is only monitored.");
      return result;
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

   ATH_MSG_DEBUG("threshold value is set as: " << threshold/Gaudi::Units::GeV << " GeV");

   // Check pt threshold for hypothesis, 
   // convert units since Muonfeature is in GeV
   if ( std::abs(pMuon->pt()) > (threshold/Gaudi::Units::GeV)){
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
                 << " and threshold cut is " << threshold/Gaudi::Units::GeV << " GeV"
                 << " so hypothesis is " << (result?"true":"false"));
  
   return result;
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
         ATH_MSG_DEBUG("Number of muon event = " << numMuon );
         ATH_MSG_DEBUG("Applying selection of multiplicity << " << m_decisionId );

	 if(m_applyOR)
	   ATH_CHECK(applyOverlapRemoval(toolInput));

         return multiplicitySelection(toolInput);
   }

   return StatusCode::SUCCESS;
}


StatusCode TrigMufastHypoTool::inclusiveSelection(std::vector<TrigMufastHypoTool::MuonClusterInfo>& toolInput) const{

   for ( auto& i: toolInput ) {
     // If muon event has difference DecisionID, it shouldn't apply.
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

   HLT::Index2DVec passingSelection( m_ptBins.size() );

   for ( size_t cutIndex=0; cutIndex < m_ptBins.size(); ++cutIndex ) {
      size_t elementIndex{ 0 };      
      for ( auto& i: toolInput ) {

	if(!m_acceptAll && m_applyOR && !i.passOR) {
	  ATH_MSG_DEBUG("skip due to overap, DecisionID " << m_decisionId );
	  continue;
	}

         // If muon event has difference DecisionID, it shouldn't apply.
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

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMufastHypoTool::applyOverlapRemoval(std::vector<TrigMufastHypoTool::MuonClusterInfo>& toolInput) const{

   ATH_MSG_DEBUG("Running Overlap Removal for muFast");

  std::vector<TrigMufastHypoTool::MuonClusterInfo*> input;

  for ( auto& i: toolInput ) {
    // If muon event has difference DecisionID, it shouldn't apply.
    if ( TrigCompositeUtils::passed( m_decisionId.numeric(), i.previousDecisionIDs ) ) {
      input.emplace_back(&i);
    }
  }

  size_t numMuon = input.size();

  auto mufastNrAllEVs     = Monitored::Scalar("NrAllEVs", -9999.);
  auto mufastNrActiveEVs  = Monitored::Scalar("NrActiveEVs", -9999.);
  auto monitorIt          = Monitored::Group(m_monTool, mufastNrAllEVs, mufastNrActiveEVs);
  if ( numMuon == 0) {
    ATH_MSG_DEBUG( "No positive previous hypo decision. Not need overlap removal." );
    mufastNrActiveEVs = numMuon;
    mufastNrAllEVs = numMuon;
    return StatusCode::SUCCESS;
  }
  else if ( numMuon == 1 ) {
    ATH_MSG_DEBUG("Number of muon event = " << numMuon );
    ATH_MSG_DEBUG("no overlap Removal necessary. exitting with all EventViews active." );
    mufastNrActiveEVs = numMuon;
    mufastNrAllEVs = numMuon;
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG("Number of muon event = " << numMuon );
    mufastNrAllEVs = numMuon;
    ATH_CHECK(checkOverlap(input));
    return StatusCode::SUCCESS;
  }


  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMufastHypoTool::checkOverlap(std::vector<TrigMufastHypoTool::MuonClusterInfo*>& input) const {

  size_t numMuon = input.size();
  unsigned int i,j;
  std::vector<unsigned int> mufastResult;

  bool errorWhenIdentifyingOverlap = false;

  for(i=0; i<numMuon; i++) {mufastResult.emplace_back(i); }
  for(i=0; i<numMuon-1; i++){
    for(j=i+1; j<numMuon; j++){
      ATH_MSG_DEBUG("++ i=" << i << " vs j=" << j);
      bool overlapped = isOverlap((*input[i]).muFast, (*input[j]).muFast);
      if( ! overlapped ){ // judged as different
	ATH_MSG_DEBUG("   judged as: different objects");
	if( mufastResult[i] == mufastResult[j] ) { // but marked as same by someone
	  ATH_MSG_DEBUG( "inconsistentency in muFast overlap removal for more than two objects" );
	  ATH_MSG_DEBUG( "two objects are judged as different but both were already marked as identical by someone else as: " );
	  ATH_MSG_DEBUG( "i/j/result[i]/result[j]=" << i << " / " << j << " / " << mufastResult[i] << " / "  << mufastResult[j] );
	  auto mufastError  = Monitored::Scalar("MufastError", -9999.);
	  auto monitorIt    = Monitored::Group(m_monTool, mufastError);
	  mufastError = TrigMufastHypoToolConsts::errorCode_inconsistent_overlap1;
	  errorWhenIdentifyingOverlap = true;
	}
      }
      else{ // judged as overlap
	if( (mufastResult[j] != j && mufastResult[i] != mufastResult[j]) || (mufastResult[j] == j && mufastResult[i] != i) ){
	  ATH_MSG_DEBUG( "inconsistentency in muFast overlap removal for more than two objects" );
	  ATH_MSG_DEBUG( "two objects are judged as overlap but only either was already marked as overlap to someone else: " );
	  ATH_MSG_DEBUG( "i/j/result[i]/result[j]=" << i << " / " << j << " / " << mufastResult[i] << " / "  << mufastResult[j] );
	  auto mufastError  = Monitored::Scalar("MufastError", -9999.);
	  auto monitorIt    = Monitored::Group(m_monTool, mufastError);
	  mufastError = TrigMufastHypoToolConsts::errorCode_inconsistent_overlap2;
	  errorWhenIdentifyingOverlap = true;
	}
	ATH_MSG_DEBUG("   judged as: overlapped objects");
	if( mufastResult[i] == i ) {
	  ATH_MSG_DEBUG( "   i is not yet marked as overlap. so, it is a newly found overlap" );
	  ATH_MSG_DEBUG( "   -> marking mufastResult[j] as i..." );
	  mufastResult[j] = i;
	} else {
	  ATH_MSG_DEBUG( "   both i/j already marked as overlap by: mufastResult[i]=" << mufastResult[i] );
	  ATH_MSG_DEBUG( "   -> do nothing..." );
	}
      }
    }
  }

  if( errorWhenIdentifyingOverlap ) {
      ATH_MSG_WARNING( "error when resolving overlap. exitting with all EVs active..." );
      auto mufastNrActiveEVs  = Monitored::Scalar("NrActiveEVs", -9999.);
      auto monitorIt          = Monitored::Group(m_monTool, mufastNrActiveEVs);
      mufastNrActiveEVs = numMuon;
      // for(i=0; i<numMuon; i++) TrigCompositeUtils::addDecisionID( m_decisionId, toolInput[i].decision );
      return StatusCode::SUCCESS;
   }

  unsigned int n_uniqueMuon = 0;
  for(i=0; i<numMuon; i++) {
    ATH_MSG_DEBUG( "muFast results: i=" << i << ": ");
    if( mufastResult[i] != i ) { ATH_MSG_DEBUG( "      overlap to j=" << mufastResult[i] ); }
    else {
      n_uniqueMuon++;
      ATH_MSG_DEBUG( "      unique" );
    }
  }

  ATH_MSG_DEBUG( "nr of unique Muons after muFast overlap removal=" << n_uniqueMuon );

  if( numMuon != n_uniqueMuon ){
    ATH_CHECK(chooseBestMuon(input, mufastResult));
  } else {
    ATH_MSG_DEBUG( "no overlap identified. exitting with all EventViews active" );
    auto mufastNrActiveEVs  = Monitored::Scalar("NrActiveEVs", -9999.);
    auto monitorIt          = Monitored::Group(m_monTool, mufastNrActiveEVs);
    mufastNrActiveEVs = n_uniqueMuon;
  }

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

bool TrigMufastHypoTool::isOverlap(const xAOD::L2StandAloneMuon *mf1,
				   const xAOD::L2StandAloneMuon *mf2) const
{

  auto mufastDR             = Monitored::Scalar("DR", -9999.);
  auto mufastMass           = Monitored::Scalar("Mass", -9999.);
  auto mufastDRLog10        = Monitored::Scalar("DRLog10", -9999.);
  auto mufastMassLog10      = Monitored::Scalar("MassLog10", -9999.);

  auto monitorIt       = Monitored::Group(m_monTool, mufastDR, mufastMass, mufastDRLog10, mufastMassLog10);

  ATH_MSG_DEBUG( "   ...mF1: pt/eta/phi=" << mf1->pt() << " / " << mf1->etaMS() << " / " << mf1->phiMS() );
  ATH_MSG_DEBUG( "   ...mF2: pt/eta/phi=" << mf2->pt() << " / " << mf2->etaMS() << " / " << mf2->phiMS() );

  // if dR or invMass is necessary but (eta,phi) info is not avaiable
  // (i.e. eta,phi=0,0; rec failed)
  const double ZERO_LIMIT_FOR_ETAPHI = 1e-4;
  if( (fabs(mf1->etaMS()) <ZERO_LIMIT_FOR_ETAPHI && fabs(mf1->phiMS()) < ZERO_LIMIT_FOR_ETAPHI) ||
      (fabs(mf2->etaMS()) <ZERO_LIMIT_FOR_ETAPHI && fabs(mf2->phiMS()) < ZERO_LIMIT_FOR_ETAPHI) ) {
    ATH_MSG_DEBUG( "   ...-> (eta,phi) info not available (rec at (eta,phi)=(0,0))" );
    ATH_MSG_DEBUG( "   ...-> but dR of invMass check is required. cannot judge overlap -> return with false" );
    return false;
  }

  // if charge or invMass is necessary but charge(=pT) info is not avaiable
  const double ZERO_LIMIT_FOR_PT = 1e-4;
  if( (fabs(mf1->pt()) <ZERO_LIMIT_FOR_PT) || (fabs(mf2->pt()) < ZERO_LIMIT_FOR_PT) ) {
    ATH_MSG_DEBUG( "   ...-> pT info not available (rec at pT=0)" );
    ATH_MSG_DEBUG( "   ...-> but same sign or invMass check is required. cannot judge overlap -> return with false" );
    return false;
  }


  // determine dR, mass threshold separately for: BB, BE, EE
  double dRThres   = 9999;
  double massThres = 9999;

  const int SADDRESS_EC = -1;
  bool isBarrel1 = (mf1->sAddress() != SADDRESS_EC ) ? true : false;
  bool isBarrel2 = (mf2->sAddress() != SADDRESS_EC ) ? true : false;

  if(  isBarrel1 && isBarrel2 ) { // BB
    ATH_MSG_DEBUG( "   ...B-B" );
    dRThres  =m_dRThresBB;
    massThres=m_massThresBB;
  }
  else if( (isBarrel1 && ! isBarrel2) || (!isBarrel1 && isBarrel2) ) { // BE
    ATH_MSG_DEBUG( "   ...B-E" );
    dRThres  =m_dRThresBE;
    massThres=m_massThresBE;
  }
  else { // EE
    ATH_MSG_DEBUG( "   ...E-E" );
    double absEta = (fabs(mf1->pt()) > fabs(mf2->pt())) ? fabs(mf1->etaMS()) : fabs(mf2->etaMS());
    unsigned int iThres=0;
    for(unsigned int i=0; i<(m_etaBinsEC.size()-1); i++) {
      if ( m_etaBinsEC[i] <= absEta && absEta < m_etaBinsEC[i+1] ) iThres = i;
    }
    ATH_MSG_DEBUG( "   ...iThres=" << iThres );
    dRThres   = m_dRThresEC[iThres];
    massThres = m_massThresEC[iThres];
  }
  ATH_MSG_DEBUG( "   ...dR   threshold=" << dRThres );
  ATH_MSG_DEBUG( "   ...mass threshold=" << massThres );


  // same sign cut
  bool sameSign = false;
  if( m_requireSameSign ) {
    sameSign = ((mf1->pt()*mf2->pt()) > 0) ? true : false;
    ATH_MSG_DEBUG( "   ...-> sameSign=" << sameSign );
  }

  // dR cut
  bool dRisClose = false;
  double dr = dR(mf1->etaMS(),mf1->phiMS(),mf2->etaMS(),mf2->phiMS());

  // for monitoring
  mufastDR = dr;
  const double monitor_limit = 1e-4;
  double dr_mon = (dr>=monitor_limit) ? dr : monitor_limit;
  mufastDRLog10 = log10(dr_mon);

  if( m_requireDR ) {
    if( dr < dRThres ) dRisClose = true;
    ATH_MSG_DEBUG( "   ...-> dR=" << dr << " : dRisClose=" << dRisClose );
  }

  // mass cut
  const double TRACK_MASS = 0;  // just assume zero mass
  bool massIsClose = false;
  double mass = invMass(TRACK_MASS,mf1->pt(),mf1->etaMS(),mf1->phiMS(),TRACK_MASS,mf2->pt(),mf2->etaMS(),mf2->phiMS());

  // for monitoring
  mufastMass = mass;
  double mass_mon = (mass>=monitor_limit) ? mass : monitor_limit;
  mufastMassLog10 = log10(mass_mon);

  if( m_requireMass ) {
    if( mass < massThres ) massIsClose = true;
    ATH_MSG_DEBUG( "   ...-> mass=" << mass << " : massIsClose=" << massIsClose );
  }

  // total judge
  bool overlap = false;
  if( ((m_requireSameSign &&   sameSign)   || (! m_requireSameSign)) &&
      ((m_requireDR       &&  dRisClose)   || (! m_requireDR))       &&
      ((m_requireMass     &&  massIsClose) || (! m_requireMass)) ) {
    overlap = true;
  }

  ATH_MSG_DEBUG( "   ...=> isOverlap=" << overlap );

  return overlap;

}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

double TrigMufastHypoTool::dR(double eta1, double phi1, double eta2, double phi2) const
{
  const double deta = eta1 - eta2;
  const double dphi = CxxUtils::deltaPhi(phi1, phi2);
  return std::sqrt(deta*deta + dphi*dphi);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

double TrigMufastHypoTool::invMass(double m1, double pt1, double eta1, double phi1,
				   double m2, double pt2, double eta2, double phi2) const
{
  const double ZERO_LIMIT = 1e-12;

  double theta1 = 2*atan2((double)exp(-eta1),1.);
  double theta2 = 2*atan2((double)exp(-eta2),1.);

  double fpt1   = fabs(pt1);
  double fpt2   = fabs(pt2);

  double px1    = fpt1*cos(phi1);
  double py1    = fpt1*sin(phi1);
  double pz1    = fpt1/tan(theta1);
  double  e1    = sqrt(px1*px1+py1*py1+pz1*pz1+m1*m1);

  double px2    = fpt2*cos(phi2);
  double py2    = fpt2*sin(phi2);
  double pz2    = fpt2/tan(theta2);
  double  e2    = sqrt(px2*px2+py2*py2+pz2*pz2+m2*m2);

  double pxsum  = px1 + px2;
  double pysum  = py1 + py2;
  double pzsum  = pz1 + pz2;
  double esum   =  e1 +  e2;

  double mass  = 0;
  double mass2 = esum*esum - pxsum*pxsum - pysum*pysum - pzsum*pzsum;
  if( mass2 > ZERO_LIMIT ) mass = sqrt(mass2);

  return mass;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMufastHypoTool::chooseBestMuon(std::vector<TrigMufastHypoTool::MuonClusterInfo*>& input, std::vector<unsigned int> mufastResult) const
{
  size_t numMuon = input.size();
  unsigned int i,j,k;

  auto mufastNrActiveEVs    = Monitored::Scalar("NrActiveEVs", -9999.);
  auto mufastNrOverlapped   = Monitored::Scalar("NrOverlapped", 0);
  auto mufastOverlappedEta  = Monitored::Scalar("OverlappedEta", -9999.);
  auto mufastOverlappedPhi  = Monitored::Scalar("OverlappedPhi", -9999.);
  auto mufastOverlappedPt   = Monitored::Scalar("OverlappedPt", -9999.);

  auto monitorIt       = Monitored::Group(m_monTool, mufastNrActiveEVs, mufastNrOverlapped,
   					  mufastOverlappedPt, mufastOverlappedEta, mufastOverlappedPhi);

  ATH_MSG_DEBUG( "--- choose best among overlaps & disable EVs (muFast based) ---" );
  for(i=0; i<numMuon; i++) {
    ATH_MSG_DEBUG( "++ i=" << i << ": result=" << mufastResult[i] );
    if( mufastResult[i] != i ) {
      ATH_MSG_DEBUG( "   overlap to some one. skip." );

      (*input[i]).passOR = false;

      continue;
    }
    std::vector<unsigned int> others;
    for(j=0; j<numMuon; j++) {
      if( mufastResult[j] == mufastResult[i] ) others.emplace_back(j);
    }
    if( others.size() == 1 ) {
      ATH_MSG_DEBUG( "   unique object. keep it active." );
      continue;
    }
    else {
      // must choose one best
      ATH_MSG_DEBUG( "   overlapped objects among: " << others );
      unsigned int best_ev = 0;
      float maxPtMf  = 0;
      float maxPtRoI = 0;
      for(k=0; k<others.size(); k++) {
	j=others[k];
	// const LVL1::RecMuonRoI* muonRoI = input[j].RecRoI;
	// float ptRoI = muonRoI->getThresholdValue();
	const xAOD::L2StandAloneMuon* mf = (*input[j]).muFast;
	float ptMf  = fabs(mf->pt());
	float ptRoI = mf->roiThreshold();
	ATH_MSG_DEBUG("     ev/PtRoI/ptMf="<< j << "/" << ptRoI << "/" << ptMf);
	if( (ptRoI-maxPtRoI) > 0.1 ) {
	  maxPtRoI = ptRoI;
	  maxPtMf  = ptMf;
	  best_ev  = j;
	}
	else if( fabs(ptRoI-maxPtRoI) < 0.1 ) {
	  if( ptMf > maxPtMf ) {
	    maxPtRoI = ptRoI;
	    maxPtMf  = ptMf;
	    best_ev  = j;
	  }
	}
      }
      ATH_MSG_DEBUG( "     best is: best_ev/maxPtRoI/maxPtMf=" << best_ev << " / " << maxPtRoI << " / " << maxPtMf );

      for(k=0; k<others.size(); k++) {
	j=others[k];
	if( j != best_ev ) {
	  ATH_MSG_DEBUG( "      EventView( j=" << j << " ) is not active" );

	  (*input[j]).passOR = false;

	  // monitoring
	  const xAOD::L2StandAloneMuon* mf = (*input[j]).muFast;
	  mufastNrOverlapped++;
	  mufastOverlappedPt = mf->pt();
	  mufastOverlappedEta = mf->etaMS();
	  mufastOverlappedPhi = mf->phiMS();
	}
	if( j == best_ev ){
	  ATH_MSG_DEBUG( "      EventView( j=" << j << " ) is best one" );
	}
      }
    }
  }
   mufastNrActiveEVs = numMuon - mufastNrOverlapped;

  return StatusCode::SUCCESS;
}
