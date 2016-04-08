/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/*
 *  MuonTriggerScaleFactors.cxx
 *
 *  Created on: Oct. 22, 2014
 *      Author: Kota Kasahara <kota.kasahara@cern.ch>
 */

#include <sstream>

#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "MuonEfficiencyCorrections/MuonTriggerScaleFactors.h"
#include "xAODEventInfo/EventInfo.h"

#include "PATInterfaces/SystematicCode.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicVariation.h"

#include "PathResolver/PathResolver.h"

static const double commonSystMTSG = 0.01;
static const double muon_barrel_endcap_boundary = 1.05;

namespace CP {

  MuonTriggerScaleFactors::MuonTriggerScaleFactors( const std::string& name):
    asg::AsgTool( name ),
    m_appliedSystematics(0),
    m_runNumber(267639),
    classname(name.c_str()),
    m_max_period(TrigMuonEff::period_undefined)
  {
    declareProperty( "MuonQuality", m_muonquality = "Medium"); // Tight,Medium, Loose, VeryLoose
    declareProperty( "Isolation", m_isolation = ""); // "", "IsoGradient", "IsoLoose", "IsoTight"
    declareProperty( "CalibrationRelease", m_calibration_version = "Data15_ACD_150903");
    // these are for debugging / testing, *not* for general use!
    declareProperty( "filename",m_fileName = "muon_trigger_eff_periodsABCD_sep09.root"); 
    declareProperty( "CustomInputFolder", m_custom_dir = "");
  }
  
  MuonTriggerScaleFactors::~MuonTriggerScaleFactors(){
  }
  
  StatusCode MuonTriggerScaleFactors::initialize(){

    if ( registerSystematics() != CP::SystematicCode::Ok) {
      return StatusCode::FAILURE;
    }

    if(applySystematicVariation(CP::SystematicSet()) != CP::SystematicCode::Ok){
      ATH_MSG_ERROR("Could not configure for nominal settings");
      return StatusCode::FAILURE;
    }
    
    m_max_period = TrigMuonEff::period_undefined;
    TDirectory* origDir = gDirectory;
    Storage storage;
    setStorage(storage);
    
    //const std::string filePath = getPathName(m_directory, m_fileName);
    std::string filePath = "";
    
    if (m_custom_dir == ""){
       filePath = PathResolverFindCalibFile(Form("MuonEfficiencyCorrections/%s/%s", m_calibration_version.c_str(),m_fileName.c_str()));
        if(filePath == ""){
            ATH_MSG_ERROR("Unable to resolve the input file " << m_fileName << " via PathResolver.");
        }
    }
    else {
        ATH_MSG_INFO("Note: setting up with user specified input file location "<<m_custom_dir<< " - this is not encouraged!");
        filePath = Form("%s/%s",m_custom_dir.c_str(),m_fileName.c_str());
    }
    
    TFile* file = TFile::Open(filePath.c_str());
    
    if (not file) {
      ATH_MSG_FATAL("MuonTriggerScaleFactors::initialize Couldn't open file " << filePath );
      return StatusCode::FAILURE;
    }
    
    TDirectory* tempDir = getTemporaryDirectory();
    tempDir->cd();
    
    for (size_t iqu = 0; iqu < storage.quality->size(); ++iqu) {
      for (size_t ibins = 0; ibins < storage.bins->size(); ++ibins) {
	for (size_t iperiod = 0; iperiod < storage.period->size(); ++iperiod) {
	  for (size_t iregion = 0; iregion < storage.region->size(); ++iregion) {
	    for (size_t itype = 0; itype < storage.type->size(); ++itype) {
	      for(size_t isys = 0; isys < storage.systematic->size(); ++isys){
		for(size_t iiso = 0; iiso < storage.isolation->size(); ++iiso){
		  
		  if(storage.type->at(itype).find("data") != std::string::npos &&
		     storage.systematic->at(isys).find("syst") != std::string::npos) continue;



		  std::string histname = ("_MuonTrigEff_" + storage.period->at(iperiod) + "_" +
					  storage.trigger->at(iperiod) + "_" +
					  storage.quality->at(iqu) + "_" +
					  storage.isolation->at(iiso) + 
					  "_EtaPhi_" +
					  storage.bins->at(ibins) + "_" +
					  storage.region->at(iregion) + "_" +
					  storage.type->at(itype) + "_" +
					  storage.systematic->at(isys));
		  
		  std::string path = (storage.quality->at(iqu) + storage.isolation->at(iiso) + "/" +
				      +"period" + storage.period->at(iperiod) + "/" +
				      storage.trigger->at(iperiod) + "/" +
				      + "eff_etaphi_" + storage.bins->at(ibins) + "_" +
				      storage.region->at(iregion) + "_" + 
				      storage.type->at(itype) + "_" +
				      storage.systematic->at(isys));
		  
		  TH2* hist = dynamic_cast<TH2*>(file->Get(path.c_str()));
		  if (not hist) {
		    ATH_MSG_WARNING("MuonTriggerScaleFactors::initialize : not available");
		    continue;
		  }
		  TH2* hcopy = dynamic_cast<TH2*>(hist->Clone());
		  if (not hcopy) {
		    ATH_MSG_FATAL("MuonTriggerScaleFactors::initialize Couldn't make copy of histogram \"%s\"");
		  }
		  
		  std::pair<EfficiencyMap::iterator, bool> rc = m_efficiencyMap.insert(EfficiencyPair(histname, hcopy));
		  if (not rc.second) {
		    ATH_MSG_FATAL("MuonTriggerScaleFactors::initialize histogram \"%s\" duplicated");
		  }
		}
	      }
	    }
	  }
	}
      }
    }
    
    file->Close();
    delete file;
    
    origDir->cd();
    
    ATH_MSG_INFO("MuonTriggerScaleFactors::initialize");

    return StatusCode::SUCCESS;
  } // end of initialize function

