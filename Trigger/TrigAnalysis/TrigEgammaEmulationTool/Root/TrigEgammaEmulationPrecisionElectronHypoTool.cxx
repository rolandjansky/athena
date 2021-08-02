#include "TrigEgammaEmulationTool/TrigEgammaEmulationPrecisionElectronHypoTool.h"
#include "GaudiKernel/SystemOfUnits.h"


using namespace Trig;

//**********************************************************************

TrigEgammaEmulationPrecisionElectronHypoTool::TrigEgammaEmulationPrecisionElectronHypoTool( const std::string& myname )
    : TrigEgammaEmulationBaseHypoTool(myname)
{}



//=================================================================

StatusCode TrigEgammaEmulationPrecisionElectronHypoTool::initialize() 
{
  ATH_CHECK( TrigEgammaEmulationBaseHypoTool::initialize());
  return StatusCode::SUCCESS;
}

//!==========================================================================

bool TrigEgammaEmulationPrecisionElectronHypoTool::emulate(const Trig::TrigData &input,
                                                           bool &pass) const
{
  pass=false;

  if( !input.roi )  return false;

  if( input.electrons.empty() )  return false;

  if (m_acceptAll){
    pass=true;
    return true;
  }

  for ( const auto& el : input.electrons )
  {
    if( decide( input, el ) ){
      pass=true;
      return true;
    }
  }
  return false;

}


//=================================================================


bool TrigEgammaEmulationPrecisionElectronHypoTool::decide(   const Trig::TrigData &input,
                                                             const xAOD::Electron *el
                                                             ) const

