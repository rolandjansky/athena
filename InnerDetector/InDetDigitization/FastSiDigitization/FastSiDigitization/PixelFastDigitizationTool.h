/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelFastDigitizationTool.h
//   Header file for class PixelFastDigitizationTool
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// Top algorithm class for Pixel fast digitization
///////////////////////////////////////////////////////////////////

#ifndef FASTSIDIGITIZATION_PIXELFASTDIGITIZATIONTOOL_H
#define FASTSIDIGITIZATION_PIXELFASTDIGITIZATIONTOOL_H

#include "PileUpTools/PileUpToolBase.h"
#include "FastSiDigitization/IPixelFastDigitizationTool.h"
#include "HitManagement/TimedHitCollection.h"
#include "InDetSimEvent/SiHit.h"
#include "InDetSimEvent/SiHitCollection.h" // cannot fwd declare
#include "InDetPrepRawData/PixelClusterContainer.h" //typedef, cannot fwd declare
#include "SiClusterizationTool/PixelGangedAmbiguitiesFinder.h"
#include "InDetPrepRawData/PixelGangedClusterAmbiguities.h" //typedef, cannot fwd declare
#include "PixelConditionsServices/IPixelCalibSvc.h"
#include "SiClusterizationTool/ClusterMakerTool.h"
#include "PileUpTools/PileUpMergeSvc.h"


//New digi
#include "TrkDigEvent/DigitizationModule.h"
#include "TrkDigInterfaces/IModuleStepper.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "InDetCondTools/ISiLorentzAngleTool.h"

#include <string>
#include <vector>
#include <list>
#include <map>


class PixelID;
class IModuleDistortionsTool;
class PRD_MultiTruthCollection;
class IAtRndmGenSvc;

namespace InDetDD{
  class SiDetectorElement;
  class PixelDetectorManager;
}
namespace CLHEP {class HepRandomEngine;}

namespace InDet {
  class PixelCluster;
  class PixelGangedAmbiguitiesFinder;
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
  /** Destructor */
  ~PixelFastDigitizationTool();

  StatusCode initialize();
  StatusCode prepareEvent(unsigned int);
  StatusCode processBunchXing( int bunchXing,
                               SubEventIterator bSubEvents,
                               SubEventIterator eSubEvents );
  StatusCode processAllSubEvents();
  StatusCode mergeEvent();
  StatusCode digitize();
  StatusCode createAndStoreRIOs();




private:


  TimedHitCollection<SiHit>* m_thpcsi;

  ServiceHandle <IAtRndmGenSvc> m_rndmSvc;             //!< Random number service
  CLHEP::HepRandomEngine*           m_randomEngine;
  std::string                m_randomEngineName;         //!< Name of the random number stream

  const InDetDD::PixelDetectorManager* m_manager;
  const PixelID* m_pixel_ID;                             //!< Handle to the ID helper

  ToolHandle<InDet::ClusterMakerTool>  m_clusterMaker;   //!< ToolHandle to ClusterMaker
  ToolHandle<ISiLorentzAngleTool> m_lorentzAngleTool{this, "LorentzAngleTool", "PixelLorentzAngleTool", "Tool to retreive Lorentz angle"};
  bool                                  m_pixUseClusterMaker; //!< use the pixel cluster maker or not

  InDet::PixelClusterContainer*         m_pixelClusterContainer;               //!< the PixelClusterContainer
  std::string                           m_pixel_SiClustersName;

  ServiceHandle<PileUpMergeSvc> m_mergeSvc;      /**< PileUp Merge service */
  int                       m_HardScatterSplittingMode; /**< Process all SiHit or just those from signal or background events */
  bool                      m_HardScatterSplittingSkipper;
  IntegerProperty  m_vetoThisBarcode;

  typedef std::multimap<IdentifierHash, InDet::PixelCluster*> Pixel_detElement_RIO_map;
  Pixel_detElement_RIO_map* m_pixelClusterMap;

