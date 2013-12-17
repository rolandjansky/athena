/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>
#include <sstream>

// Local
#include "TrigCostBase/Factory.h"
#include "TrigCostBase/UtilCore.h"

// Local
#include "TrigCostBase/UtilHack.h"
#include "TrigCostBase/UtilTrig.h"
#include "TrigCostBase/WriteTrigConfig.h"
#include "TrigCostBase/ConfigABC.h"

using namespace std;


//-----------------------------------------------------------------------------
void Trig::BunchGroup::PrintBunchGroup(ostream &os) const
{
  os << "Bunch group: key=" << key << " keynum=" << keynum << " size=" << setw(4) << size << ": " << name << endl;
}

//-----------------------------------------------------------------------------
Anp::ConfigABC::ConfigABC()
  :fDebug(false),
   fRemoveNoPS(false),
   fDefaultPS(0.0),
   fDefaultPT(0.0),
   fHasDefaultPS(false),
   fHasDefaultPT(false),
   fUseMenuGroups(true)
{
}

//-----------------------------------------------------------------------------
void Anp::ConfigABC::ReadConfig(const Registry &reg)
{
  //
  // Read PS data once
  //
  string ps_path, gr_path;
  
  if(reg.Get(AlgEvent::GetType(), "ReadXML_PS", ps_path)) {
    fDataPS = Trig::PSXMLparser   (ps_path, fDebug);
  }
  if(reg.Get(AlgEvent::GetType(), "ReadXML_Group", gr_path)) {
    fDataGr = Trig::GroupXMLparser(gr_path, fDebug);
  }
   
  reg.Get(AlgEvent::GetType(), "Debug",          fDebug);
  reg.Get(AlgEvent::GetType(), "WriteXML",       fWriteXML);  
  reg.Get(AlgEvent::GetType(), "UpdateAlg",      fUpdateName);
  reg.Get(AlgEvent::GetType(), "UseMenuGroups",  fUseMenuGroups);
  reg.Get(AlgEvent::GetType(), "RemoveNoPS",     fRemoveNoPS);

  fHasDefaultPS = reg.Get(AlgEvent::GetType(), "DefaultPS", fDefaultPS);
  fHasDefaultPT = reg.Get(AlgEvent::GetType(), "DefaultPT", fDefaultPT);

  //
  // Retreive different instance of ConfigABC algorithm
  //
  if(!fUpdateName.empty() && !fUpdateAlg.valid()) {
    fUpdateAlg = Factory<ConfigABC>::Instance().Get(fUpdateName);
  }
}

