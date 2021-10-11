/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TransportJacobian.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXUTILS_TARGETSURFACES_H
#define TRKEXUTILS_TARGETSURFACES_H

// CLHEP
#include <utility>

#include "EventPrimitives/EventPrimitives.h"
#include "TrkExUtils/ExtrapolationCell.h"
#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/BoundaryCheck.h"
#include "TrkSurfaces/Surface.h"

class MsgStream;

namespace Trk {

class TrackingVolume;

/** @class TargetSurfaces

   This class handles selection and distance estimate of surfaces whis can be
   reached in propagation/transport of particles.

   @author sarka.todorova@cern.ch

  */

/** typedef for surface and volume navigation types */
enum SurfNavigType
{
  Target = 0,
  BoundaryFrame = 1,
  BoundaryDetached = 2,
  SensitiveLayer = 3, // sensitive layer
  MaterialLayer = 4   // material layer
};
enum TVNavigType
{
  Unknown = 0,
  Frame = 1,           // static volume
  AlignableVolume = 2, // alignable volume
  DenseVolume = 3      // dense alignable volume
};

/** target surface info ( navigation ) */
struct TargetSurface
{

  const Surface* surf;            // surface
  BoundaryCheck bcheck;           // boundary check
  SurfNavigType sfType;           // surface type : boundary, layer ...
  unsigned int index;             // layer or boundary index
  const TrackingVolume* assocVol; // associated volume
  TVNavigType volType;            // volume type : static, detached, dense ...
  double distanceAlongPath;       // latest calculated distance
  double distance;                // minimal distance estimate
  float signAbsDist;              // sign of minimal distance when available
  Amg::Vector3D intersection;     // global position
  int status; // navigation status (-1 crossed, 0 in loop, 1 ahead )

  /** Constructor */
  TargetSurface(const Surface* sf,
                BoundaryCheck bc,
                SurfNavigType stype,
                int ind,
                const TrackingVolume* tVol,
                TVNavigType vtype)
    : surf(sf)
    , bcheck(std::move(bc))
    , sfType(stype)
    , index(ind)
    , assocVol(tVol)
    , volType(vtype)
  {
    distanceAlongPath = 0.;
    distance = 0.;
    signAbsDist = 0.;
    intersection = Amg::Vector3D(0., 0., 0.);
    status = 0;
  }

  /** Destructor */
  ~TargetSurface() = default;

  /** Distance info */
  void setDistance(double dAlongPath, double dMin, float sign)
  {
    distanceAlongPath = dAlongPath;
    distance = dMin;
    signAbsDist = sign;
  }

  /** Intersection info */
  void setPosition(Amg::Vector3D intPos) { intersection = std::move(intPos); }

  /** Navigation status info */
  void setStatus(int st) { status = st; }

  /** fast approximative update ( real distance > distance estimate ) */
  void fastDistanceUpdate(double step)
  {
    // temporary check ( remove after development finished )
    // if ( fabs(distance) < step ) std::cout <<"fast distance update
    // inappropriate " << std::endl; end temporary

    distance = distance > 0 ? distance - step : distance + step;
    distanceAlongPath = distanceAlongPath > 0 ? distanceAlongPath - step
                                              : distanceAlongPath + step;
  }
};

typedef std::vector<TargetSurface> TargetSurfaceVector;
typedef ExtrapolationCell<TrackParameters> ExCellCharged;
typedef ExtrapolationCell<NeutralParameters> ExCellNeutral;

class TargetSurfaces
{
public:
  /** Constructor */
  TargetSurfaces() = default;

  /** Destructor */
  ~TargetSurfaces() = default;

  /** Extract surfaces for charged propagation, step into new frame volume */
  Trk::ExtrapolationCode setOnInput(const Trk::ExCellCharged&,
                                    const Trk::Surface* sf,
                                    const BoundaryCheck& bc);

