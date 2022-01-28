/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "DerivationFrameworkHiggs/MergedElectronDetailsDecorator.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertexContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include "egammaInterfaces/IEMExtrapolationTools.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "FourMomUtils/P4Helpers.h"
#include "xAODTruth/xAODTruthHelpers.h"
#include "ElectronPhotonSelectorTools/ElectronSelectorHelpers.h"

#include "GaudiKernel/EventContext.h"

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

    ATH_CHECK(m_eventInfoKey.initialize());

    ATH_CHECK(m_electronKey.initialize());
    ATH_CHECK(m_caloMgrKey.initialize());
    return StatusCode::SUCCESS;
  }

  StatusCode MergedElectronDetailsDecorator::finalize(){
    return StatusCode::SUCCESS;
  }

  StatusCode MergedElectronDetailsDecorator::addBranches() const{

    const EventContext& ctx = Gaudi::Hive::currentContext();

    // Retrieve the xAOD event info
    SG::ReadHandle<xAOD::ElectronContainer> electrons (m_electronKey, ctx);

    SG::ReadHandle<xAOD::EventInfo> eventInfo (m_eventInfoKey, ctx);

    bool isMC = false;
    if( eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION) )
      isMC = true;


    for( const auto *const el : *electrons ){
      fillTrackDetails(el,isMC);
      fillVertexDetails(el);
      fillClusterDetails(el);
    }

    return StatusCode::SUCCESS;

  } // addBranches

  void DerivationFramework::MergedElectronDetailsDecorator::fillTruthDetails( std::vector<float>& trkMatchTrk,
                                                                              const xAOD::TrackParticle* tp,
                                                                              const xAOD::CaloCluster* cluster) const{

   trkMatchTrk.clear();
   const xAOD::TruthParticle* truthPart = xAOD::TruthHelpers::getTruthParticle( *tp );

   if( !truthPart || !truthPart->hasProdVtx() )
      return;

   const auto *prodVtx = truthPart->prodVtx();

   Amg::Vector3D pos( prodVtx->x(), prodVtx->y(), prodVtx->z() );
   Amg::Vector3D mom( truthPart->px(), truthPart->py(), truthPart->pz() );

   Trk::CurvilinearParameters  truthTP( pos, mom, truthPart->charge() );

   const EventContext& ctx = Gaudi::Hive::currentContext();
   float etaAtCalo, phiAtCalo;
   if( m_emExtrapolationTool->getEtaPhiAtCalo ( ctx,
                                           &truthTP,
                                           &etaAtCalo,
                                           &phiAtCalo) ){
     trkMatchTrk.push_back( cluster->etaBE(2) - etaAtCalo );
     trkMatchTrk.push_back( P4Helpers::deltaPhi(cluster->phiBE(2), phiAtCalo) );
   }

   return;

  }

  void DerivationFramework::MergedElectronDetailsDecorator::fillMatchDetails( std::vector<float>& trkMatchTrk,
                                                                              const xAOD::TrackParticle* tp,
                                                                              const xAOD::CaloCluster* cluster) const {

    SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{ m_caloMgrKey };
    const CaloDetDescrManager* caloDDMgr = *caloMgrHandle;

    //Reset values
    for(unsigned int i(0); i<trkMatchTrk.size(); ++i ){
      trkMatchTrk[i] = -999;
    }

    std::array<double,4>  eta = { -999.0, -999.0, -999.0, -999.0 };
    std::array<double,4>  phi = { -999.0, -999.0, -999.0, -999.0 };
    std::array<double,4>  deltaEta = { -999.0, -999.0, -999.0, -999.0 };
    std::array<double,4>  deltaPhi = { -999.0, -999.0, -999.0, -999.0 };

    const EventContext& ctx = Gaudi::Hive::currentContext();
    std::pair<std::vector<CaloSampling::CaloSample>,
              std::vector<std::unique_ptr<Trk::Surface>>>
      layersAndSurfaces =
        m_emExtrapolationTool->getClusterLayerSurfaces(*cluster, *caloDDMgr);

    if (m_emExtrapolationTool
          ->getMatchAtCalo(ctx,
                           *cluster,
                           *tp,
                           layersAndSurfaces.first,
                           layersAndSurfaces.second,
                           eta,
                           phi,
                           deltaEta,
                           deltaPhi,
                           IEMExtrapolationTools::fromPerigee)
          .isSuccess()) // Perigee
    {
      trkMatchTrk[0] = deltaEta[1];
      trkMatchTrk[1] = deltaEta[2];
      trkMatchTrk[2] = deltaPhi[1];
      trkMatchTrk[3] = deltaPhi[2];
    }

    if (m_emExtrapolationTool
          ->getMatchAtCalo(ctx,
                           *cluster,
                           *tp,
                           layersAndSurfaces.first,
                           layersAndSurfaces.second,
                           eta,
                           phi,
                           deltaEta,
                           deltaPhi,
                           IEMExtrapolationTools::fromLastMeasurement)
          .isSuccess()) // Last Measurement
    {
      trkMatchTrk[4] = deltaEta[1];
      trkMatchTrk[5] = deltaEta[2];
      trkMatchTrk[6] = deltaPhi[1];
      trkMatchTrk[7] = deltaPhi[2];
    }

    if(m_emExtrapolationTool->getMatchAtCalo ( ctx,
                                           *cluster,
                                           *tp,
                                           layersAndSurfaces.first,
                                           layersAndSurfaces.second,
                                           eta,
                                           phi,
                                           deltaEta,
                                           deltaPhi,
                                           IEMExtrapolationTools::fromPerigeeRescaled).isSuccess()) //Last Measurement
    {
      trkMatchTrk[8] = deltaPhi[2];
    }
  }


  int  DerivationFramework::MergedElectronDetailsDecorator::nSiHits( const xAOD::TrackParticle * tp )
  {
    uint8_t dummy(-1);
    int nPix = tp->summaryValue( dummy, xAOD::numberOfPixelHits )? dummy : 0;
    int nPix_DS = tp->summaryValue( dummy, xAOD::numberOfPixelDeadSensors )? dummy : 0;
    int nSCT = tp->summaryValue( dummy, xAOD::numberOfSCTHits )? dummy : 0;
    int nSCT_DS = tp->summaryValue( dummy, xAOD::numberOfSCTDeadSensors )? dummy : 0;
    return nPix + nPix_DS + nSCT + nSCT_DS;
  }


  void DerivationFramework::MergedElectronDetailsDecorator::fillTrackDetails(const xAOD::Electron* el,bool isMC) const
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

    std::vector<float> trueMatch_dEta2(el->nTrackParticles(),-999);
    std::vector<float> trueMatch_dPhi2(el->nTrackParticles(),-999);

    const auto *caloCluster =  el->caloCluster();
    if( caloCluster && caloCluster->pt() > m_minET ){
      std::vector<float> trkMatch(9,-999);
      for( unsigned int i(0); i < el->nTrackParticles(); ++i ){
        const auto *trackParticle = el->trackParticle( i );
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

          if(isMC){
            fillTruthDetails( trkMatch, trackParticle, caloCluster );
            if( trkMatch.size() == 2 ){
              trueMatch_dEta2[i] = trkMatch[0];
              trueMatch_dPhi2[i] = trkMatch[1];
            }
          }
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
    el->auxdecor<std::vector<float>>("TrackMatchingTrue_dEta2") = trueMatch_dEta2;
    el->auxdecor<std::vector<float>>("TrackMatchingTrue_dPhi2") = trueMatch_dPhi2;
  }

  void DerivationFramework::MergedElectronDetailsDecorator::fillClusterDetails(const xAOD::Electron* el)
  {
    const auto *caloCluster =  el->caloCluster();

    std::vector<float> subCluster_E;
    std::vector<float> subCluster_dEta;
    std::vector<float> subCluster_dPhi;

    static SG::AuxElement::Accessor<std::vector<ElementLink<xAOD::CaloClusterContainer> > > clusterLinksAcc("constituentClusterLinks");
    if(caloCluster && clusterLinksAcc.isAvailable(*caloCluster) ){
      std::vector<ElementLink<xAOD::CaloClusterContainer> >  clusterLinks = clusterLinksAcc(*caloCluster);
      for( const auto& link : clusterLinks){
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
    const EventContext& ctx = Gaudi::Hive::currentContext();
    const auto *caloCluster =  el->caloCluster();

    float vtxR = -999;
    float vtxZ = -999;
    float vtxM = -999;
    float vtxP = -999;
    int vtxTrkParticleIndex1 = -999;
    int vtxTrkParticleIndex2 = -999;
    float vtxTrkParticle1_dEta2 = -999;
    float vtxTrkParticle1_dPhi2 = -999;
    float vtxTrkParticle2_dEta2 = -999;
    float vtxTrkParticle2_dPhi2 = -999;
    float vtxRerr = -999;
    float vtxZerr = -999;
    float vtxMerr = -999;
    float vtxPerr = -999;
    float vtxChi2 = -999;
    int   vtxNdof = -999;
    float vtxdEta = -999;
    float vtxdPhi = -999;
    float vtxE    = -999;
    float vtxPhi  = -999;
    float vtxEta  = -999;

    if( caloCluster && caloCluster->pt() > m_minET ){
      const xAOD::TrackParticle* trk1 = nullptr;
      const xAOD::TrackParticle* trk2 = nullptr;
      int trkIndex1 = -999;
      int trkIndex2 = -999;
      for( unsigned int i(0); i < el->nTrackParticles(); ++i ){
        const auto *trackParticle = el->trackParticle( i );
        if( nSiHits(trackParticle) >= 7 &&  ElectronSelectorHelpers::passBLayerRequirement(trackParticle) ) {
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

        std::unique_ptr<xAOD::Vertex> myVertex( m_VertexFitter->fit( trksToFit, startingPoint ) );

        if(myVertex){

          vtxR    = m_V0Tools->rxy( myVertex.get() );
          vtxRerr = m_V0Tools->rxyError( myVertex.get() );
          vtxZ    = myVertex->z();
          vtxZerr = myVertex->covariancePosition()(2,2) > 0 ?  sqrt( myVertex->covariancePosition()(2,2) ): myVertex->covariancePosition()(2,2);

          std::vector<double> masses = { 0.511e-3, 0.511e-3 };
          vtxM    = m_V0Tools->invariantMass( myVertex.get(), masses);
          vtxMerr = m_V0Tools->invariantMassError( myVertex.get(), masses);

          vtxP    = m_V0Tools->pT( myVertex.get() );
          vtxPerr = m_V0Tools->pTError( myVertex.get() );

          vtxChi2 = myVertex->chiSquared();
          vtxNdof = myVertex->numberDoF();

          m_emExtrapolationTool->getEtaPhiAtCalo( ctx, myVertex.get(), &vtxdEta, &vtxdPhi);

          vtxdPhi  = P4Helpers::deltaPhi( vtxdPhi, caloCluster->phiBE(2) );
          vtxdEta -= caloCluster->etaBE(2);

          xAOD::TrackParticle::FourMom_t vertex4P = m_V0Tools->V04Momentum(myVertex.get(), 0.511);
          vtxE = vertex4P.E();
          vtxPhi = vertex4P.Phi();
          vtxEta = vertex4P.Eta();
          double momentumScaleFactor = caloCluster->e() / vtxE;

          const auto& perigeeParameters = trk1->perigeeParameters();
          Amg::Vector3D pos = perigeeParameters.position();
          Amg::Vector3D mom = perigeeParameters.momentum();
          mom *= momentumScaleFactor;
          Trk::CurvilinearParameters  scaledTrk( pos, mom, trk1->charge() );

          float etaAtCalo, phiAtCalo;
          if( m_emExtrapolationTool->getEtaPhiAtCalo (ctx,
                                                      &scaledTrk,
                                                      &etaAtCalo,
                                                      &phiAtCalo) ){
            vtxTrkParticle1_dEta2 = caloCluster->etaBE(2) - etaAtCalo;
            vtxTrkParticle1_dPhi2 = P4Helpers::deltaPhi(caloCluster->phiBE(2), phiAtCalo) ;
          }

          const auto& perigeeParameters2 = trk2->perigeeParameters();
          pos = perigeeParameters2.position();
          mom = perigeeParameters2.momentum();
          mom *= momentumScaleFactor;
          scaledTrk = Trk::CurvilinearParameters( pos, mom, trk2->charge() );

          if( m_emExtrapolationTool->getEtaPhiAtCalo (ctx,
                                                      &scaledTrk,
                                                      &etaAtCalo,
                                                      &phiAtCalo) ){
            vtxTrkParticle2_dEta2 = caloCluster->etaBE(2) - etaAtCalo;
            vtxTrkParticle2_dPhi2 = P4Helpers::deltaPhi(caloCluster->phiBE(2), phiAtCalo) ;
          }

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
    el->auxdecor<float>("vtxPt")   = vtxP;
    el->auxdecor<float>("vtxPterr")= vtxPerr;
    el->auxdecor<float>("vtxE")    = vtxE;
    el->auxdecor<float>("vtxEta")  = vtxEta;
    el->auxdecor<float>("vtxPhi")  = vtxPhi;
    el->auxdecor<int>("vtxTrkParticleIndex1") = vtxTrkParticleIndex1;
    el->auxdecor<int>("vtxTrkParticleIndex2") = vtxTrkParticleIndex2;
    el->auxdecor<float>("vtxTrkParticle1_dPhi2") = vtxTrkParticle1_dPhi2;
    el->auxdecor<float>("vtxTrkParticle1_dEta2") = vtxTrkParticle1_dEta2;
    el->auxdecor<float>("vtxTrkParticle2_dPhi2") = vtxTrkParticle2_dPhi2;
    el->auxdecor<float>("vtxTrkParticle2_dEta2") = vtxTrkParticle2_dEta2;
    el->auxdecor<float>("vtxChi2") = vtxChi2;
    el->auxdecor<int>("vtxNdof")   = vtxNdof;
    el->auxdecor<float>("vtxdEta") = vtxdEta;
    el->auxdecor<float>("vtxdPhi") = vtxdPhi;
  }




} // namespace