{
  const TrigRoiDescriptor *roi = input.roi;

  // when leaving scope it will ship data to monTool
  unsigned PassedCuts = 0; //got called (data in place)
  

  if ( std::abs( roi->eta() ) > 2.6 ) {
    ATH_MSG_DEBUG( "REJECT The electron had eta coordinates beyond the EM fiducial volume : " << roi->eta() << "; stop the chain now" );       
    return false;
  } 

  ATH_MSG_DEBUG( "; RoI ID = " << roi->roiId() << ": Eta = " << roi->eta() << ", Phi = " << roi->phi() );

  // fill local variables for RoI reference position
  double etaRef = roi->eta();
  double phiRef = roi->phi();
  ATH_MSG_DEBUG("etaRef: "<<etaRef);
  // correct phi the to right range ( probably not needed anymore )   
  if ( std::abs( phiRef ) > M_PI ) phiRef -= 2*M_PI; // correct phi if outside range


  auto pClus = el->caloCluster();
  
  if(!pClus){
    ATH_MSG_DEBUG("No calo cluster for this electron");
    return false;
  }

  float absEta = std::abs( pClus->eta() );
  
  ATH_MSG_DEBUG("absEta: "<<absEta);

  const int cutIndex = findCutIndex( absEta );



  
  float dEta =  pClus->eta() - etaRef;
  //  Deal with angle diferences greater than Pi
  float dPhi =  std::abs( pClus->phi() - phiRef );
  dPhi = ( dPhi < M_PI ? dPhi : 2*M_PI - dPhi ); // TB why only <
  float ET  = pClus->et();

  
  // eta range
  if( absEta > 2.47 ){
    ATH_MSG_DEBUG( "Electron : " << absEta << " outside eta range ");
    return false;
  } 

  PassedCuts = PassedCuts + 1; // passed eta cut
 

  // apply cuts: DeltaEta( clus-ROI )
  ATH_MSG_DEBUG( "Electron : eta="  << pClus->eta() << " roi eta=" << etaRef << " DeltaEta=" << dEta
                  << " cut: <"   << m_detacluster );
  
  if ( std::abs( pClus->eta() - etaRef ) > m_detacluster ) {
    ATH_MSG_DEBUG("REJECT Electron a cut failed");
    return false;
  }
  PassedCuts = PassedCuts + 1; //Deta
  
  // DeltaPhi( clus-ROI )
  ATH_MSG_DEBUG( ": phi="  << pClus->phi() << " roi phi="<< phiRef    << " DeltaPhi="<< dPhi << " cut: <"  << m_dphicluster );
  
  if( dPhi > m_dphicluster ) {
    ATH_MSG_DEBUG("REJECT Clsuter dPhi cut failed");
    return false;
  }
  PassedCuts = PassedCuts + 1; //DPhi



  // eta range
  if ( cutIndex == -1 ) {  // VD
    ATH_MSG_DEBUG( "Electron : " << absEta << " outside eta range " << m_etabin[m_etabin.size()-1] );
    return false;
  } else { 
    ATH_MSG_DEBUG( "eta bin used for cuts " << cutIndex );
  }
  PassedCuts = PassedCuts + 1; // passed eta cut
  
  // ET_em
  ATH_MSG_DEBUG( "Electron: ET_em=" << ET << " cut: >"  << m_eTthr[cutIndex] );
  if ( ET < m_eTthr[cutIndex] ) {
    ATH_MSG_DEBUG("REJECT et cut failed");
    return false;
  }
  PassedCuts = PassedCuts + 1; // ET_em


  // d0 for LRT
  if (m_d0 and m_d0>0.)
  {
    float trk_d0 = std::abs(el->trackParticle()->d0());
    ATH_MSG_DEBUG( "Electron: trk_d0=" << trk_d0 << " cut: >"  << m_d0 );
    if ( trk_d0 < m_d0 ) {
      ATH_MSG_DEBUG("REJECT d0 cut failed");
      return false;
    }
    PassedCuts = PassedCuts + 1; // d0
  }

  
  ATH_MSG_DEBUG("Average mu " << avgmu());

  bool pass = input.isPassed(el , avgmu(), m_pidName);


  float Rhad1(0), Rhad(0), Reta(0), Rphi(0), e277(0), weta2c(0), //emax2(0), 
    Eratio(0), DeltaE(0), f1(0), weta1c(0), wtot(0), fracm(0);
  float ptcone20(999), ptcone30(999), ptcone40(999), etcone20(999), etcone30(999), etcone40(999), topoetcone20(999), topoetcone30(999), topoetcone40(999), relptcone20(999);


  // variables based on HCAL
  // transverse energy in 1st scintillator of hadronic calorimeter/ET
  el->showerShapeValue(Rhad1, xAOD::EgammaParameters::Rhad1);
  // transverse energy in hadronic calorimeter/ET
  el->showerShapeValue(Rhad, xAOD::EgammaParameters::Rhad);

  // variables based on S2 of EM CAL
  // E(7*7) in 2nd sampling
  el->showerShapeValue(e277, xAOD::EgammaParameters::e277);
  // E(3*7)/E(7*7) in 2nd sampling
  el->showerShapeValue(Reta, xAOD::EgammaParameters::Reta);
  // E(3*3)/E(3*7) in 2nd sampling
  el->showerShapeValue(Rphi, xAOD::EgammaParameters::Rphi);
  // shower width in 2nd sampling
  el->showerShapeValue(weta2c, xAOD::EgammaParameters::weta2);

  // variables based on S1 of EM CAL
  // fraction of energy reconstructed in the 1st sampling
  el->showerShapeValue(f1, xAOD::EgammaParameters::f1);
  // shower width in 3 strips in 1st sampling
  el->showerShapeValue(weta1c, xAOD::EgammaParameters::weta1);
  // E of 2nd max between max and min in strips [NOT USED]
  // eg->showerShapeValue(emax2, xAOD::EgammaParameters::e2tsts1);
  // (E of 1st max in strips-E of 2nd max)/(E of 1st max+E of 2nd max)
  el->showerShapeValue(Eratio, xAOD::EgammaParameters::Eratio);
  // E(2nd max)-E(min) in strips
  el->showerShapeValue(DeltaE, xAOD::EgammaParameters::DeltaE);
  // total shower width in 1st sampling
  el->showerShapeValue(wtot, xAOD::EgammaParameters::wtots1);
  // E(+/-3)-E(+/-1)/E(+/-1)
  el->showerShapeValue(fracm, xAOD::EgammaParameters::fracs1);

  el->isolationValue(ptcone20, xAOD::Iso::ptcone20);

  el->isolationValue(ptcone30, xAOD::Iso::ptcone30);

  el->isolationValue(ptcone40, xAOD::Iso::ptcone40);

  el->isolationValue(etcone20, xAOD::Iso::etcone20);

  el->isolationValue(etcone30, xAOD::Iso::etcone30);

  el->isolationValue(etcone40, xAOD::Iso::etcone40);

  el->isolationValue(topoetcone20, xAOD::Iso::topoetcone20);

  el->isolationValue(topoetcone30, xAOD::Iso::topoetcone30);

  el->isolationValue(topoetcone40, xAOD::Iso::topoetcone40);

  ATH_MSG_DEBUG(" electron Cluster Et "<<ET);
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
  ATH_MSG_DEBUG( " trackPT "<<el->trackParticle()->pt());
  ATH_MSG_DEBUG( " d0      "<<el->trackParticle()->d0());
  ATH_MSG_DEBUG( " z0      "<<el->trackParticle()->z0());
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
  relptcone20 = ptcone20/el->pt();
  ATH_MSG_DEBUG("relptcone20 = " <<relptcone20  );
  ATH_MSG_DEBUG("m_RelPtConeCut = " << m_RelPtConeCut );

  // Evaluating lh *after* retrieving variables for monitoing and debuging purposes
  ATH_MSG_DEBUG("AthenaLHSelectorTool: TAccept = " << pass);
  if ( !pass ){
      ATH_MSG_DEBUG("REJECT Likelihood failed");
      return false;
  } else {
      ATH_MSG_DEBUG("ACCEPT Likelihood passed");
  }

  // Check if need to apply isolation
  // First check logic. if cut is very negative, then no isolation cut is defined
  // if m_RelPtConeCut <-100 then hypo is configured not to apply isolation
  if (m_RelPtConeCut < -100){
      ATH_MSG_DEBUG(" not applying isolation. Returning NOW");
      ATH_MSG_DEBUG("TAccept = " << pass);
      return true;
  }
  // Then, It will pass if relptcone20 is less than cut:
  pass = (relptcone20 < m_RelPtConeCut);
  //
  // Reach this point successfully  
  ATH_MSG_DEBUG( "pass = " << pass );
  return pass;



}


int TrigEgammaEmulationPrecisionElectronHypoTool::findCutIndex( float eta ) const {
  const float absEta = std::abs(eta);
  auto binIterator = std::adjacent_find( m_etabin.begin(), m_etabin.end(), [=](float left, float right){ return left < absEta and absEta < right; }  );
  if ( binIterator == m_etabin.end() ) {
    return -1;
  }
  return  binIterator - m_etabin.begin();
}

