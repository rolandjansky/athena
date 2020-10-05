/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          ConversionFinder.cxx  -  Description
                             -------------------
    begin   : 10-11-2005
    authors : Tatjana Lenz
    email   : tatjana.lenz@cern.ch
    changes :
 ***************************************************************************/
#include "InDetConversionFinder/ConversionFinder.h"

#include "TrkTrack/TrackCollection.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/EventContext.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexAuxContainer.h"

#include "egammaInterfaces/IEMExtrapolationTools.h"


namespace InDet
{
  ConversionFinder::ConversionFinder(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
  m_tracksName("InDetTrackParticles"),
  m_InDetConversionOutputName("InDetConversion"),
  m_EMExtrapolationTool("EMExtrapolationTools"),
  m_doExtrapolation(false)
  {
    /* Retrieve StoreGate container and tool names from job options */
    declareProperty("TracksName",m_tracksName);
    declareProperty("InDetConversionOutputName", m_InDetConversionOutputName);
    declareProperty("ExtrapolationTool", m_EMExtrapolationTool, "Handle of the extrapolation tool");
    declareProperty("doExtrapolation", m_doExtrapolation );
	}

  ConversionFinder::~ConversionFinder(){}

  StatusCode ConversionFinder::initialize()
  {
    ATH_CHECK( m_VertexFinderTool.retrieve() );

    m_doExtrapolation &= !m_EMExtrapolationTool.name().empty();
    ATH_CHECK( m_EMExtrapolationTool.retrieve( EnableTool {m_doExtrapolation} ) );

    ATH_CHECK( m_tracksName.initialize() );
    ATH_CHECK( m_InDetConversionOutputName.initialize() );
    resetStatistics();

    return StatusCode::SUCCESS;
  }

  StatusCode ConversionFinder::finalize()
  {
    if ( msg().level() == MSG::INFO ){
      msg(MSG::INFO) <<"Summary:  "<<endmsg;
      std::cout<< "------------------------------------------------------------"<<std::endl;
      std::cout<< "Processed: " << m_events_processed<< " events " <<std::endl;
      std::cout<< "Stored   : " << m_Gamma_stored<<" Conversions " <<std::endl;
      std::cout<< "------------------------------------------------------------"<<std::endl;
      std::cout<< "| Double Conversions       Si-Si      Si-TRT      TRT-TRT  |"<<std::endl;
      std::cout<<"| "<<
      std::setw(10)<<m_Double_Conversions<<
      std::setw(18)<<m_SiSi_Conversions<<
      std::setw(11)<<m_SiTrt_Conversions<<
      std::setw(13)<<m_TrtTrt_Conversions<<"     |"<<
      std::endl;
      std::cout<< "------------------------------------------------------------" <<std::endl;
      std::cout<< "| Single Conversions               Si                TRT   |"<<std::endl;
      std::cout<<"| "<<
      std::setw(10)<<m_Single_Conversions<<
      std::setw(25)<<m_Si_Conversions<<
      std::setw(19)<<m_Trt_Conversions<<"    |"<<
      std::endl;
      std::cout<< "------------------------------------------------------------" <<std::endl;
    }
    return StatusCode::SUCCESS;
  }

  void ConversionFinder::resetStatistics() {
    m_events_processed   = 0;
    m_Gamma_stored       = 0;
    m_Double_Conversions = 0;
    m_Single_Conversions = 0;
    m_SiSi_Conversions   = 0;
    m_SiTrt_Conversions  = 0;
    m_TrtTrt_Conversions = 0;
    m_Si_Conversions     = 0;
    m_Trt_Conversions    = 0;
  }

  namespace {
  class cleanup_pair : public std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
  {
  public:
    cleanup_pair(const std::pair<xAOD::VertexContainer*,
                                 xAOD::VertexAuxContainer*>& a_pair)
      : std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>(a_pair)
    {}
    ~cleanup_pair() {
      delete this->first;
      delete this->second;
    }
    xAOD::VertexContainer* releaseFirst() {
      xAOD::VertexContainer *tmp=this->first;
      this->first=nullptr;
      return  tmp;
    }
    xAOD::VertexAuxContainer* releaseSecond() {
      xAOD::VertexAuxContainer *tmp=this->second;
      this->second=nullptr;
      return  tmp;
    }
  };
  }

  StatusCode ConversionFinder::execute()
  {

    const EventContext& ctx =Algorithm::getContext();
    m_events_processed++;

    SG::ReadHandle<xAOD::TrackParticleContainer> trackParticleCollection(m_tracksName,ctx);
    if ( !trackParticleCollection.isValid())
    {
      ATH_MSG_WARNING( "Could not find xAOD::TrackParticleContainer " << m_tracksName << " in StoreGate.");
      // @TODO change to failure
      return StatusCode::SUCCESS;
    }

    cleanup_pair conversions( m_VertexFinderTool->findVertex ( trackParticleCollection.cptr() ) );

    if (!conversions.first || !conversions.second)
    {
      ATH_MSG_WARNING("Null pointer to conversion container");
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("New conversion container size: " << conversions.first->size());

    m_Gamma_stored += conversions.first->size();


    // Decorate the vertices with the momentum at the conversion point and
    // etaAtCalo, phiAtCalo (extrapolate each vertex)
    if(m_doExtrapolation){
      float etaAtCalo = -9999., phiAtCalo = -9999.;
      for (xAOD::Vertex *vertex : *(conversions.first))
      {

        Amg::Vector3D momentum(0., 0., 0.);
        for (unsigned int i = 0; i < vertex->nTrackParticles(); ++i){
          momentum += m_EMExtrapolationTool->getMomentumAtVertex(ctx,*vertex, i);
        }

        vertex->auxdata<float>("px") = momentum.x();
        vertex->auxdata<float>("py") = momentum.y();
        vertex->auxdata<float>("pz") = momentum.z();

        if (!m_EMExtrapolationTool->getEtaPhiAtCalo(ctx,vertex, &etaAtCalo, &phiAtCalo))
        {
          ATH_MSG_DEBUG("getEtaPhiAtCalo failed!");
        }

        // Decorate vertex with etaAtCalo, phiAtCalo
        vertex->auxdata<float>("etaAtCalo") = etaAtCalo;
        vertex->auxdata<float>("phiAtCalo") = phiAtCalo;
      }
    }

    analyzeResults(conversions.first);

    SG::WriteHandle<xAOD::VertexContainer> output(m_InDetConversionOutputName,ctx);
    if (output.record( std::unique_ptr<xAOD::VertexContainer>(conversions.releaseFirst()) ,
                       std::unique_ptr<xAOD::VertexAuxContainer>(conversions.releaseSecond())).isFailure()) {
      ATH_MSG_ERROR("Failed to record conversion vertices " << m_InDetConversionOutputName.key());
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

  void ConversionFinder::analyzeResults(xAOD::VertexContainer* convContainer) {

    for ( auto fz : *convContainer){

      int numTracksPerVertex = fz->nTrackParticles();
      if  (numTracksPerVertex == 2) m_Double_Conversions++;
      else                          m_Single_Conversions++;

      bool isTrt1 = false; bool isSi1 = false; bool isTrt2 = false; bool isSi2 = false;
      for (unsigned int i = 0; i < fz->nTrackParticles() ; ++i) {
        auto trackParticle = fz->trackParticle( i );
        if(!trackParticle) continue;
        uint8_t temp(0);
        uint8_t ncl(0);
        uint8_t ntrt(0);

        if( trackParticle->summaryValue( temp , xAOD::numberOfPixelHits) ) ncl += temp;
        if( trackParticle->summaryValue( temp , xAOD::numberOfSCTHits)   ) ncl += temp;
        if( trackParticle->summaryValue( temp , xAOD::numberOfTRTHits)   ) ntrt += temp;
        if(i==0) {
          if(ncl>0) isSi1 = true;
          if(ncl==0 && ntrt>0) isTrt1 = true;
        }
        if(i==1) {
          if(ncl>0) isSi2 = true;
          if(ncl==0 && ntrt>0) isTrt2 = true;
        }

        //Decide on the type of track combination in vertex
        if(numTracksPerVertex==2){
          if      (isSi1 && isSi2)  m_SiSi_Conversions++;
          else if(isTrt1 && isTrt2) m_TrtTrt_Conversions++;
          else                      m_SiTrt_Conversions++;
        }
        if(numTracksPerVertex==1){
          if(isSi1)  m_Si_Conversions++;
          if(isTrt1) m_Trt_Conversions++;
        }
      }
    }
  }
}

