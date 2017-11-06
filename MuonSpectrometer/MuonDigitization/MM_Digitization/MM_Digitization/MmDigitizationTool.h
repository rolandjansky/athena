/* -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_DIGITIZATIONTOOL_H
#define MM_DIGITIZATIONTOOL_H
/** @class MmDigitizationTool

    @section MM_DigitizerDetails Class methods and properties
    // ------------
    // Authors:
    //   Nektarios Chr. Benekos  <nectarios.benekos@cern.ch>
    //   Konstantinos Karakostas <Konstantinos.Karakostas@cern.ch>
    ////////////////////////////////////////////////////////////////////////////////

    In the initialize() method, the PileUpMerge and StoreGate services are initialized, and a pointer to an instance of the class
    MuonDetectorManager is retrieved from the detector store and used to obtain a MmIdHelper.
    The MMDigitContainer is initialized and the simulation identifier helper retrieved, together with the pointer to the digitization tool.
    Random numbers are obtained in the code from a dedicated stream via AtRndmSvc,
    which is also initialized in the initialize() method.
    In the execute() method, the digits and the SDOs (Simulation Data Object,
    container for simulation data to be preserved after the digitization procedue, and persistified together with the RDOs)
    containers are created and recorded on StoreGate; the GenericMuonSimHit collection are merged using the
    TimedHitCollection sorted container (done in handleMicroMegasSimhit(TimedHitPtr<GenericMuonSimHit>& hit)) method);
    into a loop over the TimedHitCollection for the given DetectorElement,
    the handleMicroMegasSimhit() method converts the SimID into the Offline ID to be associated to the Digit and pass to the
    digitization tool the drift radius and the distance to the chamber RO side (for the propagation delay computation).
    The digitization tool returns a drift time, charge and strip position which are used together with the Offline ID,
    to create the digit object (in doDigitization() method).
    The finalize() method returns a SUCCESS StatusCode if the digitization procedure ends succesfully.

    In the initialize() method...
    In the execute() method...
*/

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MagFieldInterfaces/IMagFieldSvc.h" // 15/06/2015 T.Saito

#include "HitManagement/TimedHitCollection.h"
#include "MuonSimEvent/GenericMuonSimHitCollection.h"
#include "MuonSimEvent/GenericMuonSimHit.h"
#include "PileUpTools/PileUpToolBase.h"
#include "Identifier/Identifier.h"

#include "MM_Digitization/MmSortedHitVector.h"

#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include "MuonDigToolInterfaces/IMuonDigitizationTool.h"
#include "MM_Digitization/StripsResponse.h"
#include "MM_Digitization/ElectronicsResponse.h"
#include "MM_Digitization/MMStripVmmMappingTool.h"

#include "xAODEventInfo/EventInfo.h"   // SubEventIterator
#include "xAODEventInfo/EventAuxInfo.h"// SubEventIterator

#include <string>
#include <sstream>
#include <vector>
#include <map>
/*******************************************************************************/
namespace MuonGM{
  class MuonDetectorManager;
  class MMReadoutElement;
  class MuonChannelDesign;
}
namespace CLHEP{
  class HepRandomEngine;
}

class StoreGateSvc;
class ActiveStoreSvc;
class PileUpMergeSvc;

class MmDigitContainer;
class MmDigitCollection;
class MmIdHelper;
class MicromegasHitIdHelper;
class IAtRndmGenSvc;
class MsgStream;

class IMM_DigitizationTool;
class MuonSimDataCollection;

class StripsResponse;
class ElectronicsResponse;

class TTree;
class TFile;

/*******************************************************************************/

class MmDigitizationTool : virtual public IMuonDigitizationTool, public PileUpToolBase {

public:
  MmDigitizationTool(const std::string& type, const std::string& name, const IInterface* parent);

  /** Initialize */
  virtual StatusCode initialize() override final;

