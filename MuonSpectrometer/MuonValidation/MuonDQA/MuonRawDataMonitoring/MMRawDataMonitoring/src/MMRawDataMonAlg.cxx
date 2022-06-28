/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Package : MMRawDataMonAlg
// Authors:   M. Biglietti, E. Rossi (Roma Tre)
// 
//
// DESCRIPTION:
// Subject: MM-->Offline Muon Data Quality
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonStation.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonChambersRange.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

#include "MMRawDataMonAlg.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonRIO_OnTrack/MMClusterOnTrack.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "MuonPrepRawData/MMPrepData.h"
#include "MuonSegment/MuonSegment.h"


namespace {

	static constexpr double const toDeg = 180/M_PI;

	//1e=1.6X10-4 fC
	static constexpr double conversion_charge=1.6E-04;

	static const std::array<std::string,2> MM_Side = {"CSide", "ASide"};
	static const std::array<std::string,2> EtaSector = {"1", "2"};

	struct MMOverviewHistogramStruct {
		std::vector<int> statEta_strip;
	  	std::vector<float> charge_all;
	  	std::vector<int> strp_times;
	  	std::vector<float> cl_times;
	  	std::vector<int> strip_number;
	  	std::vector<int> numberofstrips_percluster;
	  	std::vector<float> mu_TPC_angle;
	  	std::vector<float> mu_TPC_chi2;
	  	std::vector<float> R_mon;
	  	std::vector<float> z_mon;
	  	std::vector<float> x_mon;
	  	std::vector<float> y_mon;
	  	std::vector<int> stationPhi_ASide_eta1_ontrack;
	  	std::vector<int> stationPhi_ASide_eta2_ontrack;
	  	std::vector<int> stationPhi_CSide_eta1_ontrack;
	  	std::vector<int> stationPhi_CSide_eta2_ontrack;
	  	std::vector<int> sector_ASide_eta2_ontrack;
	  	std::vector<int> sector_CSide_eta1_ontrack;
	  	std::vector<int> sector_ASide_eta1_ontrack;
	  	std::vector<int> sector_CSide_eta2_ontrack;
	  	std::vector<int> stationPhi_ASide_eta1;
	  	std::vector<int> stationPhi_ASide_eta2;
	  	std::vector<int> stationPhi_CSide_eta1;
	  	std::vector<int> stationPhi_CSide_eta2;
	  	std::vector<int> sector_CSide_eta2;
	  	std::vector<int> sector_ASide_eta2;
	  	std::vector<int> sector_CSide_eta1;
	  	std::vector<int> sector_ASide_eta1;
	  	std::vector<int> stationPhi_ASide_eta1_onseg;
	  	std::vector<int> stationPhi_ASide_eta2_onseg;
	  	std::vector<int> stationPhi_CSide_eta1_onseg;
	  	std::vector<int> stationPhi_CSide_eta2_onseg;
	  	std::vector<int> sector_ASide_eta2_onseg;
	  	std::vector<int> sector_CSide_eta1_onseg;
	  	std::vector<int> sector_ASide_eta1_onseg;
	  	std::vector<int> sector_CSide_eta2_onseg;
	};

	struct MMByPhiStruct {
	  	std::vector<int> sector_lb;
	  	std::vector<int> sector_lb_ontrack;
	  	std::vector<int> sector_lb_onseg;
	};

	struct MMSummaryHistogramStruct {
	  std::vector<int> cl_size;
	  std::vector<int> pcb;
	  std::vector<int> pcb_strip;
	  std::vector<int> strip_number;
	  std::vector<int> sector_strip;
	  std::vector<float> charge;
	  std::vector<int> strp_times;
	  std::vector<float> cl_times;
	  std::vector<float> mu_TPC_angle;
	  std::vector<float> x_ontrack;
	  std::vector<float> y_ontrack;
	  std::vector<float> residuals;
	};

	struct MMEfficiencyHistogramStruct {
		std::vector<int> den;
		std::vector<int> num;
		std::vector<int> nGaps;
	};
}

/////////////////////////////////////////////////////////////////////////////
// *********************************************************************
// Public Methods
// ********************************************************************* 

MMRawDataMonAlg::MMRawDataMonAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
	AthMonitorAlgorithm(name,pSvcLocator)
{ }

/*---------------------------------------------------------*/
StatusCode MMRawDataMonAlg::initialize()
/*---------------------------------------------------------*/
{
	//init message stream
	ATH_MSG_DEBUG("initialize MMRawDataMonAlg");
	ATH_MSG_DEBUG("******************");
	ATH_MSG_DEBUG("doMMESD: " << m_doMMESD );
	ATH_MSG_DEBUG("******************");

	ATH_CHECK(AthMonitorAlgorithm::initialize());
	ATH_CHECK(m_DetectorManagerKey.initialize());
	ATH_CHECK(m_idHelperSvc.retrieve());

	ATH_MSG_INFO(" Found the MuonIdHelperSvc ");
	ATH_CHECK(m_muonKey.initialize());
	ATH_CHECK(m_MMContainerKey.initialize());
	ATH_CHECK(m_meTrkKey.initialize());
	ATH_CHECK(m_segm_type.initialize());

	ATH_MSG_DEBUG(" end of initialize " );
	ATH_MSG_INFO("MMRawDataMonAlg initialization DONE " );

	return StatusCode::SUCCESS;
} 

StatusCode MMRawDataMonAlg::fillHistograms(const EventContext& ctx) const
{
	int lumiblock = -1;
	lumiblock = GetEventInfo(ctx)->lumiBlock();
	ATH_MSG_DEBUG("MMRawDataMonAlg::MM RawData Monitoring Histograms being filled" );

	SG::ReadHandle<Muon::MMPrepDataContainer> mm_container(m_MMContainerKey,ctx);
	ATH_MSG_DEBUG("****** mmContainer->size() : " << mm_container->size());

	if(m_doMMESD) {
		MMOverviewHistogramStruct overviewPlots;
		MMSummaryHistogramStruct summaryPlots[2][16][2][2][4];
		MMByPhiStruct occupancyPlots[16][2];

		//loop in MMPrepDataContainer
		for(const Muon::MMPrepDataCollection* coll : *mm_container) {
			for(const Muon::MMPrepData* prd : *coll) {
				ATH_CHECK(fillMMOverviewVects(prd, overviewPlots, occupancyPlots));
				ATH_CHECK(fillMMSummaryVects(prd, summaryPlots));
				ATH_CHECK(fillMMHistograms(prd));
			}
		}

		if(m_do_mm_overview) fillMMOverviewHistograms(overviewPlots, occupancyPlots, lumiblock);

		ATH_CHECK(fillMMSummaryHistograms(summaryPlots));

		SG::ReadHandle<xAOD::TrackParticleContainer> meTPContainer{m_meTrkKey,ctx};
		if (!meTPContainer.isValid()) {
			ATH_MSG_FATAL("Nope. Could not retrieve "<<m_meTrkKey.fullKey());
			return StatusCode::FAILURE;
		}
		clusterFromTrack(meTPContainer.cptr(),lumiblock);
		MMEfficiency(meTPContainer.cptr());
		SG::ReadHandle<Trk::SegmentCollection> segms(m_segm_type, ctx);
        if (!segms.isValid()) {
        	ATH_MSG_ERROR("evtStore() does not contain MM segms Collection with name " << m_segm_type);
        	return StatusCode::FAILURE;
        }
        clusterFromSegments(segms.cptr(),lumiblock);
	}

	return StatusCode::SUCCESS;
}

