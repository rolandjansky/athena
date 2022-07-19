/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONDIGITIZATION_STGC_DIGITIZATIONTOOL_H
#define MUONDIGITIZATION_STGC_DIGITIZATIONTOOL_H

/** @class sTgcDigitizationTool

    @section sTGC_DigitizerDetails Class methods and properties


    In the initialize() method...
    In the execute() method...

*/

#include "PileUpTools/PileUpMergeSvc.h"
#include "PileUpTools/PileUpToolBase.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "HitManagement/TimedHitCollection.h"
#include "MuonSimEvent/sTGCSimHitCollection.h"
#include "MuonSimEvent/sTGCSimHit.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventAuxInfo.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonDigitContainer/sTgcDigitContainer.h"
#include "NSWCalibTools/INSWCalibSmearingTool.h"
#include "NSWCalibTools/INSWCalibTool.h"
#include "CLHEP/Random/RandGaussZiggurat.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "AthenaKernel/IAthRNGSvc.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "MuonCondData/NswCalibDbThresholdData.h"

#include "sTGC_Digitization/sTgcDigitMaker.h"

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <memory>

/*******************************************************************************/
namespace MuonGM{
  class MuonDetectorManager;
}
namespace CLHEP {
  class HepRandomEngine;
}

class sTgcHitIdHelper;

/*******************************************************************************/
class sTgcDigitizationTool : public PileUpToolBase {

public:
  sTgcDigitizationTool(const std::string& type, const std::string& name, const IInterface* parent);

  /** Initialize */
  virtual StatusCode initialize();

  // /** When being run from PileUpToolsAlgs, this method is called at the start of
  //       the subevts loop. Not able to access SubEvents */
  StatusCode prepareEvent(const EventContext& ctx, const unsigned int /*nInputEvents*/);
  //
  //   /** When being run from PileUpToolsAlgs, this method is called for each active
  //       bunch-crossing to process current SubEvents bunchXing is in ns */
  StatusCode  processBunchXing(int bunchXing,
                               SubEventIterator bSubEvents,
                               SubEventIterator eSubEvents);

  //   /** When being run from PileUpToolsAlgs, this method is called at the end of
  //       the subevts loop. Not (necessarily) able to access SubEvents */
  StatusCode mergeEvent(const EventContext& ctx);
  /** alternative interface which uses the PileUpMergeSvc to obtain
      all the required SubEvents. */
  virtual StatusCode processAllSubEvents(const EventContext& ctx);

  /** Just calls processAllSubEvents - leaving for back-compatibility
      (IMuonDigitizationTool) */

  /**
     reads GEANT4 hits from StoreGate in each of detector
     components corresponding to sTGC modules which are triplets
     or doublets. A triplet has tree sensitive volumes and a
     double has two. This method calls
     sTgcDigitMaker::executeDigi, which digitizes every hit, for
     every readout element, i.e., a sensitive volume of a
     chamber. (IMuonDigitizationTool)
  */
  StatusCode digitize(const EventContext& ctx);

private:
  CLHEP::HepRandomEngine* getRandomEngine(const std::string& streamName, const EventContext& ctx) const;

  /** Get next event and extract collection of hit collections */
  StatusCode getNextEvent(const EventContext& ctx);
  /** Core part of digitization use by mergeEvent (IPileUpTool) and digitize (IMuonDigitizationTool) */
  StatusCode doDigitization(const EventContext& ctx);

  ServiceHandle<PileUpMergeSvc> m_mergeSvc{this, "MergeSvc", "PileUpMergeSvc", "Merge service used in digitization"};
  ServiceHandle<IAthRNGSvc> m_rndmSvc{this, "RndmSvc", "AthRNGSvc", "Random Number Service used in Muon digitization"};
  const sTgcHitIdHelper* m_hitIdHelper{}; // not owned here
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  const MuonGM::MuonDetectorManager* m_mdManager{}; // not owned here
  std::unique_ptr<sTgcDigitMaker> m_digitizer{};
  std::unique_ptr<TimedHitCollection<sTGCSimHit>> m_thpcsTGC{};
  std::vector<std::unique_ptr<sTGCSimHitCollection>> m_STGCHitCollList{};

