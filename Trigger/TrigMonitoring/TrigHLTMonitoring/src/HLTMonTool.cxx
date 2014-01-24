/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**    @file HLTMonTool.cxx
*   
*    
*                          
*                          
*    @author Luca Fiorini, based on code from Shaun Roe, Manuel Diaz & Rob McPherson         
*    Also uses code from InDet::SCT_ResidualPullCalculator     
*/
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "GaudiKernel/IJobOptionsSvc.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h" //x

#include "GaudiKernel/MsgStream.h"//x
#include "GaudiKernel/StatusCode.h"//x
#include "GaudiKernel/ITHistSvc.h"//x
#include "GaudiKernel/PropertyMgr.h"//x
#include "GaudiKernel/IToolSvc.h"//x

#include "GaudiKernel/GaudiException.h"

#include "StoreGate/StoreGateSvc.h"

#include "TrigSteeringEvent/HLTResult.h"   

#include "TH1F.h"
#include "TH2F.h"

#include "AthenaKernel/IOVTime.h"


#include <iostream>
#include <fstream>

#include <sstream>

#include <boost/regex.hpp>
#include "TrigHLTMonitoring/HLTMonTool.h"
#include "TrigHLTMonitoring/HLTMonException.h"


//for GetKalmanUpdator
#include "GaudiKernel/ListItem.h"

/** Constructor, calls base class constructor with parameters
*
*  several properties are "declared" here, allowing selection
*  of the filepath for histograms, the first and second plane
*  numbers to be used, and the timebin.
*/
HLTMonTool::HLTMonTool(const std::string & type, 
const std::string & name,
const IInterface* parent)
: IHLTMonTool(type, name, parent), m_lbc(0)
{
  declareProperty("RegExes",m_regexlist);
  declareProperty("LumiKey",m_lumikey = "LumiBlocks");
  declareProperty("RateChains",m_rateregex);
  declareProperty("ScopeLumi",m_scopeLumi = 0);
}

/*---------------------------------------------------------*/
HLTMonTool::~HLTMonTool()
/*---------------------------------------------------------*/
{
}