  ///////////////////////
  // Public functions  //
  ///////////////////////
  
  CorrectionCode MuonTriggerScaleFactors::setRunNumber(Int_t runNumber)
  {
    TrigMuonEff::DataPeriod period = getDataPeriod(runNumber);
    if(period == TrigMuonEff::period_undefined){
      ATH_MSG_ERROR("cannot find corresponding run period. Run should be [266904,272531] for period ABC, [276073,276954] for period D.");
      m_runNumber = 267639;
      return CorrectionCode::Ok;
    }
    m_runNumber = runNumber;
    return CorrectionCode::Ok;
  }
  
  CorrectionCode MuonTriggerScaleFactors::getTriggerScaleFactor(const xAOD::MuonContainer& mucont,
								Double_t& triggersf,
								const std::string& trigger)
  {
    TrigMuonEff::Configuration configuration;
    configuration.runNumber = m_runNumber;
    configuration.setByUser = false;
    if(trigger.find("HLT_2mu14") != std::string::npos){
      CorrectionCode cc = GetTriggerSF_dimu(triggersf, configuration, mucont, trigger);
      return cc;
    }
    else {
      CorrectionCode cc = GetTriggerSF(triggersf, configuration, mucont, trigger);
      return cc;
    }
    return CorrectionCode::Ok;
  }

  CorrectionCode MuonTriggerScaleFactors::getTriggerEfficiency(const xAOD::Muon& mu,
							       Double_t& efficiency,
							       const std::string& trigger,
							       Bool_t dataType)
  {
    TrigMuonEff::Configuration configuration;
    configuration.isData = dataType;
    configuration.runNumber = m_runNumber;
    configuration.setByUser = false;
    configuration.trigger = trigger;
    const Double_t threshold = getThresholds(trigger);
    if (mu.pt() < threshold) {
      efficiency = 0;
      return CorrectionCode::Ok;
    }
    else {
      if (not configuration.setByUser) {
	if (not setConfiguration(const_cast<TrigMuonEff::Configuration&>(configuration))) {
	  efficiency = 0;
	  return CorrectionCode::Ok;
	}
      }
    }
    
    std::string systype = "";
    if( appliedSystematics().matchSystematic(CP::SystematicVariation("MUON_EFF_TrigSystUncertainty", -1)) ){
      systype = "syst_down";
    }
    else if( appliedSystematics().matchSystematic(CP::SystematicVariation("MUON_EFF_TrigSystUncertainty", 1)) ){
      systype = "syst_up";
    }
    else if( appliedSystematics().matchSystematic(CP::SystematicVariation("MUON_EFF_TrigStatUncertainty", -1)) ){
      systype = "stat_down";
    }
    else if( appliedSystematics().matchSystematic(CP::SystematicVariation("MUON_EFF_TrigStatUncertainty", 1)) ){
      systype = "stat_up";
    }
    else {
      systype = "nominal";
    }
    CorrectionCode cc = getMuonEfficiency(efficiency, configuration, mu, trigger, systype);
    return cc;
  }

