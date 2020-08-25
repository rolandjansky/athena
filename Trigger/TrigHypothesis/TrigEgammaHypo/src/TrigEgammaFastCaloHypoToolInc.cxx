/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/Combinators.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "TrigEgammaFastCaloHypoToolInc.h"


using namespace TrigCompositeUtils;

TrigEgammaFastCaloHypoToolInc::TrigEgammaFastCaloHypoToolInc( const std::string& type, 
		    const std::string& name, 
		    const IInterface* parent ) 
  : base_class( type, name, parent ),
    m_selectorTool(),
    m_lumiBlockMuTool("LumiBlockMuTool/LumiBlockMuTool"),
    m_decisionId( HLT::Identifier::fromToolName( name ) ) 
{
declareProperty("LumiBlockMuTool", m_lumiBlockMuTool, "Luminosity Tool" );
}

StatusCode TrigEgammaFastCaloHypoToolInc::initialize()  {

ATH_MSG_DEBUG("Name: "<<name()<<" UseRinger: "<<m_useRinger);

if (m_useRinger){ 
  m_selectorTool.setConstantsCalibPath( m_constantsCalibPath ); 
  m_selectorTool.setThresholdsCalibPath( m_thresholdsCalibPath ); 

  if(m_selectorTool.initialize().isFailure())
    return StatusCode::FAILURE;
  
  if (m_lumiBlockMuTool.retrieve().isFailure())
    return StatusCode::FAILURE;
  
  ATH_MSG_INFO("TrigEgammaFastCaloRingerHypo initialization completed successfully.");
 
}
  ATH_MSG_DEBUG( "Initialization completed successfully"   );   
  ATH_MSG_DEBUG( "AcceptAll           = " << ( m_acceptAll==true ? "True" : "False" ) ); 
  ATH_MSG_DEBUG( "EtaBins        = " << m_etabin      );
  ATH_MSG_DEBUG( "ETthr          = " << m_eTthr    << "( lo )/" << m_eT2thr    << "( hi )" );  
  ATH_MSG_DEBUG( "HADETthr       = " << m_hadeTthr << "( lo )/" << m_hadeT2thr << "( hi )" );
  ATH_MSG_DEBUG( "CARCOREthr     = " << m_carcorethr  );
  ATH_MSG_DEBUG( "CAERATIOthr    = " << m_caeratiothr );
  ATH_MSG_DEBUG( "dPHICLUSTERthr = " << m_dphicluster );
  ATH_MSG_DEBUG( "dETACLUSTERthr = " << m_detacluster );
  ATH_MSG_DEBUG( "WETA2thr     = "   << m_WETA2thr  );
  ATH_MSG_DEBUG( "WSTOTthr     = "   << m_WSTOTthr  );
  ATH_MSG_DEBUG( "F3thr     = "      << m_F3thr  );
  
  if ( m_etabin.size() == 0 ) {
    ATH_MSG_ERROR(  " There are no cuts set (EtaBins property is an empty list)" );
    return StatusCode::FAILURE;
  }

  unsigned int nEtaBin = m_etabin.size();
#define CHECK_SIZE( __n) if ( m_##__n.size() !=  (nEtaBin - 1) )		\
    { ATH_MSG_DEBUG(" __n size is " << m_##__n.size() << " but needs to be " << (nEtaBin - 1) ); return StatusCode::FAILURE; }

  CHECK_SIZE( eTthr );
  CHECK_SIZE( eT2thr );
  CHECK_SIZE( hadeTthr );
  CHECK_SIZE( hadeT2thr );
  CHECK_SIZE( carcorethr );
  CHECK_SIZE( caeratiothr );
  CHECK_SIZE( WETA2thr );
  CHECK_SIZE( WSTOTthr ); 
  CHECK_SIZE( F3thr );
#undef CHECK_SIZE

  ATH_MSG_DEBUG( "Tool configured for chain/id: " << m_decisionId );

  if ( not m_monTool.name().empty() ) 
    CHECK( m_monTool.retrieve() );

  return StatusCode::SUCCESS;
}



TrigEgammaFastCaloHypoToolInc::~TrigEgammaFastCaloHypoToolInc(){}

int TrigEgammaFastCaloHypoToolInc::findCutIndex( float eta ) const {
  const float absEta = std::abs(eta);
  
  auto binIterator = std::adjacent_find( m_etabin.begin(), m_etabin.end(), [=](float left, float right){ return left < absEta and absEta < right; }  );
  if ( binIterator == m_etabin.end() ) {
    return -1;
  }
  return  binIterator - m_etabin.begin();
}

StatusCode TrigEgammaFastCaloHypoToolInc::decide( std::vector<FastClusterInfo>& input )  const {
  for ( auto& i: input ) {
    if ( passed ( m_decisionId.numeric(), i.previousDecisionIDs ) ) {
      if ( decide( i ) ) {
	addDecisionID( m_decisionId, i.decision );
      }
    }
  }
  return StatusCode::SUCCESS;
}

bool TrigEgammaFastCaloHypoToolInc::decide( const ITrigEgammaFastCaloHypoTool::FastClusterInfo& input ) const {
  // ringer selection
  if (m_useRinger){
    return decide_ringer( input );
  }
  // cutbased selection
  else{
    return decide_cutbased( input );
  }
}

bool TrigEgammaFastCaloHypoToolInc::decide_cutbased( const ITrigEgammaFastCaloHypoTool::FastClusterInfo& input ) const {
  
  bool pass = false;

  auto dEta         = Monitored::Scalar( "dEta", -1. ); 
  auto dPhi         = Monitored::Scalar( "dPhi", -1. );
  auto eT_T2Calo    = Monitored::Scalar( "Et_em"   , -1.0 );
  auto hadET_T2Calo = Monitored::Scalar( "Et_had", -1.0 );
  auto rCore        = Monitored::Scalar( "RCore"       , -1.0 );
  auto energyRatio  = Monitored::Scalar( "ERatio" , -1.0 );
  auto etaBin       = Monitored::Scalar( "EtaBin", -1. );
  auto monEta       = Monitored::Scalar( "Eta", -99. ); 
  auto monPhi       = Monitored::Scalar( "Phi", -99. );
  auto F1           = Monitored::Scalar( "F1"          , -1.0 );  
  auto Weta2        = Monitored::Scalar( "Weta2"       , -1.0 );
  auto Wstot        = Monitored::Scalar( "Wstot"       , -1.0 );
  auto F3           = Monitored::Scalar( "F3"          , -1.0 );
  auto PassedCuts   = Monitored::Scalar<int>( "CutCounter", -1 );  
  auto monitorIt    = Monitored::Group( m_monTool, 
                                        dEta, dPhi, eT_T2Calo, hadET_T2Calo,
					                              rCore, energyRatio, etaBin, monEta,
					                              monPhi, F1, Weta2, Wstot, F3, PassedCuts );
  // when leaving scope it will ship data to monTool
  PassedCuts = PassedCuts + 1; //got called (data in place)

  if ( m_acceptAll ) {
    pass = true;
    ATH_MSG_DEBUG( "AcceptAll property is set: taking all events" );
  } else {
    pass = false;
    ATH_MSG_DEBUG( "AcceptAll property not set: applying selection" );
  }

  auto roiDescriptor = input.roi;
  
  if ( fabs( roiDescriptor->eta() ) > 2.6 ) {
    ATH_MSG_DEBUG( "REJECT The cluster had eta coordinates beyond the EM fiducial volume : " << roiDescriptor->eta() << "; stop the chain now" );
    pass=false; // special case       
    return pass;
  } 

  ATH_MSG_DEBUG( "; RoI ID = " << roiDescriptor->roiId()
                  << ": Eta = " << roiDescriptor->eta()
  		            << ", Phi = " << roiDescriptor->phi() );
  
  // fill local variables for RoI reference position
  double etaRef = roiDescriptor->eta();
  double phiRef = roiDescriptor->phi();
  // correct phi the to right range ( probably not needed anymore )   
  if ( fabs( phiRef ) > M_PI ) phiRef -= 2*M_PI; // correct phi if outside range
  
  auto pClus = input.cluster;
  float absEta = fabs( pClus->eta() );
  etaBin = -1;
  monEta = pClus->eta();
  monPhi = pClus->phi();
  const int cutIndex = findCutIndex( absEta );
  
  // find if electron is in calorimeter crack
  bool inCrack = ( absEta > 2.37 || ( absEta > 1.37 && absEta < 1.52 ) );
  dEta =  pClus->eta() - etaRef;
  
  //  Deal with angle diferences greater than Pi
  dPhi =  fabs( pClus->phi() - phiRef );
  dPhi = ( dPhi < M_PI ? dPhi : 2*M_PI - dPhi ); // TB why only <
  
  // calculate cluster quantities // definition taken from TrigElectron constructor     
  if ( pClus->emaxs1() + pClus->e2tsts1() > 0 ) 
  energyRatio = ( pClus->emaxs1() - pClus->e2tsts1() ) / ( pClus->emaxs1() + pClus->e2tsts1() );
  
  // ( VD ) here the definition is a bit different to account for the cut of e277 @ EF 
  if ( pClus->e277()!= 0. ) rCore = pClus->e237() / pClus->e277();
  
  //fraction of energy deposited in 1st sampling
  if ( fabs( pClus->energy() ) > 0.00001 ) F1 = ( pClus->energy( CaloSampling::EMB1 )+pClus->energy( CaloSampling::EME1 ) )/pClus->energy();
  eT_T2Calo  = pClus->et();
  if ( eT_T2Calo!=0 && pClus->eta()!=0 ) hadET_T2Calo = pClus->ehad1()/cosh( fabs( pClus->eta() ) )/eT_T2Calo;
  
  //extract Weta2 varable
  Weta2 = pClus->weta2();
  
  //extract Wstot varable
  Wstot = pClus->wstot();

  //extract F3 ( backenergy i EM calorimeter
  float e0 = pClus->energy( CaloSampling::PreSamplerB ) + pClus->energy( CaloSampling::PreSamplerE );
  float e1 = pClus->energy( CaloSampling::EMB1 ) + pClus->energy( CaloSampling::EME1 );
  float e2 = pClus->energy( CaloSampling::EMB2 ) + pClus->energy( CaloSampling::EME2 );
  float e3 = pClus->energy( CaloSampling::EMB3 ) + pClus->energy( CaloSampling::EME3 );
  float eallsamples = e0+e1+e2+e3;
  F3 = fabs( eallsamples )>0. ? e3/eallsamples : 0.; 

  // apply cuts: DeltaEta( clus-ROI )
  ATH_MSG_DEBUG( "TrigEMCluster: eta="  << pClus->eta()
  		            << " roi eta=" << etaRef << " DeltaEta=" << dEta
  		            << " cut: <"   << m_detacluster          );
  
  if ( fabs( pClus->eta() - etaRef ) > m_detacluster ) {
       ATH_MSG_DEBUG("REJECT Cluster dEta cut failed");
       return pass;
    }
  PassedCuts = PassedCuts + 1; //Deta
  
  // DeltaPhi( clus-ROI )
  ATH_MSG_DEBUG( ": phi="  << pClus->phi()
            		 << " roi phi="<< phiRef    << " DeltaPhi="<< dPhi
  		           << " cut: <"  << m_dphicluster );
  
  if( dPhi > m_dphicluster ) {
    ATH_MSG_DEBUG("REJECT Clsuter dPhi cut failed");
    return pass;
  }
  PassedCuts = PassedCuts + 1; //DPhi

  // eta range
  if ( cutIndex == -1 ) {  // VD
    ATH_MSG_DEBUG( "Cluster eta: " << absEta << " outside eta range " << m_etabin[m_etabin.size()-1] );
    return pass;
  } 
  else { 
    ATH_MSG_DEBUG( "eta bin used for cuts " << cutIndex );
  }
  PassedCuts = PassedCuts + 1; // passed eta cut
  
  // Rcore
  ATH_MSG_DEBUG ( "TrigEMCluster: Rcore=" << rCore 
  		            << " cut: >"  << m_carcorethr[cutIndex] );
  if ( rCore < m_carcorethr[cutIndex] ) {
    ATH_MSG_DEBUG("REJECT rCore cut failed");
    return pass;
  }
  PassedCuts = PassedCuts + 1; //Rcore

  // Eratio
  ATH_MSG_DEBUG( " cut: >"  << m_caeratiothr[cutIndex] );   
  if ( inCrack || F1 < m_F1thr[0] ) {
    ATH_MSG_DEBUG ( "TrigEMCluster: InCrack= " << inCrack << " F1=" << F1 );
  } 
  else {
    if ( energyRatio < m_caeratiothr[cutIndex] ) {
      ATH_MSG_DEBUG("REJECT e ratio cut failed");
      return pass;
    }
  }
  PassedCuts = PassedCuts + 1; //Eratio
  if( inCrack ) energyRatio = -1; //Set default value in crack for monitoring.
  
  // ET_em
  ATH_MSG_DEBUG( "TrigEMCluster: ET_em=" << eT_T2Calo << " cut: >"  << m_eTthr[cutIndex] );
  if ( eT_T2Calo < m_eTthr[cutIndex] ) {
    ATH_MSG_DEBUG("REJECT et cut failed");
    return pass;
  }
  PassedCuts = PassedCuts + 1; // ET_em
 
  float hadET_cut = 0.0;  
  // find which ET_had to apply	: this depends on the ET_em and the eta bin
  if ( eT_T2Calo >  m_eT2thr[cutIndex] ) {
    hadET_cut = m_hadeT2thr[cutIndex] ;
    ATH_MSG_DEBUG ( "ET_em>"     << m_eT2thr[cutIndex] << ": use high ET_had cut: <" << hadET_cut );
  } 
  else {
    hadET_cut = m_hadeTthr[cutIndex];
    ATH_MSG_DEBUG ( "ET_em<"    << m_eT2thr[cutIndex] << ": use low ET_had cut: <" << hadET_cut );
  }
  
  // ET_had
  ATH_MSG_DEBUG ( "TrigEMCluster: ET_had=" << hadET_T2Calo << " cut: <" << hadET_cut );
  if ( hadET_T2Calo > hadET_cut ) {
    ATH_MSG_DEBUG("REJECT et had cut failed");
    return pass;
  }
  PassedCuts = PassedCuts + 1; //ET_had
  
  // F1
  ATH_MSG_DEBUG ( "TrigEMCluster: F1=" << F1 << " cut: >"  << m_F1thr[0] );
  PassedCuts = PassedCuts + 1; //F1

  //Weta2
  ATH_MSG_DEBUG ( "TrigEMCluster: Weta2=" << Weta2 << " cut: <"  << m_WETA2thr[cutIndex] ); 
  if ( Weta2 > m_WETA2thr[cutIndex] ) {
    ATH_MSG_DEBUG("REJECT weta 2 cut failed");
    return pass;
  }
  PassedCuts = PassedCuts + 1; //Weta2

  //Wstot
  ATH_MSG_DEBUG ( "TrigEMCluster: Wstot=" <<Wstot << " cut: <"  << m_WSTOTthr[cutIndex] ); 
  if ( Wstot >= m_WSTOTthr[cutIndex] ) {
    ATH_MSG_DEBUG("REJECT wstot cut failed");
    return pass;
  }
  PassedCuts = PassedCuts + 1; //Wstot

  //F3
  ATH_MSG_DEBUG( "TrigEMCluster: F3=" << F3 << " cut: <"  << m_F3thr[cutIndex] ); 
  if ( F3 > m_F3thr[cutIndex] ) {
    ATH_MSG_DEBUG("REJECT F3 cut failed");
    return pass;
  }
  PassedCuts = PassedCuts + 1; //F3

  // got this far => passed!
  pass = true;

  // Reach this point successfully  
  ATH_MSG_DEBUG( "pass = " << pass );

  return pass;

}

bool TrigEgammaFastCaloHypoToolInc::decide_ringer ( const ITrigEgammaFastCaloHypoTool::FastClusterInfo& input) const{

  auto etMon          =  Monitored::Scalar("Et",-100);
  auto monEta         =  Monitored::Scalar("Eta",-100);
  auto monPhi         =  Monitored::Scalar("Phi",-100); 
  auto rnnOutMon      =  Monitored::Scalar("RnnOut",-100);
  auto total_time     = Monitored::Timer("TIME_total");
  auto propagate_time = Monitored::Timer("TIME_propagate");
  auto preproc_time   = Monitored::Timer("TIME_preproc");
  auto decide_time    = Monitored::Timer("TIME_decide");
  auto mon = Monitored::Group(m_monTool,etMon,monEta,monPhi,rnnOutMon,
                              total_time,propagate_time,preproc_time,decide_time);

  
  if ( m_acceptAll ) {
    ATH_MSG_DEBUG( "AcceptAll property is set: taking all events" );
    return true;
  } else {
    ATH_MSG_DEBUG( "AcceptAll property not set: applying selection" );
  }

  total_time.start();
  auto ringerShape = input.ringerShape;
  const xAOD::TrigEMCluster *emCluster = 0;
  if(ringerShape){
    emCluster = ringerShape->emCluster();
    //emCluster= input.cluster;
    if(!emCluster){
      ATH_MSG_WARNING( "There is no link to xAOD::TrigEMCluster into the Ringer object.");
      return false;
    }
  }
  else{
    ATH_MSG_WARNING( "There is no xAOD::TrigRingerRings link into the rnnOutput object.");
    return false;
  }

  float eta     = std::fabs(emCluster->eta());
  float et      = emCluster->et() / Gaudi::Units::GeV;
  float avgmu   = m_lumiBlockMuTool->averageInteractionsPerCrossing();
  
  if(eta>2.50) eta=2.50;///fix for events out of the ranger

  // make sure that monitoring histogram will plotting only the events of chain.
  ///Et threshold
  if(et < m_emEtCut/Gaudi::Units::GeV){
    ATH_MSG_DEBUG( "Event reproved by Et threshold. Et = " << et << ", EtCut = " << m_emEtCut/Gaudi::Units::GeV);
    return false;
  }

  monEta = emCluster->eta();
  etMon  = emCluster->et();
  monPhi = emCluster->phi();
  
  const std::vector<float> rings = ringerShape->rings();
  std::vector<float> refRings(rings.size());
  refRings.assign(rings.begin(), rings.end());
  
  ATH_MSG_DEBUG("Et = "<< et << " Eta = "<<eta << " mu = " << avgmu << "rsize = "<< refRings.size()); 

  auto output = m_selectorTool.calculate( refRings, et, eta, avgmu, propagate_time, preproc_time );
  rnnOutMon = output;
  ATH_MSG_DEBUG(name()<< " generate as NN output " <<  output );
  
  decide_time.start();
  bool accept = m_selectorTool.accept(output, et,eta,avgmu);
  decide_time.stop();

  total_time.stop();
  return accept;
}