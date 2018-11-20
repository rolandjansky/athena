/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
* @date Oct, 2010
* @brief setting up an association of SCT clusters to RDOs
*        represented by indices. 
*/
#ifndef SCTCLUSTERSCTRDOASSOCIATIONTOOL_H
#define SCTCLUSTERSCTRDOASSOCIATIONTOOL_H
 

#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include <vector>
#include <map>

#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetPrepRawData/SiCluster.h"
#include "Identifier/Identifier.h"
#include "AthContainers/DataVector.h"

#include "AthenaKernel/CLASS_DEF.h"
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
 
 
  template <class T>
    bool from_string(T& t, const std::string& s, std::ios_base& (*f)(std::ios_base&))
    {
      std::istringstream iss(s);
      return !(iss >> f >> t).fail();
    }


  ///
  ///@brief Associate all clusters which belong to given track.
  ///
  /// This is a multiple association tool.
  /// Given an @c InDet::SCT_Cluster object and a container of such objects,
  /// return the all objects in the container wich identified with TrackStateOnSurface 
  /// (Measurements) of the input track.
  ///

typedef D3PD::MultiAssociationTool<InDet::SCT_Cluster, SCT_RDORawData> SCTClusterSCTRDOBase;
  
class SCTClusterSCTRDOAssociationTool
   : public SCTClusterSCTRDOBase 
{
public:
 
  ///
  /// @brief Standard Gaudi tool constructor.
  /// @param type The name of the tool type.
  /// @param name The tool name.
  /// @param parent The tool's Gaudi parent.
  ///
  SCTClusterSCTRDOAssociationTool (const std::string& type,
				  const std::string& name,
				  const IInterface* parent);
 

 
  /// Standard initialize method.
  virtual StatusCode initialize();
  ///
  /// @brief Create any needed tuple variables.
  /// In principle we do not intend to get anything else then index 
  ///(=we don't create) variables
  /// but for debugging and validation puproses it is nice to have this.
  /// This is called at the start of the first event.
  ///
  virtual StatusCode book();
 
  ///
  /// @brief Start the iteration for a new association.
  /// @param p The object from which to associate.
  ///
  virtual StatusCode reset (const InDet::SCT_Cluster& p);
 
 
  ///
  /// @brief Return a pointer to the next element in the association.
  ///
  /// Return 0 when the association has been exhausted.
  ///
  virtual const SCT_RDORawData* next();
 

private:

  StatusCode invalidate(StatusCode);
  StatusCode tempmoduleid( Identifier id, int& calculatedID );


  const SCT_ID* m_sctId;

  int m_eventNumber;
  const SCT_RDO_Container* m_rdocont;
  std::map < int, std::map< Identifier, const SCT_RDORawData*> > m_globalmapOfRDOsPerModule;


  /// The input object.
  std::vector<const SCT_RDORawData* >::const_iterator m_rdoItr;
  std::vector<const SCT_RDORawData* >::const_iterator m_rdoEnd;


  std::vector<const SCT_RDORawData*> m_sctRDOForAssociation;


}; // class SCTClusterSCTRDOAssociationTool

 
} // namespace D3PD
 
#endif // not SCTCLUSTERSCTRDOASSOCIATIONTOOL_H