StatusCode MMRawDataMonAlg::fillMMOverviewVects( const Muon::MMPrepData* prd, MMOverviewHistogramStruct& vects, MMByPhiStruct (&occupancyPlots)[16][2] ) const
{
	Identifier Id = prd->identify();
	const std::vector<Identifier>& stripIds = prd->rdoList();
	unsigned int nStrips = stripIds.size(); // number of strips in this cluster (cluster size)
	const std::vector<uint16_t>& stripNumbers = prd->stripNumbers();

	std::string stName = m_idHelperSvc->mmIdHelper().stationNameString(m_idHelperSvc->mmIdHelper().stationName(Id));
	int gas_gap 	   = m_idHelperSvc->mmIdHelper().gasGap(Id);
	int stationEta 	   = m_idHelperSvc->mmIdHelper().stationEta(Id);
	int stationPhi     = m_idHelperSvc->mmIdHelper().stationPhi(Id);
	int multiplet 	   = m_idHelperSvc->mmIdHelper().multilayer(Id);
	int channel        = m_idHelperSvc->mmIdHelper().channel(Id);

	// Returns the charge (number of electrons) converted in fC
	float charge = prd->charge()*conversion_charge;
	// Returns the times of each strip (in ns)
	std::vector<short int> strip_times = prd->stripTimes();
	// Returns the microTPC angle (radians converted in degrees)
	float mu_TPC_angle = prd->angle()*toDeg;
	// Returns the microTPC chisq Prob.
	float mu_TPC_chi2 = prd->chisqProb();

	Amg::Vector3D pos = prd->globalPosition();
	float R = std::hypot(pos.x(),pos.y());
	int PCB = get_PCB_from_channel(channel);

	// MM gaps are back to back, so the direction of the drift (time) is different for the even and odd gaps -> flip for the even gaps
	if(gas_gap % 2 == 0) mu_TPC_angle = -mu_TPC_angle;

	vects.charge_all.push_back(charge);
	vects.numberofstrips_percluster.push_back(nStrips);
	vects.mu_TPC_angle.push_back(mu_TPC_angle);
	vects.mu_TPC_chi2.push_back(mu_TPC_chi2);
	vects.x_mon.push_back(pos.x());
	vects.y_mon.push_back(pos.y());
	vects.z_mon.push_back(pos.z());
	vects.R_mon.push_back(R);

	// 16 phi sectors, 8 stationPhi times 2 stName, MMS and MML
	int sectorPhi = get_sectorPhi_from_stationPhi_stName(stationPhi,stName);

	// Occupancy plots with PCB granularity further divided for each eta sector: -2, -1, 1, 2
	// CSide and ASide
	int iside = (stationEta>0) ? 1 : 0;

	auto& thisSect = occupancyPlots[sectorPhi-1][iside];
	const int pcb_counter = 5; // index for the PCBs from 1 to 8 as done globally for the two detector components (abs(eta)=1 and abs(eta)=2)
	int PCBeta12 = (std::abs(stationEta) == 2) ? (PCB + pcb_counter) : PCB;

	thisSect.sector_lb.push_back(get_bin_for_occ_lb_pcb_hist(multiplet,gas_gap,PCBeta12));

	// Filling Vectors for stationEta=-2
	if(stationEta==-2) {
		vects.sector_CSide_eta2.push_back(get_bin_for_occ_CSide_pcb_eta2_hist(stationEta,multiplet,gas_gap,PCB));
		vects.stationPhi_CSide_eta2.push_back(sectorPhi);
	}
	// Filling Vectors for stationEta=-1
	else if(stationEta==-1) {
		vects.sector_CSide_eta1.push_back(get_bin_for_occ_CSide_pcb_eta1_hist(stationEta,multiplet,gas_gap,PCB));
		vects.stationPhi_CSide_eta1.push_back(sectorPhi);
	}
	// Filling Vectors for stationEta=1
	else if(stationEta==1) {
		vects.sector_ASide_eta1.push_back(get_bin_for_occ_ASide_pcb_eta1_hist(stationEta,multiplet,gas_gap,PCB));
		vects.stationPhi_ASide_eta1.push_back(sectorPhi);
	}
	// Filling Vectors for stationEta=2
	else {
		vects.sector_ASide_eta2.push_back(get_bin_for_occ_ASide_pcb_eta2_hist(stationEta,multiplet,gas_gap,PCB));
		vects.stationPhi_ASide_eta2.push_back(sectorPhi);
	}

	// loop on each strip
	int sIdx = 0; // index-counter for the vector of Id's
	float cluster_time = 0;
	for(const Identifier& id: stripIds) {
		
		std::string stName_strip = m_idHelperSvc->mmIdHelper().stationNameString(m_idHelperSvc->mmIdHelper().stationName(id));
		int stationEta_strip 	 = m_idHelperSvc->mmIdHelper().stationEta(id);
		vects.statEta_strip.push_back(stationEta_strip);
		vects.strip_number.push_back(stripNumbers[sIdx]);
		vects.strp_times.push_back(strip_times.at(sIdx));
		cluster_time += strip_times.at(sIdx);
		++sIdx;
	}
	cluster_time /= strip_times.size();
	vects.cl_times.push_back(cluster_time);

	return StatusCode::SUCCESS;
}

