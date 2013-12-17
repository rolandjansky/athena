/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <bitset>
#include <cstring>
#include <sstream>
#include <string>

// ROOT
#include "TCanvas.h"
#include "TImageDump.h"
#include "TH1.h"
#include "TH2.h"
#include "TList.h"

// Local
#include "TrigCostBase/HistMan.h"
#include "TrigCostBase/UtilCore.h"
#include "TrigCostBase/UtilHack.h"
#include "TrigCostBase/UtilTrig.h"

using namespace std;

//-----------------------------------------------------------------------------
int Trig::Prune(std::vector<TrigConfChain> &pvec, std::vector<TrigConfChain> &cvec)
{
  //
  // Recusrively search for all chains which feed this chain
  //
  int icount = 0;

  // Walk through ignored vector
  std::vector<TrigConfChain>::iterator pit = pvec.begin();
  while(pit != pvec.end()) {
    // Check level
    if(pit -> getLevelId() == 1) {
      ++pit;
      continue;
    }

    // Find input chain
    const string &lname = pit->getLowerName();
    std::vector<TrigConfChain>::iterator lit = std::find(cvec.begin(), cvec.end(), lname);
    
    if(lit == cvec.end()) {
      // This chain matches nothing
      ++pit;      
    } else {
      // This chain matches: copy, remove and start over
      pvec.push_back(*lit);
      cvec.erase(lit);
      pit = pvec.begin();
      ++icount;
    }
  }

  // Walk through configured vector
  std::vector<TrigConfChain>::iterator cit = cvec.begin();
  while(cit != cvec.end()) {
    // Check level
    if(cit -> getLevelId() == 1) {
      ++cit;
      continue;
    }

    // Find input chain
    const string &lname = cit->getLowerName();
    std::vector<TrigConfChain>::iterator lit = std::find(pvec.begin(), pvec.end(), lname);

    if(lit == pvec.end()) {
      // This chain matches nothing
      ++cit;
    } else {
      // This chain matches: copy, remove and start over
      pvec.push_back(*cit);
      cvec.erase(cit);
      cit = cvec.begin();
      ++icount;
    }
  }

  if(icount == 0) {
    return 0;
  }

  // Make recursive call to make sure that we got everything  
  return icount + Trig::Prune(pvec, cvec);
}

//-----------------------------------------------------------------------------
std::string Trig::GetLevel(unsigned int level)
{
  // get trigger level as integer
  if     (level == 1) return "L1";
  else if(level == 2) return "L2";
  else if(level == 3) return "EF";
  
  cerr << "Trig::getLevel - unknown level: " << level << endl;
  return "";
}

//-----------------------------------------------------------------------------
std::string Trig::CombinedName(const std::vector<std::string> &cvec)
{
  //
  // Make name for combined trigger
  //  
  if(cvec.empty()) return "";
  else if(cvec.size() == 1) return cvec.front();
  
  // Find trigger level
  string level = cvec.front().substr(0, 2);
  if(level != "L1" && level != "L2" && level != "EF") { 
    cerr << "CombinedName - bad trigger level" << endl;
    return "";   
  }
  
  string cname = level;
  for (unsigned int i = 0; i < cvec.size(); ++i) {
    const string &iname = cvec[i];
    
    // Check level again
    if(iname.substr(0, 3) != level + "_") {
      cerr << "CombinedName - trigger level mismatch" << endl;
      return "";
    }
    
    cname = cname + "_" + iname.substr(3);
  }

  return cname;
}

//-----------------------------------------------------------------------------
void Trig::PrintVec(const std::vector<TrigConfChain> &cvec, const std::string &option) 
{
  //
  // Print chain configuration(s)
  //
  unsigned int iwidth = 0;
  for(std::vector<TrigConfChain>::const_iterator it = cvec.begin(); it != cvec.end(); ++it) {
   iwidth = std::max<unsigned>(iwidth, it -> getName().size());
  }

  for(std::vector<TrigConfChain>::const_iterator it = cvec.begin(); it != cvec.end(); ++it) {
    cout << std::setfill(' ') << std::left << std::setw(iwidth) << it -> getName();

    if(option.find("PS") != string::npos) cout << " PS=" << it -> getPrescale();
    if(option.find("PT") != string::npos) cout << " PT=" << it -> getPassThrough();
    
    if(option.find("newline") == string::npos) {
      if(it + 1 != cvec.end()) cout << ", ";
      else                     cout << endl;
    }
    else {
      cout << endl;
    }
  }
}

