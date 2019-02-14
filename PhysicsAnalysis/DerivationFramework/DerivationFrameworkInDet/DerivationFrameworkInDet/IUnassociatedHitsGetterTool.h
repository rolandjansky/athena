/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_IUNASSOCIATEDHITSGETTERTOOL_H
#define DERIVATIONFRAMEWORK_IUNASSOCIATEDHITSGETTERTOOL_H

#include "GaudiKernel/IAlgTool.h"

#include "DerivationFrameworkInDet/MinBiasPRDAssociation.h"


static const InterfaceID IID_IUnassociatedHitsGetterTool("IUnassociatedHitsGetterTool", 1 , 0);

class IUnassociatedHitsGetterTool : virtual public IAlgTool
{
 public:
  
  static const InterfaceID& interfaceID() { return IID_IUnassociatedHitsGetterTool; }

  /**
   * @brief Type-safe wrapper for @c get.
   * @param allowMissing If true, then we should not generate errors
   *        if the requested object is missing.
   *
   * Return the object as a pointer to @c MinBiasPRDAssociation.
   * Return 0 if the get fails or if the pointer can't be converted.
   *
   * This is implemented in terms of @c getTypeinfo().
   */
  virtual const MinBiasPRDAssociation* get (bool allowMissing = false) const = 0;

  /**
   * @brief Type-safe wrapper for @c releaseObjectUntyped.
   * @param p The object to release.
   *
   * Call this when you are done with the object returned by
   * @c get().  The default implementation is a no-op,
   * but if the getter dynamically allocated the object which
   * it returned, this gives it a chance to free it.
   *
   * This is implemented in terms of @c releaseObjectTypeinfo().
   */
  virtual void releaseObject (const MinBiasPRDAssociation* p) const = 0;

  
};
#endif //DERIVATIONFRAMEWORK_IUNASSOCIATEDHITSGETTERTOOL_H
