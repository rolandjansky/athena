/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PDMAKER_PIXELRDOCLUSTERASSOCIATIONTOOL_H
#define TRACKD3PDMAKER_PIXELRDOCLUSTERASSOCIATIONTOOL_H
 
#include "D3PDMakerUtils/SingleAssociationTool.h"
 
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include <vector>

#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "Identifier/Identifier.h"
#include "AthContainers/DataVector.h"

#include "AthenaKernel/CLASS_DEF.h"
#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/InDetRawDataCollection.h"
#include "InDetRawData/PixelRDO_Container.h"
#include "InDetRawData/PixelRDO_Collection.h"
#include "InDetRawData/PixelRDORawData.h"
#include "InDetRawData/PixelRDO_Collection.h" // Needed to pick up CLID.

#include <stdio.h>
#include <iostream>
#include <string>


class PixelID;


namespace InDet {
  class PixelCluster;
  class SiCluster;
}


namespace D3PD {
 
  class PixelRDOClusterAssociationTool
    : public SingleAssociationTool<PixelRDORawData, InDet::PixelCluster>
    {
    public:
      typedef SingleAssociationTool<PixelRDORawData, InDet::PixelCluster> Base;
 
      PixelRDOClusterAssociationTool (const std::string& type,
				      const std::string& name,
				      const IInterface* parent);
 
      virtual StatusCode initialize();
      virtual StatusCode book();


      virtual const InDet::PixelCluster* get (const PixelRDORawData& rdo);

    private:

      int tempmoduleid( Identifier id );
      const InDet::PixelClusterContainer *m_cl;
      std::map< int, std::vector< const InDet::PixelCluster* > > m_rdoClusterMap; 
      int m_eventNumber;
      const PixelID* m_pixelId;


      // StoreGate keys
      std::string m_clusterContainer;
 
      // NTuple variables 
 

    }; // class PixelRDOClusterAssociationTool
 
} // namespace D3PD
 
#endif // not TRACKD3PDMAKER_PIXELRDOCLUSTERASSOCIATIONTOOL_H

