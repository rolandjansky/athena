/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>
#include <sstream>
#include <boost/algorithm/string.hpp>

// Local
#include "TrigCostBase/Factory.h"
#include "TrigCostBase/UtilCore.h"
#include "TrigCostBase/EventSvc.h"
#include "TrigCostBase/UtilHack.h"
#include "TrigCostBase/UtilTrig.h"

// Local
#include "TrigCostRate/OverlapMon.h"

REGISTER_ANP_OBJECT(AlgEvent,OverlapMon)

using namespace std;

//-----------------------------------------------------------------------------
Anp::OverlapMon::OverlapMon()
  :fDebug(false),
   fDebugDetails(false),
   fWithPS(false),
   fLevel(0)
{
}

//-----------------------------------------------------------------------------
Anp::OverlapMon::~OverlapMon()
{
}

//-----------------------------------------------------------------------------
void Anp::OverlapMon::InsertUniqueString(std::vector<std::string>& strvec, const std::string& str)
{
  for ( std::vector<std::string>::const_iterator itr = strvec.begin();
	itr != strvec.end(); itr++ )
    {
      if (*itr==str) return;
    }

  // we haven't returned so str is not in strvec
  strvec.push_back(str);
}



//-----------------------------------------------------------------------------
void Anp::OverlapMon::DoConfiguration(const TrigMonConfig &config)
{
  
  if (fDebug) log() << "In DoConfiguration" << endl;


  // Build a bunch of maps 
  for (std::vector<TrigConfChain>::const_iterator chain = config.begin<TrigConfChain>();
       chain != config.end<TrigConfChain>(); chain++) {
    
    const std::vector<string> streams = chain->getStream();
    const std::vector<string> groups  = chain->getGroup();
    
    if (fDebugDetails) {
      log() << "Chain: "    << chain->getName()    
	    << " Level: "   << chain->getLevelId()
	    << " Counter: " << chain->getCounter()
	    << " Streams: ";
      bool first = true;       
      for ( std::vector < std::string >::const_iterator str = chain->getStream().begin();
	    str != chain->getStream().end(); str++) {
	if(!first) {cout << ",";} else { first=false;}
	cout << *str;
      }
      cout << endl;
      }
    
    fCounter2Chain[chain->getLevelId()-1][chain->getCounter()] = chain->getName();
    fChain2Counter[chain->getLevelId()-1][chain->getName()]    = chain->getCounter();
    fPrescale[chain->getLevelId()-1][chain->getCounter()]      = chain->getPrescale();

    std::string slower= chain->getLowerName();
    std::vector<std::string> strs;
    boost::split(strs, slower, boost::is_any_of(string(",")));

    //    for(unsigned i=0; i<strs.size();++i) cout << strs.at(i)  << "   " ;
    //if(strs.size()>0) cout << endl;

    for(unsigned i=0;i<strs.size();++i){
      fLowerChains  [chain->getLevelId()-1][chain->getCounter()].insert(strs.at(i));
    }

    // Only create the map when necessary
    if(fLevel != chain->getLevelId()) continue;
    fCounter2Chain[chain->getLevelId()-1][chain->getCounter()] = chain->getName();
    fChain2Counter[chain->getLevelId()-1][chain->getName()]    = chain->getCounter();
    fPrescale[chain->getLevelId()-1][chain->getCounter()]      = chain->getPrescale();

    if (chain->getLevelId()==fLevel) {
      for (std::vector<string>::const_iterator stream = streams.begin();
	   stream != streams.end(); stream++) 
	{
	  // insert streams if not already there (should be already there because SMK 
          // should not change during a run)
	  InsertUniqueString(fStream2Chains[*stream],chain->getName());
	  InsertUniqueString(fChain2Streams[chain->getName()],*stream);
	}

      for (std::vector<string>::const_iterator group = groups.begin();
	   group != groups.end(); group++) 
	{
	  cout << "here" << *group << " " << __LINE__ << endl;
	  // insert groups if not already there (should be already there because SMK 
          // should not change during a run)
	  if (group->find("BW")    != string::npos)
	    {
	      cout << "here" << __LINE__ << endl;
	      InsertUniqueString(fBWGrp2Chains[*group],chain->getName());
	      InsertUniqueString(fChain2BWGrps[chain->getName()],*group);
	    }
	}
    }
    
  }

  if (fDebugDetails) {
    for (str2strvec_map::const_iterator  str = fStream2Chains.begin();
	 str != fStream2Chains.end(); str++) {
      cout << "Chainlist of " << (*str).first << " stream size=" << (*str).second.size() << endl;
      cout << "   ";
      for (std::vector<std::string>::const_iterator ch = (*str).second.begin();
	   ch != (*str).second.end(); ch++) {
	cout << *ch << " ";
      }
      cout << endl;      
    }
    for (str2strvec_map::const_iterator  str = fBWGrp2Chains.begin();
	 str != fBWGrp2Chains.end(); str++) {
      cout << "Chainlist of " << (*str).first << " bwgrps size=" << (*str).second.size() << endl;
      cout << "   ";
      for (std::vector<std::string>::const_iterator ch = (*str).second.begin();
	   ch != (*str).second.end(); ch++) {
	cout << *ch << " ";
      }
      cout << endl;      
    }
  }

  //
  // Get Effective PS
  //
  for(int2float_map::iterator pre = fPrescale[1].begin(); pre != fPrescale[1].end();++pre){
    pre->second=GetEffectivePS(2,pre->first);
    //cout << "L2 Efective PS: " << pre->second << " name: " << fCounter2Chain[1][pre->first] << endl;
  }
  for(int2float_map::iterator pre = fPrescale[2].begin(); pre != fPrescale[2].end();++pre){
    pre->second=GetEffectivePS(3,pre->first);
    //cout << "EF Efective PS: " << pre->second << " name: " << fCounter2Chain[2][pre->first] << endl;
  }

  fConfig  =config;
  fIsConfig=true;
}