	bool MuonTriggerScaleFactors::maskFeetRegions(const xAOD::Muon& mu, double dR){
	//Want to take a muon (which will ideally have been previously identified as a triggering muon),
	//double check that the muon fires a L1 trigger (TrigMuonMatching::matchL1) and then
	//find the corresponding RoI from the list of fired RoI's by eta/phi matching
	//Then take the sector address and RoI and compare them to the forbidden ones, return true if forbidden
		
	//First thing: find the RoI that the muon is in, return a warning or something if it doesn't find one
		//get the event's RoI's for Lvl1 Muons
		const xAOD::MuonRoIContainer* muonrois = 0;
		StatusCode sc = evtStore()->retrieve(muonrois, "LVL1MuonRoIs"); //muonRoI's for this event
		if(!sc){
			ATH_MSG_ERROR("MuonTriggerScaleFactor::maskFeetRegions : could not retrieve LVL1MuonRoIs");
			return false; //should return error codes
		}
		
		bool trigMatch = false;
		unsigned int muSectorAddress = 0;
		int muRoI = 0;
		//loop over muon RoI's, see if they fall within some dR of the muon's eta/phi
		for(xAOD::MuonRoIContainer::const_iterator muroi_itr = muonrois->begin() ;
				muroi_itr != muonrois->end() ;
				muroi_itr++) 
		{	
			//if the muon etaphi falls within dR of the RoI's etaphi, we found our match
			if(MuonTriggerScaleFactors::dR(mu.eta(), mu.phi(), (*muroi_itr)->eta(), (*muroi_itr)->phi()) < dR){
				//if(trigMatch = true)
					//ATH_MSG_ERROR("MuonTriggerScaleFactor::maskFeetRegions : dR too big, overlaps multiple RoI's");
					//warning: dR is too big and we have overlapping RoI's
				trigMatch = true;
				muSectorAddress = (*muroi_itr)->getSectorAddress();
				muRoI = (*muroi_itr)->getRoI();
				std::cout << "MuonTriggerScaleFactor: RoI-muon Match found; sec = " << muSectorAddress << ", RoI = " << muRoI << ", RoI eta = " << (*muroi_itr)->eta() << ", phi = " << (*muroi_itr)->phi() << ", Mu eta = " << mu.eta() << ", phi = " << mu.phi();
				break;
			}
		}

		if(!trigMatch)
		  return false; // status code that indicates no RoI has been found for this muon

		//now return true if sector address and RoI fall within the feet region
		if(MuonTriggerScaleFactors::inFeetRegion(muSectorAddress, muRoI))
		{
			std::cout << ", within feet region" << std::endl;
			return true; //status code success?
		}
		else
		{
			std::cout << ", outside of feet region" << std::endl;
			return false; //status code failure?
		}
		
		//May need to change this code to return status codes, in the event that something goes wrong
	}

	bool MuonTriggerScaleFactors::inFeetRegion(unsigned int SectorAddress, unsigned int RoI){	
		//massimo's code for finding feet regions
		bool bad_roi= false;

		if ((SectorAddress & 0xC0) == 0){// barrel
    	unsigned int tow = RoI/4;    // barrel tower
    	int sec  =  ((SectorAddress & 0x3E ) >> 1)  + 32*(SectorAddress & 0x1 );   // barrel sector 0-63

    		if  (( sec==21|| sec==22 || sec==25 || sec==26 || sec==53 || sec==54 || sec==57 || sec==58 ) && // feet sectors
						( tow==2 || tow==4 || tow==5 || tow==7 )) bad_roi=true;    // new feet towers

   			if (( sec==23 || sec==24 || sec==55 || sec==56 ) && // bottom sectors
        		( tow==3 )) bad_roi=true;    // not working elevator towers

		// for debugging:
    if (bad_roi) std::cout  << " sec = " << sec << " tow = " << tow;
    }

		return bad_roi;
	}

	double MuonTriggerScaleFactors::dR(const double eta1,
		                                 const double phi1,
	  	                               const double eta2,
	    	                      			 const double phi2)
	{
		double deta = fabs(eta1 - eta2);
	  double dphi = fabs(phi1 - phi2) < TMath::Pi() ? fabs(phi1 - phi2) : 2*TMath:: \
	    Pi() - fabs(phi1 - phi2);
	  return sqrt(deta*deta + dphi*dphi);
	}

  ///////////////////////
  // Private functions //
  ///////////////////////

  CorrectionCode MuonTriggerScaleFactors::getMuonEfficiency(Double_t& eff,
							    const TrigMuonEff::Configuration& configuration,
							    const xAOD::Muon& muon,
							    const std::string& trigger,
							    const std::string& systematic)
  {
    const double mu_eta = muon.eta();
    const double mu_phi = muon.phi();
    
    const std::string type = (configuration.isData ? "_data" : "_mc");
    const std::string region = ((fabs(mu_eta) < muon_barrel_endcap_boundary) ? "_barrel" : "_endcap" );
    const std::string quality = m_muonquality;
    
    const std::string isolation = (m_isolation.empty() ? "" : "_" + m_isolation);
    
    const std::string chain = (trigger.empty() ? configuration.trigger : trigger);
    
    const std::string histname = "_MuonTrigEff_" + configuration.period + chain + "_" + quality + "_" + isolation + "_EtaPhi_" + configuration.binning + region + type +"_" + systematic;

    const EfficiencyMap* map = &m_efficiencyMap;
    
    if(configuration.replicaIndex >= 0){
      if (configuration.replicaIndex < (int) m_efficiencyMapReplicaArray.size()){
	map = &m_efficiencyMapReplicaArray.at(configuration.replicaIndex);
	
      } else {
	ATH_MSG_ERROR("MuonTriggerScaleFactors::getMuonEfficiency ; index for replicated histograms is out of range.");
	//throw std::runtime_error( "Internal coding error detected" );
      }
    }

    EfficiencyMap::const_iterator cit = map->find(histname);
    if (cit == map->end()) {
      ATH_MSG_ERROR("MuonTriggerScaleFactors::getMuonEfficiency ; Could not find what you are looking for in the efficiency map. This is either a bug or you are not using the right .root file, please double check.");
      return CorrectionCode::OutOfValidityRange;
      //throw std::runtime_error( "Internal coding error detected" );
    }
    
    const int bin = (cit->second)->FindFixBin(mu_eta, mu_phi);
    const double efficiency = (cit->second)->GetBinContent(bin);

    eff = efficiency;
    return CorrectionCode::Ok;

  }


