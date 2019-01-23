/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONDIGITIZATION_STGC_DIGITIZATIONTOOL_H
#define MUONDIGITIZATION_STGC_DIGITIZATIONTOOL_H
/** @class sTgcDigitizationTool

    @section sTGC_DigitizerDetails Class methods and properties


    In the initialize() method...
    In the execute() method...

*/

#include "MuonDigToolInterfaces/IMuonDigitizationTool.h"
#include "PileUpTools/PileUpToolBase.h"

#include "AthenaKernel/IAtRndmGenSvc.h"
#include "HitManagement/TimedHitCollection.h"
#include "Identifier/Identifier.h"
#include "MuonSimEvent/sTGCSimHitCollection.h"
#include "MuonSimEvent/sTGCSimHit.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventAuxInfo.h"

#include "CLHEP/Random/RandGaussZiggurat.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Vector/ThreeVector.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>
#include <sstream>
#include <vector>
#include <map>

/*******************************************************************************/
namespace MuonGM{
  class MuonDetectorManager;
  class sTgcReadoutElement;
}
namespace CLHEP{
  class HepRandomEngine;
}

class StoreGateSvc;
class ActiveStoreSvc;
class PileUpMergeSvc;

class sTgcDigitContainer;
class sTgcDigitCollection;
class sTgcIdHelper;
class sTgcDigitMaker;
class sTgcHitIdHelper;
class IAtRndmGenSvc;

class TFile;
class TH2F;
class TH1F;

class MuonSimDataCollection;
/*******************************************************************************/
class sTgcDigitizationTool : virtual public IMuonDigitizationTool, public PileUpToolBase {

public:
  sTgcDigitizationTool(const std::string& type, const std::string& name, const IInterface* parent);

  /** Initialize */
  virtual StatusCode initialize();

  // /** When being run from PileUpToolsAlgs, this method is called at the start of
  //       the subevts loop. Not able to access SubEvents */
  StatusCode prepareEvent(const unsigned int /*nInputEvents*/);
  //
  //   /** When being run from PileUpToolsAlgs, this method is called for each active
  //       bunch-crossing to process current SubEvents bunchXing is in ns */
  StatusCode  processBunchXing(int bunchXing,
                               SubEventIterator bSubEvents,
                               SubEventIterator eSubEvents);

  //   /** When being run from PileUpToolsAlgs, this method is called at the end of
  //       the subevts loop. Not (necessarily) able to access SubEvents */
  StatusCode mergeEvent();
  /** alternative interface which uses the PileUpMergeSvc to obtain
      all the required SubEvents. */
  virtual StatusCode processAllSubEvents();

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
  StatusCode digitize();

  /** Finalize */
  StatusCode finalize();

  /** accessors */
  ServiceHandle<IAtRndmGenSvc> getRndmSvc() const { return m_rndmSvc; }    // Random number service
  CLHEP::HepRandomEngine *getRndmEngine() const { return m_rndmEngine; } // Random number engine used

private:
  /** Get next event and extract collection of hit collections */
  StatusCode getNextEvent();
  /** Record sTgcDigitContainer and MuonSimDataCollection */
  StatusCode recordDigitAndSdoContainers();
  /** Core part of digitization use by mergeEvent (IPileUpTool) and digitize (IMuonDigitizationTool) */
  StatusCode doDigitization();

protected:
  PileUpMergeSvc *m_mergeSvc; // Pile up service
  CLHEP::HepRandomEngine *m_rndmEngine;    // Random number engine used - not init in SiDigitization
  ServiceHandle<IAtRndmGenSvc> m_rndmSvc;      // Random number service
  std::string m_rndmEngineName;// name of random engine

private:
  ServiceHandle<StoreGateSvc>              m_sgSvc;
  ActiveStoreSvc*                          m_activeStore;
  sTgcHitIdHelper*                         m_hitIdHelper;
  sTgcDigitContainer*                      m_digitContainer;
  const sTgcIdHelper*                      m_idHelper;
  const MuonGM::MuonDetectorManager*       m_mdManager;
  sTgcDigitMaker*                          m_digitizer;
  TimedHitCollection<sTGCSimHit>*   m_thpcsTGC;
  MuonSimDataCollection*                   m_sdoContainer;
  std::list<sTGCSimHitCollection*>  m_STGCHitCollList;

  std::string m_inputHitCollectionName; // name of the input objects
  std::string m_outputDigitCollectionName; // name of the output digits
  std::string m_outputSDO_CollectionName; // name of the output SDOs
  bool m_needsMcEventCollHelper;

  bool m_doToFCorrection;
  int m_doChannelTypes;
  //double m_noiseFactor;
  float m_readoutThreshold;
  float m_neighborOnThreshold;
  float m_saturation;
  
  //float m_ADC;
  bool  m_deadtimeON;
  bool  m_produceDeadDigits;
  float m_deadtimeStrip;
  float m_deadtimePad;
  float m_deadtimeWire;
  float m_readtimeStrip;
  float m_readtimePad;
  float m_readtimeWire;
  float m_timeWindowOffsetPad;
  float m_timeWindowOffsetStrip;
  float m_timeWindowPad;
  float m_timeWindowStrip;
  float m_bunchCrossingTime;
  float m_timeJitterElectronicsStrip;
  float m_timeJitterElectronicsPad;
  float m_hitTimeMergeThreshold;

  // Minimum energy deposit for hit to be digitized
  double m_energyDepositThreshold;

  std::map< Identifier, int > m_hitSourceVec;

  void readDeadtimeConfig();

  uint16_t bcTagging(const float digittime, const int channelType) const;
  int humanBC(uint16_t bctag);

  //TFile *m_file;
  //TH2F *m_SimHitOrg, *m_SimHitMerged, *m_SimHitDigitized, *m_SimHitDigitizedwPad, *m_SimHitDigitizedwoPad;
  //TH1F *m_kineticEnergy, *m_EnergyDeposit;

};

#endif // MUONDIGITIZATION_STGC_DIGITIZATIONTOOL_H