//-----------------------------------------------------------------------------
void Anp::OverlapMon::Config(const Registry &reg)
{
  //
  // Read configfuration and setup ntuple and other services
  //
  fReg = reg;
  reg.Get("OverlapMon", "Debug",        fDebug);
  reg.Get("OverlapMon", "DebugDetails", fDebugDetails);
  reg.Get("OverlapMon", "DoPS",         fWithPS);
  reg.Get("OverlapMon", "Level",        fLevel);
  if (fDebug) log() << "In Config" << endl;

  //
  // Register trigger configuration algorithm
  //
  Anp::RegisterCallback(reg, "OverlapMon::ConfAlg", fConf, this);

  //
  // Register this as an HTML page provider
  //
  Factory<HtmlABC>::Instance().Hold(AlgEvent::GetName(), Handle<HtmlABC>(this));  

  //
  // Book tree branches before Init()
  //
  fEntry = NtupleSvc::Instance("event").Branch<TrigMonEvent>("data");
}

void Anp::OverlapMon::Signal(const Registry &, const string &signal)
{
  //
  // Handle callback from configuration algorithm 
  //
  if(signal == "TRIG_CONFIG" || signal == "TRIG_CONFIG_DB") {
    if(!fConf.valid()) { 
      log() << "Signal - invalid configuration algorithm" << endl;
      return;
    }

    DoConfiguration(fConf->GetConfig()); 
  }
}


//-----------------------------------------------------------------------------
void Anp::OverlapMon::ResetActiveStreams() 
{
  for (str2bool_map::iterator str = fActiveStreams.begin();
       str != fActiveStreams.end(); str++) {
    (*str).second = false;
  }
}

void Anp::OverlapMon::SetActiveStreams(const TrigMonChain &chain) 
{
  const vector<TrigConfChain>::const_iterator iconf = fConfig.findChain(chain);
  if(iconf == fConfig.end<TrigConfChain>()) {
    return;
  }

  const vector<string> &streams = iconf->getStream();
  const vector<string> &groups  = iconf->getGroup();

  for (vector<string>::const_iterator str = streams.begin();
       str != streams.end(); str++) {
    fActiveStreams[*str]=true;
  }

  for (vector<string>::const_iterator grp = groups.begin();
       grp != groups.end(); grp++) {
    if (grp->find("BW") != string::npos) {
      fActiveStreams[*grp]=true;
    }
  }
}

bool Anp::OverlapMon::SkipStream(std::string str) {
  if (str=="EFCostMonitoring") {return true;}
  if (str=="express"         ) {return true;}
  if (str=="L1Calo"          ) {return true;}
  if (str=="L1MinBias"       ) {return true;}
  if (str=="L1Muon"          ) {return true;}
  return false;
}

//-----------------------------------------------------------------------------
bool Anp::OverlapMon::Init()
{  
  //  if (fDebug) 
  log() << "In OverlapMon::Init" << endl;
  //
  // Read configuration from init file and release pointer
  //
  //
  // Collect Scale Algorithm
  //
  if(fScaleBG.valid() || (!fWithPS)) return true;

  string scale_name;
  if(fReg.Get("OverlapMon", "ScaleBG", scale_name)) {
    fScaleBG = Factory<ScaleABC>::Instance().Get(scale_name);
    log() << "Init - " << scale_name << " algorithm name is set" << endl;
  }
  else {
    log() << "Init - " << scale_name << " algorithm name is NOT set" << endl;
  }

  return true;
}

