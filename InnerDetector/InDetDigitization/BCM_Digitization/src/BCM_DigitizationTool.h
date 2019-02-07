/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BCM_DIGITIZATION_BCM_DIGITIZATIONTOOL_H
#define BCM_DIGITIZATION_BCM_DIGITIZATIONTOOL_H

#include "PileUpTools/PileUpToolBase.h"
#include "AthenaKernel/IAthRNGSvc.h"

#include "GaudiKernel/ServiceHandle.h"

#include "InDetSimData/InDetSimData.h"
#include "InDetSimEvent/SiHit.h"

#include "InDetBCM_RawData/BCM_RDO_Container.h"
#include "InDetBCM_RawData/InDetBCM_RawDataCLASS_DEF.h"

#include "CLHEP/Geometry/Point3D.h"

#include <bitset>

// Data member classes
class PileUpMergeSvc;
class InDetSimDataCollection;

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
  virtual StatusCode processAllSubEvents() override final;
  /** PileUpToolBase methods */
  virtual StatusCode prepareEvent(unsigned int nInputEvents) override;
  virtual StatusCode processBunchXing(int bunchXing,
                                      SubEventIterator bSubEvents,
                                      SubEventIterator eSubEvents) override final;
  virtual StatusCode mergeEvent() override final;

 private:
  /** Create the RDO and SDO containers */
  StatusCode createOutputContainers();
  void processSiHit(const SiHit &currentHit, double eventTime, unsigned int evtIndex);
  void createRDOsAndSDOs();

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
  std::string m_hitCollName;      //!< Input simulation hit collection name
  std::vector<float> m_modNoise;  //!< RMS Gaussian noise
  std::vector<float> m_modSignal; //!< Most probable MIP signal
  std::vector<float> m_ninoThr;   //!< NINO threshold
  float m_mipDeposit;             //!< Most probable MIP deposit in BCM pad
  float m_effPrmDistance;         //!< Distance parameter in charge collection efficiency function
  float m_effPrmSharpness;        //!< Sharpness parameter in charge collection efficiency function
  float m_timeDelay;              //!< Time delay

  // Output objects
  BCM_RDO_Container* m_rdoContainer; //!< Output RDO container
  InDetSimDataCollection* m_simDataCollMap; //!< Output SDO map

  PileUpMergeSvc* m_mergeSvc; //!< Handle for pileup merging service
  ServiceHandle<IAthRNGSvc> m_rndmGenSvc{this, "RndmSvc", "AthRNGSvc", ""};  //!< Random number service

  // Vectors to store G4 hit information
  std::vector<float> m_enerVect[8]; //!< G4 hit energies, weighted
  std::vector<float> m_timeVect[8]; //!< G4 hit times
  std::vector<InDetSimData::Deposit> m_depositVect[8]; //!< Deposit vectors for SDO map
};

#endif // BCM_DIGITIZATION_BCM_DIGITIZATIONTOOL_H
