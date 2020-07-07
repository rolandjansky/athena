/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_DIGITIZATIONTOOL_H
#define MM_DIGITIZATIONTOOL_H
/** @class MM_DigitizationTool

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
	containers are created and recorded on StoreGate; the MMSimHit collection are merged using the
	TimedHitCollection sorted container (done in handleMicroMegasSimhit(TimedHitPtr<MMSimHit>& hit)) method);
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
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include "HitManagement/TimedHitCollection.h"
#include "MuonSimEvent/MMSimHitCollection.h"
#include "MuonSimEvent/MMSimHit.h"
#include "PileUpTools/PileUpToolBase.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonDigitContainer/MmDigitContainer.h"

#include "MM_Digitization/MM_SortedHitVector.h"

#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include "MuonDigToolInterfaces/IMuonDigitizationTool.h"
#include "MM_Digitization/MM_StripsResponseSimulation.h"
#include "MM_Digitization/MM_ElectronicsResponseSimulation.h"
#include "MM_Digitization/MM_StripVmmMappingTool.h"

#include "xAODEventInfo/EventInfo.h"   // SubEventIterator
#include "xAODEventInfo/EventAuxInfo.h"// SubEventIterator

#include "NSWCalibTools/INSWCalibSmearingTool.h"
#include "NSWCalibTools/INSWCalibTool.h"

#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MagFieldElements/AtlasFieldCache.h"

#include <string>
#include <sstream>
#include <vector>
#include <map>

/*******************************************************************************/

namespace MuonGM{
  class MuonDetectorManager;
}
namespace CLHEP{
  class HepRandomEngine;
}
class PileUpMergeSvc;
class MicromegasHitIdHelper;
class IAtRndmGenSvc;
class IMM_DigitizationTool;
class StoreGateSvc;
class TTree;
class TFile;

/*******************************************************************************/

class MM_DigitizationTool : virtual public IMuonDigitizationTool, public PileUpToolBase {

	public:
		MM_DigitizationTool(const std::string& type, const std::string& name, const IInterface* parent);

		/** Initialize */
		virtual StatusCode initialize() override final;

		/** When being run from PileUpToolsAlgs, this method is called at the start of the subevts loop. Not able to access SubEvents */
                StatusCode prepareEvent(const EventContext& ctx, const unsigned int /*nInputEvents*/) override final;

		/** When being run from PileUpToolsAlgs, this method is called for each active bunch-crossing to process current SubEvents bunchXing is in ns */
		StatusCode  processBunchXing(int bunchXing,
					     SubEventIterator bSubEvents,
					     SubEventIterator eSubEvents) override final;

		/** When being run from PileUpToolsAlgs, this method is called at the end of the subevts loop. Not (necessarily) able to access SubEvents */
		StatusCode mergeEvent(const EventContext& ctx) override final;

		/** When being run from MM_Digitizer, this method is called during the event loop */

		/** alternative interface which uses the PileUpMergeSvc to obtain
		all the required SubEvents. */
		virtual StatusCode processAllSubEvents(const EventContext& ctx) override;

		/** Just calls processAllSubEvents - leaving for back-compatibility
		(IMuonDigitizationTool) */
		StatusCode digitize(const EventContext& ctx) override;

		/** Finalize */
		StatusCode finalize() override final;

		/** accessors */
		ServiceHandle<IAtRndmGenSvc> getRndmSvc() const { return m_rndmSvc; }    // Random number service
		CLHEP::HepRandomEngine  *getRndmEngine() const { return m_rndmEngine; } // Random number engine used



	private:

		/** Record MmDigitContainer and MuonSimDataCollection */
		StatusCode getNextEvent();
		StatusCode doDigitization(const EventContext& ctx);

		bool  checkMMSimHit(const MMSimHit& /* hit */ ) const;
		MM_ElectronicsToolInput combinedStripResponseAllHits(const std::vector< MM_ElectronicsToolInput > & v_stripDigitOutput);

		// Services
		ServiceHandle<StoreGateSvc> m_storeGateService;
		PileUpMergeSvc *m_mergeSvc; // Pile up service
		ServiceHandle <IAtRndmGenSvc> m_rndmSvc;      // Random number service
		CLHEP::HepRandomEngine *m_rndmEngine;    // Random nu
		std::string m_rndmEngineName;// name of random enginember engine used - not init in SiDigitization

		SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj"};

		// Tools
		ToolHandle <IMM_DigitizationTool> m_digitTool;
		TFile *m_file;
		TTree *m_ntuple;

		ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
		MicromegasHitIdHelper*  m_muonHelper;
		const MuonGM::MuonDetectorManager* m_MuonGeoMgr;
		std::list<MMSimHitCollection*> m_MMHitCollList;

		// Settings
		double m_energyThreshold;
		int m_maskMultiplet;
		bool m_writeOutputFile;
		TimedHitCollection<MMSimHit>* m_timedHitCollection_MM; // the pileup hits

        SG::WriteHandleKey<MmDigitContainer> m_outputDigitCollectionKey{this,"OutputObjectName","MM_DIGITS","WriteHandleKey for Output MmigitContainer"}; // name of the output digits
        SG::WriteHandleKey<MuonSimDataCollection> m_outputSDO_CollectionKey{this,"OutputSDOName","MM_SDO","WriteHandleKey for Output MuonSimDataCollection"}; // name of the output SDOs

		std::string m_inputObjectName; // name of the input objects
		bool m_needsMcEventCollHelper;

		bool m_checkMMSimHits;


		//TIMING SCHEME
		bool   m_useTimeWindow;

		double m_timeWindowLowerOffset;
		double m_timeWindowUpperOffset;
		double m_DiffMagSecondMuonHit;

		// StripsResponse stuff...
		MM_StripsResponseSimulation *m_StripsResponseSimulation;

		float m_qThreshold;
		float m_driftGapWidth;
		float m_driftVelocity;
		float m_crossTalk1;
		float m_crossTalk2;

		float m_avalancheGain;

		// ElectronicsResponse stuff...
		MM_ElectronicsResponseSimulation *m_ElectronicsResponseSimulation;
		float m_peakTime; // VMM setting
		float m_electronicsThreshold; // threshold "Voltage" for histoBNL
		float m_stripdeadtime; // dead-time for strip
		float m_ARTdeadtime; // dead-time for ART

		bool  m_vmmNeighborLogic; // switch for the usage of the vmm neighbor logic

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

		float  m_tofCorrection;
		float m_bunchTime;
		float m_globalHitTime;
		float m_eventTime;
		std::vector<int> m_n_StrRespID;
		std::vector<float> m_n_StrRespCharge;
		std::vector<float> m_n_StrRespTime;

		/// tool handle for the smearing 
		bool m_doSmearing;
		ToolHandle<Muon::INSWCalibSmearingTool> m_smearingTool;

		ToolHandle<Muon::INSWCalibTool> m_calibrationTool;

};

#endif // MM_DigitizationTool
