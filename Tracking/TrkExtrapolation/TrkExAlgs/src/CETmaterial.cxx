/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CETmaterial.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include <fstream>

// Tracking
#include "TrkExAlgs/CETmaterial.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

//================ Constructor =================================================

Trk::CETmaterial::CETmaterial(const std::string& name, ISvcLocator* pSvcLocator)
  :
  AthAlgorithm(name,pSvcLocator),
  m_log(msgSvc(),name),
  m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
  m_extraprec("Trk::Extrapolator/MuonExtrapolator"),
  m_gaussDist(nullptr),
  m_flatDist(nullptr),
  m_sigmaD0(17.*Gaudi::Units::micrometer),
  m_minZ0(0.),
  m_maxZ0(0.),
  m_minEta(-3.),
  m_maxEta(3.),
  m_minTheta(0.),
  m_maxTheta(M_PI),
  m_minPhi(-M_PI),
  m_maxPhi( M_PI),
  m_minP(50000*Gaudi::Units::GeV),
  m_maxP(50000*Gaudi::Units::GeV),
  m_charge(1.),
  m_numScan(10),
  m_checkStepWise(false),
  m_printMaterial(false),
  m_printActive(false),
  m_matTotFile("material.txt"),
  m_matScanFile("material_scan.txt"),
  m_matActiveFile("mat_active.txt"),
  m_matCompFile("material_comp.txt"),
  m_backward(false),
  m_domsentry(false),
  m_doprecision(false),
  m_th(0.),
  m_ph(0.),
  m_id(0),
  m_matSaved(0.),
  m_next(nullptr),
  m_err(nullptr),
  m_outerBoundary(nullptr),
  m_trackingGeometry(nullptr),
  m_msentry(nullptr),
  m_particleType(Trk::muon),
  m_chronoStatSvc( "ChronoStatSvc", name )
{
  // used algorithms and alg tools
  declareProperty("Extrapolator"              , m_extrapolator);
  declareProperty("ExtraExtrapolator"         , m_extraprec);

  // algorithm steering
  declareProperty("StartPerigeeSigmaD0"       , m_sigmaD0);
  declareProperty("StartPerigeeMinZ0"         , m_minZ0);
  declareProperty("StartPerigeeMaxZ0"         , m_maxZ0);
  declareProperty("StartPerigeeMinEta"        , m_minEta);
  declareProperty("StartPerigeeMaxEta"        , m_maxEta);
  declareProperty("StartPerigeeMinTheta"      , m_minTheta);
  declareProperty("StartPerigeeMaxTheta"      , m_maxTheta);
  declareProperty("StartPerigeeMinPhi"        , m_minPhi);
  declareProperty("StartPerigeeMaxPhi"        , m_maxPhi);
  declareProperty("StartPerigeeMinP"          , m_minP);
  declareProperty("StartPerigeeMaxP"          , m_maxP);
  declareProperty("StartPerigeeCharge"        , m_charge);
  declareProperty("ParticleType"              , m_particleType);
  declareProperty("NumberOfScanTracks"        , m_numScan);
  declareProperty("CheckActiveLayers"         , m_checkStepWise);
  declareProperty("PrintMaterial"             , m_printMaterial);
  declareProperty("PrintActivePos"            , m_printActive);
  //declareProperty("TotalMaterialOutput"       , m_matTotFile);
  //declareProperty("ScanMaterialOutput"        , m_matScanFile);
  declareProperty("CheckBackward"             , m_backward);
  declareProperty("CheckMSentry"              , m_domsentry);
  declareProperty("CheckPrecision"            , m_doprecision);

}

//================ Destructor =================================================

Trk::CETmaterial::~CETmaterial()
{
  delete m_gaussDist;
  delete m_flatDist;
  delete m_err;
}


//================ Initialization =================================================

StatusCode Trk::CETmaterial::initialize()
{
  // Code entered here will be executed once at program start.

//  m_log.setLevel(outputLevel());
  ATH_MSG_INFO( "initialize()" );

  // Get Extrapolator from ToolService
  if (m_extrapolator.retrieve().isFailure()) {
      ATH_MSG_FATAL( "Could not retrieve Tool " << m_extrapolator << ". Exiting." );
      return StatusCode::FAILURE;
  }
  if (m_extraprec.retrieve().isFailure()) {
      ATH_MSG_FATAL( "Could not retrieve Tool " << m_extraprec << ". Exiting." );
      return StatusCode::FAILURE;
  }

  //if (m_chronoStatSvc.retrieve().isFailure()) {
  //    ATH_MSG_WARNING << "Could not retrieve Tool " << m_chronoStatSvc << ". Exiting." );
  //}

  m_gaussDist = new Rndm::Numbers(randSvc(), Rndm::Gauss(0.,1.));
  m_flatDist  = new Rndm::Numbers(randSvc(), Rndm::Flat(0.,1.));

  ATH_MSG_INFO( "initialize() successful" );
  return StatusCode::SUCCESS;
}

