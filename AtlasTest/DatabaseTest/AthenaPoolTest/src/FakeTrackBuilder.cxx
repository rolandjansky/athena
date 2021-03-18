/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "FakeTrackBuilder.h"
#include <iostream>

#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"

// include Tracking EDM
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkParameters/TrackParameters.h"

// include MaterialEffects classes
#include "TrkMaterialOnTrack/MaterialEffectsBase.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkMaterialOnTrack/EstimatedBremOnTrack.h"

//VertexOnTrack
#include "TrkVertexOnTrack/VertexOnTrack.h"

// include InDet EDM
#include "InDetPrepRawData/SiClusterCollection.h"
#include "InDetPrepRawData/SiCluster.h"
#include "Identifier/IdentifierHash.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "InDetCompetingRIOsOnTrack/CompetingPixelClustersOnTrack.h"

#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

#include "GeoPrimitives/GeoPrimitives.h"


Trk::Track* FakeTrackBuilder::buildTrack(const InDetDD::SiDetectorElementCollection* elements) {
  using namespace Trk;
  using namespace InDet;
  //unsigned int counter=0;
  //std::cout<<counter++<<std::endl;
  DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces = new DataVector<const Trk::TrackStateOnSurface>;
  PerigeeSurface periSurf;
  const RIO_OnTrack*              rot            = 0;
  const TrackParameters*          trackParameter = new Perigee(4.0, 3.0, 2.0, 1.0, 0.001, periSurf);
  const FitQuality*               fitQoS         = 0;
  const MaterialEffectsBase*      mefBase        = 0;

  trackStateOnSurfaces->push_back( new TrackStateOnSurface(rot, trackParameter, fitQoS,  mefBase) );

  //std::cout<<counter++<<std::endl;

  // test state #1 - arbitrary TP AtaCylinder
  // Amg::Transform3D t = Amg::Transform3D::Identity(); t.setIdentity();
  Trk::CylinderSurface surface(0, 1.0, 2*M_PI, 4.0);
  trackParameter = surface.createUniqueParameters<5,Trk::Charged>(0.0,1.0,3.0,4.0,0.5,0).release();
  trackStateOnSurfaces->push_back( new TrackStateOnSurface(0, trackParameter, 0,  0) );
  //std::cout<<counter++<<std::endl;

  // test state #2 - arbitrary TP AtaDisc
  Amg::Translation3D amgtranslation(1.,2.,3.);
  Amg::Transform3D* amgTransf = new Amg::Transform3D(amgtranslation);
  DiscSurface discSf(amgTransf, 1.0, 2.0);
  trackParameter = discSf.createUniqueParameters<5,Trk::Charged>(0.0,1.0,3.0,4.0,0.5,0).release();
  trackStateOnSurfaces->push_back( new TrackStateOnSurface(0, trackParameter, 0,  0) );
  //std::cout<<counter++<<std::endl;

  // test state #3 - arbitrary AtaPlane + Estimated Brem
  PlaneSurface planeSf(amgTransf, 1.0, 2.0);
  trackParameter = planeSf.createUniqueParameters<5,Trk::Charged>(0.0,1.0,3.0,4.0,0.5,0).release();
  const MaterialEffectsBase *ebr
    = new EstimatedBremOnTrack((0.7), -0.3,
  0.3, 0.03,
  planeSf);
  std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> type1(0);
  type1.set(Trk::TrackStateOnSurface::BremPoint);
  trackStateOnSurfaces->push_back( new TrackStateOnSurface(0, trackParameter, 0,  ebr,type1) );
  //std::cout<<counter++<<std::endl;

  if (elements){
    // test state #4 - AtaPlane at a valid detector element + MatEffects
    IdentifierHash idHash(0); // should be first pixel
    const InDetDD::SiDetectorElement * detEl = elements->getDetectorElement( idHash);

    // MaterialEffects
    const Trk::EnergyLoss       *eloss = new Trk::EnergyLoss((0.5),0.19);
    const Trk::ScatteringAngles *scatt = new Trk::ScatteringAngles(.1,.2,.3,.4);
    std::bitset<Trk::MaterialEffectsBase::NumberOfMaterialEffectsTypes> mefPattern(0);
    mefPattern.set(Trk::MaterialEffectsBase::EnergyLossEffects);
    mefPattern.set(Trk::MaterialEffectsBase::ScatteringEffects);
    mefPattern.set(Trk::MaterialEffectsBase::FittedMaterialEffects);
    //std::cout<<counter++<<std::endl;

    //AtaPlane on det el
    const PlaneSurface& planeDetElSf = dynamic_cast<const PlaneSurface&>(detEl->surface());
    trackParameter = planeDetElSf.createUniqueParameters<5,Trk::Charged>(0.0,1.0,3.0,4.0,0.5,0).release();

    mefBase = new Trk::MaterialEffectsOnTrack(70.7,scatt,eloss,planeDetElSf, mefPattern);
    std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern(0);
    typePattern.set(Trk::TrackStateOnSurface::Scatterer);
    trackStateOnSurfaces->push_back( new TrackStateOnSurface(0, trackParameter, 0, mefBase,  typePattern) );
  }
  //Curvi
  Amg::Vector3D pos(1.0,2.0,3.0);
  Amg::Vector3D mom(4.0,5.0,6.0);

  trackParameter = new CurvilinearParameters(pos,mom,1.0);
  std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern(0);
  typePattern.set(Trk::TrackStateOnSurface::Parameter);
  trackStateOnSurfaces->push_back( new TrackStateOnSurface(0, trackParameter, 0, 0,  typePattern) );
  //std::cout<<counter++<<std::endl;

  //VertexOnTrack
  // Amg::Vector2D locpos( 1.0, 2.0);
  // Trk::LocalParameters* vxOTLocParms = new Trk::LocalParameters(locpos);
  //
  AmgSymMatrix(2)  locCov;
  locCov.setIdentity();           // sets it to identity
  locCov(0,0) = 0.001; locCov(1,1)=0.002;
  //
  // Trk::VertexOnTrack* vOT = new VertexOnTrack(vxOTLocParms,locCov, PerigeeSurface());
  // typePattern.reset();
  // typePattern.set(Trk::TrackStateOnSurface::Parameter);
  // trackStateOnSurfaces->push_back( new TrackStateOnSurface(vOT, 0, 0, 0,  typePattern) );
  // std::cout<<counter++<<std::endl;

  if (elements) {
    const InDetDD::SiDetectorElement * detEl = *(elements->begin());
    if (!detEl)
      std::cerr<<"Unable to find any pixel Detector element!! Aborting this part of the test."<<std::endl;
    else {
      InDet::PixelClusterOnTrack* fakePix1
        = new InDet::PixelClusterOnTrack(); // make PRD-less ROT
      fakePix1->m_detEl = detEl;
      fakePix1->m_identifier = detEl->identify();
      Amg::Vector2D locpos( 5.559011, 22.613205);
      //2           Trk::LocalPosition locpos = Trk::LocalPosition( 7.315356, 21.303871);
      fakePix1->m_localParams = Trk::LocalParameters(locpos);
      fakePix1->m_localCovariance = locCov;
      unsigned int pix2 = 2832020859UL; // CLHEP::bar l1 mod(6,0) index(309,122)
      Identifier idPix2(pix2);
      InDet::PixelClusterOnTrack* fakePix2
        = new InDet::PixelClusterOnTrack();
      fakePix2->m_detEl = detEl;
      fakePix2->m_identifier = idPix2;
      locpos = Amg::Vector2D( 7.315356, 21.303871);
      fakePix2->m_localParams = Trk::LocalParameters(locpos);
      locCov(0,0) = 0.001; locCov(1,1)=0.0135;
      fakePix2->m_localCovariance = locCov;
      std::vector<const InDet::PixelClusterOnTrack*>* childrots
        = new std::vector<const InDet::PixelClusterOnTrack*>;
      childrots->push_back(fakePix1);
      childrots->push_back(fakePix2);
      std::vector<double>* assgnProb
        = new std::vector<double>;
      assgnProb->push_back(0.4);
      assgnProb->push_back(0.6);
      InDet::CompetingPixelClustersOnTrack* cProt
        = new InDet::CompetingPixelClustersOnTrack(childrots,assgnProb);
      trackStateOnSurfaces->push_back( new TrackStateOnSurface(
        cProt,0,0,0));
    }
  }
  //std::cout<<counter++<<std::endl;

  //create Track and add to collection
  FitQuality* fitQuality = new FitQuality(1.0, 2);

  Trk::TrackInfo info( TrackInfo::Unknown, Trk::undefined);
  //std::cout<<counter++<<std::endl;
  return new Trk::Track(info,  trackStateOnSurfaces, fitQuality);
}

