#include "TrigEgammaEmulationTool/TrigEgammaEmulationPrecisionPhotonHypoTool.h"
#include "GaudiKernel/SystemOfUnits.h"


using namespace Trig;

//**********************************************************************

TrigEgammaEmulationPrecisionPhotonHypoTool::TrigEgammaEmulationPrecisionPhotonHypoTool( const std::string& myname )
    : TrigEgammaEmulationBaseHypoTool(myname)
{}



//!==========================================================================

bool TrigEgammaEmulationPrecisionPhotonHypoTool::emulate(const Trig::TrigData &input,
                                                         bool &pass) const
{
  pass=false;
  if( !input.roi )  return false;
  if( input.photons.empty() )  return false;

  for ( const auto& ph : input.photons )
  {
    if( decide( input, ph ) ){
      pass=true;
      return true;
    }
  }
  
  return true;

}

//!==========================================================================

bool TrigEgammaEmulationPrecisionPhotonHypoTool::decide(  const Trig::TrigData &input,
                                                          const xAOD::Photon *ph
                                                          ) const

{
  
  const TrigRoiDescriptor *roi = input.roi;
  unsigned PassedCuts=0;

 
  // when leaving scope it will ship data to monTool
  PassedCuts = PassedCuts + 1; //got called (data in place)

  if ( std::abs( roi->eta() ) > 2.6 ) {
      ATH_MSG_DEBUG( "REJECT The photon had eta coordinates beyond the EM fiducial volume : " 
                      << roi->eta() << "; stop the chain now" );
      return false;
  } 

  ATH_MSG_DEBUG( "; RoI ID = " << roi->roiId() << ": Eta = " << roi->eta() << ", Phi = " << roi->phi() );

  // fill local variables for RoI reference position
  double etaRef = roi->eta();
  double phiRef = roi->phi();
  // correct phi the to right range ( probably not needed anymore )   
  if ( std::abs( phiRef ) > M_PI ) phiRef -= 2*M_PI; // correct phi if outside range

  auto pClus = ph->caloCluster();
  
  float absEta = std::abs( pClus->eta() );
  const int cutIndex = findCutIndex( absEta );


  
  float dEta =  pClus->eta() - etaRef;
  //  Deal with angle diferences greater than Pi
  float dPhi =  std::abs( pClus->phi() - phiRef );
  dPhi = ( dPhi < M_PI ? dPhi : 2*M_PI - dPhi ); // TB why only <
  float ET  = pClus->et();


  // eta range
  if ( absEta > 2.47) {  // VD
    ATH_MSG_DEBUG( "Photon : " << absEta << " outside eta range " );
    return false;
  } 
  PassedCuts = PassedCuts + 1; // passed eta cut

  // eta range
  if ( cutIndex == -1 ) {  // VD
    ATH_MSG_DEBUG( "Photon : " << absEta << " outside eta range " << m_etabin[m_etabin.size()-1] );
    return false;
  } else { 
    ATH_MSG_DEBUG( "eta bin used for cuts " << cutIndex );
  }
  PassedCuts = PassedCuts + 1; // passed eta cut
  
  // ET_em
  ATH_MSG_DEBUG( "Photon: ET_em=" << ET << " cut: >"  << m_eTthr[cutIndex] );
  if ( ET < m_eTthr[cutIndex] ) {
    ATH_MSG_DEBUG("REJECT et cut failed");
    return false;
  }
  PassedCuts = PassedCuts + 1; // ET_em




  // apply cuts: DeltaEta( clus-ROI )
  ATH_MSG_DEBUG( "Photon : eta="  << pClus->eta() << " roi eta=" << etaRef << " DeltaEta=" << dEta
                  << " cut: <"   << m_detacluster          );
  
  if ( std::abs( pClus->eta() - etaRef ) > m_detacluster ) {
    ATH_MSG_DEBUG("REJECT Photon a cut failed");
    return false;
  }
  PassedCuts = PassedCuts + 1; //Deta
  
  // DeltaPhi( clus-ROI )
  ATH_MSG_DEBUG( ": phi="  << pClus->phi() << " roi phi="<< phiRef    << " DeltaPhi="<< dPhi 
                  << " cut: <"  << m_dphicluster );

  if( dPhi > m_dphicluster ) {
    ATH_MSG_DEBUG("REJECT Clsuter dPhi cut failed");
    return false;
  }
  PassedCuts = PassedCuts + 1; //DPhi


  // Apply phootn offline like selection
  bool pass=false;

  if (m_pidName=="tight"){
    pass = (bool)input.photonIsEMTools[0]->accept(ph);
  }else if (m_pidName=="medium"){
    pass = (bool)input.photonIsEMTools[1]->accept(ph);
  }else if (m_pidName=="loose"){
    pass =  (bool)input.photonIsEMTools[2]->accept(ph);
  }else{
    pass = true;
  }
  


  float Rhad1(0), Rhad(0), Reta(0), Rphi(0), e277(0), weta2c(0), //emax2(0), 
    Eratio(0), DeltaE(0), f1(0), weta1c(0), wtot(0), fracm(0);
  float ptcone20(999), ptcone30(999), ptcone40(999), etcone20(999), etcone30(999), etcone40(999), topoetcone20(999), topoetcone30(999), topoetcone40(999), reletcone20(999);

    
  // variables based on HCAL
  // transverse energy in 1st scintillator of hadronic calorimeter/ET
  ph->showerShapeValue(Rhad1, xAOD::EgammaParameters::Rhad1);
  // transverse energy in hadronic calorimeter/ET
  ph->showerShapeValue(Rhad, xAOD::EgammaParameters::Rhad);

  // variables based on S2 of EM CAL
  // E(7*7) in 2nd sampling
  ph->showerShapeValue(e277, xAOD::EgammaParameters::e277);
  // E(3*7)/E(7*7) in 2nd sampling
  ph->showerShapeValue(Reta, xAOD::EgammaParameters::Reta);
  // E(3*3)/E(3*7) in 2nd sampling
  ph->showerShapeValue(Rphi, xAOD::EgammaParameters::Rphi);
  // shower width in 2nd sampling
  ph->showerShapeValue(weta2c, xAOD::EgammaParameters::weta2);

  // variables based on S1 of EM CAL
  // fraction of energy reconstructed in the 1st sampling
  ph->showerShapeValue(f1, xAOD::EgammaParameters::f1);
  // shower width in 3 strips in 1st sampling
  ph->showerShapeValue(weta1c, xAOD::EgammaParameters::weta1);
  // E of 2nd max between max and min in strips [NOT USED]
  // eg->showerShapeValue(emax2, xAOD::EgammaParameters::e2tsts1);
  // (E of 1st max in strips-E of 2nd max)/(E of 1st max+E of 2nd max)
  ph->showerShapeValue(Eratio, xAOD::EgammaParameters::Eratio);
  // E(2nd max)-E(min) in strips
  ph->showerShapeValue(DeltaE, xAOD::EgammaParameters::DeltaE);
  // total shower width in 1st sampling
  ph->showerShapeValue(wtot, xAOD::EgammaParameters::wtots1);
  // E(+/-3)-E(+/-1)/E(+/-1)
  ph->showerShapeValue(fracm, xAOD::EgammaParameters::fracs1);

  ph->isolationValue(ptcone20, xAOD::Iso::ptcone20);

  ph->isolationValue(ptcone30, xAOD::Iso::ptcone30);

  ph->isolationValue(ptcone40, xAOD::Iso::ptcone40);

  ph->isolationValue(etcone20, xAOD::Iso::etcone20);

  ph->isolationValue(etcone30, xAOD::Iso::etcone30);

  ph->isolationValue(etcone40, xAOD::Iso::etcone40);

  ph->isolationValue(topoetcone20, xAOD::Iso::topoetcone20);

  ph->isolationValue(topoetcone30, xAOD::Iso::topoetcone30);

  ph->isolationValue(topoetcone40, xAOD::Iso::topoetcone40);

  ATH_MSG_DEBUG( "  Rhad1  " << Rhad1 ) ;
  ATH_MSG_DEBUG( "  Rhad   " << Rhad ) ;
  ATH_MSG_DEBUG( "  e277   " << e277 ) ;
  ATH_MSG_DEBUG( "  Reta   " << Reta ) ;
  ATH_MSG_DEBUG( "  Rphi   " << Rphi ) ;
  ATH_MSG_DEBUG( "  weta2c " << weta2c ) ;
  ATH_MSG_DEBUG( "  f1     " << f1 ) ;
  ATH_MSG_DEBUG( "  weta1c " << weta1c ) ;
  ATH_MSG_DEBUG( "  Eratio " << Eratio ) ;
  ATH_MSG_DEBUG( "  DeltaE " << DeltaE ) ;
  ATH_MSG_DEBUG( "  wtot   " << wtot ) ;
  ATH_MSG_DEBUG( "  fracm  " << fracm ) ;
  ATH_MSG_DEBUG( " ptcone20 " << ptcone20 ) ;
  ATH_MSG_DEBUG( " ptcone30 " << ptcone30 ) ;
  ATH_MSG_DEBUG( " ptcone40 " << ptcone40 ) ;
  ATH_MSG_DEBUG( " etcone20 " << etcone20 ) ;
  ATH_MSG_DEBUG( " etcone30 " << etcone30 ) ;
  ATH_MSG_DEBUG( " etcone40 " << etcone40 ) ;
  ATH_MSG_DEBUG( " topoetcone20 " << topoetcone20 ) ;
  ATH_MSG_DEBUG( " topoetcone30 " << topoetcone30 ) ;
  ATH_MSG_DEBUG( " topoetcone40 " << topoetcone40 ) ;
  // Monitor showershapes                      
  reletcone20 = etcone20/ph->caloCluster()->et();
  ATH_MSG_DEBUG("reletcone20 = " <<reletcone20  );
  ATH_MSG_DEBUG("m_RelEtConeCut = " << m_RelEtConeCut );


  // Decode isEM bits of result to see which bits passed and which bits fialed
  if ( !pass ){
      ATH_MSG_DEBUG("REJECT isEM failed");
      return false;
  } else {
      ATH_MSG_DEBUG("ACCEPT isEM passed");
  }


  // Check if need to apply isolation
  // First check logic. if cut is very negative, then no isolation cut is defined
  // if m_RelEtConeCut <-100 then hypo is configured not to apply isolation
  if (m_RelEtConeCut < -100){
      ATH_MSG_DEBUG(" not applying isolation. Returning NOW");
      ATH_MSG_DEBUG("TAccept = " << pass);
      return true;
  }
  // Then, It will pass if reletcone20 is less than cut:
  pass = (reletcone20 < m_RelEtConeCut);
  //
  // Reach this point successfully  
  ATH_MSG_DEBUG( "pass = " << pass );

  return pass;
  
}

//!==========================================================================

int TrigEgammaEmulationPrecisionPhotonHypoTool::findCutIndex( float eta ) const {
  const float absEta = std::abs(eta);
  auto binIterator = std::adjacent_find( m_etabin.begin(), m_etabin.end(), [=](float left, float right){ return left < absEta and absEta < right; }  );
  if ( binIterator == m_etabin.end() ) {
    return -1;
  }
  return  binIterator - m_etabin.begin();
}
