/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
		ConversionFinderUtils.cxx  -  Description
		-------------------
		begin   : 01-01-2008
		authors : Tatjana Lenz, Thomas Koffas
		email   : tatjana.lenz@cern.ch, Thomas.Koffas@cern.ch
		changes :
***************************************************************************/
#include "InDetConversionFinderTools/ConversionFinderUtils.h"
#include "TrkMeasurementBase/MeasurementBase.h"
//Prep raw data
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "InDetPrepRawData/SiCluster.h"
#include "TrkTrack/Track.h"
#include "VxVertex/VxTrackAtVertex.h"

using HepGeom::Point3D;

namespace InDet {

  static const InterfaceID IID_IConversionFinderUtils("InDet::ConversionFinderUtils", 1, 0);

  ConversionFinderUtils::ConversionFinderUtils(const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent)
    : AthAlgTool(type, name, parent)
  {
    declareInterface<ConversionFinderUtils>(this);
  }

  ConversionFinderUtils::~ConversionFinderUtils() = default;

  const InterfaceID& ConversionFinderUtils::interfaceID()
  {
    return IID_IConversionFinderUtils;
  }

  StatusCode ConversionFinderUtils::initialize() {
    return StatusCode::SUCCESS;
  }

  StatusCode ConversionFinderUtils::finalize() {
    return StatusCode::SUCCESS;
  }

  /**
   * hits counter
   */
  void
  ConversionFinderUtils::countHits(
    const DataVector<const Trk::MeasurementBase>* mb,
    int& ntrt,
    int& nclus) const
  {

    DataVector<const Trk::MeasurementBase>::const_iterator its,
      itse = mb->end();

    for (its = mb->begin(); its != itse; ++its) {

      const Trk::RIO_OnTrack* ri = dynamic_cast<const Trk::RIO_OnTrack*>(*its);
      if (!ri)
        continue;

      const Trk::PrepRawData* rd = ri->prepRawData();
      if (!rd){
        continue;
      }

      if (rd->type(Trk::PrepRawDataType::SiCluster)) {
        ++nclus;
        continue;
      }

      if (rd->type(Trk::PrepRawDataType::TRT_DriftCircle)) {
        ++ntrt;
        continue;
      }

    } // end of loop over meas.bases
  }   // end of count hits method

  /**
   * hl trt hits / trt hits ratio calculater
   */
  double
  ConversionFinderUtils::trRatio(
    const DataVector<const Trk::MeasurementBase>* mb) const
  {

    DataVector<const Trk::MeasurementBase>::const_iterator itp=mb->begin(), itpe=mb->end();
    int ntrth = 0;
    int nHL = 0;
    for(;itp!=itpe;++itp) {
      const Trk::RIO_OnTrack* ri = dynamic_cast<const Trk::RIO_OnTrack*>(*itp);
      if(!ri) continue;
      const Trk::PrepRawData* rd = ri->prepRawData();

      if(!rd) continue;
      const InDet::TRT_DriftCircle* RawDataClus = dynamic_cast<const InDet::TRT_DriftCircle*>(rd);
      if(!RawDataClus) continue;
      ++ntrth;

      if(RawDataClus->highLevel()) ++nHL;
    }//end of loop over measurement bases

    if(ntrth>0) return double(nHL)/double(ntrth);
    return 1000.;
  } // end of trRatio method

  /**
   * mom fraction
   */
  double
  ConversionFinderUtils::momFraction(const Trk::TrackParameters* per1,
                                     const Trk::TrackParameters* per2) const
  {

    const Amg::Vector3D& mom_pos = per1->momentum();
    const Amg::Vector3D& mom_neg = per2->momentum();
    double momFraction = mom_pos.mag()/(mom_pos.mag() + mom_neg.mag());
    return momFraction;
  }

  /**
   * distance between two tracks
   */
  double
  ConversionFinderUtils::distBetweenTracks(const Trk::Track* trk_pos,
                                           const Trk::Track* trk_neg) const
  {

    //position of the first measurement on the positive track
    const Trk::MeasurementBase* first_pos_meas = trk_pos->measurementsOnTrack()->front();
    Amg::Vector3D trk_hit_pos = first_pos_meas->globalPosition();

    //check if really the first measurement
    for (const Trk::MeasurementBase* m : *trk_pos->measurementsOnTrack())
      if(trk_hit_pos.mag() > m->globalPosition().mag()) first_pos_meas = m;

    trk_hit_pos = first_pos_meas->globalPosition();

    //position of the first hit--->track2
    const Trk::MeasurementBase* first_neg_meas = trk_neg->measurementsOnTrack()->front();
    Amg::Vector3D trk_hit_neg = first_neg_meas->globalPosition();

    //check if really the first measurement
    for (const Trk::MeasurementBase* m : *trk_neg->measurementsOnTrack())
      if(trk_hit_neg.mag() > m->globalPosition().mag()) first_neg_meas = m;

    trk_hit_neg = first_neg_meas->globalPosition();
    double distance = 1000.;

    //check if measurements are on the same surface
    if (first_pos_meas->associatedSurface() == first_neg_meas->associatedSurface()) distance =
      sqrt(pow(trk_hit_pos[0] - trk_hit_neg[0],2.) + pow(trk_hit_pos[1] - trk_hit_neg[1],2.) +
	   pow(trk_hit_pos[2] - trk_hit_neg[2],2.));


    //if not choose the track with the fist measurement closest to 000 and calculate the distance
    //of the closest approach of another track to this measurement
    else {

      //define reference point and track parameter (--> perigees) of the second track
      Amg::Vector3D ref_point;
      const Trk::Perigee* perigee;

      if (first_pos_meas->globalPosition().mag() < first_neg_meas->globalPosition().mag()) {
        ref_point = first_pos_meas->globalPosition();
        perigee = trk_neg->perigeeParameters();
      } else {
        ref_point = first_neg_meas->globalPosition();
        perigee = trk_pos->perigeeParameters();
      }

      // when the helix can be approximated as a straight line, when the
      // distance of closest approach can be calculated as distance^2 = [momentum
      // x (ref_point-position)]^2/momentum^2
      Amg::Vector3D momentum = perigee->momentum();
      Amg::Vector3D position = perigee->position();
      double p = momentum.mag();
      Amg::Vector3D delta = position - ref_point;
      distance = sqrt(pow(delta.mag(),2.) - pow((delta.adjoint()*momentum)[0]/p,2.));
    }

    ATH_MSG_DEBUG("Distance between two tracks = "<<distance);
    return distance;
  } // end of distBetweenTracks method

