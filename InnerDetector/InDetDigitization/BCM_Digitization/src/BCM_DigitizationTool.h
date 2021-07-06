/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BCM_DIGITIZATION_BCM_DIGITIZATIONTOOL_H
#define BCM_DIGITIZATION_BCM_DIGITIZATIONTOOL_H

#include "PileUpTools/PileUpToolBase.h"
#include "AthenaKernel/IAthRNGSvc.h"

#include "GaudiKernel/ServiceHandle.h"

#include "PileUpTools/PileUpMergeSvc.h"

#include "InDetSimData/InDetSimData.h"
#include "InDetSimData/InDetSimDataCollection.h"
#include "InDetSimEvent/SiHitCollection.h"
#include "InDetSimEvent/SiHit.h"

#include "InDetBCM_RawData/BCM_RDO_Container.h"
#include "InDetBCM_RawData/InDetBCM_RawDataCLASS_DEF.h"

#include "CLHEP/Geometry/Point3D.h"

#include <bitset>

namespace CLHEP
{
  class HepRandomEngine;
}


/** Pileup tool for BCM digitization */
class BCM_DigitizationTool : public PileUpToolBase {

 public:

  /** Constructor with parameters */
  BCM_DigitizationTool(const std::string& type, const std::string& name,const IInterface* parent);

  virtual StatusCode initialize() override final;
  /** alternative interface which uses the PileUpMergeSvc to obtain
  all the required SubEvents. */
  virtual StatusCode processAllSubEvents(const EventContext& ctx) override final;
  /** PileUpToolBase methods */
  virtual StatusCode prepareEvent(const EventContext& ctx, unsigned int) override final;
  virtual StatusCode processBunchXing(int bunchXing,
                                      SubEventIterator bSubEvents,
                                      SubEventIterator eSubEvents) override final;
  virtual StatusCode mergeEvent(const EventContext& ctx) override final;

 private:
  /** Create the RDO and SDO containers */
  StatusCode createOutputContainers(const EventContext& ctx);
  void processSiHit(const SiHit &currentHit, double eventTime, unsigned int evtIndex, const EventContext& ctx);
  void createRDOsAndSDOs(const EventContext& ctx);

  /** Compute energy deposit depending on hit position */
  float computeEnergy(float simEner, HepGeom::Point3D<double> startPos, HepGeom::Point3D<double> endPos);

  /** Fill in hit pulses on analog waveform */
  std::vector<float> createAnalog(int mod, std::vector<float> enerVect, std::vector<float> timeVect);

  /** Add noise to analog waveform */
  void addNoise(int mod, std::vector<float> &analog, CLHEP::HepRandomEngine *randomEngine);

  /** Do ToT digitization */
  std::bitset<64> applyThreshold(int chan, std::vector<float> analog);

  /** Apply hole and spike filter to digital waveform */
  void applyFilter(std::bitset<64> &digital);

  /** Find first two pulses on digital waveform */
  void findPulses(std::bitset<64> digital, int &p1x, int &p1w, int &p2x, int &p2w);

  /** Create raw data object and put it in the container */
  void fillRDO(unsigned int chan, int p1x, int p1w, int p2x, int p2w);

  // Digitization parameters
  std::vector<float> m_modNoise;  //!< RMS Gaussian noise
  std::vector<float> m_modSignal; //!< Most probable MIP signal
  std::vector<float> m_ninoThr;   //!< NINO threshold
  Gaudi::Property<float> m_mipDeposit{this, "MIPDeposit", 0.0f, "Most probable MIP deposit in BCM pad"};
  Gaudi::Property<float> m_effPrmDistance{this, "EffDistanceParam", 0.0f, "Distance parameter for efficiency calculation"};
  Gaudi::Property<float> m_effPrmSharpness{this, "EffSharpnessParam", 0.0f, "Sharpness parameter for efficiency calculation"};
  Gaudi::Property<float> m_timeDelay{this, "TimeDelay", 0.0f, "Pulse time delay"};

  BooleanProperty m_onlyUseContainerName{this, "OnlyUseContainerName", true, "Don't use the ReadHandleKey directly. Just extract the container name from it."};
  SG::ReadHandleKey<SiHitCollection> m_hitsContainerKey{this, "HitCollName", "BCMHits", "Input simulation hits collection name"};
  std::string m_inputObjectName{""};

  // Write handle keys
  SG::WriteHandleKey<BCM_RDO_Container> m_outputKey{this, "OutputRDOKey", "BCM_RDOs", ""};
  SG::WriteHandleKey<InDetSimDataCollection> m_outputSDOKey{this, "OutputSDOKey", "BCM_SDO_Map", ""};

  // Output objects
  BCM_RDO_Container* m_rdoContainer{}; //!< Output RDO container
  InDetSimDataCollection* m_simDataCollMap{}; //!< Output SDO map

  ServiceHandle <PileUpMergeSvc> m_mergeSvc{this, "MergeSvc", "PileUpMergeSvc", "Merge service used in digitization"}; //!
  ServiceHandle<IAthRNGSvc> m_rndmGenSvc{this, "RndmSvc", "AthRNGSvc", ""};  //!< Random number service

  // Vectors to store G4 hit information
  std::vector<float> m_enerVect[8]; //!< G4 hit energies, weighted
  std::vector<float> m_timeVect[8]; //!< G4 hit times
  std::vector<InDetSimData::Deposit> m_depositVect[8]; //!< Deposit vectors for SDO map
};

#endif // BCM_DIGITIZATION_BCM_DIGITIZATIONTOOL_H
