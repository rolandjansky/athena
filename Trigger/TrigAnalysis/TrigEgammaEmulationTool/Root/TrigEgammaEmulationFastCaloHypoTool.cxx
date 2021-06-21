

#include "TrigEgammaEmulationTool/TrigEgammaEmulationFastCaloHypoTool.h"
#include "GaudiKernel/SystemOfUnits.h"


using namespace Trig;

//**********************************************************************

TrigEgammaEmulationFastCaloHypoTool::TrigEgammaEmulationFastCaloHypoTool( const std::string& myname )
    : TrigEgammaEmulationBaseHypoTool(myname)
{}

//!==========================================================================

bool TrigEgammaEmulationFastCaloHypoTool::emulate(const Trig::TrigData &input,
                                                  bool &pass ) const
{
  pass = false;

  if(!input.roi){
    ATH_MSG_DEBUG( "RoI is null ptr");
    return false;
  }

  if(!input.emCluster){
    ATH_MSG_DEBUG( "Cluster is null ptr");
    return false;
  }
  
  pass = m_useRinger? decide_ringer(input) : decide(input);
  
  return true;
}

//!==========================================================================


bool TrigEgammaEmulationFastCaloHypoTool::decide(  const Trig::TrigData &input ) const
{
  const TrigRoiDescriptor *roiDescriptor = input.roi;
  bool pass = false;
  int PassedCuts=0;
  float rCore, F1, F3, Weta2, Wstot, hadET_T2Calo, energyRatio = -1;


  // when leaving scope it will ship data to monTool
  PassedCuts = PassedCuts + 1; //got called (data in place)

  if ( m_acceptAll ) {
    pass = true;
    ATH_MSG_DEBUG( "AcceptAll property is set: taking all events" );
  } else {
    pass = false;
    ATH_MSG_DEBUG( "AcceptAll property not set: applying selection" );
  }


  if ( std::abs( roiDescriptor->eta() ) > 2.6 ) {
    ATH_MSG_DEBUG( "REJECT The cluster had eta coordinates beyond the EM fiducial volume : " << roiDescriptor->eta() << "; stop the chain now" );
    pass=false; // special case       
    return pass;
  } 

  ATH_MSG_DEBUG( "; RoI ID = " << roiDescriptor->roiId()
                  << ": Eta = " << roiDescriptor->eta()
  		            << ", Phi = " << roiDescriptor->phi() );
  
  // fill local variables for RoI reference position
  float etaRef = roiDescriptor->eta();
  float phiRef = roiDescriptor->phi();
  // correct phi the to right range ( probably not needed anymore )   
  if ( std::abs( phiRef ) > M_PI ) phiRef -= 2*M_PI; // correct phi if outside range
  
  auto pClus = input.emCluster;
  float absEta = std::abs( pClus->eta() );

  const int cutIndex = findCutIndex( absEta );
  
  // find if electron is in calorimeter crack
  bool inCrack = ( absEta > 2.37 || ( absEta > 1.37 && absEta < 1.52 ) );
  float dEta =  pClus->eta() - etaRef;
  
  //  Deal with angle diferences greater than Pi
  float dPhi =  std::abs( pClus->phi() - phiRef );
  dPhi = ( dPhi < M_PI ? dPhi : 2*M_PI - dPhi ); // TB why only <
  
  // calculate cluster quantities // definition taken from TrigElectron constructor     
  if ( pClus->emaxs1() + pClus->e2tsts1() > 0 ) 
  energyRatio = ( pClus->emaxs1() - pClus->e2tsts1() ) / ( pClus->emaxs1() + pClus->e2tsts1() );
  
  // ( VD ) here the definition is a bit different to account for the cut of e277 @ EF 
  if ( pClus->e277()!= 0. ) rCore = pClus->e237() / pClus->e277();
  
  //fraction of energy deposited in 1st sampling
  if ( std::abs( pClus->energy() ) > 0.00001 ) F1 = ( pClus->energy( CaloSampling::EMB1 )+pClus->energy( CaloSampling::EME1 ) )/pClus->energy();
  float eT_T2Calo  = pClus->et();
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
  F3 = std::abs( eallsamples )>0. ? e3/eallsamples : 0.; 

  // apply cuts: DeltaEta( clus-ROI )
  ATH_MSG_DEBUG( "TrigEMCluster: eta="  << pClus->eta()
  		            << " roi eta=" << etaRef << " DeltaEta=" << dEta
  		            << " cut: <"   << m_detacluster          );
  
  if ( std::abs( pClus->eta() - etaRef ) > m_detacluster ) {
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


//!==========================================================================


bool TrigEgammaEmulationFastCaloHypoTool::decide_ringer( const Trig::TrigData &input ) const
{


  if ( m_acceptAll ) {
    return true;
    ATH_MSG_DEBUG( "AcceptAll property is set: taking all events" );
  } else {
    ATH_MSG_DEBUG( "AcceptAll property not set: applying selection" );
  }

  if(input.emCluster->et() < m_emEtCut)
  {
    ATH_MSG_DEBUG( "Event reproved by Et threshold. Et = " << input.emCluster->et() << " EtCut = "  << m_emEtCut );
    return false;
  }

  if(!input.rings)
  {
    ATH_MSG_DEBUG( "Ringer is null ptr");
    return false;
  }
  
  
  if( m_pidName == "lhtight"){
    auto inputs = input.ringerTools[0]->prepare_inputs( input.rings , nullptr);
    float output = input.ringerTools[0]->predict( input.rings, inputs );
    return bool( input.ringerTools[0]->accept(input.rings, output, avgmu() ) );
  }else if( m_pidName == "lhmedium"){
    auto inputs = input.ringerTools[1]->prepare_inputs( input.rings , nullptr);
    float output = input.ringerTools[1]->predict( input.rings, inputs );
    return bool( input.ringerTools[1]->accept(input.rings, output, avgmu() ) );
  }else if( m_pidName == "lhloose"){
    auto inputs = input.ringerTools[2]->prepare_inputs( input.rings , nullptr);
    float output = input.ringerTools[2]->predict( input.rings, inputs );
    return bool( input.ringerTools[2]->accept(input.rings, output, avgmu() ) );
  }else if( m_pidName == "lhvloose"){
    auto inputs = input.ringerTools[3]->prepare_inputs( input.rings , nullptr);
    float output = input.ringerTools[3]->predict( input.rings, inputs );
    return bool( input.ringerTools[3]->accept(input.rings, output, avgmu() ) );
  }
  
  // got this far => passed!
  return true;
}


//!==========================================================================

int TrigEgammaEmulationFastCaloHypoTool::findCutIndex( float eta ) const 
{
  const float absEta = std::abs(eta);
  auto binIterator = std::adjacent_find( m_etabin.begin(), m_etabin.end(), [=](float left, float right){ return left < absEta and absEta < right; }  );
  if ( binIterator == m_etabin.end() ) {
    return -1;
  }
  return  binIterator - m_etabin.begin();
}
