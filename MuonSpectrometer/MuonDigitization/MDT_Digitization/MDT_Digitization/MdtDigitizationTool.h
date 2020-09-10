/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @class MdtDigitizationTool

    In the initialize() method, the PileUpMerge and StoreGate services
    are initialized, and a pointer to an instance of the class
    MuonDetectorManager is retrieved from the detector store and used
    to obtain a MdtIdHelper.The MdtDigitContainer is initialized and
    the simulation identifier helper retrieved, together with the
    pointer to the digitization tool. Random numbers are obtained in
    the code from a dedicated stream via AtRndmSvc, which is also
    initialized in the initialize() method.

    In the execute() method, the digits and the SDOs (Simulation Data Object,
    container for simulation data to be preserved after the digitization
    procedue, and persistified together with the RDOs) containers are created
    and recorded on StoreGate; the MDTSimHit collection are merged using the
    TimedHitCollection sorted container (done in handleMDTSimhit(TimedHitPtr
    \<MDTSimHit\>& hit)) method); into a loop over the TimedHitCollection for
    the given DetectorElement, the handleMDTSimhit() method converts the SimID
    into the Offline ID to be associated to the Digit and pass to the
    digitization tool the drift radius and the distance to the chamber RO side
    (for the propagation delay computation). The digitization tool returns a
    drift time and an ADC count which are used together with the Offline ID, to
    create the digit object (in createDigits() method).

    The finalize() method returns a SUCCESS StatusCode if the digitization
    procedure ends succesfully.

*/
#ifndef MUONDIGITIZATION_MDT_DIGITIZATIONTOOL_H
#define MUONDIGITIZATION_MDT_DIGITIZATIONTOOL_H

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "HitManagement/TimedHitCollection.h"
#include "MuonSimEvent/MDTSimHitCollection.h"
#include "MuonSimEvent/MDTSimHit.h"
#include "PileUpTools/PileUpToolBase.h"
#include "Identifier/Identifier.h"

#include "xAODEventInfo/EventInfo.h"             // NEW EDM
#include "xAODEventInfo/EventAuxInfo.h"          // NEW EDM



#include "MDT_Response/MDT_Response.h"
#include "MDT_Digitization/MDT_SortedHitVector.h"

#include <string>
#include <sstream>
#include <vector>
#include "GaudiKernel/PhysicalConstants.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Geometry/Point3D.h"
#include "AthenaKernel/IAthRNGSvc.h"
#include "CLHEP/Random/RandGaussZiggurat.h"

#include "MuonCondData/MdtCondDbData.h"
#include "MDT_Digitization/IMDT_DigitizationTool.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include "MdtCalibSvc/MdtCalibrationDbTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

//Outputs
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimData/MuonSimData.h"
#include "MuonDigitContainer/MdtDigitContainer.h"

namespace MuonGM{
  class MuonDetectorManager;
  class MdtReadoutElement;
}

class MdtHitIdHelper;
class MdtCondDbData;

// Digitization class for MDT hits
/*
  +DSL--------------------------------------------------------------------------
  The drift time can be modified according to the amount of wire sag,
  the track angle, the distance from the end of the tube, the tube length, the
  chamber phi angle. This drift time delta is turned on/off via  m_useWireSagRT
  It operates only on small wheel and big wheel end cap chambers (EIL/EIS; EML/EMS)
  The EOL/EOS chambers are presumably built with tubes bent to conform to the wire sag-
  so this code does not introduce wire-sag induced RT distortions for these chambers



  This drift time delta is turned on/off via  m_useWireSagGeom for the geomtrical wiresag effect
  It operates on all end cap chambers (EIL/EIS; EML/EMS EOL/EOS )


  -DSL----------------------------------------------------------------------------

*/
class MuonSimDataCollection;


class MdtDigitizationTool : public PileUpToolBase {

 public:
  MdtDigitizationTool(const std::string& type, const std::string& name, const IInterface* pIID);
  virtual ~MdtDigitizationTool() = default;

  /** Initialize */
  virtual StatusCode initialize() override final;

  /** When being run from PileUpToolsAlgs, this method is called at the start of the subevts loop. Not able to access SubEvents */
  StatusCode prepareEvent(const EventContext& ctx, const unsigned int /*nInputEvents*/) override final;

  /** When being run from PileUpToolsAlgs, this method is called for each active bunch-crossing to process current SubEvents bunchXing is in ns */
   virtual StatusCode processBunchXing(
                                int bunchXing,
                                SubEventIterator bSubEvents,
                                SubEventIterator eSubEvents
                                ) override final;
  /** When being run from PileUpToolsAlgs, this method is called at the end of the subevts loop. Not (necessarily) able to access SubEvents */
  StatusCode mergeEvent(const EventContext& ctx) override final;

