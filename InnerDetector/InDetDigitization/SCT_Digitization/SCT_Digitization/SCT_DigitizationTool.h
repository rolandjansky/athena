/* -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_DIGITZATION_SCT_DIGITZATIONTOOL_H
#define SCT_DIGITZATION_SCT_DIGITZATIONTOOL_H
/** @file SCT_DigitizationTool.h
 * @brief Digitize the SCT using an implementation of IPileUpTool
 * $Id: SCT_DigitizationTool.h,v 1.0 2009-09-22 18:34:42 jchapman Exp $
 * @author John Chapman - ATLAS Collaboration
 */

//Base class header
#include "PileUpTools/PileUpToolBase.h"

// Athena headers
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CommissionEvent/ComTime.h"
#include "HitManagement/TimedHitCollection.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetRawData/InDetRawDataCollection.h"
#include "InDetSimData/InDetSimDataCollection.h"
#include "InDetSimEvent/SiHitCollection.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventAuxInfo.h"

// Gaudi headers
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandleKey.h"

// STL headers
#include "boost/shared_ptr.hpp"
#include <string>

class InDetSimDataCollection;

// Forward declarations
class AtlasDetectorID; //FIXME should be removed
class ComTime;
class SCT_ID;

class ISCT_FrontEnd;
class ISCT_SurfaceChargesGenerator;
class ISCT_RandomDisabledCellGenerator;
class ISiSurfaceChargesInserter;

class SiChargedDiodeCollection;
class ISiChargedDiodesProcessorTool;
class StoreGateService;

namespace InDetDD
{
  class SCT_DetectorManager;
  class SiDetectorElement;
}

namespace CLHEP
{
  class HepRandomEngine;
}

static const InterfaceID IID_ISCT_DigitizationTool ("SCT_DigitizationTool", 1, 0);

