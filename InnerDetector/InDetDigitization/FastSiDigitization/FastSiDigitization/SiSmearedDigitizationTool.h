/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventAuxInfo.h"
#include "InDetSimData/InDetSimDataCollection.h"
#include "FastSiDigitization/ISiSmearedDigitizationTool.h"
#include <string>

#include "HitManagement/TimedHitCollection.h"
#include "InDetSimEvent/SiHit.h"
#include "InDetSimEvent/SiHitCollection.h"

#include "InDetPrepRawData/SCT_ClusterContainer.h"  // typedef
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "ISF_FatrasEvent/PlanarClusterContainer.h"
#include "ISF_FatrasEvent/PlanarCluster.h"

#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "ISF_FatrasDetDescrModel/IdHashDetElementCollection.h"
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"

#include "GaudiKernel/ITHistSvc.h"
#include "EventPrimitives/EventPrimitives.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"

#include <tuple>
typedef std::tuple< Amg::Vector2D, InDet::SiWidth, Amg::MatrixX * > ClusterInfo;

class PixelID;
class SCT_ID;
class IModuleDistortionsTool;
class TTree;
class TFile;

namespace InDetDD{class SiDetectorElement;}
namespace CLHEP {class HepRandomEngine;}

namespace InDet {
  class ClusterMakerTool;
  class PixelCluster;
  class IPixelClusteringTool;
  class SCT_Cluster;
  class SiCluster;
}

namespace InDetDD
{
  class PixelDetectorManager;
}

namespace Trk {

  class TrackingGeometry;

}

