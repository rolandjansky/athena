/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PDMAKER_SCTRDOCLUSTERASSOCIATIONTOOL_H
#define TRACKD3PDMAKER_SCTRDOCLUSTERASSOCIATIONTOOL_H
 
#include "D3PDMakerUtils/SingleAssociationTool.h"
 
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include <vector>

#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "Identifier/Identifier.h"
#include "DataModel/DataVector.h"

#include "CLIDSvc/CLASS_DEF.h"
#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/InDetRawDataCollection.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetRawData/SCT_RDO_Collection.h"
#include "InDetRawData/SCT_RDORawData.h"
#include "InDetRawData/SCT_RDO_Collection.h" // Needed to pick up CLID.

#include <stdio.h>
#include <iostream>
#include <string>


class SCT_ID;


namespace InDet {
  class SCT_Cluster;
  class SiCluster;
}


namespace D3PD {
 
  class SCTRDOClusterAssociationTool
    : public SingleAssociationTool<SCT_RDORawData, InDet::SCT_Cluster>
    {
    public:
      typedef SingleAssociationTool<SCT_RDORawData, InDet::SCT_Cluster> Base;
 
      SCTRDOClusterAssociationTool (const std::string& type,
				      const std::string& name,
				      const IInterface* parent);
 
      virtual StatusCode initialize();
      virtual StatusCode book();


      virtual const InDet::SCT_Cluster* get (const SCT_RDORawData& rdo);

    private:

      int tempmoduleid( Identifier id );
      const InDet::SCT_ClusterContainer *m_cl;
      std::map< int, std::vector< const InDet::SCT_Cluster* > > m_rdoClusterMap; 
      int m_eventNumber;
      const SCT_ID* m_sctId;


      // StoreGate keys
      std::string m_clusterContainer;
 
      // NTuple variables 
 

    }; // class SCTRDOClusterAssociationTool
 
} // namespace D3PD
 
#endif // not TRACKD3PDMAKER_SCTRDOCLUSTERASSOCIATIONTOOL_H

