/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCALOREC_CALOCELL2CLUSTERMAPPER_H
#define TRIGCALOREC_CALOCELL2CLUSTERMAPPER_H

/**
 * modified from 
OB *
 * @class CaloCell2ClusterMapper
 * @version \$Id: TrigCaloCell2ClusterMapper.h 475753 2012-01-09 21:31:14Z chapleau $
 * @author Sven Menke <menke@mppmu.mpg.de>
 * @brief create a map of CaloCluster objects each CaloCell belongs to.
 *
 * This class is an Algorithm which creates a map pointing for each
 * CaloCell to all CaloCluster objects from a given
 * CaloClusterContainer it belongs to.  Only the member cells of each
 * CaloCluster in the CaloClusterContainer are considered in the map.
 * */

class CaloDetDescrManager; 
class CaloCell_ID;

#include "AthenaKernel/IOVSvcDefs.h"

#include "TrigInterfaces/FexAlgo.h"

class TrigCaloCell2ClusterMapper : public HLT::FexAlgo {

 public:

  TrigCaloCell2ClusterMapper(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigCaloCell2ClusterMapper();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute( const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE );

  // Callback added to handle Data-driven GeoModel initialisation
  virtual StatusCode geoInit(IOVSVC_CALLBACK_ARGS);


 protected:

  const CaloDetDescrManager* m_calo_dd_man; 
  
  const CaloCell_ID* m_calo_id;
  
  /**
   * @brief Name of the CaloCell2ClusterMap in StoreGate.
   * the map can be retrieved with this key from StoreGate.  */
  std::string     m_mapOutputName; 


};
#endif // TRIGCALOREC_CALOCELL2CLUSTERMAPPER_H



