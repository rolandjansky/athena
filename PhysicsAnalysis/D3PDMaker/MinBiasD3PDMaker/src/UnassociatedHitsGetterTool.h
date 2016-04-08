/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MINBIASD3PDMAKER_UNASSOCIATEDHITSGETTERTOOL_H
#define MINBIASD3PDMAKER_UNASSOCIATEDHITSGETTERTOOL_H

#include "D3PDMakerUtils/ObjGetterTool.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>

class MinBiasPRDAssociation;
namespace Trk {
  class IPRD_AssociationTool;
}

namespace D3PD {

class UnassociatedHitsGetterTool : public D3PD::ObjGetterTool<MinBiasPRDAssociation> {
public:
  typedef D3PD::ObjGetterTool<MinBiasPRDAssociation> Base;

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

} // namespace D3PD

#endif // not MINBIASD3PDMAKER_UNASSOCIATEDHITSGETTERTOOL_H