  CorrectionCode MuonTriggerScaleFactors::GetTriggerSF_dimu(Double_t& TriggerSF,
							    const TrigMuonEff::Configuration& configuration,
							    const xAOD::MuonContainer& mucont,
							    const std::string& trigger)
  {

    if(mucont.size() != 2){
      ATH_MSG_FATAL("MuonTriggerScaleFactors::GetTriggerSF;Currently dimuon trigger chains only implemented for events with exactly 2 muons.");
    }
    
    if(not configuration.setByUser) {
      if(not setConfiguration(const_cast<TrigMuonEff::Configuration&>(configuration))) {
	TriggerSF = 0;
	return CorrectionCode::Ok;
      }
    }

    ATH_MSG_DEBUG("The trigger that you choose : " << trigger);
    const Bool_t is_single = ((trigger.find("HLT_mu20_iloose_L1MU15_OR_HLT_mu50")!=std::string::npos) ||
			      (trigger.find("HLT_mu24_iloose_L1MU15_OR_HLT_mu50")!=std::string::npos) ||
			      (trigger.find("HLT_mu14")!=std::string::npos) ||
			      (trigger.find("HLT_mu24_imedium")!=std::string::npos) ||
			      (trigger.find("HLT_mu26_imedium")!=std::string::npos));
    
    const Bool_t is_dimu = (trigger.find("HLT_2mu14") != std::string::npos);

    Double_t eff_data = 0;
    Double_t eff_mc = 0;

    std::string data_err = "";
    std::string mc_err = "";

    if( appliedSystematics().matchSystematic(CP::SystematicVariation("MUON_EFF_TrigSystUncertainty", -1)) ){
      data_err = "nominal";
      mc_err = "syst_up";
    }
    else if( appliedSystematics().matchSystematic(CP::SystematicVariation("MUON_EFF_TrigSystUncertainty", 1)) ){
      data_err = "nominal";
      mc_err = "syst_down";
    }
    else if( appliedSystematics().matchSystematic(CP::SystematicVariation("MUON_EFF_TrigStatUncertainty", -1)) ){
      data_err = "stat_down";
      mc_err = "nominal";
    }
    else if( appliedSystematics().matchSystematic(CP::SystematicVariation("MUON_EFF_TrigStatUncertainty", 1)) ){
      data_err = "stat_up";
      mc_err = "nominal";
    }
    else {
      data_err = "nominal";
      mc_err = "nominal";
    }
    configuration.isData = true;
    if (is_single and is_dimu){
      CorrectionCode result = getSingleOrDimuonEfficiency(eff_data, configuration, mucont, trigger, data_err);    
      if(result != CorrectionCode::Ok) return result;
    }
    else if (is_dimu) {
      CorrectionCode result = getDimuonEfficiency(eff_data, configuration, mucont, trigger, data_err);
      if(result != CorrectionCode::Ok) return result;
    }else {
      ATH_MSG_FATAL("MuonTriggerScaleFactors::GetTriggerSF;unknown trigger combination");
      throw std::runtime_error("unknown trigger combination");
    }
    
    configuration.isData = false;
    if (is_single and is_dimu){
      CorrectionCode result = getSingleOrDimuonEfficiency(eff_mc, configuration, mucont, trigger, mc_err);
      if(result != CorrectionCode::Ok) return result;
    }
    else if (is_dimu){
      CorrectionCode result = getDimuonEfficiency(eff_mc, configuration, mucont, trigger, mc_err);
      if(result != CorrectionCode::Ok) return result;
    }
    

    double event_SF = 1.;
    
    if (fabs(1. - eff_mc) > 0.0001){
      event_SF = eff_data/eff_mc;
    }

    TriggerSF = event_SF;
    return CorrectionCode::Ok;
  }