//-----------------------------------------------------------------------------
void Anp::OverlapMon::Exec()
{
  if (fDebugDetails) log() << "In Exec " 
  		    << 	fEntry.valid() << " " 	    
  		    << 	fEntry.GetState() << " " 	    
  		    << 	fEntry.GetEvent() << " " 	    
  		    << endl;

  //
  // Collect Event Scale Factor
  //
  double scale_bg = 1.0;
  if(fScaleBG.valid()){
    scale_bg = fScaleBG->GetScaleBCID().scale_lumi;
    if(fDebug) log() << "Exec - scaleBG algorithm valid with value: " << scale_bg << endl;
  }
  else{ 
    if(fWithPS) log() << "Exec - invalid scaleBG! Using default scale: " << scale_bg << endl;
  }
  //
  // Read new event entry
  //
  if(!(fEntry.valid() && 
       fEntry.GetState() == Ntuple::kREAD &&
       fEntry.GetEvent() == Ntuple::kPASS)) 
    {
      return;
    }

  if(!fIsConfig) return;

  // Clear stream activations
  ResetActiveStreams();

  //
  // Get info
  // 
  const TrigMonEvent &event =  fEntry.GetRef();
  const std::vector<uint16_t> &event_lv1 = event.getL1Item();
  const std::vector<uint32_t> &event_hlt = event.getChain();
  //
  // L1 Correlations
  //
  for (std::vector<uint16_t>::const_iterator ch1 = event_lv1.begin();
       ch1 !=event_lv1.end(); ch1++)  {

    // convert integer into item object
    const TrigMonL1Item item1(*ch1);

    if(!item1.isPassed(TrigMonL1Item::kL1AfterVeto)) continue;

    if(fLevel!=1) continue;
      
    const unsigned int counter1 = item1.getCounter();

    for (std::vector<uint16_t>::const_iterator ch2 = event_lv1.begin();
	 ch2 !=event_lv1.end(); ch2++) {

      // convert integer into item object
      const TrigMonL1Item item2(*ch2);

      if(!item2.isPassed(TrigMonL1Item::kL1AfterVeto)) continue;

      const unsigned int counter2 = item2.getCounter();

      counter_doublet trgs(counter1,counter2);
      if (fCounters[0].find(trgs)==fCounters[0].end()) {
	if (fDebugDetails) log() << "In Exec, initializing counter "  
				 << fCounter2Chain[0][counter1] << " " 
				 << fCounter2Chain[0][counter2] << endl;
	fCounters[0][trgs]=0;
      }
      fCounters[0][trgs]++;

      // Intializing scaleBG counters for L1
      if(fWithPS){
	if (fScaleCount[0].find(trgs)==fScaleCount[0].end()) {
	  fScaleCount[0][trgs]=0.0;
	}
	fScaleCount[0][trgs]+=scale_bg;

      } // end ScaleBG
      
    }
  }
  
  //
  // L2/EF Correlations
  //

  for (std::vector<uint32_t>::const_iterator ch1 = event_hlt.begin();
       ch1 !=event_hlt.end(); ch1++)  {    

    // convert integer into chain object
    const TrigMonChain chain1(*ch1);      
    
    const unsigned int counter1 = chain1.getCounter();
    
    if(!chain1.isPassed(TrigMonChain::kPassed)) continue; 
    //changed getLevel from 3 to 2
    if (chain1.getLevel()==fLevel) SetActiveStreams(chain1);

    for (std::vector<uint32_t>::const_iterator ch2 = event_hlt.begin();
	 ch2 !=event_hlt.end(); ch2++)  {    
      
      // convert integer into chain object
      const TrigMonChain chain2(*ch2);
      
      if (chain1.getLevel()!=chain2.getLevel()) continue;
      
      if (!chain2.isPassed(TrigMonChain::kPassed)) continue; 
      
      const unsigned int counter2 = chain2.getCounter();
	
      counter_doublet trgs(counter1,counter2);
      int lvlidx = chain1.getLevel()-1;
      if (fCounters[lvlidx].find(trgs)==fCounters[lvlidx].end()) {
	if (fDebugDetails) log() << "In Exec, initializing counter "  
				 << fCounter2Chain[lvlidx][counter1] << " " 
				 << fCounter2Chain[lvlidx][counter2] << endl;
	fCounters[lvlidx][trgs]=0;
      }
      fCounters[lvlidx][trgs]++;

      // Intializing scaleBG counters for HLT
      if(fWithPS){
	if (fScaleCount[lvlidx].find(trgs)==fScaleCount[lvlidx].end()) {
	  fScaleCount[lvlidx][trgs]=0.0;
	}
	fScaleCount[lvlidx][trgs]+=scale_bg;

      } // end ScaleBG
      
    } // chain 2
    
  } // chain 1


  // Make entries in stream overlap table
  for (str2bool_map::iterator str1 = fActiveStreams.begin();
       str1 != fActiveStreams.end(); str1++) {
    if ((*str1).second == false) continue;
    for (str2bool_map::iterator str2 = fActiveStreams.begin();
	 str2 != fActiveStreams.end(); str2++) {
      if ((*str2).second == false) continue;
      string_doublet trgs((*str1).first,(*str2).first);
      if (fStreamCounts.find(trgs)==fStreamCounts.end()) {
	fStreamCounts[trgs]=0;
      }
      fStreamCounts[trgs]++;
    }
  }
       

  // Make entries in stream-chain overlap table
  for (str2bool_map::iterator str = fActiveStreams.begin();
       str != fActiveStreams.end(); str++) {
    if ((*str).second == false) continue;

    for (std::vector<uint32_t>::const_iterator ch = event_hlt.begin();
       ch !=event_hlt.end(); ch++)  {    

      // convert integer into chain object
      const TrigMonChain chain(*ch);      
      
      if(!chain.isPassed(TrigMonChain::kPassed)) continue; 
      //set getLevel from 3 to 2
      //      if (chain.getLevel()==fLevel) SetActiveStreams(chain);

      const vector<TrigConfChain>::const_iterator iconf = fConfig.findChain(chain);
      if(iconf == fConfig.end<TrigConfChain>()) {
	log() << "Chain missing configuration" << endl;
	continue;
      }

      string_doublet trgs((*str).first,iconf->getName());
      if (fStreamChainOvlp.find(trgs)==fStreamChainOvlp.end()) {
	fStreamChainOvlp[trgs]=0;
      }
      fStreamChainOvlp[trgs]++;

    }
  }

}

//-----------------------------------------------------------------------------
void Anp::OverlapMon::Done()
{
  if (fDebug) log() << "In Done" << endl;
}