  /** alternative interface which uses the PileUpMergeSvc to obtain
  all the required SubEvents. */
  virtual StatusCode processAllSubEvents(const EventContext& ctx) override final;

  struct GeoCorOut {
  GeoCorOut( double sSag, double sTrack, Amg::Vector3D lp, double lSag ) : sagSign(sSag), trackingSign(sTrack), localPosition(lp), localSag(lSag) {}
    double sagSign;            // sign indicating wether the particle passed above or below the wire
    double trackingSign;       // sign in tracking convention indicating whether the particle passed left ot right of the wire
    Amg::Vector3D localPosition;  // point of closest approach of the particle to the wire
    double localSag;
  };

 private:
  CLHEP::HepRandomEngine*   getRandomEngine(const std::string& streamName, const EventContext& ctx) const;
  int                       digitizeTime(double time, bool isHPTDC, CLHEP::HepRandomEngine *rndmEngine) const;
  double                    minimumTof(Identifier DigitId) const;

  bool                      insideMatchingWindow(double time) const;
  bool                      insideMaskWindow(double time) const;
  bool                      checkMDTSimHit(const MDTSimHit& hit) const;

  bool                      handleMDTSimhit(const TimedHitPtr<MDTSimHit>& phit, CLHEP::HepRandomEngine *twinRndmEngine, CLHEP::HepRandomEngine *toolRndmEngine);
  bool                      createDigits(MdtDigitContainer* digitContainer, MuonSimDataCollection* sdoContainer, CLHEP::HepRandomEngine *rndmEngine);

  // calculate local hit position in local sagged wire frame, also returns whether the hit passed above or below the wire
  GeoCorOut correctGeometricalWireSag( const MDTSimHit& hit, const Identifier& id, const MuonGM::MdtReadoutElement* element ) const ;

  MDT_SortedHitVector        m_hits;

  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  MdtHitIdHelper*            m_muonHelper{};

  MDTSimHit applyDeformations(const MDTSimHit&,const MuonGM::MdtReadoutElement*,const Identifier&);

  const MuonGM::MuonDetectorManager* m_MuonGeoMgr{};

  ToolHandle <IMDT_DigitizationTool> m_digiTool{this, "DigitizationTool", "MDT_Response_DigiTool", "Tool which handle the digitization process"};
  std::list<MDTSimHitCollection*> m_MDTHitCollList;

  //TDC ELECTRONICS
  Gaudi::Property<double> m_offsetTDC{this, "OffsetTDC", 800., "TDC offset"};
  Gaudi::Property<double> m_signalSpeed{this, "SignalSpeed",Gaudi::Units::c_light, "Light speed" };
  Gaudi::Property<double> m_ns2TDCAMT{this, "ns2TDCAMT", 0.78125, "Conversion factor TDC/ns for AMT chips"};
  Gaudi::Property<double> m_ns2TDCHPTDC{this, "ns2TDCHPTDC", 0.1953125, "Conversion factor TDC/ns for HPTDC chips"};
  Gaudi::Property<double> m_resTDC{this, "ResolutionTDC", 0.5, "TDC resolution"};

  //CONFIGURATION
  Gaudi::Property<bool>   m_checkMDTSimHits {this, "CheckSimHits", true, "Control on the hit validity"};
  Gaudi::Property<bool>   m_useTof{this, "UseTof", true, "Option for the tof calculation"};
  Gaudi::Property<bool>   m_useAttenuation{this, "UseAttenuation", false, ""};
  Gaudi::Property<bool>   m_useProp{this, "UseProp", true, ""};

  //WIRE SAG
  Gaudi::Property<bool>   m_useWireSagGeom{this, "UseWireSagGeom", false, "Option for the wire sagitta correction"};
  Gaudi::Property<bool>   m_useWireSagRT{this, "UseWireSagRT", false, "Option for the wire sagitta correction"};

  //TWIN TUBE
  Gaudi::Property<bool>   m_useTwin{this, "UseTwin", false, ""};
  Gaudi::Property<bool>   m_useAllBOLTwin{this, "UseAllBOLTwin", false, ""};
  Gaudi::Property<double> m_resTwin{this, "ResolutionTwinTube", 1.05, "Twin Tube resolution"};

