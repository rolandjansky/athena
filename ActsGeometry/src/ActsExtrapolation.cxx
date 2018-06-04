// @todo: re-check this
#define ACTS_CORE_IDENTIFIER_PLUGIN "Identifier/Identifier.h"

#include "ActsGeometry/ActsExtrapolation.h"

// ATHENA
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "AthenaKernel/RNGWrapper.h"
#include "Acts/Utilities/Logger.hpp"
#include "Acts/Surfaces/PerigeeSurface.hpp"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ISvcLocator.h"

// ACTS
#include "Acts/Plugins/MaterialPlugins/MaterialTrack.hpp"

// PACKAGE
#include "ActsGeometry/Extrapolation/ParticleGun.hpp"
#include "ActsGeometry/ITrackingGeometrySvc.h"
#include "ActsGeometry/IExtrapolationTool.h"
#include "ActsGeometry/IExCellWriterSvc.h"
#include "ActsInterop/Logger.h"
#include "ActsGeometry/IMaterialTrackWriterSvc.h"
//#include "ActsGeometry/Extrapolation/ObjExCellWriter.hpp"

// STL
#include <string>

namespace {
const Amg::Vector3D origin(0., 0., 0.);
}


ActsExtrapolation::ActsExtrapolation(const std::string& name,
                                 ISvcLocator* pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator),
      m_firstEvent(true),
      m_trackingGeometrySvc("TrackingGeometrySvc", name),
      m_exCellWriterSvc("ExCellWriterSvc", name),
      m_rndmGenSvc("AthRNGSvc", name),
      m_materialTrackWriterSvc("MaterialTrackWriterSvc", name)
{
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode ActsExtrapolation::initialize() {

  ATH_MSG_INFO("ACTS Demo Algorithm is initializing");

  ATH_CHECK( m_trackingGeometrySvc.retrieve() );
  ATH_CHECK( m_exCellWriterSvc.retrieve() );
  ATH_CHECK( m_rndmGenSvc.retrieve() );
  
  ATH_CHECK( m_extrapolationTool.retrieve() );

  m_trackingGeometry = m_trackingGeometrySvc->trackingGeometry();
        
  if (m_writeMaterialTracks) {
    ATH_CHECK( m_materialTrackWriterSvc.retrieve() );
  }

  ParticleGun::Config pgCfg;
  pgCfg.nParticles = m_nParticles;
  pgCfg.pID = 11;
  pgCfg.mass = 0.51099891 * Acts::units::_MeV;
  pgCfg.charge = -1.;
  pgCfg.etaRange = {-6, 6};

  m_particleGun = std::make_unique<ParticleGun>(
      pgCfg, ACTS_ATH_LOGGER("ParticleGun"));


  


  //auto ofs = std::make_shared<std::ofstream>();
  //ofs->open("extrapolation_charged.obj");
  //ObjExCellWriter<Acts::TrackParameters>::Config objeccWriterConfig;
  //objeccWriterConfig.outputStream = ofs;
  //m_objEccWriter
      //= std::make_shared<ObjExCellWriter<Acts::TrackParameters>>(
          //objeccWriterConfig);


  return StatusCode::SUCCESS;
}




StatusCode ActsExtrapolation::execute_r(const EventContext& ctx) const 
{

  ATHRNG::RNGWrapper* rngWrapper = m_rndmGenSvc->getEngine(this);
  rngWrapper->setSeed( name(), ctx );

  std::vector<Acts::ProcessVertex> vertices = m_particleGun->generate(*rngWrapper);

  std::vector<Acts::ExtrapolationCell<Acts::TrackParameters>> ecells;


  for(size_t n=0;n<vertices.size();n++) {
    
    const Acts::ProcessVertex &pv = vertices.at(n);

    Acts::PerigeeSurface surface(pv.position());

    for(size_t pi=0;pi<pv.outgoingParticles().size();++pi) {
      const auto &particle = pv.outgoingParticles().at(pi);
      // prepare this particle for extrapolation
      double d0    = 0.;
      double z0    = 0.;
      double phi   = particle.momentum().phi();
      double theta = particle.momentum().theta();
      // treat differently for neutral particles
      double qop = particle.charge() != 0
          ? particle.charge() / particle.momentum().mag()
          : 1. / particle.momentum().mag();
      // parameters
      Acts::ActsVectorD<5> pars;
      pars << d0, z0, phi, theta, qop;
      std::unique_ptr<Acts::ActsSymMatrixD<5>> cov = nullptr;

      if (particle.charge()) {
        // charged extrapolation - with hit recording
        Acts::BoundParameters startParameters(
            std::move(cov), std::move(pars), surface);
        Acts::ExtrapolationCell<Acts::TrackParameters> ecc(startParameters);

        ecc.searchMode = m_searchMode;

        if (m_stopAtBoundary) {
          ATH_MSG_VERBOSE("StopAtBoundary set");
          ecc.addConfigurationMode(Acts::ExtrapolationMode::StopAtBoundary);
        }

        if (m_FATRAS) {
          ATH_MSG_VERBOSE("FATRAS set");
          ecc.addConfigurationMode(Acts::ExtrapolationMode::FATRAS);
        }

        if (m_collectSensitive) {
          ATH_MSG_VERBOSE("CollectSensitive set");
          ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectSensitive);
        }

        if (m_collectPassive) {
          ATH_MSG_VERBOSE("CollectPassive set");
          ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectPassive);
        }

        if (m_collectBoundary) {
          ATH_MSG_VERBOSE("CollectBoundary set");
          ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectBoundary);
        }

        if (m_collectMaterial) {
          ATH_MSG_VERBOSE("CollectMaterial set");
          ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectMaterial);
        }

        m_extrapolationTool->extrapolate(ecc);

        if (m_writeMaterialTracks) {
          Acts::MaterialTrack mTrack = makeMaterialTrack(ecc);
          m_materialTrackWriterSvc->write(std::move(mTrack));
        }

        ecells.push_back(std::move(ecc));
      }
    }
  }

  m_exCellWriterSvc->store(ecells);


  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode ActsExtrapolation::finalize() {
  //ofs->close();
  return StatusCode::SUCCESS;
}

