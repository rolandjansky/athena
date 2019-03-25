/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "DerivationFrameworkHiggs/MergedElectronDetailsDecorator.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include "egammaInterfaces/IEMExtrapolationTools.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "FourMomUtils/P4Helpers.h"

#include <vector>

namespace DerivationFramework {

  MergedElectronDetailsDecorator::MergedElectronDetailsDecorator(const std::string& t, const std::string& n, const IInterface* p):
    AthAlgTool(t,n,p),
    m_emExtrapolationTool("EMExtrapolationTools"),
    m_VertexFitter("Trk::TrkVkalVrtFitter"),
    m_V0Tools("Trk::V0Tools"),
    m_minET(5000)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("EMExtrapolationTool",m_emExtrapolationTool);
    declareProperty("VertexFitterTool",m_VertexFitter);
    declareProperty("V0Tools",m_V0Tools);
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

    if (m_VertexFitter.retrieve().isFailure()) {
      ATH_MSG_FATAL("Failed to retrieve tool: " << m_VertexFitter);
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Retrieved tool: " << m_VertexFitter);

    if (m_V0Tools.retrieve().isFailure()) {
      ATH_MSG_FATAL("Failed to retrieve tool: " << m_V0Tools);
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Retrieved tool: " << m_V0Tools);




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
      fillTrackDetails(el);
      fillVertexDetails(el);
      fillClusterDetails(el);
    }

    return StatusCode::SUCCESS;

  } // addBranches


  void DerivationFramework::MergedElectronDetailsDecorator::fillMatchDetails( std::vector<float>& trkMatchTrk,
                                                                              const xAOD::TrackParticle* tp,
                                                                              const xAOD::CaloCluster* cluster) const {

    //Reset values
    for(unsigned int i(0); i<trkMatchTrk.size(); ++i ){
      trkMatchTrk[i] = -999;
    }

    std::vector<double>  eta(4, -999.0);
    std::vector<double>  phi(4, -999.0);
    std::vector<double>  deltaEta(4, -999.0);
    std::vector<double>  deltaPhi(4, -999.0);

    bool isTRT = nSiHits(tp) < 7 ? true : false;

    // Dont bother to extrapolate TRT only tracks
    if(isTRT)
      return;


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

    if(m_emExtrapolationTool->matchesAtCalo (cluster,
                                           tp,
                                           isTRT,
                                           Trk::alongMomentum,
                                           eta,
                                           phi,
                                           deltaEta,
                                           deltaPhi,
                                           IEMExtrapolationTools::fromPerigeeRescaled)) //Last Measurement
    {
      trkMatchTrk[8] = deltaPhi[2];
    }
  }


  int  DerivationFramework::MergedElectronDetailsDecorator::nSiHits( const xAOD::TrackParticle * tp ) const
  {
    uint8_t dummy(-1);
    int nPix = tp->summaryValue( dummy, xAOD::numberOfPixelHits )? dummy : 0;
    int nPix_DS = tp->summaryValue( dummy, xAOD::numberOfPixelDeadSensors )? dummy : 0;
    int nSCT = tp->summaryValue( dummy, xAOD::numberOfSCTHits )? dummy : 0;
    int nSCT_DS = tp->summaryValue( dummy, xAOD::numberOfSCTDeadSensors )? dummy : 0;
    return nPix + nPix_DS + nSCT + nSCT_DS;
  }


