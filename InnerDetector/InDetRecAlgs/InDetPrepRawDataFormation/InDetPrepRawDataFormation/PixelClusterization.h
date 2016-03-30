/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file PixelClusterization.h
 * (c) ATLAS Detector software
 * @author D.Calvet, N.Nesvadba, T.Lari 
 * (modified s.roe, Dec 2008)
 **/

#ifndef INDETRIOMAKER_PIXELCLUSTERIZATION_H
#define INDETRIOMAKER_PIXELCLUSTERIZATION_H
//STL
#include <string>

//Gaudi
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
// Base class
#include "AthenaBaseComps/AthAlgorithm.h"

//InDet
//can't fwd declare this, needed for typedef to Pixel_RDO_Container
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "SiClusterizationTool/PixelGangedAmbiguitiesFinder.h"
#include "InDetRawData/InDetRawDataCollection.h"
#include "InDetRawData/PixelRDO_Container.h"
//



// Fwd declarations
class ISvcLocator;
class StatusCode;
class PixelRDORawData;
class PixelID;

namespace InDetDD{
  class SiDetectorManager;
}
namespace InDet {
  class IPixelClusteringTool;
  class PixelGangedAmbiguitiesFinder;
}


namespace InDet {
/**
 * Top Algorithm to clusterize the pixel RDOs
 * (InDetRawDataCollection<PixelRDORawData> -> RecInputCollection)
 * The clustering algorithm is actually a private Tool in the
 * SiClusterizationTool package
 **/
class PixelClusterization : public AthAlgorithm {
public:
  typedef InDetRawDataCollection<PixelRDORawData> COLLECTION;
  typedef InDetDD::SiDetectorManager SiDetectorManager;

  /// Constructor with parameters:
  PixelClusterization(const std::string &name,ISvcLocator *pSvcLocator);

  //@name Usual algorithm methods 
  //@{
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();
  /**    @name Disallow default instantiation, copy, assignment */
  //@{
  //@}
  PixelClusterization() = delete;
  PixelClusterization(const PixelClusterization&) = delete;
  PixelClusterization &operator=(const PixelClusterization&) = delete;
   //@}
                              
private:
  ToolHandle< IPixelClusteringTool > m_clusteringTool;
  /// class to find out which clusters shares ganged pixels
  ToolHandle< PixelGangedAmbiguitiesFinder > m_gangedAmbiguitiesFinder; 
  SG::ReadHandle<PixelRDO_Container> m_rdoContainer;
//  std::string m_dataObjectName;	           //!< RDO container name in StoreGate
//  std::string m_elementsObjectName;        //!< element collection name in StoreGate
  std::string m_managerName; 		           //!< detector manager name in StoreGate
//  std::string m_clustersName; //REMOVE LATER
//  int m_page; 			//REMOVE LATER	                     //!< page number for hash function
  const PixelID* m_idHelper;
  SG::WriteHandle<PixelClusterContainer> m_clusterContainer;
  SG::WriteHandle<PixelGangedClusterAmbiguities> m_ambiguitiesMap;
  const SiDetectorManager* m_manager;
};

}//end of ns

#endif // INDETRIOMAKER_PIXELCLUSTERIZATION_H