  /** Extract surfaces for charged propagation, step into new frame volume */
  Trk::ExtrapolationCode setOnInput(const Amg::Vector3D& position,
                                    const Amg::Vector3D& direction,
                                    const Trk::TrackingVolume*,
                                    const Trk::Surface* sf,
                                    const BoundaryCheck& bc);

  /** Ordered intersections for neutral transport, step into new frame volume */
  TargetSurfaceVector orderedIntersections(const Trk::ExCellNeutral&,
                                           const Trk::Surface* sf,
                                           const BoundaryCheck& bc);

  /** Ordered intersections for neutral transport, step into new frame volume */
  TargetSurfaceVector orderedIntersections(const Amg::Vector3D& position,
                                           const Amg::Vector3D& direction,
                                           const Trk::TrackingVolume*,
                                           const Trk::Surface* sf,
                                           const BoundaryCheck& bc);

  /** update of target surfaces at input or at frame volume boundary */
  bool initFrameVolume(const Amg::Vector3D& position,
                       const Amg::Vector3D& direction,
                       const Trk::TrackingVolume*);

  /** intersections */
  void fillSolutions(int index,
                     Amg::Vector3D gp,
                     TargetSurfaceVector& solutions);

  /** distance reevaluation */
  bool checkDistance(const Amg::Vector3D& position,
                     const Amg::Vector3D& direction,
                     double nextStep);

  /** estimated distance along path to the nearest surface */
  double distanceToNext();

  /** step over intersection */
  bool flipDirection();

  /** index of nearest surface */
  int nextSf();

  /** number of intersections along path */
  unsigned numSf();

  /** debug mode */
  bool debugMode();

  /** set debug mode */
  void setDebugModeOn() { m_debugMode = true; }

  /** set debug mode */
  void setDebugModeOff() { m_debugMode = false; }

  /** current material volume/input for propagation */
  const Trk::TrackingVolume* currentDense();

  /** current material volume/input for propagation */
  const Trk::TrackingVolume* currentFrame();

private:
  void evaluateInputDistance(Trk::TargetSurface& tt,
                             const Amg::Vector3D& pos,
                             const Amg::Vector3D& dir,
                             bool base);
  void save(Trk::TargetSurface& tt, bool base);
  void findNext();
  TargetSurfaceVector orderIntersections() const;
  bool updateDistance(int index,
                      Trk::TargetSurface& tt,
                      const Amg::Vector3D& position,
                      const Amg::Vector3D& direction);

  bool m_orderTrue; // neutral(true)/charged(false)
  bool m_flipDirection;
  bool m_absDist;
  bool m_debugMode;
  float m_tolerance;
  TargetSurfaceVector
    m_baseSurfaces; // surfaces to be followed all along the path;
  std::vector<TargetSurfaceVector>
    m_tempSurfaces;              // surfaces shadowed by an envelope
  TargetSurfaceVector m_ordered; // ordered intersections;

  Amg::Vector3D m_probePos;
  Amg::Vector3D m_probeDir;
  unsigned int m_numAlongPath;
  int m_nextSf;
  double m_distanceToNext;
  double m_lastStep;
  double m_flipDistance;

  const Trk::TrackingVolume* m_currentFrame;
  const Trk::TrackingVolume* m_currentDense;
};

inline double
TargetSurfaces::distanceToNext()
{
  return m_distanceToNext;
}

inline bool
TargetSurfaces::flipDirection()
{
  return m_flipDirection;
}

inline int
TargetSurfaces::nextSf()
{
  return m_nextSf;
}

inline unsigned int
TargetSurfaces::numSf()
{
  return m_numAlongPath;
}

inline bool
TargetSurfaces::debugMode()
{
  return m_debugMode;
}

inline const Trk::TrackingVolume*
TargetSurfaces::currentDense()
{
  return m_currentDense;
}

inline const Trk::TrackingVolume*
TargetSurfaces::currentFrame()
{
  return m_currentFrame;
}

} // end of namespace

#endif // TRKEXUTILS_TARGETSURFACES_H
