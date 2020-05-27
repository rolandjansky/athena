/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_DIGITIZATION_TRTDIGITIZATIONTOOL_H
#define TRT_DIGITIZATION_TRTDIGITIZATIONTOOL_H
/** @file TRTDigitizationTool.h
 * @brief a sample implementation of IPileUpTool to test the framework
 * $Id: PileUpStream.h,v 1.18 2008-10-31 18:34:42 calaf Exp $
 * @author Paolo Calafiura - ATLAS Collaboration
 */

#include "xAODEventInfo/EventInfo.h"  /*SubEvent*/
#include "AthenaKernel/IAthRNGSvc.h"
#include "PileUpTools/PileUpToolBase.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "TRT_PAI_Process/ITRT_PAITool.h"
#include "ITRT_SimDriftTimeTool.h"
#include "TRT_ConditionsServices/ITRT_StrawNeighbourSvc.h"
#include "TRT_ConditionsServices/ITRT_StrawStatusSummaryTool.h"
#include "TRT_ConditionsServices/ITRT_CalDbTool.h"

#include "InDetRawData/TRT_RDO_Container.h"
#include "InDetSimData/InDetSimDataCollection.h"
#include "TRTDigit.h"

// For magneticfield
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/WriteHandle.h"
#include <vector>
#include <list>
#include <set>
#include <utility> /* pair */

class TRT_ID;
class TRTProcessingOfStraw;
class TRTElectronicsProcessing;
class TRTDigCondBase;
class TRTNoise;

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

class TRTDigitizationTool : virtual public IPileUpTool, public PileUpToolBase {
public:
  TRTDigitizationTool( const std::string& type, const std::string& name, const IInterface* parent );

  /** Destructor */
  ~TRTDigitizationTool();

  /** Initialize */
  virtual StatusCode initialize() override final;

  /** Finalize */
  virtual StatusCode finalize() override final;

  ///called at the end of the subevts loop. Not (necessarily) able to access SubEvents
  virtual StatusCode mergeEvent(const EventContext& ctx) override final;

  ///called for each active bunch-crossing to process current SubEvents bunchXing is in ns
  virtual StatusCode processBunchXing( int bunchXing,
                                       SubEventIterator bSubEvents,
                                       SubEventIterator eSubEvents ) override final;
  /// return false if not interested in  certain xing times (in ns)
  /// implemented by default in PileUpToolBase as FirstXing<=bunchXing<=LastXing
  //  virtual bool toProcess(int bunchXing) const;

  virtual StatusCode prepareEvent( const EventContext& ctx, const unsigned int nInputEvents ) override final;

  /**
   * Perform digitization:
   * - Get G4 hits
   * - Process hits straw by straw [call @c ProcessStraw() ]
   * - Add noise
   * - Create RDO collection
   */
  virtual StatusCode processAllSubEvents(const EventContext& ctx) override final;

private:
  CLHEP::HepRandomEngine* getRandomEngine(const std::string& streamName, const EventContext& ctx) const;
  CLHEP::HepRandomEngine* getRandomEngine(const std::string& streamName, unsigned long int randomSeedOffset, const EventContext& ctx) const;

  Identifier getIdentifier( int hitID,
                            IdentifierHash& hashId,
                            Identifier& layerID,
                            bool& statusok ) const;

  StatusCode update( IOVSVC_CALLBACK_ARGS );        // Update of database entries.
  StatusCode ConditionsDependingInitialization();

  StatusCode lateInitialize(const EventContext& ctx);
  StatusCode processStraws(const EventContext& ctx,
                           const TimedHitCollection<TRTUncompressedHit>& thpctrt,
                           std::set<int>& sim_hitids,
                           std::set<Identifier>& simhitsIdentifiers,
                           CLHEP::HepRandomEngine *rndmEngine,
                           CLHEP::HepRandomEngine *strawRndmEngine,
                           CLHEP::HepRandomEngine *elecProcRndmEngine,
                           CLHEP::HepRandomEngine *elecNoiseRndmEngine,
                           CLHEP::HepRandomEngine *paiRndmEngine);
  StatusCode createAndStoreRDOs();

  double getCosmicEventPhase(CLHEP::HepRandomEngine *rndmEngine);

