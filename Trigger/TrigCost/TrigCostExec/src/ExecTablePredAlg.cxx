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

// Local
#include "TrigCostExec/ExecCount.h"
#include "TrigCostExec/ExecTablePredAlg.h"

REGISTER_ANP_OBJECT(ExecClient,ExecTablePredAlg)

using namespace std;

//-----------------------------------------------------------------------------------------
Anp::ExecTablePredAlg::pCount::pCount() :
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
Anp::ExecTablePredAlg::rCount::rCount(){}

//-----------------------------------------------------------------------------------------
void Anp::ExecTablePredAlg::pCount::Clear()
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
}

//-----------------------------------------------------------------------------------------
void Anp::ExecTablePredAlg::pCount::AddCount(const Anp::Exec::Count &rhs, const double &scale)
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
void Anp::ExecTablePredAlg::pCount::AddCount(const Anp::Exec::Event &, const double &scale)
{
  nevent_pred+= scale;
  nevent_pred_err+=scale*scale;
}

//-----------------------------------------------------------------------------------------
double Anp::ExecTablePredAlg::pCount::GetMeanROBTimePerRetr() const
{
  if(rob_nreq_retrv > 0) {
    return timer_rob/double(rob_nreq_retrv);
  }
  
  return 0.0;
}

//-----------------------------------------------------------------------------------------
double Anp::ExecTablePredAlg::pCount::GetMeanROBTimePerEvent() const
{
  if(nevent > 0) {
    return timer_rob/static_cast<double>(nevent_pred);
  }
  
  return 0.0;
}

