/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKCALOCLUSTERROI_H
#define TRKCALOCLUSTERROI_H

// Amg
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkSurfaces/SurfaceHolders.h"

namespace Trk {
class Surface;
class LocalParameters;

/** @class CaloClusterROI
  Class to handle ROI for CaloClusters,

*/

class CaloClusterROI : public SurfacePtrHolder
{
public:
  /** Default Constructor for POOL */
  CaloClusterROI();
  /** Copy Constructor */
  CaloClusterROI(const CaloClusterROI& ccRIO);
  /** Assignment operator */
  CaloClusterROI& operator=(const CaloClusterROI& ccRIO);
  /** Move Constructor */
  CaloClusterROI(CaloClusterROI&& ccRIO) noexcept = default;
  /** Move Assignment operator */
  CaloClusterROI& operator=(CaloClusterROI&& ccRIO) noexcept = default;

  /** Constructor with parameters  LocalParameters*,  Surface&
  This class owns the LocalParameters,  A copy of the
                    surface will be made if it is not owned by a detector
  element.    */
  CaloClusterROI(const LocalParameters* locpars,
                 const Surface& surf,
                 const double energy = 0,
                 const double widthPhi = 0.1,
                 const double widthEta = 0.1);

  /** Destructor */
  ~CaloClusterROI();

  /** Pseudo-constructor*/
  CaloClusterROI* clone() const;

  /** returns the LocalParameters */
  const LocalParameters& localParameters() const;

  /** returns the surface for the local to global transformation */
  const Surface& associatedSurface() const;

  /** method to get the global Position */
  Amg::Vector3D globalPosition() const;

  double energy() const;

  double phiWidth() const;

  double etaWidth() const;

private:
  /** local position of the Cluster, in the coordinate frame of the Trk::Surface
   * it exists on*/
  std::unique_ptr<const LocalParameters> m_localParams;

  /** Energy*/
  double m_energy;
  /** Width phi*/
  double m_phiWidth;
  /** Width eta*/
  double m_etaWidth;

}; // End of Class

} // Namespace

inline const Trk::LocalParameters&
Trk::CaloClusterROI::localParameters() const
{
  return *m_localParams;
}

inline const Trk::Surface&
Trk::CaloClusterROI::associatedSurface() const
{
  return *m_associatedSurface;
}

inline double
Trk::CaloClusterROI::energy() const
{
  return m_energy;
}

inline double
Trk::CaloClusterROI::phiWidth() const
{
  return m_phiWidth;
}

inline double
Trk::CaloClusterROI::etaWidth() const
{
  return m_etaWidth;
}

#endif // TRKCaloClusterROI_H