//================ Finalization =================================================

StatusCode Trk::CETmaterial::finalize()
{
  // Code entered here will be executed once at the end of the program run.

  if (m_chronoStatSvc) m_chronoStatSvc->chronoPrint("MS::scan");

  return StatusCode::SUCCESS;
}

//================ Execution ====================================================

StatusCode Trk::CETmaterial::execute()
{
  ATH_MSG_INFO( "execute()" );
  const EventContext& ctx = Gaudi::Hive::currentContext();
  // retrieve outer boundary cylinder surface
  if (!m_outerBoundary) {
    m_trackingGeometry = m_extrapolator->trackingGeometry();
    m_outerBoundary = &(m_trackingGeometry->highestTrackingVolume()->boundarySurfaces()[2].get()->surfaceRepresentation());
    if (!m_outerBoundary) {
      ATH_MSG_FATAL( "Could not retrieve cylinder boundary  from " << m_extrapolator << ". Exiting." );
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "boundary retrieved " );
  }

  // generate with random number generator
  //double d0 = m_gaussDist->shoot() * m_sigmaD0;
  //double z0 = m_minZ0 + m_flatDist->shoot() * (m_maxZ0-m_minZ0);
  //double phi = 2.*M_PI * m_flatDist->shoot() - M_PI;
  //double eta = m_minEta + m_flatDist->shoot()*(m_maxEta-m_minEta);
  //double ctheta = -1. + 2* m_flatDist->shoot();
  //double theta = acos(ctheta);
  //double p = m_minP + m_flatDist->shoot()*(m_maxP-m_minP);
  //double charge = (m_flatDist->shoot() > 0.5 ) ? -1. : 1.;
  //double qOverP = charge/(p);

  if (m_chronoStatSvc) m_chronoStatSvc->chronoStart("MS::scan");

  // scan
  std::vector<const TrackStateOnSurface*> material;
  std::vector<const TrackStateOnSurface*> matPrec;
  double phi = -M_PI;
  for ( unsigned int it = 0; it < m_numScan+1; it++) {
    // the initial perigee
    double z0 = m_minZ0 + (m_maxZ0-m_minZ0)/m_numScan *it;
    phi += 1*Gaudi::Units::deg; if (phi>M_PI) phi -=2*M_PI;

    double theta = m_minTheta + (m_maxTheta-m_minTheta)/m_numScan*it;
    double p = m_minP + (m_maxP-m_minP)/m_numScan *it;
    Trk::PerigeeSurface surface( Amg::Vector3D(0.,0.,0.));
    Trk::Perigee initialPerigee(0., z0, phi, theta, m_charge/p, surface);

    const Trk::TrackParameters* seed = initialPerigee.clone();
    const Trk::PerigeeSurface& pSf = initialPerigee.associatedSurface();
    material.clear();  matPrec.clear();
    const Trk::TrackParameters* currPar = seed;
    const Trk::TrackParameters* precPar = seed;
    if (m_domsentry) {
      if (!m_msentry) {
        m_msentry = m_trackingGeometry->trackingVolume("Calo::Containers::Calorimeter");
      }
      if (m_msentry) {
        const Trk::TrackParameters* msEntry =
          m_extrapolator->extrapolateToVolume(
            ctx,
            *currPar,
            *m_msentry,
            Trk::alongMomentum,
            (Trk::ParticleHypothesis)m_particleType).release();
        if (msEntry) {
          printMat(
            theta,
            phi,
            currPar->momentum().mag() - msEntry->momentum().mag(),
            Amg::error(msEntry->covariance()->inverse().eval(), Trk::theta),
            Amg::error(msEntry->covariance()->inverse().eval(), Trk::phi));

          const std::vector<const Trk::TrackStateOnSurface*>* mmsentry = m_extrapolator->extrapolateM(ctx,
                                                                                                      *currPar,
                                                                                                      msEntry->associatedSurface(),
                                                                                                      Trk::alongMomentum,
                                                                                                      false,
                                                                                                      (Trk::ParticleHypothesis)m_particleType);
          if (mmsentry ) {
            for (unsigned int i=0; i< mmsentry->size(); i++)
                if ((*mmsentry)[i])
                    ATH_MSG_DEBUG( "position:eloss:" << i << "," << (*mmsentry)[i]->trackParameters()->position() << ":"
                        << (*mmsentry)[i]->trackParameters()->momentum().mag()-currPar->momentum().mag());
            currPar = (mmsentry->back()) ?  mmsentry->back()->trackParameters() : msEntry;

            const std::vector<const Trk::TrackStateOnSurface*>* peri = m_extrapolator->extrapolateM(ctx,
                                                                                                    *currPar,
                                                                                                    pSf,
                                                                                                    Trk::oppositeMomentum,
                                                                                                    false,
                                                                                                    (Trk::ParticleHypothesis)m_particleType);
            ATH_MSG_INFO ( "material scan:backward:" );
            if (peri) ATH_MSG_DEBUG ("trPar vector size:" << peri->size() );
            for (unsigned int i=0; i< peri->size(); i++)
                if ((*peri)[i] && (*peri)[i]->trackParameters())
                    ATH_MSG_DEBUG( "position:eloss:" << i << "," << (*peri)[i]->trackParameters()->position() << ":"
                        << (*peri)[i]->trackParameters()->momentum().mag()-msEntry->momentum().mag() );

            if (peri->back() && peri->back()->trackParameters()) {
              ATH_MSG_INFO( "extrapolation to perigee:input: "
                    << initialPerigee.parameters()[0] << ","
                    << initialPerigee.parameters()[1] << ","
                    << initialPerigee.parameters()[2] << ","
                    << initialPerigee.parameters()[3] << ","
                    << initialPerigee.momentum().mag() );
              ATH_MSG_INFO( "extrapolation to perigee:output: "
                    << peri->back()->trackParameters()->parameters()[0] << ","
                    << peri->back()->trackParameters()->parameters()[1] << ","
                    << peri->back()->trackParameters()->parameters()[2] << ","
                    << peri->back()->trackParameters()->parameters()[3] << ","
                    << peri->back()->trackParameters()->momentum().mag() );
            } else {
              ATH_MSG_ERROR( "extrapolation to perigee failed for input parameters: " << msEntry->parameters() );
            }
            delete peri;
            delete msEntry;
          } else {
            ATH_MSG_ERROR( "extrapolation to MSentry failed for input parameters: " << currPar->parameters() );
            printMat(theta,phi,0.);
          }
        }
      }
      delete currPar;
      continue;
    }
    if (m_checkStepWise) {
      double matApp = 0.;
      while (currPar) {
        std::pair<const Trk::TrackParameters*,const Trk::Layer*> next = m_extrapolator->extrapolateToNextActiveLayerM(
                                                ctx,
                                                *currPar,
                                                Trk::alongMomentum,
                                                true,
                                                material,
                                                (Trk::ParticleHypothesis)m_particleType);


        const Trk::TrackParameters* nextPar = next.first;
        const Trk::Layer* lay = next.second;
        currPar = nextPar;

        if (m_doprecision && precPar && currPar ) {
          // try to extrapolate to the same surface
          const std::vector<const Trk::TrackStateOnSurface*>*  nextPrec = m_extraprec->extrapolateM(
                                                    ctx,
                                                    *precPar,currPar->associatedSurface(),
                                                    Trk::alongMomentum,
                                                    false,
                                                    (Trk::ParticleHypothesis)m_particleType);
          delete precPar;
          // collect material
          if (nextPrec) {
            for (unsigned int i=0; i< nextPrec->size(); i++) {
              const Trk::MaterialEffectsBase* mEff = (*nextPrec)[i]->materialEffectsOnTrack();
              const Trk::TrackParameters* trPar = (*nextPrec)[i]->trackParameters();
              if (mEff && trPar) {
                matApp += mEff->thicknessInX0();
              }
            }
          }
          // stop of extrapolation failed
          if (!lay || !nextPrec || nextPrec->empty() || !nextPrec->back() ) break;
          precPar = nextPrec->back()->trackParameters();
          double mat=0.;
          if (!material.empty()) for (unsigned int i=0; i< material.size(); i++) {
            if (material[i]->materialEffectsOnTrack()) mat += material[i]->materialEffectsOnTrack()->thicknessInX0();
          }
          if ( precPar ) printMatComp(theta,phi,currPar,lay->enclosingDetachedTrackingVolume()->name(),mat,matApp,currPar->parameters()[0]-precPar->parameters()[0],
                      currPar->parameters()[1]-precPar->parameters()[1]);
          else if (currPar) {
            //precPar is nullptr here
            ATH_MSG_INFO( "expected layer not reached:" << currPar->position() );
          }
        }
        if (nextPar && m_printActive) {
          int id = 0;
          if (lay) id = lay->layerType();
          double matc=0.;
          if (!material.empty()) for (unsigned int i=0; i< material.size(); i++) {
            if (material[i]->materialEffectsOnTrack()) matc += material[i]->materialEffectsOnTrack()->thicknessInX0();
          }
          else ATH_MSG_INFO( "mat & error:" << theta << "," << phi << "," << matc << ","
                  << Amg::error(nextPar->covariance()->inverse().eval(),Trk::theta) << ","
                  << Amg::error(nextPar->covariance()->inverse().eval(),Trk::phi) );

          printMatPrec(theta,phi,nextPar,nextPar,matc,id,"unknown");
        }
        if (!lay) break;
      }
      if (m_printMaterial) {
        double mat=0.;
        if (!material.empty()) for (unsigned int i=0; i< material.size(); i++) {
          if (material[i]->materialEffectsOnTrack()) {
            mat += material[i]->materialEffectsOnTrack()->thicknessInX0();
          }
        }
        printMat(theta,phi,mat);
      }
    } else {
      const std::vector<const Trk::TrackStateOnSurface*>* destParameters = m_extrapolator->extrapolateM(
                                                                              ctx,
                                                                              *currPar,
                                                                              *m_outerBoundary,
                                                                              Trk::alongMomentum,
                                                                              false,
                                                                              (Trk::ParticleHypothesis)m_particleType);

      if (m_printMaterial) {
        double mat=0.;
        if (destParameters) for (unsigned int i=0; i< destParameters->size(); i++) {
          const Trk::MaterialEffectsBase* mEff = (*destParameters)[i]->materialEffectsOnTrack();
          const Trk::TrackParameters* trPar = (*destParameters)[i]->trackParameters();
          if (trPar) {
            //const Trk::MeasuredTrackParameters* mdest = dynamic_cast<const Trk::MeasuredTrackParameters*> (trPar);
            //if (mdest) ATH_MSG_INFO( "radiation thickness and errors(theta,phi):" << theta << "," << phi << "," << mat << "," <<
            //             mdest->localErrorMatrix().error(Trk::theta) << "," << mdest->localErrorMatrix().error(Trk::phi) );
          }
          if (mEff && trPar) {
            mat += mEff->thicknessInX0();
            // find volume
            std::vector<const Trk::DetachedTrackingVolume*>* detVols = m_extrapolator->trackingGeometry()->lowestDetachedTrackingVolumes(trPar->position());
            if (detVols && !detVols->empty()) printMatScan(theta,phi,trPar->position().perp(),trPar->position().z(),mEff->thicknessInX0(),(*detVols)[0]->name());
            else printMatScan(theta,phi,trPar->position().perp(),trPar->position().z(),mEff->thicknessInX0(),m_extrapolator->trackingGeometry()->lowestStaticTrackingVolume(trPar->position())->volumeName());
          }
        }
        if (destParameters)  {
          //const Trk::MeasuredTrackParameters* mdest = dynamic_cast<const Trk::MeasuredTrackParameters*> ((*destParameters).back()->trackParameters());
          //if (mdest) ATH_MSG_INFO( "radiation thickness and errors(theta,phi):" << theta << "," << phi << "," << mat << "," <<
          //mdest->localErrorMatrix().error(Trk::theta) << "," << mdest->localErrorMatrix().error(Trk::phi) );
        }
        printMat(theta,phi,mat);
      }

      if (!destParameters || destParameters->empty() ) {
        ATH_MSG_ERROR( "extrapolation to outer boundary failed for input parameters: " << initialPerigee.parameters() );
      } else if (destParameters->back()->trackParameters()) {
        // forward extrapolation ok
        //ATH_MSG_INFO( "last material reached at: " << destParameters->back()->trackParameters()->position().perp() <<","<<destParameters->back()->position().z() );

        if (m_backward) {
          material.clear();
          const std::vector<const Trk::TrackStateOnSurface*>* peri = m_extrapolator->extrapolateM(
            ctx, *(destParameters->back()->trackParameters()),
                                                                                              pSf,
                                                                                              Trk::oppositeMomentum,
                                                                                              false,
                                                                                              (Trk::ParticleHypothesis)m_particleType);

          if (peri) {
              ATH_MSG_INFO( "trPar vector size:" << peri->size() );
            for (unsigned int i=0; i< peri->size(); i++)
              ATH_MSG_INFO( "position:" << i << "," << (*peri)[i]->trackParameters()->position() );
            ATH_MSG_INFO( "extrapolation to perigee:input: " << initialPerigee.parameters() );
            ATH_MSG_INFO( "extrapolation to perigee:output: " << peri->back()->trackParameters()->parameters() );
          } else {
            ATH_MSG_ERROR( "extrapolation to perigee failed for input parameters: " << destParameters->back()->trackParameters()->parameters() );
          }
          delete peri;
        }
      }

      delete destParameters;
    }
  }

  if (m_chronoStatSvc) m_chronoStatSvc->chronoStop("MS::scan");

  return StatusCode::SUCCESS;
}

