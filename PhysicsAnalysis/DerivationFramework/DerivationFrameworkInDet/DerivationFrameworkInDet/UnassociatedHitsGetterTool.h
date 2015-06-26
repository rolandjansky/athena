/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_UNASSOCIATEDHITSGETTERTOOL_H
#define DERIVATIONFRAMEWORK_UNASSOCIATEDHITSGETTERTOOL_H

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include <string>

#include "DerivationFrameworkInDet/IUnassociatedHitsGetterTool.h"

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
  virtual const MinBiasPRDAssociation* get (bool allowMissing = false);

  /**
     delete the pointer created by get
   */
  virtual void releaseObject (const MinBiasPRDAssociation* p);

private:


  /* Pointer to storegate **/
  StoreGateSvc* m_storeGate;

  /* StoreGate keys **/
  std::string m_trackCollection;
  std::string m_pixelClusterContainer;
  std::string m_SCTClusterContainer;
  std::string m_TRTDriftCircleContainer;

  /* PRD association tool **/
  ToolHandle<Trk::IPRD_AssociationTool> m_assoTool;

  bool m_firstTime;
  bool m_disabled;

}; // class UnassociatedHitsGetterTool

} // namespace DerivationFramework


#endif // not DERIVATIONFRAMEWORK_UNASSOCIATEDHITSGETTERTOOL_H
