/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Package : sTgcRawDataMonAlg
// Author: Sebastian Fuenzalida Garrido
// Local supervisor: Edson Carquin Lopez
// Technical supervisor: Gerardo Vasquez
//
// DESCRIPTION:
// Subject: sTgc --> sTgc raw data monitoring
///////////////////////////////////////////////////////////////////////////////////////////////////////

#include "StgcRawDataMonitoring/StgcRawDataMonAlg.h"

/////////////////////////////////////////////////////////////////////////////
// *********************************************************************
// Public Methods
// ********************************************************************* 
/////////////////////////////////////////////////////////////////////////////

sTgcRawDataMonAlg::sTgcRawDataMonAlg( const std::string& name, ISvcLocator* pSvcLocator ) : AthMonitorAlgorithm(name,pSvcLocator)	      
{
  //Declare the property 
}


StatusCode sTgcRawDataMonAlg::initialize()
{   
  ATH_CHECK(AthMonitorAlgorithm::initialize());
  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_CHECK(m_sTgcContainerKey.initialize());

  ATH_MSG_INFO("sTGCRawDataMonAlg Successfuly initialized");
  
  return StatusCode::SUCCESS;
} 

StatusCode sTgcRawDataMonAlg::fillHistograms(const EventContext& ctx) const
{  
  SG::ReadHandle<Muon::sTgcPrepDataContainer> sTgc_container(m_sTgcContainerKey, ctx);
  ATH_CHECK(sTgc_container.isValid());

  if (m_dosTgcESD && m_dosTgcOverview)  
    {
      for(const Muon::sTgcPrepDataCollection* coll : *sTgc_container)
	{	  
	  for (const Muon::sTgcPrepData* prd : *coll)
	    {
	      fillsTgcOverviewHistograms(prd, *coll);
	      fillsTgcSummaryHistograms(prd);
	    }
	}
    }
     
  return StatusCode::SUCCESS;
}

void sTgcRawDataMonAlg::fillsTgcOverviewHistograms(const Muon::sTgcPrepData *sTgc_object, const Muon::MuonPrepDataCollection<Muon::sTgcPrepData> &prd) const 
{   
  auto charge_all = Monitored::Collection("charge_all", prd, [] (const Muon::sTgcPrepData *aux) 
					  {
					    return aux -> charge();
					  });
  
  auto numberofstrips_percluster = Monitored::Collection("numberofstrips_percluster", prd, [] (const Muon::sTgcPrepData *aux) 
							 {
							   const std::vector<Identifier> &stripIds = aux -> rdoList(); 
							   return stripIds.size();
							 });
  
  fill("sTgcMonitor", charge_all, numberofstrips_percluster);
  
  std::vector<short int> strip_times_target = sTgc_object-> stripTimes();
  std::vector<int> strip_charges_target = sTgc_object-> stripCharges();
  std::vector<short unsigned int> strip_number_target = sTgc_object-> stripNumbers();

  auto time_all = Monitored::Collection("time_all", prd, [] (const Muon::sTgcPrepData *aux) 
					{
					  return aux -> time();
					});
      
  auto strip_times = Monitored::Collection("strip_times", strip_times_target);
  auto strip_charges = Monitored::Collection("strip_charges", strip_charges_target);
  auto strip_number = Monitored::Collection("strip_number", strip_number_target);

  fill("sTgcMonitor", time_all, strip_times, strip_charges, strip_number);

  auto x_mon = Monitored::Collection("x_mon", prd, [] (const Muon::sTgcPrepData *aux) 
				     {
				       Amg::Vector3D pos = aux -> globalPosition(); 
				       return pos.x();
				     });
  
  auto y_mon = Monitored::Collection("y_mon", prd, [] (const Muon::sTgcPrepData *aux) 
				     {
				       Amg::Vector3D pos = aux -> globalPosition(); 
				       return pos.y();
				     });
  
  auto z_mon = Monitored::Collection("z_mon", prd, [] (const Muon::sTgcPrepData *aux) 
				     {
				       Amg::Vector3D pos = aux -> globalPosition(); 
				       return pos.z();
				     });
  
  auto R_mon = Monitored::Collection("R_mon", prd, [] (const Muon::sTgcPrepData *aux) 
				     {
				       Amg::Vector3D pos = aux -> globalPosition(); 
				       return std::hypot(pos.x(), pos.y());
				     });

  fill("sTgcMonitor", x_mon, y_mon, z_mon, R_mon);
}