//============================================================================================

void Trk::CETmaterial::printMat(double theta, double phi, double mat, double dtheta, double dphi) const {

  std::ofstream myfilemat;
  myfilemat.open (m_matTotFile,std::ios::app);
  myfilemat<<theta<<" "<<phi<<" "<<mat<<" "<<dtheta<<" "<<dphi<<std::endl;
}


void Trk::CETmaterial::printMatScan(double theta, double phi, double r, double z, double mat, const std::string& name) const {

  std::ofstream myfilemat;
  myfilemat.open(m_matScanFile,std::ios::app);
  myfilemat << theta << " " << phi << " " << r << " " << z << " " << mat << " " << name << std::endl;
}

void Trk::CETmaterial::printMatPrec(double theta, double phi, const Trk::TrackParameters* nextPar, const Trk::TrackParameters* mdest, double mat, int id, const std::string& name) const {

  if (name.empty()) {}; // dummy to get rid of warning message (unused variable name)
  std::ofstream myfilemat;
  myfilemat.open(m_matActiveFile,std::ios::app);

  if (!m_th && !m_ph) {
    m_th = theta;
    m_ph = phi;
    m_id = id;
    m_matSaved = mat;
    delete m_next; m_next=nextPar->clone();
    delete m_err;
    m_err=nullptr;
    if (mdest) {
        m_err=new Amg::MatrixX;
	*m_err = mdest->covariance()->inverse().eval();
    }
    return;
  }

  if ( theta!=m_th || phi!=m_ph ) {

    if (m_err && m_id>0) {
      myfilemat << m_th << " " << m_ph << " " << 1 << " " << m_id  << " " <<  m_matSaved << std::endl;
      myfilemat << m_next->parameters()[Trk::locX] << " " << m_next->parameters()[Trk::locY] << " " << m_next->parameters()[Trk::phi]
               << " " << m_next->parameters()[Trk::theta] << " " << m_next->parameters()[Trk::qOverP] << std::endl;
      myfilemat << Amg::error(*m_err,Trk::locX) << " " << Amg::error(*m_err,Trk::locY)
               << " " << Amg::error(*m_err,Trk::phi) << " " << Amg::error(*m_err,Trk::theta)
               << " " << Amg::error(*m_err,Trk::qOverP) << std::endl;
    } else {
      myfilemat << m_th << " " << m_ph << " " << 0 << " " << m_id << std::endl;
      myfilemat << m_next->parameters()[Trk::locX] << " " << m_next->parameters()[Trk::locY] << " " << m_next->parameters()[Trk::phi]
               << " " << m_next->parameters()[Trk::theta] << " " << m_next->parameters()[Trk::qOverP] << std::endl;
    }
    m_th = theta;
    m_ph = phi;
    m_id = id;
    m_matSaved = mat;
    delete m_next; m_next=nextPar->clone();
    delete m_err; 
    m_err=nullptr; 
   
    if (mdest) {
        m_err=new Amg::MatrixX;
	*m_err = mdest->covariance()->inverse().eval();
    }
    return;
  }

  // update data
  if (id>1) {
    m_th = theta;
    m_ph = phi;
    m_id = id;
    m_matSaved = mat;
    delete m_next; m_next=nextPar->clone();
    delete m_err;
    m_err=nullptr; 
    if (mdest) {
        m_err=new Amg::MatrixX;
	*m_err = mdest->covariance()->inverse().eval();
    }
  }
  }

void Trk::CETmaterial::printMatComp(double theta, double phi, const Trk::TrackParameters* currPar, const std::string& name, double mat, double matApp,double dx, double dy) const
{
  std::ofstream myfilemat;
  myfilemat.open(m_matCompFile,std::ios::app);
  myfilemat << theta << " " << phi << " " << currPar->position().perp() << " " << currPar->position().z() << " " << name.substr(0,2)
           << " " << mat << " " << matApp << " " << dx << " " << dy << std::endl;
}
