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
#include "MM_Digitization/StripsResponseSimulation.h"
#include "MM_Digitization/ElectronicsResponseSimulation.h"
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

// class StripsResponse;
// class ElectronicsResponse;

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
		StatusCode  processBunchXing(int bunchXing,
									SubEventIterator bSubEvents,
									SubEventIterator eSubEvents) override final;

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



	private:

		/** Record MmDigitContainer and MuonSimDataCollection */
		StatusCode recordDigitAndSdoContainers();
		StatusCode getNextEvent();
		StatusCode doDigitization();

		bool  checkMMSimHit(const GenericMuonSimHit& /* hit */ ) const;
		MmElectronicsToolInput combinedStripResponseAllHits(const std::vector< MmElectronicsToolInput > & v_stripDigitOutput);

		// Services
		ServiceHandle<StoreGateSvc> m_storeGateService;
		ActiveStoreSvc*             m_activeStore;
		ServiceHandle<MagField::IMagFieldSvc>            m_magFieldSvc;
		PileUpMergeSvc *m_mergeSvc; // Pile up service
		ServiceHandle <IAtRndmGenSvc> m_rndmSvc;      // Random number service
		CLHEP::HepRandomEngine *m_rndmEngine;    // Random nu
		std::string m_rndmEngineName;// name of random enginember engine used - not init in SiDigitization

		// Containers
		MmDigitContainer*      m_digitContainer;
		MuonSimDataCollection* m_sdoContainer;

		// Tools
		ToolHandle <IMM_DigitizationTool> m_digitTool;
		TFile *m_file;
		TTree *m_ntuple;

		const MmIdHelper*       m_idHelper;
		MicromegasHitIdHelper*  m_muonHelper;
		const MuonGM::MuonDetectorManager* m_MuonGeoMgr;
		std::list<GenericMuonSimHitCollection*> m_MMHitCollList;

		// Settings
		double m_energyThreshold;
		int m_maskMultiplet;
		bool m_writeOutputFile;
		TimedHitCollection<GenericMuonSimHit>* m_timedHitCollection_MM; // the pileup hits

		std::string m_inputObjectName; // name of the input objects
		std::string m_outputObjectName; // name of the output digits
		std::string m_outputSDOName; // name of the output SDOs

		bool m_checkMMSimHits;


		//TIMING SCHEME
		bool   m_useTimeWindow;

		double m_timeWindowLowerOffset;
		double m_timeWindowUpperOffset;
		double m_DiffMagSecondMuonHit;

		//TDC ELECTRONICS
		// double m_ns2TDC;
		// double m_resTDC;

		// StripsResponse stuff...
		StripsResponseSimulation *m_StripsResponseSimulation;
		float m_qThreshold;
		float m_transverseDiffusionSigma;
		float m_longitudinalDiffusionSigma;
		float m_driftGapWidth;
		float m_driftVelocity;
		float m_crossTalk1;
		float m_crossTalk2;

		// ElectronicsResponse stuff...
		ElectronicsResponseSimulation *m_ElectronicsResponseSimulation;
		float m_alpha;// power of responce function
		float m_peakTime; // VMM setting
		float m_electronicsThreshold; // threshold "Voltage" for histoBNL
		float m_stripdeadtime; // dead-time for strip
		float m_ARTdeadtime; // dead-time for ART

		std::string m_vmmReadoutMode;
		std::string m_vmmARTMode;

		int m_n_Station_side;
		int m_n_Station_eta;
		int m_n_Station_phi;
		int m_n_Station_multilayer;
		int m_n_Station_layer;
		int m_n_hitStripID;
		int m_n_StrRespTrg_ID;
		int m_n_strip_multiplicity;
		int m_n_strip_multiplicity_2;
		int m_n_hitPDGId;

		double m_n_hitOnSurface_x;
		double m_n_hitOnSurface_y;
		double m_n_hitDistToChannel;
		double m_n_hitIncomingAngle,m_n_StrRespTrg_Time;
		double m_n_hitIncomingAngleRads;
		double m_n_hitKineticEnergy;
		double m_n_hitDepositEnergy;

		int m_exitcode;

		float  tofCorrection;
		float bunchTime;
		float globalHitTime;
		float eventTime;
		std::vector<int> m_n_StrRespID;
		std::vector<float> m_n_StrRespCharge;
		std::vector<float> m_n_StrRespTime;

};

#endif // MmDigitizationTool
