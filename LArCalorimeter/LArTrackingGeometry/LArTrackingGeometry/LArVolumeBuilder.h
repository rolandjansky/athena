/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LArVolumeBuilder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef LARTRACKINGGEOMETRY_LARVOLUMEBUILDER_H
#define LARTRACKINGGEOMETRY_LARVOLUMEBUILDER_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
// Trk
#include "CaloTrackingGeometry/ICaloSurfaceBuilder.h"
#include "TrkDetDescrInterfaces/ICaloTrackingVolumeBuilder.h"
// STL
#include <vector>

class GeoPVConstLink;

namespace Trk {
class ITrackingVolumeHelper;
class ITrackingVolumeCreator;
class TrackingVolume;
class Material;
class Volume;
class Layer;
}

namespace LAr {

/** @class LArVolumeBuilder

    The LArVolumeBuilder builds the TrackingVolumes for
       - LAr Barrel
       - LAr Inner Endcap
       - LAr Outer Endcap

    The HEC and Forward Calorimeter have to be added later when knowing the
   dimensions of the Tile Calorimter.

    @author Andreas.Salzburger@cern.ch
  */
class LArVolumeBuilder final
  : public AthAlgTool
  , virtual public Trk::ICaloTrackingVolumeBuilder
{

public:
  /** AlgTool style constructor */
  LArVolumeBuilder(const std::string&, const std::string&, const IInterface*);
  /** Destructor */
  virtual ~LArVolumeBuilder();

  /** AlgTool initialize method */
  virtual StatusCode initialize() override final;
  /** AlgTool finalize method */
  virtual StatusCode finalize() override final;

  /** TrackingVolumeBuilder interface method - returns vector of Volumes */
  virtual const std::vector<Trk::TrackingVolume*>* trackingVolumes(const CaloDetDescrManager& caloDDM)
    const override final;

private:
  // ------------- private methods -----------------------------------------
  static void printCheckResult(MsgStream& log, const Trk::TrackingVolume* vol);

  void printInfo(const GeoPVConstLink& pv, int gen = -1) const;
  void printChildren(const GeoPVConstLink& pv,
                     int gen,
                     int igen,
                     const Amg::Transform3D& tr) const;
  GeoPVConstLink getChild(const GeoPVConstLink& mother,
                          const std::string& name,
                          Amg::Transform3D& trIn) const;

  void throwIntoGarbage(const Trk::Material* mat) const;

  // ------------- private members -----------------------------------------

  std::string m_lArMgrLocation; //!< Location of the CaloDetDescrMgr

  //!< Helper Tool to create TrackingVolumes
  ToolHandle<Trk::ITrackingVolumeHelper> m_lArTrackingVolumeHelper;
  //!< helper for volume creation
  ToolHandle<Trk::ITrackingVolumeCreator> m_trackingVolumeCreator;

  double m_lArBarrelEnvelope; //!< envelope Cover of the Barrel
  double m_lArEndcapEnvelope; //!< envelope Cover of the Endcap

  bool m_useCaloSurfBuilder; //!< if true use DetDescr based layering, if false
                             //!< use biequidistant layering
  //!< if m_useCaloSurfBuilder == true, number of layers
  //!< per dead material region or sampling
  unsigned int m_lArLayersPerRegion;

  //!< if true use DetDescr based layering,
  //!< if false use biequidistant layering
  bool m_useCaloTrackingGeometryBounds;
  mutable float m_mbtsZ;     // MBTS layer position
  mutable float m_mbts_rmin; // MBTS layer dimensions
  mutable float m_mbts_rmax; // MBTS layer dimensions

  //!< tool required for DetDescr-based layering
  ToolHandle<ICaloSurfaceBuilder> m_calosurf;

  //internal garbage collector not MT safe
  mutable std::map<const Trk::Material*, bool> m_materialGarbage;

  // material scaling ( temporary ? )
  float m_scale_HECmaterial;
};

} // end of namespace

inline void
LAr::LArVolumeBuilder::throwIntoGarbage(const Trk::Material* mat) const
{
  if (mat)
    m_materialGarbage[mat] = true;
}

#endif // CALOTRACKINGGEOMETRY_LARVOLUMEBUILDER_H