//-----------------------------------------------------------------------------
// Make html page
void Anp::OverlapMon::Write(HtmlNode &inode, HtmlMenu &) 
{
  if (fDebug) log() << "OverlapMon::Write - In Write HTML" << endl;
 
  //return if there are no chains
  if(fChain2Counter[fLevel-1].size()==0) return;

  bool trg_counts_exist = false;

  //checking for nonzero trigger counts
  for(str2int_map::const_iterator chn1 = fChain2Counter[fLevel-1].begin(); 
      chn1 != fChain2Counter[fLevel-1].end();++chn1){
    
    counter_doublet trgs_norm(chn1->second,chn1->second);
    
    if (fCounters[fLevel-1].find(trgs_norm)!=fCounters[fLevel-1].end()) {
      if(fCounters[fLevel-1][trgs_norm]>0){ 
	trg_counts_exist = true;
	break;
      }
    }
  }

  if(!trg_counts_exist) return;

  string slevel ="L1";
  if(fLevel==2) slevel="L2";
  if(fLevel==3) slevel="EF";

  Handle<HtmlNode> top = inode.AddNode("OverlapMon_"+slevel+".html", "Trigger Overlaps "+slevel+"<br>");
  top->GetCont() << "<pre>";

  //overlap of chain by chain

  if(fWithPS) WriteChnWithPS(*top,fLevel-1,slevel);
  else WriteChn(*top,fLevel-1,slevel);

  AddGroupOverlap(top,"Stream",fStream2Chains);
  AddGroupOverlap(top,"Bandwidth Groups",fBWGrp2Chains);

  //top->GetCont() << endl << endl;
  
  top->GetCont() << "</pre>";

  if(fDebug) cout << "OverlapMon::Write - size of the bandwidth group to chains is: " << fBWGrp2Chains.size() << endl;
}

