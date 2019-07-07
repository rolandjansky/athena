/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITrkDistanceFinder.h, (c) ATLAS Detector software 2006
///////////////////////////////////////////////////////////////////

#ifndef TRKVERTEXSEEDFINDERUTILS_ITRKDISTANCEFINDER_H
#define TRKVERTEXSEEDFINDERUTILS_ITRKDISTANCEFINDER_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"
#include <optional>

//Amg
#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk
{

  class Track;
  class TrackParticleBase;

  /**
   @class ITrkDistanceFinder

   Interface class for the algotool which calculates the distance between two 
   tracks.

   @author Giacinto.Piacquadio@physik.uni-freiburg.de

   */


  class ITrkDistanceFinder : virtual public IAlgTool {

     public:
       typedef std::pair<Amg::Vector3D,Amg::Vector3D> TwoPoints;

       DeclareInterfaceID( ITrkDistanceFinder, 1, 0 );

       /** Virtual destructor */
       virtual ~ITrkDistanceFinder(){};

       /** method to do the calculation starting from two MeasuredPerigees*/
       /** If successful, returns the points on the two tracks at minimum distance. */
       virtual std::optional<TwoPoints>
       CalculateMinimumDistance(const Trk::Perigee &, const Trk::Perigee &) const = 0;

       /** method to do the calculation starting from two tracks */
       virtual std::optional<TwoPoints>
       CalculateMinimumDistance(const  Trk::Track &, const Trk::Track &) const = 0;
       
       /** method to do the calculation starting from two track particles */
       virtual std::optional<TwoPoints>
       CalculateMinimumDistance(const  Trk::TrackParticleBase &, const Trk::TrackParticleBase &) const = 0;

  };
}

#endif

