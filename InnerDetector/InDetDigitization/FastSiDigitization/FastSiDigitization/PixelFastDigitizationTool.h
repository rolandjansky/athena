/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelFastDigitizationTool.h
//   Header file for class PixelFastDigitizationTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Top algorithm class for Pixel fast digitization
///////////////////////////////////////////////////////////////////

#ifndef PIXELDIGITIZATION_PIXELFASTDIGITIZATIONTOOL_H
#define PIXELDIGITIZATION_PIXELFASTDIGITIZATIONTOOL_H

#include "PileUpTools/PileUpToolBase.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/AlgTool.h"
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "SiPropertiesSvc/ISiPropertiesSvc.h"
#include "PixelConditionsTools/IModuleDistortionsTool.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "InDetSimData/InDetSimDataCollection.h"
#include "FastSiDigitization/IPixelFastDigitizationTool.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include <string>

#include "HitManagement/TimedHitCollection.h"
#include "InDetSimEvent/SiHit.h"
#include "InDetSimEvent/SiHitCollection.h"

#include "SiClusterizationTool/IPixelClusteringTool.h"
#include "SiClusterizationTool/PixelClusteringToolBase.h"
#include "SiClusterizationTool/ClusterMakerTool.h"

#include "InDetPrepRawData/PixelClusterContainer.h"

#include "TrkTruthData/PRD_MultiTruthCollection.h"

#include "SiClusterizationTool/PixelGangedAmbiguitiesFinder.h"
#include "InDetPrepRawData/PixelGangedClusterAmbiguities.h"



class PixelID;
class IModuleDistortionsTool;
class IInDetConditionsSvc;

namespace InDetDD{class SiDetectorElement;}
namespace CLHEP {class HepRandomEngine;}

namespace InDet {
  class MergedPixelsTool;
  class ClusterMakerTool;
  class PixelCluster;
  class IPixelClusteringTool;
  class PixelGangedAmbiguitiesFinder;
  class PrepRawData;
}

class PixelFastDigitizationTool : 
virtual public PileUpToolBase, virtual public IPixelFastDigitizationTool
{

public:

   /** Constructor with parameters */
   PixelFastDigitizationTool(
			 const std::string& type,
			 const std::string& name,
			 const IInterface* parent
			 );


   StatusCode initialize();
   StatusCode prepareEvent(unsigned int);
   StatusCode processBunchXing( int bunchXing,
			       PileUpEventInfo::SubEvent::const_iterator bSubEvents,
			       PileUpEventInfo::SubEvent::const_iterator eSubEvents ); 
   StatusCode processAllSubEvents();
   StatusCode mergeEvent();
   StatusCode digitize();
   StatusCode createAndStoreRIOs();
   


 private:

   /** create and store RDO for the given collecti<on */
//   StatusCode createAndStoreRDO(const DiodeCollectionPtr& c);

   /** create RDO from given collection - called by createAndStoreRDO */
//   PixelRDO_Collection *createRDO(const DiodeCollectionPtr& c);


  std::string m_inputObjectName;     //! name of the sub event  hit collections. 
  TimedHitCollection<SiHit>* m_thpcsi;      


  std::vector<std::pair<unsigned int, int> > m_seen;
  std::list<SiHitCollection*> m_siHitCollList;

  ServiceHandle <IAtRndmGenSvc> m_rndmSvc;             //!< Random number service
  const InDetDD::PixelDetectorManager* m_manager;
  const PixelID* m_pixel_ID;                             //!< Handle to the ID helper

  CLHEP::HepRandomEngine*           m_randomEngine;
  std::string                m_randomEngineName;         //!< Name of the random number stream

  typedef std::multimap<IdentifierHash, const InDet::PixelCluster*> Pixel_detElement_RIO_map;
  Pixel_detElement_RIO_map* m_pixelClusterMap;

  double                                m_pixTanLorentzAngleScalor; //!< scale the lorentz angle effect
  mutable float                         m_siTanLorentzAngle;
  mutable float                         m_siClusterShift;
  mutable int                           m_siRdosValid;
  bool                                  m_pixEmulateSurfaceCharge;  //!< emulate the surface charge 
  double                                m_pixSmearPathLength;       //!< the 2. model parameter: smear the path
  bool                                  m_pixSmearLandau;           //!< if true : landau else: gauss
  mutable float                         m_siSmearRn; 
  mutable int                           m_siDeltaPhiRaw;
  mutable int                           m_siDeltaEtaRaw;
  mutable int                           m_siDeltaPhiCut;
  mutable int                           m_siDeltaEtaCut;
  double                                m_pixMinimalPathCut;        //!< the 1. model parameter: minimal 3D path in pixel
  double                                m_pixPathLengthTotConv;     //!< from path length to tot
  bool                                  m_pixUseClusterMaker;       //!< use the pixel cluster maker or not  
  
  /** ToolHandle to ClusterMaker */
  ToolHandle<InDet::ClusterMakerTool>  m_clusterMaker;
  
  bool                                  m_pixModuleDistortion;       //!< simulationn of module bowing
  ToolHandle<IModuleDistortionsTool>    m_pixDistortionTool;         //!< respect the pixel distortions
  std::vector<double>                   m_pixPhiError;              //!< phi error when not using the ClusterMaker  
  std::vector<double>                   m_pixEtaError;              //!< eta error when not using the ClusterMaker
  int                                   m_pixErrorStrategy;         //!< error strategy for the  ClusterMaker

  InDet::PixelClusterContainer*         m_pixelClusterContainer;               //!< the PixelClusterContainer
  
  ServiceHandle<PileUpMergeSvc> m_mergeSvc;      /**< PileUp Merge service */
  std::string                           m_prdTruthNamePixel;
  PRD_MultiTruthCollection*             m_pixPrdTruth;              //!< the PRD truth map for SCT measurements
  
  ServiceHandle<IInDetConditionsSvc>    m_pixelCondSummarySvc;   //!< Handle to pixel conditions service
  
  ToolHandle< InDet::PixelGangedAmbiguitiesFinder > m_gangedAmbiguitiesFinder; 

  std::string                           m_pixel_SiClustersName;

  bool isActiveAndGood(const ServiceHandle<IInDetConditionsSvc> &svc, const IdentifierHash &idHash, const Identifier &id, bool querySingleChannel, const char *elementName, const char *failureMessage = "") const;
  bool areNeighbours(const std::vector<Identifier>& group,  const Identifier& rdoID, InDetDD::SiDetectorElement* /*element*/, const PixelID& pixelID) const;

   PixelFastDigitizationTool();
   PixelFastDigitizationTool(const PixelFastDigitizationTool&);

   PixelFastDigitizationTool& operator=(const PixelFastDigitizationTool&);


//   void addSDO( const DiodeCollectionPtr& collection );



};

#endif // PIXELDIGITIZATION_PIXELDIGITIZATION_H