void Anp::OverlapMon::AddGroupOverlap(Handle<HtmlNode>  top, 
				      std::string       gname,
				      str2strvec_map&   grp2chn) 
{
  // only for the correct trigger level
  if(fChain2Counter[fLevel-1].size()==0) return;  

  // make file name with no white space
  stringstream levelKey;
  levelKey << fLevel;
  string fname = "OverlapMon_"+gname+"_Page_L"+levelKey.str()+".html";
  size_t f =0;
  while ( (f = fname.find(" ")) != string::npos) {
    fname.replace(f,1,"_");
  }
  cout << "File name = " << fname << " " << gname << endl;

  // Setup table to hole the overlaps
  top->GetCont() << "Overlap of " << gname << " with other streams" << endl
		 << "<style type=\"text/css\">"<< endl
		 << "table,  th {border:1px solid black;}"<< endl
		 << "th { padding:10px; background-color:white; color:black; }"<< endl
		 << "td {padding-left:15px;padding-right:15px;}"<< endl
		 << "</style>"<< endl
		 << "<script src=\"sorttable.js\"></script>" << endl
		 << "<table border=1 frame=box rules=cols class=\"sortable\">" << endl
		 << "<tr><th></th></tr>"<< endl;

  //
  // Add a stream overlap page
  //
  Handle<HtmlNode> strpage = top->AddNode(fname, gname+" Overlap Table<br><br>");

  ostream &os_strs = strpage->GetCont();
  os_strs << "<pre>" << endl;
  os_strs << "Numbers in table percentage of the row " << gname << " that is also in the column " << gname << endl;
  os_strs << "<style type=\"text/css\">"<< endl
	  << "table,  th {border:1px solid black;}"<< endl
	  << "th { padding:10px; background-color:white; color:black; }"<< endl
	  << "td {padding-left:15px;padding-right:15px;}"<< endl
	  << "</style>"<< endl
	  << "<script src=\"sorttable.js\"></script>" << endl
	  << "<table border=1 frame=box rules=cols class=\"sortable\">" << endl
	  << "<tr>"
	  << "<th></th>";

  // column headers
  for (str2strvec_map::const_iterator  str2 = grp2chn.begin();
       str2 != grp2chn.end(); str2++) {
    if (SkipStream((*str2).first)) continue;
    os_strs << "<th>"<< (*str2).first << "</th>";	
  }
  os_strs << "</tr>" << endl;

  
  // rows = one per stream
  for (str2strvec_map::const_iterator  str1 = grp2chn.begin();
       str1 != grp2chn.end(); str1++) {
    if (SkipStream((*str1).first)) continue;

    // start row
    os_strs << "<tr>"
       << "<td align=\"left\"> "   << (*str1).first << " </td>";

    
    //    bool str2_lt_str1 = true;
    for (str2strvec_map::const_iterator  str2 = grp2chn.begin(); str2 != grp2chn.end(); str2++) {
      if (SkipStream((*str2).first)) continue;

      
      const int EFidx = fLevel-1;
      double total_overlap = double(fStreamCounts[string_doublet((*str1).first,(*str2).first)]);
      double total_str1    = double(fStreamCounts[string_doublet((*str1).first,(*str1).first)]);

      // entry is link to chain vs chain table
      os_strs << "<td align=\"center\"> ";      
      Handle<HtmlNode> stream_pair_page;
      if (total_overlap>0.0) {
	if (total_str1>0.0) { total_overlap/=total_str1; total_overlap*=100.0; }
	stream_pair_page = strpage->AddNode("OverlapMon_"+(*str1).first+"_"+(*str2).first+".html",
					    Form("%3.1f",total_overlap));
      } else {
	continue;
      }
      os_strs << " </td>";

      // make chain vs chain table
      ostream &os = stream_pair_page->GetCont();
      os << "<pre>" << endl;
      os << "Overlap between " << (*str1).first << " and " << (*str2).first << " is " << total_overlap << "%" << endl;
      os << "Numbers in table are as a percentage of the common events in " 
	 << (*str1).first << " and " << (*str2).first << endl;
      os << "<style type=\"text/css\">"<< endl
	 << "table,  th {border:1px solid black;}"<< endl
	 << "th { padding:10px; background-color:white; color:black; }"<< endl
	 << "td {padding-left:15px;padding-right:15px;}"<< endl
	 << "</style>"<< endl
	 << "<script src=\"sorttable.js\"></script>" << endl
	 << "<table border=1 frame=box rules=cols class=\"sortable\">" << endl
	 << "<tr>"
	 << "<th></th>";

      // column headers
      for (std::vector<std::string>::const_iterator ch2 = (*str2).second.begin();
	     ch2 != (*str2).second.end(); ch2++) {
	os << "<th>"<< *ch2 << "</th>";	
      }
      os << "</tr>" << endl;
      
      // table body, outer loop = rows
      for (std::vector<std::string>::const_iterator ch1 = (*str1).second.begin();
	   ch1 != (*str1).second.end(); ch1++) {

	unsigned int counter1 = fChain2Counter[EFidx][*ch1];

	os << "<tr>"
	   << "<td align=\"left\"> "   << *ch1                         << " </td>";

	// table body, inner loop = entries (columns)
	for (std::vector<std::string>::const_iterator ch2 = (*str2).second.begin();
	     ch2 != (*str2).second.end(); ch2++) {
	  
	  unsigned int counter2 = fChain2Counter[EFidx][*ch2];
	  
	  counter_doublet trgs(counter1,counter2);	 
	  double overlap12 = 0;	
	  //	  if(fWithPS && fScaleCount[EFidx].find(trgs)!=fScaleCount[EFidx].end()){
	  //  overlap12 = fScaleCount[EFidx][trgs];
	  //} 
	  if (fCounters[EFidx].find(trgs)!=fCounters[EFidx].end()) {
	    overlap12 = fCounters[EFidx][trgs];
	  }

	  double denom = fStreamCounts[string_doublet((*str1).first,(*str2).first)];
	  if (denom>0.0) {
	    overlap12/=denom;
	    overlap12*=100.0;
	  } else {
	    overlap12 = 0.0;
	  }
	  
	  if (overlap12<0.0001) {
	    os << "<td align=\"center\"> </td>";
	  } else {
	    os << "<td align=\"center\"> " << Form("%3.1f",overlap12)  << " </td>";
	  }
	 	  
	}

	os << "</tr>" << endl;

      }      
      os << "</table>" << endl;
      os << "</pre>" << endl;


      stream_pair_page->Write();
      stream_pair_page.release();


    }
    os_strs << "</tr>" << endl;
  }
  os_strs << "</table>" << endl;
  os_strs << "</pre>" << endl;

  
  //
  // Add  stream to chain overlap pages
  //

  for (str2strvec_map::const_iterator  str1 = grp2chn.begin();
       str1 != grp2chn.end(); str1++) {
    if (SkipStream((*str1).first)) continue;

    //Handle<HtmlNode> str2ch_page = top->AddNode("OverlapMon_Stream_to_Chain_"+(*str1).first+".html", 
    //"Overlap of "+(*str1).first+" chains with other streams<br>");

    Handle<HtmlNode> str2ch_page(new HtmlNode("OverlapMon_Stream_to_Chain_"+(*str1).first+"_L"+levelKey.str()+".html",HtmlNode::kHTML));
    top->AddChild(str2ch_page);

    top->GetCont() << "<tr><th>" << str2ch_page->GetLink((*str1).first) << "</th></tr>" << endl;

    ostream &os = str2ch_page->GetCont();
    os << "<pre>" << endl;
    os << "<h2>Overlap of " << (*str1).first << " chains with each stream </h2>" << endl;
    os << "Numbers are as a percentage of events in the "<< (*str1).first << " stream" << endl;
    os << "<style type=\"text/css\">"<< endl
       << "table,  th {border:1px solid black;}"<< endl
       << "th { padding:10px; background-color:white; color:black; }"<< endl
       << "td {padding-left:15px;padding-right:15px;}"<< endl
       << "</style>"<< endl
       << "<script src=\"sorttable.js\"></script>" << endl
       << "<table border=1 frame=box rules=cols class=\"sortable\">" << endl;


    // headers = streams, with total overlap
    os << "<tr>"
       << "<th align=\"right\"><br><br>  Total Overlap --></th>";  
    for (str2strvec_map::const_iterator  str2 = grp2chn.begin();
	 str2 != grp2chn.end(); str2++) {
      if (SkipStream((*str2).first)) continue;	 
      
      double overlap = double(fStreamCounts[string_doublet((*str1).first,(*str2).first)]);
      double total   = double(fStreamCounts[string_doublet((*str1).first,(*str1).first)]); 
      if (total>0.0) {
	overlap/=total;
	overlap*=100.0;
      }
      os << "<th>"<< (*str2).first 
	 << "<br><br>"<< Form("%3.1f",overlap) << "</th>";	
    }
    os << "</tr>" << endl;
        
    // rows = one for each chain in stream 1
    for (std::vector<std::string>::const_iterator ch1 = (*str1).second.begin();
	 ch1 != (*str1).second.end(); ch1++) {
          
      // row label
      os << "<tr>" 
	 << "<td align=\"left\"> "   << *ch1 << " </td>";
    
      // row entries = one for each stream 
      for (str2strvec_map::const_iterator  str2 = grp2chn.begin();
	   str2 != grp2chn.end(); str2++) {
	if (SkipStream((*str2).first)) continue;

	double overlap = 0;
	string_doublet trgs((*str2).first,*ch1);
	str_doublet_map::const_iterator itr = fStreamChainOvlp.find(trgs);
	if (itr!=fStreamChainOvlp.end()) {
	  overlap = (*itr).second;
	}
	
	if (overlap==0) {
	  os << "<td align=\"center\"> </td>";
	} else {
	  double total   = fStreamCounts[string_doublet((*str1).first,(*str1).first)]; 
	  if (total>0.0) {
	    overlap/=total;
	    overlap*=100.0;
	  }	  
	  os << "<td align=\"center\"> " <<  Form("%3.1f",overlap) << " </td>";
	}
      }
      
      os << "</tr>" << endl; // end row

    }      
    // end table
    os << "</table>" << endl;
    os << "</pre>" << endl;
    }

  os_strs << endl;

  top->GetCont() << "</table>" << endl;
}

