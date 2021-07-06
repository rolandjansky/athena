/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AssociatedMaterial.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_ASSOCIATEDMATERIAL_H
#define TRKGEOMETRY_ASSOCIATEDMATERIAL_H

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Trk includes
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/MaterialStep.h"

class MsgStream;

namespace Trk {

class Layer;
class TrackingVolume;

/** @class AssociatedMaterial

    It is used in the Mapping process ( using MaterialSteps ),
    the validation and recostruction ( using MaterialProperties )

    @author Andreas.Salzburger@cern.ch
*/

class AssociatedMaterial {
 public:
  /** Default constructor */
  AssociatedMaterial();

  /** Constructor with explicit arguments  */
  AssociatedMaterial(const Amg::Vector3D& hitpos, float steplength, float X0,
                     float L0, float A, float Z, float rho,
                     float correctionFactor, const Trk::TrackingVolume* tvol,
                     const Trk::Layer* lay);

  /** Constructor with Material */
  AssociatedMaterial(const Amg::Vector3D& hitpos, float steplength,
                     const Trk::Material&, float correctionFactor,
                     const Trk::TrackingVolume* tvol, const Trk::Layer* lay);

  /** Constructor with Material Properties - ownership stays with holder*/
  AssociatedMaterial(const Amg::Vector3D& hitpos,
                     const Trk::MaterialProperties*, float correctionFactor,
                     const Trk::TrackingVolume* tvol, const Trk::Layer* lay);

  /** Constructor with MaterialStep - ownership stays with holder */
  AssociatedMaterial(const Trk::MaterialStep*, float correctionFactor,
                     const Trk::TrackingVolume* tvol, const Trk::Layer* lay);

  /** Constructor for empty material bin found in validation */
  AssociatedMaterial(const Amg::Vector3D& hitpos,
                     const Trk::TrackingVolume* tvol, const Trk::Layer* lay);

  /** Copy constructor*/
  AssociatedMaterial(const AssociatedMaterial& am);

  /** Move constructor*/
  AssociatedMaterial(AssociatedMaterial&& am) noexcept;

  /** Destructor */
  ~AssociatedMaterial();

  /**Assignment operator */
  AssociatedMaterial& operator=(const AssociatedMaterial& am);

  /**Move Assignment operator */
  AssociatedMaterial& operator=(AssociatedMaterial&& am) noexcept;

  /** Trivial Access methods */
  const Trk::MaterialProperties* materialProperties() const;

  /** Trivial Access methods */
  const Trk::MaterialStep* materialStep() const;

  /** Trivial Access methods */
  const Amg::Vector3D& materialPosition() const;

  /** Trivial Access methods */
  double correctionFactor() const;

  /** Trivial Access methods */
  const Trk::TrackingVolume* associatedTrackingVolume() const;

  /** Trivial Access methods */
  const Trk::Layer* associatedLayer() const;

  /** Access method : steplength */
  double steplength() const;

  /** Access method : steplength */
  double steplengthInX0() const;

  /** Access method : steplength */
  double steplengthInL0() const;

  /** Access method : material X0/A/Z/rho */
  double x0() const;
  double l0() const;
  double A() const;
  double Z() const;
  double rho() const;

  /** Output Method for MsgStream, to be overloaded by child classes */
  MsgStream& dump(MsgStream& sl) const;

  /** Output Method for std::ostream, to be overloaded by child classes */
  std::ostream& dump(std::ostream& sl) const;

 private:
  Amg::Vector3D m_materialPosition;
  float m_correctionFactor;
  const Trk::MaterialProperties* m_materialProperties;
  const Trk::MaterialStep* m_materialStep;
  const Trk::TrackingVolume* m_trackingVolume;
  const Trk::Layer* m_layer;
  bool m_cleanup;
};

/** Trivial Access methods */
inline const Trk::MaterialProperties* AssociatedMaterial::materialProperties()
    const {
  return m_materialProperties;
}

/** Trivial Access methods */
inline const Trk::MaterialStep* AssociatedMaterial::materialStep() const {
  return m_materialStep;
}

/** Trivial Access methods */
inline const Amg::Vector3D& AssociatedMaterial::materialPosition() const {
  return m_materialPosition;
}

/** Trivial Access methods */
inline double AssociatedMaterial::correctionFactor() const {
  return m_correctionFactor;
}

/** Trivial Access methods */
inline const Trk::TrackingVolume* AssociatedMaterial::associatedTrackingVolume()
    const {
  return m_trackingVolume;
}

/** Trivial Access methods */
inline const Trk::Layer* AssociatedMaterial::associatedLayer() const {
  return m_layer;
}

/** Access method : */
inline double AssociatedMaterial::x0() const {
  if (m_materialProperties) return m_materialProperties->x0();
  if (m_materialStep) return m_materialStep->x0();
  return 0.;
}

/** Access method : steplength */
inline double AssociatedMaterial::steplength() const {
  if (m_materialProperties) return m_correctionFactor;
  if (m_materialStep) return m_materialStep->steplength();
  return 0.;
}

/** Access method : steplength */
inline double AssociatedMaterial::steplengthInX0() const {
  if (m_materialProperties)
    return m_materialProperties->thicknessInX0() * m_correctionFactor;
  if (m_materialStep) return m_materialStep->steplengthInX0();
  return 0.;
}

/** Access method : steplength */
inline double AssociatedMaterial::steplengthInL0() const {
  if (m_materialProperties)
    return m_materialProperties->thicknessInL0() * m_correctionFactor;
  if (m_materialStep) return m_materialStep->steplengthInL0();
  return 0.;
}

inline double AssociatedMaterial::l0() const {
  if (m_materialProperties) return m_materialProperties->l0();
  if (m_materialStep) return m_materialStep->l0();
  return 0.;
}

inline double AssociatedMaterial::A() const {
  if (m_materialProperties) return m_materialProperties->averageA();
  if (m_materialStep) return m_materialStep->A();
  return 0.;
}

inline double AssociatedMaterial::Z() const {
  if (m_materialProperties) return m_materialProperties->averageZ();
  if (m_materialStep) return m_materialStep->Z();
  return 0.;
}

inline double AssociatedMaterial::rho() const {
  if (m_materialProperties) return m_materialProperties->averageRho();
  if (m_materialStep) return m_materialStep->rho();
  return 0.;
}

/**Overload of << operator for both, MsgStream and std::ostream for debug
 * output*/
MsgStream& operator<<(MsgStream& sl, const AssociatedMaterial& mstep);
std::ostream& operator<<(std::ostream& sl, const AssociatedMaterial& mstep);

}  // namespace Trk

#endif