//-----------------------------------------------------------------------------
bool Trig::MakeLowerVecForPT(const std::vector<TrigConfChain> &cvec,
			     const std::vector<TrigConfChain> &pvec,
			     std::vector<TrigConfChain> &ovec)
{
  //
  // Find input chains for pass-through chains
  //
  // cvec = all configured chains, 
  // pvec = pass (selected) chains for this chain or group
  // ovec = output chains at previous level
  //
  bool no_error = true;

  for(unsigned int i = 0; i < pvec.size(); ++i) {
    for(unsigned int j = i+1; j < pvec.size(); ++j) {
      if(pvec[i].getName() == pvec[j].getName()) {
	cerr << "MakeLowerVecForPT - []: duplicate chains: (" << i << ", " << j << ")" 
	     << pvec[i].getName() << "==" << pvec[j].getName() << endl;
	no_error = false;
      }
    }
  }

  for(std::vector<TrigConfChain>::const_iterator it = pvec.begin(); it != pvec.end(); ++it) {
    // Use seeded chains with non-zero pass-through factor
    if(it -> getPassThrough() > 0.0 && !(it->getLowerName().empty())) { 
      const std::vector<TrigConfChain>::const_iterator pit = std::find(cvec.begin(),
								       cvec.end(), 
								       it->getLowerName());
      if(pit == cvec.end()) {
	cerr << "MakeLowerVecForPT - failed to find input chain for: " << it->getName() << endl;
	continue;
      }
      
      if(pit -> getLevelId() + 1 != it -> getLevelId()) {
	cerr << "MakeLowerVecForPT - logic error for levels: " 
	     << pit -> getLevelId() << " + 1 != " << it -> getLevelId() << endl;
	continue;
      }

      // Check that chain does not have zero prescale
      if(!(pit->getPrescale() > 0.0)) {
	cerr << "MakeLowerVecForPT - bad prescale for previous chain: " << pit->getPrescale() << endl;
	continue;
      }
      // Copy previous chain and increase its prescale by PT factor
      TrigConfChain chain = *pit;
      chain.setPrescale(chain.getPrescale()*it->getPassThrough());

      // Save chain
      if(std::find(ovec.begin(), ovec.end(), chain) != ovec.end()) {
	no_error = true;
	cerr << "MakeLowerVecForPT - chain already exists: " << it->getName() << endl;
      }
      
      ovec.push_back(chain);
    }
    else if(it->getLowerName().empty()) {
      // Unseeded case: copy previous chain and multiply PS by PT 
      for(std::vector<TrigConfChain>::const_iterator cit = cvec.begin(); cit != cvec.end(); ++cit) {
	if(cit->getLevelId()+1 != it->getLevelId()) continue;
	
	TrigConfChain chain = *cit;
	chain.setPrescale(chain.getPrescale()*it->getPassThrough());
	
	// Save chain
	if(std::find(ovec.begin(), ovec.end(), chain) != ovec.end()) {
	  no_error = true;
	  cerr << "MakeLowerVecForPT - chain already exists: " << it->getName() << endl;
	}
      }
    }
  }

  if(!no_error) {
    cerr << "MakeLowerVecForPT - printing " << pvec.size() << " chain(s)" << endl;
    for(std::vector<TrigConfChain>::const_iterator it = pvec.begin(); it != pvec.end(); ++it) {
      cerr << it->getName() << " ";
    }
    cerr << endl;
  }

  return no_error;
}

//-----------------------------------------------------------------------------
void Trig::MakeGroupCompliment(const std::vector<TrigConfChain> &confg_vec,
			       const std::vector<TrigConfChain> &group_vec,
			       std::vector<TrigConfChain> &result_vec)
{
  //
  // Find input chains for pass-through chains
  //
  // confg_vec  = all configured chains, 
  // group_vec  = pass (selected) chains for this chain or group
  // result_vec = output chains at previous level
  //

  // Determine trigger element
  unsigned int level = 0;
  for(std::vector<TrigConfChain>::const_iterator it = group_vec.begin(); it != group_vec.end(); ++it) {
    // Look for this chain in group vector
    if(level == 0) { 
      level = it->getLevelId();
    }
    else if(level != it->getLevelId()) {
      cerr << "MakeGroupCompliment - trigger level mismatch within group" << endl;
      return;
    }
  }
  
  for(std::vector<TrigConfChain>::const_iterator cit = confg_vec.begin(); 
      cit != confg_vec.end(); ++cit) {
    // Look for this chain in group vector
    const TrigConfChain &chain = *cit;
    if(chain.getLevelId() != level) {
      continue;
    }

    const std::vector<TrigConfChain>::const_iterator git = std::find(group_vec.begin(),
								     group_vec.end(), chain);
    if(git == group_vec.end()) {
      // Save chain
      result_vec.push_back(chain);
    }
  }
}

