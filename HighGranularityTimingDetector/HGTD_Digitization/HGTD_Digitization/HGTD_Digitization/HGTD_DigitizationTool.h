/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_Digitization/HGTD_DigitizationTool.h
 *
 * @author Tao Wang <tao.wang@cern.ch>
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 *
 * @date August, 2021
 *
 * @brief
 *
 *  TODO:
 *   - FIXME: For Rel22: change IAtRndmGenSvc to IAthRNGSvc!!!
 *
 *   - the time window set in the digitization has an impact on the luminosity
 *     studies and should be agreed on with the relevant subgroups!
 *   - there is no addition of random hits due to noise yet, once this is added
 * simulateNoisePerDetectorElement can be implemented
 */

#ifndef HGTD_DIGITZATION_HGTD_DIGITZATIONTOOL_H
#define HGTD_DIGITZATION_HGTD_DIGITZATIONTOOL_H

#include "PileUpTools/PileUpToolBase.h"

#include "AthenaKernel/IAtRndmGenSvc.h"
// #include "HGTD_Digitization/IHGTD_SurfaceChargesGenerator.h"
#include "HGTD_RawData/HGTD_RDOCollection.h"
#include "HGTD_RawData/HGTD_RDOContainer.h"
#include "HitManagement/TimedHitCollection.h"
#include "InDetSimEvent/SiHitCollection.h"

#include <memory>
#include <string>

class HGTD_ID;
class HGTD_DetectorManager;
class SiChargedDiodeCollection;
class InDetSimDataCollection;
class IFrontEnd;
class ISiChargedDiodesProcessorTool;

namespace CLHEP {
class HepRandomEngine;
}

static const InterfaceID IID_IHGTD_DigitizationTool("HGTD_DigitizationTool", 1,
                                                    0);

class HGTD_DigitizationTool : virtual public IPileUpTool,
                              public PileUpToolBase {
public:
  static const InterfaceID& interfaceID();
  
  HGTD_DigitizationTool(const std::string& type, const std::string& name,
                        const IInterface* parent);

  //////////////////////////////////////////////////////////////////////////////
  /*** Methods completing the IPileUpTool interfaces ***/

  virtual StatusCode initialize() override final;

  virtual StatusCode processAllSubEvents() override final;

  virtual StatusCode prepareEvent(unsigned int) override final;

  virtual StatusCode
  processBunchXing(int bunch_xing, SubEventIterator sub_event_itr,
                   SubEventIterator sub_event_itr_end) override final;

  virtual StatusCode mergeEvent() override final;
  //////////////////////////////////////////////////////////////////////////////

private:
  //////////////////////////////////////////////////////////////////////////////
  ////////////////////////////     WORKHORSES     //////////////////////////////

  /** @brief Retrieve the SiHit collection and place it in a TimedHitCollection.
   */
  StatusCode retrieveHitCollection();

  StatusCode digitizeHitsPerDetectorElement();

  /** @brief FIXME: will be implemented once we know how much noise we expect.
   *
   */
  // StatusCode simulateNoisePerDetectorElement();

  /** @brief Step by step processor tools applied on diodes.
   *  These should simulate: Preamplifier -> Discriminator -> TDC.
   *  Is called within digitizeHitsPerDetectorElement.
   *
   * Used tools: FrontEndTool, which calls the Amplifier inside
   */
  void applyProcessorTools(SiChargedDiodeCollection*);

  std::unique_ptr<HGTD::HGTD_RDOCollection>
  createRDOCollection(SiChargedDiodeCollection*);

  StatusCode storeRDOCollection(std::unique_ptr<HGTD::HGTD_RDOCollection>);

  // inserts the created SDO elements into the m_sdo_collection object.
  void createAndStoreSDO(SiChargedDiodeCollection* collection);

  //////////////////////////////////////////////////////////////////////////////

  ServiceHandle<PileUpMergeSvc> m_merge_svc;

  const HGTD_ID* m_id_helper;
  const HGTD_DetectorManager* m_det_mgr;

  HGTD::HGTD_RDOContainer* m_hgtd_rdo_container;

  InDetSimDataCollection* m_sdo_collection;
  std::string m_output_sdo_coll_name;

  /** @brief In order to process all element rather than just those with hits we
   * create a vector to keep track of which elements have been processed.
   * This is useful for adding random noise (?!)
   * NB. an element is an hgtd module */
  std::vector<bool> m_processed_elements;

  /** @brief Name of the sub event SiHit collection. */
  std::string m_input_collection_name;

  // NB: even though it is a flat collection, it acts more like a container,
  // since the actual collections can be retrieved one by one with the
  // nextDetectorElement method.
  std::unique_ptr<TimedHitCollection<SiHit>> m_timed_hit_coll;

  // FIXME is this meant as a "keapalive" vector?
  std::vector<SiHitCollection*> m_hit_coll_ptrs;

  /** @brief This collection stores the energy deposits in their
   * digitized form, all the tools are run over it. */
  // TODO: maybe this can be a local member??
  // std::unique_ptr<SiChargedDiodeCollection> m_charged_diodes;

  /** @brief Only SiHits within the defined time window are processed in the
   * digitization procedure, the rest is discarded. Given in ns.*/
  float m_active_time_window;

  ServiceHandle<IAtRndmGenSvc> m_rndm_svc;

  CLHEP::HepRandomEngine* m_rndm_engine;

  // ToolHandle<IHGTD_SurfaceChargesGenerator> m_hgtd_surf_charge_gen;

  ToolHandle<IFrontEnd> m_hgtd_front_end_tool;

  std::list<ISiChargedDiodesProcessorTool*> m_diode_processor_tools;
};

inline const InterfaceID& HGTD_DigitizationTool::interfaceID() {
  return IID_IHGTD_DigitizationTool;
}

#endif // HGTD_DIGITZATION_HGTD_DIGITZATIONTOOL_H