  ToolHandle<Muon::INSWCalibSmearingTool> m_smearingTool{this,"SmearingTool","Muon::NSWCalibSmearingTool/STgcCalibSmearingTool"};
  ToolHandle<Muon::INSWCalibTool> m_calibTool{this,"CalibrationTool","Muon::NSWCalibTool/NSWCalibTool"};

  SG::WriteHandleKey<sTgcDigitContainer> m_outputDigitCollectionKey{this,"OutputObjectName","sTGC_DIGITS","WriteHandleKey for Output sTgcDigitContainer"}; // name of the output digits
  SG::WriteHandleKey<MuonSimDataCollection> m_outputSDO_CollectionKey{this,"OutputSDOName","sTGC_SDO","WriteHandleKey for Output MuonSimDataCollection"}; // name of the output SDOs
  
  Gaudi::Property<bool> m_doSmearing{this,"doSmearing",false};
  Gaudi::Property<bool> m_needsMcEventCollHelper{this,"UseMcEventCollectionHelper",false};
  Gaudi::Property<bool> m_doToFCorrection{this,"doToFCorrection",false};
  Gaudi::Property<bool> m_doEfficiencyCorrection{this,"doEfficiencyCorrection",false};

  Gaudi::Property<std::string> m_rndmEngineName{this,"RndmEngine","MuonDigitization","Random engine name"};

  Gaudi::Property<bool> m_onlyUseContainerName{this, "OnlyUseContainerName", true, "Don't use the ReadHandleKey directly. Just extract the container name from it."};
  SG::ReadHandleKey<sTGCSimHitCollection> m_hitsContainerKey{this, "InputObjectName", "sTGC_Hits", "name of the input object"};
  std::string m_inputObjectName{""};

  Gaudi::Property<bool> m_useCondThresholds{this, "useCondThresholds", true, "Use conditions data to get VMM charge threshold values"};
  SG::ReadCondHandleKey<NswCalibDbThresholdData> m_condThrshldsKey {this, "CondThrshldsKey", "NswCalibDbThresholdData", "Key of NswCalibDbThresholdData object containing calibration data (VMM thresholds)"};

  Gaudi::Property<int> m_doChannelTypes{this,"doChannelTypes",3};

  Gaudi::Property<float> m_deadtimeStrip{this,"DeadtimeElectronicsStrip",50};
  Gaudi::Property<float> m_deadtimePad{this,"DeadtimeElectronicsPad",5};
  Gaudi::Property<float> m_timeWindowPad{this,"timeWindowPad",30};
  Gaudi::Property<float> m_timeWindowStrip{this,"timeWindowStrip",30};

  Gaudi::Property<double> m_energyDepositThreshold{this,"energyDepositThreshold",300.0*CLHEP::eV,"Minimum energy deposit for hit to be digitized"};
  Gaudi::Property<double> m_limitElectronKineticEnergy{this,"limitElectronKineticEnergy",5.0*CLHEP::MeV,"Minimum kinetic energy for electron hit to be digitized"};

  const float m_chargeThreshold{0.02f};

  const float m_timeJitterElectronicsStrip{2.f}; //ns
  const float m_timeJitterElectronicsPad{2.f}; //ns
  const float m_hitTimeMergeThreshold{30.f}; //30ns = resolution of peak finding descriminator

  
  bool m_deadtimeON{true};
  bool m_produceDeadDigits{false};
  
  float m_deadtimeWire{5.f};
  float m_readtimeStrip{6.25f};
  float m_readtimePad{6.25f};
  float m_readtimeWire{6.25f};

  void readDeadtimeConfig();

  uint16_t bcTagging(const float digittime, const int channelType) const;
  int humanBC(uint16_t bctag);

};

#endif // MUONDIGITIZATION_STGC_DIGITIZATIONTOOL_H
