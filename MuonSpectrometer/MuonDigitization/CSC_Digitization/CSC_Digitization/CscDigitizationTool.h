/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONDIGITIZATION_CSCDIGITIZATIONTOOL_H
#define MUONDIGITIZATION_CSCDIGITIZATIONTOOL_H

#include <memory>

#include "PileUpTools/PileUpToolBase.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaKernel/IAthRNGSvc.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CSC_Digitization/CSC_Digitizer.h"
#include "MuonDigitContainer/CscDigitContainer.h"
#include "MuonSimEvent/CSCSimHit.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "HitManagement/TimedHitCollection.h"
#include "CscCalibTools/ICscCalibTool.h"
#include "MuonSimEvent/CSCSimHitCollection.h"
#include "MuonSimData/CscSimDataCollection.h"
#include "PileUpTools/PileUpMergeSvc.h"

#include "CSC_Digitizer.h"

// Author: Ketevi A. Assamagan
// BNL, October 27, 2003

// Digitization algorithm for the CSC:
// get the hit container from Storegate
// loop over the hits
// digitize each hit
// loop over the digit and build the digit container
// store the digits in StoreGate

class CscDigitizationTool : public PileUpToolBase {

  typedef std::vector<CscSimData::Deposit> deposits;
  typedef std::map<IdentifierHash,std::pair<double,double> > csc_map;
  typedef std::map<IdentifierHash,std::vector<float> > csc_newmap;


public:

  CscDigitizationTool(const std::string& type, const std::string& name, const IInterface* pIID);

  ~CscDigitizationTool() = default;

  virtual StatusCode initialize() override final;

  // PileUpTool methods...
  ///called at the end of the subevts loop. Not (necessarily) able to access subEvents
  virtual StatusCode mergeEvent(const EventContext& ctx)  override final;

  ///called for each active bunch-crossing to process current subEvents. bunchXing is in ns
  virtual  StatusCode processBunchXing(
                                       int bunchXing,
                                       SubEventIterator bSubEvents,
                                       SubEventIterator eSubEvents
                                       ) override final;

  /// return false if not interested in  certain xing times (in ns)
  /// implemented by default in PileUpToolBase as FirstXing<=bunchXing<=LastXing
  //  virtual bool toProcess(int bunchXing) const;
  virtual StatusCode prepareEvent(const EventContext& ctx, unsigned int /*nInputEvents*/)  override final;

  /// alternative interface which uses the PileUpMergeSvc to obtain
  /// all the required SubEvents.
  virtual StatusCode processAllSubEvents(const EventContext& ctx)  override final;

public: //possibly these should be private?
  StatusCode FillCollectionWithNewDigitEDM(csc_newmap& data_SampleMap, //csc_newmap& data_SampleMapOddPhase,
                                           std::map<IdentifierHash,deposits>& myDeposits,bool phaseToSet,
                                           CscDigitContainer* cscDigits,CscSimDataCollection* cscSimData);
  StatusCode FillCollectionWithOldDigitEDM(csc_map& data_map, std::map<IdentifierHash,deposits>& myDeposits,CscDigitContainer* cscDigits,CscSimDataCollection* cscSimData);

  StatusCode CoreDigitization(CscDigitContainer* cscDigits,CscSimDataCollection* cscSimData, CLHEP::HepRandomEngine* rndmEngine);

  // Get next event and extract collection of hit collections:
  StatusCode getNextEvent(const EventContext& ctx);

private:

  ToolHandle<ICscCalibTool> m_pcalib{this, "cscCalibTool", "CscCalibTool", "CSC calibration tool"};

  BooleanProperty m_onlyUseContainerName{this, "OnlyUseContainerName", true, "Don't use the ReadHandleKey directly. Just extract the container name from it."};
  SG::ReadHandleKey<CSCSimHitCollection> m_hitsContainerKey{this, "InputObjectName", "CSC_Hits", "name of the input objects"}; // name of the input objects
  std::string m_inputObjectName{""};
  SG::WriteHandleKey<CscSimDataCollection> m_cscSimDataCollectionWriteHandleKey{this,"CSCSimDataCollectionOutputName","CSC_SDO","WriteHandleKey for Output CscSimDataCollection"};
  SG::WriteHandleKey<CscDigitContainer> m_cscDigitContainerKey{this,"OutputObjectName","CSC_DIGITS","CSC digit container object"};

  const MuonGM::MuonDetectorManager* m_geoMgr{nullptr};
  std::unique_ptr<CSC_Digitizer> m_cscDigitizer{nullptr};

  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  Gaudi::Property<double> m_pedestal{this, "pedestal",0.0, ""};
  Gaudi::Property<bool> m_maskBadChannel{this, "maskBadChannels", true, ""};
  Gaudi::Property<double> m_amplification{this, "amplification", 0.58e5, ""};

  //pile-up
  TimedHitCollection<CSCSimHit>* m_thpcCSC{nullptr}; // the hits
  std::list<CSCSimHitCollection*> m_cscHitCollList; // only for pileup events..
  //pileup truth veto
  Gaudi::Property<bool>   m_includePileUpTruth{this, "IncludePileUpTruth", true, "Include pile-up truth info"};

  Gaudi::Property<double> m_timeWindowLowerOffset{this, "WindowLowerOffset", -25., ""};
  Gaudi::Property<double> m_timeWindowUpperOffset{this, "WindowUpperOffset", +25., ""};
  Gaudi::Property<bool>   m_isPileUp{this, "isPileUp", false, ""};
  Gaudi::Property<bool>   m_newDigitEDM{this, "NewDigitEDM", true, ""};
  Gaudi::Property<double> m_driftVelocity{this, "DriftVelocity", 60, ""}; // 60 / (1e-6 * 1e9); // 6 cm/microsecond -> mm/ns // 0.06
  Gaudi::Property<double> m_electronEnergy{this, "ElectronEnergy", 66, ""}; // eV
  Gaudi::Property<bool>   m_NInterFixed{this, "NInterFixed", false, ""};

  ServiceHandle<PileUpMergeSvc> m_mergeSvc{this, "PileUpMergeSvc", "PileUpMergeSvc", ""}; // Pile up service

  ServiceHandle <IAthRNGSvc> m_rndmSvc{this, "RndmSvc", "AthRNGSvc", ""};      // Random number service
};

#endif // MUONDIGITIZATION_CSCDIGITIZATIONTOOL_H
