/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
* @date Oct, 2010
* @brief setting up an association of pixel clusters to RDOs
*        represented by indices. 
*/
#ifndef PIXELCLUSTERPIXELRDOASSOCIATIONTOOL_H
#define PIXELCLUSTERPIXELRDOASSOCIATIONTOOL_H
 

#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include <vector>
#include <map>

#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetPrepRawData/SiCluster.h"
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
 
 
  template <class T>
    bool from_strings(T& t, const std::string& s, std::ios_base& (*f)(std::ios_base&))
    {
      std::istringstream iss(s);
      return !(iss >> f >> t).fail();
    }


  ///
  ///@brief Associate all clusters which belong to given track.
  ///
  /// This is a multiple association tool.
  /// Given an @c InDet::PixelCluster object and a container of such objects,
  /// return the all objects in the container wich identified with TrackStateOnSurface 
  /// (Measurements) of the input track.
  ///

typedef D3PD::MultiAssociationTool<InDet::PixelCluster, PixelRDORawData> PixelClusterPixelRDOBase;
  
class PixelClusterPixelRDOAssociationTool
   : public PixelClusterPixelRDOBase 
{
public:
 
  ///
  /// @brief Standard Gaudi tool constructor.
  /// @param type The name of the tool type.
  /// @param name The tool name.
  /// @param parent The tool's Gaudi parent.
  ///
  PixelClusterPixelRDOAssociationTool (const std::string& type,
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
  virtual StatusCode reset (const InDet::PixelCluster& p);
 
 
  ///
  /// @brief Return a pointer to the next element in the association.
  ///
  /// Return 0 when the association has been exhausted.
  ///
  virtual const PixelRDORawData* next();
 

private:

  StatusCode invalidate(StatusCode);
  StatusCode tempmoduleid( Identifier id, int& calculatedID );


  const PixelID* m_pixelId;

  int m_eventNumber;
  const PixelRDO_Container* m_rdocont;
  std::map < int, std::map< Identifier, const PixelRDORawData*> > m_globalmapOfRDOsPerModule;


  /// The input object.
  std::vector<const PixelRDORawData* >::const_iterator m_rdoItr;
  std::vector<const PixelRDORawData* >::const_iterator m_rdoEnd;


  std::vector<const PixelRDORawData*> m_pixelRDOForAssociation;


}; // class PixelClusterPixelRDOAssociationTool

 
} // namespace D3PD
 
#endif // not PIXELCLUSTERPIXELRDOASSOCIATIONTOOL_H
