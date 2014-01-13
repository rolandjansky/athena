/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>
#include <sstream>
#include <exception>

// ROOT
#include "TCanvas.h"

// COOL
#include "CoolApplication/Application.h"
#include "CoolKernel/ValidityKey.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/Exception.h"
#include "CoolKernel/IObject.h"
#include "CoolKernel/IObjectIterator.h"
#include "CoralKernel/Context.h"

// Local
#include "TrigCostBase/Factory.h"
#include "TrigCostBase/HistMan.h"
#include "TrigCostBase/UtilCore.h"
#include "TrigCostBase/UtilHack.h"
#include "TrigCostBase/UtilTrig.h"

// Local
#include "TrigCostRate/ReadCool.h"

REGISTER_ANP_OBJECT(AlgEvent,ReadCool)

using namespace std;

//-----------------------------------------------------------------------------
Anp::ReadCool::ReadCool()
  :fCanvas(0),
   fDir(0),
   fDebug(false),
   fFillBGHist(false),
   fReadCounts(false),
   fReadBCIDs(false),
   fCoolTrigId("COOLONL_TRIGGER/COMP200"),
   fCoolTDAQId("COOLONL_TDAQ/MONP200"),
   fDirName("cool"),
   fLumiChan(0),
   fLumiChanBCID(102),
   fConfigRun(0),
   fConfigLB(0),
   fMinCount(10),
   fCountEvents(0),
   fRun(0),
   fLumi(0),
   fRunList(0)
{
}

//-----------------------------------------------------------------------------
Anp::ReadCool::~ReadCool()
{
}

//-----------------------------------------------------------------------------
void Anp::ReadCool::Config(const Registry &reg)
{
  //
  // Read configuration and setup ntuple branches
  //
  reg.Get("ReadCool", "Debug",        fDebug);
  reg.Get("ReadCool", "COOLTRIGID",   fCoolTrigId);
  reg.Get("ReadCool", "COOLTDAQID",   fCoolTDAQId);
  reg.Get("ReadCool::FillBGHist",     fFillBGHist);
  reg.Get("ReadCool::ReadCounts",     fReadCounts);
  reg.Get("ReadCool::ReadBCIDs",      fReadBCIDs);
  reg.Get("ReadCool::LumiChan",       fLumiChan);
  reg.Get("ReadCool::LumiChanBCID",   fLumiChanBCID);
  reg.Get("ReadCool::ConfigRun",      fConfigRun);
  reg.Get("ReadCool::ConfigLB",       fConfigLB);
  reg.Get("ReadCool::MinCount",       fMinCount);
  reg.Get("ReadCool::DirName",        fDirName);
  reg.Get("ReadCool::HistOpt",        fHistOpt);

  //
  // Book input data handle
  //
  fEntry = NtupleSvc::Instance("event").Branch<TrigMonEvent>("data");
  
  //
  // Add self to global Factory list - used by html algorithm and other clients
  //
  Factory<HtmlABC>::Instance().Hold(AlgEvent::GetName(), Handle<HtmlABC>(this));
  Factory<LumiABC>::Instance().Hold(AlgEvent::GetName(), Handle<LumiABC>(this));

  //
  // Register trigger configuration algorithm
  //
  Anp::RegisterCallback(reg, "ReadCool::ConfAlg", fConfAlg, this);

  //
  // Set COOL connection id
  //
  fCoolTrigDB.SetConnectionCOOL(fCoolTrigId);
  fCoolTDAQDB.SetConnectionCOOL(fCoolTDAQId);

  log() << "Config" << endl
	<< "   ReadCounts: " << fReadCounts << endl
	<< "   COOLTRIGID: " << fCoolTrigId << endl
	<< "   COOLTDAQID: " << fCoolTDAQId << endl;
}