  /// Configurable properties
  ToolHandle<ITRT_PAITool> m_TRTpaiToolXe{this, "PAI_Tool_Xe", "TRT_PAI_Process_Xe", "The PAI model for ionisation in the TRT Xe gas"};
  ToolHandle<ITRT_PAITool> m_TRTpaiToolAr{this, "PAI_Tool_Ar", "TRT_PAI_Process_Ar", "The PAI model for ionisation in the TRT Ar gas"};
  ToolHandle<ITRT_PAITool> m_TRTpaiToolKr{this, "PAI_Tool_Kr", "TRT_PAI_Process_Kr", "The PAI model for ionisation in the TRT Kr gas"};
  ToolHandle<ITRT_SimDriftTimeTool> m_TRTsimdrifttimetool{this, "SimDriftTimeTool", "TRT_SimDriftTimeTool", "Drift time versus distance (r-t-relation) for TRT straws"};
  ToolHandle<ITRT_StrawStatusSummaryTool> m_sumTool{this, "InDetTRTStrawStatusSummaryTool", "TRT_StrawStatusSummaryTool", ""};
  ToolHandle<ITRT_CalDbTool> m_calDbTool{this, "InDetTRTCalDbTool", "TRT_CalDbTool", ""};
  ServiceHandle<PileUpMergeSvc> m_mergeSvc{this, "MergeSvc", "PileUpMergeSvc", "Merge service"};
  ServiceHandle<IAthRNGSvc> m_rndmSvc{this, "RndmSvc", "AthRNGSvc", ""};  //!< Random number service
  ServiceHandle<ITRT_StrawNeighbourSvc> m_TRTStrawNeighbourSvc{this, "TRT_StrawNeighbourSvc", "TRT_StrawNeighbourSvc", ""};
  // Read handle for conditions object to get the field cache
  SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj",
                                                                             "Name of the Magnetic Field conditions object key"};
  Gaudi::Property<bool> m_onlyUseContainerName{this, "OnlyUseContainerName", true, "Don't use the ReadHandleKey directly. Just extract the container name from it."};
  SG::ReadHandleKey<TRTUncompressedHitCollection> m_hitsContainerKey{this, "DataObjectName", "TRTUncompressedHits", "Data Object Name"};
  std::string m_dataObjectName{""};
  SG::WriteHandleKey<TRT_RDO_Container> m_outputRDOCollName{this,"OutputObjectName","TRT_RDOs","WHK Output Object name"}; /**< name of the output RDOs. */
  SG::WriteHandleKey<InDetSimDataCollection> m_outputSDOCollName{this,"OutputSDOName","TRT_SDO_Map","WHK Output SDO container name"}; /**< name of the output SDOs. */
  SG::WriteHandle<TRT_RDO_Container> m_trtrdo_container; //RDO container handle

  Gaudi::Property<bool> m_printOverrideableSettings{this, "PrintOverrideableSettings", false, "Print overrideable settings"};
  Gaudi::Property<bool> m_printUsedDigSettings{this, "PrintDigSettings", true, "Print ditigization settings"};
  Gaudi::Property<int> m_HardScatterSplittingMode{this, "HardScatterSplittingMode", 0, ""};
  Gaudi::Property<int> m_UseGasMix{this, "UseGasMix", 0, ""};
  Gaudi::Property<unsigned long int> m_randomSeedOffset{this, "RandomSeedOffset", 678910, ""};

  TRTDigSettings* m_settings{};

  std::vector<std::pair<unsigned int, int> > m_seen;
  std::vector<TRTDigit> m_vDigits; /**< Vector of all digits */
  TRTElectronicsProcessing * m_pElectronicsProcessing{};
  TRTProcessingOfStraw* m_pProcessingOfStraw{};
  TRTDigCondBase* m_pDigConditions{};
  TRTNoise* m_pNoise{};
  //unsigned int m_timer_eventcount;
  const InDetDD::TRT_DetectorManager* m_manager{};
  const TRT_ID* m_trt_id{};       /**< TRT Id Helper */
  std::list<TRTUncompressedHitCollection*> m_trtHitCollList;
  TimedHitCollection<TRTUncompressedHit>* m_thpctrt{};
  bool m_alreadyPrintedPDGcodeWarning{false};
  double m_minCrossingTimeSDO{0.0};
  double m_maxCrossingTimeSDO{0.0};
  double m_minpileuptruthEkin{0.0};
  // const  ComTime* m_ComTime{};
  double m_cosmicEventPhase{0.0};     // local replacement for the comTime service
  const HepPDT::ParticleDataTable* m_particleTable{};
  int m_dig_vers_from_condDB{-1};
  std::string m_digverscontainerkey{"/TRT/Cond/DigVers"};
  bool m_first_event{true};
  bool m_condDBdigverfoldersexists{false};

  bool m_HardScatterSplittingSkipper{false};

};

#endif
