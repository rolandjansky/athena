/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file GeoModelUtilities/src/GeoVisitVolumes.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2019
 * @brief Visitor to process all volumes under a GeoModel node.
 *
 * Based on GeoAccessVolumeAction.
 */


#include "GeoModelUtilities/GeoVisitVolumes.h"


#include "GeoModelUtilities/GeoVisitVolumes.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoSerialDenominator.h"
#include "GeoModelKernel/GeoSerialIdentifier.h"

#include <string>


GeoVisitVolumes::GeoVisitVolumes (IGeoVisitVolumesAction& action,
                                  int depthLimit /*= 1*/)
  : m_action (action)
  , m_nameTag(nullptr)
  , m_serialDenominator(nullptr)
  , m_idTag(nullptr)
  , m_serialIdentifier(nullptr)
{
  setDepthLimit (depthLimit);
  m_pendingTransformList.reserve(1);
}


void GeoVisitVolumes::handleTransform (const GeoTransform *xform)
{
  m_pendingTransformList.push_back (xform);
}


void GeoVisitVolumes::handleVol (const GeoVPhysVol *vol)
{
  //      
  // Return if we have not got to any children:     
  //     
  if(getPath()->getLength()-1 == 0)
    return;

  GeoTrf::Transform3D transform;
  GeoTrf::Transform3D defTransform;

  if (m_pendingTransformList.size() == 1) {
    transform = m_pendingTransformList[0]->getTransform(nullptr);
    defTransform = m_pendingTransformList[0]->getDefTransform(nullptr);
  }
  else {
    transform = GeoTrf::Transform3D::Identity();
    defTransform = GeoTrf::Transform3D::Identity();
    for (const GeoTransform* t : m_pendingTransformList) {
      transform    = transform    * t->getTransform(nullptr);
      defTransform = defTransform * t->getDefTransform(nullptr);
    }
  }

  const static std::string anon = "ANON";
  const std::string& name = m_nameTag ? m_nameTag->getName() : anon;

  int id = m_idTag ? m_idTag->getIdentifier() : -1;
    
  m_action (id, name, vol, transform, defTransform);

  m_idTag   = nullptr;
  m_nameTag = nullptr;
  m_pendingTransformList.clear();
}


void GeoVisitVolumes::handlePhysVol (const GeoPhysVol *vol)
{
  handleVol (vol);
}

void GeoVisitVolumes::handleFullPhysVol (const GeoFullPhysVol *vol)
{
  handleVol (vol);
}


void GeoVisitVolumes::handleNameTag (const GeoNameTag *nameTag)
{
  m_nameTag = nameTag;
  m_serialDenominator = nullptr;
}


void GeoVisitVolumes::handleSerialDenominator (const GeoSerialDenominator *sD)
{
  m_serialDenominator = sD;
}


#if defined(FLATTEN) && defined(__GNUC__)
// We compile this package with optimization, even in debug builds; otherwise,
// the heavy use of Eigen makes it too slow.  However, from here we may call
// to out-of-line Eigen code that is linked from other DSOs; in that case,
// it would not be optimized.  Avoid this by forcing all Eigen code
// to be inlined here if possible.
__attribute__ ((flatten))
#endif
void GeoVisitVolumes::handleSerialTransformer (const GeoSerialTransformer  *sT)
{
  const GeoVPhysVol* vol = &*sT->getVolume();
  unsigned int ncopies = sT->getNCopies();
  int idbase = -1;
  if (m_serialIdentifier) {
    idbase = m_serialIdentifier->getBaseId();
  }

  GeoTrf::Transform3D transform (GeoTrf::Transform3D::Identity());  
  GeoTrf::Transform3D defTransform (GeoTrf::Transform3D::Identity());
  for (const GeoTransform* t : m_pendingTransformList) {
    transform    = transform    * t->getTransform(nullptr);
    defTransform = defTransform * t->getDefTransform(nullptr);
  }

  for (unsigned int i = 0; i < ncopies; i++) {
    int id = idbase;
    if (id >= 0) {
      id += i;
    }

    std::string name;
    if (m_serialDenominator) {
      name = m_serialDenominator->getBaseName() + std::to_string(i);
    }
    else {
      name = "ANON";
    }

    m_action (id, name, vol,
              transform * sT->getTransform (i),
              defTransform * sT->getTransform (i));
  }

  m_idTag   = nullptr;
  m_nameTag = nullptr;
  m_pendingTransformList.clear();
}


void GeoVisitVolumes::handleIdentifierTag (const GeoIdentifierTag *idTag)
{
  m_idTag = idTag;
  m_serialIdentifier = nullptr;
}


void GeoVisitVolumes::handleSerialIdentifier(const GeoSerialIdentifier *sI)
{
  m_serialIdentifier = sI;
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
GeoVolumeVec_t geoGetVolumes (const GeoGraphNode* node, int depthLimit /*= 1*/)
{
  GeoVolumeVec_t ret;
  ret.reserve (20);
  geoVisitVolumes ([&] (int /*id*/,
                        const std::string& /*name*/,
                        const GeoVPhysVol* volume,
                        const GeoTrf::Transform3D& transform,
                        const GeoTrf::Transform3D& /*defTransform*/)
                   { ret.emplace_back (volume, transform); },
                   node,
                   depthLimit);
  return ret;
}