Trk::Track* FakeTrackBuilder::buildBrokenTrack(const InDetDD::SiDetectorElementCollection* /*elements*/) {
  // using namespace Trk;
  //
  // DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces = new DataVector<const Trk::TrackStateOnSurface>;
  //
  // if (elements) {
  //   unsigned int pix1 = 2832011902UL; // CLHEP::bar l1 mod(6,0) index(274,125)
  //   Identifier idPix1(pix1);
  //   InDet::PixelClusterOnTrack* fakePix1
  //     = new InDet::PixelClusterOnTrack(); // make PRD-less ROT
  //   fakePix1->m_detEl = 0;
  //   fakePix1->m_identifier = idPix1;
  //   Trk::LocalPosition locpos = Trk::LocalPosition( 5.559011, 22.613205);
  // //2           Trk::LocalPosition locpos = Trk::LocalPosition( 7.315356, 21.303871);
  //   fakePix1->m_localParams = Trk::LocalParameters(locpos);
  //   Trk::CovarianceMatrix*  locCov = new Trk::CovarianceMatrix(2);
  //   locCov->fast(1,1) = 0.001; locCov->fast(2,2)=0.0135;
  //   fakePix1->m_localErrMat = Trk::ErrorMatrix(locCov);
  //
  //   // AtaPlane - no surface
  //   CLHEP::HepRotation rotation;
  //   CLHEP::Hep3Vector pos(10.0,20.0,30.0);
  //   PlaneSurface planeSf(new HepGeom::Transform3D(rotation,pos));
  //   AtaPlane* trackParameter = new AtaPlane(4.0, 3.0, 2.0, 1.0, 0.001, planeSf );
  //   delete trackParameter->m_associatedSurface;
  //   trackParameter->m_associatedSurface=0;
  // std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> type1(0);
  // type1.set(Trk::TrackStateOnSurface::BremPoint);
  // trackStateOnSurfaces->push_back( new TrackStateOnSurface(0, trackParameter, 0,  0,type1) );
  //
  //   trackStateOnSurfaces->push_back( new TrackStateOnSurface(
  //     fakePix1,0,0,0));
  //
  // }
  //                 //create Track and add to collection
  // FitQuality* fitQuality = new FitQuality(1.0, 2);
  //
  // Trk::TrackInfo info( TrackInfo::Unknown, Trk::undefined);
  //         //  Trk::Track* track = new Trk::Track(Track::unknown,  trackStateOnSurfaces, fitQuality);
  // return new Trk::Track(info,  trackStateOnSurfaces, fitQuality);
    return 0;
}

Rec::TrackParticle* FakeTrackBuilder::buildTrackParticle(const InDetDD::SiDetectorElementCollection* /*elements*/) {
  //FIXME - complete
  return 0;
}
