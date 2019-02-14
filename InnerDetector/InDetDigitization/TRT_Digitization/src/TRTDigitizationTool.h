/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_DIGITIZATION_TRTDIGITIZATIONTOOL_H
#define TRT_DIGITIZATION_TRTDIGITIZATIONTOOL_H
/** @file TRTDigitizationTool.h
 * @brief a sample implementation of IPileUpTool to test the framework
 * $Id: PileUpStream.h,v 1.18 2008-10-31 18:34:42 calaf Exp $
 * @author Paolo Calafiura - ATLAS Collaboration
 */

#include "xAODEventInfo/EventInfo.h"  /*SubEvent*/
#include "PileUpTools/PileUpToolBase.h"
#include "AthenaKernel/IAthRNGSvc.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include "TRTDigit.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "InDetRawData/TRT_RDO_Container.h"
#include "InDetSimData/InDetSimDataCollection.h"

#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/WriteHandle.h"
#include <vector>
#include <list>
#include <set>
#include <utility> /* pair */

class ITRT_PAITool;
class ITRT_SimDriftTimeTool;
class TRT_ID;
class TRTProcessingOfStraw;
class TRTElectronicsProcessing;
class TRTDigCondBase;
class TRTNoise;
class ITRT_StrawNeighbourSvc;
class ITRT_StrawStatusSummarySvc;

namespace CLHEP{
  class HepRandomEngine;
}

namespace HepPDT{
  class ParticleDataTable;
}

#include "HitManagement/TimedHitCollection.h"
class TRTUncompressedHit;
#include "InDetSimEvent/TRTUncompressedHitCollection.h"
//class TRTUncompressedHitCollection;
namespace InDetDD {
  class TRT_DetectorManager;
}

class TRTDigSettings;

static const InterfaceID IID_ITRTDigitizationTool ("TRTDigitizationTool",1,0);

class TRTDigitizationTool : virtual public IPileUpTool, public PileUpToolBase {
public:
  static const InterfaceID& interfaceID();
  TRTDigitizationTool( const std::string& type, const std::string& name, const IInterface* parent );

  /** Destructor */
  ~TRTDigitizationTool();

  ///called at the end of the subevts loop. Not (necessarily) able to access SubEvents
  StatusCode mergeEvent();

  ///called for each active bunch-crossing to process current SubEvents bunchXing is in ns
  StatusCode processBunchXing( int bunchXing,
			       SubEventIterator bSubEvents,
			       SubEventIterator eSubEvents );
  /// return false if not interested in  certain xing times (in ns)
  /// implemented by default in PileUpToolBase as FirstXing<=bunchXing<=LastXing
  //  virtual bool toProcess(int bunchXing) const;

  StatusCode prepareEvent( const unsigned int /*nInputEvents*/ );

  /** Initialize */
  virtual StatusCode initialize();

  /**
   * Perform digitization:
   * - Get G4 hits
   * - Process hits straw by straw [call @c ProcessStraw() ]
   * - Add noise
   * - Create RDO collection
   */
  virtual StatusCode processAllSubEvents();

  /** Finalize */
  StatusCode finalize();

private:
  CLHEP::HepRandomEngine* getRandomEngine(const std::string& streamName) const;

  Identifier getIdentifier( int hitID,
			    IdentifierHash& hashId,
			    Identifier& layerID,
			    bool& statusok ) const;

  StatusCode update( IOVSVC_CALLBACK_ARGS );        // Update of database entries.
  StatusCode ConditionsDependingInitialization();

  StatusCode lateInitialize(CLHEP::HepRandomEngine *noiseRndmEngine,
                            CLHEP::HepRandomEngine *elecNoiseRndmEngine,
                            CLHEP::HepRandomEngine *elecProcRndmEngine,
                            CLHEP::HepRandomEngine *fakeCondRndmEngine);
  StatusCode processStraws(std::set<int>& sim_hitids, std::set<Identifier>& simhitsIdentifiers,
                           CLHEP::HepRandomEngine *rndmEngine,
                           CLHEP::HepRandomEngine *strawRndmEngine,
                           CLHEP::HepRandomEngine *elecProcRndmEngine,
                           CLHEP::HepRandomEngine *elecNoiseRndmEngine);
  StatusCode createAndStoreRDOs();

  // The straw's gas mix: 1=Xe, 2=Kr, 3=Ar
  int StrawGasType(Identifier& TRT_Identifier) const;

  unsigned int getRegion(int hitID);
  double getCosmicEventPhase(CLHEP::HepRandomEngine *rndmEngine);

  std::vector<std::pair<unsigned int, int> > m_seen;
  std::vector<TRTDigit> m_vDigits; /**< Vector of all digits */
  std::string m_dataObjectName; /**< Name of the hits collections */
  SG::WriteHandleKey<TRT_RDO_Container> m_outputRDOCollName{this,"OutputObjectName","TRT_RDOs","WHK Output Object name"}; /**< name of the output RDOs. */
  SG::WriteHandleKey<InDetSimDataCollection> m_outputSDOCollName{this,"OutputSDOName","TRT_SDO_Map","WHK Output SDO container name"}; /**< name of the output SDOs. */
  SG::WriteHandle<TRT_RDO_Container> m_trtrdo_container; //RDO container handle

  bool m_printOverrideableSettings;
  bool m_printUsedDigSettings;
  ToolHandle<ITRT_PAITool> m_TRTpaiToolXe;
  ToolHandle<ITRT_PAITool> m_TRTpaiToolAr;
  ToolHandle<ITRT_PAITool> m_TRTpaiToolKr;
  ToolHandle<ITRT_SimDriftTimeTool> m_TRTsimdrifttimetool;
  ServiceHandle<PileUpMergeSvc> m_mergeSvc;      /**< PileUp Merge service */

  TRTElectronicsProcessing * m_pElectronicsProcessing;
  TRTProcessingOfStraw* m_pProcessingOfStraw;
  TRTDigCondBase* m_pDigConditions;
  TRTNoise* m_pNoise;

  //unsigned int m_timer_eventcount;
  ServiceHandle<IAthRNGSvc> m_rndmSvc{this, "RndmSvc", "AthRNGSvc", ""};  //!< Random number service
  ServiceHandle<ITRT_StrawNeighbourSvc> m_TRTStrawNeighbourSvc;

  const InDetDD::TRT_DetectorManager* m_manager;
  TRTDigSettings* m_settings;
  const TRT_ID* m_trt_id;       /**< TRT Id Helper */
  std::list<TRTUncompressedHitCollection*> m_trtHitCollList;
  TimedHitCollection<TRTUncompressedHit>* m_thpctrt;
  bool m_alreadyPrintedPDGcodeWarning;
  double m_minCrossingTimeSDO;
  double m_maxCrossingTimeSDO;
  double m_minpileuptruthEkin;

  // const  ComTime* m_ComTime;

  const HepPDT::ParticleDataTable* m_particleTable;
  int m_dig_vers_from_condDB;
  std::string m_digverscontainerkey;
  bool m_first_event;
  bool m_condDBdigverfoldersexists;

  int m_HardScatterSplittingMode;
  bool m_HardScatterSplittingSkipper;

  int m_UseGasMix;
  double m_cosmicEventPhase;     // local replacement for the comTime service
  ServiceHandle<ITRT_StrawStatusSummarySvc> m_sumSvc;
  IntegerProperty m_vetoThisBarcode;

};

inline const InterfaceID& TRTDigitizationTool::interfaceID()
{
  return IID_ITRTDigitizationTool;
}

#endif
