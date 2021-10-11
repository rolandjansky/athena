/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

    StatusCode sc = m_summaryTool.retrieve();
    if (sc.isFailure() || !m_summaryTool) {
      ATH_MSG_WARNING(m_summaryTool.type() << " not found! ");
      if (m_useModuleMap ) {
	ATH_MSG_FATAL(m_summaryTool.type() << " is compulsory with this tool configuration");
	return StatusCode::FAILURE;
      }
    }
    else{
      ATH_MSG_INFO("Retrieved service " <<  m_summaryTool.type());
    }
   
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
                                  Identifier& gangedID) const
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

  const InDetDD::SiDetectorElement* PixelClusteringToolBase::preClusterizationChecks(const InDetRawDataCollection<PixelRDORawData> &collection) const
  {
    const unsigned int RDO_size = collection.size();
    if ( RDO_size==0) {
        ATH_MSG_DEBUG("Empty RDO collection");
        return nullptr;
    }

    IdentifierHash idHash = collection.identifyHash();

    // If module is bad, do not create a cluster collection
    if (m_useModuleMap && !(m_summaryTool->isGood(idHash))) 
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
    const InDetDD::SiDetectorElement* element = preClusterizationChecks(collection);
    if (not element)
      return nullptr;

    return doClusterization(collection, pixelID, element);
  }

  bool PixelClusteringToolBase::isGoodRDO(const IdentifierHash& moduleID, const Identifier& rdoID) const
  {
    return !m_useModuleMap || m_summaryTool->isGood(moduleID, rdoID);
  }
      

}
