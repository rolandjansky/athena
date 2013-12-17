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
#include "TrigCostBase/UtilTrig.h"
#include "TrigCostBase/ScaleABC.h"

// Local
#include "TrigCostExec/ExecCount.h"
#include "TrigCostExec/ExecTablePred.h"

REGISTER_ANP_OBJECT(ExecClient,ExecTablePred)

using namespace std;

//-----------------------------------------------------------------------------------------
Anp::ExecTablePred::pCount::pCount() :
      alg_id(0),
      det_id(0),
      ros_id(0),
      roi_id(0),
      chn_id(0),
      ros_name(""),
      select_det(false),
      select_ros(false),
      select_roi(false),
      limit_slow(0.0),
      nevent(0),
      nevent_pred(0.0),
      nevent_pred_err(0.0),
      nroi(0),
      timer_alg(0.0),
      timer_rob(0.0),  
      alg_ncalls(0.0),
      alg_ncache(0.0),
      alg_nslow(0.0),      
      rob_nreq(0.0),
      rob_nreq_cache(0.0),
      rob_nreq_retrv(0.0),
      rob_nreq_other(0.0),
      rob_size(0.0),	  
      rob_size_cache(0.0),
      rob_size_retrv(0.0),
      rob_size_other(0.0),
      rob_nids(0.0),	  
      rob_nids_cache(0.0),
      rob_nids_retrv(0.0),
      rob_nids_other(0.0),
      ros_nreq(0.0),	  
      ros_nreq_cache(0.0),
      ros_nreq_retrv(0.0),
      ros_nreq_other(0.0),
      ros_nreq_err(0.0),
      ros_nreq_retrv_err(0.0),
      ros_size_retrv_err(0.0)
      {
      }

//-----------------------------------------------------------------------------------------
Anp::ExecTablePred::rCount::rCount(){}

//-----------------------------------------------------------------------------------------
void Anp::ExecTablePred::pCount::Clear()
{
  nevent     = 0;
  nevent_pred= 0.0;
  nevent_pred_err=0.0;
  nroi       = 0;
  timer_alg  = 0.0;
  timer_rob  = 0.0;
  alg_ncalls = 0.0;
  alg_ncache = 0.0;
  alg_nslow  = 0.0;      
  rob_nreq   = 0.0;
  rob_nreq_cache = 0.0;
  rob_nreq_retrv = 0.0;
  rob_nreq_other = 0.0;
  rob_size       = 0.0;
  rob_size_cache = 0.0;
  rob_size_retrv = 0.0;
  rob_size_other = 0.0;
  rob_nids       = 0.0;
  rob_nids_cache = 0.0;
  rob_nids_retrv = 0.0;
  rob_nids_other = 0.0;
  ros_nreq       = 0.0;
  ros_nreq_cache = 0.0;
  ros_nreq_retrv = 0.0;
  ros_nreq_other = 0.0;
  ros_nreq_err       = 0.0;
  ros_nreq_retrv_err = 0.0;
  ros_size_retrv_err = 0.0;
  //ros_nrob.clear();
  //  ros_nrob_retrv.clear();
}

