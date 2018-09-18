/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsGeometry/ActsAlignedExtrapAlg.h"

// ATHENA
#include "StoreGate/ReadCondHandleKey.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "AthenaKernel/RNGWrapper.h"
#include "GaudiKernel/ServiceHandle.h"
#include "ActsInterop/Logger.h"

// PACKAGE
#include "ActsGeometry/IExtrapolationTool.h"
#include "ActsGeometry/IExCellWriterSvc.h"

// ACTS
#include "Acts/Utilities/Logger.hpp"
#include "Acts/Surfaces/PerigeeSurface.hpp"

// STL
#include <thread>
#include <chrono>
#include <memory>

ActsAlignedExtrapAlg::ActsAlignedExtrapAlg( const std::string& name, 
            ISvcLocator* pSvcLocator ) 
  : AthAlgorithm( name, pSvcLocator ),
    m_exCellWriterSvc("ExCellWriterSvc", name),
    m_rndmGenSvc("AthRNGSvc", name)
{}


ActsAlignedExtrapAlg::~ActsAlignedExtrapAlg() {}


StatusCode ActsAlignedExtrapAlg::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  ATH_CHECK(m_rch.initialize());

  //ATH_MSG_INFO( "m_rch id:  " << m_rch.fullKey() );
  
  ATH_CHECK( m_exCellWriterSvc.retrieve() );
  ATH_CHECK( m_rndmGenSvc.retrieve() );
  
  ATH_CHECK( m_extrapolationTool.retrieve() );
  
  //ParticleGun::Config pgCfg;
  //pgCfg.nParticles = 1;
  //pgCfg.pID = 11;
  //pgCfg.mass = 0.51099891 * Acts::units::_MeV;
  //pgCfg.charge = -1.;
  //pgCfg.etaRange = {1.5, 4};

  //m_particleGun = std::make_unique<ParticleGun>(
      //pgCfg, Acts::makeAthenaLogger(this, "ParticleGun"));
  
  //m_trackingGeometry = m_trackingGeometrySvc->trackingGeometry();

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode ActsAlignedExtrapAlg::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------

StatusCode ActsAlignedExtrapAlg::execute() {
  ATH_MSG_DEBUG("execute " << name());

  // tmp
  //m_trackingGeometrySvc->trackingGeometry();

  //auto ctx = Gaudi::Hive::currentContext();

  //auto eventID = getContext().eventID();
  //ATH_MSG_INFO("LB = " << eventID.lumi_block());
  
  //ATHRNG::RNGWrapper* rngWrapper = m_rndmGenSvc->getEngine(this);
  //rngWrapper->setSeed( name(), ctx );
  
  //std::vector<Acts::ProcessVertex> vertices = m_particleGun->generate(*rngWrapper);
  
  //std::vector<Acts::ExtrapolationCell<Acts::TrackParameters>> ecells;

  //for(size_t n=0;n<vertices.size();n++) {
    
    //const Acts::ProcessVertex &pv = vertices.at(n);

    //Acts::PerigeeSurface surface(pv.position());

    //for(size_t pi=0;pi<pv.outgoingParticles().size();++pi) {
      //const auto &particle = pv.outgoingParticles().at(pi);
      //// prepare this particle for extrapolation
      //double d0    = 0.;
      //double z0    = 0.;
      //double phi   = particle.momentum().phi();
      //double theta = particle.momentum().theta();
      //// treat differently for neutral particles
      //double qop = particle.charge() != 0
          //? particle.charge() / particle.momentum().mag()
          //: 1. / particle.momentum().mag();
      //// parameters
      //Acts::ActsVectorD<5> pars;
      //pars << d0, z0, phi, theta, qop;
      //std::unique_ptr<Acts::ActsSymMatrixD<5>> cov = nullptr;

      //if (particle.charge()) {
        //// charged extrapolation - with hit recording
        //Acts::BoundParameters startParameters(
            //std::move(cov), std::move(pars), surface);
        //Acts::ExtrapolationCell<Acts::TrackParameters> ecc(startParameters);

        //ecc.searchMode = 1;

        //ecc.addConfigurationMode(Acts::ExtrapolationMode::StopAtBoundary);

        //ecc.addConfigurationMode(Acts::ExtrapolationMode::FATRAS);

        //ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectSensitive);

        //ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectPassive);

        //ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectBoundary);

        //ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectMaterial);

        //auto ctx = Gaudi::Hive::currentContext();
        //std::cout << __FUNCTION__ << ": eventID = " << ctx.eventID() << std::endl;
        //m_extrapolationTool->extrapolate(ecc);

        ////if (m_writeMaterialTracks) {
          ////Acts::MaterialTrack mTrack = makeMaterialTrack(ecc);
          ////m_materialTrackWriterSvc->write(std::move(mTrack));
        ////}

        //ecells.push_back(std::move(ecc));
      //}
    //}
  //}
  
  //m_exCellWriterSvc->store(ecells);

  //EventIDBase t( getContext().eventID() );
  
  //SG::ReadCondHandle<ShiftCondObj> rch( m_rch );
  //const ShiftCondObj *shift = *rch;
  //if (shift != 0) {
    //ATH_MSG_INFO("  read CH: " << rch.key() << " = " << *shift );
  //} else {
    //ATH_MSG_ERROR("  CDO ptr for " << rch.key() << " == zero");
  //}
  
  return StatusCode::SUCCESS;
}