void MMRawDataMonAlg::fillMMOverviewHistograms( const MMOverviewHistogramStruct& vects,  MMByPhiStruct (&occupancyPlots)[16][2], int lb ) const 
{
	auto charge_all = Monitored::Collection("charge_all", vects.charge_all);
	auto numberofstrips_percluster = Monitored::Collection("numberofstrips_percluster", vects.numberofstrips_percluster);
	fill("mmMonitor", charge_all, numberofstrips_percluster);

	auto strip_times = Monitored::Collection("strip_times", vects.strp_times);
	auto cluster_times = Monitored::Collection("cluster_times", vects.cl_times);
	auto mu_TPC_angle = Monitored::Collection("mu_TPC_angle", vects.mu_TPC_angle);
	auto mu_TPC_chi2 = Monitored::Collection("mu_TPC_chi2", vects.mu_TPC_chi2);
	auto strip_number = Monitored::Collection("strip_number", vects.strip_number);
	auto statEta_strip = Monitored::Collection("statEta_strip", vects.statEta_strip);
	fill("mmMonitor", strip_times, cluster_times, mu_TPC_angle, mu_TPC_chi2, strip_number, statEta_strip);

	auto x_mon = Monitored::Collection("x_mon", vects.x_mon);
	auto y_mon = Monitored::Collection("y_mon", vects.y_mon);
	auto z_mon = Monitored::Collection("z_mon", vects.z_mon);
	auto R_mon = Monitored::Collection("R_mon", vects.R_mon);
	fill("mmMonitor", x_mon, y_mon, z_mon, R_mon);

	auto lb_mon = Monitored::Scalar<int>("lb_mon", lb);

	for(int statPhi=0; statPhi<16; ++statPhi) {
		for(int iside=0; iside<2; ++iside) {
			auto& occ_lb = occupancyPlots[statPhi][iside];
			auto sector_lb = Monitored::Collection("sector_lb_"+MM_Side[iside]+"_phi"+std::to_string(statPhi+1),occ_lb.sector_lb);
			std::string MM_sideGroup = "MM_sideGroup" + MM_Side[iside];
			fill(MM_sideGroup, lb_mon, sector_lb);
		}
	}

	auto sector_CSide_eta2 	   = Monitored::Collection("sector_CSide_eta2",vects.sector_CSide_eta2);
	auto sector_CSide_eta1 	   = Monitored::Collection("sector_CSide_eta1",vects.sector_CSide_eta1);
	auto sector_ASide_eta1 	   = Monitored::Collection("sector_ASide_eta1",vects.sector_ASide_eta1);
	auto sector_ASide_eta2 	   = Monitored::Collection("sector_ASide_eta2",vects.sector_ASide_eta2);
	auto stationPhi_CSide_eta1 = Monitored::Collection("stationPhi_CSide_eta1",vects.stationPhi_CSide_eta1);
	auto stationPhi_CSide_eta2 = Monitored::Collection("stationPhi_CSide_eta2",vects.stationPhi_CSide_eta2);
	auto stationPhi_ASide_eta1 = Monitored::Collection("stationPhi_ASide_eta1",vects.stationPhi_ASide_eta1);
	auto stationPhi_ASide_eta2 = Monitored::Collection("stationPhi_ASide_eta2",vects.stationPhi_ASide_eta2);
	fill("mmMonitor", sector_CSide_eta1, sector_CSide_eta2, sector_ASide_eta1, sector_ASide_eta2, stationPhi_CSide_eta1, stationPhi_CSide_eta2, stationPhi_ASide_eta1, stationPhi_ASide_eta2);
}

StatusCode MMRawDataMonAlg::fillMMSummaryVects( const Muon::MMPrepData* prd, MMSummaryHistogramStruct (&vects)[2][16][2][2][4]) const
{
  Identifier Id = prd->identify();
  const std::vector<Identifier>& stripIds = prd->rdoList();
  
  std::string stName   	= m_idHelperSvc->mmIdHelper().stationNameString(m_idHelperSvc->mmIdHelper().stationName(Id));
  int thisStationEta      = m_idHelperSvc->mmIdHelper().stationEta(Id);
  int thisStationPhi       = m_idHelperSvc->mmIdHelper().stationPhi(Id);
  int thisMultiplet        = m_idHelperSvc->mmIdHelper().multilayer(Id);
  int thisGasgap          = m_idHelperSvc->mmIdHelper().gasGap(Id);
  int ch             = m_idHelperSvc->mmIdHelper().channel(Id);
  float thisCharge=prd->charge()*conversion_charge;
  float thisMu_TPC_angle=prd->angle()*toDeg;
  std::vector<short int> strip_times = prd->stripTimes();

    
    if ( thisGasgap % 2 == 0 ) thisMu_TPC_angle = - thisMu_TPC_angle;

    int phi=get_sectorPhi_from_stationPhi_stName(thisStationPhi ,stName);

    // CSide and ASide
    int iside = (thisStationEta>0) ? 1 : 0;

    // 2 eta sectors depending on Eta=+-1 (0) and +-2 (1)
    int sectorEta=get_sectorEta_from_stationEta(thisStationEta);

    //  auto& Vectors = vects[iside][phisec][thisStationPhi-1][sectorEta][thisMultiplet-1][thisGasgap-1];
    auto& Vectors = vects[iside][phi-1][sectorEta][thisMultiplet-1][thisGasgap-1];
    Vectors.mu_TPC_angle.push_back(thisMu_TPC_angle);
    Vectors.charge.push_back(thisCharge);
    
    unsigned int csize = stripIds.size();
    Vectors.cl_size.push_back(csize);
    int PCB = get_PCB_from_channel(ch);
    Vectors.pcb.push_back(PCB);
  
    // loop on strips
    int sIdx = 0;
    const std::vector<uint16_t>& stripNumbers=prd->stripNumbers();
    float cluster_time = 0;
    for ( const Identifier& id : stripIds) {

    	int stationEta       = m_idHelperSvc->mmIdHelper().stationEta(id);
    	int gas_gap          = m_idHelperSvc->mmIdHelper().gasGap(Id);
    	int multiplet        = m_idHelperSvc->mmIdHelper().multilayer(Id);
    	// Filling Vectors for both sides, considering each strip
    	Vectors.strip_number.push_back(stripNumbers[sIdx]);
    	Vectors.strp_times.push_back(strip_times.at(sIdx));
	cluster_time += strip_times.at(sIdx);
	Vectors.pcb_strip.push_back( get_PCB_from_channel(stripNumbers[sIdx]));
    	++sIdx;
    	if(iside==1)    Vectors.sector_strip.push_back(get_bin_for_occ_ASide_hist(stationEta,multiplet,gas_gap));
    	if(iside==0)    Vectors.sector_strip.push_back(get_bin_for_occ_CSide_hist(stationEta,multiplet,gas_gap));
    }
    cluster_time /= strip_times.size();
	Vectors.cl_times.push_back(cluster_time);

    return StatusCode::SUCCESS;
}

