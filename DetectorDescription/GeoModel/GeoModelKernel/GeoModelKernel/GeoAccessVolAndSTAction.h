/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GeoAccessVolAndSTAction_h
#define GeoAccessVolAndSTAction_h 1

#include <vector>
#include "CLHEP/Geometry/Transform3D.h"

#include "GeoModelKernel/GeoNodeAction.h"

class GeoVPhysVol;
class GeoTransform;

//
// Class: GeoAccessVolAndSTAction
//
//    Accesses volumes and serial transformers, returning the ith volume 
//    and also its transformation relative to the parent.
//

class GeoAccessVolAndSTAction : public GeoNodeAction
{
 public:
  GeoAccessVolAndSTAction(unsigned int Index);
  virtual ~GeoAccessVolAndSTAction();

  virtual void handleTransform(const GeoTransform* xform);
  virtual void handlePhysVol(const GeoPhysVol* vol);
  virtual void handleFullPhysVol(const GeoFullPhysVol* vol);

  PVConstLink getVolume() const;
  const HepGeom::Transform3D & getTransform() const;
  const HepGeom::Transform3D & getDefTransform() const;
  const std::string & getName() const;

  virtual void handleNameTag(const GeoNameTag *nameTag);
  virtual void handleSerialDenominator(const GeoSerialDenominator* sD);

  virtual void handleSerialTransformer(const GeoSerialTransformer* sT);
  const GeoSerialTransformer* getSerialTransformer() const;

  virtual void handleIdentifierTag(const GeoIdentifierTag* idTag);

  Query<unsigned int> getId() const;

 private:
  GeoAccessVolAndSTAction(const GeoAccessVolAndSTAction &right);
  const GeoAccessVolAndSTAction & operator=(const GeoAccessVolAndSTAction &right);

  // A pointer to the ith physical volume under this one.
  PVConstLink m_volume;

  // A pointer to the Serial Transformer
  const GeoSerialTransformer* m_serialTransformer;
 
  // The transformation and default transformation to the ith volume.
  HepGeom::Transform3D m_transform;
  HepGeom::Transform3D m_defTransform;

  unsigned int m_index;
  unsigned int m_counter;

  // The name of the volume.  From a nametag or a serial denominator
  mutable std::string m_name;

  // The identifier of the volume.  From an identifier tag.
  mutable Query<unsigned int> m_id;

  // A pointer to a name tag.  If the volume is named.
  const GeoNameTag *m_nameTag;
  const GeoSerialDenominator *m_serialDenominator;
  const GeoIdentifierTag *m_idTag;

  std::vector<const GeoTransform *>  m_pendingTransformList;
  unsigned int m_serialDenomPosition;
};

#endif