class SiSmearedDigitizationTool : virtual public PileUpToolBase,
  virtual public ISiSmearedDigitizationTool
{

public:

   /** Constructor with parameters */
  SiSmearedDigitizationTool(
                               const std::string& type,
                               const std::string& name,
                               const IInterface* parent
                               );


  StatusCode initialize();
  StatusCode prepareEvent(unsigned int);
  StatusCode processBunchXing( int bunchXing,
                               SubEventIterator bSubEvents,
                               SubEventIterator eSubEvents );
  StatusCode processAllSubEvents();
  StatusCode mergeEvent();

  typedef std::multimap<IdentifierHash, const InDet::PixelCluster*> Pixel_detElement_RIO_map;
  typedef std::multimap<IdentifierHash, const iFatras::PlanarCluster*> Planar_detElement_RIO_map;
  typedef std::multimap<IdentifierHash, const InDet::SCT_Cluster*> SCT_detElement_RIO_map;
  StatusCode mergeClusters(Pixel_detElement_RIO_map * cluster_map);
  StatusCode mergeClusters(Planar_detElement_RIO_map * cluster_map);
  StatusCode mergeClusters(SCT_detElement_RIO_map * cluster_map);

  StatusCode digitize();
  StatusCode createAndStoreRIOs();
  StatusCode retrieveTruth();
  StatusCode finalize();

  template<typename CLUSTER>
    double calculateDistance(CLUSTER * clusterA, CLUSTER * clusterB);

  template<typename CLUSTER>
    double calculateSigma(CLUSTER * clusterA, CLUSTER * clusterB);

  template<typename CLUSTER>
    ClusterInfo calculateNewCluster(CLUSTER * clusterA, CLUSTER * clusterB);

  template<typename CLUSTER>
    StatusCode FillTruthMap(PRD_MultiTruthCollection*, CLUSTER*, TimedHitPtr<SiHit>);

 private:

  TimedHitCollection<SiHit>* m_thpcsi;
  ServiceHandle <IAtRndmGenSvc> m_rndmSvc;             //!< Random number service

  const InDetDD::PixelDetectorManager* m_manager_pix;
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};

  const PixelID* m_pixel_ID;                             //!< Handle to the ID helper
  const SCT_ID* m_sct_ID;                             //!< Handle to the ID helper

  CLHEP::HepRandomEngine*           m_randomEngine;
  std::string                m_randomEngineName;         //!< Name of the random number stream

  mutable float m_pitch_X;
  mutable float m_pitch_Y;

  bool m_merge;
  double m_nSigma;

  bool m_useDiscSurface;

  InDet::PixelClusterContainer*  m_pixelClusterContainer;               //!< the PixelClusterContainer

  InDet::SCT_ClusterContainer*  m_sctClusterContainer;               //!< the SCT_ClusterContainer

  iFatras::PlanarClusterContainer*  m_planarClusterContainer;               //!< the SCT_ClusterContainer

  ServiceHandle<PileUpMergeSvc> m_mergeSvc;      /**< PileUp Merge service */
  int                       m_HardScatterSplittingMode; /**< Process all SiHit or just those from signal or background events */
  bool                      m_HardScatterSplittingSkipper;
  IntegerProperty  m_vetoThisBarcode;

  PRD_MultiTruthCollection* m_pixelPrdTruth{};
  std::string               m_prdTruthNamePixel;

  PRD_MultiTruthCollection* m_SCTPrdTruth{};
  std::string               m_prdTruthNameSCT;

  PRD_MultiTruthCollection* m_planarPrdTruth{};
  std::string               m_prdTruthNamePlanar;

  SiHitCollection* m_simHitColl{};
  std::string      m_inputObjectName;     //! name of the sub event  hit collections.

  std::list<SiHitCollection*> m_siHitCollList;

  Pixel_detElement_RIO_map* m_pixelClusterMap{};

  Planar_detElement_RIO_map* m_planarClusterMap{};

  SCT_detElement_RIO_map* m_sctClusterMap{};

  bool m_SmearPixel;

  bool m_emulateAtlas;

  iFatras::IdHashDetElementCollection*                    m_detElementMap{};
  std::string                                    m_detElementMapName;

  std::string                           m_pixel_SiClustersName;
  std::string                           m_Sct_SiClustersName;
  std::string                           m_planar_SiClustersName;

  bool       m_checkSmear;

  ITHistSvc* m_thistSvc;
  TFile*              m_outputFile;  //!< the root file
  TTree*              m_currentTree; //!< the tree to store information from pixel and SCT (before and after smearing)

  double           m_x_pixel;
  double           m_y_pixel;
  double           m_x_exit_pixel;
  double           m_y_exit_pixel;
  double           m_z_exit_pixel;
  double           m_x_entry_pixel;
  double           m_y_entry_pixel;
  double           m_z_entry_pixel;
  double           m_x_pixel_global;
  double           m_y_pixel_global;
  double           m_z_pixel_global;

  double           m_x_SCT;
  double           m_x_exit_SCT;
  double           m_y_exit_SCT;
  double           m_z_exit_SCT;
  double           m_x_entry_SCT;
  double           m_y_entry_SCT;
  double           m_z_entry_SCT;
  double           m_x_SCT_global;
  double           m_y_SCT_global;
  double           m_z_SCT_global;

  double           m_x_pixel_smeared;
  double           m_y_pixel_smeared;
  double           m_x_SCT_smeared;

  double           m_Err_x_pixel;
  double           m_Err_y_pixel;
  double           m_Err_x_SCT;
  double           m_Err_y_SCT;

  ServiceHandle<Trk::ITrackingGeometrySvc>     m_trackingGeometrySvc;        //!< Service handle for retrieving the TrackingGeometry
  mutable const Trk::TrackingGeometry*              m_trackingGeometry;           //!< The TrackingGeometry to be retrieved
  std::string                                  m_trackingGeometryName;       //!< The Name of the TrackingGeometry

  bool m_useCustomGeometry;

  SiSmearedDigitizationTool();
  SiSmearedDigitizationTool(const SiSmearedDigitizationTool&);

  SiSmearedDigitizationTool& operator=(const SiSmearedDigitizationTool&);


};

#endif // SISMEAREDDDIGITIZATION_SISMEAREDDIGITIZATIONTOOL_H
