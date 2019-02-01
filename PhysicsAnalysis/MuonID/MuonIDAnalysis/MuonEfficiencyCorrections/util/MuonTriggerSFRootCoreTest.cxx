/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

// Script for testing MuonTriggerScaleFactors. For example run with following command: MuonTriggerSFRootCoreTest -x AOD.11499205._000007.pool.root.1 -y 2015 -mc mc15a -t HLT_mu24 -r 278727

// System include(s):
#include <string>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TStopwatch.h>
#include <TString.h>

// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#   include "xAODRootAccess/tools/ReturnCheck.h"
#   include "AsgTools/Check.h"
#endif // ROOTCORE

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"

// Local include(s):
#include "MuonEfficiencyCorrections/MuonTriggerScaleFactors.h"
#include "PATInterfaces/ISystematicsTool.h"
#include "PATInterfaces/SystematicsUtil.h"






#define CHECK_CPSys(Arg) \
    if (Arg.code() == CP::SystematicCode::Unsupported){    \
        Warning(#Arg,"Unsupported systematic (in line %i) ",__LINE__); \
    }      

static const std::vector<std::string> qualities { "HighPt", "Tight", "Medium", "Loose", "LowPt" };
static const std::vector<std::string> binnings { "coarse", "fine" };
static const std::vector<std::string> types { "data", "mc" };

// Copied from MuonTriggerScaleFactors.cxx
CP::CorrectionCode getThreshold(Int_t& threshold, const std::string& trigger) {
    std::size_t index = trigger.find("HLT_mu");
    if (index != std::string::npos) {
        std::string rawNumber = trigger.substr(index + 6);
        if (!rawNumber.empty() && isdigit(rawNumber[0])) {
            std::stringstream(rawNumber) >> threshold;
            if (threshold < 10) threshold = 10000;
            else threshold = (threshold + 1) * 1000;
            return CP::CorrectionCode::Ok;
        }
    }
    return CP::CorrectionCode::Error;
}

class MuonTriggerSFTester{

public:
  MuonTriggerSFTester(const char* appName, const std::string& trigger){
    m_systematics = CP::make_systematics_vector(CP::SystematicRegistry::getInstance().recommendedSystematics());
    m_appName = appName;
    m_errorsCount = 0;
    m_warningsCount = 0;
    m_trigger = trigger;

  }

  int initialiseTools(const std::string& customFileName, const std::string& customInputFolder, int year, const std::string& period){
    for (size_t i = 0; i < qualities.size(); i++) {
      std::vector<CP::MuonTriggerScaleFactors*> tools{};
      for (size_t j = 0; j < binnings.size(); j++) {
	CP::MuonTriggerScaleFactors* tool = new CP::MuonTriggerScaleFactors("TrigSF_" + qualities[i] + "_" + binnings[j]);
	ASG_CHECK_SA(m_appName, tool->setProperty("MuonQuality", qualities[i]));
	ASG_CHECK_SA(m_appName, tool->setProperty("Binning", binnings[j]));
	ASG_CHECK_SA(m_appName, tool->setProperty("filename", customFileName));
	ASG_CHECK_SA(m_appName, tool->setProperty("CustomInputFolder", customInputFolder));
	ASG_CHECK_SA(m_appName, tool->setProperty("UseExperimental", true));
	ASG_CHECK_SA(m_appName, tool->setProperty("AllowZeroSF", true));
	ASG_CHECK_SA(m_appName, tool->setProperty("forceYear", year));
	ASG_CHECK_SA(m_appName, tool->setProperty("forcePeriod", period));

	tool->msg().setLevel( MSG::WARNING );
	ASG_CHECK_SA(m_appName, tool->initialize());
	//CP::CorrectionCode result = tool->setRunNumber(atoi(runNumber));
	//if (result != CP::CorrectionCode::Ok){
	//    Error(m_appName, "Could not set run number");
	//    return 1;
	//}
	tools.push_back(tool);
      }
      m_triggerSFTools.push_back(tools);
    }
    return 0;
  }

  int processEvent(xAOD::TEvent& event, int year, const std::string& period){
    const xAOD::EventInfo* ei = nullptr;
    RETURN_CHECK(m_appName, event.retrieve(ei, "EventInfo"));
    
    const xAOD::MuonContainer* muons = nullptr;
    RETURN_CHECK(m_appName, event.retrieve(muons, "Muons"));
    if(year == -1 && period == "")
      static const SG::AuxElement::ConstAccessor<unsigned int> acc_rnd("RandomRunNumber");
    for (size_t i = 0; i < qualities.size(); i++) {
      for (size_t j = 0; j < binnings.size(); j++) {        
	for (size_t k = 0; k < m_systematics.size(); k++) {
	  if(m_trigger == "HLT_mu26_ivarmedium" || m_trigger == "HLT_mu50" || m_trigger == "HLT_mu26_ivarmedium_OR_HLT_mu50")
	    checkSingleMuonTrigger(ei, muons, i, j, k);
	  if(muons->size() != 2)
	    continue;
	  if(binnings[j] == "coarse")
	    continue; // not supported right now
	  if(m_trigger == "HLT_2mu14" || m_trigger == "HLT_2mu10"){
	    checkSymDiMuonTrigger(ei, muons, i, j, k);
	  }
	  else{
	    for(auto muon : *muons)
	      checkASymDiMuonTrigger(ei, *muon, i, j, k);
	  }
	}
      }
    }
    return 0;
  }

    int checkASymDiMuonTrigger(const xAOD::EventInfo* ei, const xAOD::Muon& muon, unsigned int iquality, unsigned int ibin, unsigned int isystematic){
    CHECK_CPSys(m_triggerSFTools[iquality][ibin]->applySystematicVariation(m_systematics[isystematic]));

    Int_t threshold = 0;
    CP::CorrectionCode result = getThreshold(threshold, m_trigger);
    if (result != CP::CorrectionCode::Ok) {
      Error("MuonTriggerSFRootCoreTest", "Could not extract threshold for trigger %s", m_trigger.c_str());
      return 1;
    }
    if (muon.pt() < threshold or fabs(muon.eta()) > 2.5)
      return 1;

    bool displayWarning = false;


    
    double efficiencyMC = 0.;
    double efficiencyData = 0.;

    result = m_triggerSFTools[iquality][ibin]->getTriggerEfficiency(muon,
								    efficiencyMC,
								    m_trigger,
								    false);
    if (result != CP::CorrectionCode::Ok) {
      Error(m_appName, "Could not retrieve MC efficeincy. Parameters:\n        Event number = %i,\n        Quality = %s,\n        Binning = %s,\n        Systematic = %s", static_cast<int>(ei->eventNumber()), qualities[iquality].c_str(), binnings[ibin].c_str(), m_systematics[isystematic].name().c_str());
      m_errorsCount++;
    }

    result = m_triggerSFTools[iquality][ibin]->getTriggerEfficiency(muon,
								    efficiencyData,
								    m_trigger,
								    true);
    if (result != CP::CorrectionCode::Ok) {
      Error(m_appName, "Could not retrieve data efficeincy. Parameters:\n        Event number = %i,\n        Quality = %s,\n        Binning = %s,\n        Systematic = %s", static_cast<int>(ei->eventNumber()), qualities[iquality].c_str(), binnings[ibin].c_str(), m_systematics[isystematic].name().c_str());
      m_errorsCount++;
    }
    auto triggerSF = efficiencyMC/efficiencyData;
    if (triggerSF < 0.2 || triggerSF > 1.2) {
      if (displayWarning) {
	Warning(m_appName, "Retrieved trigger scale factor %.3f is outside of expected range from 0.2 to 1.2. Parameters:\n        Event number = %i,\n        Quality = %s,\n        Binning = %s,\n        Systematic = %s", triggerSF, static_cast<int>(ei->eventNumber()), qualities[iquality].c_str(), binnings[ibin].c_str(), m_systematics[isystematic].name().c_str());
	m_warningsCount++;
      }
    }
    return 0;
  }

  int checkSymDiMuonTrigger(const xAOD::EventInfo* ei, const xAOD::MuonContainer* muons, unsigned int iquality, unsigned int ibin, unsigned int isystematic){
    CHECK_CPSys(m_triggerSFTools[iquality][ibin]->applySystematicVariation(m_systematics[isystematic]));
    double triggerSF = 0.;
    CP::CorrectionCode result;
    result = m_triggerSFTools[iquality][ibin]->getTriggerScaleFactor(*muons, triggerSF, m_trigger);
    if (result != CP::CorrectionCode::Ok) {
      Error(m_appName, "Could not retrieve trigger scale factors. Parameters:\n        Event number = %i,\n        Quality = %s,\n        Binning = %s,\n        Systematic = %s", static_cast<int>(ei->eventNumber()), qualities[iquality].c_str(), binnings[ibin].c_str(), m_systematics[isystematic].name().c_str());
      m_errorsCount++;
    }
    if (triggerSF < 0.2 || triggerSF > 1.2) {
      // Allow scale factor to be outside of this range in case all the muons are below the threshold
      Int_t threshold = 0;
      CP::CorrectionCode result = getThreshold(threshold, m_trigger);
      if (result != CP::CorrectionCode::Ok) {
	Error("MuonTriggerSFRootCoreTest", "Could not extract threshold for trigger %s", m_trigger.c_str());
	return 1;
      }
      bool displayWarning = false;
      xAOD::MuonContainer::const_iterator mu_end = muons->end();
      for (xAOD::MuonContainer::const_iterator mu_itr = muons->begin(); mu_itr != mu_end; ++mu_itr) {
	if ((**mu_itr).pt() >= threshold and fabs((**mu_itr).eta()) < 2.5) displayWarning = true;
      }
                        
      if (displayWarning) {
	Warning(m_appName, "Retrieved trigger scale factor %.3f is outside of expected range from 0.2 to 1.2. Parameters:\n        Event number = %i,\n        Quality = %s,\n        Binning = %s,\n        Systematic = %s", triggerSF, static_cast<int>(ei->eventNumber()), qualities[iquality].c_str(), binnings[ibin].c_str(), m_systematics[isystematic].name().c_str());
	m_warningsCount++;
      }
    }

    return 0;
  }

  
  int checkSingleMuonTrigger(const xAOD::EventInfo* ei, const xAOD::MuonContainer* muons, unsigned int iquality, unsigned int ibin, unsigned int isystematic){
    	  
    CHECK_CPSys(m_triggerSFTools[iquality][ibin]->applySystematicVariation(m_systematics[isystematic]));
    CP::CorrectionCode result;
    xAOD::MuonContainer::const_iterator mu_end = muons->end();
    for (xAOD::MuonContainer::const_iterator mu_itr = muons->begin(); mu_itr != mu_end; ++mu_itr) {
      for (size_t l = 0; l < types.size(); l++) {
	if (types[l] != "data" || m_systematics[isystematic].name().find("TrigSystUncertainty") == std::string::npos) {
	  Double_t eff;
	  auto binNumber = m_triggerSFTools[iquality][ibin]->getBinNumber(**mu_itr, m_trigger);
	  result = m_triggerSFTools[iquality][ibin]->getTriggerEfficiency(**mu_itr, eff, m_trigger, types[l] == "data");
	  if (result != CP::CorrectionCode::Ok) {
	    Error(m_appName, "Could not retrieve trigger efficiency. Paramaters:\n        Event number = %i,\n        Quality = %s,\n        Binning = %s,\n        Systematic = %s,\n        Type = %s ", static_cast<int>(ei->eventNumber()), qualities[iquality].c_str(), binnings[ibin].c_str(), m_systematics[isystematic].name().c_str(), types[l].c_str());
	    m_errorsCount++;
	  }
	  if (eff < 0 || eff > 1) {
	    Warning(m_appName, "Retrieved trigger efficiency %.3f is outside of expected range from 0 to 1. Paramaters:\n        Event number = %i,\n        Quality = %s,\n        Binning = %s,\n        Systematic = %s", eff, static_cast<int>(ei->eventNumber()), qualities[iquality].c_str(), binnings[ibin].c_str(), m_systematics[isystematic].name().c_str());
	    m_warningsCount++;
	  }
	  if (binNumber < 0 || binNumber > 238) {
	    Warning(m_appName, "Retrieved bin number %.i is outside of expected range from 0 to 238. Paramaters:\n        Event number = %i,\n        Quality = %s,\n        Binning = %s,\n        Systematic = %s", binNumber, static_cast<int>(ei->eventNumber()), qualities[iquality].c_str(), binnings[ibin].c_str(), m_systematics[isystematic].name().c_str());
	    m_warningsCount++;
	  }
				
	}
      }
    }

    double triggerSF = 0.;
    result = m_triggerSFTools[iquality][ibin]->getTriggerScaleFactor(*muons, triggerSF, m_trigger);
    if (result != CP::CorrectionCode::Ok) {
      Error(m_appName, "Could not retrieve trigger scale factors. Parameters:\n        Event number = %i,\n        Quality = %s,\n        Binning = %s,\n        Systematic = %s", static_cast<int>(ei->eventNumber()), qualities[iquality].c_str(), binnings[ibin].c_str(), m_systematics[isystematic].name().c_str());
      m_errorsCount++;
    }
    if (triggerSF < 0.2 || triggerSF > 1.2) {
      // Allow scale factor to be outside of this range in case all the muons are below the threshold
      Int_t threshold = 0;
      CP::CorrectionCode result = getThreshold(threshold, m_trigger);
      if (result != CP::CorrectionCode::Ok) {
	Error("MuonTriggerSFRootCoreTest", "Could not extract threshold for trigger %s", m_trigger.c_str());
	return 1;
      }
      bool displayWarning = false;
      xAOD::MuonContainer::const_iterator mu_end = muons->end();
      for (xAOD::MuonContainer::const_iterator mu_itr = muons->begin(); mu_itr != mu_end; ++mu_itr) {
	if ((**mu_itr).pt() >= threshold and fabs((**mu_itr).eta()) < 2.5) displayWarning = true;
      }
                        
      if (displayWarning) {
	Warning(m_appName, "Retrieved trigger scale factor %.3f is outside of expected range from 0.2 to 1.2. Parameters:\n        Event number = %i,\n        Quality = %s,\n        Binning = %s,\n        Systematic = %s", triggerSF, static_cast<int>(ei->eventNumber()), qualities[iquality].c_str(), binnings[ibin].c_str(), m_systematics[isystematic].name().c_str());
	m_warningsCount++;
      }
    }
    double tmpEffData = 1.;
    double tmpEffMC = 1.;
    for (auto muon: *muons) {
      double effData = 0.;
      double effMC = 0.;
      double scaleFactor = 0.;
      if (m_triggerSFTools[iquality][ibin]->getTriggerEfficiency(*muon, effData, m_trigger, true) != CP::CorrectionCode::Ok) {
	Error("MuonTriggerSFRootCoreTest", "Could not extract data trigger efficiency for %s which is %f. Parameters:\n        Event number = %i,\n        Quality = %s,\n        Binning = %s,\n        Systematic = %s \n muon pt: %f  \n eta: %f \n phi: %f", m_trigger.c_str(), scaleFactor, static_cast<int>(ei->eventNumber()), qualities[iquality].c_str(), binnings[ibin].c_str(), m_systematics[isystematic].name().c_str(), muon->pt(), muon->eta(), muon->phi());
	return 1;
      }
      if (m_triggerSFTools[iquality][ibin]->getTriggerEfficiency(*muon, effMC, m_trigger, false) != CP::CorrectionCode::Ok) {
	Error("MuonTriggerSFRootCoreTest", "Could not extract MC trigger efficiency for %s", m_trigger.c_str());
	return 1;
      }
      if (m_triggerSFTools[iquality][ibin]->getTriggerScaleFactor(*muon, scaleFactor, m_trigger) != CP::CorrectionCode::Ok) {
	Error("MuonTriggerSFRootCoreTest", "Could not extract MC trigger efficiency for %s", m_trigger.c_str());
	return 1;
      }
      if (scaleFactor < 0.2 || scaleFactor > 1.2 ) {
	Int_t threshold = 0;
	if (getThreshold(threshold, m_trigger) != CP::CorrectionCode::Ok) {
	  Error("MuonTriggerSFRootCoreTest", "Could not extract threshold for trigger %s", m_trigger.c_str());
	  return 1;
	}
	if(muon->pt() >= threshold and fabs(muon->eta()) < 2.5)
	  Warning(m_appName, "Retrieved single muon trigger scale factor %.3f is outside of expected range from 0.2 to 1.2. Parameters:\n        Event number = %i,\n        Quality = %s,\n        Binning = %s,\n        Systematic = %s \n muon pt: %f  \n  eta: %f \n phi: %f", scaleFactor, static_cast<int>(ei->eventNumber()), qualities[iquality].c_str(), binnings[ibin].c_str(), m_systematics[isystematic].name().c_str(), muon->pt(), muon->eta(), muon->phi());
      }
      tmpEffData *= 1.-effData;
      tmpEffMC *= 1.-effMC;
    }
    auto sfSingleCalc = (1. - tmpEffData)/(1. - tmpEffMC);
    if ( triggerSF > 0.2 && (sfSingleCalc - triggerSF) / triggerSF > 0.02) {

      // Warning(m_appName, "Invalid single muon SF result. Parameters:\n        Event number = %i,\n        Quality = %s,\n        Binning = %s \n Systematic = %s \n single SF: %f \n combined SF %f \n ", static_cast<int>(ei->eventNumber()), qualities[iquality].c_str(), binnings[ibin].c_str(), systematics[isystematic].name().c_str(), sfSingleCalc, triggerSFrSF);
      m_warningsCount++;
    }
    return 0;
  }


public:
  int m_errorsCount = 0;
  int m_warningsCount = 0;

private:
  std::vector<std::vector<CP::MuonTriggerScaleFactors*>> m_triggerSFTools;
  std::vector<CP::SystematicSet> m_systematics;
  const char* m_appName;
  std::string m_trigger;

};


int main(int argc, char* argv[]) {

    const char* APP_NAME = argv[0];
    CP::CorrectionCode::enableFailure();
    // Read the config provided by the user
    const char* xAODFileName = "";
    std::string customInputFolder = "";
    std::string customFileName = "";
    const char* nrOfEntries = "";
    std::string trigger = "";
    int year = -1;
    std::string period = "";
    for (int i = 1; i < argc - 1; i++) {
        std::string arg = std::string(argv[i]);
        if (arg == "-x") {
            xAODFileName = argv[i + 1];
        }
        else if (arg == "-d")
            customInputFolder = argv[i + 1];
        else if (arg == "-f")
            customFileName = argv[i + 1];
        else if (arg == "-e")
            nrOfEntries = argv[i + 1];
        else if (arg == "-t")
            trigger = argv[i + 1];
	else if (arg == "-y")
	  year = atoi(argv[i + 1]);
	else if (arg == "-p")
            period = argv[i + 1];
    }
    
    bool error = false;
    if (xAODFileName[0] == '\0') {
        Error(APP_NAME, "xAOD file name missing!");
        error = true;
    }
    if (trigger == "") {
        Error(APP_NAME, "trigger missing!");
        error = true;
    }
    if (error) {
        Error(APP_NAME, "  Usage: %s -x [xAOD file name] -y [year] -p [period] -t [trigger] -d [custom input folder] -f [custom file name] -e [number of events to process]", APP_NAME);
        return 1;
    }

    RETURN_CHECK(APP_NAME, xAOD::Init(APP_NAME));

    Info(APP_NAME, "Opening file: %s", xAODFileName);
    std::auto_ptr<TFile> ifile(TFile::Open(xAODFileName, "READ"));
    if (!ifile.get()) {
        Error(APP_NAME, " Unable to load xAOD input file");
        return 1;
    }

    xAOD::TEvent event;
    RETURN_CHECK(APP_NAME, event.readFrom(ifile.get(), xAOD::TEvent::kClassAccess));
    Info(APP_NAME, "Number of events in the file: %i",  static_cast<int>(event.getEntries()));

    // Decide how many events to run over:
    int nrOfEntriesToRunOver = 1000;//event.getEntries();
    if (!std::string(nrOfEntries).empty()) {
        int e = atoll(nrOfEntries);
        if (e < nrOfEntriesToRunOver) {
            nrOfEntriesToRunOver = e;
        }
    }

    
    MuonTriggerSFTester sfChecker(APP_NAME, trigger);
    sfChecker.initialiseTools(customFileName, customInputFolder, year, period);
    for(Long64_t entry = 0; entry < nrOfEntriesToRunOver; entry++) {
        // Tell the object which entry to look at:
        event.getEntry(entry);
	sfChecker.processEvent(event, year, period);

    }
	
    Info(APP_NAME, "%i events successfully processed, %i warnings, %i errors detected.", nrOfEntriesToRunOver, sfChecker.m_warningsCount, sfChecker.m_errorsCount);
    return 0;
}