void sTgcRawDataMonAlg::fillsTgcSummaryHistograms(const Muon::sTgcPrepData *sTgc_object) const
{
  Identifier Id    = sTgc_object   -> identify();
  if(!Id.is_valid()) return;
  int stationPhi   = m_idHelperSvc -> stgcIdHelper().stationPhi(Id);
  int stationEta   = m_idHelperSvc -> stgcIdHelper().stationEta(Id);
  int multiplet    = m_idHelperSvc -> stgcIdHelper().multilayer(Id);
  int gas_gap      = m_idHelperSvc -> stgcIdHelper().gasGap(Id);  
  int channel_type = m_idHelperSvc -> stgcIdHelper().channelType(Id);
  int charge       = sTgc_object   -> charge();
  int iside        = (stationEta > 0) ? 1 : 0;
  std::string stationName = m_idHelperSvc -> stgcIdHelper().stationNameString(m_idHelperSvc -> stgcIdHelper().stationName(Id));
  int stationPhiComplete = get_sectorPhi_from_stationPhi_stName(stationPhi, stationName);
 
  std::vector<int> strip_charges_vec = sTgc_object -> stripCharges();  
  std::vector<short unsigned int> strip_numbers_perPhi_vec = sTgc_object -> stripNumbers();

  std::vector<int> charge_vec;
  std::vector<int> stationPhi_vec;
  std::vector<int> stationEta_vec;
  std::vector<int> stationEta_perPhi_vec;

  charge_vec.push_back(charge);
  stationPhi_vec.push_back(stationPhiComplete);
  stationEta_vec.push_back(stationEta);
  stationEta_perPhi_vec.push_back(stationEta);
  
  if (channel_type == 0)
    {
      auto charge_perLayer_pad_ = Monitored::Collection("charge_pad_" + GeometricSectors::sTgc_Side[iside] + "_multiplet_" + std::to_string(multiplet) + "_gasgap_" + std::to_string(gas_gap), charge_vec);
      auto stationPhi_ = Monitored::Collection("sector_" + GeometricSectors::sTgc_Side[iside] + "_phi_multiplet_" + std::to_string(multiplet) + "_gasgap_" + std::to_string(gas_gap), stationPhi_vec);
      auto stationEta_ = Monitored::Collection("sector_" + GeometricSectors::sTgc_Side[iside] + "_eta_multiplet_" + std::to_string(multiplet) + "_gasgap_" + std::to_string(gas_gap), stationEta_vec);
      fill("sTgc_sideGroup" + GeometricSectors::sTgc_Side[iside], charge_perLayer_pad_, stationPhi_, stationEta_);    
    }
  
  else if (channel_type == 1)
    {      
      auto charge_perLayer_strip_ = Monitored::Collection("charge_strip_" + GeometricSectors::sTgc_Side[iside] + "_multiplet_" + std::to_string(multiplet) + "_gasgap_" + std::to_string(gas_gap), charge_vec);
      auto stationPhi_ = Monitored::Collection("sector_" + GeometricSectors::sTgc_Side[iside] + "_phi_multiplet_" + std::to_string(multiplet) + "_gasgap_" + std::to_string(gas_gap), stationPhi_vec);
      auto stationEta_ = Monitored::Collection("sector_" + GeometricSectors::sTgc_Side[iside] + "_eta_multiplet_" + std::to_string(multiplet) + "_gasgap_" + std::to_string(gas_gap), stationEta_vec); 
      auto stationEta_perPhi_ = Monitored::Collection("sector_" + GeometricSectors::sTgc_Side[iside] + "_eta_multiplet_" + std::to_string(multiplet) + "_gasgap_" + std::to_string(gas_gap) + "_stationPhi_" + std::to_string(stationPhiComplete), stationEta_perPhi_vec);
      auto stripNumber_perLayer_perPhi_strip_ = Monitored::Collection("stripNumber_strip_" + GeometricSectors::sTgc_Side[iside] + "_multiplet_" + std::to_string(multiplet) + "_gasgap_" + std::to_string(gas_gap) + "_stationPhi_" + std::to_string(stationPhiComplete), strip_numbers_perPhi_vec);
      auto charge_perLayer_perPhi_strip_ = Monitored::Collection("charge_strip_" + GeometricSectors::sTgc_Side[iside] + "_multiplet_" + std::to_string(multiplet) + "_gasgap_" + std::to_string(gas_gap) + "_stationPhi_" + std::to_string(stationPhiComplete), charge_vec);
      fill("sTgc_sideGroup" + GeometricSectors::sTgc_Side[iside], charge_perLayer_strip_, stationPhi_, stationEta_, stationEta_perPhi_, stripNumber_perLayer_perPhi_strip_, charge_perLayer_perPhi_strip_);    
    }
  
  else if (channel_type == 2)
    {
      auto charge_perLayer_wire_ = Monitored::Collection("charge_wire_" + GeometricSectors::sTgc_Side[iside] + "_multiplet_" + std::to_string(multiplet) + "_gasgap_" + std::to_string(gas_gap), charge_vec);
      auto stationPhi_ = Monitored::Collection("sector_" + GeometricSectors::sTgc_Side[iside] + "_phi_multiplet_" + std::to_string(multiplet) + "_gasgap_" + std::to_string(gas_gap), stationPhi_vec);
      auto stationEta_ = Monitored::Collection("sector_" + GeometricSectors::sTgc_Side[iside] + "_eta_multiplet_" + std::to_string(multiplet) + "_gasgap_" + std::to_string(gas_gap), stationEta_vec);
      fill("sTgc_sideGroup" + GeometricSectors::sTgc_Side[iside], charge_perLayer_wire_, stationPhi_, stationEta_);    
    }
}

