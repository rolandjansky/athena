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

#include "PileUpTools/PileUpToolBase.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "HitManagement/TimedHitCollection.h"
#include "MuonSimEvent/MMSimHitCollection.h"
#include "MuonSimEvent/MMSimHit.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonDigitContainer/MmDigitContainer.h"
#include "MM_Digitization/MM_SortedHitVector.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "MM_Digitization/MM_StripsResponseSimulation.h"
#include "MM_Digitization/MM_ElectronicsResponseSimulation.h"
#include "MM_Digitization/MM_StripVmmMappingTool.h"
#include "xAODEventInfo/EventInfo.h"   // SubEventIterator
#include "xAODEventInfo/EventAuxInfo.h"// SubEventIterator
#include "NSWCalibTools/INSWCalibSmearingTool.h"
#include "NSWCalibTools/INSWCalibTool.h"
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MagFieldElements/AtlasFieldCache.h"
#include "MM_Digitization/IMM_DigitizationTool.h"

#include <string>
#include <sstream>
#include <vector>
#include <map>

/*******************************************************************************/

namespace MuonGM{
  class MuonDetectorManager;
}

class PileUpMergeSvc;
class MicromegasHitIdHelper;
class StoreGateSvc;
class TTree;
class TFile;

/*******************************************************************************/

class MM_DigitizationTool : public PileUpToolBase {

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
		StatusCode digitize(const EventContext& ctx);

		/** Finalize */
		StatusCode finalize() override final;

	private:
		/** Record MmDigitContainer and MuonSimDataCollection */
		StatusCode getNextEvent();
		StatusCode doDigitization(const EventContext& ctx);

		bool  checkMMSimHit(const MMSimHit& /* hit */ ) const;
		MM_ElectronicsToolInput combinedStripResponseAllHits(const std::vector< MM_ElectronicsToolInput > & v_stripDigitOutput);

		ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
		ToolHandle<Muon::INSWCalibSmearingTool> m_smearingTool{this,"SmearingTool","Muon::NSWCalibSmearingTool/MMCalibSmearingTool"};
		ToolHandle<Muon::INSWCalibTool> m_calibrationTool{this,"CalibrationTool","Muon::NSWCalibTool/NSWCalibTool"};
		ToolHandle<IMM_DigitizationTool> m_digitTool{this,"DigitizationTool","MM_Response_DigitTool","Tool which handles the digitization process"};

		SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj"};

		Gaudi::Property<std::string> m_inputObjectName{this,"InputObjectName","MicromegasSensitiveDetector","name of the input objects"};
		Gaudi::Property<std::string> m_vmmReadoutMode{this,"vmmReadoutMode","peak","For readout (DAQ) path. Can be peak or threshold"};
		Gaudi::Property<std::string> m_vmmARTMode{this,"vmmARTMode","threshold","For ART (trigger) path. Can be peak or threshold"};

		Gaudi::Property<double> m_energyThreshold{this,"EnergyThreshold",50,"Minimal energy to produce a PRD"};
		Gaudi::Property<double> m_timeWindowLowerOffset{this,"WindowLowerOffset",-300,"processBunchXing between -250 and 150 ns (look at config file)"};
		Gaudi::Property<double> m_timeWindowUpperOffset{this,"WindowUpperOffset",300};
		Gaudi::Property<double> m_DiffMagSecondMuonHit{this,"DiffMagSecondMuonHit",0.1};

		Gaudi::Property<int> m_maskMultiplet{this,"MaskMultiplet",0,"0: all, 1: first, 2: second, 3: both"};

		Gaudi::Property<bool> m_writeOutputFile{this,"SaveInternalHistos",true};
		Gaudi::Property<bool> m_needsMcEventCollHelper{this,"UseMcEventCollectionHelper",false};
		Gaudi::Property<bool> m_checkMMSimHits{this,"CheckSimHits",true,"Control on the hit validity"};
		Gaudi::Property<bool> m_useTimeWindow{this,"UseTimeWindow",true};
		Gaudi::Property<bool> m_vmmNeighborLogic{this,"VMMNeighborLogic",true};
		Gaudi::Property<bool> m_doSmearing{this,"doSmearing",false,"set the usage or not of the smearing tool for realistic detector performance"};

		// Constants vars for the MM_StripsResponseSimulation class
		// qThreshold=2e, we accept a good strip if the charge is >=2e

		//Three gas mixture mode,	Ar/CO2=93/7, Ar/CO2=80/20, Ar/CO2/Iso=93/5/2
		//each mode have different transverseDiffusionSigma/longitudinalDiffusionSigma/driftVelocity/avalancheGain/interactionDensityMean/interactionDensitySigma/lorentzAngle
		Gaudi::Property<float> m_qThreshold{this,"qThreshold",0.001,"Charge Threshold"};
		Gaudi::Property<float> m_driftGapWidth{this,"DriftGapWidth",5.168,"Drift Gap Width of 5.04 mm + 0.128 mm (the amplification gap)"};
		Gaudi::Property<float> m_crossTalk1{this,"crossTalk1",0.1,"Strip Cross Talk with Nearest Neighbor"};
		Gaudi::Property<float> m_crossTalk2{this,"crossTalk2",0.03,"Strip Cross Talk with 2nd Nearest Neighbor"};

		Gaudi::Property<float> m_avalancheGain{this,"AvalancheGain",8.0e3,"avalanche Gain for rach gas mixture"};

		// Constants vars for the MM_ElectronicsResponseSimulation
		Gaudi::Property<float> m_peakTime{this,"peakTime",100,"The VMM peak time setting"};
		Gaudi::Property<float> m_electronicsThreshold{this,"electronicsThreshold",15000,"threshold Voltage for histoBNL, 2*(Intrinsic noise ~3k e)"};
		Gaudi::Property<float> m_stripdeadtime{this,"StripDeadTime",200,"dead-time for strip, default value 200 ns = 8 BCs"};
		Gaudi::Property<float> m_ARTdeadtime{this,"ARTDeadTime",200,"dead-time for ART, default value 200 ns = 8 BCs"};

        SG::WriteHandleKey<MmDigitContainer> m_outputDigitCollectionKey{this,"OutputObjectName","MM_DIGITS","WriteHandleKey for Output MmigitContainer"}; // name of the output digits
        SG::WriteHandleKey<MuonSimDataCollection> m_outputSDO_CollectionKey{this,"OutputSDOName","MM_SDO","WriteHandleKey for Output MuonSimDataCollection"}; // name of the output SDOs

		PileUpMergeSvc *m_mergeSvc; // Pile up service

		TFile *m_file;
		TTree *m_ntuple;

		MicromegasHitIdHelper* m_muonHelper;
		const MuonGM::MuonDetectorManager* m_MuonGeoMgr;
		std::list<MMSimHitCollection*> m_MMHitCollList;
		TimedHitCollection<MMSimHit>* m_timedHitCollection_MM; // the pileup hits
		MM_StripsResponseSimulation* m_StripsResponseSimulation;
		MM_ElectronicsResponseSimulation* m_ElectronicsResponseSimulation;

		float m_driftVelocity;
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
		double m_n_hitIncomingAngle;
		double m_n_StrRespTrg_Time;
		double m_n_hitIncomingAngleRads;
		double m_n_hitKineticEnergy;
		double m_n_hitDepositEnergy;

		int m_exitcode;

		float m_tofCorrection;
		float m_bunchTime;
		float m_globalHitTime;
		float m_eventTime;
		std::vector<int> m_n_StrRespID;
		std::vector<float> m_n_StrRespCharge;
		std::vector<float> m_n_StrRespTime;
};

#endif // MM_DigitizationTool
