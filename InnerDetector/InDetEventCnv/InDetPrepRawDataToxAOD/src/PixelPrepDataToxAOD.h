/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "xAODTracking/TrackMeasurementValidation.h"


#include <string>

class PixelID;
class SiHit;
class InDetSimDataCollection;


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

  void  addSDOInformation( xAOD::TrackMeasurementValidation* xprd,
                           const InDet::PixelCluster* prd,
                           const InDetSimDataCollection* sdoCollection ) const;


  void  addSiHitInformation( xAOD::TrackMeasurementValidation* xprd, 
                             const InDet::PixelCluster* prd,
                             const SiHitCollection* collection) const;
  
  std::vector<SiHit>  findAllHitsCompatibleWithCluster(const InDet::PixelCluster* prd,
                                                       const SiHitCollection* collection) const;


  void  addNNTruthInfo( xAOD::TrackMeasurementValidation* xprd,
                        const InDet::PixelCluster* prd, 
                        const SiHitCollection* collection ) const;

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
  std::string  m_clustercontainer;
  std::string  m_SDOcontainer;
  std::string  m_sihitContainer;
  std::string  m_multiTruth;

  bool  m_useTruthInfo;
  bool  m_writeSDOs;
  bool  m_writeSiHits;
  bool  m_writeNNinformation;
  bool  m_writeRDOinformation;

   

};


#endif 
