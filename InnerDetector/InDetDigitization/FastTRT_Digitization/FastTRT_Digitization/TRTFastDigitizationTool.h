/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASTTRT_DIGITIZATION_FASTTRT_DIGITIZATIONTOOL_H
#define FASTTRT_DIGITIZATION_FASTTRT_DIGITIZATIONTOOL_H
/** @file TRTFastDigitizationTool.h
 * @brief a sample implementation of IPileUpTool to test the framework
 * $Id: PileUpStream.h,v 1.18 2008-10-31 18:34:42 calaf Exp $
 * @author Paolo Calafiura - ATLAS Collaboration
 */

#include "EventInfo/PileUpEventInfo.h"
#include "PileUpTools/PileUpToolBase.h"
#include "FastTRT_Digitization/ITRTFastDigitizationTool.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "InDetPrepRawData/TRT_DriftCircleContainer.h"

#include "GaudiKernel/RndmGenerators.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include "TrkParameters/TrackParameters.h"

#include "Identifier/Identifier.h"
#include "CLHEP/Random/RandGauss.h"

#include "TrkTruthData/PRD_MultiTruthCollection.h"

//#include "GaudiKernel/IPartPropSvc.h"
//#include "HepPDT/ParticleDataTable.hh"

#include "HitManagement/TimedHitCollection.h"
#include "InDetSimEvent/TRTUncompressedHitCollection.h"

#include <utility>
#include <vector>
#include <map>


class PileUpMergeSvc;
class IAtRndmGenSvc;
class TRT_ID;
class TRTUncompressedHit;

namespace InDetDD {
  class TRT_DetectorManager;
}

class TRT_ID;
class StoreGateSvc;
class ITRT_DriftFunctionTool;


class TRTFastDigitizationTool : public PileUpToolBase, virtual public ITRTFastDigitizationTool {
public:
  TRTFastDigitizationTool( const std::string &type, const std::string &name, const IInterface *parent );

  ///called at the end of the subevts loop. Not (necessarily) able to access
  ///SubEvents
  StatusCode mergeEvent();

  ///called for each active bunch-crossing to process current SubEvents
  /// bunchXing is in ns
  StatusCode processBunchXing( int bunchXing,
                               PileUpEventInfo::SubEvent::const_iterator bSubEvents,
                               PileUpEventInfo::SubEvent::const_iterator eSubEvents );

  /// return false if not interested in  certain xing times (in ns)
  /// implemented by default in PileUpToolBase as FirstXing<=bunchXing<=LastXing
  //  virtual bool toProcess(int bunchXing) const;

  StatusCode prepareEvent( const unsigned int /*nInputEvents*/ );

  ///alternative interface which uses the PileUpMergeSvc to obtain all
  ///the required SubEvents.
  StatusCode processAllSubEvents();

  /** Initialize */
  virtual StatusCode initialize();

  /** Finalize */
  StatusCode finalize();

private:

  StatusCode produceDriftCircles();

  Identifier getIdentifier( int hitID, IdentifierHash &hash, Identifier &layer_id, bool &status ) const;

  StatusCode createAndStoreRIOs();

  static double getDriftRadiusFromXYZ( const TimedHitPtr< TRTUncompressedHit > &hit );
  HepGeom::Point3D< double > getGlobalPosition( const TimedHitPtr< TRTUncompressedHit > &hit );
  static double HTProbabilityElectron_high_pt( double eta );
  static double HTProbabilityElectron_low_pt( double eta );
  static double HTProbabilityMuon_5_20( double eta );
  static double HTProbabilityMuon_60( double eta );
  static double strawEfficiency( double driftRadius );                  // defined, but not used
  double particleMass( int i ) const;

  // Tools and Services
  ToolHandle< ITRT_DriftFunctionTool > m_trtDriftFunctionTool;
  ServiceHandle< PileUpMergeSvc > m_mergeSvc;                           // PileUp Merge service
  ServiceHandle < IAtRndmGenSvc > m_atRndmGenSvc;                       // Random number service
  CLHEP::HepRandomEngine *m_randomEngine;
  std::string m_randomEngineName;                                       // Name of the random number stream
  //ServiceHandle< IPartPropSvc > m_partPropSvc;
  //HepPDT::ParticleDataTable *m_particleDataTable;

  // Containers
  std::string m_trtHitCollectionKey;                                    // INPUT:  TRT_HitCollection
  std::list<TRTUncompressedHitCollection*> m_trtHitCollList;
  TimedHitCollection<TRTUncompressedHit> *m_thpctrt;
  std::string m_trtDriftCircleKey;                                      // OUTPUT: InDet::TRT_DriftCircleContainer
  InDet::TRT_DriftCircleContainer *m_trtDriftCircleContainer;
  std::string m_prdMultiTruthKey;                                       // OUTPUT: Trk::PRD_MultiTruthCollection
  PRD_MultiTruthCollection *m_prdMultiTruthCollection;

  std::multimap< Identifier, InDet::TRT_DriftCircle * > m_driftCircleMap;

  // Helpers
  const InDetDD::TRT_DetectorManager *m_trt_manager;
  const TRT_ID *m_trt_id;                                               // TRT Id Helper

  // Constants
  bool m_trtTRsimulation;
  double m_trtTailFraction;              // part of the fraction in Tails
  double m_trtSigmaDriftRadius;          // sigma of one TRT straw in R
  double m_trtSigmaDriftRadiusTail;      // sigma of one TRT straw in R (Tail)
  double m_trtFitAmplitude;              // Fit parameter for TRT efficiency calculation
  double m_trtFitMu;                     // Fit parameter for TRT efficiency calculation
  double m_trtFitR;                      // Fit parameter for TRT efficiency calculation
  double m_trtFitSigma;                  // Fit parameter for TRT efficiency calculation
  double m_trtFitConstant;
  double m_trtTRprobParC1;               // defined, but not used
  double m_trtTRprobParC2;
  double m_trtTRprobParC3;
  double m_trtTRprobParC4;

  // Split configuration
  int    m_HardScatterSplittingMode; /**< Process all TRT_Hits or just those from signal or background events */
  bool   m_HardScatterSplittingSkipper;
  IntegerProperty  m_vetoThisBarcode;
};

#endif //FASTTRT_DIGITIZATION_FASTTRT_DIGITIZATIONTOOL_H
