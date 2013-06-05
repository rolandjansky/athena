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
#include "GaudiKernel/IIncidentListener.h"
// Base class
#include "AthenaBaseComps/AthAlgorithm.h"

//InDet
//can't fwd declare this, needed for typedef to Pixel_RDO_Container
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetRawData/InDetRawDataCollection.h"
//



// Fwd declarations
class ISvcLocator;
class StatusCode;
class Incident;
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
class PixelClusterization : public AthAlgorithm, 
                            public IIncidentListener {
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
   //@}
                              
  /// Incident listener method re-declared
  virtual void handle( const Incident& incident );

private:
  /**    @name Disallow default instantiation, copy, assignment */
  //@{
  PixelClusterization();
  PixelClusterization(const PixelClusterization&);
  PixelClusterization &operator=(const PixelClusterization&);
  //@}
                              
  ToolHandle< IPixelClusteringTool > m_clusteringTool;
  /// class to find out which clusters shares ganged pixels
  ToolHandle< PixelGangedAmbiguitiesFinder > m_gangedAmbiguitiesFinder; 
  std::string m_dataObjectName;	           //!< RDO container name in StoreGate
  std::string m_elementsObjectName;        //!< element collection name in StoreGate
  std::string m_managerName; 		           //!< detector manager name in StoreGate
  std::string m_clustersName; 
  int m_page; 				                     //!< page number for hash function
  const PixelID* m_idHelper;
  PixelClusterContainer* m_clusterContainer;
  const SiDetectorManager* m_manager;
};

}//end of ns

#endif // INDETRIOMAKER_PIXELCLUSTERIZATION_H