//-----------------------------------------------------------------------------
void Trig::WidthPad(const std::vector<TrigConfChain> &cvec, std::vector<std::string> &nvec)
{
  //
  // Convert chain vector to vector of equal width names
  //
  unsigned int iwidth = 0;
  for(std::vector<TrigConfChain>::const_iterator it = cvec.begin(); it != cvec.end(); ++it) {
    iwidth = std::max<unsigned int>(iwidth, it->getName().size());
  }

  for(std::vector<TrigConfChain>::const_iterator it = cvec.begin(); it != cvec.end(); ++it) {
    nvec.push_back(Anp::PadStrBack(it->getName(), iwidth));
  }  
}

//-----------------------------------------------------------------------------
void Trig::PrintColumns(vector<string> svec,
			ostream &os,
			const int ncolumn,
			Anp::Handle<Anp::HtmlNode> pnode)
{
  //
  // Print string vector in columns
  //
  if(svec.empty()) return;

  //
  // Sort strings
  //
  std::sort(svec.begin(), svec.end());

  //
  // First pass, find widest name
  //
  int icount = 0;
  map<unsigned, unsigned> col_map;
  
  for(unsigned int i = 0; i < svec.size(); ++i) {
    col_map.insert(pair<unsigned, unsigned>(icount, 0));    
    col_map[icount] = std::max<unsigned>(col_map[icount], svec[i].size());
    
    if(++icount == ncolumn) icount = 0;
  }
  
  //
  // Second pass, print values, use links if available
  //
  icount = 0;

  for(unsigned i = 0; i < svec.size(); ++i) {
    string sname = svec[i];

    //
    // Find link to this name
    // 
    if(pnode.valid()) {
      Anp::Handle<Anp::HtmlNode> snode = pnode -> FindChild(sname);
      if(snode.valid()) { 
	sname = snode->GetLink(Anp::PadStrBack(sname, col_map[icount]));
      }
      else {
	sname = Anp::PadStrBack(sname, col_map[icount]);
      }
    }
    else {
      sname = Anp::PadStrBack(sname, col_map[icount]);
    }

    os << sname << " ";
    
    if(++icount == ncolumn) {
      icount = 0;
      os << endl;
    }
  }
  
  if(icount != 0) {
    os << endl;
  }
}

//-----------------------------------------------------------------------------
std::vector<TrigConfSeq> Trig::GetSeq(const TrigConfChain &chain,
				      const TrigMonConfig &config)
{
  return Trig::GetSeq(chain, const_cast<TrigMonConfig &>(config).getVec<TrigConfSeq>());
}

//-----------------------------------------------------------------------------
std::vector<TrigConfSeq> Trig::GetSeq(const std::vector<TrigConfChain> &chains,
				      const std::vector<TrigConfSeq> &seq_vec)
{
  //
  // Collect sequences for chains
  //
  std::vector<TrigConfSeq> seq;

  for(unsigned int i = 0; i < chains.size(); ++i) {
    const std::vector<TrigConfSeq> seq_one = Trig::GetSeq(chains[i], seq_vec); 
    seq.insert(seq.end(), seq_one.begin(), seq_one.end());
  }

  return seq;
}

//-----------------------------------------------------------------------------
std::vector<TrigConfSeq> Trig::GetSeq(const TrigConfChain &chain,
				      const std::vector<TrigConfSeq> &seq_vec)
{
  //
  // Find all matching sequences for a chain without multiplicities
  // 
  std::vector<TrigConfSeq> seq;
  const vector<TrigConfSig> &sig_vec = chain.getSignature();

  // Iterator over signatures
  for(vector<TrigConfSig>::const_iterator isig = sig_vec.begin(); isig != sig_vec.end(); ++isig) {
    // Handy reference for TrigConfSeq
    const TrigConfSig &sig = *isig;
    
    // Iterate over output tes
    const std::vector<uint32_t> &te_vec = sig.getOutputTEs();
    for(unsigned int i = 0; i < te_vec.size(); ++i) {
      const uint32_t te_id = te_vec[i];
      std::vector<TrigConfSeq>::const_iterator iseq = std::find(seq_vec.begin(), seq_vec.end(), te_id);
      if(iseq != seq_vec.end()) {
	seq.push_back(*iseq);
      }
      else {
	std::cerr << "GetSeq error: missing TE" << std::endl;
      }
    }
  }

  return seq;
}


