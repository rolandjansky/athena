/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelClusteringToolBase.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef SICLUSTERIZATIONTOOL_PIXELCLUSTERINGTOOLBASE_H
#define SICLUSTERIZATIONTOOL_PIXELCLUSTERINGTOOLBASE_H

//#include "GaudiKernel/AlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "SiClusterizationTool/IPixelClusteringTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "InDetConditionsSummaryService/IInDetConditionsTool.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/SiDetectorElementStatus.h"
#include "InDetIdentifier/PixelID.h"
#include "PixelReadoutGeometry/IPixelReadoutManager.h"

class IInDetConditionsTool;
template <class T> class ServiceHandle;

namespace InDet {

class ClusterMakerTool;

class PixelClusteringToolBase : public extends<AthAlgTool, IPixelClusteringTool> 
{

public:
     PixelClusteringToolBase
           (const std::string& type,
	    const std::string& name,
	    const IInterface* parent);
     
     virtual ~PixelClusteringToolBase() = default;
     virtual StatusCode initialize();
     virtual StatusCode finalize();

     virtual PixelClusterCollection* clusterize(const InDetRawDataCollection<PixelRDORawData> &collection, const PixelID& pixelID) const final;

     virtual PixelClusterCollection *doClusterization(
	  const InDetRawDataCollection<PixelRDORawData>& collection,
	  const PixelID& pixelID,
	  const InDetDD::SiDetectorElement* element,
          const InDet::SiDetectorElementStatus *pixelDetElStatus) const = 0;
protected:
     // Determines if a pixel cell (whose identifier is the first argument) is 
     // a ganged pixel. If this is the case, the last argument assumes the 
     // value of the identifier of the cell it is ganged with. 
     // The second argument is the pixel module the hit belongs to.
     static bool isGanged(const Identifier& rdoID,
		   const InDetDD::SiDetectorElement* element,
		   Identifier & gangedID) ;

     bool isGoodRDO(const InDet::SiDetectorElementStatus *pixelDetElStatus, const IdentifierHash& moduleHash, const Identifier& rdoID) const {
        VALIDATE_STATUS_ARRAY(m_useModuleMap && pixelDetElStatus, pixelDetElStatus->isChipGood(moduleHash,m_pixelReadout->getFE(rdoID, m_pixelId->wafer_id(rdoID))), m_summaryTool->isGood(moduleHash, rdoID));
        return !m_useModuleMap || ( pixelDetElStatus
                                    ? pixelDetElStatus->isChipGood(moduleHash,
                                                                 m_pixelReadout->getFE(rdoID, m_pixelId->wafer_id(rdoID)))
                                    : m_summaryTool->isGood(moduleHash, rdoID));
     }

     ToolHandle<IInDetConditionsTool> m_summaryTool{this, "PixelConditionsSummaryTool", "PixelConditionsSummaryTool", "Tool to retrieve Pixel Conditions summary"};

    /** @brief Optional read handle to get status data to test whether a pixel detector element is good.
     * If set to e.g. PixelDetectorElementStatus the event data will be used instead of the pixel conditions summary tool.
     */
     SG::ReadHandleKey<InDet::SiDetectorElementStatus> m_pixelDetElStatus
        {this, "PixelDetElStatus", "" , "Key of SiDetectorElementStatus for Pixel"};

     ServiceHandle<InDetDD::IPixelReadoutManager> m_pixelReadout
        {this, "PixelReadoutManager", "PixelReadoutManager", "Pixel readout manager" };
     const PixelID* m_pixelId = nullptr;

     BooleanProperty m_useModuleMap{this, "UsePixelModuleMap", true, "Use bad modules map"};
     BooleanProperty m_addCorners{this, "AddCorners", true};

     SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey{this, "PixelDetEleCollKey", "PixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};

     SG::ReadHandle<InDet::SiDetectorElementStatus> getPixelDetElStatus() const {
        SG::ReadHandle<InDet::SiDetectorElementStatus> pixelDetElStatus;
        if (!m_pixelDetElStatus.empty()) {
           pixelDetElStatus = SG::ReadHandle<InDet::SiDetectorElementStatus>(m_pixelDetElStatus);
           if (!pixelDetElStatus.isValid()) {
              std::stringstream msg;
              msg << "Failed to get " << m_pixelDetElStatus.key() << " from StoreGate in " << name();
              throw std::runtime_error(msg.str());
           }
        }
        return pixelDetElStatus;
     }

private:
     const InDetDD::SiDetectorElement* preClusterizationChecks(const InDetRawDataCollection<PixelRDORawData> &collection,
                                                               const InDet::SiDetectorElementStatus *pixelDetElStatus) const;

};

}
#endif // SICLUSTERIZATIONTOOL_PIXELCLUSTERINGTOOLBASE_H
