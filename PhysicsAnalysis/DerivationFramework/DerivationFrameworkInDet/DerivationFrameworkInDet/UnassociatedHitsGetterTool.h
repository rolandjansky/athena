/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_UNASSOCIATEDHITSGETTERTOOL_H
#define DERIVATIONFRAMEWORK_UNASSOCIATEDHITSGETTERTOOL_H

#include "DerivationFrameworkInDet/IUnassociatedHitsGetterTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"

#include "GaudiKernel/ToolHandle.h"

#include "TrkTrack/TrackCollection.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"

#include "TrkToolInterfaces/IPRDtoTrackMapTool.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include <atomic>
#include <string>

class MinBiasPRDAssociation;

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

  /* StoreGate keys **/
  SG::ReadHandleKey<TrackCollection> m_trackCollection
     { this, "TrackCollection", "Tracks", "" };
  SG::ReadHandleKey<InDet::PixelClusterContainer>    m_pixelClusterContainer
     { this, "PixelClusters", "PixelClusters", ""};
  SG::ReadHandleKey<InDet::SCT_ClusterContainer>     m_SCTClusterContainer
     { this, "SCTClusterContainer", "SCT_Clusters", ""};
  SG::ReadHandleKey<InDet::TRT_DriftCircleContainer> m_TRTDriftCircleContainer
     { this, "TRTDriftCircleContainer", "TRT_DriftCircles", ""};

  // For P->T converter of SCT_Clusters
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};

  ToolHandle<Trk::IPRDtoTrackMapTool>         m_assoTool
     {this, "AssociationTool", "InDet::InDetPRDtoTrackMapToolGangedPixels" };
  SG::ReadHandleKey<Trk::PRDtoTrackMap>       m_prdToTrackMapKey
     {this, "PRDtoTrackMap", "",""};
}; // class UnassociatedHitsGetterTool

} // namespace DerivationFramework


#endif // not DERIVATIONFRAMEWORK_UNASSOCIATEDHITSGETTERTOOL_H