//----------------------------------------------------------------------------------
std::map<std::string, Trig::PSData> Trig::PSXMLparser(const std::string &fname, bool debug)
{
  //
  // Read Prescale/Passthrough info from XML file
  //
  std::map<std::string, Trig::PSData> psmap;
  
  if(fname.empty()) {
    if(debug) {
      cout << "Trig::PSXMLparser - empty input file name" << endl;
    }
    return psmap;
  }

  std::vector<std::string> files;
  Anp::StringTok(files, fname, ",");

  for(unsigned i = 0; i < files.size(); ++i) {
    const string file = files.at(i);

    if(debug) {
      cout << "Trig::PSXMLparser - will now parse XML file: " << file << endl;
    }
    
    Anp::Registry PSdef = Anp::ParseXml(file, debug);
    
    if(debug) {
      cout << "Trig::PSXMLparser - parsed XML file to Registry" << endl;
    }
    
    const std::map<std::string, Trig::PSData> costPS = PSXMLCost   (PSdef, debug);
    const std::map<std::string, Trig::PSData> mlv1PS = PSXMLMenuLV1(PSdef, debug);
    const std::map<std::string, Trig::PSData> mhltPS = PSXMLMenuHLT(PSdef, debug);
    
    psmap.insert(costPS.begin(), costPS.end());
    psmap.insert(mlv1PS.begin(), mlv1PS.end());
    psmap.insert(mhltPS.begin(), mhltPS.end());
  }

  return psmap;
}

//----------------------------------------------------------------------------------
std::map<std::string, Trig::BunchGroup> Trig::BunchXMLparser(const std::string &fname, bool debug)
{
  //
  // Read bunch group info from XML file
  //
  std::map<std::string, BunchGroup> bgmap;
  
  if(fname.empty()) {
    if(debug) {
      cout << "Trig::BunchXMLparser - empty input file name" << endl;
    }
    return bgmap;
  }

  std::vector<std::string> files;
  Anp::StringTok(files, fname, ",");
  
  for(unsigned i = 0; i < files.size(); ++i) {
    const string file = files.at(i);
    
    if(debug) {
      cout << "Trig::BunchXMLparser - will now parse XML file: " << file << endl;
    }
    
    Anp::Registry BGreg = Anp::ParseXml(file, debug);
    
    while(BGreg.KeyExists("bunchgroup")) {
      //
      // Read and then erase new "bunch" key
      //
      Anp::Registry bunch_reg;
      BGreg.Get("bunchgroup", bunch_reg);
      BGreg.RemoveKey("bunchgroup");
      
      if(debug) {
	cout << "  read new \"bunchgroup\" key" << endl;
      }
    
      while(bunch_reg.KeyExists("bunchgrouptype")) {
	//
	// Read and then erase new "group" key
	//
	Anp::Registry grp_reg;
	bunch_reg.Get("bunchgrouptype", grp_reg);
	bunch_reg.RemoveKey("bunchgrouptype");

	BunchGroup data;
	if(grp_reg.Get("bunchgroup_key",    data.key)  && 
	   grp_reg.Get("bunchgroup_size",   data.size) &&
	   grp_reg.Get("bunchgroup_keynum", data.keynum)) {
	  bgmap[data.key] = data;
	}
	else {
	  cout << "  failed to extract key \"bunchgrouptype\"" << endl;
	}
      }
    } 
  }
  if(debug) {
    cout << "Trig::BunchXMLparser - parsed XML file to Registry" << endl;
  }
  
  return bgmap;
}

//----------------------------------------------------------------------------------
std::map<std::string, Trig::PSData> Trig::PSXMLCost(Anp::Registry &PSdef, bool debug)
{
  //
  // Parse XML assumming cost convention for PS values
  //
  std::map<std::string, Trig::PSData> psmap;

  while(PSdef.KeyExists("level")) {
    //
    // Read and then erase new "level" key
    //
    Anp::Registry level_reg;
    PSdef.Get("level", level_reg);
    PSdef.RemoveKey("level");
    
    if(debug) {
      cout << "  read new \"level\" key" << endl;
    }
    
    while(level_reg.KeyExists("signature")) {
      //
      // Read and then erase new "signature" key
      //
      Anp::Registry sig_reg;
      level_reg.Get("signature", sig_reg);
      level_reg.RemoveKey("signature");

      std::string name;
      PSData data;
      
      data.has_ps = sig_reg.Get("chain_prescale",   data.ps);
      data.has_es = sig_reg.Get("express_prescale",   data.es_ps);
      data.has_pt = sig_reg.Get("passthrough",      data.pt);

      if(sig_reg.Get("sig_name", name) && (data.has_ps || data.has_pt || data.has_es)) {
	psmap[name] = data;
      }
      else {
	if(debug) {
	  cout << "  failed to extract key \"signature\"" << endl;
	}
      }
    }
  }
  
  return psmap;     
}

