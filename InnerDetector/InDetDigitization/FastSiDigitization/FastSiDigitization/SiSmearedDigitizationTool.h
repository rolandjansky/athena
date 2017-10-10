/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiSmearedDigitizationTool.h
//   Header file for class SiSmearedDigitizationTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Top algorithm class for Pixel fast smeared digitization
///////////////////////////////////////////////////////////////////

#ifndef FASTSIDIGITIZATION_SISMEAREDDIGITIZATIONTOOL_H
#define FASTSIDIGITIZATION_SISMEAREDDIGITIZATIONTOOL_H

#include "PileUpTools/PileUpToolBase.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/AlgTool.h"
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "SiPropertiesSvc/ISiPropertiesSvc.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "InDetSimData/InDetSimDataCollection.h"
#include "FastSiDigitization/ISiSmearedDigitizationTool.h"

#include "HitManagement/TimedHitCollection.h"
#include "InDetSimEvent/SiHit.h"
#include "InDetSimEvent/SiHitCollection.h"

#include "InDetPrepRawData/PixelClusterContainer.h"

#include "TrkTruthData/PRD_MultiTruthCollection.h"

#include "GaudiKernel/ITHistSvc.h"
#include "EventPrimitives/EventPrimitives.h"

#include <tuple>
typedef std::tuple< Amg::Vector2D, InDet::SiWidth, Amg::MatrixX * > ClusterInfo;

class PixelID;

namespace InDetDD{class SiDetectorElement;}
namespace CLHEP {class HepRandomEngine;}

namespace InDet {
  class PixelCluster;
  class IPixelClusteringTool;
  class SiCluster;
}

namespace InDetDD
{
  class PixelDetectorManager;
}

class SiSmearedDigitizationTool : virtual public PileUpToolBase, 
  virtual public ISiSmearedDigitizationTool
{

public:

  /** Constructor with parameters */
  SiSmearedDigitizationTool( const std::string& type,
			    const std::string& name,
			    const IInterface* parent );
  StatusCode initialize();
  StatusCode prepareEvent(unsigned int);
  StatusCode processBunchXing( int bunchXing,
			       PileUpEventInfo::SubEvent::const_iterator bSubEvents,
			       PileUpEventInfo::SubEvent::const_iterator eSubEvents ); 
  StatusCode processAllSubEvents();
  StatusCode mergeEvent();
  
  typedef std::multimap<IdentifierHash, const InDet::PixelCluster*> Pixel_detElement_RIO_map;
     
  StatusCode digitize();
  StatusCode createAndStoreRIOs();
  StatusCode retrieveTruth();
  StatusCode finalize();

  template<typename CLUSTER> 
    StatusCode FillTruthMap(PRD_MultiTruthCollection*, CLUSTER*, TimedHitPtr<SiHit>); 
  
 private:
  TimedHitCollection<SiHit>* m_thpcsi;      
  ServiceHandle <IAtRndmGenSvc> m_rndmSvc;             //!< Random number service
  
  const InDetDD::PixelDetectorManager* m_manager_pix;
  
  const PixelID* m_pixel_ID;                             //!< Handle to the ID helper
  
  CLHEP::HepRandomEngine*    m_randomEngine;
  std::string                m_randomEngineName;         //!< Name of the random number stream
  
  InDet::PixelClusterContainer*  m_pixelClusterContainer;               //!< the PixelClusterContainer
  
  ServiceHandle<PileUpMergeSvc> m_mergeSvc;      /**< PileUp Merge service */
    
  PRD_MultiTruthCollection* m_prdTruthCollection;
  std::string               m_prdTruthName;

  SiHitCollection* m_simHitColl;
  std::string      m_inputObjectName;     //! name of the sub event  hit collections. 
  
  std::vector<std::pair<unsigned int, int> > m_seen;
  std::list<SiHitCollection*> m_siHitCollList;
  
  Pixel_detElement_RIO_map* m_pixelClusterMap;
//       
  double m_sigmaX; // mm
  double m_sigmaY; // mm
  
  std::string                           m_siClustersName;
  int  m_nClusters;
  int  m_nTruthClusters;
  
  SiSmearedDigitizationTool();
  SiSmearedDigitizationTool(const SiSmearedDigitizationTool&);
  
  SiSmearedDigitizationTool& operator=(const SiSmearedDigitizationTool&);
   
   
};

#endif // FASTSIDIGITIZATION_SISMEAREDDIGITIZATIONTOOL_H
