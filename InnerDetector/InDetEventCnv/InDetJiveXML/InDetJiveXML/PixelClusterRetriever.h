/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_PIXELCLUSTERRETRIEVER_H
#define JIVEXML_PIXELCLUSTERRETRIEVER_H

#include "JiveXML/IDataRetriever.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetJiveXML/IInDetGeoModelTool.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"

namespace JiveXML
{

  
  /**
   * @class PixelClusterRetriever
   * @brief Retrieves all @c InDet::SiCluster objects
   *
   *  - @b Properties
   *    - <em>PixelClusters</em><tt> = 'PixelClusters'</tt>: @copydoc m_PixelClusterCollName
   *    - <em>PixelTruthMap</em><tt> = 'PRD_MultiTruthPixel'</tt>: @copydoc m_PixelTruthMapName
   *    .
   *
   *  - @b Retrieved @b Data
   *    - <em>x0,y0,z0</em> : global position of the cluster
   *    - <em>widthx</em> : cluster width in local x direction
   *    - <em>widthy</em> : cluster width in local y direction
   *    - <em>id</em> : id of the cluster
   *    - <em>eloss</em> : energy loss
   *    - <em>phiModule,etaModule</em> @f$\eta@f$ and @f$\phi@f$ of module in detector coordinates
   *    - <em>numBarcodes</em> : number of associated truth tracks
   *    - <em>barcodes</em> : barcodes of the associated truth tracks
   *    .
   */
  class PixelClusterRetriever : virtual public IDataRetriever,
                                     public AthAlgTool {
    
    public:

      /// Standard Constructor
      PixelClusterRetriever(const std::string& type,const std::string& name,const IInterface* parent);

      /// Retrieve all the data
      virtual StatusCode retrieve(ToolHandle<IFormatTool> &FormatTool); 

      /// Return the name of the data type
      virtual std::string dataTypeName() const { return m_typeName; }

      /// initialize only geo model tool
      virtual StatusCode initialize();

    private:

      /// The data type that is generated by this retriever
      const std::string m_typeName;

      /// A tool handle to the geo model tool
      const ToolHandle<IInDetGeoModelTool> m_geo
         {this, "GeoModelTool","JiveXML::InDetGeoModelTool/InDetGeoModelTool",""};

      /// The StoreGate key for the SiClusterCollection to retrieve
      std::string m_PixelClusterCollName;
      /// The StoreGate key for the PRD MultiTruthMap with the track associations
      std::string m_PixelTruthMapName;

      SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey{this, "PixelDetEleCollKey", "PixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};
    };
}
#endif