//-----------------------------------------------------------------------------
void Anp::ConfigABC::UpdateConfig(TrigMonConfig &config) const
{
  //
  // Update trigger configuration
  //

  vector<TrigConfChain> &chains = config.getVec<TrigConfChain>();
  set<uint32_t> ids;

  for(unsigned i = 0; i < chains.size(); ++i) {
    TrigConfChain &chain = chains.at(i);

    if(fHasDefaultPS) chain.setPrescale(fDefaultPS);
    if(fHasDefaultPT) chain.setPassThrough(fDefaultPT);
    
    if(!ids.insert(chain.getEncodedId()).second) {
      log() << "UpdateConfig - duplicate chain counter with: " << chain.getName() << endl;
    }
  }
  
  //
  // Update configuration using different ConfigABC instance
  //
  if(fUpdateAlg.valid()) {
    
    const TrigMonConfig &uconf = fUpdateAlg->GetConfig();
    map<string, Trig::PSData> psmap;

    for(unsigned i = 0; i < uconf.size<TrigConfChain>(); ++i) {
      const TrigConfChain &chain = uconf.at<TrigConfChain>(i);
      Trig::PSData data;

      data.counter = chain.getCounter();
      data.ps      = chain.getPrescale();
      data.pt      = chain.getPassThrough();
      data.es_ps   = Anp::GetExpressPS(chain);
      data.has_ps  = true;
      data.has_pt  = true;
      data.has_es  = !(data.es_ps < 1.0);
	
      psmap[chain.getName()] = data;
    }
    
    log() << "UpdateConfig - update config with ConfigABC instance: " << fUpdateAlg->GetName() << endl;

    Trig::UpdateChainPS(config, psmap);
  }

  //
  // Update configuration from XML
  //
  if(!fDataPS.empty()) {
    log() << "UpdateConfig - apply PS from XML" << endl;

    Trig::UpdateChainPS(config, fDataPS);

    if(fRemoveNoPS) {
      std::vector<TrigConfChain> &chains = config.getVec<TrigConfChain>();
      std::vector<TrigConfChain>::iterator cit = chains.begin();

      while(cit != chains.end()) {
	if(fDataPS.find(cit->getName()) == fDataPS.end()) {
	  cit = chains.erase(cit);
	}
	else {
	  cit++;
	}
      }
    }
  }

  //
  // Remove all menu defined groups
  //
  if(!fUseMenuGroups) {
    for(unsigned i = 0; i < chains.size(); ++i) {
      Anp::RemoveChainGroups(chains.at(i));
    }
  }

  //
  // Add groups from XML
  //
  if(!fDataGr.empty()) {
    log() << "UpdateConfig - add groups from XML" << endl;
    
    UpdateGroups(config, fDataGr, fDebug);
  }

  //
  // Write final configuration to XML file
  //
  if(!fWriteXML.empty()) {

    string wpath = fWriteXML;
    if     (wpath.at(wpath.size()-1) == '.') wpath += "/";
    else if(wpath.at(wpath.size()-1) != '/') wpath += "_";

    stringstream path_lv1, path_hlt;  
    
    path_lv1 << wpath << AlgEvent::GetName() << "_LV1_PS.xml";
    path_hlt << wpath << AlgEvent::GetName() << "_HLT_PS.xml";
    
    log () << "Set - writing XML files: " << endl
	   << "   " << path_lv1.str() << endl
	   << "   " << path_hlt.str() << endl;
    
    Registry xreg;
    xreg.Set("XmlPathLV1", path_lv1.str());
    xreg.Set("XmlPathHLT", path_hlt.str());
    
    WriteTrigConfig wtc;
    wtc.Config(xreg);
    wtc.Write(chains);
  }
}


//-----------------------------------------------------------------------------
std::set<std::string> Anp::ConfigABC::GetItemThr(unsigned ctpid) const
{
  //
  // Find list of thresholds for this L1 item
  //
  const ItemThrMap::const_iterator it = fItemThr.find(ctpid);
  if(it != fItemThr.end()) {
    return it->second;
  }
  
  return set<std::string>();
}

//-----------------------------------------------------------------------------
void Anp::ConfigABC::ReadItemTh(const TrigMonConfig &config)
{
  //
  // Build map from L1 CTP ID to L1 BG key
  //
  fItemThr.clear();

  for(unsigned i = 0; i < config.size<TrigConfChain>(); ++i) {
    const TrigConfChain &c = config.at<TrigConfChain>(i);

    if(c.getLevelId() != 1) {
      continue;
    }

    //
    // Fill L1 threshold information
    //
    set<string> bgset;

    const std::vector<TrigConfSig> &sig_list = c.getSignature();
    
    for(unsigned int j = 0; j < sig_list.size(); ++j) {
      const std::vector<uint32_t> &tes = sig_list.at(j).getOutputTEs();
      
      for(unsigned t = 0; t < tes.size(); ++t) {
	//
	// Find L1 threshold config
	//
	const vector<TrigConfSeq>::const_iterator iterSeq = config.findSeq(tes.at(t));
	if(iterSeq == config.end<TrigConfSeq>()) {
	  continue;
	}	
	
	bgset.insert(iterSeq->getName());
      }
    }
    
    fItemThr[c.getCounter()] = bgset;
  }
}