StatusCode MMRawDataMonAlg::fillMMSummaryHistograms( const MMSummaryHistogramStruct (&vects)[2][16][2][2][4]) const {

	for(int iside=0; iside<2; ++iside) {
		std::string MM_sideGroup = "MM_sideGroup" + MM_Side[iside];
		for(int statPhi=0; statPhi<16; ++statPhi) {
			for(int statEta=0; statEta<2; ++statEta) {
				for(int multiplet=0; multiplet<2; ++multiplet) {
					for(int gas_gap=0; gas_gap<4; ++gas_gap) {
						auto& Vectors = vects[iside][statPhi][statEta][multiplet][gas_gap];
						auto sector_strip = Monitored::Collection("sector_strip_" + MM_Side[iside] + "_phi" + std::to_string(statPhi+1), Vectors.sector_strip);
						auto strip_number = Monitored::Collection("strip_number_" + MM_Side[iside] + "_phi" + std::to_string(statPhi+1), Vectors.strip_number);
						if(!Vectors.strip_number.empty())
						{
						  auto cluster_size = Monitored::Collection("cluster_size_" + MM_Side[iside] + "_phi" + std::to_string(statPhi+1) + "_eta" + std::to_string(statEta+1) + "_ml" + std::to_string(multiplet+1) + "_gap" + std::to_string(gas_gap+1), Vectors.cl_size);
							auto strip_times = Monitored::Collection("strp_time_" + MM_Side[iside] + "_phi" + std::to_string(statPhi+1) + "_eta" + std::to_string(statEta+1) + "_ml" + std::to_string(multiplet+1) + "_gap" + std::to_string(gas_gap+1), Vectors.strp_times);
							auto cluster_time = Monitored::Collection("cluster_time_" + MM_Side[iside] + "_phi" + std::to_string(statPhi+1) + "_eta" + std::to_string(statEta+1) + "_ml" + std::to_string(multiplet+1) + "_gap" + std::to_string(gas_gap+1), Vectors.cl_times);
							auto charge_perPCB = Monitored::Collection("charge_perPCB_" + MM_Side[iside] + "_phi" + std::to_string(statPhi+1) + "_eta" + std::to_string(statEta+1) + "_ml" + std::to_string(multiplet+1) + "_gap" + std::to_string(gas_gap+1), Vectors.charge);
							auto pcb_mon = Monitored::Collection("pcb_mon_" + MM_Side[iside] + "_phi" + std::to_string(statPhi+1) + "_eta" + std::to_string(statEta+1) + "_ml" + std::to_string(multiplet+1) + "_gap" + std::to_string(gas_gap+1), Vectors.pcb);
							auto pcb_strip_mon = Monitored::Collection("pcb_strip_mon_" + MM_Side[iside] + "_phi" + std::to_string(statPhi+1) + "_eta" + std::to_string(statEta+1) + "_ml" + std::to_string(multiplet+1) + "_gap" + std::to_string(gas_gap+1), Vectors.pcb_strip);
							fill(MM_sideGroup, cluster_size, strip_times, cluster_time, charge_perPCB, pcb_mon, pcb_strip_mon);
						}
					
						fill(MM_sideGroup, strip_number, sector_strip);
					}
				}
			}
		}
	}

	return StatusCode::SUCCESS;
}

StatusCode MMRawDataMonAlg::fillMMHistograms( const Muon::MMPrepData* ) const{
  return StatusCode::SUCCESS;
}

