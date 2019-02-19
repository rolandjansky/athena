/* -*- C++ -*- */


/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASTSIDIGITZATION_SCT_FASTDIGITZATIONTOOL_H
#define FASTSIDIGITZATION_SCT_FASTDIGITZATIONTOOL_H
/** @file SCT_FastDigitizationTool.h
 * @brief Digitize the SCT using an implementation of IPileUpTool
 * $Id: SCT_DigitizationTool.h,v 1.0 2009-09-22 18:34:42 jchapman Exp $
 * @author John Chapman - ATLAS Collaboration
 */

#include "FastSiDigitization/ISCT_FastDigitizationTool.h"

#include "PileUpTools/PileUpToolBase.h"

#include "AthenaKernel/IAtRndmGenSvc.h"

#include "HitManagement/TimedHitCollection.h"
#include "InDetSimEvent/SiHit.h"
#include "InDetSimEvent/SiHitCollection.h"

#include "InDetPrepRawData/SCT_ClusterContainer.h"  // typedef
#include "InDetPrepRawData/SiClusterContainer.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

#include "TrkEventTPCnv/TrkEventPrimitives/HepSymMatrix_p1.h"

#include "TrkTruthData/PRD_MultiTruthCollection.h"

#include "EventPrimitives/EventPrimitives.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "InDetCondTools/ISiLorentzAngleTool.h"

// Gaudi
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/AlgTool.h"

#include "boost/shared_ptr.hpp"

#include <vector>
#include <list>
#include <utility> /* pair */
#include <map>
#include <string>

//FIXME - not used anywhere?
// #ifndef MAXSTEPS
// #define MAXSTEPS 15
// #endif

// #ifndef MAXDRIFTSTEPS
// #define MAXDRIFTSTEPS 15
// #endif


class InDetSimDataCollection;

class SCT_ID;

class SiChargedDiodeCollection;
class StoreGateService;

namespace InDet {
  class ClusterMakerTool;
  class SCT_Cluster;
  class SiCluster;
}

namespace CLHEP
{
  class HepRandomEngine;
}

namespace Trk {
  class Surface;
}

namespace CLHEP {
  class     HepSymMatrix ;   /// CLHEP
}

class SCT_FastDigitizationTool :
  virtual public PileUpToolBase, virtual public ISCT_FastDigitizationTool
{

public:
  SCT_FastDigitizationTool(const std::string& type,
                           const std::string& name,
                           const IInterface* parent);
  /**
     @brief Called before processing physics events
  */
  virtual StatusCode initialize();
  StatusCode prepareEvent( unsigned int );
  StatusCode processBunchXing( int bunchXing,
                               SubEventIterator bSubEvents,
                               SubEventIterator eSubEvents );
  StatusCode mergeEvent();
  StatusCode processAllSubEvents();
  StatusCode createAndStoreRIOs();

private:

  StatusCode digitize();
  StatusCode createOutputContainers();
  bool NeighbouringClusters(const std::vector<Identifier>& potentialClusterRDOList,  const InDet::SCT_Cluster *existingCluster) const;
  void Diffuse(HepGeom::Point3D<double>& localEntry, HepGeom::Point3D<double>& localExit, double shiftX, double shiftY ) const;

  std::string m_inputObjectName;     //! name of the sub event  hit collections.

  std::list<SiHitCollection*> m_siHitCollList;

  const SCT_ID* m_sct_ID;                              //!< Handle to the ID helper
  ServiceHandle<PileUpMergeSvc> m_mergeSvc;            //!< PileUp Merge service
  int                       m_HardScatterSplittingMode; /**< Process all SiHit or just those from signal or background events */
  bool                      m_HardScatterSplittingSkipper;

  ServiceHandle <IAtRndmGenSvc> m_rndmSvc;             //!< Random number service
  CLHEP::HepRandomEngine       *m_randomEngine;        //!< Pointer to the random number Engine
  std::string                   m_randomEngineName;    //!< Name of the random number stream

  TimedHitCollection<SiHit>* m_thpcsi;

  ToolHandle<InDet::ClusterMakerTool>  m_clusterMaker;
  ToolHandle<ISiLorentzAngleTool> m_lorentzAngleTool{this, "LorentzAngleTool", "SiLorentzAngleTool/SCTLorentzAngleTool", "Tool to retreive Lorentz angle"};
  bool m_sctUseClusterMaker;       //!< use the pixel cluster maker or not
  IntegerProperty  m_vetoThisBarcode;

  typedef std::multimap<IdentifierHash, InDet::SCT_Cluster*> SCT_detElement_RIO_map;
  SCT_detElement_RIO_map* m_sctClusterMap;

  SG::WriteHandle<InDet::SCT_ClusterContainer>  m_sctClusterContainer; //!< the SCT_ClusterContainer
  SG::WriteHandle<PRD_MultiTruthCollection>     m_sctPrdTruth;         //!< the PRD truth map for SCT measurements
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};

  double m_sctSmearPathLength;       //!< the 2. model parameter: smear the path
  bool m_sctSmearLandau;           //!< if true : landau else: gauss
  bool m_sctEmulateSurfaceCharge;  //!< emulate the surface charge
  double m_sctTanLorentzAngleScalor; //!< scale the lorentz angle effect
  bool m_sctAnalogStripClustering; //!< not being done in ATLAS: analog strip clustering
  int m_sctErrorStrategy;         //!< error strategy for the  ClusterMaker
  bool m_sctRotateEC;

  bool m_mergeCluster; //!< enable the merging of neighbour SCT clusters >
  double m_DiffusionShiftX_barrel;
  double m_DiffusionShiftY_barrel;
  double m_DiffusionShiftX_endcap;
  double m_DiffusionShiftY_endcap;
  double m_sctMinimalPathCut;        //!< the 1. model parameter: minimal 3D path in strip

  Amg::Vector3D stepToStripBorder(const InDetDD::SiDetectorElement& sidetel,
                                  //const Trk::Surface& surface,
                                  double localStartX, double localStartY,
                                  double localEndX, double localEndY,
                                  double slopeYX,
                                  double slopeZX,
                                  const Amg::Vector2D& stripCenter,
                                  int direction) const;




};
#endif // FASTSIDIGITZATION_SCT_FASTDIGITZATIONTOOL_H