  /** When being run from PileUpToolsAlgs, this method is called at the start of the subevts loop. Not able to access SubEvents */
  StatusCode prepareEvent(const unsigned int /*nInputEvents*/) override final;

  /** When being run from PileUpToolsAlgs, this method is called for each active bunch-crossing to process current SubEvents bunchXing is in ns */
#ifdef ATHENA_20_20
  StatusCode  processBunchXing(int bunchXing,
                               PileUpEventInfo::SubEvent::const_iterator bSubEvents,
                               PileUpEventInfo::SubEvent::const_iterator eSubEvents) override final;
#else
  StatusCode  processBunchXing(int bunchXing,
                               SubEventIterator bSubEvents,
                               SubEventIterator eSubEvents) override final;
#endif  

  /** When being run from PileUpToolsAlgs, this method is called at the end of the subevts loop. Not (necessarily) able to access SubEvents */
  StatusCode mergeEvent() override final;

  /** When being run from MM_Digitizer, this method is called during the event loop */

  /** alternative interface which uses the PileUpMergeSvc to obtain
      all the required SubEvents. */
  virtual StatusCode processAllSubEvents();

  /** Just calls processAllSubEvents - leaving for back-compatibility
      (IMuonDigitizationTool) */
  StatusCode digitize() override;

  /** Finalize */
  StatusCode finalize() override final;

  /** accessors */
  ServiceHandle<IAtRndmGenSvc> getRndmSvc() const { return m_rndmSvc; }    // Random number service
  CLHEP::HepRandomEngine  *getRndmEngine() const { return m_rndmEngine; } // Random number engine used

  void setAmplification(const double amplification) {
    m_amplification = amplification;
  }

  void set (const double bunchTime);



private:

  ServiceHandle<StoreGateSvc> m_sgSvc;
  ActiveStoreSvc*             m_activeStore;

  ServiceHandle<MagField::IMagFieldSvc>            m_magFieldSvc;

  /** Record MmDigitContainer and MuonSimDataCollection */
  StatusCode recordDigitAndSdoContainers();

  MmDigitContainer*      m_digitContainer;
  MuonSimDataCollection* m_sdoContainer;
  MmSortedHitVector      m_hits;

  const MmIdHelper*       m_idHelper;
  MicromegasHitIdHelper*  muonHelper;

  const MuonGM::MuonDetectorManager* m_MuonGeoMgr;

  ToolHandle <IMM_DigitizationTool> m_digitTool;
  std::list<GenericMuonSimHitCollection*> m_MMHitCollList;
  bool  checkMMSimHit(const GenericMuonSimHit& /* hit */ ) const;

  //CONFIGURATION
  bool m_validationSetup;
  double m_energyThreshold;
  bool m_saveInternalHistos;
  bool m_checkMMSimHits;
  bool m_useTof;
  bool m_useAttenuation;
  bool m_useProp;

  //pile-up
  TimedHitCollection<GenericMuonSimHit>* m_thpcMM; // the hits

  ///////////////////////////////////////////////////////////////////
  // Access to the event methods:
  ///////////////////////////////////////////////////////////////////
  // Get next event and extract collection of hit collections:
  StatusCode getNextEvent();
  StatusCode doDigitization();

  void  fillMaps(const GenericMuonSimHit * mmHit, const Identifier digitId, const double driftR);
  int   digitizeTime(double time) const;
  bool outsideWindow(double time) const; // default +-50...

  MmElectronicsToolInput CombinedStripResponseAllhits(const std::vector< MmElectronicsToolInput > & v_StripdigitOutput);
  //TIMING SCHEME
  bool   m_useTimeWindow;
  double m_inv_c_light;
  double m_DiffMagSecondMuonHit;

  //TDC ELECTRONICS
  double m_ns2TDC;
  double m_resTDC;

  // RandomGenerators
  //  double m_FlatDist;
  //  double m_GaussDist;
  //  double m_PoissonDist;
  //  double m_GammaDist;
  double m_Polya;
  double m_timeWindowLowerOffset;
  double m_timeWindowUpperOffset;
  double m_bunchTime;
  double * m_sprob;
  double m_amplification;

