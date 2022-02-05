// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file GeoModelUtilities/GeoVisitVolumes.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2019
 * @brief Visitor to process all volumes under a GeoModel node.
 *
 * Based on GeoAccessVolumeAction.
 */


#ifndef GEOMODELUTILITIES_GEOVISITVOLUMES_H
#define GEOMODELUTILITIES_GEOVISITVOLUMES_H

/// Ensure that the extensions for the Vector3D are properly loaded
#include "GeoPrimitives/GeoPrimitives.h"

#include "GeoModelKernel/GeoNodeAction.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include <vector>


class GeoVPhysVol;
class GeoTransform;
class GeoVAlignmentStore;


/**
 * @brief Define a simple callback for each volume found.
 *        It may be more convenient to use the
 *        templated versions below.
 */
class IGeoVisitVolumesAction
{
public:
  virtual ~IGeoVisitVolumesAction() = default;
  virtual void operator() (int id,
                           const std::string& name,
                           const GeoVPhysVol* volume,
                           const GeoTrf::Transform3D& transform,
                           const GeoTrf::Transform3D& defTransform) = 0;
};


/**
 * @brief Visitor to process all volumes under a GeoModel node.
 */
class GeoVisitVolumes : public GeoNodeAction
{
public:
  GeoVisitVolumes (IGeoVisitVolumesAction& action, int depthLimit = 1);

  /// Handles a Transform.
  virtual void handleTransform (const GeoTransform *xform) override;

  /// Handles a physical volume.
  virtual void handlePhysVol (const GeoPhysVol *vol) override;

  /// Handles a physical volume.
  virtual void handleFullPhysVol (const GeoFullPhysVol *vol) override;

  /// Handles a Name Tag.
  virtual void handleNameTag (const GeoNameTag *nameTag) override;

  /// Handles a Serial Denominator.
  virtual void handleSerialDenominator (const GeoSerialDenominator *sD) override;

  /// Handles a Serial Transformer
  virtual void handleSerialTransformer (const GeoSerialTransformer  *sT) override;

  /// Handles an Identifier Tag.
  virtual void handleIdentifierTag (const GeoIdentifierTag *idTag) override;

  /// Handles an Serial Identifier
  virtual void handleSerialIdentifier(const GeoSerialIdentifier *sI) override;


private:
   GeoVisitVolumes (const GeoVisitVolumes &right) = delete;
   GeoVisitVolumes & operator=(const GeoVisitVolumes &right) = delete;

   void handleVol (const GeoVPhysVol *vol);

   IGeoVisitVolumesAction& m_action;

  /// A pointer to a name tag.  If the volume is named.
  const GeoNameTag *m_nameTag;

  /// A pointer to a serial denominator.  If one exists.
  const GeoSerialDenominator *m_serialDenominator;

  /// A pointer to an identifier tag.  If the volume is identified.
  const GeoIdentifierTag *m_idTag;

  /// List of Pending Transformations.
  std::vector<const GeoTransform *>  m_pendingTransformList;

  const GeoSerialIdentifier *m_serialIdentifier;
};


/**
 * @brief Callback object calling a templated functional.
 */
template <class FUNCTION>
class GeoVisitVolumesAction
  : public IGeoVisitVolumesAction
{
public:
  GeoVisitVolumesAction (FUNCTION f) : m_f (f) {}
  virtual void operator() (int id,
                           const std::string& name,
                           const GeoVPhysVol* volume,
                           const GeoTrf::Transform3D& transform,
                           const GeoTrf::Transform3D& defTransform) override
  {
    m_f (id, name, volume, transform, defTransform);
  }
  FUNCTION m_f;
};



/**
 * @brief Template helper for running the visitor.
 */
template <class FUNCTION>
void geoVisitVolumes (FUNCTION f, const GeoGraphNode* node, int depthLimit = 1)
{
  GeoVisitVolumesAction<FUNCTION> act (f);
  GeoVisitVolumes visitor (act, depthLimit);
  node->exec (&visitor);
}


/**
 * @brief Return the child volumes and associated transforms.
 * @param node Root of the graph to traverse.
 * @param depthLimit Depth limit for the traversal.
 *
 * Returns a vector of (volume, transform) pairs for volumes in the graph.
 * The same volume may be returned multiple times with different transforms
 * in the case of a GeoSerialTransform.
 */
typedef std::vector<std::pair<const GeoVPhysVol*, GeoTrf::Transform3D> >
  GeoVolumeVec_t;
GeoVolumeVec_t geoGetVolumes (const GeoGraphNode* node, int depthLimit = 1);


#endif // not GEOMODELUTILITIES_GEOVISITVOLUMES_H
