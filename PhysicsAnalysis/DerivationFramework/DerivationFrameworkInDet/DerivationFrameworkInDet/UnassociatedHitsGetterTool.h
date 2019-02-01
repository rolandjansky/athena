/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_UNASSOCIATEDHITSGETTERTOOL_H
#define DERIVATIONFRAMEWORK_UNASSOCIATEDHITSGETTERTOOL_H

#include "DerivationFrameworkInDet/IUnassociatedHitsGetterTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/ToolHandle.h"

#include <atomic>
#include <string>

class MinBiasPRDAssociation;
namespace Trk {
  class IPRD_AssociationTool;
}

namespace DerivationFramework {

class UnassociatedHitsGetterTool : public AthAlgTool, virtual public IUnassociatedHitsGetterTool {
public:

  UnassociatedHitsGetterTool(const std::string& type,
		     const std::string& name,
		     const IInterface* parent);
  virtual ~UnassociatedHitsGetterTool();

  StatusCode initialize();

  /**
     get method:
      compute the number of unassociated his
      wrap the info in a MinBiasPRDAssociation object
  */
  virtual const MinBiasPRDAssociation* get (bool allowMissing = false) const;

  /**
     delete the pointer created by get
   */
  virtual void releaseObject (const MinBiasPRDAssociation* p) const;

private:


  /* Pointer to storegate **/
  StoreGateSvc* m_storeGate;

  /* StoreGate keys **/
  std::string m_trackCollection;
  std::string m_pixelClusterContainer;
  std::string m_SCTClusterContainer;
  std::string m_TRTDriftCircleContainer;

  // For P->T converter of SCT_Clusters
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};

  /* PRD association tool **/
  ToolHandle<Trk::IPRD_AssociationTool> m_assoTool;

  mutable std::atomic_bool m_firstTime;
  mutable std::atomic_bool m_disabled;

}; // class UnassociatedHitsGetterTool

} // namespace DerivationFramework


#endif // not DERIVATIONFRAMEWORK_UNASSOCIATEDHITSGETTERTOOL_H
