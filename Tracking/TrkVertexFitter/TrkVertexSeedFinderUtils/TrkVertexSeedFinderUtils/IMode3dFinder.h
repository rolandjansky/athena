/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMode3dFinder.h, (c) ATLAS Detector software 2006
///////////////////////////////////////////////////////////////////

#ifndef TRKSEEDFINDERUTILS_IMODE3DFINDER_H
#define TRKSEEDFINDERUTILS_IMODE3DFINDER_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkVertexSeedFinderUtils/SeedFinderParamDefs.h"

namespace Trk
{

  /**
   @class IMode3dFinder

   Interface class for the algotool which calculates the mode of a 
   threedimensional distribution.

   @author Giacinto.Piacquadio@physik.uni-freiburg.de

   */

  /// Auxillary interface for getting back additional data.
  /// Mostly for debugging/monitoring.
  class IMode3dInfo
  {
  public:
    virtual ~IMode3dInfo() = default;

    virtual unsigned int Modes1d(std::vector<float> &,
                                 std::vector<float> &, 
                                 std::vector<float> &,
                                 std::vector<float> &) const = 0 ;

    virtual const std::vector<int>& AcceptedCrossingPointsIndices() const = 0;

    virtual void getCorrelationDistance( double &cXY, double &cZ ) const = 0 ;

    virtual int perigeesAtSeed( std::vector<const Trk::TrackParameters*>& perigees , 
                                const std::vector<const Trk::TrackParameters*>& perigeeList ) const = 0;

    virtual void setTrkidx (std::vector< std::pair <int, int> >&& trkidx) = 0;
  };


  class IMode3dFinder : virtual public IAlgTool
  {
  public:
    DeclareInterfaceID( IMode3dFinder, 1, 0);

    /**
     * @brief Obtain the 3d-mode (position) from a list of positions
     *        (distribution in space)
     * @param vx Primary vertex x-coordinate.
     * @param vy Primary vertex y-coordinate.
     * @param points List of points with weights.
     */
    virtual Amg::Vector3D
    getMode (const double vx,
             const double vy,
             const std::vector<Trk::PositionAndWeight> &) const = 0;


    /**
     * @brief Obtain the 3d-mode (position) from a list of positions
     *        (distribution in space)
     * @param vx Primary vertex x-coordinate.
     * @param vy Primary vertex y-coordinate.
     * @param points List of points with weights.
     * @param info[out] Optionally returns an object for retrieving
     *                  additional information.  May be left null if additional
     *                  information is not available.
     */
    virtual Amg::Vector3D
    getMode (const double vx,
             const double vy,
             const std::vector<Trk::PositionAndWeight>& points,
             std::unique_ptr<IMode3dInfo>& info) const = 0;


    /**
     * @brief Obtain the 3d-mode (position) from a list of positions
     *        (distribution in space)
     * @param vx Primary vertex x-coordinate.
     * @param vy Primary vertex y-coordinate.
     * @param points List of points --- unweighted!
     */
    virtual Amg::Vector3D
    getMode (const double vx,
             const double vy,
             const std::vector<Amg::Vector3D>& points) const = 0;

    
    /**
     * @brief Obtain the 3d-mode (position) from a list of positions
     *        (distribution in space)
     * @param vx Primary vertex x-coordinate.
     * @param vy Primary vertex y-coordinate.
     * @param points List of points --- unweighted!
     * @param info[out] Optionally returns an object for retrieving
     *                  additional information.  May be left null if additional
     *                  information is not available.
     */
    virtual Amg::Vector3D
    getMode (const double vx,
             const double vy,
             const std::vector<Amg::Vector3D>& points,
             std::unique_ptr<IMode3dInfo>& info) const = 0;
  };
}

#endif