//-----------------------------------------------------------------------------------------
void Anp::ExecTablePred::pCount::AddCount(const Anp::Exec::Count &rhs, const double &scale)
{
  //
  // Add another count
  //
  ++nevent;

  nevent_pred+= scale*static_cast<double>(rhs.nevent);
  nevent_pred_err+= TMath::Power(scale*static_cast<double>(rhs.nevent),2);
  nroi       += rhs.nroi;
  timer_alg  += scale*rhs.timer_alg;
  timer_rob  += scale*rhs.timer_rob;

  alg_ncalls += scale*static_cast<double>(rhs.alg_ncalls);
  alg_ncache += scale*static_cast<double>(rhs.alg_ncache);
  alg_nslow  += scale*static_cast<double>(rhs.alg_nslow);

  rob_nreq       += scale*static_cast<double>(rhs.rob_nreq);
  rob_nreq_cache += scale*static_cast<double>(rhs.rob_nreq_cache);
  rob_nreq_retrv += scale*static_cast<double>(rhs.rob_nreq_retrv);
  rob_nreq_other += scale*static_cast<double>(rhs.rob_nreq_other);

  rob_size       += scale*Exec::ConvertWordsToKB(rhs.rob_size);//*static_cast<double>(rhs.rob_size);
  rob_size_cache += scale*Exec::ConvertWordsToKB(rhs.rob_size_cache);//*static_cast<double>(rhs.rob_size_cache);
  rob_size_retrv += scale*Exec::ConvertWordsToKB(rhs.rob_size_retrv);//*static_cast<double>(rhs.rob_size_retrv);
  rob_size_other += scale*Exec::ConvertWordsToKB(rhs.rob_size_other);//*static_cast<double>(rhs.rob_size_other);

  rob_nids       += scale*static_cast<double>(rhs.rob_nids);      //*scale;
  rob_nids_cache += scale*static_cast<double>(rhs.rob_nids_cache);//*scale;
  rob_nids_retrv += scale*static_cast<double>(rhs.rob_nids_retrv);//*scale;
  rob_nids_other += scale*static_cast<double>(rhs.rob_nids_other);//*scale;

  ros_nreq       += scale*static_cast<double>(rhs.ros_nreq);
  ros_nreq_cache += scale*static_cast<double>(rhs.ros_nreq_cache);
  ros_nreq_retrv += scale*static_cast<double>(rhs.ros_nreq_retrv);

  ros_nreq_err       += TMath::Power(scale*static_cast<double>(rhs.ros_nreq),2);
  ros_nreq_retrv_err += TMath::Power(scale*static_cast<double>(rhs.ros_nreq_retrv),2);
  ros_size_retrv_err += TMath::Power(scale*Exec::ConvertWordsToKB(rhs.rob_size_retrv),2);
}

//-----------------------------------------------------------------------------------------
void Anp::ExecTablePred::pCount::AddCount(const Anp::Exec::Event &, const double &scale)
{
  nevent_pred+= scale;
  nevent_pred_err+=scale*scale;
}

//-----------------------------------------------------------------------------------------
double Anp::ExecTablePred::pCount::GetMeanROBTimePerRetr() const
{
  if(rob_nreq_retrv > 0) {
    return timer_rob/double(rob_nreq_retrv);
  }
  
  return 0.0;
}

//-----------------------------------------------------------------------------------------
double Anp::ExecTablePred::pCount::GetMeanROBTimePerEvent() const
{
  if(nevent > 0) {
    return timer_rob/static_cast<double>(nevent_pred);
  }
  
  return 0.0;
}

//--------------------------------------------------------------------------------------      
Anp::ExecTablePred::ExecTablePred()
  :fDir(0),
   fCanvas(0),
   fDebug(true),
   fFillNode(false),
   fMakeHist(false),
   fPlots(false),
   fTimeout(0),
   fDirName()
{
}

