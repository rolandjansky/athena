/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITrkDistanceFinder.h, (c) ATLAS Detector software 2006
///////////////////////////////////////////////////////////////////

#ifndef TRKVERTEXSEEDFINDERUTILS_ITRKDISTANCEFINDER_H
#define TRKVERTEXSEEDFINDERUTILS_ITRKDISTANCEFINDER_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"

//Amg
#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk
{

  class Track;
  class TrackParticleBase;

  static const InterfaceID IID_ITRKDISTANCEFINDER("ITrkDistanceFinder", 1, 1);

  /**
   @class ITrkDistanceFinder

   Interface class for the algotool which calculates the distance between two 
   tracks.

   @author Giacinto.Piacquadio@physik.uni-freiburg.de

   */


  class ITrkDistanceFinder : virtual public IAlgTool {

     public:
       /** Virtual destructor */
       virtual ~ITrkDistanceFinder(){};

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_ITRKDISTANCEFINDER; };
       
       /** method to do the calculation starting from two MeasuredPerigees*/
       /** return value is true if calculation is successfull */
       virtual bool CalculateMinimumDistance(const Trk::Perigee &, const Trk::Perigee &) = 0;

       /** method to do the calculation starting from two tracks */
       virtual bool CalculateMinimumDistance(const  Trk::Track &, const Trk::Track &) = 0;
       
       /** method to do the calculation starting from two track particles */
       virtual bool CalculateMinimumDistance(const  Trk::TrackParticleBase &, const Trk::TrackParticleBase &) = 0;

       /**method to obtain the distance (call CalculateMinimumDistance before) **/
       virtual double GetDistance() const =0;

       /** method to obtain the points on the two tracks at minimum distance **/
       virtual const std::pair<Amg::Vector3D,Amg::Vector3D> GetPoints() const =0;
       

  };
}

#endif