//----------------------------------------------------------------------------------
std::map<std::string, Trig::PSData> Trig::PSXMLMenuLV1(Anp::Registry &PSdef, bool debug)
{
  //
  // Parse XML assumming LV1 menu convention for PS values
  //
  std::map<std::string, Trig::PSData> psmap; 
  
  if(debug) {
    cout << "PSXMLMenuLV1 - parsing LVL1Config xml file..." << endl;
  }
  
  Anp::Registry menu, ps_set;

  if(!PSdef.Get("TriggerMenu", menu)) {
    cout << "PSXMLMenuLV1 - missing TriggerMenu element" << endl;
    return psmap;
  }
  if(!PSdef.Get("PrescaleSet", ps_set)) {
    cout << "PSXMLMenuLV1 - missing PrescaleSet element" << endl;
    return psmap;
  }

  while(menu.KeyExists("TriggerItem")) {
    //
    // Read L1 item name and counter
    //
    Anp::Registry item_reg;
    menu.Get("TriggerItem", item_reg);
    menu.RemoveKey("TriggerItem");
    
    std::string name;
    PSData data;  
    
    item_reg.Get("name",  name);
    item_reg.Get("ctpid", data.counter);
    
    psmap[name] = data;
    
    if(debug) {
      cout << "PSXMLMenuLV1 - ctpid=" << data.counter << " name=" << name << endl;
    }
  }

  while(ps_set.KeyExists("Prescale")) {
    //
    // Read L1 item name and counter
    //
    Anp::Registry ps_reg;
    ps_set.Get("Prescale", ps_reg);
    ps_set.RemoveKey("Prescale");
    
    std::string name;
    PSData data;  
    
    double value_d = 0, value_m = 0, value_n = 0, prescale = -1.0;
    int counter = -1;
        
    ps_reg.Get("ctpid", counter);

    if(ps_reg.Get("D", value_d) &&
       ps_reg.Get("M", value_m) &&
       ps_reg.Get("N", value_n)) {
      
      if(std::fabs(value_m+1.0) < 0.1) {
	cout << "PSXMLMenuLV1 - bad \"m\" value for ctpid=" << counter << endl;
	continue;
      }
      
      prescale = value_n*(value_m+value_d+1)/(value_m+1);
    
      if(debug) {
	cout << "PSXMLMenuLV1 - ctpid=" << counter 
	     << " d=" << value_d << " m=" << value_m << " n=" << value_n 
	     << " ps=" << prescale << endl;
      }
    }
    else if(!ps_reg.Get("Prescale", prescale)) {
      cout << "PSXMLMenuLV1 - failed to read PS for ctpid=" << counter << endl;
      continue;
    }
    
    for(map<string, Trig::PSData>::iterator it = psmap.begin(); it != psmap.end(); ++it) {
      PSData &data = it->second;

      if(counter >= 0 && static_cast<unsigned>(counter) == data.counter) {
	data.has_ps = true;
	data.ps     = prescale;
      }
    }
  }
  
  return psmap;
}

//----------------------------------------------------------------------------------
std::map<std::string, Trig::PSData> Trig::PSXMLMenuHLT(Anp::Registry &PSdef, bool debug)
{
  //
  // Parse XML assumming LV1 menu convention for PS values
  //
  std::map<std::string, Trig::PSData> psmap;

  if(debug) {
    cout << "PSXMLMenuHLT - parsing HLTConfig xml file..." << endl;
  }
  
  Anp::Registry menu;
  if(!PSdef.Get("CHAIN_LIST", menu)) {
    cout << "PSXMLMenuHLT - missing CHAIN_LIST element" << endl;
    return psmap;
  }

  while(menu.KeyExists("CHAIN")) {
    //
    // Read chain name and counter
    //
    Anp::Registry chain_reg;
    menu.Get("CHAIN", chain_reg);
    menu.RemoveKey("CHAIN");
    
    std::string name;
    chain_reg.Get("chain_name", name);

    PSData data;      
    data.has_ps = chain_reg.Get("prescale",     data.ps);
    data.has_pt = chain_reg.Get("pass_through", data.pt);
    
    Anp::Registry streams;
    chain_reg.Get("STREAMTAG_LIST", streams);

    while(streams.KeyExists("STREAMTAG")) {
      //
      // Read express stream PS
      // 
      Anp::Registry stream_reg;
      streams.Get("STREAMTAG", stream_reg);
      streams.RemoveKey("STREAMTAG");
      
      std::string stream_name, stream_type;
      stream_reg.Get("stream", stream_name);
      stream_reg.Get("type",   stream_type);

      if(stream_name == "express" && stream_type == "express") {
	data.has_es = stream_reg.Get("prescale", data.es_ps);
      }
    }
    
    psmap[name] = data;
    
    if(debug) {
      cout << "PSXMLMenuLV1 - chain=" << name
	   << " PS=" << data.ps << " PT=" << data.pt << " ES_PS=" << data.es_ps << endl;
    }
  }

  return psmap;
}

