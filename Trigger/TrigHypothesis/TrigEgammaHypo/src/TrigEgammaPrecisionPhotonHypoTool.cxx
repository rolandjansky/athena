/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/Combinators.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/PhotonContainer.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "TrigEgammaPrecisionPhotonHypoTool.h"

namespace TCU = TrigCompositeUtils;

TrigEgammaPrecisionPhotonHypoTool::TrigEgammaPrecisionPhotonHypoTool( const std::string& type, 
        const std::string& name, 
        const IInterface* parent ) 
  : base_class( type, name, parent ),
    m_decisionId( HLT::Identifier::fromToolName( name ) ) {
}


StatusCode TrigEgammaPrecisionPhotonHypoTool::initialize()  
{
  ATH_MSG_DEBUG( "Initialization completed successfully"   );    
  ATH_MSG_DEBUG( "EtaBins        = " << m_etabin   );
  ATH_MSG_DEBUG( "ETthr          = " << m_eTthr    );
  ATH_MSG_DEBUG( "dPHICLUSTERthr = " << m_dphicluster );
  ATH_MSG_DEBUG( "dETACLUSTERthr = " << m_detacluster );
  
   if ( m_etabin.empty() ) {
    ATH_MSG_ERROR(  " There are no cuts set (EtaBins property is an empty list)" );
    return StatusCode::FAILURE;
  }

  // Now we try to retrieve the ElectronPhotonSelectorTools that we will use to apply the photon Identification. This is a *must*


  // Retrieving Luminosity info
  ATH_MSG_DEBUG( "Retrieving luminosityCondData..."  );
  ATH_CHECK( m_avgMuKey.initialize() );

  unsigned int nEtaBin = m_etabin.size();

#define CHECK_SIZE( __n) if ( m_##__n.size() !=  (nEtaBin - 1) )		\
    { ATH_MSG_DEBUG(" __n size is " << m_##__n.size() << " but needs to be " << (nEtaBin - 1) ); return StatusCode::FAILURE; }

  CHECK_SIZE( eTthr );
#undef CHECK_SIZE

  ATH_MSG_DEBUG( "Tool configured for chain/id: " << m_decisionId );

  if ( not m_monTool.name().empty() ) 
    CHECK( m_monTool.retrieve() );

  return StatusCode::SUCCESS;
}


bool TrigEgammaPrecisionPhotonHypoTool::decide( const ITrigEgammaPrecisionPhotonHypoTool::PhotonInfo& input ) const 
{

  bool pass = false;

  auto mon_ET              = Monitored::Scalar( "Et_em", -1.0 );
  auto mon_dEta            = Monitored::Scalar( "dEta", -1.0 );
  auto mon_dPhi            = Monitored::Scalar( "dPhi", -1.0 );
  auto mon_etaBin          = Monitored::Scalar( "EtaBin", -1.0 );
  auto mon_Eta             = Monitored::Scalar( "Eta", -99. );
  auto mon_Phi             = Monitored::Scalar( "Phi", -99. );
  auto mon_mu              = Monitored::Scalar("mu",   -1.);
  auto mon_etcone20        = Monitored::Scalar("etcone20",   -99.);
  auto mon_topoetcone20    = Monitored::Scalar("topoetcone20",   -99.);
  auto mon_reletcone20     = Monitored::Scalar("reletcone20",   -99.);
  auto mon_reltopoetcone20 = Monitored::Scalar("reltopoetcone20",   -99.);
  auto PassedCuts          = Monitored::Scalar<int>( "CutCounter", -1 );  
  auto monitorIt           = Monitored::Group( m_monTool, mon_ET, mon_dEta, mon_dPhi, 
                                        mon_etaBin, mon_Eta, mon_Phi, mon_mu, 
                                        mon_etcone20, mon_topoetcone20, mon_reletcone20, mon_reltopoetcone20, PassedCuts );

  // when leaving scope it will ship data to monTool
  PassedCuts = PassedCuts + 1; //got called (data in place)

  float ET(0), dEta(0), dPhi(0), eta(0), phi(0);

  auto roiDescriptor = input.roi;

  if ( fabs( roiDescriptor->eta() ) > 2.6 ) {
      ATH_MSG_DEBUG( "REJECT The photon had eta coordinates beyond the EM fiducial volume : " 
                    << roiDescriptor->eta() << "; stop the chain now" );
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

  auto pClus = input.photon->caloCluster();
  
  float absEta = fabs( pClus->eta() );
  const int cutIndex = findCutIndex( absEta );
  
  dEta =  pClus->eta() - etaRef;
  //  Deal with angle diferences greater than Pi
  dPhi =  fabs( pClus->phi() - phiRef );
  dPhi = ( dPhi < M_PI ? dPhi : 2*M_PI - dPhi ); // TB why only <
  ET  = pClus->et();
  eta = pClus->eta();
  phi = pClus->phi();
  // apply cuts: DeltaEta( clus-ROI )
  ATH_MSG_DEBUG( "Photon : eta="  << pClus->eta()
                  << " roi eta=" << etaRef << " DeltaEta=" << dEta
                  << " cut: <"   << m_detacluster          );
  
  if ( fabs( pClus->eta() - etaRef ) > m_detacluster ) {
    ATH_MSG_DEBUG("REJECT Photon a cut failed");
    return pass;
  }
  mon_Eta = eta;
  mon_dEta = dEta;
  PassedCuts = PassedCuts + 1; //Deta
  
  // DeltaPhi( clus-ROI )
  ATH_MSG_DEBUG( ": phi="  << pClus->phi()
                  << " roi phi="<< phiRef    << " DeltaPhi="<< dPhi
                  << " cut: <"  << m_dphicluster );
  
  if( dPhi > m_dphicluster ) {
    ATH_MSG_DEBUG("REJECT Clsuter dPhi cut failed");
    return pass;
  } 
  mon_Phi = phi;
  mon_dPhi = dPhi;
  PassedCuts = PassedCuts + 1; //DPhi

  // eta range
  if ( cutIndex == -1 ) {  // VD
    ATH_MSG_DEBUG( "Photon : " << absEta << " outside eta range " << m_etabin[m_etabin.size()-1] );
    return pass;
  } else { 
    ATH_MSG_DEBUG( "eta bin used for cuts " << cutIndex );
  }
  mon_etaBin = m_etabin[cutIndex]; 
  PassedCuts = PassedCuts + 1; // passed eta cut
  
  // ET_em
  ATH_MSG_DEBUG( "Photon: ET_em=" << ET << " cut: >"  << m_eTthr[cutIndex] );
  if ( ET < m_eTthr[cutIndex] ) {
    ATH_MSG_DEBUG("REJECT et cut failed");
    return pass;
  }
  mon_ET = ET; 
  PassedCuts = PassedCuts + 1; // ET_em
  
  if(input.pidDecorator.count(m_pidName)){
    pass = input.pidDecorator.at(m_pidName);
  }

  // get average luminosity information to calculate LH
  float avg_mu = 0; 
  SG::ReadDecorHandle<xAOD::EventInfo,float> eventInfoDecor(m_avgMuKey);
  if(eventInfoDecor.isPresent()) {
    avg_mu = eventInfoDecor(0);
    ATH_MSG_DEBUG("Average mu " << avg_mu);
  }
  mon_mu = avg_mu;

  float Rhad1(0), Rhad(0), Reta(0), Rphi(0), e277(0), weta2c(0), //emax2(0), 
        Eratio(0), DeltaE(0), f1(0), weta1c(0), wtot(0), fracm(0);
  float ptcone20(999), ptcone30(999), ptcone40(999), etcone20(999), etcone30(999), 
        etcone40(999), topoetcone20(999), topoetcone30(999), topoetcone40(999), reletcone20(999), reltopoetcone20(999);

    
  // variables based on HCAL
  // transverse energy in 1st scintillator of hadronic calorimeter/ET
  input.photon->showerShapeValue(Rhad1, xAOD::EgammaParameters::Rhad1);
  // transverse energy in hadronic calorimeter/ET
  input.photon->showerShapeValue(Rhad, xAOD::EgammaParameters::Rhad);

  // variables based on S2 of EM CAL
  // E(7*7) in 2nd sampling
  input.photon->showerShapeValue(e277, xAOD::EgammaParameters::e277);
  // E(3*7)/E(7*7) in 2nd sampling
  input.photon->showerShapeValue(Reta, xAOD::EgammaParameters::Reta);
  // E(3*3)/E(3*7) in 2nd sampling
  input.photon->showerShapeValue(Rphi, xAOD::EgammaParameters::Rphi);
  // shower width in 2nd sampling
  input.photon->showerShapeValue(weta2c, xAOD::EgammaParameters::weta2);

  // variables based on S1 of EM CAL
  // fraction of energy reconstructed in the 1st sampling
  input.photon->showerShapeValue(f1, xAOD::EgammaParameters::f1);
  // shower width in 3 strips in 1st sampling
  input.photon->showerShapeValue(weta1c, xAOD::EgammaParameters::weta1);
  // E of 2nd max between max and min in strips [NOT USED]
  // eg->showerShapeValue(emax2, xAOD::EgammaParameters::e2tsts1);
  // (E of 1st max in strips-E of 2nd max)/(E of 1st max+E of 2nd max)
  input.photon->showerShapeValue(Eratio, xAOD::EgammaParameters::Eratio);
  // E(2nd max)-E(min) in strips
  input.photon->showerShapeValue(DeltaE, xAOD::EgammaParameters::DeltaE);
  // total shower width in 1st sampling
  input.photon->showerShapeValue(wtot, xAOD::EgammaParameters::wtots1);
  // E(+/-3)-E(+/-1)/E(+/-1)
  input.photon->showerShapeValue(fracm, xAOD::EgammaParameters::fracs1);

  input.photon->isolationValue(ptcone20, xAOD::Iso::ptcone20);

  input.photon->isolationValue(ptcone30, xAOD::Iso::ptcone30);

  input.photon->isolationValue(ptcone40, xAOD::Iso::ptcone40);

  input.photon->isolationValue(etcone20, xAOD::Iso::etcone20);

  input.photon->isolationValue(etcone30, xAOD::Iso::etcone30);

  input.photon->isolationValue(etcone40, xAOD::Iso::etcone40);

  input.photon->isolationValue(topoetcone20, xAOD::Iso::topoetcone20);

  input.photon->isolationValue(topoetcone30, xAOD::Iso::topoetcone30);

  input.photon->isolationValue(topoetcone40, xAOD::Iso::topoetcone40);

  ATH_MSG_DEBUG( " Rhad         = " << Rhad ) ;
  ATH_MSG_DEBUG( " Rhad1        = " << Rhad1 ) ;
  ATH_MSG_DEBUG( " e277         = " << e277 ) ;
  ATH_MSG_DEBUG( " Reta         = " << Reta ) ;
  ATH_MSG_DEBUG( " Rphi         = " << Rphi ) ;
  ATH_MSG_DEBUG( " weta2c       = " << weta2c ) ;
  ATH_MSG_DEBUG( " f1           = " << f1 ) ;
  ATH_MSG_DEBUG( " weta1c       = " << weta1c ) ;
  ATH_MSG_DEBUG( " Eratio       = " << Eratio ) ;
  ATH_MSG_DEBUG( " DeltaE       = " << DeltaE ) ;
  ATH_MSG_DEBUG( " wtot         = " << wtot ) ;
  ATH_MSG_DEBUG( " fracm        = " << fracm ) ;
  ATH_MSG_DEBUG( " ptcone20     = " << ptcone20 ) ;
  ATH_MSG_DEBUG( " ptcone30     = " << ptcone30 ) ;
  ATH_MSG_DEBUG( " ptcone40     = " << ptcone40 ) ;
  ATH_MSG_DEBUG( " etcone20     = " << etcone20 ) ;
  ATH_MSG_DEBUG( " etcone30     = " << etcone30 ) ;
  ATH_MSG_DEBUG( " etcone40     = " << etcone40 ) ;
  ATH_MSG_DEBUG( " topoetcone20 = " << topoetcone20 ) ;
  ATH_MSG_DEBUG( " topoetcone30 = " << topoetcone30 ) ;
  ATH_MSG_DEBUG( " topoetcone40 = " << topoetcone40 ) ;

  if ( !pass ){
      ATH_MSG_DEBUG("REJECT isEM failed");
      return pass;
  } else {
      ATH_MSG_DEBUG("ACCEPT isEM passed");
  }
  // Monitor showershapes                      
  mon_etcone20 = etcone20;
  reletcone20 = etcone20/input.photon->caloCluster()->et();
  ATH_MSG_DEBUG("reletcone20 = " <<reletcone20  );
  mon_reletcone20 = reletcone20;
  ATH_MSG_DEBUG("m_RelEtConeCut = " << m_RelEtConeCut );

  mon_topoetcone20 = topoetcone20;
  reltopoetcone20 = topoetcone20/input.photon->caloCluster()->et();
  ATH_MSG_DEBUG("reltopoetcone20 = " <<reltopoetcone20  );
  mon_reltopoetcone20 = reltopoetcone20;
  ATH_MSG_DEBUG("m_RelTopoEtConeCut = " << m_RelTopoEtConeCut );

  // Check if need to apply isolation
  // First check logic. if cut is very negative, then no isolation cut is defined
  // Applies to both reletcone20 and reltopoetcone20
  if (m_RelEtConeCut > 900){
      ATH_MSG_DEBUG(" not applying etcone20/et isolation.");
  }
  if (m_RelTopoEtConeCut > 900){
      ATH_MSG_DEBUG(" not applying topoetcone20/et isolation.");
  }
  // Then, It will pass if reletcone20 is less than cut:
  bool pass_reletcone20 = (m_RelEtConeCut > 900 || reletcone20 < m_RelEtConeCut);
  bool pass_reltopoetcone20 = (m_RelTopoEtConeCut > 900 || reltopoetcone20 < m_RelTopoEtConeCut);
  ATH_MSG_DEBUG(" pass_reletcone20 =  "  << pass_reletcone20 );
  ATH_MSG_DEBUG(" pass_reltopoetcone20 =  "  << pass_reltopoetcone20 );
  pass = pass_reletcone20 && pass_reltopoetcone20;
  // Reach this point successfully  
  ATH_MSG_DEBUG( "pass = " << pass );

  return pass;
 
}

int TrigEgammaPrecisionPhotonHypoTool::findCutIndex( float eta ) const {
  const float absEta = std::abs(eta);
  
  auto binIterator = std::adjacent_find( m_etabin.begin(), m_etabin.end(), [=](float left, float right){ return left < absEta and absEta < right; }  );
  if ( binIterator == m_etabin.end() ) {
    return -1;
  }
  return  binIterator - m_etabin.begin();
}


StatusCode TrigEgammaPrecisionPhotonHypoTool::decide( std::vector<PhotonInfo>& input )  const {
  for ( auto& i: input ) {
    if ( TCU::passed ( m_decisionId.numeric(), i.previousDecisionIDs ) ) {
      if ( decide( i ) ) {
        TCU::addDecisionID( m_decisionId, i.decision );
      }
    }
  }
  return StatusCode::SUCCESS;
}