  // StripsResponse stuff...
  StripsResponse *m_StripsResponse;
  float m_qThreshold, m_diffusSigma, m_LogitundinalDiffusSigma, m_driftGap, m_driftVelocity, m_crossTalk1, m_crossTalk2;
  float m_qThresholdForTrigger;
  std::string m_gasFileName;

  // ElectronicsResponse stuff...
  ElectronicsResponse *m_ElectronicsResponse;
  float m_alpha;// power of responce function
  // float m_RC ;// time constant of responce function
  float m_peakTime; // VMM setting
  float m_electronicsThreshold; // threshold "Voltage" for histoBNL
  float m_stripdeadtime; // dead-time for strip
  float m_ARTdeadtime; // dead-time for ART
  TFile *m_file;
  TTree *m_ntuple;
  TH1I *m_AngleDistr, *m_AbsAngleDistr, *m_ClusterLength2D, *m_ClusterLength, *m_gasGap,  *m_gasGapDir ;

  int m_n_Station_side, m_n_Station_eta, m_n_Station_phi, m_n_Station_multilayer, m_n_Station_layer, m_n_hitStripID, m_n_StrRespTrg_ID, m_n_strip_multiplicity, m_n_strip_multiplicity_2;
  int exitcode, m_n_hitPDGId;
  double m_n_hitOnSurface_x, m_n_hitOnSurface_y, m_n_hitDistToChannel, m_n_hitIncomingAngle,m_n_StrRespTrg_Time, m_n_hitIncomingAngleRads, m_n_hitKineticEnergy, m_n_hitDepositEnergy;
  float  tofCorrection, bunchTime, globalHitTime, eventTime;
  std::vector<int> m_n_StrRespID;
  std::vector<float> m_n_StrRespCharge, m_n_StrRespTime;

  // vector <TH1F*> m_histoBNL;

  // private methods need to compute the induce charge on the strip
  double qStrip        (const int & nElectrons, const double & gammaDist) const;
  double qStripR       (const double x) const;
  double qStripR       (const double x, const std::string stationType) const;
  double qStripPhi     (const double x, const std::string stationType) const;
  double fparamPhi     (const double x, const int N, const double * p) const;
  //	  double getDriftTime(const MMReadoutElement* descriptor, const Amg::Vector3D& pos) const;


protected:

  PileUpMergeSvc *m_mergeSvc; // Pile up service
  std::string m_inputObjectName; // name of the input objects
  std::string m_outputObjectName; // name of the output digits
  std::string m_outputSDOName; // name of the output SDOs

  ServiceHandle <IAtRndmGenSvc> m_rndmSvc;      // Random number service
  CLHEP::HepRandomEngine *m_rndmEngine;    // Random number engine used - not init in SiDigitization
  std::string m_rndmEngineName;// name of random engine
  int m_maskMultiplet;

};
/*******************************************************************************/
inline bool MmDigitizationTool::outsideWindow(double driftTime) const {
  double time = driftTime; //m_bunchTime is included already....updated one..
  //  double time = driftTime+m_bunchTime;
  return time < m_timeWindowLowerOffset || time > m_timeWindowUpperOffset;
}
/*******************************************************************************/
inline double MmDigitizationTool::qStrip (const int & nElectrons, const double& gammaDist) const {

  // find the charge on the wire

  //  double amplification = 0.58e5;
  double amplification = m_amplification;
  double stripCharge = 0.0;

  for (int i=0; i<nElectrons; i++) {
    double RNDpol = 0.0;
    if (m_Polya > -1.0) {
      RNDpol = gammaDist/(1.0+m_Polya);
    }
    stripCharge += amplification*RNDpol;
  }
  return stripCharge;
}
/*******************************************************************************/
inline void MmDigitizationTool::set (const double bunchTime) {m_bunchTime = bunchTime;}

#endif // MmDigitizationTool