//---------------------------------------------------------------------------------------
void Anp::OverlapMon::WriteChn(HtmlNode &inode, const unsigned index, const string key)
{
  //
  // Writing an overlap table chain by chain
  //
  if(index>2){ 
    cout << "OverlapMon::WriteChn - Error index out of range" << endl;
    return;
  }

  // Make sure that we have chain counts
  if(fChain2Counter[index].size()==0) return;

  // map from overlap table to chain id
  std::map<unsigned, Handle<HtmlNode> > m_chn_olap;

  Handle<HtmlNode> anode = inode.AddNode("OverlapMon_chn_"+key+".html", "Trigger Overlaps for "+key+" Chains <br>");

  ostream &os = anode->GetCont();

  os << "<pre>" << endl;
  
  os << "Overlap tables for the Following Chains: " << endl;
  os << "<style type=\"text/css\">"<< endl
     << "table,  th {border:1px solid black;}"<< endl
     << "th { padding:10px; background-color:white; color:black; }"<< endl
     << "td {padding-left:15px;padding-right:15px;}"<< endl
     << "</style>"<< endl
     << "<script src=\"sorttable.js\"></script>" << endl
     << "<table border=1 frame=box rules=cols class=\"sortable\">" << endl
     << "<tr><th>Chain Name</th></tr>"<< endl;

  // Chains without overlap
  set<unsigned> nover, nocounts;

  // write column headers - for chain with overlap
  for(str2int_map::const_iterator chn1 = fChain2Counter[index].begin(); 
      chn1 != fChain2Counter[index].end();++chn1){
    
    // get total chain counts for chn1 to normalize the column
    counter_doublet trgs_norm(chn1->second,chn1->second);
    double chn1_tot = -1.0;
    const unsigned counter = chn1->second;
    if (fCounters[index].find(trgs_norm)!=fCounters[index].end()) {
	chn1_tot = fCounters[index][trgs_norm];
    }
    
    // create a chain overlap table  when there are chain counts
    if(chn1_tot > 0.0){ 
      
      stringstream chn_path; 
      chn_path << "OverlapMon_chn" << key << "_" << chn1->second << ".html";
      
      Handle<HtmlNode> trigo(new HtmlNode(chn_path.str(),HtmlNode::kHTML));

      m_chn_olap[counter] = trigo; //anode->AddNode(chn_path.str(), "Trigger Overlap for "+chn1->first+" <br>");

      anode->AddChild(trigo);

      ostream &cs = trigo->GetCont();

      cs << "<pre>" << endl;
      cs << "Numbers in table are percentage of overlap with the chain " << chn1->first << endl;
      cs << "<style type=\"text/css\">"<< endl
	 << "table,  th {border:1px solid black;}"<< endl
	 << "th { padding:10px; background-color:white; color:black; }"<< endl
	 << "td {padding-left:15px;padding-right:15px;}"<< endl
	 << "</style>"<< endl
	 << "<script src=\"sorttable.js\"></script>" << endl
	 << "<table border=1 frame=box rules=cols class=\"sortable\">" << endl
	 << "<tr><th>Chain Name</th><th>Overlap (%)</th></tr>" << endl;

      // add link to chn1 overlap table
      os << "<tr>"
	 << "<td align=\"left\">" << trigo->GetLink(chn1->first) << "</td>"
	 << "</tr>" << endl;

    }
    else nocounts.insert(chn1->second);
  }

  os << "</table>" << endl;
  
  // table body, outer loop = rows
  for (str2int_map::const_iterator chn1 = fChain2Counter[index].begin(); 
       chn1 != fChain2Counter[index].end();++chn1) {
    
    const unsigned int counter1 = chn1->second;
   
    if(nocounts.find(counter1)!=nocounts.end()) continue;
 
    ostream &cs = m_chn_olap[counter1]->GetCont();

    // get total chain counts for chn1 to normalize the column
    counter_doublet trgs_norm(counter1,counter1);
    double chn1_tot = -1.0;
    if (fCounters[index].find(trgs_norm)!=fCounters[index].end()) {
	chn1_tot = fCounters[index][trgs_norm];
    }
    
    // table body, inner loop = entries (columns)
    for (str2int_map::const_iterator chn2 = fChain2Counter[index].begin(); 
	 chn2 != fChain2Counter[index].end();++chn2) {
	  
      unsigned int counter2 = chn2->second;
      
      if(nocounts.find(counter2)!=nocounts.end()) continue;

      counter_doublet trgs(counter1,counter2);	 
      double overlap12 = 0;	
      if (fCounters[index].find(trgs)!=fCounters[index].end()) {
	overlap12 = fCounters[index][trgs];
      }
      
      if (chn1_tot>0.0) {
	overlap12/=chn1_tot;
	overlap12*=100.0;
      } else {
	overlap12 = 0.0;
      }

      // write the row
      

      if (overlap12<0.0001){
	nover.insert(chn2->second);
      }
      else{
	cs << "<tr>"
	   << "<td align=\"left\"> "   << chn2->first                   << " </td>";
	cs << "<td align=\"center\"> " << Form("%3.1f",overlap12)  << " </td>";
      }
      cs << "</tr>" << endl;
	 	  
    }

    // end table
    cs << "</table>" << endl;

    //chains with no overlap
    cs << endl << endl
       << "Chains with no overlap to " << chn1->first << endl;
    for(set<unsigned>::const_iterator ch = nover.begin(); ch!=nover.end(); ++ch){

      cs << "   " << fCounter2Chain[index][*ch] << endl;
    }
    
    cs << "</pre>" << endl;
  } 
   
  // Chains without any counts
  os << endl << "Chains with no counts:" << endl;
  for(set<unsigned>::const_iterator nch = nocounts.begin(); nch!=nocounts.end(); ++nch){

    os << "   " << fCounter2Chain[index][*nch] << endl;
  }

  os << "</pre>" << endl;
}

