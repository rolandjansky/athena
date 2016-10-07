/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <sstream>
#include <map>
#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include "TrigMonitorBase/TrigLBNHist.h"

#include <boost/tokenizer.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/ThreadGaudi.h"
#include "TrigInterfaces/Algo.h"
#include "TrigInterfaces/IMonitoredAlgo.h"
#include "TrigGenericMonitoringTool.h"

using namespace std;

TrigGenericMonitoringTool::TrigGenericMonitoringTool(const std::string & type, 
						     const std::string & name,
						     const IInterface* parent)
  : TrigMonitorToolBase(type, name, parent)
{ 
  declareProperty("Histograms", m_histograms, "Definitions of histograms");
  declareInterface<IMonitorToolBase>(this);
}

TrigGenericMonitoringTool::~TrigGenericMonitoringTool() {
}


StatusCode TrigGenericMonitoringTool::bookHists() {

  ATH_MSG_DEBUG("bookHists");
  
  // Clear private members (bookHists can be called multiple times)
  m_fillers.clear();

  ATH_CHECK(service("THistSvc", m_rootHistSvc));
  
  m_algo = dynamic_cast<const IMonitoredAlgo*>(parent());
  if ( !m_algo ) {
    ATH_MSG_WARNING("Attached to algorithm which is not of type IMonitoredAlgo");
    return StatusCode::FAILURE;
  }
  const INamedInterface* parentAlg = dynamic_cast<const INamedInterface*>(parent());
  if (parentAlg==0) {
    ATH_MSG_ERROR("Cannot retrieve INamedInterface of parent algorithm");
    return StatusCode::FAILURE;
  }
  
  m_parentName =  parentAlg->name();

  
  histogramCategory["EXPERT"]   = new TrigMonGroup ( this, m_parentName, expert );
  histogramCategory["SHIFT"]    = new TrigMonGroup ( this, m_parentName, shift );
  histogramCategory["DEBUG"]    = new TrigMonGroup ( this, m_parentName, debug );
  histogramCategory["RUNSTAT"]  = new TrigMonGroup ( this, m_parentName, runstat );
  histogramCategory["EXPRESS"]  = new TrigMonGroup ( this, m_parentName, express );

  for (vector<string>::const_iterator citem = m_histograms.begin(); citem != m_histograms.end(); ++citem ) {
    HistogramDef def = parseJobOptHistogram(*citem);
    if ( ! def.ok || createFiller(def).isFailure() ) {
      ATH_MSG_DEBUG("Demand to monitor variable(s): " << def.name  << " can't be satisfied");
    }
  }

  if ( m_fillers.empty() ) {
    ATH_MSG_ERROR("No variables to be monitored, detach this tool, it will save time");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

void TrigGenericMonitoringTool::setOpts(TH1* histo, const std::string& opt) {
  // try to apply an option
  if ( opt.find("kCanRebin") != std::string::npos ) {
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0)
     histo->SetCanExtend(TH1::kAllAxes);
  }else {
     histo->SetCanExtend(TH1::kNoAxis);  
#else
    histo->SetBit(TH1::kCanRebin);
  } else {
    histo->ResetBit(TH1::kCanRebin);
#endif    
  }
  // try to apply option to make Sumw2 in histogram
  if ( opt.find("Sumw2") != std::string::npos ) {
    histo->Sumw2();
  }
}

void TrigGenericMonitoringTool::setLabels(TH1* histo, const std::vector<std::string>& labels) {
  if ( ! labels.empty() ){
    for ( int i = 0; i < std::min( (int)labels.size(), (int)histo->GetNbinsX() ); ++i ) {
      int bin = i+1;
      histo->GetXaxis()->SetBinLabel(bin, labels[i].c_str());
      ATH_MSG_DEBUG("setting label X" <<  labels[i] << " for bin " << bin);
    }

    for ( int i = (int)histo->GetNbinsX(); i < std::min( (int)labels.size(), (int)histo->GetNbinsX()+(int)histo->GetNbinsY() ); ++i ) {
      int bin = i+1-(int)histo->GetNbinsX();
      histo->GetYaxis()->SetBinLabel(bin, labels[i].c_str());
      ATH_MSG_DEBUG("setting label Y" <<  labels[i] << " for bin " << bin);
    }
  }
}



template<class H> 
TH1* TrigGenericMonitoringTool::create1D( TH1*& histo, ITrigLBNHist*& histoLBN,
					     const HistogramDef& def ) {
  H* htemp = new H( def.alias.c_str(), def.title.c_str(), def.xbins, def.xmin, def.xmax );
  histo = htemp;

  if ( def.opt.find("kLBN") != std::string::npos ) {
      TrigLBNHist<H>* lh = new TrigLBNHist<H>(*htemp);
      delete htemp; histo = lh; histoLBN = lh;
  }  
  return histo;
}

template<class H> 
TH1* TrigGenericMonitoringTool::createProfile( TProfile*& histo, ITrigLBNHist*& histoLBN,
					     const HistogramDef& def ) {
  H* htemp = new H( def.alias.c_str(), def.title.c_str(), def.xbins, def.xmin, def.xmax, def.ymin, def.ymax );
  histo = htemp;

  if ( def.opt.find("kLBN") != std::string::npos ) {
      TrigLBNHist<H>* lh = new TrigLBNHist<H>(*htemp);
      delete htemp; histo = lh; histoLBN = lh;
  }  
  return histo;
}

template<class H> 
TH1* TrigGenericMonitoringTool::create2D( TH2*& histo, ITrigLBNHist*& histoLBN,
					     const HistogramDef& def ) {
  H* htemp = new H( def.alias.c_str(), def.title.c_str(), 
		    def.xbins, def.xmin, def.xmax, def.ybins, def.ymin, def.ymax  );
  histo = htemp;

  if ( def.opt.find("kLBN") != std::string::npos ) {
      TrigLBNHist<H>* lh = new TrigLBNHist<H>(*htemp);
      delete htemp; histo = lh; histoLBN = lh;
  }  
  return histo;
}

template<class H> 
TH1* TrigGenericMonitoringTool::create2DProfile( TProfile2D*& histo, ITrigLBNHist*& histoLBN,
					     const HistogramDef& def ) {
  H* htemp = new H( def.alias.c_str(), def.title.c_str(), 
  		    def.xbins, def.xmin, def.xmax, def.ybins, def.ymin, def.ymax, def.zmin, def.zmax  );
  histo = htemp;

  if ( def.opt.find("kLBN") != std::string::npos ) {
      TrigLBNHist<H>* lh = new TrigLBNHist<H>(*htemp);
      delete htemp; histo = lh; histoLBN = lh;
  }  
  return histo;
}

StatusCode TrigGenericMonitoringTool::createFiller(const HistogramDef& def) {
  TH1* histo(0);
  TH1* histo1D(0);
  TProfile* histoProfile(0);
  TH2* histo2D(0);
  TProfile2D* histo2DProfile(0);
  ITrigLBNHist* histoLBN(0);
  // create 1D 



  // create 1D
  if (def.type == "TH1F") 
    histo = create1D<TH1F>(histo1D, histoLBN, def );

  else if (def.type == "TH1D") 
    histo = create1D<TH1D>(histo1D, histoLBN, def );
  
  else if (def.type == "TH1I") 
    histo = create1D<TH1I>(histo1D, histoLBN, def );

  else if (def.type == "TProfile") {
    histo = createProfile<TProfile>(histoProfile, histoLBN, def);
  }
  

  //create 2D
  else if (def.type == "TH2F")
    histo = create2D<TH2F>(histo2D, histoLBN, def );

  else if (def.type == "TH2D")
    histo = create2D<TH2D>(histo2D, histoLBN, def );

  else if (def.type == "TH2I") 
    histo = create2D<TH2I>(histo2D, histoLBN, def );
  
  else if (def.type == "TProfile2D"){
    histo = create2DProfile<TProfile2D>(histo2DProfile, histoLBN, def);
  }
  
  if (histo == 0 ) {
    ATH_MSG_WARNING("Can not create yet histogram of type: " << def.type);
    ATH_MSG_WARNING("Try one of: TH1F,TH1D,TH1I,TH2F,TH2D,TH2I, TProfile, TProfile2D");
    return StatusCode::FAILURE;
  }

  setLabels(histo, def.labels);
  setOpts(histo, def.opt);

  //hook
  

  
  // create filler for 1D
  if ( histo1D && !histoProfile){
    std::map<std::string, IMonitoredAlgo::IGetter*>::const_iterator varIt;
    varIt = m_algo->variables().find(def.name[0]);
    if ( varIt != m_algo->variables().end() ) {
      
      if ( def.opt.find("kCumulative") != std::string::npos ) {
        ATH_MSG_DEBUG("Variable: " << def.name << " from parent algorithm: "
                      << m_parentName << " will be histogrammed in Cummulative histogram");
        HistogramFiller* f = new CumulativeHistogramFiller1D(histo1D, varIt->second);
        m_fillers.push_back(f);
      } 
      else if (def.opt.find("kVecUO") != std::string::npos) {
        ATH_MSG_DEBUG("Variable: " << def.name << " from parent algorithm: "
                      << m_parentName << " will be added to histogram");
        if (unsigned(histo1D->GetNbinsX()+2) != varIt->second->size()) 
          ATH_MSG_WARNING("Variable: " << def.name << " from parent algorithm: " << m_parentName 
                          << " has different dimension: " << varIt->second->size() 
                          << " than hisogram: " << histo1D->GetNbinsX()
                          << " booked for it and kVecUO options is requested (variable has to accomodate Under/Overflows too)");

      	HistogramFiller* f = new VecHistogramFiller1DWithOverflows(histo1D, varIt->second);
        m_fillers.push_back(f);
        
      } 
      else if (def.opt.find("kVec") != std::string::npos) {
        ATH_MSG_DEBUG("Variable: " << def.name << " from parent algorithm: "
                      << m_parentName << " will be added to histogram");
        if (unsigned(histo1D->GetNbinsX()) != varIt->second->size()) 
          ATH_MSG_WARNING("Variable: " << def.name << " from parent algorithm: " << m_parentName 
                          << " has different dimension: " << varIt->second->size() 
                          << " than hisogram: " << histo1D->GetNbinsX()
                          << " booked for it and kVec options is requested");
       
        HistogramFiller* f = new VecHistogramFiller1D(histo1D, varIt->second);
        m_fillers.push_back(f);
 
      } 
      else {
        ATH_MSG_DEBUG("Variable: " << def.name << " from parent algorithm: " << m_parentName
                      <<  " will be histogrammed");
        HistogramFiller* f = new HistogramFiller1D(histo1D, varIt->second);
        m_fillers.push_back(f);
      }
    } else {
      ATH_MSG_WARNING("Variable: " << def.name << " not exported by parent algorithm: " << m_parentName  << " this are available:");
      for ( varIt = m_algo->variables().begin(); varIt != m_algo->variables().end(); ++varIt) {
        msg() << MSG::WARNING << varIt->first << " ";	
      }
      msg() << MSG::WARNING << endreq;
      
      return StatusCode::FAILURE;
    }

  } else if ( histo2DProfile ){
    std::map<std::string, IMonitoredAlgo::IGetter*>::const_iterator var1It, var2It, var3It;
    var1It = m_algo->variables().find(def.name[0]);
    var2It = m_algo->variables().find(def.name[1]);
    var3It = m_algo->variables().find(def.name[2]);

    if ( var1It != m_algo->variables().end() && var2It != m_algo->variables().end() && var3It != m_algo->variables().end() ) {
      ATH_MSG_DEBUG("Variables: " << def.name[0] << "," << def.name[1] << "," << def.name[2]
                    << " from parent algorithm: " << m_parentName
                    << " will be histogrammed in 2D Profile histogram");
      m_fillers.push_back(new HistogramFiller2DProfile(histo2DProfile, var1It->second, var2It->second, var3It->second));		  
    }
    else{
      ATH_MSG_WARNING("Variables: " << def.name[0] << "," << def.name[1] << "," << def.name[2] << " not exported by parent algorithm: " << m_parentName);
      for ( var1It = m_algo->variables().begin(); var1It != m_algo->variables().end(); ++var1It) {
        msg() << MSG::WARNING << var1It->first << " ";	
      }
      msg() << MSG::WARNING << endreq;
      return StatusCode::FAILURE;
    }

  } else if ( histo2D || histoProfile ){
    std::map<std::string, IMonitoredAlgo::IGetter*>::const_iterator var1It, var2It;
    var1It = m_algo->variables().find(def.name[0]);
    var2It = m_algo->variables().find(def.name[1]);
    
    if ( var1It != m_algo->variables().end() && var2It != m_algo->variables().end() ) {
      ATH_MSG_DEBUG("Variables: " << def.name[0] << "," << def.name[1]
                    << " from parent algorithm: " << m_parentName
                    << " will be histogrammed in 2D histogram");
      if (histo2D)
        m_fillers.push_back(new HistogramFiller2D(histo2D, var1It->second, var2It->second));
      else {       
        m_fillers.push_back(new HistogramFillerProfile(histoProfile, var1It->second, var2It->second));
      }
    }
    else {
      ATH_MSG_WARNING("Variables: " << def.name[0] << "," << def.name[1] << " not exported by parent algorithm: " << m_parentName);
      for ( var1It = m_algo->variables().begin(); var1It != m_algo->variables().end(); ++var1It) {
        msg() << MSG::WARNING << var1It->first << " ";	
      }
      msg() << MSG::WARNING << endreq;
      return StatusCode::FAILURE;
    }
  }
  
  //  std::string path = generatePath(def.path, histo->GetName());
  //  msglog << MSG::DEBUG << "Variable: " << histo->GetName() << " will be histogrammed in histogram booked in: " << path << " with title: " << def.title << endreq;  


  if ( def.opt.find("kLBN") != std::string::npos ) {
    if ( histogramCategory[def.path]->regHist(histoLBN).isFailure() ) {
      ATH_MSG_WARNING("LBN Histogram for variable: " << def.name << " for parent algorithm: " << m_parentName << " can't be booked");   
    }
  } else {
    if ( histogramCategory[def.path]->regHist(histo).isFailure() ) {
      ATH_MSG_WARNING("Histogram for variable: " << def.name << " for parent algorithm: " << m_parentName << " can't be booked");
    }
  }
  
  return StatusCode::SUCCESS;
  }

StatusCode TrigGenericMonitoringTool::fillHists() {
  unsigned fills(0);
  std::vector<HistogramFiller*>::iterator i;
  for( i = m_fillers.begin(); i != m_fillers.end(); ++i ) {
    fills += (*i)->fill();
  }

  // Can be useful for debugging, in case one suspects a monitored container with ever increasing size
  ATH_MSG_DEBUG(fills << " histogram fills done");
  
  return StatusCode::SUCCESS;
}


StatusCode TrigGenericMonitoringTool::finalHists() {
  std::map<std::string, TrigMonGroup*>::const_iterator i;

  for ( i = histogramCategory.begin() ; i != histogramCategory.end(); ++i) {
    delete i->second;
  }
  histogramCategory.clear();
  

  std::vector<HistogramFiller*>::iterator fIt;   
  for ( fIt = m_fillers.begin(); fIt != m_fillers.end(); ++fIt ) {
    delete (*fIt);
  }
  m_fillers.clear();

  return StatusCode::SUCCESS;
}


const TrigGenericMonitoringTool::HistogramDef TrigGenericMonitoringTool::parseJobOptHistogram(const std::string& histDef) {
  /* Parse histogram defintion
     Example:
     1D: "EXPERT, TH1I, Name, Title;Alias, nBins, xmin, xmax, BinLabel1:BinLabel2:BinLabel3, kCumulative"
  */

  ATH_MSG_DEBUG("parseJobOptHistogram(\"" << histDef << "\")");
  
  // convert histogram definition string to an array of strings
  list<string> histProperty;
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer_t;
  boost::char_separator<char> sep(","); 
  tokenizer_t tokens(histDef, sep);
  for (tokenizer_t::iterator itr = tokens.begin(); itr != tokens.end(); ++itr) {
    string word = *itr;
    boost::trim(word);
    histProperty.push_back(word); 
  } 
  // return value
  HistogramDef histPar;
  list<string>::iterator itr = histProperty.begin();
  
  // stream
  string word = (*itr);
  boost::to_upper(word);
  //if (word.find("EXPERT") != string::npos || word.find("SHIFT") != string::npos) {
  if (word == "EXPERT" || word == "SHIFT" || word == "DEBUG" || word == "RUNSTAT" ) {
    histPar.path = word;
    itr = histProperty.erase(itr);
  }
  else
    histPar.path = "EXPERT";
  
  const char* warning = " NOT booked: ";
  histPar.ok   = false;
  histPar.ycut = false;
  histPar.zcut = false;
  
  if (histProperty.size() < 5) return histPar;   
  
  histPar.type = (*itr);
  itr = histProperty.erase(itr);

  

  histPar.name.push_back(*itr);
  itr = histProperty.erase(itr);
  if (histPar.type.find("TH2") == 0 || histPar.type == "TProfile") {
    histPar.name.push_back(*itr);
    itr = histProperty.erase(itr);
  }
  else if (histPar.type == "TProfile2D") {
    //twice more as there are three variables
    histPar.name.push_back(*itr);
    itr = histProperty.erase(itr);
    histPar.name.push_back(*itr);
    itr = histProperty.erase(itr);
  }


  // find if names contain alias 
  string::size_type semicolon_pos = histPar.name.back().find(';');
  if (  semicolon_pos != string::npos ) { // remove what is after the ; and set alias
    string actual_name = histPar.name.back().substr(0, semicolon_pos);    
    histPar.alias = histPar.name.back().substr(semicolon_pos+1);
    histPar.name.back() = actual_name;
  } else {
    histPar.alias = histPar.name[0];
    if ( histPar.name.size() == 2 ) {
      histPar.alias += "_vs_"+histPar.name[1];
    }
    else if (histPar.name.size() == 3 ){
      histPar.alias += "_vs_"+histPar.name[1]+"_vs_"+histPar.name[2];
    }
  }

  histPar.title = (*itr);
  itr = histProperty.erase(itr);
  
  if (histProperty.size() < 2) {
    ATH_MSG_WARNING(histPar.alias << warning << "NOT enough parameters for defining 1-D histogram");
    return histPar;   
  }
  
  try {
    histPar.xbins = boost::lexical_cast<int>(*itr);
    itr = histProperty.erase(itr);
  }
  catch (boost::bad_lexical_cast&) {
    ATH_MSG_WARNING(histPar.alias << warning << "int expected for xbins while got"  << histProperty);
    return histPar;
  }
  
  if (histProperty.size() < 2) {
    ATH_MSG_WARNING(histPar.name[0] << warning << "xmin and xmax expected");
    return histPar;
  }
  
  try {
    histPar.xmin = boost::lexical_cast<double>(*itr);
    itr = histProperty.erase(itr);
  }
  catch (boost::bad_lexical_cast&) {
    ATH_MSG_WARNING(histPar.name[0] << warning << "double expected for xmin");
    return histPar;
  }
  
  try {
    histPar.xmax = boost::lexical_cast<double>(*itr);
    itr = histProperty.erase(itr);
  }
  catch (boost::bad_lexical_cast&) {
    ATH_MSG_WARNING(histPar.name[0] << warning << "double expected for xmax");
    return histPar;
  }
  
  
  
  if (histPar.type.find("TH2") == 0) {
    if (histProperty.size() < 2) {
      ATH_MSG_WARNING(histPar.name[0] << warning << "y-axis definition expected for TH2");
      return histPar;
    }
    
    try {
      histPar.ybins = boost::lexical_cast<int>(*itr);
      itr = histProperty.erase(itr);
    }
    catch (boost::bad_lexical_cast&) {
      ATH_MSG_WARNING(histPar.name[0] << warning << "int expected for ybins");
      return histPar;
    }
    
    if (histProperty.size() < 2) {
      ATH_MSG_WARNING(histPar.name[0] << warning << "ymin and ymax expected");
      return histPar;
    }
    
    try {
      histPar.ymin = boost::lexical_cast<double>(*itr);
      itr = histProperty.erase(itr);
    }
    catch (boost::bad_lexical_cast&) {
      ATH_MSG_WARNING(histPar.name[0] << warning << "double expected for ymin");
      return histPar;
    }
    
    try {
      histPar.ymax = boost::lexical_cast<double>(*itr);
      itr = histProperty.erase(itr);
    }
    catch (boost::bad_lexical_cast&) {
      ATH_MSG_WARNING(histPar.name[0] << warning << "double expected for ymax");
      return histPar;
    }
  } //-end of TH2
  //TProfile
  else if (histPar.type == "TProfile" && histProperty.size() >= 2) {
    // limited y-range
    try {
      histPar.ymin = boost::lexical_cast<double>(*itr);
      itr = histProperty.erase(itr);
    }
    catch (boost::bad_lexical_cast&) {
      ATH_MSG_WARNING(histPar.name[0] << warning << "double expected for ymin of TProfile");
      return histPar;
    }
    
    try {
      histPar.ymax = boost::lexical_cast<double>(*itr);
      itr = histProperty.erase(itr);
    }
    catch (boost::bad_lexical_cast&) {
      ATH_MSG_WARNING(histPar.name[0] << warning << "double expected for ymax of TProfile");
      return histPar;
    }
    histPar.ybins = 0; // not used
    histPar.ycut = true;
  }
  //TProfile2D
  else if (histPar.type == "TProfile2D"){

    if (histProperty.size() < 2) {
      ATH_MSG_WARNING(histPar.name[0] << warning << "y-axis definition expected for TProfile2D");
      return histPar;
    }
    
    try {
      histPar.ybins = boost::lexical_cast<int>(*itr);
      itr = histProperty.erase(itr);
    }
    catch (boost::bad_lexical_cast&) {
      ATH_MSG_WARNING(histPar.name[0] << warning << "int expected for ybins");
      return histPar;
    }
    
    if (histProperty.size() < 2) {
      ATH_MSG_WARNING(histPar.name[0] << warning << "ymin and ymax expected");
      return histPar;
    }
    
    try {
      histPar.ymin = boost::lexical_cast<double>(*itr);
      itr = histProperty.erase(itr);
    }
    catch (boost::bad_lexical_cast&) {
      ATH_MSG_WARNING(histPar.name[0] << warning << "double expected for ymin");
      return histPar;
    }
    
    try {
      histPar.ymax = boost::lexical_cast<double>(*itr);
      itr = histProperty.erase(itr);
    }
    catch (boost::bad_lexical_cast&) {
      ATH_MSG_WARNING(histPar.name[0] << warning << "double expected for ymax");
      return histPar;
    }
    //For limited z range
    if(histProperty.size() >= 2){
      try {
        histPar.zmin = boost::lexical_cast<double>(*itr);
        itr = histProperty.erase(itr);
      }
      catch (boost::bad_lexical_cast&) {
        ATH_MSG_WARNING(histPar.name[0] << warning << "double expected for zmin of TProfile2D");
        return histPar;
      }
    
      try {
        histPar.zmax = boost::lexical_cast<double>(*itr);
        itr = histProperty.erase(itr);
      }
      catch (boost::bad_lexical_cast&) {
        ATH_MSG_WARNING(histPar.name[0] << warning << "double expected for zmax of TProfile2D");
        return histPar;
      }

      histPar.zcut = true;
    }
  }

  if (itr->find(":") != std::string::npos ) { // it means that last paramater has format str1:str2:str3:str4 which means this are bins labels
    // breate it 
    boost::char_separator<char> colon(":");
    tokenizer_t labels(*itr, colon);
    for ( tokenizer_t::iterator l = labels.begin(); l != labels.end(); ++l ) {
      histPar.labels.push_back(*l);
    }
    itr = histProperty.erase(itr);
  }
  
  // opt
  if (itr != histProperty.end()) {
    histPar.opt = (*itr);
  }
    
  histPar.ok = true;
  return histPar;
}



/////////////////////////////////////////////////////////////////////////////
// fillers
/////////////////////////////////////////////////////////////////////////////

unsigned TrigGenericMonitoringTool::HistogramFiller1D::fill() {
  unsigned i(0);
  for (; i < m_variable->size() ; ++i )
    m_histogram->Fill(m_variable->get(i));
  return i;
} 

unsigned TrigGenericMonitoringTool::CumulativeHistogramFiller1D::fill() {
  unsigned i(0);
  for (; i < m_variable->size() ; ++i ) {
    unsigned bin = m_histogram->FindBin( m_variable->get(i) );
    for ( unsigned j = bin; j > 0; --j) 
      m_histogram->AddBinContent(j);
  }
  return i;  
}


unsigned TrigGenericMonitoringTool::VecHistogramFiller1D::fill() {
  unsigned i(0);  
  for (; i < m_variable->size() ; ++i ) {
    double v = m_variable->get(i);
    m_histogram->AddBinContent(i+1, v);
    m_histogram->SetEntries(m_histogram->GetEntries() + v );
  }
  return i;
}



unsigned TrigGenericMonitoringTool::VecHistogramFiller1DWithOverflows::fill() {
  unsigned i(0);
  for (; i < m_variable->size() ; ++i ) {
    double v = m_variable->get(i);
    m_histogram->AddBinContent(i, v); 
    m_histogram->SetEntries(m_histogram->GetEntries() + v );
    //cout << "2 called with " << v << endl;
  }
  return i;
}

TrigGenericMonitoringTool::HistogramFillerProfile::HistogramFillerProfile(TProfile* hist, IMonitoredAlgo::IGetter* var1, 
								IMonitoredAlgo::IGetter* var2) 
  : m_histogram(hist), m_variable1(var1), m_variable2(var2) {
}  

unsigned TrigGenericMonitoringTool::HistogramFillerProfile::fill() {
  if ( !m_histogram )
    return 0;

  unsigned i(0);
  if ( m_variable1->size() != m_variable2->size() ) {
    if ( m_variable1->size() == 1 ) {
      // first variable is scalar -- loop over second
      for ( i = 0; i < m_variable2->size() ; ++i )
	m_histogram->Fill(m_variable1->get(0), m_variable2->get(i));  
    } else if (m_variable2->size() == 1)  {
      // second varaible is scalar -- loop oer first
      for ( i = 0; i < m_variable1->size() ; ++i )
	m_histogram->Fill(m_variable1->get(i), m_variable2->get(0));  
    }
    return i;
  }

  for ( i = 0; i < m_variable1->size() ; ++i )
    m_histogram->Fill(m_variable1->get(i), m_variable2->get(i));
  
  return i;
}

TrigGenericMonitoringTool::HistogramFiller2DProfile::HistogramFiller2DProfile(TProfile2D* hist, IMonitoredAlgo::IGetter* var1, 
									      IMonitoredAlgo::IGetter* var2,
									      IMonitoredAlgo::IGetter* var3) 
  : m_histogram(hist), m_variable1(var1), m_variable2(var2), m_variable3(var3) {
}  

unsigned TrigGenericMonitoringTool::HistogramFiller2DProfile::fill() {
  if ( !m_histogram )
    return 0;

  unsigned i(0);
  /*HERE NEED TO INCLUDE CASE IN WHICH SOME VARIABLES ARE SCALAR AND SOME VARIABLES ARE VECTORS
  unsigned i(0);
  if (m_variable1->size() != m_variable2->size() || m_variable1->size() != m_variable3->size() || m_variable2->size() != m_variable3->size() ) {

  }*/

  //For now lets just consider the case in which all variables are of the same length
  for ( i = 0; i < m_variable1->size() ; ++i )
    m_histogram->Fill(m_variable1->get(i), m_variable2->get(i), m_variable3->get(i));
  
  return i;
}




TrigGenericMonitoringTool::HistogramFiller2D::HistogramFiller2D(TH2* hist, IMonitoredAlgo::IGetter* var1, 
								IMonitoredAlgo::IGetter* var2) 
  : m_histogram(hist), m_variable1(var1), m_variable2(var2) {
}  

unsigned TrigGenericMonitoringTool::HistogramFiller2D::fill() {
  if ( !m_histogram )
    return 0;

  unsigned i(0);
  if ( m_variable1->size() != m_variable2->size() ) {
    if ( m_variable1->size() == 1 ) {
      // first variable is scalar -- loop over second
      for ( i = 0; i < m_variable2->size() ; ++i )
	m_histogram->Fill(m_variable1->get(0), m_variable2->get(i));  
    } else if (m_variable2->size() == 1)  {
      // second varaible is scalar -- loop over first
      for ( i = 0; i < m_variable1->size() ; ++i )
	m_histogram->Fill(m_variable1->get(i), m_variable2->get(0));  
    }
    return i;
  }

  for ( i = 0; i < m_variable1->size() ; ++i )
    m_histogram->Fill(m_variable1->get(i), m_variable2->get(i));

  return i;
}