  CorrectionCode MuonTriggerScaleFactors::GetTriggerSF(Double_t& TriggerSF,
						       const TrigMuonEff::Configuration& configuration,
						       const xAOD::MuonContainer& mucont,
						       const std::string& trigger)
  {
    const Double_t threshold = getThresholds(trigger);
    
    double rate_not_fired_data = 1.;
    double rate_not_fired_mc   = 1.;
    
    for (const auto& mu : mucont){
      
      double eff_data = 0., eff_mc = 0.;
      
      if (mu->pt() < threshold) {
	
	eff_data = 0.;
	eff_mc   = 0.;
	
      } else {
	
	if (not configuration.setByUser) {
	  if (not setConfiguration(const_cast<TrigMuonEff::Configuration&>(configuration))) {
	    TriggerSF = 0;
	    return CorrectionCode::Ok;
	  }
	}
	
	std::string muon_trigger_name = trigger;
	std::string data_err = "";
	std::string mc_err = "";
	if( appliedSystematics().matchSystematic(CP::SystematicVariation("MUON_EFF_TrigSystUncertainty", -1)) ){
	  data_err = "nominal";
	  mc_err = "syst_up";
	}
	else if( appliedSystematics().matchSystematic(CP::SystematicVariation("MUON_EFF_TrigSystUncertainty", 1)) ){
	  data_err = "nominal";
	  mc_err = "syst_down";
	}
	else if( appliedSystematics().matchSystematic(CP::SystematicVariation("MUON_EFF_TrigStatUncertainty", -1)) ){
	  data_err = "stat_down";
	  mc_err = "nominal";
	}
	else if( appliedSystematics().matchSystematic(CP::SystematicVariation("MUON_EFF_TrigStatUncertainty", 1)) ){
	  data_err = "stat_up";
	  mc_err = "nominal";
	}
	else {
	  data_err = "nominal";
	  mc_err = "nominal";
	}
	
	configuration.isData = true;
	CorrectionCode result_data = getMuonEfficiency(eff_data, configuration, *mu, muon_trigger_name, data_err);
	if(result_data != CorrectionCode::Ok) return result_data;
	configuration.isData = false;
	CorrectionCode result_mc = getMuonEfficiency(eff_mc, configuration, *mu, muon_trigger_name, mc_err);
	if(result_mc != CorrectionCode::Ok) return result_data;	
      }
      rate_not_fired_data *= ( 1. - eff_data );
      rate_not_fired_mc   *= ( 1. - eff_mc );
    }
    
    double event_SF = 1.;
    
    if ((mucont.size()) and
	(fabs(1. - rate_not_fired_mc) > 0.0001)){
      
      event_SF        = ( 1. - rate_not_fired_data ) / ( 1. - rate_not_fired_mc );

    }

    TriggerSF = event_SF;
    
    return CorrectionCode::Ok;
  }

  CorrectionCode MuonTriggerScaleFactors::getSingleOrDimuonEfficiency(Double_t& eff,
								      const TrigMuonEff::Configuration& config,
								      const xAOD::MuonContainer& mucont,
								      const std::string& chain,
								      const std::string& systematic)
    
  {
    ATH_MSG_DEBUG("getSingleOrDimuonEfficiency : the argument chain is " << chain);
    ATH_MSG_DEBUG("getSingleOrDimuonEfficiency : Configuration.trigger is " << config.trigger);
    
    const Double_t threshold = getThresholds(chain);

    xAOD::MuonContainer::const_iterator mu1 = mucont.begin();
    xAOD::MuonContainer::const_iterator mu2 = mucont.begin() + 1;

    // single muon trigger
    std::string singletrig = "";
    if(chain.find("HLT_mu20_iloose_L1MU15_OR_HLT_mu50") != std::string::npos){
      singletrig = "HLT_mu20_iloose_L1MU15_OR_HLT_mu50";
    }
    else if(chain.find("HLT_mu24_iloose_L1MU15_OR_HLT_mu50") != std::string::npos){
      singletrig = "HLT_mu24_iloose_L1MU15_OR_HLT_mu50";
    }
    else if(chain.find("HLT_mu24_imedium_OR_HLT_mu50") != std::string::npos){
      singletrig = "HLT_mu24_iloose_L1MU15_OR_HLT_mu50";
    }
    else if(chain.find("HLT_mu26_imedium_OR_HLT_mu50") != std::string::npos){
      singletrig = "HLT_mu24_iloose_L1MU15_OR_HLT_mu50";
    }
    else {
      ATH_MSG_ERROR("MuonTriggerScaleFactors::getSingleOrDimuonEfficiency ; Invalid dimuon or combination of single and dimuon trigger chain name given");
    }    
    double mu1_eff = 0;
    if((**mu1).pt()*0.001 > threshold){
      CorrectionCode result_mu1 = getMuonEfficiency(mu1_eff, config, **mu1, singletrig, systematic);
      if(result_mu1 != CorrectionCode::Ok)return result_mu1;
    }
    double mu2_eff = 0;
    if((**mu1).pt()*0.001 > threshold){
      CorrectionCode result_mu2 = getMuonEfficiency(mu2_eff, config, **mu2, singletrig, systematic);
      if(result_mu2 != CorrectionCode::Ok) return result_mu2;
    }
    
    Double_t SingleEff = mu1_eff + mu2_eff - mu1_eff*mu2_eff;
    
    // Di-muon trigger
    std::string dimutrig = "";
    if(chain.find("HLT_2mu14") != std::string::npos) {
      dimutrig = "HLT_2mu14";
    }
    else {
      ATH_MSG_ERROR("MuonTriggerScaleFactors::getSingleOrDimuonEfficiency ; Invalid dimuon or combination of single and dimuon trigger chain name given");
    }
    Double_t dimu_eff_2mu14;
    CorrectionCode result = getDimuonEfficiency(dimu_eff_2mu14, config, mucont, dimutrig, systematic);
    if(result != CorrectionCode::Ok) return result;
    
    Double_t SingleOrDimu = 0;

    SingleOrDimu = SingleEff + dimu_eff_2mu14 - SingleEff*dimu_eff_2mu14;

    eff = SingleOrDimu;

    return CorrectionCode::Ok;
  }