void MMRawDataMonAlg::clusterFromTrack(const xAOD::TrackParticleContainer*  muonContainer, int lb) const
{
	MMSummaryHistogramStruct summaryPlots[2][2][4]; // side, multilayer, gas gap
	MMSummaryHistogramStruct summaryPlots_full[2][16][2][2][4]; // side, phi, eta, multilayer, gas gap
	MMSummaryHistogramStruct sumPlots[2][16][2][2][4]; // side, phi, eta, multilayer, gas gap
	MMOverviewHistogramStruct overviewPlots;
	MMByPhiStruct occupancyPlots[16][2]; // sector, side
	int ntrk=0;

	for(const xAOD::TrackParticle* meTP : *muonContainer) {
		
		if(!meTP) continue;

		auto eta_trk = Monitored::Scalar<float>("eta_trk", meTP->eta());
		auto phi_trk = Monitored::Scalar<float>("phi_trk", meTP->phi());
		auto pt_trk = Monitored::Scalar<float>("pt_trk", meTP->pt()/1000.);

		//retrieve the original track
		const Trk::Track* meTrack = meTP->track();
		if(!meTrack) continue;
		// get the vector of measurements on track
		const DataVector<const Trk::MeasurementBase>* meas = meTrack->measurementsOnTrack();
		bool isMM=false;
		for(const Trk::MeasurementBase* it : *meas) {
			const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(it);
			if(!rot) continue;
			Identifier rot_id = rot->identify();
			if(!m_idHelperSvc->isMM(rot_id)) continue;
			isMM=true;
			const Muon::MMClusterOnTrack* cluster = dynamic_cast<const Muon::MMClusterOnTrack*>(rot);
			if(!cluster) continue;

			std::string stName = m_idHelperSvc->mmIdHelper().stationNameString(m_idHelperSvc->mmIdHelper().stationName(rot_id));
			int stEta          = m_idHelperSvc->mmIdHelper().stationEta(rot_id);
			int stPhi          = m_idHelperSvc->mmIdHelper().stationPhi(rot_id);
			int multi          = m_idHelperSvc->mmIdHelper().multilayer(rot_id);
			int gap            = m_idHelperSvc->mmIdHelper().gasGap(rot_id);
			int ch             = m_idHelperSvc->mmIdHelper().channel(rot_id);

			// MMS and MML phi sectors
			//				int phisec = (stNumber%2==0) ? 1 : 0;
			int sectorPhi = get_sectorPhi_from_stationPhi_stName(stPhi,stName); // 1->16
			int PCB = get_PCB_from_channel(ch);
			int iside = (stEta > 0) ? 1 : 0;

			auto& vects = overviewPlots;
			auto& thisSect = occupancyPlots[sectorPhi-1][iside];
			auto& vect = sumPlots[iside][sectorPhi-1][std::abs(stEta)-1][multi-1][gap-1];

			const Muon::MMPrepData* prd = cluster->prepRawData();
			const std::vector<Identifier>& stripIds = prd->rdoList();
			unsigned int csize = stripIds.size();
			const std::vector<uint16_t>& stripNumbers = prd->stripNumbers();
			float charge = prd->charge()*conversion_charge;
			std::vector<short int> s_times = prd->stripTimes();

			vects.numberofstrips_percluster.push_back(csize);
			vects.charge_all.push_back(charge);
			vect.cl_size.push_back(csize);
			vect.pcb.push_back(PCB);

			float c_time = 0;
			for(unsigned int sIdx=0; sIdx<stripIds.size(); ++sIdx)
			{
				vects.strp_times.push_back(s_times.at(sIdx));
				vect.strip_number.push_back(stripNumbers[sIdx]);
				vect.strp_times.push_back(s_times.at(sIdx));
				vect.pcb_strip.push_back(get_PCB_from_channel(stripNumbers[sIdx]));
				c_time += s_times.at(sIdx);
			}
			c_time /= s_times.size();
			vects.cl_times.push_back(c_time);
			vect.cl_times.push_back(c_time);
			vect.charge.push_back(charge);

			const int pcb_counter = 5; // index for the PCBs from 1 to 8 as done globally for the two detector components (abs(eta)=1 and abs(eta)=2)
			int PCBeta12 = (std::abs(stEta) == 2) ? (PCB + pcb_counter) : PCB;
			thisSect.sector_lb_ontrack.push_back(get_bin_for_occ_lb_pcb_hist(multi,gap,PCBeta12));
				
			// Occupancy plots with PCB granularity further divided for each eta sector: -2, -1, 1, 2
			// Filling Vectors for stationEta=-1 - cluster on track
			if(stEta==-1) {
				vects.stationPhi_CSide_eta1_ontrack.push_back(sectorPhi);
				vects.sector_CSide_eta1_ontrack.push_back(get_bin_for_occ_CSide_pcb_eta1_hist(stEta, multi, gap, PCB));
			}
			// Filling Vectors for stationEta=-2 - cluster on track
			else if(stEta==-2) {
				vects.stationPhi_CSide_eta2_ontrack.push_back(sectorPhi);
				vects.sector_CSide_eta2_ontrack.push_back(get_bin_for_occ_CSide_pcb_eta2_hist(stEta,multi,gap,PCB));
			}
			// Filling Vectors for stationEta=1 - cluster on track
			else if(stEta==1) {
				vects.stationPhi_ASide_eta1_ontrack.push_back(sectorPhi);
				vects.sector_ASide_eta1_ontrack.push_back(get_bin_for_occ_ASide_pcb_eta1_hist(stEta,multi,gap,PCB));
			}
			// Filling Vectors for stationEta=2 - cluster on track
			else {
				vects.stationPhi_ASide_eta2_ontrack.push_back(sectorPhi);
				vects.sector_ASide_eta2_ontrack.push_back(get_bin_for_occ_ASide_pcb_eta2_hist(stEta,multi,gap,PCB));
			}

			float x = cluster->localParameters()[Trk::loc1];
			for(const Trk::TrackStateOnSurface* trkState : *meTrack->trackStateOnSurfaces()) {
					
				if(!(trkState)) continue;
				if (!trkState->type(Trk::TrackStateOnSurface::Measurement)) continue;

				Identifier surfaceId = (trkState)->surface().associatedDetectorElementIdentifier();
				if(!m_idHelperSvc->isMM(surfaceId)) continue;
				
				int trk_stEta = m_idHelperSvc->mmIdHelper().stationEta(surfaceId);
				int trk_stPhi = m_idHelperSvc->mmIdHelper().stationPhi(surfaceId);
				int trk_multi = m_idHelperSvc->mmIdHelper().multilayer(surfaceId);
				int trk_gap   = m_idHelperSvc->mmIdHelper().gasGap(surfaceId);
				
				if( (trk_stPhi == stPhi) && (trk_stEta == stEta) && (trk_multi == multi) && (trk_gap == gap)) {
				  double x_trk = trkState->trackParameters()->parameters()[Trk::loc1];
				  int sectorPhi = get_sectorPhi_from_stationPhi_stName(trk_stPhi,stName); // 1->16
				  int side 	= (stEta > 0) ? 1 : 0;
				  float res_stereo = (x - x_trk);
					if(m_do_stereoCorrection) {
						float stereo_angle = ((multi == 1 && gap < 3) || (multi == 2 && gap > 2)) ? 0 : 0.02618;
						double y_trk = trkState->trackParameters()->parameters()[Trk::locY];
						float stereo_correction = ( (multi == 1 && gap < 3) || (multi == 2 && gap > 2) ) ? 0 : ( ((multi == 1 && gap == 3) || (multi == 2 && gap ==1 )) ? (-std::sin(stereo_angle)*y_trk) : std::sin(stereo_angle)*y_trk );
						res_stereo = (x - x_trk)*std::cos(stereo_angle) - stereo_correction;
					}
					auto residual_mon = Monitored::Scalar<float>("residual", res_stereo);
					auto stPhi_mon = Monitored::Scalar<float>("stPhi_mon",sectorPhi);
					fill("mmMonitor", residual_mon, eta_trk, phi_trk, stPhi_mon);
					int abs_stEta = get_sectorEta_from_stationEta(stEta); // 0 or 1
					auto& vectors = summaryPlots_full[side][sectorPhi-1][abs_stEta][multi-1][gap-1];
					vectors.residuals.push_back(res_stereo);
				}
			}//TrackStates

		} // loop on meas
		if(isMM) {
		  ++ntrk;
		  fill("mmMonitor", pt_trk);
		}

		for(int iside = 0; iside < 2; ++iside) {
			std::string MM_sideGroup = "MM_sideGroup" + MM_Side[iside];
			for(int statPhi = 0; statPhi < 16; ++statPhi) {
				for(int statEta = 0; statEta < 2; ++statEta) {
					for(int multiplet = 0; multiplet < 2; ++multiplet) {
						for(int gas_gap = 0; gas_gap < 4; ++gas_gap) {
							auto& vects = summaryPlots_full[iside][statPhi][statEta][multiplet][gas_gap];
							auto residuals_gap = Monitored::Collection("residuals_"+MM_Side[iside]+"_phi"+std::to_string(statPhi+1)+"_stationEta"+EtaSector[statEta]+"_multiplet"+std::to_string(multiplet+1)+"_gas_gap"+std::to_string(gas_gap+1),vects.residuals);
							fill(MM_sideGroup, residuals_gap);
						}
					}
				}
			}
		}

		for(const Trk::TrackStateOnSurface* trkState : *meTrack->trackStateOnSurfaces()) {
			if(!(trkState)) continue;
			if (!trkState->type(Trk::TrackStateOnSurface::Measurement)) continue;
			Identifier surfaceId = (trkState)->surface().associatedDetectorElementIdentifier();
			if(!m_idHelperSvc->isMM(surfaceId)) continue;

			const Trk::MeasurementBase* meas = trkState->measurementOnTrack() ;
			if(!meas) continue;
			
			const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(meas);
                        if(!rot) continue;
                        Identifier rot_id = rot->identify();
                        if(!m_idHelperSvc->isMM(rot_id)) continue;
			
			const Amg::Vector3D& pos = (trkState)->trackParameters()->position();
			int stEta = m_idHelperSvc->mmIdHelper().stationEta(surfaceId);
			int multi = m_idHelperSvc->mmIdHelper().multilayer(surfaceId);
			int gap   = m_idHelperSvc->mmIdHelper().gasGap(surfaceId);

			// CSide and ASide
			int iside = (stEta > 0) ? 1 : 0;
			auto& Vectors = summaryPlots[iside][multi-1][gap-1];

			// Filling x-y position vectors using the trackStateonSurface
			Vectors.x_ontrack.push_back(pos.x());
			Vectors.y_ontrack.push_back(pos.y());
		}
	} // loop on muonContainer

	auto ntrack = Monitored::Scalar<int>("ntrk",ntrk);
	fill("mmMonitor", ntrack);

	auto& vects = overviewPlots;
	auto stationPhi_CSide_eta1_ontrack = Monitored::Collection("stationPhi_CSide_eta1_ontrack",vects.stationPhi_CSide_eta1_ontrack);
	auto stationPhi_CSide_eta2_ontrack = Monitored::Collection("stationPhi_CSide_eta2_ontrack",vects.stationPhi_CSide_eta2_ontrack);
	auto stationPhi_ASide_eta1_ontrack = Monitored::Collection("stationPhi_ASide_eta1_ontrack",vects.stationPhi_ASide_eta1_ontrack);
	auto stationPhi_ASide_eta2_ontrack = Monitored::Collection("stationPhi_ASide_eta2_ontrack",vects.stationPhi_ASide_eta2_ontrack);
	auto sector_ASide_eta1_ontrack = Monitored::Collection("sector_ASide_eta1_ontrack",vects.sector_ASide_eta1_ontrack);
	auto sector_ASide_eta2_ontrack = Monitored::Collection("sector_ASide_eta2_ontrack",vects.sector_ASide_eta2_ontrack);
	auto sector_CSide_eta2_ontrack = Monitored::Collection("sector_CSide_eta2_ontrack",vects.sector_CSide_eta2_ontrack);                                                                  
	auto sector_CSide_eta1_ontrack = Monitored::Collection("sector_CSide_eta1_ontrack",vects.sector_CSide_eta1_ontrack);   
	auto lb_ontrack = Monitored::Scalar<int>("lb_ontrack", lb);
	auto csize = Monitored::Collection("nstrips_ontrack", vects.numberofstrips_percluster);
	auto charge = Monitored::Collection("charge_ontrack", vects.charge_all);
	auto stime = Monitored::Collection("strip_time_on_track", vects.strp_times);
	auto ctime = Monitored::Collection("cluster_time_on_track", vects.cl_times);
	fill("mmMonitor", csize, charge, stime, ctime, stationPhi_CSide_eta1_ontrack, stationPhi_CSide_eta2_ontrack, stationPhi_ASide_eta1_ontrack, stationPhi_ASide_eta2_ontrack, sector_CSide_eta1_ontrack, sector_CSide_eta2_ontrack, sector_ASide_eta1_ontrack,sector_ASide_eta2_ontrack, lb_ontrack);

	for(int iside = 0; iside < 2; ++iside) {
		std::string MM_sideGroup = "MM_sideGroup" + MM_Side[iside];
		for(int statPhi = 0; statPhi < 16; ++statPhi) {
			for(int statEta = 0; statEta < 2; ++statEta) {
				for(int multiplet = 0; multiplet < 2; ++multiplet) {
					for(int gas_gap = 0; gas_gap < 4; ++gas_gap) {
						auto& vects = sumPlots[iside][statPhi][statEta][multiplet][gas_gap];
						auto strip_number = Monitored::Collection("strip_number_ontrack_" + MM_Side[iside] + "_phi" +std::to_string(statPhi+1), vects.strip_number);
						if(!vects.strip_number.empty())
						{
						  auto clus_size = Monitored::Collection("cluster_size_ontrack_" + MM_Side[iside] + "_phi" + std::to_string(statPhi+1) + "_eta" + std::to_string(statEta+1) + "_ml" + std::to_string(multiplet+1) + "_gap" + std::to_string(gas_gap+1), vects.cl_size);
						  auto strip_times = Monitored::Collection("strp_time_ontrack_" + MM_Side[iside] + "_phi" + std::to_string(statPhi+1) + "_eta" + std::to_string(statEta+1) + "_ml" + std::to_string(multiplet+1) + "_gap" + std::to_string(gas_gap+1), vects.strp_times);
						  auto cluster_time = Monitored::Collection("cluster_time_ontrack_" + MM_Side[iside] + "_phi" + std::to_string(statPhi+1) + "_eta" + std::to_string(statEta+1) + "_ml" + std::to_string(multiplet+1) + "_gap" + std::to_string(gas_gap+1), vects.cl_times);
						  auto charge_perPCB = Monitored::Collection("charge_perPCB_ontrack_" + MM_Side[iside] + "_phi" + std::to_string(statPhi+1) + "_eta" + std::to_string(statEta+1) + "_ml" + std::to_string(multiplet+1) + "_gap" + std::to_string(gas_gap+1), vects.charge);
						  auto pcb_mon = Monitored::Collection("pcb_mon_ontrack_" + MM_Side[iside] + "_phi" + std::to_string(statPhi+1) + "_eta" + std::to_string(statEta+1) + "_ml" + std::to_string(multiplet+1) + "_gap" + std::to_string(gas_gap+1), vects.pcb);
						  auto pcb_strip_mon = Monitored::Collection("pcb_strip_mon_ontrack_" + MM_Side[iside] + "_phi" + std::to_string(statPhi+1) + "_eta" + std::to_string(statEta+1) + "_ml" + std::to_string(multiplet+1) + "_gap" + std::to_string(gas_gap+1), vects.pcb_strip);
						
						  fill(MM_sideGroup, clus_size, strip_times, cluster_time, charge_perPCB, pcb_mon, pcb_strip_mon);
						}

					}
				}
			}
			auto& occ_lb = occupancyPlots[statPhi][iside];
			auto sector_lb_ontrack = Monitored::Collection("sector_lb_"+MM_Side[iside]+"_phi"+std::to_string(statPhi+1)+"_ontrack",occ_lb.sector_lb_ontrack);
			fill(MM_sideGroup, lb_ontrack, sector_lb_ontrack);
		}
		for(int multiplet=0; multiplet<2; ++multiplet) {
			for(int gas_gap=0; gas_gap<4; ++gas_gap) {
				auto& Vectors = summaryPlots[iside][multiplet][gas_gap];
				auto x_ontrack = Monitored::Collection("x_"+MM_Side[iside]+"_multiplet"+std::to_string(multiplet+1)+"_gas_gap_"+std::to_string(gas_gap+1)+"_ontrack", Vectors.x_ontrack);
				auto y_ontrack = Monitored::Collection("y_"+MM_Side[iside]+"_multiplet"+std::to_string(multiplet+1)+"_gas_gap_"+std::to_string(gas_gap+1)+"_ontrack", Vectors.y_ontrack);
				fill(MM_sideGroup, x_ontrack, y_ontrack);
			}
		}
	}

}