//-----------------------------------------------------------------------------
void Trig::UpdateChainPS(TrigMonConfig &config,
			 const std::map<std::string, Trig::PSData> &data,
			 bool)
{
  //
  // Update trigger configuration using PS values from XML file
  //
  if(data.empty()) {
    return;
  }

  //
  // Find maximum width of chain names
  //
  unsigned nwidth = 0;
  for(map<string, Trig::PSData>::const_iterator it = data.begin(); it != data.end(); ++it) {
    nwidth = std::max<unsigned>(nwidth, it->first.size());
  }
  
  vector<TrigConfChain> &chains = config.getVec<TrigConfChain>();

  for(unsigned i = 0; i < chains.size(); ++i) {
    TrigConfChain &chain = chains.at(i);
    
    //
    // Override PS if new values are provided
    //
    const map<string, Trig::PSData>::const_iterator ps_pt = data.find(chain.getName());
    if(ps_pt == data.end()) {
      continue;
    }

    const Trig::PSData &data = ps_pt->second;
    const std::string name = "   "+Anp::PadStrBack(chain.getName(), nwidth)+" ";
    
    if(data.has_ps) {
      if(std::fabs(chain.getPrescale()-data.ps) > 1.0e-6) {
	cout << name << "old, new PS:    " << chain.getPrescale()      << ", " << data.ps << endl;
	chain.setPrescale(data.ps);
      }
    }
  }

  for(unsigned i = 0; i < chains.size(); ++i) {
    TrigConfChain &chain = chains.at(i);
    
    //
    // Override PT if new values are provided
    //
    const map<string, Trig::PSData>::const_iterator ps_pt = data.find(chain.getName());
    if(ps_pt == data.end()) {
      continue;
    }
    
    const Trig::PSData &data = ps_pt->second;
    const std::string name = "   "+Anp::PadStrBack(chain.getName(), nwidth)+" ";
  
    if(data.has_pt) {
      if(std::fabs(chain.getPassThrough()-data.pt) > 1.0e-6) {
	cout << name << "old, new PT:    " << chain.getPassThrough()   << ", " << data.pt << endl;
	chain.setPassThrough(data.pt);
      }
    }
  }
    
  for(unsigned i = 0; i < chains.size(); ++i) {
    TrigConfChain &chain = chains.at(i);


    //
    // Override express PS if new values are provided
    //
    const map<string, Trig::PSData>::const_iterator ps_pt = data.find(chain.getName());
    if(ps_pt == data.end()) {
      continue;
    }

    const Trig::PSData &data = ps_pt->second;
    const std::string name = "   "+Anp::PadStrBack(chain.getName(), nwidth)+" ";

    if(data.has_es) {
      if(std::fabs(Anp::GetExpressPS(chain)-data.es_ps) > 1.0e-6) {
	cout << name << "old, new ES PS: " << Anp::GetExpressPS(chain) << ", " << data.es_ps << endl;
	Anp::SetExpressPS(chain, data.es_ps);	
      }
    }
  }
}

//-----------------------------------------------------------------------------
void Trig::UpdateGroups(TrigMonConfig &config,
			const std::vector<GrData> &grplist,
			bool debug)
{

  //
  // Added groups from XML
  //
  if(grplist.empty()) {
    return;
  }

  //
  // Print group content
  // 
  for (std::vector<GrData>::const_iterator grp = grplist.begin(); grp != grplist.end(); grp++) {    
    cout << "   group: " << grp->name << endl;
    for (std::vector<std::string>::const_iterator chain = grp->chains.begin(); chain != grp->chains.end(); chain++)  {	  
      cout << "      " << *chain << endl;
    }
  }

  //
  // Loop over chains in configuration adding groups
  //
  vector<TrigConfChain> &chains = config.getVec<TrigConfChain>();
  for(vector<TrigConfChain>::iterator chain = chains.begin(); chain != chains.end(); chain++) {
      
    for(unsigned i = 0; i < grplist.size(); ++i) {
      const GrData &group = grplist.at(i);
      
      for(unsigned j = 0; j < group.chains.size(); ++j) {
	const string &group_chain = group.chains.at(j);

	if(group_chain != chain->getName()) {
	  continue;
	}

	const std::vector<std::string>& chain_groups = chain->getGroup();
	if(find(chain_groups.begin(), chain_groups.end(), group.name) != chain_groups.end()) {
	  if(debug) {
	    cout << "UpdateGroups - ignore existing group " << group_chain << " for chain: " << chain->getName() << endl;
	  }
	  continue;
	}

	if(debug) {
	  cout << "UpdateGroups - add group " << group.name << " for chain: " << chain->getName() << endl;
	}
	
	chain->addGroup(group.name);
      }
    }
  }
}

//----------------------------------------------------------------------------------
std::string Trig::BitsAsString_uint32(uint32_t val) 
{
  std::stringstream str;

  for(int b = 31; b >= 0; --b) {
    uint32_t mask = 0;
    mask |= 1 << b;

    if(val & mask) {
      str << '1';
    }
    else {
      str << '0';
    }
    if(b == 16) {
      str << " ";
    }
  }
 
  return str.str();
}