  void DerivationFramework::MergedElectronDetailsDecorator::fillTrackDetails(const xAOD::Electron* el) const
  {
    std::vector<float> trkMatchTrkP_dEta1(el->nTrackParticles(),-999);
    std::vector<float> trkMatchTrkP_dEta2(el->nTrackParticles(),-999);
    std::vector<float> trkMatchTrkP_dPhi1(el->nTrackParticles(),-999);
    std::vector<float> trkMatchTrkP_dPhi2(el->nTrackParticles(),-999);
    std::vector<float> trkMatchTrkLM_dEta1(el->nTrackParticles(),-999);
    std::vector<float> trkMatchTrkLM_dEta2(el->nTrackParticles(),-999);
    std::vector<float> trkMatchTrkLM_dPhi1(el->nTrackParticles(),-999);
    std::vector<float> trkMatchTrkLM_dPhi2(el->nTrackParticles(),-999);
    std::vector<float> trkMatchTrkR_dPhi2(el->nTrackParticles(),-999);

    auto caloCluster =  el->caloCluster();
    if( caloCluster && caloCluster->pt() > m_minET ){
      std::vector<float> trkMatch(9,-999);
      for( unsigned int i(0); i < el->nTrackParticles(); ++i ){
        auto trackParticle = el->trackParticle( i );
        if(trackParticle){
          fillMatchDetails( trkMatch, trackParticle, caloCluster);
          trkMatchTrkP_dEta1[i]  = trkMatch[0];
          trkMatchTrkP_dEta2[i]  = trkMatch[1];
          trkMatchTrkP_dPhi1[i]  = trkMatch[2];
          trkMatchTrkP_dPhi2[i]  = trkMatch[3];
          trkMatchTrkLM_dEta1[i] = trkMatch[4];
          trkMatchTrkLM_dEta2[i] = trkMatch[5];
          trkMatchTrkLM_dPhi1[i] = trkMatch[6];
          trkMatchTrkLM_dPhi2[i] = trkMatch[7];
          trkMatchTrkR_dPhi2[i]  = trkMatch[8];
        }
      }
    }

    el->auxdecor<std::vector<float>>("TrackMatchingP_dEta1") = trkMatchTrkP_dEta1;
    el->auxdecor<std::vector<float>>("TrackMatchingP_dEta2") = trkMatchTrkP_dEta2;
    el->auxdecor<std::vector<float>>("TrackMatchingP_dPhi1") = trkMatchTrkP_dPhi1;
    el->auxdecor<std::vector<float>>("TrackMatchingP_dPhi2") = trkMatchTrkP_dPhi2;
    el->auxdecor<std::vector<float>>("TrackMatchingLM_dEta1") = trkMatchTrkLM_dEta1;
    el->auxdecor<std::vector<float>>("TrackMatchingLM_dEta2") = trkMatchTrkLM_dEta2;
    el->auxdecor<std::vector<float>>("TrackMatchingLM_dPhi1") = trkMatchTrkLM_dPhi1;
    el->auxdecor<std::vector<float>>("TrackMatchingLM_dPhi2") = trkMatchTrkLM_dPhi2;
    el->auxdecor<std::vector<float>>("TrackMatchingR_dPhi2")  = trkMatchTrkR_dPhi2;
  }

  void DerivationFramework::MergedElectronDetailsDecorator::fillClusterDetails(const xAOD::Electron* el) const
  {
    auto caloCluster =  el->caloCluster();

    std::vector<float> subCluster_E;
    std::vector<float> subCluster_dEta;
    std::vector<float> subCluster_dPhi;

    static SG::AuxElement::Accessor<std::vector<ElementLink<xAOD::CaloClusterContainer> > > clusterLinksAcc("constituentClusterLinks");
    if(caloCluster && clusterLinksAcc.isAvailable(*caloCluster) ){
      std::vector<ElementLink<xAOD::CaloClusterContainer> >  clusterLinks = clusterLinksAcc(*caloCluster);
      for( auto link : clusterLinks){
        if( link.isValid() ){
          subCluster_E.push_back( (*link)->e() );
          subCluster_dEta.push_back( caloCluster->eta() - (*link)->eta() );
          float dphi = P4Helpers::deltaPhi( caloCluster->phi(), (*link)->phi() );
          subCluster_dPhi.push_back(dphi);
        }
      }
    }
    el->auxdecor<std::vector<float>>("SubCluster_E") = subCluster_E;
    el->auxdecor<std::vector<float>>("SubCluster_dEta") = subCluster_dEta;
    el->auxdecor<std::vector<float>>("SubCluster_dPhi") = subCluster_dPhi;
  }


