
#include "TrigEgammaEmulationToolMT/TrigEgammaEmulationPrecisionCaloHypoTool.h"
#include "GaudiKernel/SystemOfUnits.h"


using namespace Trig;

//**********************************************************************

TrigEgammaEmulationPrecisionCaloHypoTool::TrigEgammaEmulationPrecisionCaloHypoTool( const std::string& myname )
    : TrigEgammaEmulationBaseHypoTool(myname)
{}



//!==========================================================================


bool TrigEgammaEmulationPrecisionCaloHypoTool::emulate(const Trig::TrigData &input,
                                                      bool &pass) const
{
  pass=false;

  if( !input.roi )  return false;

  if( input.clusters.empty() )  return false;

  for ( const auto &cl : input.clusters )
  {
    if( decide( input, cl ) ){
      pass=true;
      return true;
    }
  }

  return false;
}




bool TrigEgammaEmulationPrecisionCaloHypoTool::decide(   const Trig::TrigData &input,
                                                         const xAOD::CaloCluster *pClus ) const
{
  unsigned PassedCuts=0;
  
  if(!input.roi){
    ATH_MSG_DEBUG("L1 not found");
    return false;
  }
  
  // when leaving scope it will ship data to monTool
  PassedCuts = PassedCuts + 1; //got called (data in place)

  auto roiDescriptor = input.roi;


  if ( std::abs( roiDescriptor->eta() ) > 2.6 ) {
      ATH_MSG_DEBUG( "REJECT The cluster had eta coordinates beyond the EM fiducial volume : " << roiDescriptor->eta() << "; stop the chain now" );
      return false;
  } 

  ATH_MSG_DEBUG( "; RoI ID = " << roiDescriptor->roiId()
          << ": Eta = " << roiDescriptor->eta()
          << ", Phi = " << roiDescriptor->phi() );

  // fill local variables for RoI reference position
  double etaRef = roiDescriptor->eta();
  double phiRef = roiDescriptor->phi();
  // correct phi the to right range ( probably not needed anymore )   
  if ( std::abs( phiRef ) > M_PI ) phiRef -= 2*M_PI; // correct phi if outside range
  
  float absEta = std::abs( pClus->eta() );

  const int cutIndex = findCutIndex( absEta );
  
  float dEta =  pClus->eta() - etaRef;
  //  Deal with angle diferences greater than Pi
  float dPhi =  std::abs( pClus->phi() - phiRef );
  dPhi = ( dPhi < M_PI ? dPhi : 2*M_PI - dPhi ); // TB why only <
  float eT_Cluster  = pClus->et();
  // apply cuts: DeltaEta( clus-ROI )
  ATH_MSG_DEBUG( "CaloCluster: eta="  << pClus->eta()
  		 << " roi eta=" << etaRef << " DeltaEta=" << dEta
  		 << " cut: <"   << m_detacluster          );
  
  if ( std::abs( pClus->eta() - etaRef ) > m_detacluster ) {
    ATH_MSG_DEBUG("REJECT Cluster dEta cut failed");
    return false;
  }
  PassedCuts = PassedCuts + 1; //Deta
  
  // DeltaPhi( clus-ROI )
  ATH_MSG_DEBUG( ": phi="  << pClus->phi()
  		 << " roi phi="<< phiRef    << " DeltaPhi="<< dPhi
  		 << " cut: <"  << m_dphicluster );
  
  if( dPhi > m_dphicluster ) {
    ATH_MSG_DEBUG("REJECT Clsuter dPhi cut failed");
    return false;
  }
  PassedCuts = PassedCuts + 1; //DPhi



  // eta range
  if ( cutIndex == -1 ) {  // VD
    ATH_MSG_DEBUG( "Cluster eta: " << absEta << " outside eta range " << m_etabin[m_etabin.size()-1] );
    return false;
  } else { 
    ATH_MSG_DEBUG( "eta bin used for cuts " << cutIndex );
  }
  PassedCuts = PassedCuts + 1; // passed eta cut
  
  // ET_em
  ATH_MSG_DEBUG( "CaloCluster: ET_em=" << eT_Cluster << " cut: >"  << m_eTthr[cutIndex] );
  if ( eT_Cluster < m_eTthr[cutIndex] ) {
    ATH_MSG_DEBUG("REJECT et cut failed");
    return false;
  }
  PassedCuts = PassedCuts + 1; // ET_em

  
  // got this far => passed!
  // Reach this point successfully  
  ATH_MSG_DEBUG( "pass!" );

  return true;
 
}

//!==========================================================================

int TrigEgammaEmulationPrecisionCaloHypoTool::findCutIndex( float eta ) const {
  const float absEta = std::abs(eta);
  
  auto binIterator = std::adjacent_find( m_etabin.begin(), m_etabin.end(), [=](float left, float right){ return left < absEta and absEta < right; }  );
  if ( binIterator == m_etabin.end() ) {
    return -1;
  }
  return  binIterator - m_etabin.begin();
}






