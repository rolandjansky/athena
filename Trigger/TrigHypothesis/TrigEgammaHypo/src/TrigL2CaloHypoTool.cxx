/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "DecisionHandling/HLTIdentifier.h"

#include "TrigL2CaloHypoTool.h"



TrigL2CaloHypoTool::TrigL2CaloHypoTool( const std::string& type, 
		    const std::string& name, 
		    const IInterface* parent ) :
  AthAlgTool( type, name, parent ),
  m_id( name ),
  m_monTool( "GenericMonitoringTool/MOnTool", this )
{
  declareProperty( "AcceptAll",      m_acceptAll=true );
  declareProperty( "EtaBins",        m_etabin );
  declareProperty( "ETthr",          m_eTthr );
  declareProperty( "ET2thr",         m_eT2thr );
  declareProperty( "HADETthr",       m_hadeTthr );
  declareProperty( "HADET2thr",      m_hadeT2thr );
  declareProperty( "CARCOREthr",     m_carcorethr );
  declareProperty( "CAERATIOthr",    m_caeratiothr );
  declareProperty( "dETACLUSTERthr", m_detacluster=0.2 );
  declareProperty( "dPHICLUSTERthr", m_dphicluster=0.2 );
  declareProperty( "F1thr",          m_F1thr );  
  declareProperty( "WETA2thr",       m_WETA2thr );
  declareProperty( "WSTOTthr",       m_WSTOTthr );
  declareProperty( "F3thr",          m_F3thr );

  declareProperty( "MonTool", m_monTool=ToolHandle<GenericMonitoringTool>( "", this ) );
}

StatusCode TrigL2CaloHypoTool::initialize()  {
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

  unsigned int nEtaBin=m_etabin.size();
  if ( m_eTthr.size() != nEtaBin-1 ) {
    ATH_MSG_ERROR(  " etThr size is " <<  m_eTthr.size() << " but needs " << nEtaBin-1 );
    return StatusCode::FAILURE;
  }
  
  if ( m_eT2thr.size() != nEtaBin-1 ) {
    ATH_MSG_ERROR(  " et2Thr size is " <<  m_eT2thr.size() << " but needs " << nEtaBin-1 );
    return StatusCode::FAILURE;
  }

  if ( m_hadeTthr.size() != nEtaBin-1 ) {
    ATH_MSG_ERROR(  " hadetThr size is " <<  m_hadeTthr.size() << " but needs " << nEtaBin-1 );
    return StatusCode::FAILURE;
  }

  if ( m_hadeT2thr.size() != nEtaBin-1 ) {
    ATH_MSG_ERROR(  " hadet2Thr size is " <<  m_hadeT2thr.size() << " but needs " << nEtaBin-1 );
    return StatusCode::FAILURE;
  }
  
  if ( m_carcorethr.size() != nEtaBin-1 ) {
    ATH_MSG_ERROR(  " carcore size is " <<  m_carcorethr.size() << " but needs " << nEtaBin-1 );
    return StatusCode::FAILURE;
  }
  
  if ( m_caeratiothr.size() != nEtaBin-1 ) {
    ATH_MSG_ERROR(  " caeratio size is " <<  m_caeratiothr.size() << " but needs " << nEtaBin-1 );
    return StatusCode::FAILURE;
  }

  if ( m_WETA2thr.size() != nEtaBin-1 ) {
    ATH_MSG_ERROR(  " Weta2 size is " <<  m_WETA2thr.size() << " but needs " << nEtaBin-1 );
    return StatusCode::FAILURE;
  }

   if ( m_WSTOTthr.size() != nEtaBin-1 ) {
    ATH_MSG_ERROR(  " Wstot size is " <<  m_WSTOTthr.size() << " but needs " << nEtaBin-1 );
    return StatusCode::FAILURE;
  }

   if ( m_F3thr.size() != nEtaBin-1 ) {
    ATH_MSG_ERROR(  " Wstot size is " <<  m_WSTOTthr.size() << " but needs " << nEtaBin-1 );
    return StatusCode::FAILURE;
  }  

  ATH_MSG_DEBUG( "Tool configured for chain/id: " << m_id  );
  return StatusCode::SUCCESS;
}