  void DerivationFramework::MergedElectronDetailsDecorator::fillVertexDetails(const xAOD::Electron* el) const
  {
    auto caloCluster =  el->caloCluster();

    float vtxR = -999;
    float vtxZ = -999;
    float vtxM = -999;
    int vtxTrkParticleIndex1 = -999;
    int vtxTrkParticleIndex2 = -999;
    float vtxRerr = -999;
    float vtxZerr = -999;
    float vtxMerr = -999;
    float vtxChi2 = -999;
    int   vtxNdof = -999;
    float vtxdEta = -999;
    float vtxdPhi = -999;

    if( caloCluster && caloCluster->pt() > m_minET ){
      const xAOD::TrackParticle* trk1 = nullptr;
      const xAOD::TrackParticle* trk2 = nullptr;
      int trkIndex1 = -999;
      int trkIndex2 = -999;
      for( unsigned int i(0); i < el->nTrackParticles(); ++i ){
        auto trackParticle = el->trackParticle( i );
        if( nSiHits(trackParticle) >= 7 ) {
          if ( trk1 == nullptr ){
            trk1 = trackParticle;
            trkIndex1 = i;
          } else if( trk2 == nullptr ){
            if( trk1->charge() == trackParticle->charge() )
              continue;
            trk2 = trackParticle;
            trkIndex2 = i;
            break;
          }
        }
      }
      if( trk2 != nullptr){
        // fit vertex

        // Take the initial guess as the first tracks global position
        const Trk::Perigee&  perigeeParameters = trk1->perigeeParameters();
        Amg::Vector3D  startingPoint = perigeeParameters.position();

        std::vector<const xAOD::TrackParticle*> trksToFit;
        trksToFit.push_back( trk1 );
        trksToFit.push_back( trk2 );
        vtxTrkParticleIndex1 = trkIndex1;
        vtxTrkParticleIndex2 = trkIndex2;

        xAOD::Vertex* myVertex = m_VertexFitter->fit( trksToFit, startingPoint );

        if(myVertex){

          vtxR    = m_V0Tools->rxy( myVertex );
          vtxRerr = m_V0Tools->rxyError( myVertex );
          vtxZ    = myVertex->z();
          vtxZerr = sqrt( myVertex->covariancePosition()(2,2) );

          std::vector<double> masses = { 0.511e-3, 0.511e-3 };
          vtxM    = m_V0Tools->invariantMass( myVertex, masses);
          vtxMerr = m_V0Tools->invariantMassError( myVertex, masses);

          vtxChi2 = myVertex->chiSquared();
          vtxNdof = myVertex->numberDoF();

          m_emExtrapolationTool->getEtaPhiAtCalo( myVertex, &vtxdEta, &vtxdPhi);
          
          vtxdPhi  = P4Helpers::deltaPhi( vtxdPhi, caloCluster->phiBE(2) ); 
          vtxdEta -= caloCluster->etaBE(2);
        } else {
          vtxChi2 = 0;
          vtxNdof = 0;
        }
      }
    }
    el->auxdecor<float>("vtxR")    = vtxR;
    el->auxdecor<float>("vtxRerr") = vtxRerr;
    el->auxdecor<float>("vtxZ")    = vtxZ;
    el->auxdecor<float>("vtxZerr") = vtxZerr;
    el->auxdecor<float>("vtxM")    = vtxM;
    el->auxdecor<float>("vtxMerr") = vtxMerr;
    el->auxdecor<int>("vtxTrkParticleIndex1") = vtxTrkParticleIndex1;
    el->auxdecor<int>("vtxTrkParticleIndex2") = vtxTrkParticleIndex2;
    el->auxdecor<float>("vtxChi2") = vtxChi2;
    el->auxdecor<int>("vtxNdof")   = vtxNdof;
    el->auxdecor<float>("vtxdEta") = vtxdEta;
    el->auxdecor<float>("vtxdPhi") = vtxdPhi;
  }




} // namespace
