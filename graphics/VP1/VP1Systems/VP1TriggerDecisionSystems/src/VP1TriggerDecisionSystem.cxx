/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1TriggerDecisionSystems/VP1TriggerDecisionSystem.h"
#include <string>
#include <vector>

// TODO: check the include of the Chain.h file here below, maybe it's obsolete because it seems it's not used
//#include "TrigSteering/Chain.h" // OLD include
#include "TrigSteeringEvent/Chain.h" // NEW include - Requested by Trigger experts, 13 Sep 2013

#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/Lvl1Item.h"

class VP1TriggerDecisionSystem::Clockwork
{
public:
  Clockwork();
  ~Clockwork();

  ToolHandle<Trig::TrigDecisionTool>  m_trigDec;

  void getInfo(const std::string& key,const std::string&/*type*/,std::ostringstream& os);

  VP1TriggerDecisionSystem * theclass;
};

VP1TriggerDecisionSystem::Clockwork::Clockwork()
  : m_trigDec("TrigDec::TrigDecisionTool",0)
{
}

//____________________________________________________________________
VP1TriggerDecisionSystem::VP1TriggerDecisionSystem()
  : IVP1System("TriggerDecison",
	       "System for browsing the Trigger Decison [unmaintained]",
	       "Weiyi Zhang, wyizhang@cern.ch"), 
    m_clock(new Clockwork())
{
  m_clock->theclass=this;
}


//____________________________________________________________________
VP1TriggerDecisionSystem::~VP1TriggerDecisionSystem()
{
}


//____________________________________________________________________
void VP1TriggerDecisionSystem::create( StoreGateSvc* /*triggerDecisonTool*/ )
{
  StatusCode sc = m_clock->m_trigDec.retrieve();
  if(sc.isFailure())
    message("Error: retrieve TrigDecisionTool failed!");

  m_clock->m_trigDec->ExperimentalAndExpertMethods()->enable();
}

//____________________________________________________________________
void VP1TriggerDecisionSystem::refresh( StoreGateSvc* )
{
  QStringList entry_key;
  QStringList entry_type;
  QStringList entry_id;

  std::vector<std::string> l1triggers = m_clock->m_trigDec->getListOfTriggers("L1_.*");
  if(!l1triggers.empty()) {
    for (std::vector<std::string>::const_iterator aItemName = l1triggers.begin(); aItemName != l1triggers.end(); ++aItemName) {
      if((*aItemName!="") && (*aItemName!=" ")) {
	std::ostringstream os1; 	os1 << *aItemName; 	entry_key << os1.str().c_str();
	std::ostringstream os2; 	os2 << m_clock->m_trigDec->isPassed(*aItemName);  entry_type << os2.str().c_str();
      }
      updateGUI();
    }
    entriesChanged(entry_key,entry_type);
  }else{
    message("WARNING:: The TriggerDecision Tool is not configured well for Level 1.");
    message("WARNING:: You may need a configure file (usually 'LVL1Config.xml').");
    message("WARNING:: Please refer to 'vp1 -h' for further instructions.");
  }

  std::vector<std::string> l2triggers = m_clock->m_trigDec->getListOfTriggers("L2_.*");
  if(!l2triggers.empty()){
    //const std::vector<HLT::Chain*>* allChains = m_clock->m_trigDec->getConfiguredChains(TrigDec::L2);
    for (std::vector<std::string>::const_iterator aChainName = l2triggers.begin(); aChainName != l2triggers.end(); ++aChainName) {
      if((*aChainName!=" ") && (*aChainName!="")) {
	std::ostringstream os1;  os1 << *aChainName;   	entry_key << os1.str().c_str();
	std::ostringstream os2;  os2 << m_clock->m_trigDec->isPassed(*aChainName) ;  	entry_type << os2.str().c_str();
      }
      updateGUI();
    }
    entriesChanged(entry_key,entry_type);
  } else {
    message("WARNING:: The TriggerDecision Tool is not configured well for Level 2.");
    message("WARNING:: You may need a configure file (usually 'pureSteering_menu.xml').");
    message("WARNING:: Please refer to 'vp1 -h' for further instructions.");
  }

  std::vector<std::string> eftriggers = m_clock->m_trigDec->getListOfTriggers("EF_.*");
  if(!eftriggers.empty()) {
    for (std::vector<std::string>::const_iterator aChainName = eftriggers.begin(); aChainName != eftriggers.end(); ++aChainName) {
      if((*aChainName!=" ") && (*aChainName!="")) {
	std::ostringstream os1;  os1 << *aChainName;   	entry_key << os1.str().c_str();
	std::ostringstream os2;  os2 << m_clock->m_trigDec->isPassed(*aChainName);  	entry_type << os2.str().c_str();
      }
      updateGUI();
    }
    entriesChanged(entry_key,entry_type);
  } else {
    message("WARNING:: The TriggerDecision Tool is not configured well for Event Filter.");
    message("WARNING:: You may need a configure file (usually 'pureSteering_menu.xml').");
    message("WARNING:: Please refer to 'vp1 -h' for further instruction.");
  }
}

