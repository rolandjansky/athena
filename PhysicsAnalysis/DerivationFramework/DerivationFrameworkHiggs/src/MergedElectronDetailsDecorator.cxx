/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "DerivationFrameworkHiggs/MergedElectronDetailsDecorator.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODEgamma/ElectronContainer.h"


#include "egammaInterfaces/IEMExtrapolationTools.h"
#include <vector>

namespace DerivationFramework {
  
  MergedElectronDetailsDecorator::MergedElectronDetailsDecorator(const std::string& t, const std::string& n, const IInterface* p):
    AthAlgTool(t,n,p),
    m_emExtrapolationTool("EMExtrapolationTools"),
    m_minET(5000)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("EMExtrapolationTool",m_emExtrapolationTool);
    declareProperty("MinET",m_minET);
  }
  
  MergedElectronDetailsDecorator::~MergedElectronDetailsDecorator() {}
  
  
  StatusCode MergedElectronDetailsDecorator::initialize() {
    
    ATH_MSG_INFO("Initialize " );

    if (m_emExtrapolationTool.retrieve().isFailure()) {
      ATH_MSG_FATAL("Failed to retrieve tool: " << m_emExtrapolationTool); 
      return StatusCode::FAILURE; 
    } 
    ATH_MSG_INFO("Retrieved tool: " << m_emExtrapolationTool);

    return StatusCode::SUCCESS;
  }
  
  StatusCode MergedElectronDetailsDecorator::finalize(){
    return StatusCode::SUCCESS;
  }
  
  StatusCode MergedElectronDetailsDecorator::addBranches() const{    

    // Retrieve the xAOD event info
    const xAOD::ElectronContainer *electrons = nullptr;
    ATH_CHECK( evtStore()->retrieve(electrons,"Electrons") );
    if( !electrons )
    {
      return StatusCode::FAILURE;
    }

    for( const auto& el : *electrons ){
      
      std::vector<float> trkMatchTrk0(8,-999);
      std::vector<float> trkMatchTrk1(8,-999);
      std::vector<float> trkMatchTrk2(8,-999);
      
      auto caloCluster =  el->caloCluster();

      if( caloCluster && caloCluster->pt() > m_minET ){ 

        for( unsigned int i(0); i < el->nTrackParticles(); ++i ){
          if( i > 2 ) {
            break;
          }
          
          auto trackParticle = el->trackParticle( i );
          if(trackParticle){        
            if( i == 0 ) {
              fillMatchDetails( trkMatchTrk0, trackParticle, caloCluster);
            } else if( i == 1 ) {
              fillMatchDetails( trkMatchTrk1, trackParticle, caloCluster);
            } else if( i == 2 ) {
              fillMatchDetails( trkMatchTrk2, trackParticle, caloCluster);
            }
          }
        }  
      }
         // delta layer {1,2}  {Eta, phi}, {first and last measurement}  

      // Decorate the enums
      el->auxdecor<std::vector<float>>("Track0Matching") = trkMatchTrk0 ;
      el->auxdecor<std::vector<float>>("Track1Matching") = trkMatchTrk1 ;
      el->auxdecor<std::vector<float>>("Track2Matching") = trkMatchTrk2 ;
    }

    return StatusCode::SUCCESS;

  } // addBranches
  
 
  void DerivationFramework::MergedElectronDetailsDecorator::fillMatchDetails( std::vector<float>& trkMatchTrk, 
                                                                              const xAOD::TrackParticle* tp, 
                                                                              const xAOD::CaloCluster* cluster) const {

    std::vector<double>  eta(4, -999.0);
    std::vector<double>  phi(4, -999.0);   
    std::vector<double>  deltaEta(4, -999.0);
    std::vector<double>  deltaPhi(4, -999.0);   
  
    uint8_t dummy(-1);
    int nPix = tp->summaryValue( dummy, xAOD::numberOfPixelHits )? dummy :-1;
    int nPix_DS = tp->summaryValue( dummy, xAOD::numberOfPixelDeadSensors )? dummy :-1;
    int nSCT = tp->summaryValue( dummy, xAOD::numberOfSCTHits )? dummy :-1; 
    int nSCT_DS = tp->summaryValue( dummy, xAOD::numberOfSCTDeadSensors )? dummy :-1;

    bool isTRT = nPix + nPix_DS + nSCT + nSCT_DS < 7 ? true : false;

    if(m_emExtrapolationTool->matchesAtCalo (cluster, 
                                           tp, 
                                           isTRT,
                                           Trk::alongMomentum, 
                                           eta,
                                           phi,
                                           deltaEta, 
                                           deltaPhi, 
                                           IEMExtrapolationTools::fromPerigee)) // Perigee
    {
      trkMatchTrk[0] = deltaEta[1];   
      trkMatchTrk[1] = deltaEta[2];   
      trkMatchTrk[2] = deltaPhi[1];   
      trkMatchTrk[3] = deltaPhi[2];   
    }
    
    if(m_emExtrapolationTool->matchesAtCalo (cluster, 
                                           tp, 
                                           isTRT,
                                           Trk::alongMomentum, 
                                           eta,
                                           phi,
                                           deltaEta, 
                                           deltaPhi, 
                                           IEMExtrapolationTools::fromLastMeasurement)) //Last Measurement
    {
      trkMatchTrk[4] = deltaEta[1];   
      trkMatchTrk[5] = deltaEta[2];   
      trkMatchTrk[6] = deltaPhi[1];   
      trkMatchTrk[7] = deltaPhi[2];   
    }

  }

} // namespace
