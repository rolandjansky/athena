/* -*- C++ -*- */

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STRIPDIGITIZATION_STRIPDIGITIZATIONTOOL_H
#define STRIPDIGITIZATION_STRIPDIGITIZATIONTOOL_H
/** @file StripDigitizationTool.h
 * @brief Digitize the ITkStrip using an implementation of IPileUpTool
 */

//Base class header
#include "PileUpTools/PileUpToolBase.h"

// Athena headers
#include "AthenaKernel/IAthRNGSvc.h"
#include "HitManagement/TimedHitCollection.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetSimData/InDetSimDataCollection.h"
#include "InDetSimEvent/SiHitCollection.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include "SiDigitization/IFrontEnd.h"
#include "SiDigitization/IRandomDisabledCellGenerator.h"
#include "SiDigitization/ISurfaceChargesGenerator.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/WriteHandleKey.h"

// Gaudi headers
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// STL headers
#include <limits>
#include <memory>
#include <string>


// Forward declarations
class ISiChargedDiodesProcessorTool;
class SCT_ID;
class SiChargedDiodeCollection;

typedef std::unordered_map<int, std::unique_ptr<SiChargedDiodeCollection>> SiChargedDiodeCollectionMap;
typedef std::pair<const int, std::unique_ptr<SiChargedDiodeCollection>> SiChargedDiodeCollectionIterator;

namespace CLHEP
{
  class HepRandomEngine;
}

namespace ITk
{

class StripDigitizationTool : public extends<PileUpToolBase, IPileUpTool>
{
public:
  static const InterfaceID& interfaceID();
  StripDigitizationTool(const std::string& type,
                       const std::string& name,
                       const IInterface* parent);
  virtual ~StripDigitizationTool();
  /**
     @brief Called before processing physics events
  */
  virtual StatusCode prepareEvent(const EventContext& ctx, unsigned int) override final;
  virtual StatusCode processBunchXing(int bunchXing,
                                      SubEventIterator bSubEvents,
                                      SubEventIterator eSubEvents) override final;
  virtual StatusCode mergeEvent(const EventContext& ctx) override final;

  virtual StatusCode initialize() override final;
  virtual StatusCode processAllSubEvents(const EventContext& ctx) override final;

protected:

  bool digitizeElement(const EventContext& ctx, SiChargedDiodeCollectionMap& chargedDiodes, TimedHitCollection<SiHit>*& thpcsi, CLHEP::HepRandomEngine * rndmEngine); //!
  void applyProcessorTools(SiChargedDiodeCollection* chargedDiodes, CLHEP::HepRandomEngine * rndmEngine) const; //!
  void addSDO(SiChargedDiodeCollection* collection, SG::WriteHandle<InDetSimDataCollection>* simDataCollMap) const;

  void storeTool(ISiChargedDiodesProcessorTool* p_processor) {m_diodeCollectionTools.push_back(p_processor);}

private:

  /**
     @brief initialize the required services
  */
  StatusCode initServices();
  /**
     @brief Initialize the StripFrontEnd AlgTool
  */
  StatusCode initFrontEndTool();
  /**
     @brief Initialize the StripRandomDisabledCellGenerator AlgTool
  */
  StatusCode initDisabledCells();
  /**
     @brief Initialize the StripSurfaceChargesGenerator AlgTool
  */
  StatusCode initSurfaceChargesGeneratorTool();

  /** RDO and SDO methods*/
  /**
     @brief Create RDOs from the SiChargedDiodeCollection for the current wafer and save to StoreGate
     @param chDiodeCollection       list of the SiChargedDiodes on the current wafer
  */
  StatusCode createAndStoreRDO(SiChargedDiodeCollection* chDiodeCollection, SG::WriteHandle<SCT_RDO_Container>* rdoContainer) const;
  /**
     @brief Create RDOs from the SiChargedDiodeCollection for the current wafer
     @param collection       list of the SiChargedDiodes on the current wafer
  */
  std::unique_ptr<SCT_RDO_Collection> createRDO(SiChargedDiodeCollection* collection) const;

  StatusCode getNextEvent(const EventContext& ctx);
  void       digitizeAllHits(const EventContext& ctx, SG::WriteHandle<SCT_RDO_Container>* rdoContainer, SG::WriteHandle<InDetSimDataCollection>* simDataCollMap, std::vector<bool>* processedElements, TimedHitCollection<SiHit>* thpcsi, CLHEP::HepRandomEngine * rndmEngine); //!< digitize all hits
  void       digitizeNonHits(const EventContext& ctx, SG::WriteHandle<SCT_RDO_Container>* rdoContainer, SG::WriteHandle<InDetSimDataCollection>* simDataCollMap, const std::vector<bool>* processedElements, CLHEP::HepRandomEngine * rndmEngine) const;     //!< digitize SCT without hits