void MMRawDataMonAlg::MMEfficiency( const xAOD::TrackParticleContainer*  muonContainer) const
{
	int numOfGaps = 4;
	MMEfficiencyHistogramStruct effPlots[2][2][16][2][4];
	MMEfficiencyHistogramStruct Gaps[2][2][16][2];

	static const std::array<std::string,2> MM_Side = {"CSide", "ASide"};
	static const std::array<std::string,2> EtaSector = {"1","2"};

	for (const xAOD::TrackParticle* meTP  : *muonContainer) {
		if (!meTP) continue;
		auto eta_trk = Monitored::Scalar<float>("eta_trk", meTP->eta());
		auto phi_trk = Monitored::Scalar<float>("phi_trk", meTP->phi());

		float pt_trk = meTP->pt();
		if(pt_trk < m_cut_pt) continue;
		// retrieve the original track
		const Trk::Track* meTrack = meTP->track();
		if(!meTrack) continue;
		// get the vector of measurements on track
		const DataVector<const Trk::MeasurementBase>* meas = meTrack->measurementsOnTrack();

		for(const Trk::MeasurementBase* it: *meas) {
			const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(it);
			if (!rot) continue;
			Identifier rot_id = rot->identify();
			if (!m_idHelperSvc->isMM(rot_id)) continue;

			const Muon::MMClusterOnTrack* cluster = dynamic_cast<const Muon::MMClusterOnTrack*>(rot);
			if (!cluster) continue;
			std::string stName   = m_idHelperSvc->mmIdHelper().stationNameString(m_idHelperSvc->mmIdHelper().stationName(rot_id));
			int stEta= m_idHelperSvc->mmIdHelper().stationEta(rot_id);
			int stPhi= m_idHelperSvc->mmIdHelper().stationPhi(rot_id);
			int phi = get_sectorPhi_from_stationPhi_stName(stPhi,stName);
			int multi = m_idHelperSvc->mmIdHelper().multilayer(rot_id);
			int gap=  m_idHelperSvc->mmIdHelper().gasGap(rot_id);
			int ch=  m_idHelperSvc->mmIdHelper().channel(rot_id);
			int pcb=get_PCB_from_channel(ch);
			int abs_stEta= get_sectorEta_from_stationEta(stEta);
			int iside = (stEta > 0) ? 1 : 0;
			Gaps[iside][abs_stEta][phi-1][multi-1].nGaps.push_back(gap);
			// denominator
			for(int ga=0; ga<numOfGaps; ++ga)
				if(effPlots[iside][abs_stEta][phi-1][multi-1][ga].den.size()==0) effPlots[iside][abs_stEta][phi-1][multi-1][ga].den.push_back(pcb-1);
			//numerator
			if(effPlots[iside][abs_stEta][phi-1][multi-1][gap-1].num.size()==0) effPlots[iside][abs_stEta][phi-1][multi-1][gap-1].num.push_back(pcb-1);
		}
	} // loop on tracks

	unsigned  int nGaptag=3;

	for(int s=0; s<2; ++s) {
		std::string MM_sideGroup = "MM_sideGroup"+MM_Side[s];
		for(int e=0; e<2; ++e) {
			for(int p=0; p<16; ++p) {
				for(int m=0; m<2; ++m) {
					if(Gaps[s][e][p][m].nGaps.size()<nGaptag) continue;
					for(int ga=0; ga<4; ++ga){
						for (unsigned int i=0; i<effPlots[s][e][p][m][ga].den.size(); ++i){
							int den_pcb = effPlots[s][e][p][m][ga].den.at(i);
							auto traversed_pcb = Monitored::Scalar<int>("pcb_eta"+std::to_string(e+1)+"_"+MM_Side[s]+"_phi"+std::to_string(p)+"_multiplet"+std::to_string(m+1)+"_gas_gap"+std::to_string(ga+1),den_pcb);
							auto traversed_pcb_allphi = Monitored::Scalar<int>("pcb_eta"+std::to_string(e+1)+"_allphi_"+MM_Side[s]+"_multiplet"+std::to_string(m+1)+"_gas_gap"+std::to_string(ga+1),den_pcb);
							auto isHit = effPlots[s][e][p][m][ga].num.size() > 0;
							auto hitcut = Monitored::Scalar<int>("hitcut", (int)isHit);
							fill(MM_sideGroup, traversed_pcb, traversed_pcb_allphi, hitcut);
						}
					}
				}
			}
		}
	}
}


