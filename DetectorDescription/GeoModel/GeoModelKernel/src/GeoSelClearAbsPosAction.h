/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOMODELKERNEL_GEOSELCLEARABSPOSACTION_H
#define GEOMODELKERNEL_GEOSELCLEARABSPOSACTION_H

/**
 * @class GeoSelClearAbsPosAction
 *
 * @brief This action class has been introduced in order to speed up GeoAlignableTransform::setDelta(),
 * which is heavily used by alignment callbacks.
 *
 * The action takes a pointer to GeoAlignableTransform
 * and clears position caches only for those daughters of given
 * physical volume, which are affected by this alignable transform.
 */

#include "GeoModelKernel/GeoNodeAction.h"

class GeoAlignableTransform;

class GeoSelClearAbsPosAction:public GeoNodeAction
{
 public:
  GeoSelClearAbsPosAction(const GeoAlignableTransform* alignTrans);
  virtual ~GeoSelClearAbsPosAction();

 private:

  // Handles a Transform.
  virtual void handleTransform (const GeoTransform *xform);
  
  // Handles a physical volume.
  virtual void handlePhysVol (const GeoPhysVol *vol);
  
  // Handles a physical volume.
  virtual void handleFullPhysVol (const GeoFullPhysVol *vol);
  
 private:
  GeoSelClearAbsPosAction(const GeoSelClearAbsPosAction &right);
  GeoSelClearAbsPosAction & operator=(const GeoSelClearAbsPosAction &right);
  
  const GeoAlignableTransform* m_alignTrans;
  bool m_needToClearCache;
};

#endif