//____________________________________________________________________
void VP1TriggerDecisionSystem::erase()
{
  entriesChanged(QStringList(),QStringList());
}

//____________________________________________________________________

QStringList VP1TriggerDecisionSystem::getInfoForEntry(QString key,QString type)
{
  if(!m_clock->m_trigDec)
    return QStringList();

  std::ostringstream os;
  try {
    m_clock->getInfo(key.toStdString(),type.toStdString(),os);
  }
  catch (std::runtime_error e) {
    os  << "Exception thrown when getting info for key="<<key.toStdString()<<" of type="<<type.toStdString();
    os  << "Exception: " << e.what() << std::endl;
  }

  return QString(os.str().c_str()).split("\n");
}

//____________________________________________________________________
void VP1TriggerDecisionSystem::Clockwork::getInfo(const std::string& key,const std::string&/*type*/,std::ostringstream& os)
{
  StatusCode status;
  os << " Trigger Information: " << std::endl;

  // For LV1 ROI informations
  if(key.find("L1",0)!= std::string::npos) {
    os << "	LV1: " << key << std::endl;
    const LVL1CTP::Lvl1Item*  l1iterm = m_trigDec->ExperimentalAndExpertMethods()->getItemDetails(key);
    if(!l1iterm) {
      os << "	LV1 " << l1iterm->name() << " isPassed: " 	<< l1iterm->isPassed() << std::endl;
      os << "	LV1 " << l1iterm->name() << " isPrescaled: " 	<< l1iterm->isPrescaled()
	 << " : "  <<  l1iterm->prescaleFactor()   << std::endl;
      os << "	LV1 " << l1iterm->name() << " isVeto: " 		<< l1iterm->isVeto() << std::endl;
    }
  }

  if(key.find("L2",0)!= std::string::npos) {
    Trig::FeatureContainer fc = m_trigDec->features(key);
    std::vector<Trig::Feature<TrigRoiDescriptor> > roif = fc.get<TrigRoiDescriptor>();
    if(!roif.empty()) {
      os << "	LV2: " << 	key 	<< "   VecSize: " << roif.size() << std::endl;
      std::vector<Trig::Feature<TrigRoiDescriptor> >::iterator roiIt = roif.begin();
      for ( ;roiIt != roif.end(); ++roiIt ) {
	const TrigRoiDescriptor* roi = *roiIt;
	if (!roi) {
	  theclass->message("ERROR: get roi failed");
	  continue;
	}
	os << "  "<<  roiIt->label() << " ROI: "
	   <<  "(" << roi->eta() << ", " << roi->phi() << ") " << std::endl;;
      }
    }
    
  }

  // For EF ROI informations 
  if(key.find("EF",0)!= std::string::npos) {
    Trig::FeatureContainer fc = m_trigDec->features(key);
    std::vector<Trig::Feature<TrigRoiDescriptor> > roif = fc.get<TrigRoiDescriptor>();
    if(!roif.empty()) {
      os << "      EF: " << 	key 	<< "   VecSize: " << roif.size() << std::endl;
      
      std::vector<Trig::Feature<TrigRoiDescriptor> >::iterator roiIt = roif.begin();
      for ( ;roiIt != roif.end(); ++roiIt ) {
	const TrigRoiDescriptor* roi = *roiIt;
	if (!roi) {
	  theclass->message("ERROR: get roi failed");
	  continue;
	}
	
	os << "  "<<  roiIt->label() << " ROI: "
	   <<  "(" << roi->eta() << ", " << roi->phi() << ") " << std::endl;;
      }
    }
  }

}


