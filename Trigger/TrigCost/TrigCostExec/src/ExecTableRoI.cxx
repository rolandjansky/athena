/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>

// ROOT
#include "TCanvas.h"
#include "TDirectory.h"
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"

// Local
#include "TrigCostBase/Factory.h"
#include "TrigCostBase/HistMan.h"
#include "TrigCostBase/Ptr.h"
#include "TrigCostBase/UtilTrig.h"

// Local
#include "TrigCostExec/Util.h"
#include "TrigCostExec/ExecCount.h"
#include "TrigCostExec/ExecCountRoI.h"
#include "TrigCostExec/ExecTableRoI.h"

REGISTER_ANP_OBJECT(ExecClient,ExecTableRoI)

using namespace std;

//--------------------------------------------------------------------------------------      
Anp::ExecTableRoI::TrigTimeSum::TrigTimeSum() :
  name(""),
  hRoITime(0),
  hLBAvgRoITime(0),
  hLBTime(0)
{
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableRoI::TrigTimeSum::AddRoITime(const unsigned &lb,const double &time, const bool MakeRoIPlot)
{
  //adding the algorithm time from one RoI
  if(hRoITime) hRoITime->Fill(time);

  //
  //creating a lb if it does not exist
  //
  if(MakeRoIPlot){
    if(lb>=LBTimeAndRoI.size()){
      
      RoIPair *roiTime = new RoIPair();
      roiTime->first  = 0.0;
      roiTime->second = 0;
      
      LBTimeAndRoI.insert(LBTimeAndRoI.end(),1+lb-LBTimeAndRoI.size(),*roiTime);
    } 
    
    if(lb>=LBTimeAndRoI.size()) cout << "ExecTableRoI::TrigTimeSum::AddRoITime - lb time too big" << endl;

    //adding RoI time to LB pair 
    RoIPair &roit = LBTimeAndRoI.at(lb);
    
    roit.first += time;
    roit.second+=1;

  }
  
}

//--------------------------------------------------------------------------------------      
Anp::ExecTableRoI::ExecTableRoI()
  :fDir(0),
   fDirAlg(0),
   fDirChn(0),
   fCanvas(0),
   fDebug(false),
   fMakeHist(false), 
   fMakeRoIPlot(false),
   fDirName(),
   fHistKey(""),
   fAlgLink(""),
   fChnLink(""),
   hNRoIPerCall(0),
   hNRoIPerEvent(0),
   hAlgTimePerRoI(0),
   hLBnRoI(0)
{
}

//--------------------------------------------------------------------------------------      
Anp::ExecTableRoI::~ExecTableRoI()
{
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableRoI::SetReg(const Registry &reg)
{
  //
  // Copy configuration
  //
  fReg = reg;
  
  //
  // Read configuration
  //
  reg.Get("Debug",      fDebug);
  reg.Get("Level",      fLevel);
  reg.Get("HistKey",    fHistKey);
  reg.Get("HistOpt",    fHistOpt);
  reg.Get("MakeHist",   fMakeHist);
  reg.Get("DirName",    fDirName);
  reg.Get("ClientLink", fClientLink);
  reg.Get("AlgDir",     fAlgLink);
  reg.Get("ChnDir",     fChnLink);
  reg.Get("MakeRoIPlot",fMakeRoIPlot);

  if(fMakeHist) {
    fCanvas = new TCanvas(GetClientName().c_str(), "", 650, 400);
  } 
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableRoI::SetDir(TDirectory *dir)
{
  //
  // Set directory for histograms
  //
  fDir    = Anp::GetDir(dir, fDirName);
  if(dir){
    cout << "ExecTableRoI::SetDir - TDirectory is not defined" << endl;
    fDirAlg = Anp::GetDir(dir->GetMotherDir(), fAlgLink);
    fDirChn = Anp::GetDir(dir->GetMotherDir(), fChnLink);
  }

  //
  // Create histograms
  //
  hNRoIPerCall   = GetTH1("nroi_call");
  hNRoIPerEvent  = GetTH1("nroi_event");
  hAlgTimePerRoI = GetTH1("alg_time_per_roi");
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableRoI::ProcessAlg(const Exec::AlgCall &alg)
{

  //
  // Create unique set of roi ids
  //
  std::set<uint32_t> rois;
  
  for(unsigned i = 0; i < alg.roi_data.size(); ++i) {
    const Exec::ROILoad &roi = alg.roi_data.at(i);
    
    if(!rois.insert(roi.roi_id).second) {
      log() << "ProcessAlg - duplicate roi: " << roi.roi_id << endl;
    }
    //
    // Store alg call for each unique Roi
    //
    fSingleRoIMap[roi.roi_id].push_back(alg);
  }

  //
  // Store algorithm calls within each unique roi id set
  //
  fRoIMap[rois].push_back(alg);
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableRoI::FinishEvent(const Exec::Event &event)
{
  //
  // Make plots per RoI
  //
  std::set<uint32_t> event_rois;
  vector<ExecClient *> event_clients;

  //
  // Loop over unique roi ids
  //
  for(SingleRoIMap::const_iterator rit = fSingleRoIMap.begin(); rit != fSingleRoIMap.end(); ++rit) {
    const std::vector<Exec::AlgCall> &algs   = rit->second;
    double roi_alg_time = 0.0;
    
    for(std::vector<Exec::AlgCall>::const_iterator call = algs.begin(); call !=algs.end(); ++call){ 

      roi_alg_time += call->alg_data.getTimer();

      //
      // Filling a map of RoI alg time for each algorithm and chain
      //
      if(call->alg_data.isCalled()){ 

	RoITimeMap::iterator ralg = fAlgRoItMap.find(call->alg_conf->getNameId());
	RoITimeMap::iterator rchn = fChnRoItMap.find(call->chn_conf->getId());

	if(ralg == fAlgRoItMap.end()){ 
	  pair<string,double>  *palg = new pair<std::string,double>(call->alg_conf->getName(),0.0);
	  ralg = fAlgRoItMap.insert(RoITimeMap::value_type(call->alg_conf->getNameId(),*palg)).first;
	}

	if(rchn == fChnRoItMap.end()){ 
	  pair<string,double>  *pchn = new pair<std::string,double>(call->chn_conf->getName(),0.0);
	  rchn = fChnRoItMap.insert(RoITimeMap::value_type(call->chn_conf->getId(),*pchn)).first;
	}

	(ralg->second).second+=call->alg_data.getTimer();
	(rchn->second).second+=call->alg_data.getTimer();
      }
    }

    hAlgTimePerRoI->Fill(roi_alg_time);

    //
    // Filling histograms for each alg called in this RoI
    //
    for(RoITimeMap::const_iterator atime = fAlgRoItMap.begin(); atime != fAlgRoItMap.end(); ++atime){

	TrigTimeSum* alg = GetTrigTimeSum(atime->first,"ALG",(atime->second).first);

	if(alg){
	  alg->AddRoITime(event.lb,atime->second.second,fMakeRoIPlot);
	}
	else cout << "ExecTableRoI::FinishEvent - ERROR failed to create a new alg TrigTimeSum" << endl;
    }

    for(RoITimeMap::const_iterator ctime = fChnRoItMap.begin(); ctime != fChnRoItMap.end(); ++ctime){

	TrigTimeSum* chn = GetTrigTimeSum(ctime->first,"CHN",(ctime->second).first);

	if(chn) chn->AddRoITime(event.lb,ctime->second.second,fMakeRoIPlot);
	else cout << "ExecTableRoI::FinishEvent - ERROR failed to create a new chn TrigTimeSum" << endl;
    }

    fAlgRoItMap.clear();
    fChnRoItMap.clear();
  }

  //
  // Loop over unique roi id sets
  //
  for(RoIMap::const_iterator rit = fRoIMap.begin(); rit != fRoIMap.end(); ++rit) {
    const std::set<uint32_t>         &rois = rit->first;
    const std::vector<Exec::AlgCall> &algs = rit->second;

    //
    // Create list of unique RoIs
    //
    event_rois.insert(rois.begin(), rois.end());
    
    //
    // Fill per call histograms
    //
    if(hNRoIPerCall) hNRoIPerCall->Fill(rois.size());

    //
    // Process individual algorithm for current RoI set
    //
    for(unsigned i = 0; i < algs.size(); ++i) {
      const Exec::AlgCall &alg = algs.at(i);
      
      Clients &c = FindClients(rois, alg);

      //
      // Fill histograms per RoI 
      //
      if(c.roi_client.valid()) {
	c.roi_client->ProcessAlg(alg);
	event_clients.push_back(c.roi_client.get());
      }
      
      //
      // Fill RoI counts
      //
      if(alg.roi_data.size()==rois.size() && alg.roi_data.size()==1){
	unsigned th = unsigned(alg.roi_data.at(0).roi_th);
	if(fDebug) log() << "threshhold: " << th << endl;
	if(th>= c.th_clients.size()){

	  for(unsigned jk=c.th_clients.size(); jk<th+1;++jk){
	    THClients *new_th_client = new THClients();
	    stringstream roi_th_str;
	    roi_th_str << "th" << jk;
	    
	    // Hack need to fix
	    new_th_client->roi_client = BookClient(c.roi_client->GetClientName(), 0x0, Anp::GetDir(fDir, c.roi_client->GetClientName()+"/"+roi_th_str.str()));
	    //new_th_client->hRoISize   = MakeLBHist("roi_size_vs_lb", "RoI Size", exlbs.first, exlbs.second, Anp::GetDir(fDirAlg, algname));
	    
	    c.th_clients.push_back(*new_th_client);
	    
	  }
	  if(fDebug) log() << "FinishEvent - vector out of range!" << endl; 
	}


	THClients &roiClient = c.th_clients.at(th);
	roiClient.roi_client->ProcessAlg(alg);
	event_clients.push_back(roiClient.roi_client.get());

	if(event.lb>=roiClient.RoIThreshCounts.size()){

	  RoICounts *new_rcounts = new RoICounts();
	  roiClient.RoIThreshCounts.insert(roiClient.RoIThreshCounts.end(),1+event.lb-roiClient.RoIThreshCounts.size(),*new_rcounts);
	}

	RoICounts &rcounts = roiClient.RoIThreshCounts.at(event.lb);
	++rcounts.counts;
	rcounts.size += alg.roi_data.at(0).roi_area;
      }

      //
      // Fill histograms per RoI and detector group
      //
      set<uint32_t> det_ids;
      
      for(set<uint32_t>::const_iterator dit = alg.det_ids.begin(); dit != alg.det_ids.end(); ++dit) {
	det_ids.insert(*dit & 0xf);
      }

      for(map<unsigned, Handle<ExecClient> >::iterator dit = c.det_clients.begin(); dit != c.det_clients.end(); ++dit) {	
	const uint32_t det = dit->first;
	Handle<ExecClient> &d = dit->second;
	
	if(det_ids.count(det) && d.valid()) {
	  d->ProcessAlg(alg);
	  event_clients.push_back(d.get());
	}
      }
    }
  }

  //
  // Fill the number of RoIs and add one event
  //
  AddRoICounts(event.lb,event_rois.size());

  //
  // Fill per event histograms
  //
  if(hNRoIPerEvent)       hNRoIPerEvent      ->Fill(event_rois.size());

  //
  // Cleanup
  //
  fRoIMap.clear();
  fSingleRoIMap.clear();

  //
  // Remove duplicate clients
  //
  std::sort(event_clients.begin(), event_clients.end());
  vector<ExecClient *>::iterator citer = std::unique(event_clients.begin(), event_clients.end());
  event_clients.erase(citer, event_clients.end());

  //
  // Process event for active clients
  //
  for(unsigned i = 0; i < event_clients.size(); ++i) {
    ExecClient *client = event_clients.at(i);
    if(client) {
      client->FinishEvent(event);
    }
  }
}

//--------------------------------------------------------------------------------------      
const Anp::Exec::Count& Anp::ExecTableRoI::GetCount() const
{
  return fCount;
}

//--------------------------------------------------------------------------------------
void Anp::ExecTableRoI::WriteExec(HtmlNode &inode, HtmlMenu &, const Exec::Summary &sum)
{
  //
  // Write out results for entire run (regardless of lumi block)
  //
  ostream &os = inode.GetCont();

  //
  // Print histograms if requested
  //
  if(!fMakeHist) return;

  //
  // Set alg histogram titles
  //
  pair<unsigned,unsigned> exlbs = GetMaxLB(sum);

  for(SingleRoITrigMap::iterator lbtrig = fAlgRoIMap.begin(); lbtrig != fAlgRoIMap.end(); ++lbtrig){
    
    const string algname = lbtrig->second.name; //sum.GetAlgName(lbtrig->first);

    //
    // Create Histgrams for RoI time
    //
    if(fMakeRoIPlot){
      lbtrig->second.hLBTime       = MakeLBHist("alg_time_vs_lb",         "Algorithm Time (ms)",          exlbs.first, exlbs.second, Anp::GetDir(fDirAlg, algname));
      lbtrig->second.hLBAvgRoITime = MakeLBHist("alg_avg_roi_time_vs_lb", "Avg RoI Algorithm Time (ms)",  exlbs.first, exlbs.second, Anp::GetDir(fDirAlg, algname));
    }

    const vector<RoIPair> &lbs = lbtrig->second.LBTimeAndRoI;
    // Loop over lbs with algorithm time and number of calls
    // Also fill chain structure
    for(unsigned lb = 0;lb < lbs.size(); ++lb){

      //if((lb < exlbs.first) || (lb > exlbs.second)) continue;

      // Fill time from this LB
      double nRoI    = double(lbs.at(lb).second);
      if(lbtrig->second.hLBTime) lbtrig->second.hLBTime->SetBinContent(lb, lbs.at(lb).first);
      if(!(nRoI>0)) nRoI=1.0;
	
      if(lbtrig->second.hLBTime) lbtrig->second.hLBTime->SetBinError(lb, lbs.at(lb).first/(TMath::Sqrt(nRoI)));

      if(nRoI > 0){ 

	double avgtime = (lbs.at(lb).first)/nRoI;

	if(lbtrig->second.hLBAvgRoITime){
	  lbtrig->second.hLBAvgRoITime->SetBinContent(lb,avgtime);
	  lbtrig->second.hLBAvgRoITime->SetBinError  (lb,avgtime/(TMath::Sqrt(nRoI)));
	}
      }
    }
  }

  //create chain histograms
  for(SingleRoITrigMap::iterator lbtrig = fChnRoIMap.begin(); lbtrig != fChnRoIMap.end(); ++lbtrig){
    
    const string chnname = lbtrig->second.name; //sum.GetChnName(lbtrig->first);

    //
    // Create Histgrams
    //
    if(fMakeRoIPlot){
      lbtrig->second.hLBTime       = MakeLBHist("chn_time_vs_lb",         "Algorithm Time (ms)",          exlbs.first, exlbs.second, Anp::GetDir(fDirChn, chnname));
      lbtrig->second.hLBAvgRoITime = MakeLBHist("chn_avg_roi_time_vs_lb", "Avg RoI Algorithm Time (ms)",  exlbs.first, exlbs.second, Anp::GetDir(fDirChn, chnname));
    }
    const vector<RoIPair> &lbs = lbtrig->second.LBTimeAndRoI;

    for(unsigned lb = 0;lb < lbs.size(); ++lb){

      //if((lb < exlbs.first) || (lb > exlbs.second)) continue;

      //Filling with the LB time
      double nRoI    = double(lbs.at(lb).second);
      if(nRoI<=0) nRoI=1.0;
      if(lbtrig->second.hLBTime && nRoI>0){
	lbtrig->second.hLBTime->Fill(lb, lbs.at(lb).first);
	lbtrig->second.hLBTime->Fill(lb, lbs.at(lb).first/(TMath::Sqrt(nRoI)));
      }

      if(nRoI > 0){ 

	double avgtime = (lbs.at(lb).first)/nRoI;

	if(lbtrig->second.hLBAvgRoITime){
	  lbtrig->second.hLBAvgRoITime->SetBinContent(lb,avgtime);
	  lbtrig->second.hLBAvgRoITime->SetBinError  (lb,avgtime/(TMath::Sqrt(nRoI)));
	}
      }
    }
  }

  //
  // Fill Number of RoIs per event histogram
  //
  hLBnRoI = MakeLBHist("evt_nroi_vs_lb", "Avg Number RoI/Event", 0, fRoICount.size(), fDir);

  if(hLBnRoI){
    for(unsigned lb = 0; lb<fRoICount.size(); ++lb){
      float avg_nroi = 0.0;
      const RoICount &roiC = fRoICount.at(lb);

      if(roiC.first!=0) avg_nroi = static_cast<float>(roiC.second)/ static_cast<float>(roiC.first);

      hLBnRoI->Fill(lb,avg_nroi);
    }
  }

  string base = inode.GetBase()+"_";
  string suff;

  if     (fHistOpt.find("png") != string::npos) suff = ".png";
  else if(fHistOpt.find("eps") != string::npos) suff = ".eps";

  if(fCanvas) {
    fCanvas->cd();
  }

  os << endl << "Histograms filled per RoI:" << endl; 

  if(hNRoIPerCall)   Anp::PrintHist(inode, hNRoIPerCall, fCanvas, base+"nroi_per_call"+suff,
				    "  Number of RoIs per Alg Call", fHistOpt);
  if(hNRoIPerEvent)  Anp::PrintHist(inode, hNRoIPerEvent, fCanvas, base+"nroi_per_event"+suff,
				    "  Number of RoIs per Event", fHistOpt);
  if(hAlgTimePerRoI) Anp::PrintHist(inode, hAlgTimePerRoI, fCanvas, base+"alg_time_per_roi"+suff,
				    "  Algorithm Time per RoI", fHistOpt);
}

//--------------------------------------------------------------------------------------
Anp::ExecTableRoI::Clients& Anp::ExecTableRoI::FindClients(const std::set<uint32_t> &rois, 
							   const Exec::AlgCall &alg)
{
  //
  // Find histograms for matching 
  //
  unsigned roi_key = 0;
  //  unsigned roi_th  = 0;
  TrigMonRoi::Type roi_type = TrigMonRoi::kNone;

  if(rois.size() == 0) { 
    roi_key = 300;
  }
  else if(rois.size() > 1) {
    roi_key = 400;
  }
  else if(alg.roi_data.size() == rois.size()) {
    //
    // Single roi - read roi type
    //
    const Exec::ROILoad &roi = alg.roi_data.back();
    if(roi.roi_id != *rois.begin()) {
      log() << "FindClients - logic error for RoI id" << endl;
      static Clients c;
      return c;
    }
    
    roi_key  = roi.roi_type;
    roi_type = roi.roi_type;
    //   roi_th   = unsigned(roi.roi_th);
  }
  else {
    log() << "FindClients - logic error for RoI count" << endl;
    static Clients c;
    return c;
  }

  ClientMap::iterator cit = fClients.find(roi_key);
  if(cit == fClients.end()) {
    //
    // Make new clients
    //
    string roi_str;
    
    if     (roi_key == 300) roi_str = "roi_zero";
    else if(roi_key == 400) roi_str = "roi_many";
    else                    roi_str = "roi_"+AsStringRoIType(roi_type);
    
    Clients cs;
    cs.roi_client       = BookClient(roi_str, 0x0, fDir);
    cs.det_clients[0x1] = BookClient(roi_str, 0x1, fDir);
    cs.det_clients[0x2] = BookClient(roi_str, 0x2, fDir);
    cs.det_clients[0x3] = BookClient(roi_str, 0x3, fDir);
    cs.det_clients[0x4] = BookClient(roi_str, 0x4, fDir);
    cs.det_clients[0x5] = BookClient(roi_str, 0x5, fDir);
    cs.det_clients[0x6] = BookClient(roi_str, 0x6, fDir);
    cs.det_clients[0x8] = BookClient(roi_str, 0x8, fDir);

    //
    // adding RoI threshold Counts - there are 8 allowable thresholds
    //
    for(unsigned i=0; i<8; ++i){
      THClients *new_th_client  = new THClients();

      // Add threshold directory
      stringstream roi_th_str;
      roi_th_str << "th" << i;

      new_th_client->roi_client = BookClient(roi_str, 0x0, Anp::GetDir(fDir, roi_str+"/"+roi_th_str.str()));
      //new_th_client->hRoISize   = MakeLBHist("roi_size_vs_lb", "RoI Size", exlbs.first, exlbs.second, Anp::GetDir(fDirAlg, algname));

      cs.th_clients.push_back(*new_th_client);
    }

    cit = fClients.insert(ClientMap::value_type(roi_key, cs)).first;
  }

  return cit->second;
}

//--------------------------------------------------------------------------------------
Anp::Handle<Anp::ExecClient> Anp::ExecTableRoI::BookClient(string name, unsigned detector, TDirectory *dir) const
{
  //
  // Make one client
  //
  
  Registry reg(fReg);
  reg.Set("DoAlg", false);
  reg.Set("DoDet", false);
  reg.Set("DoROS", false);

  if(detector > 0) {
    name = name + "_" + Anp::AsStringDetType(detector);
  }
  else if(detector==0){
    reg.Set("DoAlg", true);
    reg.Set("MakeHist","yes");
  }

  if(fDebug) {
    log() << "BookClient - new client: " << name << endl;
  }
  
  Handle<ExecClient> client(new ExecCount());
    
  client->SetClientName(name);
  client->SetClientType("");
  client->SetReg(reg);
  client->SetDir(Anp::GetDir(dir, name));

  return client;
}

//--------------------------------------------------------------------------------------      
Anp::ExecClient* Anp::ExecTableRoI::GetRoIClient(const TrigConfChain &chn_conf)
{
  //
  // Make client for chain
  //
  unsigned counter = chn_conf.getEncodedId();

    Registry reg(fReg);
    reg.Set("DoAlg", false);
    reg.Set("DoDet", false);
    reg.Set("DoROS", false);
    reg.Set("DoChn", false);
    reg.Set("DoRoi", true);
    reg.Set("ChnId", counter);

    if(fDebug) {
      cout << "ExecTableChn::GetChnClient - " << chn_conf.getName() << endl;
    }

    stringstream link_str;
    link_str << "CHN_NAME_" << setw(10) << setfill('0') << counter;

    Handle<ExecClient> client(new ExecCountRoI());
    
    client->SetClientName(chn_conf.getName());
    client->SetClientLink(link_str.str());
    client->SetClientType("chain");
    client->SetReg(reg);
    client->SetDir(Anp::GetDir(fDir, "chn/"+chn_conf.getName()));

    fRoI = client;
    return fRoI.get();
}

//---------------------------------------------------------------------------------------
Anp::ExecTableRoI::TrigTimeSum* Anp::ExecTableRoI::GetTrigTimeSum(const unsigned &trigid, const string &type, const string &name)
{
  SingleRoITrigMap::iterator trig = fAlgRoIMap.end();

  if(type=="ALG"){
    
    trig = fAlgRoIMap.find(trigid);
    
    if(trig == fAlgRoIMap.end()){
      
      TrigTimeSum *sum = new TrigTimeSum();
      
      sum->name = name;

      if(sum) sum->hRoITime = GetTrigTH1(type,name); 
      else cout << "ExecTableRoI::GetTrigTimeSum - ERROR failed to create a new TrigTimeSum" << endl;

      trig = fAlgRoIMap.insert(SingleRoITrigMap::value_type(trigid,*sum)).first;
    }
  }

  else if(type=="CHN"){

    trig = fChnRoIMap.find(trigid);
    
    if(trig == fChnRoIMap.end()){
      
      TrigTimeSum *sum = new TrigTimeSum();

      sum->name = name;

      if(sum) sum->hRoITime = GetTrigTH1(type,name); 
      else cout << "ExecTableRoI::GetTrigTimeSum - ERROR failed to create a new TrigTimeSum" << endl;

      trig = fChnRoIMap.insert(SingleRoITrigMap::value_type(trigid,*sum)).first;
    }
  }

  return &(trig->second);
}

//---------------------------------------------------------------------------------------
TH1* Anp::ExecTableRoI::GetTrigTH1(const string &type, const string &name) const
{ 
  //
  // Create histogram
  //
  TH1 *h = new TH1F("alg_time_per_roi", name.c_str(),200, 0,-1);
  if(type=="ALG"){  
    h->SetDirectory(Anp::GetDir(fDirAlg, name));
  }
  else if(type=="CHN"){  
    h->SetDirectory(Anp::GetDir(fDirChn, name));
  }

  h->GetXaxis()->CenterTitle();
  h->GetXaxis()->SetTitle("Algorithm Time (ms) per RoI");
  h->GetYaxis()->CenterTitle();
  h->GetYaxis()->SetTitle("#RoI");  

  return h;
}

//---------------------------------------------------------------------------------------
TH1* Anp::ExecTableRoI::GetTH1(const std::string &key, const std::string &name) const
{
  //
  // Histograms are owned by TDirectory
  //
  TH1 *h = Anp::HistMan::Instance().CreateTH1(key, fHistKey);

  if(h && !name.empty()) {
    h->SetName(name.c_str());
  }

  Anp::SetDir(h, fDir);

  return h;
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableRoI::AddRoICounts(const unsigned &lb, const unsigned nRoI)
{
  //
  // Adds the RoI counts for lumi block
  //
  if(lb>=fRoICount.size()){
      
    RoICount *roiSum = new RoICount();
    
    fRoICount.insert(fRoICount.end(),1+lb-fRoICount.size(),*roiSum);
  }

  if(lb>=fRoICount.size()) log() << "AddRoICounts - too many lb" << endl;

  RoICount &thisLB = fRoICount.at(lb);

  // Add to the event count
  ++thisLB.first;

  // Add to the roi count
  thisLB.second+=nRoI;
}

//--------------------------------------------------------------------------------------      
std::ostream& Anp::ExecTableRoI::log() const
{
  std::cout << GetClientName() << "::";
  return std::cout;
}
