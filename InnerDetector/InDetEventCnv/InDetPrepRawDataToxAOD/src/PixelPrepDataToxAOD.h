/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelPrepDataToxAOD.h
//   Header file for class PixelPrepDataToxAOD
///////////////////////////////////////////////////////////////////

#ifndef PIXELPREPDATATOXAOD_H
#define PIXELPREPDATATOXAOD_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "InDetSimEvent/SiHitCollection.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "xAODTracking/TrackMeasurementValidation.h"
#include "xAODTracking/TrackMeasurementValidationContainer.h"

#include "PixelConditionsData/PixelModuleData.h"
#include "InDetCondTools/ISiLorentzAngleTool.h"
#include "PixelConditionsServices/IPixelByteStreamErrorsSvc.h"
#include "PixelConditionsServices/IPixelCalibSvc.h"

#include <string>

class PixelID;
class SiHit;
class InDetSimDataCollection;

class IdentifierHash;

namespace InDet
{
  class PixelCluster;
}

namespace InDetDD
{
  class SiCellId; 
}



class PixelPrepDataToxAOD : public AthAlgorithm  {

public:
  // Constructor with parameters:
  PixelPrepDataToxAOD(const std::string &name,ISvcLocator *pSvcLocator);

  // Basic algorithm methods:
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:

  std::vector< std::vector< int > >  addSDOInformation( xAOD::TrackMeasurementValidation* xprd,
							const InDet::PixelCluster* prd,
							const InDetSimDataCollection& sdoCollection ) const;


  void  addSiHitInformation( xAOD::TrackMeasurementValidation* xprd, 
                             const InDet::PixelCluster* prd,
                             const std::vector<SiHit> & matchingHits ) const;
  
  std::vector<SiHit>  findAllHitsCompatibleWithCluster(const InDet::PixelCluster* prd,
                                                       const SiHitCollection& collection,
						       std::vector< std::vector< int > > & trkBCs) const;


  void  addNNTruthInfo( xAOD::TrackMeasurementValidation* xprd,
                        const InDet::PixelCluster* prd, 
                        const std::vector<SiHit> & matchingHits ) const;

  void  addNNInformation( xAOD::TrackMeasurementValidation* xprd, 
                         const InDet::PixelCluster* pixelCluster, 
                         const unsigned int SizeX, 
                         const unsigned int SizeY ) const;

  void  addRdoInformation( xAOD::TrackMeasurementValidation* xprd, 
                         const InDet::PixelCluster* pixelCluster) const;



   InDetDD::SiCellId getCellIdWeightedPosition(  const InDet::PixelCluster* pixelCluster,
                                                 int *rrowMin = 0,
                                                 int *rrowMax = 0,
                                                 int *rcolMin = 0,
                                                 int *rcolMax = 0 ) const;


  const PixelID *m_PixelHelper;

  bool  m_useTruthInfo;
  bool  m_writeSDOs;
  bool  m_writeSiHits;
  bool  m_writeNNinformation;
  bool  m_writeRDOinformation;
  bool m_useSiHitsGeometryMatching;

  ServiceHandle<IPixelCalibSvc> m_calibSvc;

  SG::ReadCondHandleKey<PixelModuleData> m_condDCSStateKey{this, "PixelDCSStateCondData", "PixelDCSStateCondData", "Pixel FSM state key"};
  SG::ReadCondHandleKey<PixelModuleData> m_condDCSStatusKey{this, "PixelDCSStatusCondData", "PixelDCSStatusCondData", "Pixel FSM status key"};

  SG::ReadCondHandleKey<PixelModuleData> m_readKeyTemp{this, "ReadKeyeTemp", "PixelDCSTempCondData",         "Key of input sensor temperature conditions folder"};
  SG::ReadCondHandleKey<PixelModuleData> m_readKeyHV  {this, "ReadKeyHV",    "PixelDCSHVCondData",           "Key of input bias voltage conditions folder"};

  ServiceHandle<IPixelByteStreamErrorsSvc> m_pixelBSErrorsSvc;
  ToolHandle<ISiLorentzAngleTool> m_lorentzAngleTool{this, "LorentzAngleTool", "SiLorentzAngleTool", "Tool to retreive Lorentz angle"};

  // -- Private members   
  bool m_firstEventWarnings;
  bool m_need_sihits;

  SG::ReadHandleKey<InDet::PixelClusterContainer> m_clustercontainer_key;
  SG::ReadHandleKey<SiHitCollection> m_sihitContainer_key;
  SG::ReadHandleKey<InDetSimDataCollection> m_SDOcontainer_key;
  SG::ReadHandleKey<PRD_MultiTruthCollection> m_multiTruth_key;

  SG::WriteHandleKey<xAOD::TrackMeasurementValidationContainer> m_write_xaod;
  SG::WriteHandleKey<std::vector<unsigned int>> m_write_offsets;
};


#endif 