  //TIMING SCHEME
  Gaudi::Property<bool>   m_useTimeWindow{this, "UseTimeWindow", true, ""};
  Gaudi::Property<double> m_bunchCountOffset{this, "BunchCountOffset", -200., "Bunch crossing offset"};
  Gaudi::Property<double> m_matchingWindow{this, "MatchingWindow", 1000., "Matching window"};
  Gaudi::Property<double> m_maskWindow{this, "MaskWindow", 250., "Masked window"};
  Gaudi::Property<double> m_deadTime{this, "DeadTime", 700., "MDT drift tube dead time"};
  Gaudi::Property<bool>   m_DiscardEarlyHits{this, "DiscardEarlyHits", true, ""};

  //COSMICS
  Gaudi::Property<bool> m_useOffSet1{this, "UseOffSet1", true, ""};
  Gaudi::Property<bool> m_useOffSet2{this, "UseOffSet2", true, ""};

  //Conditions Database
  Gaudi::Property<bool> m_UseDeadChamberSvc{this, "UseDeadChamberSvc",   false, ""};
  Gaudi::Property<bool> m_t0_from_DB{this, "GetT0FromBD", false, ""};

  //B-lines
  Gaudi::Property<bool> m_useDeformations{this, "UseDeformations", false, ""};

  //MULTI-CHARGE PARTICLES DIGITIZATION
  Gaudi::Property<bool> m_DoQballCharge{this, "DoQballCharge", false, "dEdx for Qballs with account of electric charge"};

  //STATIONS TO MASK
  Gaudi::Property< std::vector<std::string> > m_maskedStations{this, "MaskedStations", {}, "Stations to be masked at digi level"};
  struct maskedStation {
  maskedStation(std::string n, std::string e, std::string p):
    maskedName(n),maskedEta(e),maskedPhi(p),imaskedEta(0),imaskedPhi(0)
    {
      if (e!="*") {std::istringstream v1(maskedEta);v1>>imaskedEta;}
      if (p!="*") {std::istringstream v1(maskedPhi);v1>>imaskedPhi;}
    }
    std::string maskedName;
    std::string maskedEta;
    std::string maskedPhi;
    int imaskedEta;
    int imaskedPhi;
  };

  std::vector<maskedStation> m_vMaskedStations;

  //list of Identifiers returned by the Conditions Service to mask stations
  std::vector<Identifier> m_IdentifiersToMask;

  //pile-up
  TimedHitCollection<MDTSimHit>* m_thpcMDT{}; // the hits

  //pileup truth veto
  Gaudi::Property<bool> m_includePileUpTruth{this, "IncludePileUpTruth", true, "Include pile-up truth info"};

  ///////////////////////////////////////////////////////////////////
  // Access to the event methods:
  ///////////////////////////////////////////////////////////////////
  // Get next event and extract collection of hit collections:
  StatusCode                getNextEvent(const EventContext& ctx);
  StatusCode doDigitization(const EventContext& ctx, MdtDigitContainer* digitContainer, MuonSimDataCollection* sdoContainer);
  MdtDigitCollection*       getDigitCollection(Identifier elementId, MdtDigitContainer* digitContainer);
  void                fillMaps(const MDTSimHit * mdtHit, const Identifier digitId,
                               const double driftR);

protected:
  ServiceHandle<PileUpMergeSvc> m_mergeSvc{this, "PileUpMergeSvc", "PileUpMergeSvc", ""}; // Pile up service
  BooleanProperty m_onlyUseContainerName{this, "OnlyUseContainerName", true, "Don't use the ReadHandleKey directly. Just extract the container name from it."};
  SG::ReadHandleKey<MDTSimHitCollection> m_hitsContainerKey{this, "InputObjectName", "MDT_Hits", ""}; // name of the input objects
  std::string m_inputObjectName{""};
  SG::WriteHandleKey<MdtDigitContainer> m_outputObjectKey{this,"OutputObjectName","MDT_DIGITS","WriteHandleKey for Output MdtDigitContainer"};
  SG::WriteHandleKey<MuonSimDataCollection> m_outputSDOKey{this,"OutputSDOName","MDT_SDO","WriteHandleKey for Output MuonSimDataCollection"};

  ServiceHandle <IAthRNGSvc> m_rndmSvc{this, "RndmSvc", "AthRNGSvc", ""};      // Random number service

  ToolHandle<MdtCalibrationDbTool> m_calibrationDbTool{this, "CalibrationDbTool", "MdtCalibrationDbTool", ""};
  SG::ReadCondHandleKey<MdtCondDbData> m_readKey{this, "ReadKey", "MdtCondDbData", "Key of MdtCondDbData"};
};

#endif