//---------------------------------------------------------------------------------------
double Anp::OverlapMon::GetEffectivePS(const unsigned level, const unsigned counter) {
  //
  // Calculate the effective PS 
  //
  double ps=fPrescale[level-1][counter];
  double ps_lower = 0.0;
  const set<string> &ids = fLowerChains[level-1][counter];
  if (fDebugDetails) cout << "Len: " << ids.size() << endl;
  
  for(set<string>::const_iterator c1 = ids.begin(); c1 != ids.end();++c1){
    unsigned count = fChain2Counter[level-2][*c1];
    if (fDebugDetails){
      cout << "Counter: " << counter << " Level: " << level << " input: " << *c1 << " counter: " << count << endl;
      cout << "PS1: " << ps_lower << " PS2: " << fPrescale[level-2][count]  << endl;
    }
    ps_lower=TriggerOverlap(ps_lower,fPrescale[level-2][count]);

  }

  return ps*ps_lower;
}

//---------------------------------------------------------------------------------------
double Anp::OverlapMon::TriggerOverlap(const double ps1, const double ps2) {
  if(!(ps1>0.0) && !(ps2>0.0)) return 0.0;
  if(!(ps1>0.0)) return ps2;
  if(!(ps2>0.0)) return ps1;
  return 1.0/(1.0/ps1 + 1.0/ps2 -1.0/( ps1*ps2));
}