//--------------------------------------------------------------------------------------      
Anp::ExecTablePredAlg::ExecTablePredAlg()
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
Anp::ExecTablePredAlg::~ExecTablePredAlg()
{
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTablePredAlg::SetReg(const Registry &reg)
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
void Anp::ExecTablePredAlg::SetDir(TDirectory *dir)
{
  //
  // Set directory for histograms
  //
  fDir = Anp::GetDir(dir, fDirName);
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTablePredAlg::ProcessAlg(const Exec::AlgCall &alg)
{
  //
  // Count one algorithm execution
  //
  const uint32_t alg_id = alg.alg_conf->getNameId();

  ExecClient *client = GetALGClient(alg_id, alg);
  
  if(!client) cout << "ProcessAlg - Invalid pointer to ALG client for id: " << alg_id << endl;
  
  client->ProcessAlg(alg);
  
  fActiveClients.push_back(client);
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTablePredAlg::FinishEvent(const Exec::Event &event)
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

  //
  // Loop over the alg map from this event and add it to the scaled algorithm map
  //
  for(ALGMap::iterator alg = fALG.begin(); alg!=fALG.end(); ++alg){
    
    rCount* count = GetALGCount(alg->first);

    if(!count){
      log() << "FinishEvent - Invalid pointer to ALG name: " << alg->first << endl;
      continue;
    }

    if(alg->second->GetCount().nevent>0){ 

      count->alg.AddCount(alg->second->GetCount(), scale_bg);

//      log() << "FinishEvent - algcalls: " << alg->second->GetCount().alg_ncalls
//	    << " caches: "                << alg->second->GetCount().alg_ncache
//	    << " scaleBG: "               << scale_bg
//	    << endl;
    }


    if(alg->second->GetCount().nevent>1) log() << "FinishEvent - error too many events inside the count: " << alg->second->GetCount().nevent << endl;

    if(!fShortClient && (fLevel==2 || fLevel==0)){
      const CountMap &roses = alg->second->GetROSCount();
      for(CountMap::const_iterator ros = roses.begin();ros!=roses.end();++ros){
	//
	// Add alg counts to the prediction
	//
	pCount *rosCnt = GetROSCount(ros->first,count->roses);
	rosCnt->AddCount(ros->second,scale_bg);
	rosCnt->AddCount(event,scale_bg);
      }
    }
  }
  
  fActiveClients.clear();
  for(ALGMap::iterator iter = fALG.begin(); iter!=fALG.end(); ++iter){
    iter->second->ClearCount();
  }
}

//--------------------------------------------------------------------------------------      
const Anp::Exec::Count& Anp::ExecTablePredAlg::GetCount() const
{
  return fCount;
}

//--------------------------------------------------------------------------------------
Anp::ExecClient* Anp::ExecTablePredAlg::GetALGClient(const unsigned alg_id, const Exec::AlgCall &alg)
{
  //
  // Make client for algorithm
  //
  ALGMap::iterator ialg = fALG.find(alg_id);
  if(ialg == fALG.end()) {

    Registry reg(fReg);
    reg.Set("DoAlg", false);
    reg.Set("DoDet", false);
    reg.Set("DoROS", true);
    reg.Set("DoRoi", false);
    reg.Set("AlgId", alg_id);

    Ptr<TrigConfAlg> alg_conf = alg.alg_conf;

    if(fDebug) {
      log() << "GetALGClient - " << alg_conf->getName() << endl;
    }

    Handle<ExecClient> client(new ExecCount());
    ialg = fALG.insert(ALGMap::value_type(alg_id, client)).first;
    
    client->SetClientName(alg_conf->getName());
    client->SetClientType("");
    client->SetReg(reg);
    client->SetDir(Anp::GetDir(fDir, alg_conf->getName()));
  }

  return ialg->second.get();
}

//--------------------------------------------------------------------------------------
Anp::ExecTablePredAlg::rCount* Anp::ExecTablePredAlg::GetALGCount(const unsigned alg_id)
{
  //
  // Make Count for ALG
  //
  ALGCountMap::iterator ialg = fALGCount.find(alg_id);
  if(ialg == fALGCount.end()) {
    rCount count;

    count.alg.alg_id   = alg_id;
    //count.alg.alg_name = client->GetClientName();

    ialg = fALGCount.insert(ALGCountMap::value_type(alg_id, count)).first;
  }

  return &(ialg->second);
}

//--------------------------------------------------------------------------------------
Anp::ExecTablePredAlg::pCount* Anp::ExecTablePredAlg::GetROSCount(const unsigned ros_id, CountVec& roses)
{
  //
  // Find the alg count
  //
  for(unsigned i=0;i<roses.size();++i){
    if(roses.at(i).ros_id==ros_id) return &roses.at(i);
  }
  //
  // Add in ros
  //
  pCount newRos;
  newRos.ros_id = ros_id;
  roses.push_back(newRos);

  return &(roses.at(roses.size()-1));
}

//--------------------------------------------------------------------------------------
void Anp::ExecTablePredAlg::WriteExec(HtmlNode &anode, HtmlMenu &, const Exec::Summary &sum)
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
     << "<th> ALG </th>"
     << "<th> Cost Events Processed </th>"
     << "<th> Events Rate (Hz) </th>"
     << "<th> wall Time (s) </th>"
     << "<th> #lbs </th>"
     << "<th> # Alg Calls </th>"
     << "<th> # Alg Caches </th>";
  if(fLevel==2 || fLevel==0){
    os   << "<th> ROS retrieval Rate (Hz) </th>"
	 << "<th> Bandwidth (kb/s) </th>";
    }
  os << "<th> Call rate (Hz)</th>"
     << "<th> Cache rate (Hz)</th>"
     << "<th> Alg time/call (ms) </th>"
     << "<th> Alg time/event (ms) </th>"
     << "<th> Total alg time (s) </th>"
     << "<th> Total alg time (%) </th>"
     << "</tr>" << endl;

  rCountVec algs;
  long double tot_alg_time =0.0;

  for(ALGCountMap::iterator it = fALGCount.begin(); it != fALGCount.end(); ++it) {
    algs.push_back(it->second);
    tot_alg_time +=it->second.alg.timer_alg;
  }

  //
  // Sort alg by number of retrievals
  //
  std::sort(algs.begin(), algs.end(), SortrCount(""));    

  for(unsigned i = 0; i < algs.size(); ++i) {

    const pCount   &count    = algs.at(i).alg;
    const string   &alg_name = sum.GetAlgName(count.alg_id);//count.alg_name;
    CountVec       roses     = algs.at(i).roses;

    //
    // Sort ros by number of retrievals
    //
    std::sort(roses.begin(), roses.end(), SortpCount("rob_nreq_retrv")); 

    string alg_link = alg_name;

    //
    // Creating ROS summary of requests for each ALG
    //
    if(sum.write_full && (fLevel==2 || fLevel==0)) {
      Handle<HtmlNode> nodeA(new HtmlNode(sum.html_base+"_"+alg_name+"_alg.html", HtmlNode::kHTML));
      anode.AddChild(nodeA);
      nodeA->SetBase(sum.html_base+"_"+alg_name);
      
      if(nodeA.valid()){
	nodeA->GetCont() << "<pre>" 
			 << "ALG: " << alg_link << endl;
	nodeA->GetCont() << endl << "Prediction Table shows summary for data requests and retrievals per Algorithm:"
			 << endl << "   - using enhanced bias data with event weights to predict higher luminosity ALG request rates";
	
	nodeA->GetCont() << "<script src=\"sorttable.js\"></script>" << endl
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
			 << "<th> Time per ALG retrieval (ms) </th>"
			 << "<th> Time per event (ms) </th>"
			 << "<th> ROB/request </th>"
			 << "</tr>" << endl; 
	
	alg_link = nodeA->GetLink(alg_link);
	//
	// Writing the summary table for each algorithm
	//
	for(unsigned j=0;j<roses.size();++j) WriteTable(nodeA.ref(), roses.at(j),sum.GetROSName(roses.at(j).ros_id), sum);
	nodeA->GetCont() << "</pre>" << endl;
      }
    }
    //
    // Write ALG entry
    //
    WriteTableAlg(anode,count,alg_link,sum,tot_alg_time);
  }
  
  os << "</table>" << endl;
}

//--------------------------------------------------------------------------------------
void Anp::ExecTablePredAlg::WriteTableAlg(HtmlNode &anode,const pCount &count, const string& ros_link, const Exec::Summary &sum, const long double &tot_alg_time)
{
  //
  // Write out algorithm summary
  //
  ostream &os = anode.GetCont();

  double lb_time = sum.wall_time;

  //
  // Create web page for current Alg
  //
  
  if(fDebug) log() << "WriteExec - bandwidth number of kb: " << count.rob_size_retrv << " events: " << count.nevent_pred << endl;
  
  //
  // Calculating the Error on the predicted number of events
  //
  long double err_tot_ret  = TMath::Sqrt(count.ros_nreq_retrv_err);
  long double err_tot_siz  = TMath::Sqrt(count.ros_size_retrv_err);
  long double err_evt_rate = TMath::Sqrt(count.nevent_pred_err   );

  //  const string ret_per_event_str = Anp::Round2Pair(ret_per_event,  1.0).first;
  //const string rob_per_req_str   = Anp::Round2Pair(rob_per_req,    1.0).first;
  const string alg_time_str            = Anp::Round2Pair(count.timer_alg/1000.0,1.0).first;
  const string pred_alg_calls_str      = Anp::Round2Pair(count.alg_ncalls,1.0).first;
  const string pred_alg_cache_str      = Anp::Round2Pair(count.alg_ncache,1.0).first;

  string call_str = Anp::Round2Pair(count.alg_ncalls , 1.0).first;     
  string cach_str = Anp::Round2Pair(count.alg_ncache , 1.0).first; 
  string size_ret_str = Anp::Round2Pair(count.rob_size_retrv , 1.0).first;     
  string rate_ret_str = Anp::Round2Pair(count.ros_nreq_retrv , 1.0).first; 
  string err_rate_ret_str = Anp::Round2Pair(err_tot_ret      , 1.0).first; 
  string err_size_ret_str = Anp::Round2Pair(err_tot_siz      , 1.0).first; 

  string alg_time_pEvt = "N/A"; 
  string alg_time_pCal = "N/A"; 
  string alg_time_perc = "N/A";

  if(count.nevent_pred>0.0){
    alg_time_pEvt = Anp::Round2Pair(count.timer_alg/count.nevent_pred      , 1.0).first;
  }

  if(count.alg_ncalls>0.0){
    alg_time_pCal = Anp::Round2Pair(count.timer_alg/count.alg_ncalls      , 1.0).first;
  }

  if(tot_alg_time>0.0){
    alg_time_perc = Anp::Round2Pair(100.0*count.timer_alg/tot_alg_time      , 1.0).first;
  }

  if(fDebug) log() << "WriteExec - bandwidth before dividing by time: " << count.rob_size_retrv << " string: " << size_ret_str << endl; 
  
  long double evt_rate = count.nevent_pred;
  double nlbs          = double(sum.sum_lbs.size());

  //
  // Add up the LB time in seconds
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
    call_str     = Anp::Round2Pair(count.alg_ncalls     /lb_time          , 1.0).first;
    cach_str     = Anp::Round2Pair(count.alg_ncache     /lb_time          , 1.0).first;
    size_ret_str = Anp::Round2Pair(count.rob_size_retrv /lb_time          , 1.0).first;
    rate_ret_str = Anp::Round2Pair(count.rob_nreq_retrv /lb_time          , 1.0).first;
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
     << "<td align=\"center\"> " << pred_alg_calls_str                               << " </td>"
     << "<td align=\"center\"> " << pred_alg_cache_str                               << " </td>";
  if(fLevel==2 || fLevel==0){
    os << "<td align=\"center\"> " << rate_ret_str   << " +/- " << err_rate_ret_str    << " </td>"
       << "<td align=\"center\"> " << size_ret_str   << " +/- " << err_size_ret_str    << " </td>";
  }
  os << "<td align=\"center\"> " << call_str                                         << " </td>"
     << "<td align=\"center\"> " << cach_str                                         << " </td>"
     << "<td align=\"center\"> " << alg_time_pEvt                                    << " </td>"
     << "<td align=\"center\"> " << alg_time_pCal                                    << " </td>"
     << "<td align=\"center\"> " << alg_time_str                                     << " </td>"
     << "<td align=\"center\"> " << alg_time_perc                                    << " </td>"
     << "</tr>" << endl;
}

//--------------------------------------------------------------------------------------
void Anp::ExecTablePredAlg::WriteTable(HtmlNode &anode,const pCount &count, const string& ros_link, const Exec::Summary &sum)
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
TH1* Anp::ExecTablePredAlg::MakeHist(const std::string &hname, 
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
std::ostream& Anp::ExecTablePredAlg::log() const
{
  std::cout << GetClientName() << "::";
  return std::cout;
}

//---------------------------------------------------------------------------------------
bool Anp::SortrCount::operator()(const Anp::ExecTablePredAlg::rCount &lhs, const Anp::ExecTablePredAlg::rCount &rhs) const
{
  //
  // Compare two objects based on key
  //
  if(fKey == "rob_nreq_retrv") {
    return lhs.alg.rob_nreq_retrv > rhs.alg.rob_nreq_retrv;
  }

  //
  // Compare by total time
  //
  return lhs.alg.timer_alg > rhs.alg.timer_alg;
}

//---------------------------------------------------------------------------------------
bool Anp::SortpCount::operator()(const Anp::ExecTablePredAlg::pCount &lhs, const Anp::ExecTablePredAlg::pCount &rhs) const
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