  std::string                           m_prdTruthNamePixel;
  PRD_MultiTruthCollection*             m_pixPrdTruth;              //!< the PRD truth map for Pixel measurements

  //  ServiceHandle<IInDetConditionsSvc>    m_pixelCondSummarySvc;   //!< Handle to pixel conditions service

  ToolHandle< InDet::PixelGangedAmbiguitiesFinder > m_gangedAmbiguitiesFinder;

  std::string m_inputObjectName;     //! name of the sub event  hit collections.

  std::list<SiHitCollection*> m_siHitCollList;

  double                                m_pixTanLorentzAngleScalor; //!< scale the lorentz angle effect
  bool                                  m_pixEmulateSurfaceCharge;  //!< emulate the surface charge
  double                                m_pixSmearPathLength;       //!< the 2. model parameter: smear the path
  bool                                  m_pixSmearLandau;           //!< if true : landau else: gauss
  mutable int                           m_siDeltaPhiCut;
  mutable int                           m_siDeltaEtaCut;
  double                                m_pixMinimalPathCut;        //!< the 1. model parameter: minimal 3D path in pixel
  double                                m_pixPathLengthTotConv;     //!< from path length to tot
  bool                                  m_pixModuleDistortion;       //!< simulationn of module bowing
  ToolHandle<IModuleDistortionsTool>    m_pixDistortionTool;         //!< respect the pixel distortions
  std::vector<double>                   m_pixPhiError;              //!< phi error when not using the ClusterMaker
  std::vector<double>                   m_pixEtaError;              //!< eta error when not using the ClusterMaker
  int                                   m_pixErrorStrategy;         //!< error strategy for the  ClusterMaker
  double                                m_pixDiffShiftBarrX; //Shift of the track to improve cluster size description
  double                                m_pixDiffShiftBarrY; //Shift of the track to improve cluster size description
  double                                m_pixDiffShiftEndCX; //Shift of the track to improve cluster size description
  double                                m_pixDiffShiftEndCY; //Shift of the track to improve cluster size description
  double                                m_ThrConverted;

  bool m_mergeCluster; //!< enable the merging of neighbour Pixel clusters >
  short m_splitClusters; //!< merging parameter used to define two clusters as neighbour >
  bool m_acceptDiagonalClusters; //!< merging parameter used to define two clusters as neighbour >
  std::string                           m_pixelClusterAmbiguitiesMapName;
  InDet::PixelGangedClusterAmbiguities* m_ambiguitiesMap;
  ServiceHandle<IPixelCalibSvc>         m_pixelCalibSvc;

  //  bool isActiveAndGood(const ServiceHandle<IInDetConditionsSvc> &svc, const IdentifierHash &idHash, const Identifier &id, bool querySingleChannel, const char *elementName, const char *failureMessage = "") const;
  bool areNeighbours(const std::vector<Identifier>& group,  const Identifier& rdoID, InDetDD::SiDetectorElement* /*element*/, const PixelID& pixelID) const;

  PixelFastDigitizationTool();
  PixelFastDigitizationTool(const PixelFastDigitizationTool&);

  PixelFastDigitizationTool& operator=(const PixelFastDigitizationTool&);

  ToolHandle<Trk::IModuleStepper>       m_digitizationStepper;

  Trk::DigitizationModule * buildDetectorModule(const InDetDD::SiDetectorElement* ) const;

 Amg::Vector3D CalculateIntersection(const Amg::Vector3D & Point, const Amg::Vector3D & Direction, Amg::Vector2D PlaneBorder, double halfthickness) const;
 void Diffuse(HepGeom::Point3D<double>& localEntry, HepGeom::Point3D<double>& localExit, double shiftX, double shiftY ) const;
  //   void addSDO( const DiodeCollectionPtr& collection );



};

#endif // FASTSIDIGITIZATION_PIXELDIGITIZATION_H