  CorrectionCode MuonTriggerScaleFactors::getDimuonEfficiency(Double_t& eff,
							      const TrigMuonEff::Configuration& configuration,
							      const xAOD::MuonContainer& mucont,
							      const std::string& chain,
							      const std::string& systematic)
  {

    DileptonThresholds thresholds;
    getDileptonThresholds(thresholds);
    
    double threshold_leg1 = 0.;
    double threshold_leg2 = 0.;
    
    if(chain.find("2mu14") != std::string::npos) {
      threshold_leg1 = thresholds.mu14;
      threshold_leg2 = thresholds.mu14;
    }
    else {
      ATH_MSG_ERROR("MuonTriggerScaleFactors::getDimuonEfficiency ; Invalid dimuon or combination of single and dimuon trigger chain name given");
    }

    DileptonTrigger dimuon;
    getDileptonLegs(chain, dimuon);
    
    xAOD::MuonContainer::const_iterator mu1 = mucont.begin();
    xAOD::MuonContainer::const_iterator mu2 = mucont.begin() + 1;

    // data
    Double_t eff1 = 0;
    if((**mu1).pt()*0.001 > threshold_leg1){
      CorrectionCode result1 = getMuonEfficiency(eff1, configuration, (**mu1), dimuon.leg1, systematic);
      if(result1 != CorrectionCode::Ok) return result1;
    }
    Double_t eff2 = 0;
    if((**mu2).pt()*0.001 > threshold_leg2){
      CorrectionCode result2 = getMuonEfficiency(eff2, configuration, (**mu2), dimuon.leg2, systematic);
      if(result2 != CorrectionCode::Ok) return result2;
    }
    
    //    Double_t TrigEff = 1 - (1 - eff1)*(1 - eff2);
    Double_t TrigEff = eff1*eff2;
    
    eff = TrigEff;
      
    return CorrectionCode::Ok;
  }
  
  CorrectionCode MuonTriggerScaleFactors::setMaxPeriod(const TrigMuonEff::DataPeriod x)
  {
    m_max_period = x;
    return CorrectionCode::Ok;
  }

  // Private functions
  void MuonTriggerScaleFactors::getDileptonLegs(const std::string& chain,
					DileptonTrigger& dilepton)
  {
    if (chain.find("2mu14") != std::string::npos) {
      dilepton.leg1 = "HLT_mu14";
      dilepton.leg2 = "HLT_mu14";
      dilepton.bothLegs = "HLT_mu14";
    }
    else {
      dilepton.leg1 = "";
      dilepton.leg2 = "";
      dilepton.bothLegs = "";
      //ATH_MSG_ERROR( "MuonTriggerScaleFactors::getDileptonLegs Invalid dilepton or combination of single and dilepton trigger chain name given");
    }
  }