  /**
     @brief Called when m_WriteSCT1_RawData is altered. Does nothing, but required by Gaudi.
  */
  void SetupRdoOutputType(Gaudi::Details::PropertyBase&);

  FloatProperty m_tfix{this, "FixedTime", -999., "Fixed time for Cosmics run selection"};
  BooleanProperty m_enableHits{this, "EnableHits", true, "Enable hits"};
  BooleanProperty m_onlyHitElements{this, "OnlyHitElements", false, "Process only elements with hits"};
  BooleanProperty m_cosmicsRun{this, "CosmicsRun", false, "Cosmics run selection"};
  BooleanProperty m_barrelonly{this, "BarrelOnly", false, "Only Barrel layers"};
  BooleanProperty m_randomDisabledCells{this, "RandomDisabledCells", false, "Use Random disabled cells, default no"};
  BooleanProperty m_createNoiseSDO{this, "CreateNoiseSDO", false, "Create SDOs for strips with only noise hits (huge increase in SDO collection size"};
  IntegerProperty m_HardScatterSplittingMode{this, "HardScatterSplittingMode", 0, "Control pileup & signal splitting. Process all SiHit or just those from signal or background events"};
  BooleanProperty m_WriteSCT1_RawData{this, "WriteSCT1_RawData", false, "Write out SCT1_RawData rather than SCT3_RawData"};

  BooleanProperty m_onlyUseContainerName{this, "OnlyUseContainerName", true, "Don't use the ReadHandleKey directly. Just extract the container name from it."};
  SG::ReadHandleKey<SiHitCollection> m_hitsContainerKey{this, "InputObjectName", "StripHits", "Input HITS collection name"};
  std::string m_inputObjectName{""};

  SG::WriteHandleKey<SCT_RDO_Container> m_rdoContainerKey{this, "OutputObjectName", "SCT_RDOs", "Output Object name"};
  SG::WriteHandle<SCT_RDO_Container> m_rdoContainer; //!< RDO container handle
  SG::WriteHandleKey<InDetSimDataCollection> m_simDataCollMapKey{this, "OutputSDOName", "StripSDO_Map", "Output SDO container name"};
  SG::WriteHandle<InDetSimDataCollection>            m_simDataCollMap; //!< SDO Map handle
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_stripDetEleCollKey{this, "StripDetEleCollKey", "ITkStripDetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};

  ToolHandle<IFrontEnd> m_sct_FrontEnd{this, "FrontEnd", "StripFrontEnd", "Handle the Front End Electronic tool"};
  ToolHandle<ISurfaceChargesGenerator> m_sct_SurfaceChargesGenerator{this, "SurfaceChargesGenerator", "StripSurfaceChargesGenerator", "Choice of using a more detailed charge drift model"};
  ToolHandle<IRandomDisabledCellGenerator> m_sct_RandomDisabledCellGenerator{this, "RandomDisabledCellGenerator", "StripRandomDisabledCellGenerator", ""};
  ServiceHandle<IAthRNGSvc> m_rndmSvc{this, "RndmSvc", "AthRNGSvc", ""};  //!< Random number service
  ServiceHandle <PileUpMergeSvc> m_mergeSvc{this, "MergeSvc", "PileUpMergeSvc", "Merge service used in Pixel & SCT digitization"}; //!

  const SCT_ID* m_detID{nullptr}; //!< Handle to the ID helper
  std::unique_ptr<TimedHitCollection<SiHit>> m_thpcsi{nullptr};
  std::vector<ISiChargedDiodesProcessorTool*> m_diodeCollectionTools;
  std::vector<bool> m_processedElements; //!< vector of processed elements - set by digitizeHits() */
  std::vector<std::unique_ptr<SiHitCollection>> m_hitCollPtrs;
  bool m_HardScatterSplittingSkipper{false};
};

static const InterfaceID IID_IStripDigitizationTool("StripDigitizationTool", 1, 0);
inline const InterfaceID& StripDigitizationTool::interfaceID() {
  return IID_IStripDigitizationTool;
}

} // namespace ITk

#endif // not STRIPDIGITIZATION_STRIPDIGITIZATIONTOOL_H