//-----------------------------------------------------------------------------
void Anp::ReadCool::Signal(const Registry &, const string &signal)
{
  //
  // Handle callback from configuration algorithm 
  //
  if(signal == "TRIG_CONFIG" || signal == "TRIG_CONFIG_DB") {
    if(fConfAlg.valid()) { 
      //
      // Fill id -> name map
      //
      LumiABC::FillTrigNames(fConfAlg->GetConfig());
    }
    else {
      log() << "Signal - invalid configuration algorithm" << endl;
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::ReadCool::Save(TDirectory *dir)
{
  fDir = Anp::GetDir(dir, fDirName);
}

//-----------------------------------------------------------------------------
bool Anp::ReadCool::Init()
{  
  if(fConfigRun > 0) {
    log() << "Init - read COOL data using predefined run and LB:" << endl
	  << "   ConfigRun = " << fConfigRun << endl
	  << "   ConfigLB  = " << fConfigLB  << endl;
    
    Run(fConfigRun, fConfigLB);

    //
    // Release handle to TrigMonEvent - configuration is read just once
    //
    fEntry.release();
  }

  return true;
}

//-----------------------------------------------------------------------------
void Anp::ReadCool::Exec()
{
  //
  // Read new event entry
  //
  if(fEntry.valid() && 
     fEntry.GetState() == Ntuple::kREAD &&
     fEntry.GetEvent() == Ntuple::kPASS) {

    if(fDebug) {
      log() << "Exec - reading TrigMonEvent..." << endl;
    }

    Run(fEntry.GetRef().getRun(), fEntry.GetRef().getLumi());
  }
}

//-----------------------------------------------------------------------------
void Anp::ReadCool::Done()
{
  //
  // Print summary
  //
  log() << "Done - job summary: "   << endl;
  
  //
  // Compute liftime fraction
  //
  for(unsigned i = 0; i < fInfos.size(); ++i) {
    Lumi::BlockInfo &b = fInfos.at(i);
    
    double frac_sum = 0.0, frac_count = 0.0;
    
    for(unsigned i = 0; i < b.countsL1.size(); ++i) {
      const Ptr<Lumi::LV1Count> &c = b.countsL1.at(i);
      
      if(!c.valid() || c->count_tap <= fMinCount || c->count_tav <= fMinCount) {
	continue;
      }
      
      const double lfrac = double(c->count_tav)/double(c->count_tap);
      
      frac_count += 1;
      frac_sum   += lfrac;    
    }
  
    if(frac_count > 0) {
      b.lifetimeFrac = frac_sum/frac_count;
    }

    //
    // Compute scale factors for rates and correct for lifetime fraction
    //
    double lb_scale_factor = 0.0;
    
    if(b.GetLBDuration() > 0.0 && b.lifetimeFrac > 0.0) {
      lb_scale_factor = 1.0/(b.GetLBDuration()*b.lifetimeFrac);
    }
    
    b.scaleCorrL1 = lb_scale_factor;
    b.scaleCorrL2 = lb_scale_factor;
    b.scaleCorrEF = lb_scale_factor;
  }

  //
  // Write out 
  //
  WriteSummary(std::cout);
}

//-----------------------------------------------------------------------------
long double Anp::ReadCool::GetIntegratedTime() const
{
  //
  // Return total time for all processed lumi blocks
  // 
  long double total_time = 0.0;

  for(unsigned i = 0; i < fInfos.size(); ++i) {
    const Lumi::BlockInfo &b = fInfos.at(i);
    
    if(MatchRunLB(b.run, b.lb)) {
      total_time += b.GetLBDuration();
    }
  }

  return total_time;
}

//-----------------------------------------------------------------------------
long double Anp::ReadCool::GetIntegratedLumi() const
{
  //
  // Return time averaged instantenous luminosity for all processed lumi blocks
  //
  long double total_lumi = 0.0;

  for(unsigned i = 0; i < fInfos.size(); ++i) {
    const Lumi::BlockInfo &b = fInfos.at(i);
    
    if(MatchRunLB(b.run, b.lb)) {
      total_lumi += b.lumiInstan*b.GetLBDuration();
    }
  }

  return total_lumi;
}

//-----------------------------------------------------------------------------
long double Anp::ReadCool::GetRateCorrection(unsigned) const
{
  return 1.0;
}

//-----------------------------------------------------------------------------
long double Anp::ReadCool::GetMeanInstanLumi() const
{
  //
  // Return time averaged instantenous luminosity for all processed lumi blocks
  //
  long double total_lumi = 0.0, total_time = 0.0;

  for(unsigned i = 0; i < fInfos.size(); ++i) {
    const Lumi::BlockInfo &b = fInfos.at(i);
    
    if(MatchRunLB(b.run, b.lb)) {
      total_time += b.GetLBDuration();
      total_lumi += b.GetLBDuration()*b.lumiInstan;
    }
  }
  
  if(total_time > 0.0) {
    return total_lumi/total_time;
  }

  return 0.0;
}

//-----------------------------------------------------------------------------
void Anp::ReadCool::Run(unsigned run, unsigned lumi)
{  
  //
  // Read COOL data for new run
  //
  fCountEvents++;

  if(fRun == 0 || std::find(fRunList.begin(), fRunList.end(), run) == fRunList.end()) {
    fRun = run;
    fRunList.push_back(run); 

    log() << "Run - new run #" << run << endl;

    if(!fCoolTrigDB.OpenDB()) {
      log() << "Run - failed to open COOL DB connection... stop now." << endl;
      return;
    }

    if(!fCoolTDAQDB.OpenDB()) {
      log() << "Run - failed to open COOL DB TDAQ connection... stop now." << endl;
      return;
    }

    //
    // Read luminosity data for current run
    //
    map<unsigned, Lumi::BlockInfo> infos;
    ReadRunData(fRun, infos);
    
    for(map<unsigned, Lumi::BlockInfo>::iterator it = infos.begin(); it != infos.end(); ++it) {
      fInfos.push_back(it->second);
    }

    fCoolTrigDB.CloseDB();
    fCoolTDAQDB.CloseDB();
  }

  //
  // Record all unique run numbers and lumi blocks
  //
  if(lumi > 0 && fLumi != lumi) {
    fLumi = lumi;
    fRunMap[run].insert(lumi);
  }
}

//-----------------------------------------------------------------------------
void Anp::ReadCool::Write(HtmlNode &inode, HtmlMenu &)
{
  //
  // Write html summary
  //
  WriteSummary(inode.GetCont());

  inode.GetCont() << endl;
  
  Anp::SetROOT_defaults();  
  fCanvas = new TCanvas(AlgEvent::GetName().c_str(), "", 650, 400);

  //
  // Create node for this algorithm and write run-lb summary table
  //
  Handle<HtmlNode> node(new HtmlNode(AlgEvent::GetName()+"_table.html", HtmlNode::kHTML));
  inode.AddChild(node);
  inode.GetCont() << node->GetLink("Online trigger counts per lumi block (from COOL)") << endl;
  node->SetBase(AlgEvent::GetName());

  WriteTable(node.ref());

  delete fCanvas; fCanvas = 0;
}

//-----------------------------------------------------------------------------
void Anp::ReadCool::WriteSummary(std::ostream &os) const
{
  //
  // Write summary for processed data
  //
  set<unsigned> runs;
  
  for(unsigned i = 0; i < fInfos.size(); ++i) {
    const Lumi::BlockInfo &b = fInfos.at(i);
    runs.insert(b.run);
  }
  
  os << "Run(s):                ";  
  
  for(set<unsigned>::const_iterator irun = runs.begin(); irun != runs.end(); ++irun) {
    os << *irun << " ";
  }
  
  unsigned nlb = 0;
  for(RunLumiMap::const_iterator it = fRunMap.begin(); it != fRunMap.end(); ++it) {
    nlb += it->second.size();
  }

  os << endl
     << "Number of lumi blocks: " << nlb                 << endl
     << "Integrated time:       " << GetIntegratedTime() << endl
     << "Integrated lumi:       " << GetIntegratedLumi() << endl;
}

//-----------------------------------------------------------------------------
void Anp::ReadCool::ReadRunData(unsigned run, std::map<unsigned, Lumi::BlockInfo> &infos)
{
  //
  // Read lumi block data for run
  //  - start and stop time of each lumi block
  //  - read GlobalL1AcceptCount
  //
  log() << "ReadRunData - reading data for run: " << run << endl;

  cool::ValidityKey since(run);
  cool::ValidityKey until(run);
  cool::ValidityKey maskLB(0xffffffff);
  since <<= 32; // upper 31 bits are run number
  until <<= 32; // upper 31 bits are run number
  until  += maskLB;

  cool::IDatabasePtr &cool_ptr = fCoolTrigDB.cool_ptr();
  cool::IDatabasePtr &tdaq_ptr = fCoolTDAQDB.cool_ptr();
      
  uint64_t run_beg_time = 0, run_end_time = 0;

  //
  // Read luminosity start and end times for luminosity blocks
  //
  if(cool_ptr->existsFolder("/TRIGGER/LUMI/LBLB")) {

    log() << "ReadRunData - reading folder: /TRIGGER/LUMI/LBLB" << endl;

    cool::IFolderPtr folderLB = cool_ptr->getFolder("/TRIGGER/LUMI/LBLB");
    cool::IObjectIteratorPtr objectsLB = folderLB->browseObjects(since, until, 0);
    
    while ( objectsLB -> goToNext() ) {
      const cool::IObject &obj = objectsLB->currentRef();
      const cool::IRecord &payload = obj.payload();
      const unsigned lumi = (obj.since()  & 0xffff);

      Lumi::BlockInfo &data = infos[lumi];

      data.run         = run; 
      data.lb          = lumi;
      data.lumiBegTime = payload["StartTime"].data<cool::UInt63>();
      data.lumiEndTime = payload["EndTime"].data<cool::UInt63>();

      if(run_beg_time == 0 && run_end_time == 0) {
	run_beg_time = data.lumiBegTime;
	run_end_time = data.lumiEndTime;
      }
      else {
	run_beg_time = std::min(run_beg_time, data.lumiBegTime);
	run_end_time = std::max(run_end_time, data.lumiEndTime);
      }
    }
  }
  else { 
    log()<< "ReadRunData - missing COOL folder: /TRIGGER/LUMI/LBLB" << endl; 
  }

  //
  // Read global L1 accept counter
  //
  if (cool_ptr->existsFolder("/TRIGGER/LVL1/CTPCORELBDATA")) {
    
    log() << "ReadRunData - reading folder: /TRIGGER/LVL1/CTPCORELBDATA" << endl;

    cool::IFolderPtr folderCount = cool_ptr->getFolder("/TRIGGER/LVL1/CTPCORELBDATA");
    cool::IObjectIteratorPtr objectsCount = folderCount->browseObjects(since, until, 0);
    
    while ( objectsCount -> goToNext() ) {
      const cool::IObject &obj = objectsCount->currentRef();
      const unsigned lumi = (obj.since()  & 0xffff);

      Lumi::BlockInfo &data = infos[lumi];
      data.L1accept = obj.payloadValue<cool::UInt63>("GlobalL1AcceptCounter");
    }
  }
  else { 
    log() << "ReadRunData - missing COOL folder: /TRIGGER/LVL1/CTPCORELBDATA" << endl;
  }

  if(fReadCounts) {
    //
    // Read L1 item counters
    //
    if(cool_ptr->existsFolder("/TRIGGER/LUMI/LVL1COUNTERS")) {
      
      log() << "ReadRunData - reading folder: /TRIGGER/LUMI/LVL1COUNTERS" << endl;
      
      cool::IFolderPtr folderCount = cool_ptr->getFolder("/TRIGGER/LUMI/LVL1COUNTERS");

      for(unsigned id = 0; id < 256; ++id) {
	cool::ChannelSelection ch_sel(id);
		
	if(folderCount->existsChannel(id)) {
	  cool::IObjectIteratorPtr objectsCount = folderCount->browseObjects(since, until, ch_sel);
	  ReadLV1Counts(objectsCount, infos);
	}
      }
    }
    else { 
      log() << "ReadRunData - missing COOL folder: /TRIGGER/LUMI/LVL1COUNTERS" << endl;
    }

    //
    // Read L2 chain counters
    //
    if(cool_ptr->existsFolder("/TRIGGER/LUMI/LVL2COUNTERS")) {
      
      log() << "ReadRunData - reading folder: /TRIGGER/LUMI/LVL2COUNTERS" << endl;
      
      cool::IFolderPtr folderCount = cool_ptr->getFolder("/TRIGGER/LUMI/LVL2COUNTERS");
      cool::IObjectIteratorPtr objectsCount = folderCount->browseObjects(since, until, 0);

      ReadHLTCounts(2, objectsCount, infos);
    }
    else { 
      log() << "ReadRunData - missing COOL folder: /TRIGGER/LUMI/LVL2COUNTERS" << endl;
    }
    
    //
    // Read L2 chain counters
    //
    if (cool_ptr->existsFolder("/TRIGGER/LUMI/EFCOUNTERS")) {
      
      log() << "ReadRunData - reading folder: /TRIGGER/LUMI/EFCOUNTERS" << endl;
      
      cool::IFolderPtr folderCount = cool_ptr->getFolder("/TRIGGER/LUMI/EFCOUNTERS");
      cool::IObjectIteratorPtr objectsCount = folderCount->browseObjects(since, until, 0);
      
      ReadHLTCounts(3, objectsCount, infos);
    }
    else { 
      log() << "ReadRunData - missing COOL folder: /TRIGGER/LUMI/EFCOUNTERS" << endl;
    }
  }
 
  //
  // Read luminosity information
  //
  if (cool_ptr->existsFolder("/TRIGGER/LUMI/LBLESTONL")) {    
    log() << "ReadRunData - reading folder: /TRIGGER/LUMI/LBLESTONL" << endl;

    cool::IFolderPtr folderCount = cool_ptr->getFolder("/TRIGGER/LUMI/LBLESTONL");
    cool::IObjectIteratorPtr objectsCount = folderCount->browseObjects(since, until, 0);
    
    while ( objectsCount -> goToNext() ) {
      const cool::IObject &obj = objectsCount->currentRef();
      const unsigned lumi = (obj.since()  & 0xffff);

      const uint32_t channel = obj.channelId();

      Lumi::BlockInfo &data = infos[lumi];
      
      data.lumiInstan  = obj.payloadValue<cool::Float>("LBAvInstLumi");      
      data.lumiValid   = obj.payloadValue<cool::UInt32>("Valid");

      if(fDebug) {
	cout << "   lb=" << lumi << ", lumi=" << data.lumiInstan 
	     << ", valid=" << data.lumiValid << ", channel=" << channel << endl;
      }
    }
  }
  else {
    log() << "ReadRunData - missing COOL folder: /TRIGGER/LUMI/LBLESTONL" << endl;
  }

  //
  // Read luminosity information per BX
  //
  if(fReadBCIDs) {
    if (tdaq_ptr->existsFolder("/TDAQ/OLC/BUNCHLUMIS")) {    
      log() << "ReadRunData - reading folder: /TDAQ/OLC/BUNCHLUMIS" << endl;
      
      cool::IFolderPtr folder = tdaq_ptr->getFolder("/TDAQ/OLC/BUNCHLUMIS");
      
      const std::vector<cool::ChannelId> &channels = folder->listChannels();
      
      for(unsigned i = 0; i < channels.size(); ++i) {
	log() << "ReadRunData - /TDAQ/OLC/BUNCHLUMIS: channel=" << channels.at(i) << endl;
      }
      
      cool::IObjectIteratorPtr objectsIter = folder->browseObjects(run_beg_time, run_end_time, fLumiChanBCID);      
      ReadLumiPerBX(objectsIter);
    }
    else {
      log() << "ReadRunData - missing COOL folder: /TDAQ/OLC/BUNCHLUMIS" << endl;
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::ReadCool::ReadLV1Counts(cool::IObjectIteratorPtr &it, 
				  std::map<unsigned, Lumi::BlockInfo> &infos)
{
  //
  // Read L2 chain counts
  //
  while ( it -> goToNext() ) {
    const cool::IObject &obj = it->currentRef();
    const unsigned lumi = (obj.since()  & 0xffff);
           
    Ptr<Lumi::LV1Count> ptr = ObjectFactory<Lumi::LV1Count>::Instance().CreateObject();
    
    ptr->ctp_id    = obj.channelId();
    ptr->count_tav = obj.payloadValue<cool::UInt63>("L1Accept");
    ptr->count_tap = obj.payloadValue<cool::UInt63>("AfterPrescale");
    ptr->count_tbp = obj.payloadValue<cool::UInt63>("BeforePrescale");
    
    Lumi::BlockInfo &data = infos[lumi];
    data.countsL1.push_back(ptr);
  }
}

//-----------------------------------------------------------------------------
namespace Anp
{
  void unpackRateBlob(unsigned level,
		      unsigned char format,
		      const cool::Blob64k &blob,
		      Lumi::BlockInfo &data)
  {
    if(format == 1) {
      
      const unsigned int NData = 5;
      
      //
      // const 4 unsigned integers per chain in this level
      //
      const int n_lvl = blob.size() / ( NData * sizeof(unsigned int) ); 

      //
      // Iterate over data and hope it does not crash...
      // 
      const unsigned int* p = static_cast<const unsigned int*>(blob.startingAddress());
      
      for(int i = 0; i < n_lvl; i++) {
       	
	unsigned counter  = *(p++);
	unsigned input    = *(p++);
	unsigned after_ps = *(p++);
	unsigned raw      = *(p++);
	unsigned total    = *(p++);
	
	Ptr<Lumi::HLTCount> ptr = ObjectFactory<Lumi::HLTCount>::Instance().CreateObject();

	ptr->chain_level     = level;
	ptr->chain_counter   = counter;
	ptr->count_input     = input;
	ptr->count_after_ps  = after_ps;
	ptr->count_raw       = raw;
	ptr->count_total     = total;
	
	if     (level == 2) data.countsL2.push_back(ptr);
	else if(level == 3) data.countsEF.push_back(ptr);
      }
    }
  }
  
  std::vector<uint8_t> unpackBlobs(const cool::Blob64k &blob) 
  {
    const uint8_t* p = static_cast<const uint8_t*>(blob.startingAddress());
    
    std::vector<uint8_t> bvec(blob.size(), 0);
    
    for(int i = 0; i < blob.size(); ++i, ++p) {
      bvec.at(i) = *p;
    }

    return bvec;
  }

  void unpackLumiBlob(const cool::Blob64k &blob)
  {
    //
    // const 4 unsigned integers per chain in this level
    //
    const long size = blob.size();
    if(size < 1) {
      return;
    }
    
    /*
    const unsigned char* p = static_cast<const unsigned char*>(blob.startingAddress());
    
    const unsigned bss = ((*p) % 100) / 10;
    const unsigned smd = ((*p) % 100) % 10;
    
    cout << "  blob size=" << size << " bss=" << bss << " smd=" << smd << endl;
    
    if(smd == 2) {
      if(size < 3) {
	return;
      }
      
      const std::vector<uint8_t> bvec = unpackBlobs(blob);
      const unsigned bcid_size = (bvec[1] << 8) + bvec[2];

      cout << "  BCID vector size=" << bcid_size << endl;

      for(int i = 0; i < std::min<int>(3, bvec.size()); ++i) {	
	cout << "   bvec[" << i << "]=" << static_cast<int>(bvec.at(i)) << endl;
      }
    }
    else {
    }
    */
  }
}

//-----------------------------------------------------------------------------
void Anp::ReadCool::ReadHLTCounts(unsigned level,
				  cool::IObjectIteratorPtr &it, 
				  std::map<unsigned, Lumi::BlockInfo> &infos)
{
  //
  // Read L2 chain counts
  //
  while ( it -> goToNext() ) {
    const cool::IObject &obj = it->currentRef();
    const unsigned lumi = (obj.since()  & 0xffff);
    
    Lumi::BlockInfo &data = infos[lumi];
    
    const unsigned char  format = obj.payloadValue<cool::UChar>("Format");
    const cool::Blob64k &blob   = obj.payloadValue<cool::Blob64k>("Data");

    Anp::unpackRateBlob(level, format, blob, data);
  }
}

//-----------------------------------------------------------------------------------------
void Anp::ReadCool::ReadLumiPerBX(cool::IObjectIteratorPtr &it)
{
  //
  // Read lumi per bcid
  //
  while ( it -> goToNext() ) {

    const cool::IObject &obj = it->currentRef();
    const cool::UInt63 runlb =  obj.payloadValue<cool::UInt63>("RunLB");
    
    const unsigned lb  = (runlb & 0xffffffff);
    const unsigned run = (runlb >> 32);

    const float          lumi = obj.payloadValue<cool::Float>("AverageRawInstLum");
    const cool::Blob64k &blob = obj.payloadValue<cool::Blob64k>("BunchRawInstLum");
    
    if(fDebug) {
      log() << "ReadLumiPerBX - run=" << run << " lb=" << lb << ": average lumi=" << lumi << endl;
    }

    Anp::unpackLumiBlob(blob);
  } 
}

//-----------------------------------------------------------------------------------------
void Anp::ReadCool::WriteTable(HtmlNode &inode) const
{
  //
  // Write full run and lb table
  //
  ostream &os = inode.GetCont();
  
  os << "<pre><style type=\"text/css\">"<< endl
     << "table,  th {border:1px solid black;}"<< endl
     << "th { padding:10px; background-color:white; color:black; }"<< endl
     << "td {padding-left:15px;padding-right:15px;}"<< endl
     << "</style>"<< endl
     << "<script src=\"sorttable.js\"></script>" << endl
     << "<table border=1 frame=box rules=cols class=\"sortable\">" << endl
     << "<tr>"
     << "<th> Run </th>"
     << "<th> LB </th>"
     << "<th> Start </th>"
     << "<th> Inst L </th>"
     << "<th> Live fraction </th>"
     << "<th> Live fraction per BG </th>"
     << "<th> Scale L1 </th>"
     << "<th> Scale L2 </th>"
     << "<th> Scale EF </th>"
     << "<th> L1 </th>"
     << "<th> L2 </th>"
     << "<th> EF </th>"
     << "</tr>" 
     << endl;
  
  for(unsigned i = 0; i < fInfos.size(); ++i) {
    const Lumi::BlockInfo &b = fInfos.at(i);

    os << "<tr>" 
       << "<td align=\"right\">" << b.run                  << "</td>"
       << "<td align=\"right\">" << b.lb                   << "</td>"
       << "<td align=\"right\">" << b.GetBegTimeAsString() << "</td>"
       << "<td align=\"right\">" << b.lumiInstan           << "</td>";

    //
    // Make lifetime histogram and add link to histogram (optional)
    //    
    FillLifetimeFraction(b, inode);

    os << "<td align=\"right\">" << b.scaleCorrL1          << "</td>"
       << "<td align=\"right\">" << b.scaleCorrL2          << "</td>"
       << "<td align=\"right\">" << b.scaleCorrEF          << "</td>";
    
    stringstream base_str;
    base_str << inode.GetBase() 
	     << "run_" << b.run << "_lb" << setw(4) << setfill('0') << b.lb << "_table";

    //
    // Write L1 counts
    //
    stringstream nameL1;
    nameL1 << b.L1accept;
    
    Handle<HtmlNode> nodeL1(new HtmlNode(base_str.str()+"L1.html", HtmlNode::kHTML));
    inode.AddChild(nodeL1);
    WriteLV1Counts(nodeL1.ref(), b);    

    //
    // Write L2 counts
    //
    stringstream nameL2;
    nameL2 << b.L2accept;

    Handle<HtmlNode> nodeL2(new HtmlNode(base_str.str()+"L2.html", HtmlNode::kHTML));
    inode.AddChild(nodeL2);
    WriteHLTCounts(nodeL2.ref(), b, 2);

    //
    // Write EF counts
    //
    stringstream nameEF;
    nameEF << b.EFaccept;

    Handle<HtmlNode> nodeEF(new HtmlNode(base_str.str()+"EF.html", HtmlNode::kHTML));
    inode.AddChild(nodeEF);
    WriteHLTCounts(nodeEF.ref(), b, 3);
    
    //
    // Insert links
    //
    os << "<td align=\"right\">" << nodeL1->GetLink(nameL1.str()) << "</td>"
       << "<td align=\"right\">" << nodeL2->GetLink(nameL2.str()) << "</td>"
       << "<td align=\"right\">" << nodeEF->GetLink(nameEF.str()) << "</td>"
       << "</tr>"
       << endl;    
  }

  os << "</table></pre>" << endl;
}

//-----------------------------------------------------------------------------------------
void Anp::ReadCool::FillLifetimeFraction(const Lumi::BlockInfo &b, HtmlNode &node) const
{
  //
  // Make histogram with lifetime for all L1 triggers in a lumi block
  //

  TH1 *hfrac = GetTH1("lifetime_frac"); 
  map<string, TH1 *> bgmap;

  for(unsigned i = 0; i < b.countsL1.size(); ++i) {
    const Ptr<Lumi::LV1Count> &c = b.countsL1.at(i);
    
    if(!c.valid() || c->count_tap <= fMinCount || c->count_tav <= fMinCount) {
      continue;
    }
    
    const double lfrac = double(c->count_tav)/double(c->count_tap);

    if(hfrac) hfrac->Fill(lfrac);
    
    //
    // Fill lifetime fraction histogram for each BG
    //
    if(fFillBGHist) {
      const string bgkey = GetItemBG(c->ctp_id);
      
      map<string, TH1 *>::iterator bit = bgmap.find(bgkey);
      if(bit == bgmap.end()) {
	//
	// Make new BG histogram
	//
	TH1 *h = GetTH1("lifetime_frac", "lifetime_frac_"+bgkey); 
	
	bit = bgmap.insert(map<string, TH1 *>::value_type(bgkey, h)).first;
      }
      
      TH1 *hb = bit->second;
      if(hb) {
	hb->Fill(lfrac);
      }
    }
  }

  //
  // Print and insert histogram
  //
  if(!hfrac) {
    node.GetCont() << "<td align=\"right\">" << b.lifetimeFrac << "</td>";
    return;
  }

  string suff;
  if     (fHistOpt.find("eps") != string::npos) suff = ".eps";
  else if(fHistOpt.find("png") != string::npos) suff = ".png";

  stringstream link_str, name_str;

  link_str << b.lifetimeFrac;
  name_str << node.GetBase() 
	   << "run_" << b.run << "_lb" << setw(4) << setfill('0') << b.lb << "_lifetime";
  
  //
  // Insert link into html table with lifetime distribution
  //
  node.GetCont() << "<td align=\"right\">";
  
  Anp::PrintHist(node, hfrac, fCanvas, name_str.str()+suff, link_str.str(), fHistOpt);
 
  node.GetCont() << "</td>";

  //
  // Print livetime histogram for individual BGs
  //
  node.GetCont() << "<td align=\"right\">";

  for(map<string, TH1 *>::iterator bit = bgmap.begin(); bit != bgmap.end(); ++bit) {
    const string name_bg = name_str.str()+"_"+bit->first+suff;
    
    node.GetCont() << " ";
    Anp::PrintHist(node, bit->second, fCanvas, name_bg, bit->first, fHistOpt);
  }

  node.GetCont() << "</td>";
}

//-----------------------------------------------------------------------------
bool Anp::ReadCool::MatchRunLB(unsigned run, unsigned lb) const
{
  //
  // Check if this run and lumi pair has been processed by this module
  //
  const RunLumiMap::const_iterator rit = fRunMap.find(run);
  if(rit != fRunMap.end()) {
    return rit->second.count(lb);
  }
  
  return false;
}

//---------------------------------------------------------------------------------------
TH1* Anp::ReadCool::GetTH1(const std::string &key, const std::string &name) const
{
  //
  // Histograms are owned by TDirectory
  //
  TH1 *h = Anp::HistMan::Instance().CreateTH1(key, "ReadCool");
  
  if(h && !name.empty()) {
    h->SetName(name.c_str());
  }

  Anp::SetDir(h, fDir);

  return h;
}

//---------------------------------------------------------------------------------------
std::string Anp::ReadCool::GetItemBG(unsigned ctpid) const
{
  //
  // Get bunch group name for L1 item
  //
  if(!fConfAlg.valid()) {
    log() << "GetItemBG - missing configuration algorithm" << endl;
    return "UNKNOWN_BG";
  }

  //
  // Find list of L1 thresholds
  //
  const set<std::string> thrs = fConfAlg->GetItemThr(ctpid);
  
  //
  // Find BG name
  //
  for(set<std::string>::const_iterator it = thrs.begin(); it != thrs.end(); ++it) {
    const string &th = *it;
    
    if(th.find("BGRP") != string::npos && th != "BGRP0") {
      return th;
    }
  }

  //
  // Find BG name
  //
  for(set<std::string>::const_iterator it = thrs.begin(); it != thrs.end(); ++it) {
    const string &th = *it;
    
    if(th.find("BGRP") != string::npos) {
      return th;
    }
  }

  return "UNKNOWN_BG";
}