//----------------------------------------------------------------------------------
std::string Trig::BitsAsString_uint16(uint16_t val) 
{
  std::stringstream str;

  for(int b = 15; b >= 0; --b) {
    uint16_t mask = 0;
    mask |= 1 << b;

    if(val & mask) {
      str << '1';
    }
    else {
      str << '0';
    }
  }
 
  return str.str();
}

//----------------------------------------------------------------------------------
std::string Trig::SeqStateAsString(const TrigMonSeq &entry)
{
  //
  // Return sequence state as a string
  //
  if(entry.isInitial())         return "kInitial";
  if(entry.isExecuted())        return "kStart";
  if(entry.isAlreadyExecuted()) return "kAlreadyExecutedl";
  if(entry.isPrevious())        return "kPrevious";
  
  return "Unknown";
}

//----------------------------------------------------------------------------------
vector<Trig::GrData> Trig::GroupXMLparser(const std::string &fname,
					  bool debug)
{
  //
  // Read Group info from Registry 
  //
  vector<Trig::GrData> group_list;
  
  if(fname.empty()) {
    if(debug) {
      cout << "Trig::GroupXMLparser - empty input file name" << endl;
    }

    return group_list;
  }

  if(debug) {
    cout << "Trig::GroupXMLparser - will now parse XML file... " << endl;
  }

  Anp::Registry GroupDef = Anp::ParseXml(fname, debug);
  
  if(debug) {
    cout << "Trig::GroupXMLparser - will now parse Registry... " << endl;
  }

  while(GroupDef.KeyExists("cplx_signature")) {
    //
    // Read and then erase new key
    //    
    Anp::Registry cplx_reg;
    GroupDef.Get("cplx_signature", cplx_reg);
    GroupDef.RemoveKey("cplx_signature");
    
    string type, name;
    if(!cplx_reg.Get("type", type) ||
       !cplx_reg.Get("sig_name", name) ) {

       continue;
    }
    
    if(debug) {
      cout << "  group: " << name << endl;
    }
    
    //
    // Read chain names
    //
    Anp::Registry comp_reg;
    cplx_reg.Get("components", comp_reg);

    vector<string> chains;    

    while(comp_reg.KeyExists("sig")) {
      //
      // Read and then erase new key
      //    
      Anp::Registry sig_reg;
      comp_reg.Get("sig", sig_reg);
      comp_reg.RemoveKey("sig");
      
      string trig;
      if(sig_reg.Get("sig_name", trig) && !trig.empty()) {
	//
	// Add new chain to group
	//
	if(std::find(chains.begin(), chains.end(), trig) == chains.end()) {
	  chains.push_back(trig);
	  if(debug) {
	    cout << "    trigger: " << trig << endl;
	  }
	} 
	else {
	  if(debug) {
	    cout << "    error - ignore duplicate trigger: " << trig << endl;
	  }
	}
      }
    }
    
    Trig::GrData gr;
    
    gr.name = name;
    gr.type = type;
    gr.chains = chains; 
    
    group_list.push_back(gr); 
  }
  
  return group_list;     
}

//-----------------------------------------------------------------------------
namespace Anp
{
  std::string AsString(TrigMonL1Item::Decision d)
  {
    if     (d == TrigMonL1Item::kL1BeforePrescale) return "kL1BeforePrescale";
    else if(d == TrigMonL1Item::kL1AfterPrescale)  return "kL1AfterPrescale";
    else if(d == TrigMonL1Item::kL1AfterVeto)      return "kL1AfterVeto";

    return "kReset";
  }

  std::string AsString(TrigMonChain::Decision d)
  {
    if     (d == TrigMonChain::kPassedThrough)    return "kPassedThrough";
    else if(d == TrigMonChain::kResurrected)      return "kResurrected";
    else if(d == TrigMonChain::kPassed)           return "kPassed";
    else if(d == TrigMonChain::kPassedRaw)        return "kPassedRaw";
    else if(d == TrigMonChain::kPrescaled)        return "kPrescaled";
    else if(d == TrigMonChain::kL1BeforePrescale) return "kL1BeforePrescale";
    else if(d == TrigMonChain::kL1AfterPrescale)  return "kL1AfterPrescale";
    else if(d == TrigMonChain::kL1AfterVeto)      return "kL1AfterVeto";

    return "kReset";
  }
  
  TrigMonL1Item::Decision AsTriggerBitLV1(const std::string &b)
  {
    if     (b == "kL1BeforePrescale" || b == "TBP") return TrigMonL1Item::kL1BeforePrescale;
    else if(b == "kL1AfterPrescale"  || b == "TAP") return TrigMonL1Item::kL1AfterPrescale;
    else if(b == "kL1AfterVeto"      || b == "TAV") return TrigMonL1Item::kL1AfterVeto;

    return TrigMonL1Item::kReset;
  }

