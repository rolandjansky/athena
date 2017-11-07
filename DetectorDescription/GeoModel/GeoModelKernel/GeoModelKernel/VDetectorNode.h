/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOMODELKERNEL_VDETECTORNODE_H
#define GEOMODELKERNEL_VDETECTORNODE_H

/**
 * @class VDetectorNode
 *
 * @brief Base class for detector node.  Detector nodes combine
 *	factories and managers.  They are going out of style.  We
 *	now prefer to separate the two functions.
 *
 *	This class is provided for backwards compatibility,
 *	only.  Please use the classes "GeoVDetectorFactory" and
 *	"GeoVDetectorManager".
 *
 *	This class will disappear in future releases.
 */

#include "GeoModelKernel/GeoVDetectorManager.h"
#include "GeoModelKernel/GeoVDetectorFactory.h"

class GeoPhysVol;

class VDetectorNode : public GeoVDetectorFactory,
  public GeoVDetectorManager
{
 public:
  VDetectorNode();
  virtual ~VDetectorNode();

  //	Access to the Detector Manager.  VDetectorNode returns
  //	itself.
  virtual const GeoVDetectorManager * getDetectorManager() const;

 private:
  VDetectorNode(const VDetectorNode &right);
  const VDetectorNode & operator=(const VDetectorNode &right);

};

#endif


