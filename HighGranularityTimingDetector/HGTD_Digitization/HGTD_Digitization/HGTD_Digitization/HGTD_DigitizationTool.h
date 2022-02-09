/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
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
 *   - the time window set in the digitization has an impact on the luminosity
 *     studies and should be agreed on with the relevant subgroups!
 *   - there is no addition of random hits due to noise yet, once this is added
 *     simulateNoisePerDetectorElement can be implemented
 */

#ifndef HGTD_DIGITZATION_HGTD_DIGITZATIONTOOL_H
#define HGTD_DIGITZATION_HGTD_DIGITZATIONTOOL_H

#include "PileUpTools/PileUpToolBase.h"
#include "PileUpTools/PileUpMergeSvc.h"

#include "AthenaKernel/IAthRNGSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "HGTD_Digitization/IHGTD_SurfaceChargesGenerator.h"
#include "HGTD_ReadoutGeometry/HGTD_DetectorElementCollection.h"
#include "HGTD_RawData/HGTD_RDOCollection.h"
#include "HGTD_RawData/HGTD_RDOContainer.h"
#include "HitManagement/TimedHitCollection.h"
#include "InDetSimData/InDetSimDataCollection.h"
#include "InDetSimEvent/SiHitCollection.h"

#include <memory>
#include <string>

class HGTD_ID;
class SiChargedDiodeCollection;
class InDetSimDataCollection;
class IFrontEnd;
class ISiChargedDiodesProcessorTool;

namespace CLHEP {
class HepRandomEngine;
}

class HGTD_DigitizationTool : virtual public IPileUpTool,
                              public PileUpToolBase {
public:
  static const InterfaceID& interfaceID();

  HGTD_DigitizationTool(const std::string& type, const std::string& name,
                        const IInterface* parent);

  //////////////////////////////////////////////////////////////////////////////
  /*** Methods completing the IPileUpTool interfaces ***/

  virtual StatusCode initialize() override final;

  virtual StatusCode processAllSubEvents(const EventContext& ctx) override final;

  virtual StatusCode prepareEvent(const EventContext& ctx, unsigned int) override final;

  virtual StatusCode processBunchXing(int bunch_xing,
                                      SubEventIterator sub_event_itr,
                                      SubEventIterator sub_event_itr_end) override final;

  virtual StatusCode mergeEvent(const EventContext& ctx) override final;
  //////////////////////////////////////////////////////////////////////////////

private:
  //////////////////////////////////////////////////////////////////////////////
  ////////////////////////////     WORKHORSES     //////////////////////////////

  /** @brief Retrieve the SiHit collection and place it in a TimedHitCollection.
   */
  StatusCode retrieveHitCollection(const EventContext& ctx);

  StatusCode digitizeHitsPerDetectorElement(const EventContext& ctx, CLHEP::HepRandomEngine* rndmEngine);

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
  void applyProcessorTools(SiChargedDiodeCollection*, CLHEP::HepRandomEngine* rndmEngine) const;

  std::unique_ptr<HGTD_RDOCollection> createRDOCollection(SiChargedDiodeCollection*) const;

  StatusCode storeRDOCollection(std::unique_ptr<HGTD_RDOCollection>);

  // inserts the created SDO elements into the m_sdo_collection_map object.
  void createAndStoreSDO(SiChargedDiodeCollection* collection);

  //////////////////////////////////////////////////////////////////////////////

  FloatProperty m_integrated_luminosity{this, "IntegratedLuminosity", 0, "Integrated Luminosity"};
  BooleanProperty m_smear_meantime{this, "SmearMeanTime", true, "Smear mean time based on radius and luminosity"};
  /** @brief Only SiHits within the defined time window are processed in the
   * digitization procedure, the rest is discarded. Given in ns.*/
  FloatProperty m_active_time_window{this, "ActiveTimeWindow", 10000, "Hits within this time window are used for digitization"};
  FloatProperty m_charge_threshold{this, "ChargeThreshold", 625., "Minimum charge threshold in ASIC"};

  BooleanProperty m_onlyUseContainerName{this, "OnlyUseContainerName", true, "Don't use the ReadHandleKey directly. Just extract the container name from it."};
  SG::ReadHandleKey<SiHitCollection> m_hitsContainerKey{this, "InputObjectName", "HGTD_Hits", "Input HITS collection name"};
  std::string m_inputObjectName{""};

  SG::WriteHandle<HGTD_RDOContainer> m_hgtd_rdo_container; //!< RDO container handle
  SG::WriteHandleKey<HGTD_RDOContainer> m_output_rdo_cont_key{this, "OutputObjectName", "HGTD_RDOs", "Output Object name"};
  SG::WriteHandle<InDetSimDataCollection> m_sdo_collection_map; //!< SDO Map handle
  SG::WriteHandleKey<InDetSimDataCollection> m_output_sdo_coll_key{this, "OutputSDOName", "HGTD_SDO_Map", "Output SDO container name"};
  SG::ReadCondHandleKey<InDetDD::HGTD_DetectorElementCollection> m_HGTDDetEleCollKey{this, "HGTDDetEleCollKey", "HGTD_DetectorElementCollection", "Key of HGTD_DetectorElementCollection for HGTD"};

  ServiceHandle<PileUpMergeSvc> m_merge_svc{this, "MergeSvc", "PileUpMergeSvc", "Merge service used in Pixel & HGTD digitization"}; //!
  ServiceHandle<IAthRNGSvc> m_rndm_svc{this, "RndmSvc", "AthRNGSvc", "Random Number Service used in HGTD & Pixel digitization"};  //!< Random number service
  std::list<ISiChargedDiodesProcessorTool*> m_diode_processor_tools;
  ToolHandle<IFrontEnd> m_hgtd_front_end_tool{this, "FrontEnd", "HGTD_FrontEndTool", "Tool for pulse shape simulation"};
  ToolHandle<IHGTD_SurfaceChargesGenerator> m_hgtd_surf_charge_gen{this, "SurfaceChargesGenerator", "HGTD_SurfaceChargesGenerator", "Choice of using a more detailed charge drift model"};

  const HGTD_ID* m_id_helper{nullptr}; //!< Handle to the ID helper
  // NB: even though it is a flat collection, it acts more like a container,
  // since the actual collections can be retrieved one by one with the
  // nextDetectorElement method.
  std::unique_ptr<TimedHitCollection<SiHit>> m_timed_hit_coll{nullptr};
  /** @brief In order to process all element rather than just those with hits we
   * create a vector to keep track of which elements have been processed.
   * This is useful for adding random noise (?!)
   * NB. an element is an hgtd module */
  std::vector<bool> m_processed_elements; //!
  // FIXME is this meant as a "keapalive" vector?
  std::vector<std::unique_ptr<SiHitCollection>> m_hit_coll_ptrs{};
};

static const InterfaceID IID_IHGTD_DigitizationTool("HGTD_DigitizationTool", 1, 0);
inline const InterfaceID& HGTD_DigitizationTool::interfaceID() {
  return IID_IHGTD_DigitizationTool;
}

#endif // HGTD_DIGITZATION_HGTD_DIGITZATIONTOOL_H