  TrigMonChain::Decision AsTriggerBitHLT(const std::string &b)
  {
    if     (b == "kPassedThrough") return TrigMonChain::kPassedThrough;
    else if(b == "kResurrected")   return TrigMonChain::kResurrected;
    else if(b == "kPassed")        return TrigMonChain::kPassed;
    else if(b == "kPassedRaw")     return TrigMonChain::kPassedRaw;

    return TrigMonChain::kReset;
  }
}

//-----------------------------------------------------------------------------
void Anp::PrintHist(HtmlNode &snode, 
		    TH1 *h,		    
		    TCanvas *canvas,
		    string canvas_name,
		    string text,
		    string opts)
{
  //
  // Print histograms
  //

  if(canvas_name.find("eps") == string::npos && 
     canvas_name.find("png") == string::npos) {
    return;
  }

  HtmlNode *rnode = snode.GetRoot();
  if(!rnode) {
    cerr << "PrintHist - missing root HtmlNode" << endl;
    return;
  }

  ostream &os = snode.GetCont();

  if(!h || h->GetEntries() < 1) {
    Handle<HtmlNode> node = rnode->FindNode("empty_histogram.html");
    if(!node.valid()) {
      node = Handle<HtmlNode>(new HtmlNode("empty_histogram.html", HtmlNode::kHTML));
      rnode->AddChild(node);
      
      node -> GetCont() 
	<< "<pre>" << endl
	<< "This histogram has no entries" << endl
	<< "</pre>" << endl;
    }
    
    os << node->GetLink(text) << endl;
    return;
  }
  
  if(!canvas) {
    Handle<HtmlNode> node = rnode->FindNode("no_canvas.html");
    if(!node.valid()) {
      node = Handle<HtmlNode>(new HtmlNode("no_canvas.html", HtmlNode::kHTML));
      rnode->AddChild(node);      
      
      node -> GetCont() 
	<< "<pre>" << endl
	<< "Histograms were disabled for this run" << endl
	<< "</pre>" << endl;
    }
    
    os << node->GetLink(text) << endl;
    return;
  }
  
  const string cpath = snode.GetAbsolutePath()+"/"+canvas_name;
  
  TDirectory *dir = h->GetDirectory();
  h->SetDirectory(0);


  if(dynamic_cast<TH2 *>(h)) {
    h->Draw("colz");
  }
  else if(opts.find("DRAW_ERROR") != string::npos) {
    h->Draw("e1");
  }
  else {
    h->Draw();
  }

  TList* canvas_pads = dynamic_cast<TList *> (canvas->GetListOfPrimitives());
  TListIter canvas_pads_it( canvas_pads );
  TPad* canvas_pad = NULL;

  if(opts.find("logy") != string::npos && h->Integral() > 0.0) {
    while ((canvas_pad = dynamic_cast<TPad *>(canvas_pads_it.Next()) )) {
      canvas_pad->SetLogy(1);
    }
    canvas->SetLogy(1);
    canvas_pads_it.Reset();
  }
  if(opts.find("logz") != string::npos && h->Integral() > 0.0) {
    while ((canvas_pad = dynamic_cast<TPad *>(canvas_pads_it.Next()))) {
      canvas_pad->SetLogz(1);
    }

    canvas->SetLogz(1);
    canvas_pads_it.Reset();
  }

  if(canvas_name.find("eps") != string::npos) {
    canvas -> Print(cpath.c_str());
  }
  else if(canvas_name.find("png") != string::npos) {
    TImageDump *imdump = new TImageDump(cpath.c_str());
    canvas -> Paint();
    delete imdump;
  }
  
  if(opts.find("logy") != string::npos) {
    while ((canvas_pad = dynamic_cast<TPad *>(canvas_pads_it.Next()) )) {
      canvas_pad->SetLogy(0);
    }

    canvas->SetLogy(0);
    canvas_pads_it.Reset();
  }
  if(opts.find("logz") != string::npos) {
    while ((canvas_pad = dynamic_cast<TPad *>(canvas_pads_it.Next()))) {
      canvas_pad->SetLogz(0);
    }

    canvas->SetLogz(0);
    canvas_pads_it.Reset();
  }

  h->SetDirectory(dir);  
 
  //
  // Insert gif link
  //
  HtmlNode cnode(canvas_name, HtmlNode::kGIF);
  os << cnode.GetLink(text) << endl;
}

//----------------------------------------------------------------------------
unsigned Anp::GetWidth(const double val)
{
  if(val > 0.0) {
    return static_cast<unsigned>(std::floor(std::log10(val+1.0)))+1;
  }
  else if(val < 0.0) {
    return static_cast<unsigned>(std::floor(std::log10(std::fabs(val)+1.0)))+2;
  }
  
  return 0;
}

//----------------------------------------------------------------------------
void Anp::SetROOT_defaults()
{
  Anp::HistMan::Instance().SetROOTDefaults();
}