StatusCode TrigL2CaloHypoTool::finalize()  {
  return StatusCode::SUCCESS;
}

TrigL2CaloHypoTool::~TrigL2CaloHypoTool()
{}


bool TrigL2CaloHypoTool::decide(  const xAOD::TrigEMCluster* pClus, const TrigRoiDescriptor* roiDescriptor )  const {
  bool pass = false;

  using namespace Monitored;
  // TB Not sure if anything else than the CutCounter should monitored it in every cut tool, 
  // Should be the quantity filled after the succesful selection?

  auto dEta         = MonitoredScalar::declare( "dEta", -1. ); 
  auto dPhi         = MonitoredScalar::declare( "dPhi", -1. );
  auto eT_T2Calo    = MonitoredScalar::declare( "Et_em"   , -1.0 );
  auto hadET_T2Calo = MonitoredScalar::declare( "Et_had", -1.0 );
  auto rCore        = MonitoredScalar::declare( "RCore"       , -1.0 );
  auto energyRatio  = MonitoredScalar::declare( "ERatio" , -1.0 );
  auto etaBin       = MonitoredScalar::declare( "EtaBin", -1. );
  auto monEta       = MonitoredScalar::declare( "Eta", -99. ); 
  auto monPhi       = MonitoredScalar::declare( "Phi", -99. );
  auto F1           = MonitoredScalar::declare( "F1"          , -1.0 );  
  auto Weta2        = MonitoredScalar::declare( "Weta2"       , -1.0 );
  auto Wstot        = MonitoredScalar::declare( "Wstot"       , -1.0 );
  auto F3           = MonitoredScalar::declare( "F3"          , -1.0 );
  auto PassedCuts   = MonitoredScalar::declare<int>( "CutCounter", -1 );  
  auto monitorIt = MonitoredScope::declare( m_monTool, 
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
  

  if ( fabs( roiDescriptor->eta() ) > 2.6 ) {
      ATH_MSG_DEBUG( "The cluster had eta coordinates beyond the EM fiducial volume : " << roiDescriptor->eta() << "; stop the chain now" );
      pass=false; // special case 
      return pass;
  } 

  ATH_MSG_DEBUG( "; RoI ID = "   << roiDescriptor->roiId()
		 << ": Eta = "      << roiDescriptor->eta()
		 << ", Phi = "      << roiDescriptor->phi() );

  // fill local variables for RoI reference position
  double etaRef = roiDescriptor->eta();
  double phiRef = roiDescriptor->phi();
  // correct phi the to right range ( probably not needed anymore )   
  if ( fabs( phiRef ) > M_PI ) phiRef -= 2*M_PI; // correct phi if outside range

  
  float absEta = fabs( pClus->eta() );
  etaBin = -1;
  monEta = pClus->eta();
  monPhi = pClus->phi();
  for ( std::size_t iBin = 0; iBin < m_etabin.size()-1; iBin++ )
    if ( absEta > m_etabin[iBin] && absEta < m_etabin[iBin+1] ) etaBin = iBin; 
 
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
  
  if ( fabs( pClus->eta() - etaRef ) > m_detacluster ) return pass;
  PassedCuts = PassedCuts + 1; //Deta
  
  // DeltaPhi( clus-ROI )
  ATH_MSG_DEBUG( ": phi="  << pClus->phi()
		 << " roi phi="<< phiRef    << " DeltaPhi="<< dPhi
		 << " cut: <"  << m_dphicluster );
  
  if( dPhi > m_dphicluster ) return pass;
  PassedCuts = PassedCuts + 1; //DPhi

  // eta range
  if ( etaBin==-1 ) {  // VD
    ATH_MSG_DEBUG( "Cluster eta: " << absEta << " outside eta range " << m_etabin[m_etabin.size()-1] );
    return pass;
  } else { 
    ATH_MSG_DEBUG( "eta bin used for cuts " << etaBin );
  }
  PassedCuts = PassedCuts + 1; // passed eta cut
  
  // Rcore
  ATH_MSG_DEBUG ( "TrigEMCluster: Rcore=" << rCore 
	  << " cut: >"  << m_carcorethr[etaBin] );
  if ( rCore < m_carcorethr[etaBin] )  return pass;
  PassedCuts = PassedCuts + 1; //Rcore

  // Eratio
  ATH_MSG_DEBUG( " cut: >"  << m_caeratiothr[etaBin] );   
  if ( inCrack || F1 < m_F1thr[0] ) {
    ATH_MSG_DEBUG ( "TrigEMCluster: InCrack= " << inCrack << " F1=" << F1 );
  } else {
    if ( energyRatio < m_caeratiothr[etaBin] ) return pass;
  }
  PassedCuts = PassedCuts + 1; //Eratio
  if( inCrack ) energyRatio = -1; //Set default value in crack for monitoring.
  
  // ET_em
  ATH_MSG_DEBUG( "TrigEMCluster: ET_em=" << eT_T2Calo << " cut: >"  << m_eTthr[etaBin] );
  if ( eT_T2Calo < m_eTthr[etaBin] ) return pass;
  PassedCuts = PassedCuts + 1; // ET_em
 
  float hadET_cut = 0.0;  
  // find which ET_had to apply	: this depends on the ET_em and the eta bin
  if ( eT_T2Calo >  m_eT2thr[etaBin] ) {
    hadET_cut = m_hadeT2thr[etaBin] ;

    ATH_MSG_DEBUG ( "ET_em>"     << m_eT2thr[etaBin] << ": use high ET_had cut: <" << hadET_cut );
  } else {
    hadET_cut = m_hadeTthr[etaBin];

    ATH_MSG_DEBUG ( "ET_em<"    << m_eT2thr[etaBin] << ": use low ET_had cut: <" << hadET_cut );
  }
  
  // ET_had
  ATH_MSG_DEBUG ( "TrigEMCluster: ET_had=" << hadET_T2Calo << " cut: <" << hadET_cut );

  if ( hadET_T2Calo > hadET_cut ) return pass;
  PassedCuts = PassedCuts + 1; //ET_had
  
  // F1
  ATH_MSG_DEBUG ( "TrigEMCluster: F1=" << F1 << " cut: >"  << m_F1thr[0] );
  // if ( m_F1 < m_F1thr[0] ) return pass;  //( VD ) not cutting on this variable, only used to select whether to cut or not on eRatio
  PassedCuts = PassedCuts + 1; //F1


  //Weta2
  ATH_MSG_DEBUG ( "TrigEMCluster: Weta2=" << Weta2 << " cut: <"  << m_WETA2thr[etaBin] ); 
  if ( Weta2 > m_WETA2thr[etaBin] ) return pass;
  PassedCuts = PassedCuts + 1; //Weta2


  //Wstot
  ATH_MSG_DEBUG ( "TrigEMCluster: Wstot=" <<Wstot << " cut: <"  << m_WSTOTthr[etaBin] ); 
  if ( Wstot >= m_WSTOTthr[etaBin] ) return pass;
  PassedCuts = PassedCuts + 1; //Wstot

  //F3
  ATH_MSG_DEBUG( "TrigEMCluster: F3=" << F3 << " cut: <"  << m_F3thr[etaBin] ); 
  if ( F3 > m_F3thr[etaBin] ) return pass;
  PassedCuts = PassedCuts + 1; //F3


  // got this far => passed!
  pass = true;

  // Reach this point successfully  
  ATH_MSG_DEBUG( "pass = " << pass );

  return pass;
}