void MMRawDataMonAlg::clusterFromSegments(const Trk::SegmentCollection* segms, int lb) const
{
	MMOverviewHistogramStruct overviewPlots;
  	MMByPhiStruct occupancyPlots[16][2];
  	MMSummaryHistogramStruct summaryPlots[2][16][2][2][4];
	int nseg=0;

  	for (Trk::SegmentCollection::const_iterator s = segms->begin(); s != segms->end(); ++s) {
  		const Muon::MuonSegment* segment = dynamic_cast<const Muon::MuonSegment*>(*s);
  		if (segment == nullptr) {
  			ATH_MSG_DEBUG("no pointer to segment!!!");
  			break;
		}
		bool isMM=false;
    	for(unsigned int irot=0;irot<segment->numberOfContainedROTs();irot++){
    		const Trk::RIO_OnTrack* rot = segment->rioOnTrack(irot);
    		if(!rot) continue;
    		Identifier rot_id = rot->identify();
    		if(!m_idHelperSvc->isMM(rot_id)) continue;
		isMM=true;
    		const Muon::MMClusterOnTrack* cluster = dynamic_cast<const Muon::MMClusterOnTrack*>(rot);
    		if(!cluster) continue;

    		std::string stName = m_idHelperSvc->mmIdHelper().stationNameString(m_idHelperSvc->mmIdHelper().stationName(rot_id));
			int stEta          = m_idHelperSvc->mmIdHelper().stationEta(rot_id);
			int stPhi          = m_idHelperSvc->mmIdHelper().stationPhi(rot_id);
			int multi          = m_idHelperSvc->mmIdHelper().multilayer(rot_id);
			int gap            = m_idHelperSvc->mmIdHelper().gasGap(rot_id);
			int ch             = m_idHelperSvc->mmIdHelper().channel(rot_id);

      		// MMS and MML phi sectors                                                                                                                                                                                                  
      		int sectorPhi = get_sectorPhi_from_stationPhi_stName(stPhi,stName);
      		int PCB = get_PCB_from_channel(ch);
      		int iside = (stEta > 0) ? 1 : 0;

      		const Muon::MMPrepData* prd = cluster->prepRawData();
      		const std::vector<Identifier>& stripIds = prd->rdoList();
      		unsigned int csize = stripIds.size();
      		const std::vector<uint16_t>& stripNumbers = prd->stripNumbers();

      		auto& pcb_vects = summaryPlots[iside][sectorPhi-1][std::abs(stEta)-1][multi-1][gap-1];
      		pcb_vects.cl_size.push_back(csize);
		pcb_vects.pcb.push_back(PCB);
      		std::vector<short int> s_times = prd->stripTimes();
      		float c_time = 0;
      		for(unsigned int sIdx=0; sIdx<csize; ++sIdx) {
		  pcb_vects.strp_times.push_back(s_times.at(sIdx));
		  pcb_vects.pcb_strip.push_back( get_PCB_from_channel(stripNumbers[sIdx]));
		  c_time += s_times.at(sIdx);
		}
      		c_time /= s_times.size();
      		pcb_vects.cl_times.push_back(c_time);

      		float charge = prd->charge()*conversion_charge;
		pcb_vects.charge.push_back(charge);

      		auto& vects = overviewPlots;

      		auto& thisSect = occupancyPlots[sectorPhi-1][iside];
      		const int pcb_counter = 5; // index for the PCBs from 1 to 8 as done globally for the two detector components (abs(eta)=1 and abs(eta)=2)                                                                                   
      		int PCBeta12 = (std::abs(stEta) == 2) ? (PCB + pcb_counter) : PCB;
      		thisSect.sector_lb_onseg.push_back(get_bin_for_occ_lb_pcb_hist(multi,gap,PCBeta12));

      		// Filling Vectors for stationEta=-1 - cluster on track                                                                                                                                                                     
     		if(stEta==-1) {
				vects.stationPhi_CSide_eta1_onseg.push_back(sectorPhi);
				vects.sector_CSide_eta1_onseg.push_back(get_bin_for_occ_CSide_pcb_eta1_hist(stEta, multi, gap, PCB));
      		}
      		// Filling Vectors for stationEta=-2 - cluster on track                                                                                                                                                         
      		else if(stEta==-2) {
				vects.stationPhi_CSide_eta2_onseg.push_back(sectorPhi);
				vects.sector_CSide_eta2_onseg.push_back(get_bin_for_occ_CSide_pcb_eta2_hist(stEta,multi,gap,PCB));
      		}
      		// Filling Vectors for stationEta=1 - cluster on track                                                                                                                                                                      
      		else if(stEta==1) {
				vects.stationPhi_ASide_eta1_onseg.push_back(sectorPhi);
				vects.sector_ASide_eta1_onseg.push_back(get_bin_for_occ_ASide_pcb_eta1_hist(stEta,multi,gap,PCB));
      		}
      		// Filling Vectors for stationEta=2 - cluster on track                                                                                                                                                                      
      		else {
				vects.stationPhi_ASide_eta2_onseg.push_back(sectorPhi);
				vects.sector_ASide_eta2_onseg.push_back(get_bin_for_occ_ASide_pcb_eta2_hist(stEta,multi,gap,PCB));
      		}
    	} // loop on ROT container                                                                                                                                                                                                           
	if (isMM==true) ++nseg;

  	} // loop on segment collection                                                                                                                                                                  
	auto nsegs = Monitored::Scalar<int>("nseg",nseg);
	fill("mmMonitor", nsegs);

  	auto& vects = overviewPlots;
  	auto stationPhi_CSide_eta1_onseg = Monitored::Collection("stationPhi_CSide_eta1_onseg",vects.stationPhi_CSide_eta1_onseg);
  	auto stationPhi_CSide_eta2_onseg = Monitored::Collection("stationPhi_CSide_eta2_onseg",vects.stationPhi_CSide_eta2_onseg);
  	auto stationPhi_ASide_eta1_onseg = Monitored::Collection("stationPhi_ASide_eta1_onseg",vects.stationPhi_ASide_eta1_onseg);
  	auto stationPhi_ASide_eta2_onseg = Monitored::Collection("stationPhi_ASide_eta2_onseg",vects.stationPhi_ASide_eta2_onseg);
  	auto sector_ASide_eta1_onseg = Monitored::Collection("sector_ASide_eta1_onseg",vects.sector_ASide_eta1_onseg);
  	auto sector_ASide_eta2_onseg = Monitored::Collection("sector_ASide_eta2_onseg",vects.sector_ASide_eta2_onseg);
  	auto sector_CSide_eta2_onseg = Monitored::Collection("sector_CSide_eta2_onseg",vects.sector_CSide_eta2_onseg);
  	auto sector_CSide_eta1_onseg = Monitored::Collection("sector_CSide_eta1_onseg",vects.sector_CSide_eta1_onseg);
  	auto lb_onseg = Monitored::Scalar<int>("lb_onseg", lb);
	
  	fill("mmMonitor", stationPhi_CSide_eta1_onseg, stationPhi_CSide_eta2_onseg, stationPhi_ASide_eta1_onseg, stationPhi_ASide_eta2_onseg, sector_CSide_eta1_onseg, sector_CSide_eta2_onseg, sector_ASide_eta1_onseg, sector_ASide_eta2_onseg, lb_onseg);


  	for(int iside = 0; iside < 2; ++iside) {
    	std::string MM_sideGroup = "MM_sideGroup" + MM_Side[iside];
    	for(int statPhi=0; statPhi<16; ++statPhi) {
      		auto& occ_lb = occupancyPlots[statPhi][iside];
      		auto sector_lb_onseg = Monitored::Collection("sector_lb_"+MM_Side[iside]+"_phi"+std::to_string(statPhi+1)+"_onseg",occ_lb.sector_lb_onseg);
      		fill(MM_sideGroup, lb_onseg, sector_lb_onseg);

      		for(int statEta = 0; statEta < 2; ++statEta) {
				for(int multiplet = 0; multiplet < 2; ++multiplet) {
	  				for(int gas_gap = 0; gas_gap < 4; ++gas_gap) {
	    				auto& pcb_vects = summaryPlots[iside][statPhi][statEta][multiplet][gas_gap];

	    				if(pcb_vects.pcb.empty()) continue;

	    				auto pcb_mon = Monitored::Collection("pcb_mon_onseg_" + MM_Side[iside] + "_phi" + std::to_string(statPhi+1) + "_eta" + std::to_string(statEta+1) + "_ml" + std::to_string(multiplet+1) + "_gap" + std::to_string(gas_gap+1), pcb_vects.pcb);
	    				auto pcb_strip_mon = Monitored::Collection("pcb_strip_mon_onseg_" + MM_Side[iside] + "_phi" + std::to_string(statPhi+1) + "_eta" + std::to_string(statEta+1) + "_ml" + std::to_string(multiplet+1) + "_gap" + std::to_string(gas_gap+1), pcb_vects.pcb_strip);
	    				auto strip_times = Monitored::Collection("strp_time_onseg_" + MM_Side[iside] + "_phi" + std::to_string(statPhi+1) + "_eta" + std::to_string(statEta+1) + "_ml" + std::to_string(multiplet+1) + "_gap" + std::to_string(gas_gap+1), pcb_vects.strp_times);
	    				auto cluster_time = Monitored::Collection("cluster_time_onseg_" + MM_Side[iside] + "_phi" + std::to_string(statPhi+1) + "_eta" + std::to_string(statEta+1) + "_ml" + std::to_string(multiplet+1) + "_gap" + std::to_string(gas_gap+1), pcb_vects.cl_times);
	    				auto clus_size = Monitored::Collection("cluster_size_onseg_" + MM_Side[iside] + "_phi" + std::to_string(statPhi+1) + "_eta" + std::to_string(statEta+1) + "_ml" + std::to_string(multiplet+1) + "_gap" + std::to_string(gas_gap+1), pcb_vects.cl_size);
	    				auto charge_perPCB = Monitored::Collection("charge_perPCB_onseg_" + MM_Side[iside] + "_phi" + std::to_string(statPhi+1) + "_eta" + std::to_string(statEta+1) + "_ml" + std::to_string(multiplet+1) + "_gap" + std::to_string(gas_gap+1), pcb_vects.charge);
					auto clus_size_all = Monitored::Collection("cluster_size_onseg", pcb_vects.cl_size); 
					auto charge_all = Monitored::Collection("charge_onseg", pcb_vects.charge);
					auto strip_times_all = Monitored::Collection("strp_time_onseg", pcb_vects.strp_times);
	    				fill(MM_sideGroup, clus_size, strip_times, charge_perPCB, cluster_time, pcb_mon, pcb_strip_mon);
					fill("mmMonitor", clus_size_all, charge_all, strip_times_all);
	  				}
				}
      		}
    	}
  	}
}