/*---------------------------------------------------------*/
StatusCode HLTMonTool:: init()
/*---------------------------------------------------------*/
{ 
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode HLTMonTool::book( ) //suppress 'unused' compiler warning
/*---------------------------------------------------------*/
{
  StatusCode sc = StatusCode::SUCCESS;
  ATH_MSG_VERBOSE("HLTMonTool::book()");
  ATH_MSG_VERBOSE("Regular Expressions used for chain selection:\n" << m_regexlist);
  
//  if (isNewRun) {  // Rebooking is now handled in the MonagedMonitorTool -- LS 20 Jan 2014
      addMonGroup(new MonGroup(this,"HLT/ResultMon",run));
      
      sc = GetL1SummaryAndLBInfo();
      if(sc.isFailure()) ATH_MSG_WARNING("failed getting necessary info for booking");
      sc = bookResultAndConsistencyHistograms("L2");
      if(sc.isFailure()) ATH_MSG_WARNING("failed booking result and consistency histos L2");
      sc = bookResultAndConsistencyHistograms("EF");
      if(sc.isFailure()) ATH_MSG_WARNING("failed booking result and consistency histos EF");
      sc = bookLvl1Histograms();
      if(sc.isFailure()) ATH_MSG_WARNING("failed booking LVL1 histos");
      sc = bookHLTHistograms();
      if(sc.isFailure()) ATH_MSG_WARNING("failed booking HLT Histos");
//  }
  if(sc.isFailure()) ATH_MSG_WARNING("failed at booking histograms for HLTMonTool");
  return sc;
}

/*---------------------------------------------------------*/
StatusCode HLTMonTool::fill()
/*---------------------------------------------------------*/
{
  ATH_MSG_VERBOSE("fill()");
  StatusCode sc = StatusCode::SUCCESS;

  //go to original MonGroup
  setCurrentMonGroup("HLT/ResultMon");  
  sc = fillResultAndConsistencyHistograms("HLTResult_L2", hist("ConfigConsistency_L2"), hist("HLTResultL2"));
  if(sc.isFailure()) ATH_MSG_WARNING("Filling Result and Consistency histograms failed for L2");
  sc = fillResultAndConsistencyHistograms("HLTResult_EF", hist("ConfigConsistency_EF"), hist("HLTResultEF"));
  if(sc.isFailure()) ATH_MSG_WARNING("Filling Result and Consistency histograms failed for EF");
  
  fillLvl1Histograms();
  if(sc.isFailure()) ATH_MSG_WARNING("Filling Level 1 histograms failed");
  
  std::vector<std::string> myChains = getTDT()->getListOfTriggers("L2_.*|EF_.*");
  if (!(myChains.empty())) {
    std::vector<std::string>::const_iterator chIt;
    for (chIt=myChains.begin(); chIt!=myChains.end(); ++chIt) {
      sc = fillForChain(*chIt);
      if(sc.isFailure()) ATH_MSG_WARNING("Filling Histograms failed for Chain " << *chIt);
    }
  }
  else {
    ATH_MSG_WARNING("No L2 and/or EF chains found using TDT");
    sc = StatusCode::FAILURE;
  }
  return sc;
}

StatusCode  HLTMonTool::proc(){//suppress 'unused' compiler warning
  return StatusCode::SUCCESS;
}

StatusCode HLTMonTool::GetL1SummaryAndLBInfo(){
  StatusCode sc = StatusCode::SUCCESS;
  ATH_MSG_DEBUG("getting Level 1 triggers and Lumiblock information.");
  m_L1_summary = getTDT()->getChainGroup("L1_.*")->getListOfTriggers();    

  if(m_L1_summary.empty()){
    sc = StatusCode::FAILURE;
    ATH_MSG_WARNING("List of L1 Items from TDT is empty");
  }
  if(m_scopeLumi){
    sc = m_inputMetaStore->retrieve(m_lbc,m_lumikey);
    if(sc.isFailure()){
      ATH_MSG_INFO("Couldn't retrieve LumiBlockCollection!");
      m_scopeLumi = 0; //switch off rate monitoring
    }
    else{
      ATH_MSG_DEBUG("Retrieved LumiBlockCollection! Size is " << m_lbc->size());
      
      // loop over lumiblockcollection, count lumiblocks for histo booking
      LumiBlockCollection::iterator lbit = m_lbc->begin();
      int m_nLBs = 0;
      uint32_t start, stop;
      uint32_t first = (((IOVRange*)(*lbit))->start()).event();
      for(;lbit!=m_lbc->end();++lbit){
	IOVRange* range = (IOVRange*)(*lbit);
	start = (range->start()).event();
	stop = (range->stop()).event();
	m_nLBs += stop - start + 1;
      }//loop over contigous lb ranges
      
      //get all LumiBlocks for this run
      ATH_MSG_DEBUG("Now making CoolQuery database access!");
      m_coolquery = new CoolQuery("COOLOFL_TRIGGER/COMP200","L1_MBTS_1"); // <<-- L1_MBTS_1 is just dummy initial value
      if(m_coolquery){
	m_coolquery->openDbConn();
	
	m_coolquery->setIOV(IOVTime(getRunNr(),first).re_time(), IOVTime(getRunNr(),stop).re_time());
	std::string m_parlvl1lblbfolder = "/TRIGGER/LUMI/LBLB";// for time information
	m_lbStartTimes = m_coolquery->getObjMapFromFolderAtChan<cool::UInt63>("StartTime", m_parlvl1lblbfolder, 0);
	m_lbStopTimes = m_coolquery->getObjMapFromFolderAtChan<cool::UInt63>("EndTime", m_parlvl1lblbfolder, 0);
	delete m_coolquery;
      }else{
	ATH_MSG_WARNING("Couldn't create CoolQuery datase access.");
	m_scopeLumi = 0; //switch off rate monitoring
	sc = StatusCode::FAILURE;
      }
    }
  }
  return sc;
}

StatusCode HLTMonTool::bookResultAndConsistencyHistograms(const std::string& lvl){
  StatusCode sc = StatusCode::SUCCESS; 

  std::string resultname = "HLTResult"+lvl;
  std::string consistname = "ConfigConsistency_"+lvl;

  std::string help[] = {"HLTResult","isAccepted","isPassThrough"};
  addHistogram(new TH1F(resultname.c_str(),std::string("HLT "+lvl+" Result").c_str(), 3, 0.5, 3.5));
  for(int i=1;i<=3;i++) hist(resultname)->GetXaxis()->SetBinLabel(i,help[i-1].c_str());
  
  const char* labels[] = {"DB NULL","BS NULL","Inconsistent"};
  const char* keys[] = {"SMK ","HLT Prescale"};
  addHistogram(new TH1F(consistname.c_str(),std::string("Trigger Configuration Consistency "+lvl).c_str(),6,0.5,6.5),"HLT/ResultMon");      
  std::stringstream binlabel;
  for(uint i = 0; i < sizeof(keys)/sizeof(keys[0]); ++i){
    for(uint j = 0; j < sizeof(labels)/sizeof(labels[0]); ++j){
      binlabel.str("");
      binlabel << keys[i] << labels[j];
      hist(consistname)->GetXaxis()->SetBinLabel(3*i+j+1, binlabel.str().c_str());
    }
  }

  return sc;
}

StatusCode HLTMonTool::bookLvl1Histograms(){
  StatusCode sc = StatusCode::SUCCESS;
  int n_L1bins = m_L1_summary.size();  
  addHistogram(new TH1F("L1Events","Events per Item at L1", n_L1bins, 0.5, n_L1bins+0.5));
  for(int i = 1;i <= n_L1bins; i++) hist("L1Events")->GetXaxis()->SetBinLabel(i,m_L1_summary[i-1].c_str());
  return sc;
}

StatusCode HLTMonTool::bookHLTHistograms(){
  StatusCode sc = StatusCode::SUCCESS;
  std::map<std::string,std::string> streams = m_regexlist;
  std::map<std::string,std::string>::iterator strItr;
  for (strItr=streams.begin();strItr!=streams.end(); ++strItr)
    sc = bookHLTHistogramsForStream(strItr->first, strItr->second);
  return sc;
}

StatusCode HLTMonTool::bookHLTHistogramsForStream(const std::string& name, const std::string& regex){
  ATH_MSG_DEBUG("booking histograms for stream " << name);
  StatusCode sc = StatusCode::SUCCESS;
  std::string monpath = "HLT/ResultMon/" + name;
  std::vector<std::string>::iterator chItr;

  addMonGroup(new MonGroup(this,monpath,run));
  std::vector<std::string> seplvl[] = {getTDT()->getListOfTriggers("L2_"+regex.substr(8,regex.length()-8)),
				       getTDT()->getListOfTriggers("EF_"+regex.substr(8,regex.length()-8))
  };                                 //^^^ MAGIC NUMBERS AT WORK HERE ^^^
  
  //create helper strings
  std::stringstream tmp_histname;  
  const char* triggerstatus[]={"RAW","PS","PT"};
  const char* levels[]={"L2","EF"};
  
  for(uint i=0;i<sizeof(levels)/sizeof(levels[0]);++i)
    for (uint j=0; j<sizeof(triggerstatus)/sizeof(triggerstatus[0]); ++j)
      if(seplvl[i].size()>0) {
	tmp_histname.str("");//reset
	tmp_histname << levels[i] << "_" << name << triggerstatus[j];
	ATH_MSG_DEBUG("\tbooking --> " << tmp_histname.str());
	addHistogram(new TH1F(tmp_histname.str().c_str(),name.c_str(),seplvl[i].size(),0.5,seplvl[i].size()+0.5),monpath);
	
	for (chItr=seplvl[i].begin(); chItr!=seplvl[i].end(); chItr++)
	  hist(tmp_histname.str(),monpath)->GetXaxis()->SetBinLabel(chItr-(seplvl[i]).begin()+1,chItr->c_str());
      }

  //add a roi histo
  addHistogram(new TH2F(std::string(name+"RoIs").c_str(),tmp_histname.str().c_str(),100,-5,5,64,-3.2,3.2),monpath);
  
  monpath+="/Rates";
  addMonGroup(new MonGroup(this,monpath,run));
  
  
  if(m_scopeLumi)
    for(std::vector<std::string>::iterator rItr=m_rateregex.begin(); rItr!=m_rateregex.end();++rItr){
      std::vector<std::string> ratechains = getTDT()->getListOfTriggers(*rItr);
      for(chItr = ratechains.begin(); chItr!=ratechains.end();++chItr){
	ATH_MSG_VERBOSE("booking Rate histogram for chain " << *chItr << " in " << monpath);
	addHistogram(new TH1F(chItr->c_str(),std::string("Rate_"+(*chItr)).c_str(),m_nLBs,0.5,m_nLBs+0.5),monpath);
	for(LumiBlockCollection::iterator lbit=m_lbc->begin();lbit!=m_lbc->end();++lbit){
	  IOVRange* range = (IOVRange*)(*lbit);
	  uint32_t start = (range->start()).event();
	  uint32_t stop = (range->stop()).event();
	  for(uint32_t i=start;i<=stop;++i)
	    hist(chItr->c_str(),monpath)->GetXaxis()->SetBinLabel(start+(i-1),((std::string)IOVTime(getRunNr(),start+(i-1))).c_str());
	}//loop over contigous lb ranges
      }
    }
  return sc;
}

StatusCode HLTMonTool::fillResultAndConsistencyHistograms(const std::string& key,TH1* chist, TH1* rhist){
  StatusCode sc = StatusCode::SUCCESS;
  const HLT::HLTResult*  HLTResult(0);
  sc = m_storeGate->retrieve(HLTResult,key);
  if (sc.isFailure()) ATH_MSG_DEBUG(" Failed to retrieve HLT Result " << key);
  else {
    uint32_t bskeys[] = {HLTResult->getConfigSuperMasterKey(), HLTResult->getConfigPrescalesKey()};
    uint32_t dbkeys[] = {m_configsvc->masterKey(), m_configsvc->hltPrescaleKey()};
    for(int i = 0; i < 2; ++i) {
      ATH_MSG_VERBOSE("\t\t" << dbkeys[i] << "\t\t" << bskeys[i]);
      if(dbkeys[i]==0) chist->Fill(3*i+1);
      if(bskeys[i]==0) chist->Fill(3*i+2);
      if(dbkeys[i]!=bskeys[i]) chist->Fill(3*i+3);
    }

    rhist->Fill(1);
    if (HLTResult->isAccepted()) rhist->Fill(2); 
    if (HLTResult->isPassThrough()) rhist->Fill(3);
  }
  if(sc.isFailure()) ATH_MSG_INFO("failed filling result and consistenct for " << key);
  return sc;
}

StatusCode HLTMonTool::fillLvl1Histograms(){
  StatusCode sc = StatusCode::SUCCESS;
  for (std::vector<std::string>::iterator aItem = m_L1_summary.begin();
       aItem !=m_L1_summary.end(); ++aItem)
    if (getTDT()->isPassed(*aItem))
      hist("L1Events")->Fill((*aItem).c_str(),1);
  if(sc.isFailure()) ATH_MSG_INFO("failed filling LVL1 Histograms");
  return sc;
}

StatusCode HLTMonTool::fillForChain(const std::string& chain){
  StatusCode sc = StatusCode::SUCCESS;
  
  //get results
  std::map<std::string,bool> results;
  bool isEFChain = !(chain.compare(0,2,"EF"));
  bool isL2Chain = !(chain.compare(0,2,"L2"));
  if(!(isEFChain||isL2Chain)){
    ATH_MSG_DEBUG(chain << "is neither EF nor L2 chain");
    return StatusCode::FAILURE;
  }
 
  unsigned int cond = isEFChain ? TrigDefs::EF_passedRaw : TrigDefs::L2_passedRaw;
  results["RAW"] = getTDT()->isPassed(chain, cond);
  results["PS"] = results["RAW"] && (getTDT()->getPrescale(chain) != 1);
  results["PT"] = getTDT()->isPassed(chain, TrigDefs::eventAccepted);
  
  //set up iterators and such
  const std::string basepath = "HLT/ResultMon/";
  std::map<std::string,std::string> streams = m_regexlist;
  std::map<std::string,bool>::iterator rsIt;
  std::map<std::string,std::string>::const_iterator sIt = streams.begin();
  std::vector<std::string>::iterator rItr=m_rateregex.begin();
  
  
  
  
  //loop streams, if chain matched stream, loop result and if result is true fill
  for (;sIt!=streams.end();++sIt){ if (boost::regex_match(chain.c_str(),boost::regex(sIt->second))) {
      for (rsIt=results.begin();rsIt!=results.end(); rsIt ++){ if (rsIt->second) {
	  ATH_MSG_DEBUG(chain << " passed " << rsIt->first << " and matches " << *sIt);
	  const std::string monpath = basepath+sIt->first;
	  const std::string name = chain.substr(0,3)+sIt->first+rsIt->first;
	  
	  // ------------ Standard Fill ---------------------
	  hist(name,monpath)->Fill(chain.c_str(),1);
	  
	  // ------------ Fill Rate Histograms --------------
	  if(m_scopeLumi){ for(;rItr!=m_rateregex.end();++rItr){ if((*rItr)==chain) {
		ATH_MSG_DEBUG("lumi block number:" << getLumiBlockNr());
		IOVTime runlumitime = IOVTime(getRunNr(),getLumiBlockNr());
		double LBduration;
		std::map<cool::ValidityKey, cool::UInt63>::iterator t1,t2;
		t1=m_lbStartTimes.find(runlumitime.re_time());
		t2=m_lbStopTimes.find(runlumitime.re_time());
		if((t1!=m_lbStartTimes.end())&&(t2!=m_lbStopTimes.end())) {
		  LBduration = (t1->second - t2->second)/1.0e+09;
		  hist(chain,monpath+"/Rates")->Fill(((std::string)runlumitime).c_str(),LBduration);
		}
		ATH_MSG_INFO((std::string)runlumitime);
	      }
	    }
	  }
	  
	  // ------------ Fill RoI Histograms ---------------
	  if (rsIt->first=="RAW" && isEFChain) {
	    std::vector<Trig::Feature<TrigRoiDescriptor> >::const_iterator roiIt;
	    const std::vector<Trig::Feature<TrigRoiDescriptor> > rois = (getTDT()->features(chain)).get<TrigRoiDescriptor>("initialRoI"); 
	    for (roiIt=rois.begin(); roiIt!=rois.end(); ++roiIt) {
	      const TrigRoiDescriptor* roi = roiIt->cptr();
	      if(!roi) ATH_MSG_DEBUG("\tmissing roi pointer");
	      else hist2(sIt->first + "RoIs",monpath)->Fill(roi->eta(),roi->phi());
	    }
	  }

	}
      }
    }
  }
  if(sc.isFailure()) ATH_MSG_INFO("failed filling for chain " << chain);
  return sc;
}