class SCT_DigitizationTool :
  virtual public IPileUpTool,
  public PileUpToolBase
{
public:
  static const InterfaceID& interfaceID();
  SCT_DigitizationTool(const std::string& type,
                       const std::string& name,
                       const IInterface* parent);
  /**
     @brief Called before processing physics events
  */
  StatusCode prepareEvent(unsigned int) override final;
  virtual StatusCode processBunchXing(int bunchXing,
                                      SubEventIterator bSubEvents,
                                      SubEventIterator eSubEvents) override final;
  virtual StatusCode mergeEvent() override final;

  virtual StatusCode initialize() override final;
  virtual StatusCode processAllSubEvents() override final;

protected:

  bool       digitizeElement(SiChargedDiodeCollection* chargedDiodes); //!
  void       applyProcessorTools(SiChargedDiodeCollection* chargedDiodes); //!
  void       addSDO(SiChargedDiodeCollection* collection);

  void storeTool(ISiChargedDiodesProcessorTool* p_processor) {m_diodeCollectionTools.push_back(p_processor);}
  void store(const AtlasDetectorID* p_helper) {m_atlasID = p_helper;}  //FIXME should be removed

private:

  /**
     @brief initialize the required services
  */
  StatusCode initServices();
  /**
     @brief initialize the random number engine
  */
  StatusCode initRandomEngine();
  /**
     @brief Initialize the SCT_FrontEnd AlgTool
  */
  StatusCode initFrontEndTool();
  /**
     @brief Initialize the SCT_RandomDisabledCellGenerator AlgTool
  */
  StatusCode initDisabledCells();
  /**
     @brief Initialize the SCT_SurfaceChargesGenerator AlgTool
  */
  StatusCode initSurfaceChargesGeneratorTool();

  /** RDO and SDO methods*/
  /**
     @brief Create RDOs from the SiChargedDiodeCollection for the current wafer and save to StoreGate
     @param chDiodeCollection       list of the SiChargedDiodes on the current wafer
  */
  StatusCode createAndStoreRDO(SiChargedDiodeCollection* chDiodeCollection);
  /**
     @brief Create RDOs from the SiChargedDiodeCollection for the current wafer
     @param chDiodeCollection       list of the SiChargedDiodes on the current wafer
  */

  SCT_RDO_Collection* createRDO(SiChargedDiodeCollection* collection);

  StatusCode getNextEvent();
  void       digitizeAllHits();     //!< digitize all hits
  void       digitizeNonHits();     //!< digitize SCT without hits

  float m_tfix;           //!< Use fixed timing for cosmics
  float m_comTime;         //!< Use Commission time for timing

  bool m_enableHits;            //!< Flag to enable hits
  bool m_onlyHitElements;       //!<
  bool m_cosmicsRun;            //!< Select a cosmic run
  bool m_useComTime;            //!< Flag to set the use of cosmics time for timing
  bool m_barrelonly;            //!< Only the barrel layers
  bool m_randomDisabledCells;   //!< Use Random disabled cells, default no
  bool m_createNoiseSDO;        //!< Create SDOs for strips with only noise hits (huge increase in SDO collection size)
  int  m_HardScatterSplittingMode; //!< Process all SiHit or just those from signal or background events
  bool m_HardScatterSplittingSkipper;
  BooleanProperty m_WriteSCT1_RawData;     //!< Write out SCT1_RawData rather than SCT3_RawData RDOs

  std::vector<bool> m_processedElements; //!< vector of processed elements - set by digitizeHits() */
  /**
     @brief Called when m_WriteSCT1_RawData is altered. Does nothing, but required by Gaudi.
  */

  void SetupRdoOutputType(Property&);

  SG::ReadHandleKey<ComTime>        m_ComTimeKey ; //!< Handle to retrieve commissioning timing info from SG

  const SCT_ID*                                      m_detID;                             //!< Handle to the ID helper
  const InDetDD::SCT_DetectorManager*                m_detMgr;                            //!< Handle to Si detector manager
  ToolHandle<ISCT_FrontEnd>                          m_sct_FrontEnd;                      //!< Handle the Front End Electronic tool
  ToolHandle<ISCT_SurfaceChargesGenerator>           m_sct_SurfaceChargesGenerator;       //!< Handle the surface chage generator tool
  ToolHandle<ISCT_RandomDisabledCellGenerator>       m_sct_RandomDisabledCellGenerator;   //!< Handle the Ampilifier tool for the Front End

  std::vector<SiHitCollection*> m_hitCollPtrs;

  SG::WriteHandleKey<SCT_RDO_Container>              m_rdoContainerKey; //!< RDO container key
  SG::WriteHandle<SCT_RDO_Container>                 m_rdoContainer; //!< RDO container handle
  SG::WriteHandleKey<InDetSimDataCollection>         m_simDataCollMapKey; //!< SDO Map key
  SG::WriteHandle<InDetSimDataCollection>            m_simDataCollMap; //!< SDO Map handle

  std::string                                        m_inputObjectName;     //! name of the sub event  hit collections.
  ServiceHandle <IAtRndmGenSvc>                      m_rndmSvc;             //!< Random number service
  ServiceHandle <PileUpMergeSvc>                     m_mergeSvc; //!

  CLHEP::HepRandomEngine*                            m_rndmEngine;          //! Random number engine used - not init in SiDigitization
  const AtlasDetectorID*                             m_atlasID;  //FIXME should be replaced with m_detID usage
  std::list<ISiChargedDiodesProcessorTool*>          m_diodeCollectionTools;
  TimedHitCollection<SiHit>*                         m_thpcsi;
  SiChargedDiodeCollection*                          m_chargedDiodes;
  IntegerProperty                                    m_vetoThisBarcode;


};

inline const InterfaceID& SCT_DigitizationTool::interfaceID()
{
  return IID_ISCT_DigitizationTool;
}

#endif // SCT_DIGITZATION_SCT_DIGITZATIONTOOL_H
