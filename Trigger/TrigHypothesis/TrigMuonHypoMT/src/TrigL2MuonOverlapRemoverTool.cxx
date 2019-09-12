/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoring/Monitored.h"
#include "TrigL2MuonOverlapRemoverTool.h"
#include "CLHEP/Units/SystemOfUnits.h"

using namespace TrigCompositeUtils;
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonOverlapRemoverTool::TrigL2MuonOverlapRemoverTool(const std::string& type, 
				       const std::string & name,
				       const IInterface* parent )
   : AthAlgTool( type, name, parent ),
     m_decisionId( HLT::Identifier::fromToolName( name ) ) 
{
}

TrigL2MuonOverlapRemoverTool::~TrigL2MuonOverlapRemoverTool(){
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonOverlapRemoverTool::initialize()
{
   ATH_MSG_DEBUG("Initializing " << name() );
 
   ATH_MSG_DEBUG( "Tool configured for chain/id: " << m_decisionId );

   // MufastOverlapRemoval
   if( m_doMufastBasedRemoval ) {
      ATH_MSG_DEBUG( "--- muFast based overlap removal as: ---"     );
      if( m_muFastRequireDR ) {
	 ATH_MSG_DEBUG( "+ dR cut:" );
	 if( (m_muFastEtaBinsEC.size()-1) != m_muFastDRThresEC.size() ) {
	    ATH_MSG_DEBUG( "bad thresholds setup .... exiting!" );
	    return StatusCode::FAILURE;
	 }
	 ATH_MSG_DEBUG( "     B-B : dR < " << m_muFastDRThresBB );
	 ATH_MSG_DEBUG( "     B-E : dR < " << m_muFastDRThresBE );
	 ATH_MSG_DEBUG( "     E-E : " ); 
	 for(unsigned int i=0; i<m_muFastDRThresEC.size(); i++) {
	    ATH_MSG_DEBUG( "        EtaBin " << m_muFastEtaBinsEC[i] << " - " << m_muFastEtaBinsEC[i+1]
		  << " : dR < " << m_muFastDRThresEC[i] );
	 }
      }
      if( m_muFastRequireMass ) {
	 ATH_MSG_DEBUG( "+ Mass cut:" );
	 if( (m_muFastEtaBinsEC.size()-1) != m_muFastMassThresEC.size() ) {
	    ATH_MSG_DEBUG( "bad thresholds setup .... exiting!" );
	    return StatusCode::FAILURE;
	 }
	 ATH_MSG_DEBUG( "     B-B : Mass < " << m_muFastMassThresBB );
	 ATH_MSG_DEBUG( "     B-E : Mass < " << m_muFastMassThresBE );
	 ATH_MSG_DEBUG( "     E-E : " ); 
	 for(unsigned int i=0; i<m_muFastMassThresEC.size(); i++) {
	    ATH_MSG_DEBUG( "        EtaBin " << m_muFastEtaBinsEC[i] << " - " << m_muFastEtaBinsEC[i+1]
		  << " : Mass < " << m_muFastMassThresEC[i] );
	 }
      }
      if( m_muFastRequireSameSign ) ATH_MSG_DEBUG( "+ Same charge sign" );
   }

   // MucombOverlapRemoval
   if( m_doMucombBasedRemoval ) {
      ATH_MSG_DEBUG( "--- muComb based overlap removal as: ---"     );
      if( m_muCombRequireDR ) {
	 ATH_MSG_DEBUG( "+ dR cut:" );
	 if( (m_muCombEtaBins.size()-1) != m_muCombDRThres.size() ) {
	    ATH_MSG_DEBUG( "bad thresholds setup .... exiting!" );
	    return StatusCode::FAILURE;
	 }
	 for(unsigned int i=0; i<m_muCombDRThres.size(); i++) {
	    ATH_MSG_DEBUG( "     EtaBin " << m_muCombEtaBins[i] << " - " << m_muCombEtaBins[i+1]
		  << " : dR < " << m_muCombDRThres[i] );
	 }
      }
      if( m_muCombRequireMufastDR ) {
	 ATH_MSG_DEBUG( "+ dr(by mF) cut:" );
	 if( (m_muCombEtaBins.size()-1) != m_muCombMufastDRThres.size() ) {
	    ATH_MSG_DEBUG( "bad thresholds setup .... exiting!" );
	    return StatusCode::FAILURE;
	 }
	 for(unsigned int i=0; i<m_muCombMufastDRThres.size(); i++) {
	    ATH_MSG_DEBUG( "     EtaBin " << m_muCombEtaBins[i] << " - " << m_muCombEtaBins[i+1]
		  << " : dR(mF) < " << m_muCombMufastDRThres[i] );
	 }
      }
      if( m_muCombRequireMass ) {
	 ATH_MSG_DEBUG( "+ Mass cut:" );
	 if( (m_muCombEtaBins.size()-1) != m_muCombMassThres.size() ) {
	    ATH_MSG_DEBUG( "bad thresholds setup .... exiting!" );
	    return StatusCode::FAILURE;
	 }
	 for(unsigned int i=0; i<m_muCombMassThres.size(); i++) {
	    ATH_MSG_DEBUG( "     EtaBin " << m_muCombEtaBins[i] << " - " << m_muCombEtaBins[i+1]
		  << " : Mass < " << m_muCombMassThres[i] );
	 }
      }
      if( m_muCombRequireSameSign ) ATH_MSG_DEBUG( "+ Same charge sign" );
   }

   if ( not m_monTool.name().empty() ) {
      ATH_CHECK( m_monTool.retrieve() );
      ATH_MSG_DEBUG("MonTool name: " << m_monTool);
   }

   ATH_MSG_DEBUG("Initializing" << name() << "successfully");
 
   return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonOverlapRemoverTool::decide(std::vector<L2SAMuonOverlapInfo>& toolInput) const {

   ATH_MSG_DEBUG("Retrieved from TrigL2MuonOverlapRemoverMufastAlg and Running TrigL2MuonOverlapRemoverTool for selections.");

   std::vector<TrigL2MuonOverlapRemoverTool::L2SAMuonOverlapInfo> input;

   for( unsigned int i=0; i<toolInput.size(); ++i ) {
      if ( TrigCompositeUtils::passed( m_decisionId.numeric(), toolInput[i].previousDecisionIDs) ){
         input.push_back(toolInput[i]);
      }
   }

   size_t numMuon = input.size();

   if ( numMuon == 0) {
      ATH_MSG_DEBUG( "No positive previous hypo decision. Not need overlap removal." );
      auto mufastNrAllEVs     = Monitored::Scalar("NrAllEVs", -9999.);
      auto mufastNrActiveEVs  = Monitored::Scalar("NrActiveEVs", -9999.);
      auto monitorIt          = Monitored::Group(m_monTool, mufastNrAllEVs, mufastNrActiveEVs);
      mufastNrActiveEVs = numMuon;
      mufastNrAllEVs = numMuon;
      return StatusCode::SUCCESS;
   }
   else if ( numMuon == 1 ) {
      ATH_MSG_DEBUG("Number of muon event = " << numMuon );
      ATH_MSG_DEBUG("no overlap Removal necessary. exitting with all EventViews active." );
      auto mufastNrAllEVs     = Monitored::Scalar("NrAllEVs", -9999.);
      auto mufastNrActiveEVs  = Monitored::Scalar("NrActiveEVs", -9999.);
      auto monitorIt          = Monitored::Group(m_monTool, mufastNrAllEVs, mufastNrActiveEVs);
      mufastNrActiveEVs = numMuon;
      mufastNrAllEVs = numMuon;
      if ( TrigCompositeUtils::passed( m_decisionId.numeric(), input[0].previousDecisionIDs) ) {
         TrigCompositeUtils::addDecisionID(m_decisionId, input[0].decision);
      } else {
         ATH_MSG_DEBUG("Not match DecisionID:" << m_decisionId );
      }
      return StatusCode::SUCCESS;
   } else {
      ATH_MSG_DEBUG("Number of muon event = " << numMuon );
      auto mufastNrAllEVs  = Monitored::Scalar("NrAllEVs", -9999.);
      auto monitorIt       = Monitored::Group(m_monTool, mufastNrAllEVs);
      mufastNrAllEVs = numMuon;
      checkOverlap(input);
      return StatusCode::SUCCESS;
   }
   return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonOverlapRemoverTool::checkOverlap(std::vector<L2SAMuonOverlapInfo>& toolInput) const {

   size_t numMuon = toolInput.size();
   unsigned int i,j;
   std::vector<unsigned int> mufastResult;

   for(i=0; i<numMuon; i++) {mufastResult.push_back(i); }
   for(i=0; i<numMuon-1; i++){
      if( TrigCompositeUtils::passed( m_decisionId.numeric(), toolInput[i].previousDecisionIDs ) ){
         for(j=i+1; j<numMuon; j++){
            if( TrigCompositeUtils::passed( m_decisionId.numeric(), toolInput[j].previousDecisionIDs ) ){
               ATH_MSG_DEBUG("++ i=" << i << " vs j=" << j);
               bool overlapped = isOverlap(toolInput[i].overlap, toolInput[j].overlap);
               if( ! overlapped ){ // judged as different
                  ATH_MSG_DEBUG("   judged as: differenr objects");
                  if( mufastResult[i] == mufastResult[j] ) { // but marked as same by someone
      	             ATH_MSG_ERROR( "inconsistentency in muFast based overlap removal for more than two objects" );
      	             ATH_MSG_DEBUG( "two objects are judged as different but both were already marked as identical by someone else as: " );
      	             ATH_MSG_DEBUG( "i/j/result[i]/result[j]=" << i << " / " << j << " / " << mufastResult[i] << " / "  << mufastResult[j] );
                     auto mufastError  = Monitored::Scalar("MufastError", -9999.);
                     auto monitorIt    = Monitored::Group(m_monTool, mufastError);
                     mufastError = TrigL2MuonOverlapRemoverToolConsts::errorCode_inconsistent_overlap1;
                     return StatusCode::FAILURE;
                  }
               }
               else{ // judged as overlap
                  if( (mufastResult[j] != j && mufastResult[i] != mufastResult[j]) || (mufastResult[j] == j && mufastResult[i] != i) ){
                     ATH_MSG_ERROR( "inconsistentency in muFast based overlap removal for more than two objects" );
      	             ATH_MSG_DEBUG( "two objects are judged as overlap but only either was already marked as overlap to someone else: " );
      	             ATH_MSG_DEBUG( "i/j/result[i]/result[j]=" << i << " / " << j << " / " << mufastResult[i] << " / "  << mufastResult[j] );
                     auto mufastError  = Monitored::Scalar("MufastError", -9999.);
                     auto monitorIt    = Monitored::Group(m_monTool, mufastError);
                     mufastError = TrigL2MuonOverlapRemoverToolConsts::errorCode_inconsistent_overlap2;
                     return StatusCode::FAILURE;
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
      } 
   }


   unsigned int n_uniqueMuon = 0;
   for(i=0; i<numMuon; i++) {
     ATH_MSG_DEBUG( "muFast based results: i=" << i << ": ");
     if( mufastResult[i] != i ) { ATH_MSG_DEBUG( "      overlap to j=" << mufastResult[i] ); }
     else {
        n_uniqueMuon++;
        ATH_MSG_DEBUG( "      unique" );
        if( ! TrigCompositeUtils::passed( m_decisionId.numeric(), toolInput[i].previousDecisionIDs ) ){
           ATH_MSG_DEBUG("      ,but not match DecisionID:" << m_decisionId );
        }
     }
   }

   ATH_MSG_DEBUG( "nr of unique Muons after muFast-based removal=" << n_uniqueMuon );

   if( numMuon != n_uniqueMuon ){
      chooseBestMuon(toolInput, mufastResult);
   } else { 
      ATH_MSG_DEBUG( "no overlap identified. exitting with all EventViews active" );
      auto mufastNrActiveEVs  = Monitored::Scalar("NrActiveEVs", -9999.);
      auto monitorIt          = Monitored::Group(m_monTool, mufastNrActiveEVs);
      mufastNrActiveEVs = n_uniqueMuon;
      for(i=0; i<numMuon; i++){
         if( TrigCompositeUtils::passed( m_decisionId.numeric(), toolInput[i].previousDecisionIDs ) ){
            ATH_MSG_DEBUG("Match DecisionID:" << m_decisionId );
            TrigCompositeUtils::addDecisionID( m_decisionId, toolInput[i].decision );
         }
         else{ ATH_MSG_DEBUG( ",but not match DecisionID:" << m_decisionId );}
      }
      return StatusCode::SUCCESS;
   }
   return StatusCode::SUCCESS;
}


// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

bool TrigL2MuonOverlapRemoverTool::isOverlap(const xAOD::L2StandAloneMuon *mf1,
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
      dRThres  =m_muFastDRThresBB; 
      massThres=m_muFastMassThresBB; 
   }
   else if( (isBarrel1 && ! isBarrel2) || (!isBarrel1 && isBarrel2) ) { // BE
      ATH_MSG_DEBUG( "   ...B-E" );
      dRThres  =m_muFastDRThresBE;
      massThres=m_muFastMassThresBE;
   }
   else { // EE
      ATH_MSG_DEBUG( "   ...E-E" );
      double absEta = (fabs(mf1->pt()) > fabs(mf2->pt())) ? fabs(mf1->etaMS()) : fabs(mf2->etaMS());
      unsigned int iThres=0;
      for(unsigned int i=0; i<(m_muFastEtaBinsEC.size()-1); i++) {
         if ( m_muFastEtaBinsEC[i] <= absEta && absEta < m_muFastEtaBinsEC[i+1] ) iThres = i;
      }
      ATH_MSG_DEBUG( "   ...iThres=" << iThres );
      dRThres   = m_muFastDRThresEC[iThres];
      massThres = m_muFastMassThresEC[iThres];
   }
   ATH_MSG_DEBUG( "   ...dR   threshold=" << dRThres );
   ATH_MSG_DEBUG( "   ...mass threshold=" << massThres );


   // same sign cut
   bool sameSign = false;
   if( m_muFastRequireSameSign ) {
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

   if( m_muFastRequireDR ) {
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

   if( m_muFastRequireMass ) {
      if( mass < massThres ) massIsClose = true;
      ATH_MSG_DEBUG( "   ...-> mass=" << mass << " : massIsClose=" << massIsClose );
  }

   // total judge
   bool overlap = false;
   if( ((m_muFastRequireSameSign &&   sameSign)   || (! m_muFastRequireSameSign)) &&
       ((m_muFastRequireDR       &&  dRisClose)   || (! m_muFastRequireDR))       &&
       ((m_muFastRequireMass     &&  massIsClose) || (! m_muFastRequireMass)) ) {
      overlap = true;
   }

   ATH_MSG_DEBUG( "   ...=> isOverlap=" << overlap );

   return overlap;

}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

double TrigL2MuonOverlapRemoverTool::dR(double eta1, double phi1, double eta2, double phi2) const
{
   double deta = eta1 - eta2;
   double dphi = fabs(phi1 - phi2);
   if( dphi > CLHEP::pi ) dphi = CLHEP::twopi - dphi;
   double dR = pow( (deta*deta + dphi*dphi), 0.5 );
   return dR;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

double TrigL2MuonOverlapRemoverTool::invMass(double m1, double pt1, double eta1, double phi1,
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


StatusCode TrigL2MuonOverlapRemoverTool::chooseBestMuon(std::vector<L2SAMuonOverlapInfo>& toolInput, std::vector<unsigned int> mufastResult) const
{
   size_t numMuon = toolInput.size();
   unsigned int i,j,k;

   auto mufastNrActiveEVs    = Monitored::Scalar("NrActiveEVs", -9999.);
   auto mufastNrOverlapped   = Monitored::Scalar("NrOverlapped", 0);
   auto mufastOverlappedEta  = Monitored::Scalar("OverlappedEta", -9999.);
   auto mufastOverlappedPhi  = Monitored::Scalar("OverlappedPhi", -9999.);
   auto mufastOverlappedPt   = Monitored::Scalar("OverlappedPt", -9999.);

   auto monitorIt       = Monitored::Group(m_monTool, mufastNrActiveEVs, mufastNrOverlapped,
                                                      mufastOverlappedPt, mufastOverlappedEta, mufastOverlappedPhi);

   if( m_doMufastBasedRemoval ) {
      ATH_MSG_DEBUG( "--- choose best among overlaps & disable EVs (muFast based) ---" );
      for(i=0; i<numMuon; i++) {
         if( TrigCompositeUtils::passed( m_decisionId.numeric(), toolInput[i].previousDecisionIDs ) ){
	    ATH_MSG_DEBUG( "++ i=" << i << ": result=" << mufastResult[i] );
	    if( mufastResult[i] != i ) {
	       ATH_MSG_DEBUG( "   overlap to some one. skip." );
	       continue;
	    }
	    std::vector<unsigned int> others;
	    for(j=0; j<numMuon; j++) {
	       if( mufastResult[j] == mufastResult[i] ) others.push_back(j);
	    }
	    if( others.size() == 1 ) {
	       ATH_MSG_DEBUG( "   unique object. keep it active." );
               TrigCompositeUtils::addDecisionID( m_decisionId, toolInput[i].decision ); 
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
                  const LVL1::RecMuonRoI* muonRoI = toolInput[j].RecRoI;
                  float ptRoI = muonRoI->getThresholdValue();                  
	          const xAOD::L2StandAloneMuon* mf = toolInput[j].overlap;
	          float ptMf  = fabs(mf->pt());
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
                     
	             // monitoring
	             const xAOD::L2StandAloneMuon* mf = toolInput[j].overlap;
	             mufastNrOverlapped++;
	             mufastOverlappedPt = mf->pt();
	             mufastOverlappedEta = mf->etaMS();
	             mufastOverlappedPhi = mf->phiMS();
	          }
                  if( j == best_ev ){
                     ATH_MSG_DEBUG( "      EventView( j=" << j << " ) is best one" );
                     TrigCompositeUtils::addDecisionID( m_decisionId, toolInput[j].decision ); 
                  }
               }
	    }
         }
         else { ATH_MSG_DEBUG( "this muon is not match previousDecisionID:" << m_decisionId << "  -> skip." ); }
      }
      mufastNrActiveEVs = numMuon - mufastNrOverlapped;
   }


   return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonOverlapRemoverTool::decide(std::vector<L2CBMuonOverlapInfo>& toolInput) const {

   ATH_MSG_DEBUG("Retrieved from TrigL2MuonOverlapRemoverMucombAlg and Running TrigL2MuonOverlapRemoverTool for selections.");

   std::vector<TrigL2MuonOverlapRemoverTool::L2CBMuonOverlapInfo> input;

   for( unsigned int i=0; i<toolInput.size(); ++i ) {
      if ( TrigCompositeUtils::passed( m_decisionId.numeric(), toolInput[i].previousDecisionIDs) ){
         input.push_back(toolInput[i]);
      }
   }

   size_t numMuon = input.size();

   if ( numMuon == 0) {
      ATH_MSG_DEBUG( "No positive previous hypo decision. Not need overlap removal." );
      auto mucombNrAllEVs     = Monitored::Scalar("NrAllEVs", -9999.);
      auto mucombNrActiveEVs  = Monitored::Scalar("NrActiveEVs", -9999.);
      auto monitorIt          = Monitored::Group(m_monTool, mucombNrAllEVs, mucombNrActiveEVs);
      mucombNrActiveEVs = numMuon;
      mucombNrAllEVs = numMuon;
      return StatusCode::SUCCESS;
   }
   else if ( numMuon == 1 ) {
      ATH_MSG_DEBUG("Number of muon event = " << numMuon );
      ATH_MSG_DEBUG("no overlap Removal necessary. exitting with all EventViews active." );
      auto mucombNrAllEVs     = Monitored::Scalar("NrAllEVs", -9999.);
      auto mucombNrActiveEVs  = Monitored::Scalar("NrActiveEVs", -9999.);
      auto monitorIt          = Monitored::Group(m_monTool, mucombNrAllEVs, mucombNrActiveEVs);
      mucombNrActiveEVs = numMuon;
      mucombNrAllEVs = numMuon;
      if ( TrigCompositeUtils::passed( m_decisionId.numeric(), input[0].previousDecisionIDs) ) {
         TrigCompositeUtils::addDecisionID(m_decisionId, input[0].decision);
      } else {
         ATH_MSG_DEBUG("Not match DecisionID:" << m_decisionId );
      }
      return StatusCode::SUCCESS;
   } else {
      ATH_MSG_DEBUG("Number of muon event = " << numMuon );
      auto mucombNrAllEVs  = Monitored::Scalar("NrAllEVs", -9999.);
      auto monitorIt       = Monitored::Group(m_monTool, mucombNrAllEVs);
      mucombNrAllEVs = numMuon;
      checkOverlap(input);
      return StatusCode::SUCCESS;
   }

   return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonOverlapRemoverTool::checkOverlap(std::vector<L2CBMuonOverlapInfo>& toolInput) const {

   size_t numMuon = toolInput.size();
   unsigned int i,j;
   std::vector<unsigned int> mucombResult;

   for(i=0; i<numMuon; i++) {mucombResult.push_back(i); }
   for(i=0; i<numMuon-1; i++){
      if( TrigCompositeUtils::passed( m_decisionId.numeric(), toolInput[i].previousDecisionIDs ) ){
         for(j=i+1; j<numMuon; j++){
            if( TrigCompositeUtils::passed( m_decisionId.numeric(), toolInput[j].previousDecisionIDs ) ){
               ATH_MSG_DEBUG("++ i=" << i << " vs j=" << j);
               bool overlapped = isOverlap(toolInput[i].overlap, toolInput[j].overlap);
               if( ! overlapped ){ // judged as different
                  ATH_MSG_DEBUG("   judged as: differenr objects");
                  if( mucombResult[i] == mucombResult[j] ) { // but marked as same by someone
      	             ATH_MSG_ERROR( "inconsistentency in muComb based overlap removal for more than two objects" );
      	             ATH_MSG_DEBUG( "two objects are judged as different but both were already marked as identical by someone else as: " );
      	             ATH_MSG_DEBUG( "i/j/result[i]/result[j]=" << i << " / " << j << " / " << mucombResult[i] << " / "  << mucombResult[j] );
                     auto mucombError  = Monitored::Scalar("MucombError", -9999.);
                     auto monitorIt          = Monitored::Group(m_monTool, mucombError);
                     mucombError = TrigL2MuonOverlapRemoverToolConsts::errorCode_inconsistent_overlap1;
                     return StatusCode::FAILURE;
                  }
               }
               else{ // judged as overlap
                  if( (mucombResult[j] != j && mucombResult[i] != mucombResult[j]) || (mucombResult[j] == j && mucombResult[i] != i) ){
                     ATH_MSG_ERROR( "inconsistentency in muComb based overlap removal for more than two objects" );
      	             ATH_MSG_DEBUG( "two objects are judged as overlap but only either was already marked as overlap to someone else: " );
      	             ATH_MSG_DEBUG( "i/j/result[i]/result[j]=" << i << " / " << j << " / " << mucombResult[i] << " / "  << mucombResult[j] );
                     auto mucombError  = Monitored::Scalar("MucombError", -9999.);
                     auto monitorIt          = Monitored::Group(m_monTool, mucombError);
                     mucombError = TrigL2MuonOverlapRemoverToolConsts::errorCode_inconsistent_overlap2;
                     return StatusCode::FAILURE;
                  }
                  ATH_MSG_DEBUG("   judged as: overlapped objects");
                  if( mucombResult[i] == i ) {
      	             ATH_MSG_DEBUG( "   i is not yet marked as overlap. so, it is a newly found overlap" );
      	             ATH_MSG_DEBUG( "   -> marking mucombResult[j] as i..." );
      	             mucombResult[j] = i;
      	          } else {
      	             ATH_MSG_DEBUG( "   both i/j already marked as overlap by: mucombResult[i]=" << mucombResult[i] );
      	             ATH_MSG_DEBUG( "   -> do nothing..." );
      	          }
               }
            }
         }
      } 
   }

   unsigned int n_uniqueMuon = 0;
   for(i=0; i<numMuon; i++) {
     ATH_MSG_DEBUG( "muComb based results: i=" << i << ": ");
     if( mucombResult[i] != i ) { ATH_MSG_DEBUG( "      overlap to j=" << mucombResult[i] ); }
     else {
        n_uniqueMuon++;
        ATH_MSG_DEBUG( "      unique" );
        if( ! TrigCompositeUtils::passed( m_decisionId.numeric(), toolInput[i].previousDecisionIDs ) ){
           ATH_MSG_DEBUG("      ,but not match DecisionID:" << m_decisionId );
        }
     }
   }

   ATH_MSG_DEBUG( "nr of unique Muons after muComb-based removal=" << n_uniqueMuon );

   if( numMuon != n_uniqueMuon ){
      chooseBestMuon(toolInput, mucombResult);
   } else { 
      ATH_MSG_DEBUG( "no overlap identified. exitting with all EventViews active" );
      auto mucombNrActiveEVs  = Monitored::Scalar("NrActiveEVs", -9999.);
      auto monitorIt          = Monitored::Group(m_monTool, mucombNrActiveEVs);
      mucombNrActiveEVs = n_uniqueMuon;
      for(i=0; i<numMuon; i++){
         if( TrigCompositeUtils::passed( m_decisionId.numeric(), toolInput[i].previousDecisionIDs ) ){
            ATH_MSG_DEBUG("Match DecisionID:" << m_decisionId );
            TrigCompositeUtils::addDecisionID( m_decisionId, toolInput[i].decision );
         }
         else{ ATH_MSG_DEBUG( ",but not match DecisionID:" << m_decisionId );}
      }
      return StatusCode::SUCCESS;
   }
   return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

bool TrigL2MuonOverlapRemoverTool::isOverlap(const xAOD::L2CombinedMuon *combMf1,
				  	     const xAOD::L2CombinedMuon *combMf2) const
{

   auto mucombDR             = Monitored::Scalar("DR", -9999.);
   auto mucombMass           = Monitored::Scalar("Mass", -9999.);
   auto mucombDRLog10        = Monitored::Scalar("DRLog10", -9999.);
   auto mucombMassLog10      = Monitored::Scalar("MassLog10", -9999.);

   auto monitorIt       = Monitored::Group(m_monTool, mucombDR, mucombMass, mucombDRLog10, mucombMassLog10);


   ATH_MSG_DEBUG( "   ...mF1: pt/eta/phi=" << combMf1->pt()/CLHEP::GeV << " / " << combMf1->eta() << " / " << combMf1->phi() );
   ATH_MSG_DEBUG( "   ...mF2: pt/eta/phi=" << combMf2->pt()/CLHEP::GeV << " / " << combMf2->eta() << " / " << combMf2->phi() );

   // if dR or invMass is necessary but (eta,phi) info is not avaiable
   // (i.e. eta,phi=0,0; rec failed)
   const double ZERO_LIMIT_FOR_ETAPHI = 1e-4;
   if( (fabs(combMf1->eta()) <ZERO_LIMIT_FOR_ETAPHI && fabs(combMf1->phi()) < ZERO_LIMIT_FOR_ETAPHI) ||
       (fabs(combMf2->eta()) <ZERO_LIMIT_FOR_ETAPHI && fabs(combMf2->phi()) < ZERO_LIMIT_FOR_ETAPHI) ) {
      ATH_MSG_DEBUG( "   ...-> (eta,phi) info not available (rec at (eta,phi)=(0,0))" );
      if( m_muCombRequireDR || m_muCombRequireMass ) {
         ATH_MSG_DEBUG( "   ...-> but dR of invMass check is required. cannot judge overlap -> return with false" );
         return false;
      }
   }

   // if charge or invMass is necessary but charge(=pT) info is not avaiable
   const double ZERO_LIMIT_FOR_PT = 1e-4;
   if( (fabs(combMf1->pt()) <ZERO_LIMIT_FOR_PT) || (fabs(combMf2->pt()) < ZERO_LIMIT_FOR_PT) ) {
      ATH_MSG_DEBUG( "   ...-> pT info not available (rec at pT=0)" );
      if( m_muCombRequireSameSign || m_muCombRequireMass ) {
         ATH_MSG_DEBUG( "   ...-> but same sign or invMass check is required. cannot judge overlap -> return with false" );
         return false;
      }
   }

   // determine etabin and thresholds
   double absEta = (fabs(combMf1->pt()) > fabs(combMf2->pt())) ? fabs(combMf1->eta()) : fabs(combMf2->eta());
   unsigned int iThres = 0;
   for(unsigned int i=0; i<(m_muCombEtaBins.size()-1); i++) {
      if ( m_muCombEtaBins[i] <= absEta && absEta < m_muCombEtaBins[i+1] ) iThres = i;
   }
   double dRThres     = m_muCombDRThres[iThres];
   double dRbyMFThres = m_muCombMufastDRThres[iThres];
   double massThres   = m_muCombMassThres[iThres];
   ATH_MSG_DEBUG( "   ...iThres=" << iThres );
   if(m_muCombRequireDR)        ATH_MSG_DEBUG( "   ...dR       threshold=" << dRThres     );
   if(m_muCombRequireMufastDR)  ATH_MSG_DEBUG( "   ...dR(byMF) threshold=" << dRbyMFThres );
   if(m_muCombRequireMass)      ATH_MSG_DEBUG( "   ...mass     threshold=" << massThres   );

   // same sign cut
   bool sameSign = false;
   if( m_muCombRequireSameSign ) {
      sameSign = ((combMf1->pt()*combMf2->pt()) > 0) ? true : false;
      ATH_MSG_DEBUG( "   ...-> sameSign=" << sameSign );
   }

   // dR cut
   bool dRisClose = false;
   double dr = dR(combMf1->eta(),combMf1->phi(),combMf2->eta(),combMf2->phi());

   mucombDR = dr;
   const double monitor_limit = 1e-4;
   double dr_mon = (dr>=monitor_limit) ? dr : monitor_limit;
   mucombDRLog10 = log10(dr_mon);

   if( m_muCombRequireDR ) {
      if( dr < dRThres ) dRisClose = true;
      ATH_MSG_DEBUG( "   ...-> dR=" << dr << " : dRisClose=" << dRisClose );
   }

   // dR(by MF) cut
   bool dRbyMFisClose = false;
   if( m_muCombRequireMufastDR ) {
      const xAOD::L2StandAloneMuon* mf1 = combMf1->muSATrack();
      const xAOD::L2StandAloneMuon* mf2 = combMf2->muSATrack();
      if( mf1 == 0 || mf2 == 0 ) {
         ATH_MSG_DEBUG( "mF link from combinedMF broken" );
         ATH_MSG_DEBUG( "   ...-> mF dR is required but mF link broken. cannot judge overlap -> return with false" );
         return false;
      }
      else {
         // here, we do not check (eta,phi) of mF is not (0,0)
         // (i.e. we apply muComb based cut even if muFast rec is failed)
         double dRByMF = dR(mf1->etaMS(),mf1->phiMS(),mf2->etaMS(),mf2->phiMS());
            if( dRByMF < dRbyMFThres ) dRbyMFisClose = true;
            ATH_MSG_DEBUG( "   ...-> dR(by MF)=" << dRByMF << " : dRbyMFisClose=" << dRbyMFisClose );
      }
   }

   // mass cut
   const double TRACK_MASS = 0;  // just assume zero mass
   bool massIsClose = false;
   double mass = invMass(TRACK_MASS,combMf1->pt()/CLHEP::GeV,combMf1->eta(),combMf1->phi(),TRACK_MASS,combMf2->pt()/CLHEP::GeV,combMf2->eta(),combMf2->phi());

   mucombMass = mass;
   double mass_mon = (mass>=monitor_limit) ? mass : monitor_limit;
   mucombMassLog10 = log10(mass_mon);

   if( m_muCombRequireMass ) {
      if( mass < massThres ) massIsClose = true;
      ATH_MSG_DEBUG( "   ...-> mass=" << mass << " : massIsClose=" << massIsClose );
   }

   // total judge
   bool overlap = false;
   if( ((m_muCombRequireSameSign &&   sameSign)     || (! m_muCombRequireSameSign)) &&
       ((m_muCombRequireDR       &&  dRisClose)     || (! m_muCombRequireDR))       &&
       ((m_muCombRequireMufastDR &&  dRbyMFisClose) || (! m_muCombRequireMufastDR)) &&
       ((m_muCombRequireMass     &&  massIsClose)   || (! m_muCombRequireMass)) ) {
      overlap = true; 
   }

   ATH_MSG_DEBUG( "   ...=> isOverlap=" << overlap );

   return overlap;

}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonOverlapRemoverTool::chooseBestMuon(std::vector<L2CBMuonOverlapInfo>& toolInput, std::vector<unsigned int> mucombResult) const
{
   size_t numMuon = toolInput.size();
   unsigned int i,j,k;

   auto mucombNrActiveEVs    = Monitored::Scalar("NrActiveEVs", -9999.);
   auto mucombNrOverlapped   = Monitored::Scalar("NrOverlapped", 0);
   auto mucombOverlappedEta  = Monitored::Scalar("OverlappedEta", -9999.);
   auto mucombOverlappedPhi  = Monitored::Scalar("OverlappedPhi", -9999.);
   auto mucombOverlappedPt   = Monitored::Scalar("OverlappedPt", -9999.);

   auto monitorIt       = Monitored::Group(m_monTool, mucombNrActiveEVs, mucombNrOverlapped,
                                                      mucombOverlappedPt, mucombOverlappedEta, mucombOverlappedPhi);

   if( m_doMucombBasedRemoval ) {
      ATH_MSG_DEBUG( "--- choose best among overlaps & disable EVs (muComb based) ---" );
      for(i=0; i<numMuon; i++) {
         if( TrigCompositeUtils::passed( m_decisionId.numeric(), toolInput[i].previousDecisionIDs ) ){
	    ATH_MSG_DEBUG( "++ i=" << i << ": result=" << mucombResult[i] );
	    if( mucombResult[i] != i ) {
	       ATH_MSG_DEBUG( "   overlap to some one. skip." );
	       continue;
	    }
	    std::vector<unsigned int> others;
	    for(j=0; j<numMuon; j++) {
	       if( mucombResult[j] == mucombResult[i] ) others.push_back(j);
	    }
	    if( others.size() == 1 ) {
	       ATH_MSG_DEBUG( "   unique object. keep it active." );
               TrigCompositeUtils::addDecisionID( m_decisionId, toolInput[i].decision ); 
	       continue;
	    }
	    else {// must choose one best
	       ATH_MSG_DEBUG( "   overlapped objects among: " << others );
	       unsigned int best_ev = 0;
	       float maxPtCombMf  = 0;
	       for(k=0; k<others.size(); k++) {
	          j=others[k];
                  
	          float ptCombMf  = 0.;
	          const xAOD::L2CombinedMuon* combMf = toolInput[j].overlap;
	          ptCombMf  = fabs(combMf->pt()/CLHEP::GeV);
                  ATH_MSG_DEBUG("     j="<< j << " , ptCombMf=" << ptCombMf);
	          if( ptCombMf > maxPtCombMf ) {
	             maxPtCombMf  = ptCombMf;
	             best_ev  = j;
	          }
               }
	       ATH_MSG_DEBUG( "      best is: best_ev/maxPtCombMf=" << best_ev << " / " <<  maxPtCombMf );
	    
	       for(k=0; k<others.size(); k++) {
	          j=others[k];
	          if( j != best_ev ) {
	             ATH_MSG_DEBUG( "      EventView( j=" << j << " ) is not active" );
                     
	             // monitoring
	             const xAOD::L2CombinedMuon* CombMf = toolInput[j].overlap;
	             mucombNrOverlapped++;
	             mucombOverlappedPt = CombMf->pt()* CombMf->charge() /CLHEP::GeV;
	             mucombOverlappedEta = CombMf->eta();
	             mucombOverlappedPhi = CombMf->phi();
	          }
                  if( j == best_ev ){
                     ATH_MSG_DEBUG( "      EventView( j=" << j << " ) is best one" );
                     TrigCompositeUtils::addDecisionID( m_decisionId, toolInput[j].decision ); 
                  }
               }
	    }
         }
         else { ATH_MSG_DEBUG( "this muon is not match previousDecisionID:" << m_decisionId << "  -> skip." ); }
      }
      mucombNrActiveEVs = numMuon - mucombNrOverlapped;
   }

   return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

