/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelClusteringToolBase.cxx
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "SiClusterizationTool/PixelClusteringToolBase.h"
#include "SiClusterizationTool/ClusterMakerTool.h"
#include "InDetIdentifier/PixelID.h"
#include "TrkSurfaces/RectangleBounds.h"

namespace InDet
{

  PixelClusteringToolBase::PixelClusteringToolBase(const std::string &type,
      const std::string &name,
      const IInterface *parent) :
      base_class(type,name,parent)
  {
  }

  StatusCode PixelClusteringToolBase::initialize()
  {
    ATH_MSG_INFO("initialize()");

    ATH_CHECK(m_pixelDetEleCollKey.initialize());

    ATH_CHECK( m_summaryTool.retrieve( DisableTool{!m_useModuleMap || (!m_pixelDetElStatus.empty() && !VALIDATE_STATUS_ARRAY_ACTIVATED) }));

    ATH_CHECK( m_pixelDetElStatus.initialize( !m_pixelDetElStatus.empty()) );
    if (!m_pixelDetElStatus.empty()) {
       ATH_CHECK( m_pixelReadout.retrieve() );
    }
    ATH_CHECK( detStore()->retrieve(m_pixelId, "PixelID") );

    return StatusCode::SUCCESS;
  }

  StatusCode PixelClusteringToolBase::finalize()
  {
    return StatusCode::SUCCESS;
  }

  //--------------------------------------------------------------------------
  // Determines if a pixel cell (whose identifier is the first argument) is 
  // a ganged pixel. If this is the case, the last argument assumes the 
  // value of the identifier of the cell it is ganged with. 
  // The second argument is the pixel module the hit belongs to.  
  bool PixelClusteringToolBase::isGanged(const Identifier& rdoID,
                                  const InDetDD::SiDetectorElement* element,  
                                  Identifier& gangedID) 
  {
    // returns true for ganged pixels. If true returns Identifier of pixel
    InDetDD::SiCellId cellID = element->cellIdFromIdentifier (rdoID);
    if (element->numberOfConnectedCells (cellID) > 1) {
      InDetDD::SiCellId gangedCellID = element->connectedCell (cellID,1);
      gangedID = element->identifierFromCellId (gangedCellID);
      return true;
    } else {
      gangedID = Identifier();
      return false;
    }
  }

  const InDetDD::SiDetectorElement* PixelClusteringToolBase::preClusterizationChecks(const InDetRawDataCollection<PixelRDORawData> &collection,
                                                                                     const InDet::SiDetectorElementStatus *pixelDetElStatus) const
  {
    const unsigned int RDO_size = collection.size();
    if ( RDO_size==0) {
        ATH_MSG_DEBUG("Empty RDO collection");
        return nullptr;
    }

    IdentifierHash idHash = collection.identifyHash();

    // If module is bad, do not create a cluster collection
    VALIDATE_STATUS_ARRAY(m_useModuleMap && pixelDetElStatus, pixelDetElStatus->isGood(idHash), m_summaryTool->isGood(idHash));
    if (m_useModuleMap && ( pixelDetElStatus ? !pixelDetElStatus->isGood(idHash) : !(m_summaryTool->isGood(idHash))))
      return nullptr;

    SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> pixelDetEleHandle(m_pixelDetEleCollKey);
    const InDetDD::SiDetectorElementCollection* pixelDetEle(*pixelDetEleHandle);
    if (not pixelDetEleHandle.isValid() or pixelDetEle == nullptr) {
      ATH_MSG_FATAL(m_pixelDetEleCollKey.fullKey() << " is not available.");
      return nullptr;
    }

    const InDetDD::SiDetectorElement* element = pixelDetEle->getDetectorElement(idHash);
    const Trk::RectangleBounds *mybounds = dynamic_cast<const Trk::RectangleBounds *>(&element->surface().bounds());
    if (not mybounds) {
      ATH_MSG_ERROR("Dynamic cast failed at "<<__LINE__<<" of PixelClusteringToolBase.cxx.");
      return nullptr;
    }
 
    return element;
  }

  PixelClusterCollection* PixelClusteringToolBase::clusterize(const InDetRawDataCollection<PixelRDORawData> &collection, const PixelID& pixelID) const
  {
    const InDet::SiDetectorElementStatus *pixelDetElStatus= (!m_pixelDetElStatus.empty() ? getPixelDetElStatus().cptr() : nullptr);
    const InDetDD::SiDetectorElement* element = preClusterizationChecks(collection, pixelDetElStatus);
    if (not element)
      return nullptr;

    return doClusterization(collection, pixelID, element, pixelDetElStatus);
  }


}