  Double_t MuonTriggerScaleFactors::getThresholds(const std::string& trigger)
  {
    Double_t result = 20000*1.05;
    if(trigger.find("HLT_mu14") != std::string::npos) result = 14000*1.05;
    if(trigger.find("HLT_mu20_iloose_L1MU15") != std::string::npos) result = 20000*1.05;
    if(trigger.find("HLT_mu20_iloose_L1MU15") != std::string::npos) result = 20000*1.05;
    if(trigger.find("HLT_mu24_iloose_L1MU15") != std::string::npos) result = 24000*1.05;
    if(trigger.find("HLT_mu24_imedium") != std::string::npos) result = 24000*1.05;
    if(trigger.find("HLT_mu26_imedium") != std::string::npos) result = 26000*1.05;
    return result;
  }

  
  bool MuonTriggerScaleFactors::setConfiguration(TrigMuonEff::Configuration& config,
						 TrigMuonEff::DataPeriod period) const
  {
    if (period == TrigMuonEff::period_undefined) {
      period = getDataPeriod(config.runNumber);
      if (m_max_period != TrigMuonEff::period_undefined) {
	if (period > m_max_period) {
	  static bool hasWarned = false;
	  if (not hasWarned) {
	    ATH_MSG_WARNING("MuonTriggerScaleFactors::period out of range:setting it to ");
	    hasWarned = true;
	  }
	  period = m_max_period;
	}
      }
      
      if (period == TrigMuonEff::period_undefined) {
	ATH_MSG_ERROR("MuonTriggerScaleFactors::setConfiguration RunNumber is not in 2015 dataset. No scale factors calculated.");
	return false;
      }
      
      if (period == TrigMuonEff::period_runnumber_zero) {
	ATH_MSG_WARNING("MuonTriggerScaleFactors::setConfiguration;RunNumber is 0. If the value of pileupReweight is 0, this is no problem. (Trigger SF also returns 0)");
	return false;
      }
    }
    switch (period) {
    case TrigMuonEff::per2015ABC:
      config.period = "ABC_";
      config.binning = "coarse";
      config.trigger = "HLT_mu20_iloose_L1MU15_OR_HLT_mu50";
      break;
      
    case TrigMuonEff::per2015D:
      config.period = "D_";
      config.binning = "coarse";
      config.trigger = "HLT_mu20_iloose_L1MU15_OR_HLT_mu50";
      break;

    default:
      ATH_MSG_ERROR("MuonTriggerScaleFactors::setConfiguration;Unknown data period " << config.runNumber << " requested");
      return false;
    }
    
    return true;
  }
  
  void MuonTriggerScaleFactors::getDileptonThresholds(DileptonThresholds& thresholds)
  {
    thresholds.mu10 = 11;
    thresholds.mu14 = 15;
    thresholds.mu18 = 19;
    thresholds.mu8noL1 = 10;
  }
  
  TrigMuonEff::DataPeriod MuonTriggerScaleFactors::getDataPeriod(int runNumber)
  {
    if( runNumber >= 266904 && runNumber <= 272531 ) return TrigMuonEff::per2015ABC;
    if( runNumber >= 276073 && runNumber <= 276954 ) return TrigMuonEff::per2015D;
    return TrigMuonEff::perUnDefined;
  }
  
  void
  MuonTriggerScaleFactors::setStorage(Storage& storage) const
  {
    static const std::string type_[] = {"data", "mc"};
    static const std::vector<std::string> type(type_, type_ + sizeof(type_)/sizeof(std::string));
    
    static const std::string region_[]  = {"barrel", "endcap"};
    static const std::vector<std::string> region(region_, region_ + sizeof(region_)/sizeof(std::string));

    static std::vector<std::string> quality2015;
    quality2015.clear();
    quality2015.push_back("Tight");
    quality2015.push_back("Medium");
    quality2015.push_back("Loose");

    static std::vector<std::string> isolation2015;
    isolation2015.clear();
    isolation2015.push_back("");
    isolation2015.push_back("_IsoGradient");
    isolation2015.push_back("_IsoLoose");
    isolation2015.push_back("_IsoTight");

    // 2015
    static const std::string bins2015_[] = {"coarse", "fine"};
    static const std::vector<std::string> bins2015(bins2015_, bins2015_ + sizeof(bins2015_)/sizeof(std::string));
    
    static const std::string trigger2015_[] = {"HLT_mu20_iloose_L1MU15","HLT_mu20_iloose_L1MU15",
					       "HLT_mu24_iloose_L1MU15","HLT_mu24_iloose_L1MU15",
					       "HLT_mu24_imedium","HLT_mu24_imedium",
					       "HLT_mu26_imedium","HLT_mu26_imedium",
					       "HLT_mu50","HLT_mu50",
					       "HLT_mu20_iloose_L1MU15_OR_HLT_mu50","HLT_mu20_iloose_L1MU15_OR_HLT_mu50",
					       "HLT_mu24_iloose_L1MU15_OR_HLT_mu50","HLT_mu24_iloose_L1MU15_OR_HLT_mu50",
					       "HLT_mu24_imedium_OR_HLT_mu50","HLT_mu24_imedium_OR_HLT_mu50",
					       "HLT_mu26_imedium_OR_HLT_mu50", "HLT_mu26_imedium_OR_HLT_mu50"};
    
    static const std::vector<std::string> trigger2015(trigger2015_, trigger2015_ + sizeof(trigger2015_)/sizeof(std::string));
    
    static const std::string period2015_[] = {"ABC", "D",
					      "ABC", "D",
					      "ABC", "D",
					      "ABC", "D",
					      "ABC", "D",
					      "ABC", "D",
					      "ABC", "D",
					      "ABC", "D",
					      "ABC", "D"};

    static const std::vector<std::string> period2015(period2015_, period2015_ + sizeof(period2015_)/sizeof(std::string));
    
    static const std::string systematic_[] = {"nominal","stat_up","stat_down",
					      "syst_up","syst_down"};

    static const std::vector<std::string> systematic(systematic_, systematic_ + sizeof(systematic_)/sizeof(std::string));
    
    storage.type = &type;
    storage.region = &region;
    storage.systematic = &systematic;
    storage.trigger = &trigger2015;
    storage.period = &period2015;
    storage.quality = &quality2015;
    storage.isolation = &isolation2015;
    storage.bins = &bins2015;
    
    if (storage.period->size() != storage.trigger->size()) {
       ATH_MSG_FATAL("MuonTriggerScaleFactors::setStorage : size of period and trigger is different : check configuration");
    }
    
    return;
  }
  
