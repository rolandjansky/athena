/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
	  const InDetDD::SiDetectorElement* element) const = 0;
protected:
     // Determines if a pixel cell (whose identifier is the first argument) is 
     // a ganged pixel. If this is the case, the last argument assumes the 
     // value of the identifier of the cell it is ganged with. 
     // The second argument is the pixel module the hit belongs to.
     bool isGanged(const Identifier& rdoID,
		   const InDetDD::SiDetectorElement* element,
		   Identifier & gangedID) const;

     bool isGoodRDO(const IdentifierHash& moduleID, const Identifier& rdoID) const;
	  
     ToolHandle<IInDetConditionsTool> m_summaryTool{this, "PixelConditionsSummaryTool", "PixelConditionsSummaryTool", "Tool to retrieve Pixel Conditions summary"};
     BooleanProperty m_useModuleMap{this, "UsePixelModuleMap", true, "Use bad modules map"};
     BooleanProperty m_addCorners{this, "AddCorners", true};

     SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey{this, "PixelDetEleCollKey", "PixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};

private:
     const InDetDD::SiDetectorElement* preClusterizationChecks(const InDetRawDataCollection<PixelRDORawData> &collection) const;

};

}
#endif // SICLUSTERIZATIONTOOL_PIXELCLUSTERINGTOOLBASE_H
