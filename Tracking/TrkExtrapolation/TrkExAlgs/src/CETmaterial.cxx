/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CETmaterial.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include <iostream>
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


// Framework
#include "StoreGate/StoreGateSvc.h"

//================ Constructor =================================================

Trk::CETmaterial::CETmaterial(const std::string& name, ISvcLocator* pSvcLocator)
  :
  AthAlgorithm(name,pSvcLocator),
  m_log(msgSvc(),name),
  m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
  m_extraprec("Trk::Extrapolator/MuonExtrapolator"),
  m_gaussDist(0),
  m_flatDist(0),
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
  m_next(0),
  m_err(0),
  m_outerBoundary(0),
  m_trackingGeometry(0),
  m_msentry(0),
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

  // retrieve outer boundary cylinder surface
  if (!m_outerBoundary) {
    m_trackingGeometry = m_extrapolator->trackingGeometry();
    m_outerBoundary = &(m_trackingGeometry->highestTrackingVolume()->boundarySurfaces()[2].getPtr()->surfaceRepresentation());
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
    //double phi = m_minPhi + (m_maxPhi-m_minPhi)/m_numScan *it;
    //phi += M_PI/180.; if (phi>M_PI) phi -=2*M_PI;
    //double theta = m_minTheta +  m_flatDist->shoot()*(m_maxTheta-m_minTheta);
    double theta = m_minTheta + (m_maxTheta-m_minTheta)/m_numScan*it;
    double p = m_minP + (m_maxP-m_minP)/m_numScan *it;
    Trk::PerigeeSurface surface( Amg::Vector3D(0.,0.,0.));
    Trk::Perigee initialPerigee(0., z0, phi, theta, m_charge/p, surface);
    // cosmics!
    //Trk::GlobalPosition pos(0.,0.,z0);
    //Trk::GlobalDirection mom(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta));
    //Trk::Perigee initialPerigee( pos,mom,m_charge/p,pos);
    //std::cout << "theta,phi,pos:" << theta << "," << phi << "," << initialPerigee.position() << std::endl;
    const Trk::TrackParameters* seed = initialPerigee.clone();
    const Trk::PerigeeSurface& pSf = initialPerigee.associatedSurface();
    //const Trk::MeasuredTrackParameters* measIn = dynamic_cast<const Trk::MeasuredTrackParameters*> (seed);
    //if (measIn) std::cout << "input covariance:" << measIn->localErrorMatrix().covariance() << std::endl;
    material.clear();  matPrec.clear();
    const Trk::TrackParameters* currPar = seed;
    const Trk::TrackParameters* precPar = seed;
    //const Trk::Layer* layPrec = 0;
    if (m_domsentry) {
      if (!m_msentry) {
        //m_msentry = m_trackingGeometry->trackingVolume("MuonSpectrometerEntrance");
        m_msentry = m_trackingGeometry->trackingVolume("Calo::Containers::Calorimeter");
        //std::cout << "msentry volume?" << m_msentry << std::endl;
      }
      if (m_msentry) {
        const Trk::TrackParameters* msEntry = m_extrapolator->extrapolateToVolume(*currPar,*m_msentry,Trk::alongMomentum,
                                                                                  (Trk::ParticleHypothesis)m_particleType);
        if (msEntry) {
          //std::cout << theta << "," << phi << ": MS entrance reached at:" << msEntry->position() << "," << msEntry->momentum().mag() << std::endl;
           printMat(theta,phi,
                  currPar->momentum().mag()-msEntry->momentum().mag(),
                  Amg::error(msEntry->covariance()->inverse().eval(),Trk::theta),
                  Amg::error(msEntry->covariance()->inverse().eval(),Trk::phi));
          //if (meas) std::cout << "errors:" << meas->localErrorMatrix().error(Trk::theta) << "," << meas->localErrorMatrix().error(Trk::phi) << std::endl;

          const std::vector<const Trk::TrackStateOnSurface*>* mmsentry = m_extrapolator->extrapolateM(*currPar,
                                                                                                    msEntry->associatedSurface(),
                                                                                                    Trk::alongMomentum,
                                                                                                    false,
                                                                                                    (Trk::ParticleHypothesis)m_particleType);
          if (mmsentry ) {
            //std::cout << "material scan:forward:" << std::endl;
            //if (mmsentry) std::cout << "trPar vector size:" << mmsentry->size() << std::endl;
            for (unsigned int i=0; i< mmsentry->size(); i++)
                if ((*mmsentry)[i])
                    std::cout << "position:eloss:" << i << "," << (*mmsentry)[i]->trackParameters()->position() << ":"
                        << (*mmsentry)[i]->trackParameters()->momentum().mag()-currPar->momentum().mag() << std::endl;
            currPar = (mmsentry->back()) ?  mmsentry->back()->trackParameters() : msEntry;

            const std::vector<const Trk::TrackStateOnSurface*>* peri = m_extrapolator->extrapolateM(*currPar,
                                                                                                    pSf,
                                                                                                    Trk::oppositeMomentum,
                                                                                                    false,
                                                                                                    (Trk::ParticleHypothesis)m_particleType);
            std::cout << "material scan:backward:" << std::endl;
            if (peri) std::cout << "trPar vector size:" << peri->size() << std::endl;
            for (unsigned int i=0; i< peri->size(); i++)
                if ((*peri)[i] && (*peri)[i]->trackParameters())
                    std::cout << "position:eloss:" << i << "," << (*peri)[i]->trackParameters()->position() << ":"
                        << (*peri)[i]->trackParameters()->momentum().mag()-msEntry->momentum().mag() << std::endl;

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
        std::pair<const Trk::TrackParameters*,const Trk::Layer*> next = m_extrapolator->extrapolateToNextActiveLayerM(*currPar,
                                                Trk::alongMomentum,
                                                true,
                                                material,
                                                (Trk::ParticleHypothesis)m_particleType);

        //if (next.first) std::cout << "precise extrapolation to:" << next.first->position() << "," << next.second << std::endl;

        const Trk::TrackParameters* nextPar = next.first;
        const Trk::Layer* lay = next.second;
        currPar = nextPar;

        if (m_doprecision && precPar && currPar ) {
          // try to extrapolate to the same surface
          const std::vector<const Trk::TrackStateOnSurface*>*  nextPrec = m_extraprec->extrapolateM(*precPar,currPar->associatedSurface(),
                                                    Trk::alongMomentum,
                                                    false,
                                                    (Trk::ParticleHypothesis)m_particleType);
          //if (nextPrec && nextPrec->size() && nextPrec->back()->trackParameters())
          //  std::cout << "approx. extrapolation to:" << nextPrec->back()->trackParameters()->position() << std::endl;
          delete precPar;
          // collect material
          if (nextPrec) {
            for (unsigned int i=0; i< nextPrec->size(); i++) {
              const Trk::MaterialEffectsBase* mEff = (*nextPrec)[i]->materialEffectsOnTrack();
              const Trk::TrackParameters* trPar = (*nextPrec)[i]->trackParameters();
              if (mEff && trPar) {
                matApp += mEff->thicknessInX0();
                //std::cout << "mat approx:" << mEff->associatedSurface().center() << "," << mEff->thicknessInX0() << std::endl;
              }
            }
          }
          // stop of extrapolation failed
          if (!lay || !nextPrec || !nextPrec->size() || !nextPrec->back() ) break;
          precPar = nextPrec->back()->trackParameters();
          double mat=0.;
          if (material.size()) for (unsigned int i=0; i< material.size(); i++) {
            if (material[i]->materialEffectsOnTrack()) mat += material[i]->materialEffectsOnTrack()->thicknessInX0();
          }
          if ( precPar ) printMatComp(theta,phi,currPar,lay->enclosingDetachedTrackingVolume()->name(),mat,matApp,currPar->parameters()[0]-precPar->parameters()[0],
                      currPar->parameters()[1]-precPar->parameters()[1]);
          else if (currPar) std::cout << "expected layer not reached:" << currPar->position() << "," << precPar->position() << std::endl;
        }
        if (nextPar && m_printActive) {
          //std::vector<const Trk::DetachedTrackingVolume*>* detVols = m_extrapolator->trackingGeometry()->lowestDetachedTrackingVolumes(nextPar->position());
          int id = 0;
          if (lay) id = lay->layerType();
          double matc=0.;
          if (material.size()) for (unsigned int i=0; i< material.size(); i++) {
            if (material[i]->materialEffectsOnTrack()) matc += material[i]->materialEffectsOnTrack()->thicknessInX0();
          }
          //else ATH_MSG_INFO( "par && cov matrix:" << nextPar->parameters()[Trk::locY] << "," << mdest->localErrorMatrix().error(Trk::locY) );
          else ATH_MSG_INFO( "mat & error:" << theta << "," << phi << "," << matc << ","
                  << Amg::error(nextPar->covariance()->inverse().eval(),Trk::theta) << ","
                  << Amg::error(nextPar->covariance()->inverse().eval(),Trk::phi) );

          printMatPrec(theta,phi,nextPar,nextPar,matc,id,"unknown");
          //if (material.back()->materialEffectsOnTrack())
          //  printMatPrec(theta,phi,nextPar,mdest,material.back()->materialEffectsOnTrack()->thicknessInX0(),id,(*detVols)[0]->name());
        }
        if (!lay) break;
      }
      if (m_printMaterial) {
        double mat=0.;
        if (material.size()) for (unsigned int i=0; i< material.size(); i++) {
          if (material[i]->materialEffectsOnTrack()) {
            //std::cout << "material surface prec:" << material[i]->materialEffectsOnTrack()->associatedSurface().center() << ","
            //    << material[i]->materialEffectsOnTrack()->thicknessInX0() << std::endl;
            mat += material[i]->materialEffectsOnTrack()->thicknessInX0();
          }
        }
        printMat(theta,phi,mat);
        //std::cout << "material difference:prec,approx:" << mat << "," << matApp << std::endl;
      }
    } else {
      const std::vector<const Trk::TrackStateOnSurface*>* destParameters = m_extrapolator->extrapolateM(*currPar,
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
            if (detVols && detVols->size()) printMatScan(theta,phi,trPar->position().perp(),trPar->position().z(),mEff->thicknessInX0(),(*detVols)[0]->name());
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

      if (!destParameters || !destParameters->size() ) {
        ATH_MSG_ERROR( "extrapolation to outer boundary failed for input parameters: " << initialPerigee.parameters() );
      } else if (destParameters->back()->trackParameters()) {
        // forward extrapolation ok
        //ATH_MSG_INFO( "last material reached at: " << destParameters->back()->trackParameters()->position().perp() <<","<<destParameters->back()->position().z() );

        if (m_backward) {
          material.clear();
          const std::vector<const Trk::TrackStateOnSurface*>* peri = m_extrapolator->extrapolateM(*(destParameters->back()->trackParameters()),
                                                                                              pSf,
                                                                                              Trk::oppositeMomentum,
                                                                                              false,
                                                                                              (Trk::ParticleHypothesis)m_particleType);
          //std::cout << "material scan:backward:"<<it <<","<<destParameters->size() << std::endl;

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
  return;
}


void Trk::CETmaterial::printMatScan(double theta, double phi, double r, double z, double mat, std::string name) const {

  std::ofstream myfilemat;
  myfilemat.open(m_matScanFile,std::ios::app);
  myfilemat << theta << " " << phi << " " << r << " " << z << " " << mat << " " << name << std::endl;
  return;
}

void Trk::CETmaterial::printMatPrec(double theta, double phi, const Trk::TrackParameters* nextPar, const Trk::TrackParameters* mdest, double mat, int id, std::string name) const {

  if (name=="") {}; // dummy to get rid of warning message (unused variable name)
  std::ofstream myfilemat;
  //std::cout << "opening output file" << std::endl;
  myfilemat.open(m_matActiveFile,std::ios::app);
  //std::cout << "opened:" << nextPar << "," << mdest << std::endl;

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
  return;
}

void Trk::CETmaterial::printMatComp(double theta, double phi, const Trk::TrackParameters* currPar, std::string name, double mat, double matApp,double dx, double dy) const
{
  std::ofstream myfilemat;
  myfilemat.open(m_matCompFile,std::ios::app);
  myfilemat << theta << " " << phi << " " << currPar->position().perp() << " " << currPar->position().z() << " " << name.substr(0,2)
           << " " << mat << " " << matApp << " " << dx << " " << dy << std::endl;

  return;
}