  TDirectory* MuonTriggerScaleFactors::getTemporaryDirectory(void) const
  {
    gROOT->cd();
    TDirectory* tempDir = 0;
    int counter = 0;
    while (not tempDir) {
      std::stringstream dirname;
      dirname << "MuonTriggerScaleFactorsTempDir_%i" << counter;
      if (gROOT->GetDirectory((dirname.str()).c_str())) {
	++counter;
	continue;
      }
      tempDir = gROOT->mkdir((dirname.str()).c_str());
      if (not tempDir) {
	ATH_MSG_ERROR("getTemporaryDirectory::Temporary directory could not be created");
      }
    }
    return tempDir;
  }
  
  //=======================================================================
  //   Systematics Interface
  //=======================================================================
  bool MuonTriggerScaleFactors::isAffectedBySystematic( const CP::SystematicVariation& systematic ) const {
    if(!systematic.empty()){
      CP::SystematicSet sys = affectingSystematics();
      return sys.find(systematic) != sys.end();
    }
    return true;
  }

  /// returns: the list of all systematics this tool can be affected by
  CP::SystematicSet MuonTriggerScaleFactors::affectingSystematics() const {
    CP::SystematicSet mySysSet;
    
    mySysSet.insert(CP::SystematicVariation("MUON_EFF_TrigStatUncertainty", 1));
    mySysSet.insert(CP::SystematicVariation("MUON_EFF_TrigStatUncertainty", -1));
    mySysSet.insert(CP::SystematicVariation("MUON_EFF_TrigSystUncertainty", 1));
    mySysSet.insert(CP::SystematicVariation("MUON_EFF_TrigSystUncertainty", -1));

    return mySysSet;
  }

  // Register the systematics with the registry and add them to the recommended list
  CP::SystematicCode MuonTriggerScaleFactors::registerSystematics() {
    CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
    if (registry.registerSystematics(*this) != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR("Failed to add systematic to list of recommended systematics.");
      return CP::SystematicCode::Unsupported;
    }
    return CP::SystematicCode::Ok;
  }

  /// returns: the list of all systematics this tool recommends to use
  CP::SystematicSet MuonTriggerScaleFactors::recommendedSystematics() const{
    return affectingSystematics();
  }

  CP::SystematicCode MuonTriggerScaleFactors::
  applySystematicVariation ( const CP::SystematicSet& systConfig )
  {
    // First, check if we already know this systematic configuration
    auto itr = m_systFilter.find(systConfig);

    // If it's a new input set, we need to filter it
    if( itr == m_systFilter.end() ){

      // New systematic. We need to parse it.
      static CP::SystematicSet affectingSys = affectingSystematics();
      CP::SystematicSet filteredSys;
      if (!CP::SystematicSet::filterForAffectingSystematics(systConfig, affectingSys, filteredSys)){
	ATH_MSG_ERROR("Unsupported combination of systematics passed to the tool!");
	return CP::SystematicCode::Unsupported;
      }

      // Insert filtered set into the map
      itr = m_systFilter.insert(std::make_pair(systConfig, filteredSys)).first;
    }

    CP::SystematicSet& mySysConf = itr->second;

    // Check to see if the set of variations tries to add in the uncertainty up and down. Since the errors
    // are symetric this would result in 0 and so should not be done.
    if( (mySysConf.matchSystematic( CP::SystematicVariation("MUON_EFF_TrigStatUncertainty", 1)) && 
	 mySysConf.matchSystematic( CP::SystematicVariation("MUON_EFF_TrigStatUncertainty", -1))) ||
	(mySysConf.matchSystematic( CP::SystematicVariation("MUON_EFF_TrigSystUncertainty", 1)) && 
	 mySysConf.matchSystematic( CP::SystematicVariation("MUON_EFF_TrigSystUncertainty", -1)) )){
      return CP::SystematicCode::Unsupported;
    }

    m_appliedSystematics = &mySysConf;
    return CP::SystematicCode::Ok;
  }
  
} /* namespace CP */