//---------------------------------------------------------------------------------------
void Anp::OverlapMon::WriteChnWithPS(HtmlNode &inode, const unsigned index, const string key)
{
  //
  // Writing an overlap table chain by chain
  //
  if(index>2){ 
    cout << "OverlapMon::WriteChnWithPS - Error index out of range" << endl;
    return;
  }

  // Make sure that we have chain counts
  if(fChain2Counter[index].size()==0) return;

  // map from overlap table to chain id
  std::map<unsigned, Handle<HtmlNode> > m_chn_olap;

  Handle<HtmlNode> anode = inode.AddNode("OverlapMon_chn_"+key+".html", "Trigger Overlaps for "+key+" Chains <br>");

  ostream &os = anode->GetCont();

  os << "<pre>" << endl;
  
  os << "Overlap tables for the Following Chains: " << endl;
  os << "<style type=\"text/css\">"<< endl
     << "table,  th {border:1px solid black;}"<< endl
     << "th { padding:10px; background-color:white; color:black; }"<< endl
     << "td {padding-left:15px;padding-right:15px;}"<< endl
     << "</style>"<< endl
     << "<script src=\"sorttable.js\"></script>" << endl
     << "<table border=1 frame=box rules=cols class=\"sortable\">" << endl
     << "<tr><th>Chain Name</th>";
  if(fWithPS){
    os << "<th>Prescale</th>";
  }
  os << "</tr>"<< endl;

  // Chains without overlap
  set<unsigned> nover, nocounts;

  // write column headers - for chain with overlap
  for(str2int_map::const_iterator chn1 = fChain2Counter[index].begin(); 
      chn1 != fChain2Counter[index].end();++chn1){
    
    // get total chain counts for chn1 to normalize the column
    counter_doublet trgs_norm(chn1->second,chn1->second);
    double chn1_tot_raw = -1.0;
    //double chn1_tot_pred = -1.0; //Variable not used [tamartin 2014-01-13]
    const unsigned counter = chn1->second;

    if(fWithPS && fScaleCount[index].find(trgs_norm)!=fScaleCount[index].end()){
      //chn1_tot_pred = fScaleCount[index][trgs_norm]; //Variable not used [tamartin 2014-01-13]
    }//fScaleCount
    if (fCounters[index].find(trgs_norm)!=fCounters[index].end()) {
	chn1_tot_raw = fCounters[index][trgs_norm];
    }
    
    float ps = fPrescale[index][counter];
    if (fDebugDetails)    cout << "First test! counter: " << counter << " ps: " << ps << " levl: " << index << " name:  " << chn1->first << endl;
    // create a chain overlap table  when there are chain counts
    if((chn1_tot_raw > 0.0 && !fWithPS) || ((ps==1.0) && fWithPS)){ // || ps ==-1.0
      
      stringstream chn_path; 
      chn_path << "OverlapMon_chn" << key << "_" << chn1->second << ".html";
      
      Handle<HtmlNode> trigo(new HtmlNode(chn_path.str(),HtmlNode::kHTML));

      m_chn_olap[counter] = trigo; //anode->AddNode(chn_path.str(), "Trigger Overlap for "+chn1->first+" <br>");

      anode->AddChild(trigo);

      ostream &cs = trigo->GetCont();

      cs << "<pre>" << endl;
      cs << "Numbers in table are percentage of overlap with the chain " << chn1->first << endl;
      cs << "<style type=\"text/css\">"<< endl
	 << "table,  th {border:1px solid black;}"<< endl
	 << "th { padding:10px; background-color:white; color:black; }"<< endl
	 << "td {padding-left:15px;padding-right:15px;}"<< endl
	 << "</style>"<< endl
	 << "<script src=\"sorttable.js\"></script>" << endl
	 << "<table border=1 frame=box rules=cols class=\"sortable\">" << endl
	 << "<tr><th>Chain Name</th>";

      if(fWithPS){
	cs << "<th>Raw Overlap (%)</th><th> Overlap Accounting for PS and Luminosity (%)</th><th>Prescale</th></tr>" << endl;

      }
      else cs << "<th>Overlap (%)</th></tr>" << endl;

      // add link to chn1 overlap table
      os << "<tr>"
	 << "<td align=\"left\">" << trigo->GetLink(chn1->first) << "</td>";
      
      if(fWithPS){
	os << "<td>" << ps << "</td>";
      }
      os << "</tr>" << endl;

    }
    else nocounts.insert(chn1->second);
  }

  os << "</table>" << endl;
  
  // table body, outer loop = rows
  for (str2int_map::const_iterator chn1 = fChain2Counter[index].begin(); 
       chn1 != fChain2Counter[index].end();++chn1) {
    
    const unsigned int counter1 = chn1->second;
   
    if(nocounts.find(counter1)!=nocounts.end()) continue;

    float ps1 = fPrescale[index][counter1];

    if(ps1!=1.0 && fWithPS) continue;
 
    ostream &cs = m_chn_olap[counter1]->GetCont();

    // get total chain counts for chn1 to normalize the column
    counter_doublet trgs_norm(counter1,counter1);
    double chn1_tot_raw = -1.0;
    double chn1_tot_pred = -1.0;
    if(fWithPS && fScaleCount[index].find(trgs_norm)!=fScaleCount[index].end()){
      chn1_tot_pred = fScaleCount[index][trgs_norm];
    }
    if (fCounters[index].find(trgs_norm)!=fCounters[index].end()) {
	chn1_tot_raw = fCounters[index][trgs_norm];
    }
    
    // table body, inner loop = entries (columns)
    for (str2int_map::const_iterator chn2 = fChain2Counter[index].begin(); 
	 chn2 != fChain2Counter[index].end();++chn2) {
	  
      unsigned int counter2 = chn2->second;
      
      if(nocounts.find(counter2)!=nocounts.end()) continue;

      counter_doublet trgs(counter1,counter2);	 
      double overlap12 = 0.0, overlap12_raw=0.0;
      if(fWithPS && fScaleCount[index].find(trgs)!=fScaleCount[index].end()){
	overlap12 = fScaleCount[index][trgs];
      }
      if (fCounters[index].find(trgs)!=fCounters[index].end()) {
	overlap12_raw = fCounters[index][trgs];
      }
      
      if (chn1_tot_pred>0.0) {
	overlap12/=chn1_tot_pred;
	overlap12*=100.0;
      }
      else{
	overlap12     = 0.0;
      }

      if (chn1_tot_raw>0.0) {
	overlap12_raw/=chn1_tot_raw;
	overlap12_raw*=100.0;
      } else {
	overlap12_raw = 0.0;
      }

      // write the row
      float ps2 = fPrescale[index][counter2];
      if (fDebugDetails) cout << "Counter : " << counter2 << " ps2: " << ps2 << " level -1: " << index << " name: " << chn2->first << endl;
      //      if(ps2==-1.0) ps2 = 1.0;
      if(ps2==0.0) ps2 = -1.0;

      if (overlap12<0.0001 && overlap12_raw<0.0001){
	nover.insert(chn2->second);
      }
      else{
	cs << "<tr>"
	   << "<td align=\"left\"> "   << chn2->first                   << " </td>";
	cs << "<td align=\"center\"> " << Form("%3.1f",overlap12_raw)   << " </td>";
	if(fWithPS){
	  cs << "<td align=\"center\"> " << Form("%3.1f",overlap12/ps2)  << " </td>"
	     << "<td align=\"center\"> " << ps2                          << " </td>";
	}
      }
      cs << "</tr>" << endl;
	 	  
    }

    // end table
    cs << "</table>" << endl;

    //chains with no overlap
    cs << endl << endl
       << "Chains with no overlap to " << chn1->first << endl;
    for(set<unsigned>::const_iterator ch = nover.begin(); ch!=nover.end(); ++ch){

      cs << "   " << fCounter2Chain[index][*ch] << endl;
    }
    
    cs << "</pre>" << endl;
  } 
   
  // Chains without any counts
  os << endl << "Chains with no counts:" << endl;
  for(set<unsigned>::const_iterator nch = nocounts.begin(); nch!=nocounts.end(); ++nch){

    os << "   " << fCounter2Chain[index][*nch] << endl;
  }

  os << "</pre>" << endl;
}
