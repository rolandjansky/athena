/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class TrackPropagationHelper              //
//                                                            //
//  Description: Helper class for using extrapolators to      //
//               extend raw track data                        //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//          Actual extrapolation code by Thijs Cornelissen    //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef TRACKPROPAGATIONHELPER_H
#define TRACKPROPAGATIONHELPER_H

#include "VP1Base/VP1HelperClassBase.h"

#include "TrkSurfaces/PlaneSurface.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"

//#include "CLHEP/Geometry/Point3D.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include <vector>



namespace Trk {
  class Track;
  class IExtrapolator;
  class Volume;
}

class TrackPropagationHelper : public VP1HelperClassBase {
public:

  TrackPropagationHelper( IVP1System* sys );
  ~TrackPropagationHelper();

  bool makePointsNeutral( std::vector<Amg::Vector3D >& points, const Trk::Track* );
  bool makePointsCharged( std::vector<Amg::Vector3D >& points, const Trk::Track*,
			  Trk::IExtrapolator * extrapolator, Trk::ParticleHypothesis hypo = Trk::nonInteracting, bool useMEOT=false, const Trk::Volume* volume=0 );

  std::vector<Trk::PlaneSurface>& getExtrapolationSurfaces() const;
  bool showExtrapolationSurfaces() const;
  
  // double maxR2ForTracks() const; //!< Returns the maximum r^2 to draw track curves up to. Set by 

private:

  class Imp;
  Imp * m_d;

};

#endif