  /**
   * return first track parameters
   */
  const Trk::TrackParameters*
  ConversionFinderUtils::getTrkParameters(const Trk::Track* track) const
  {
    const DataVector<const Trk::TrackStateOnSurface>* tsos = track->trackStateOnSurfaces();
    if(!tsos) return nullptr;

    DataVector<const Trk::TrackStateOnSurface>::const_iterator itse = tsos->end();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator itsb = tsos->begin();

    for(;itsb!=itse;++itsb) {
      if((*itsb)->measurementOnTrack()) {
        const Trk::TrackParameters* trkP = (*itsb)->trackParameters();
        if(trkP->associatedSurface().center().perp()>=10.) return trkP;
      }//end of meas on track check
    }//end of loop over all track states on surface

    return nullptr;
  }

  /**
   * return first track particle parameters
   */
  const Trk::TrackParameters*
  ConversionFinderUtils::getTrkParticleParameters(
    const Trk::TrackParticleBase* track) const
  {

    std::vector<const Trk::TrackParameters*>::const_iterator vpb = track->trackParameters().begin();
    std::vector<const Trk::TrackParameters*>::const_iterator vpe = track->trackParameters().end();
    for(;vpb != vpe; ++vpb) if((*vpb)->position().perp()>=10.) return (*vpb);
    return nullptr;
  }

  /* add recalculated perigees to the track*/
  const Trk::Track*
  ConversionFinderUtils::addNewPerigeeToTrack(const Trk::Track* track,
                                              const Trk::Perigee* mp) const
  {

    // fitQuality from track
    const Trk::FitQuality* fq = track->fitQuality()->clone();
    if(!fq) return nullptr;

    // output datavector of TSOS
    DataVector<const Trk::TrackStateOnSurface>*	    ntsos = new DataVector<const Trk::TrackStateOnSurface>;
    const DataVector<const Trk::TrackStateOnSurface>* tsos = track->trackStateOnSurfaces();
    if(!tsos) {delete ntsos; return nullptr;}
    DataVector<const Trk::TrackStateOnSurface>::const_iterator its,itse = tsos->end();
    for(its=tsos->begin();its!=itse;++its) {

      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
      typePattern.set(Trk::TrackStateOnSurface::Perigee);
      const Trk::TrackStateOnSurface* per_tsos =
        ((*its)->type(Trk::TrackStateOnSurface::Perigee))
          ? new Trk::TrackStateOnSurface(nullptr, mp->clone(), nullptr, nullptr, typePattern)
          : (*its)->clone();
      ntsos->push_back(per_tsos);
    }

    //Construct the new track
    Trk::TrackInfo info;
    Trk::Track* newTrk = new Trk::Track(info, ntsos, fq);
    return newTrk;
  }

  xAOD::Vertex*
  ConversionFinderUtils::correctVxCandidate(xAOD::Vertex* initVxCandidate,
                                            Amg::Vector3D guessVertex) const
  {
    Amg::Vector3D correctVertex(initVxCandidate->position().x()+guessVertex.x(),
			     initVxCandidate->position().y()+guessVertex.y(),
			     initVxCandidate->position().z()+guessVertex.z());

    Amg::Vector3D globalVertexPosition(correctVertex.x(),correctVertex.y(),correctVertex.z());

    std::vector<Trk::VxTrackAtVertex> tmpVTAV;

    std::vector<Trk::VxTrackAtVertex> trkAtVtx = initVxCandidate->vxTrackAtVertex();
    for (unsigned int i = 0; i < trkAtVtx.size() ; ++i) {
      Trk::VxTrackAtVertex vtxTrack = trkAtVtx[i];
      const Trk::TrackParameters*  vtxPer = vtxTrack.perigeeAtVertex();
      const AmgVector(5)& iv = vtxPer->parameters();
      AmgSymMatrix(5) em(*(vtxPer->covariance()));
      Trk::PerigeeSurface surface (globalVertexPosition);

      Trk::TrackParameters* tmpMeasPer = surface.createParameters<5,Trk::Charged>(0.,0.,iv[2],iv[3],iv[4],&em);

      Trk::VxTrackAtVertex trkV(vtxTrack.trackQuality().chiSquared(),
                                tmpMeasPer);
      tmpVTAV.push_back(trkV);
    }//end of loop over VxTracksAtVertex

    if(tmpVTAV.size()!=2) return nullptr;

    //Create the xAOD::Vertex and set the position and VxTrackAtVertex properly
    xAOD::Vertex *vx = new xAOD::Vertex(*initVxCandidate);
    vx->setPosition(correctVertex);
    vx->vxTrackAtVertex().clear();
    for (unsigned int i = 0; i < tmpVTAV.size() ; ++i) {
      Trk::VxTrackAtVertex vtxTrack = tmpVTAV[i];
      vx->vxTrackAtVertex().push_back(vtxTrack);
    }

    return vx;
  }//end of correct vxCandidate method

} // end InDet