//--------------------------------------------------------------------------------------      
Anp::ExecTablePred::~ExecTablePred()
{
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTablePred::SetReg(const Registry &reg)
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
  reg.Get("Timeout",    fTimeout);
  reg.Get("HistOpt",    fHistOpt);
  reg.Get("MakeHist",   fMakeHist);
  reg.Get("RoiPlots",   fPlots);
  reg.Get("DirName",    fDirName);
  reg.Get("FillNode",   fFillNode);
  reg.Get("ClientLink", fClientLink);
  reg.Get("ShortClient", fShortClient);

  if(fMakeHist) {
    fCanvas = new TCanvas(GetClientName().c_str(), "", 650, 400);
  }

  //
  // Collect Scale Algorithm
  //
  string scale_name;
  if(fReg.Get("ScaleBG", scale_name)) {
    fScaleBG = Factory<ScaleABC>::Instance().Get(scale_name);
  }
  else {
    log() << "Done - scaleBG algorithm name is not set" << endl;
  }
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTablePred::SetDir(TDirectory *dir)
{
  //
  // Set directory for histograms
  //
  fDir = Anp::GetDir(dir, fDirName);
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTablePred::ProcessAlg(const Exec::AlgCall &alg)
{
  //
  // Count one algorithm execution
  //

  for(set<uint32_t>::const_iterator rit = alg.ros_ids.begin(); rit != alg.ros_ids.end(); ++rit) {
    const uint32_t ros_id = *rit;

    ExecClient *client = GetROSClient(ros_id, alg);
    if(!client) cout << "ProcessAlg - Invalid pointer to ROS client for id: " << ros_id << endl;
    client->ProcessAlg(alg);
    
    fActiveClients.push_back(client);
  }
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTablePred::FinishEvent(const Exec::Event &event)
{
  
  //
  // Remove duplicate pointers
  //
  std::sort(fActiveClients.begin(), fActiveClients.end());
  ClientVec::iterator it = std::unique(fActiveClients.begin(), fActiveClients.end());
  fActiveClients.erase(it, fActiveClients.end());
  
  //
  // Count one event
  //
  for(unsigned i = 0; i < fActiveClients.size(); ++i) {
    fActiveClients.at(i)->FinishEvent(event);
  }
  
  //
  // Collect Event Scale Factor
  //
  double scale_bg = 1.0;
  if(fScaleBG.valid()){
    scale_bg = fScaleBG->GetScaleBCID().scale_lumi;
    if(fDebug) log() << "Done - scaleBG algorithm valid with value: " << scale_bg << endl;
  }
  else log() << "Done - invalid scaleBG! Using default scale: " << scale_bg << endl;

  for(ROSMap::iterator ros = fROS.begin(); ros!=fROS.end(); ++ros){
    rCount* count = GetROSCount(ros->first,ros->second);
    if(!count){
      log() << "FinishEvent - Invalid pointer to ROS name: " << ros->first << endl;
      continue;
    }

    if(ros->second->GetCount().nevent>0) count->ros.AddCount(ros->second->GetCount(), scale_bg);//scale_bg
    if(ros->second->GetCount().nevent>1) log() << "FinishEvent - error too many events inside the count: " << ros->second->GetCount().nevent << endl;

    //
    // Only execute the following if the full information is being read out
    //
    if(!fShortClient){
      const CountMap &algs = ros->second->GetAlgCount();
      for(CountMap::const_iterator alg = algs.begin();alg!=algs.end();++alg){
	//
	// Add alg counts to the prediction
	//
	pCount *algCnt = GetALGCount(alg->first,count->algs);
	algCnt->AddCount(alg->second,scale_bg);
	algCnt->AddCount(event,scale_bg);
      }
      
      const CountMap &chns = ros->second->GetChnCount();
      for(CountMap::const_iterator chn = chns.begin();chn!=chns.end();++chn){
	//
	// Add chn counts to the prediction
	//
	pCount *chnCnt = GetCHNCount(chn->first,count->chns);
	chnCnt->AddCount(chn->second,scale_bg);
	chnCnt->AddCount(event,scale_bg);
      }
    }
  }
  
  fActiveClients.clear();
  for(ROSMap::iterator iter = fROS.begin(); iter!=fROS.end(); ++iter){
    iter->second->ClearCount();
  }
}

//--------------------------------------------------------------------------------------      
const Anp::Exec::Count& Anp::ExecTablePred::GetCount() const
{
  return fCount;
}

//--------------------------------------------------------------------------------------
Anp::ExecClient* Anp::ExecTablePred::GetROSClient(const unsigned ros_id, const Exec::AlgCall &alg)
{
  //
  // Make client for algorithm
  //
  ROSMap::iterator iros = fROS.find(ros_id);
  if(iros == fROS.end()) {

    Registry reg(fReg);
    reg.Set("DoAlg", true);
    reg.Set("DoChn", true);
    reg.Set("DoDet", false);
    reg.Set("DoROS", false);
    reg.Set("DoRoi", false);
    reg.Set("ROSId", ros_id);
    
    if(fDebug) {
      log() << "GetROSClient - " << alg.GetROSName(ros_id) << endl;
    }

    Handle<ExecClient> client(new ExecCount());
    iros = fROS.insert(ROSMap::value_type(ros_id, client)).first;
    
    client->SetClientName(alg.GetROSName(ros_id));
    client->SetClientType("");
    client->SetReg(reg);
    client->SetDir(Anp::GetDir(fDir, alg.GetROSName(ros_id)));
  }

  return iros->second.get();
}

//--------------------------------------------------------------------------------------
Anp::ExecTablePred::rCount* Anp::ExecTablePred::GetROSCount(const unsigned ros_id, const Handle<Anp::ExecClient> client)
{
  //
  // Make Count for ROS
  //
  ROSCountMap::iterator iros = fROSCount.find(ros_id);
  if(iros == fROSCount.end()) {
    rCount count;

    count.ros.ros_id   = ros_id;
    count.ros.ros_name = client->GetClientName();

    iros = fROSCount.insert(ROSCountMap::value_type(ros_id, count)).first;
  }

  return &(iros->second);
}

//--------------------------------------------------------------------------------------
Anp::ExecTablePred::pCount* Anp::ExecTablePred::GetALGCount(const unsigned alg_id, CountVec& algs)
{
  //
  // Find the alg count
  //
  for(unsigned i=0;i<algs.size();++i){
    if(algs.at(i).alg_id==alg_id) return &algs.at(i);
  }
  //
  // Add in alg
  //
  pCount newAlg;
  newAlg.alg_id = alg_id;
  algs.push_back(newAlg);

  return &(algs.at(algs.size()-1));
}

//--------------------------------------------------------------------------------------
Anp::ExecTablePred::pCount* Anp::ExecTablePred::GetCHNCount(const unsigned chn_id, CountVec& chns)
{
  //
  // Find the alg count
  //
  for(unsigned i=0;i<chns.size();++i){
    if(chns.at(i).chn_id==chn_id) return &chns.at(i);
  }
  //
  // Add in chn
  //
  pCount newChn;
  newChn.chn_id = chn_id;
  chns.push_back(newChn);

  return &(chns.at(chns.size()-1));
}

//--------------------------------------------------------------------------------------
void Anp::ExecTablePred::WriteExec(HtmlNode &anode, HtmlMenu &, const Exec::Summary &sum)
{
  //
  // Write out results for entire run (regardless of lumi block)
  //

  ostream &os = anode.GetCont();

  os << endl << "Prediction Table shows summary for data requests and retrievals per ROS:"
     << endl << "   - using enhanced bias data with event weights to predict higher luminosity ROS request rates";

  os << "<script src=\"sorttable.js\"></script>" << endl
     << "<p style=\"font-family:times; font-size:11px;\">" << endl
     << "<table border=\"1\" cellpadding=\"2\" cellspacing=\"0\" class=\"sortable\">" << endl
     << "<tr>"
     << "<th> ROS </th>"
     << "<th> Cost Events Processed </th>"
     << "<th> Events Rate (Hz) </th>"
     << "<th> wall Time (s) </th>"
     << "<th> #lbs </th>"
     << "<th> Requests </th>"
     << "<th> Retrievals </th>"
     << "<th> Retrievals/event </th>"
     << "<th> Request rate (Hz)</th>"
     << "<th> Retrieval rate (Hz)</th>"
     << "<th> Retrieved data (Kb/s)</th>"
     << "<th> Time per ROS retrieval (ms) </th>"
     << "<th> Time per event (ms) </th>"
     << "<th> ROB/request </th>"
     << "</tr>" << endl;

  rCountVec roses;

  for(ROSCountMap::iterator it = fROSCount.begin(); it != fROSCount.end(); ++it) {
    roses.push_back(it->second);
  }

  //
  // Sort ros by number of retrievals
  //
  std::sort(roses.begin(), roses.end(), SortrCount("rob_nreq_retrv"));    

  for(unsigned i = 0; i < roses.size(); ++i) {

    const pCount   &count    = roses.at(i).ros;
    const string   &ros_name = count.ros_name;
    CountVec       algs     = roses.at(i).algs;
    CountVec       chns     = roses.at(i).chns;

    string ros_link = ros_name;

    //
    // Creating Algorithm summary of requests for each ROS
    //
    if(sum.write_full) {

      //
      // Sort alg by number of retrievals
      //
      std::sort(algs.begin(), algs.end(), SortpCount("rob_nreq_retrv")); 
      std::sort(chns.begin(), chns.end(), SortpCount("rob_nreq_retrv")); 

      Handle<HtmlNode> nodeA(new HtmlNode(sum.html_base+"_"+ros_name+"_ros.html", HtmlNode::kHTML));
      anode.AddChild(nodeA);
      nodeA->SetBase(sum.html_base+"_"+ros_name);
      
      if(nodeA.valid()){
	nodeA->GetCont() << "<pre>" 
			 << "ROS: " << ros_link << endl;
	nodeA->GetCont() << endl << "Prediction Table shows summary for data requests and retrievals per Algorithm:"
			 << endl << "   - using enhanced bias data with event weights to predict higher luminosity ROS request rates";
	
	nodeA->GetCont() << "<script src=\"sorttable.js\"></script>" << endl
			 << "<p style=\"font-family:times; font-size:11px;\">" << endl
			 << "<table border=\"1\" cellpadding=\"2\" cellspacing=\"0\" class=\"sortable\">" << endl
			 << "<tr>"
			 << "<th> Algorithm </th>"
			 << "<th> Cost Events Processed </th>"
			 << "<th> Events Rate (Hz) </th>"
			 << "<th> wall Time (s) </th>"
			 << "<th> #lbs </th>"
			 << "<th> Requests </th>"
			 << "<th> Retrievals </th>"
			 << "<th> Retrievals/event </th>"
			 << "<th> Request rate (Hz)</th>"
			 << "<th> Retrieval rate (Hz)</th>"
			 << "<th> Retrieved data (Kb/s)</th>"
			 << "<th> Time per ROS retrieval (ms) </th>"
			 << "<th> Time per event (ms) </th>"
			 << "<th> ROB/request </th>"
			 << "</tr>" << endl; 
	
	ros_link = nodeA->GetLink(ros_link);
	//
	// Writing the summary table for each algorithm
	//
	for(unsigned j=0;j<algs.size();++j) WriteTable(nodeA.ref(), algs.at(j),sum.GetAlgName(algs.at(j).alg_id), sum);

	nodeA->GetCont() << "</table>" << endl << endl;
	//
	// Write Summary of chain requests
	//
	nodeA->GetCont() << "<script src=\"sorttable.js\"></script>" << endl
			 << "<p style=\"font-family:times; font-size:11px;\">" << endl
			 << "<table border=\"1\" cellpadding=\"2\" cellspacing=\"0\" class=\"sortable\">" << endl
			 << "<tr>"
			 << "<th> Chain </th>"
			 << "<th> Cost Events Processed </th>"
			 << "<th> Events Rate (Hz) </th>"
			 << "<th> wall Time (s) </th>"
			 << "<th> #lbs </th>"
			 << "<th> Requests </th>"
			 << "<th> Retrievals </th>"
			 << "<th> Retrievals/event </th>"
			 << "<th> Request rate (Hz)</th>"
			 << "<th> Retrieval rate (Hz)</th>"
			 << "<th> Retrieved data (Kb/s)</th>"
			 << "<th> Time per ROS retrieval (ms) </th>"
			 << "<th> Time per event (ms) </th>"
			 << "<th> ROB/request </th>"
			 << "</tr>" << endl; 

	for(unsigned j=0;j<chns.size();++j) WriteTable(nodeA.ref(), chns.at(j),sum.GetChnName(chns.at(j).chn_id), sum);

	nodeA->GetCont() << "</table>" << "</pre>" << endl;
      }
    }
    //
    // Write ROS entry
    //
    WriteTable(anode,count,ros_link,sum);
  }
  
  os << "</table>" << endl;
}

//--------------------------------------------------------------------------------------
void Anp::ExecTablePred::WriteTable(HtmlNode &anode,const pCount &count, const string& ros_link, const Exec::Summary &sum)
{
  //
  // Write out algorithm summary
  //
  ostream &os = anode.GetCont();

  double lb_time = sum.wall_time;

  //
  // Create web page for current ROS
  //
  
  double ret_per_event = 0.0;
  double rob_per_req   = 0.0;
  
  if(count.nevent_pred > 0) {
    ret_per_event = count.rob_nreq_retrv/count.nevent_pred;
  }
  if(count.rob_nreq > 0) {
    rob_per_req = count.rob_nids/count.rob_nreq;
  }
  
  if(fDebug) log() << "WriteExec - bandwidth number of kb: " << count.rob_size_retrv << " events: " << count.nevent_pred << endl;
  
  //
  // Calculating the Error on the predicted number of events
  //
  long double err_tot_req  = TMath::Sqrt(count.ros_nreq_err      );
  long double err_tot_ret  = TMath::Sqrt(count.ros_nreq_retrv_err);
  long double err_tot_siz  = TMath::Sqrt(count.ros_size_retrv_err);
  long double err_evt_rate = TMath::Sqrt(count.nevent_pred_err   );

  const string ret_per_event_str = Anp::Round2Pair(ret_per_event, 1.0).first;
  const string rob_per_req_str   = Anp::Round2Pair(rob_per_req,   1.0).first;

  string size_ret_str = Anp::Round2Pair(count.rob_size_retrv , 1.0).first;     
  string rate_req_str = Anp::Round2Pair(count.ros_nreq       , 1.0).first;    
  string rate_ret_str = Anp::Round2Pair(count.ros_nreq_retrv , 1.0).first; 
  string err_rate_req_str = Anp::Round2Pair(err_tot_req      , 1.0).first;    
  string err_rate_ret_str = Anp::Round2Pair(err_tot_ret      , 1.0).first; 
  string err_size_ret_str = Anp::Round2Pair(err_tot_siz      , 1.0).first;    

  if(fDebug) log() << "WriteExec - bandwidth before dividing by time: " << count.rob_size_retrv << " string: " << size_ret_str << endl; 
  
  long double tot_req  = count.rob_nreq; 
  long double tot_ret  = count.rob_nreq_retrv; 
  long double time_ret = count.GetMeanROBTimePerRetr();
  long double time_evt = count.GetMeanROBTimePerEvent();
  long double evt_rate = count.nevent_pred;
  double nlbs          = double(sum.sum_lbs.size());

  //
  // Add up the LB time
  //
  vector<Exec::SummaryLB> sumLBs = sum.sum_lbs;
  lb_time=0.0;
  for(unsigned j=0; j<sumLBs.size();++j) lb_time += sumLBs.at(j).lb_time;
  
  if(nlbs==0){ 
    nlbs = 1.0;
    lb_time = sum.wall_time;

  }
  else{
    if(fDebug) log() << "WriteExec - number of LBs is NONzero - time: " << lb_time << endl
		     << "     size ret: " << count.rob_size_retrv << endl;
  }
  
  if(lb_time>0){
    //
    // scaling the req/ret rate back by the number of LBs
    //
    size_ret_str = Anp::Round2Pair(count.rob_size_retrv /lb_time          , 1.0).first;
    rate_req_str = Anp::Round2Pair(count.rob_nreq       /lb_time          , 1.0).first;    
    rate_ret_str = Anp::Round2Pair(count.rob_nreq_retrv /lb_time          , 1.0).first;
    err_rate_req_str = Anp::Round2Pair(err_tot_req      /lb_time          , 1.0).first;    
    err_rate_ret_str = Anp::Round2Pair(err_tot_ret      /lb_time          , 1.0).first; 
    err_size_ret_str = Anp::Round2Pair(err_tot_siz      /lb_time          , 1.0).first; 
    err_evt_rate = err_evt_rate     /lb_time;
    evt_rate     = count.nevent_pred/lb_time;
    if(fDebug) log() << "WriteExec - bandwidth before after by time: " << count.rob_size_retrv/lb_time << " string: " << size_ret_str << endl;
  }
  else{
    if(fDebug) log() << "WriteExec - ERROR number of LBs is zero" << endl
		     << "     size ret: " << count.rob_size_retrv << endl;
  }
  
  //
  // Write HTML table entries
  //
  os << "<tr>" 
     << "<td align=\"left\"> "   << ros_link                                         << " </td>"
     << "<td align=\"center\"> " << count.nevent                                     << " </td>"
     << "<td align=\"center\"> " << evt_rate       << " +/- " << err_evt_rate        << " </td>"
     << "<td align=\"center\"> " << lb_time                                          << " </td>"
     << "<td align=\"center\"> " << static_cast<unsigned>(nlbs)                      << " </td>"
     << "<td align=\"center\"> " << tot_req                                          << " </td>"
     << "<td align=\"center\"> " << tot_ret                                          << " </td>"
     << "<td align=\"center\"> " << ret_per_event_str                                << " </td>"
     << "<td align=\"center\"> " << rate_req_str   << " +/- " << err_rate_req_str    << " </td>"
     << "<td align=\"center\"> " << rate_ret_str   << " +/- " << err_rate_ret_str    << " </td>"
     << "<td align=\"center\"> " << size_ret_str   << " +/- " << err_size_ret_str    << " </td>"
     << "<td align=\"center\"> " << time_ret                                         << " </td>"
     << "<td align=\"center\"> " << time_evt                                         << " </td>"
     << "<td align=\"center\"> " << rob_per_req_str                                  << " </td>"
     << "</tr>" << endl;
}

//--------------------------------------------------------------------------------------      
TH1* Anp::ExecTablePred::MakeHist(const std::string &hname, 
				 const std::string &yaxis, 
				 unsigned min_lb, 
				 unsigned max_lb,
				 TDirectory *dir)
{
  //
  // Create histogram
  //
  if(!(min_lb > 0 && min_lb < max_lb)) {
    return 0;
  }

  TH1 *h = new TH1F(hname.c_str(), "", max_lb-min_lb+1, min_lb, max_lb+1);
  h->SetDirectory(dir);
  h->SetStats(false);
  h->GetXaxis()->CenterTitle();
  h->GetXaxis()->SetTitle("luminosity block");
  h->GetYaxis()->CenterTitle();
  h->GetYaxis()->SetTitle(yaxis.c_str());  

  return h;
}

//--------------------------------------------------------------------------------------      
std::ostream& Anp::ExecTablePred::log() const
{
  std::cout << GetClientName() << "::";
  return std::cout;
}

//---------------------------------------------------------------------------------------
bool Anp::SortrCount::operator()(const Anp::ExecTablePred::rCount &lhs, const Anp::ExecTablePred::rCount &rhs) const
{
  //
  // Compare two objects based on key
  //
  if(fKey == "rob_nreq_retrv") {
    return lhs.ros.rob_nreq_retrv > rhs.ros.rob_nreq_retrv;
  }

  //
  // Compare by total time
  //
  return lhs.ros.timer_alg > rhs.ros.timer_alg;
}

//---------------------------------------------------------------------------------------
bool Anp::SortpCount::operator()(const Anp::ExecTablePred::pCount &lhs, const Anp::ExecTablePred::pCount &rhs) const
{
  //
  // Compare two objects based on key
  //
  if(fKey == "rob_nreq_retrv") {
    return lhs.rob_nreq_retrv > rhs.rob_nreq_retrv;
  }

  //
  // Compare by total time
  //
  return lhs.timer_alg > rhs.timer_alg;
}
