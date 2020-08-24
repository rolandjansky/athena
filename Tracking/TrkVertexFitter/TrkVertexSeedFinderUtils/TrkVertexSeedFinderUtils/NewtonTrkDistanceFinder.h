/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXSEEDFINDERUTILS_NEWTONTRKDISTANCEFINDER_H
#define TRKVERTEXSEEDFINDERUTILS_NEWTONTRKDISTANCEFINDER_H

#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexSeedFinderUtils/SeedFinderParamDefs.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"

namespace Trk
{

  /**
  @class NewtonTrkDistanceFinder

   AlgoTool which uses an iterative Newton method in two dimensions (the two points along the 
   two tracks) in order to find their minimum distance. For each track you can also provade a starting 
   point, in order to avoid local maxima or an undefined quadratic form near a point...

   @author Giacinto.Piacquadio@physik.uni-freiburg.de

   */

  static const InterfaceID IID_NewtonTrkDistanceFinder("NewtonTrkDistanceFinder", 1, 1);
  
  class NewtonTrkDistanceFinder : public AthAlgTool
  {
  public:

    static const InterfaceID& interfaceID()
      {
        return IID_NewtonTrkDistanceFinder;
      };

    
    //default constructor due to Athena interface
    NewtonTrkDistanceFinder(const std::string& t, const std::string& n, const IInterface*  p);
    
    
    //
    //destructor
    virtual ~NewtonTrkDistanceFinder();

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    
  const TwoPoints GetClosestPoints(const Perigee & a, const Perigee & b) const {
    //with the constractur of PointOnTrackPar a track is constructed with, as seed, 
    //directly the point of closest approach (see for info PointOnTrack.h)
    return GetClosestPoints(PointOnTrack(a),PointOnTrack(b));
  }
  
  TwoPoints GetClosestPoints(const PointOnTrack &, const PointOnTrack &) const;

  const TwoPoints GetClosestPoints(const TwoTracks & twotracks) const {
    return GetClosestPoints(twotracks.getFirstPerigee(),twotracks.getSecondPerigee());
  }
  
  const TwoPoints GetClosestPoints(const TwoPointOnTrack & twopointontrack) const {
    return GetClosestPoints(twopointontrack.first,twopointontrack.second);
  }

  private:
  //parameters for precision
  double m_precision;//as job option
  double m_maxloopnumber;//as job option

  //variables for magnetic field service needed to retrieve the correct Bz
  SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey 
        {this, "AtlasFieldCacheCondObj", "fieldCondObj", "Name of the Magnetic Field conditions object key"};

  };

  namespace Error {
    struct NewtonProblem {
      const char* p;
      NewtonProblem(const char* q) {p=q; }
    };	
  }

}

#endif
