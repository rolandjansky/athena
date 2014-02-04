/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <sstream>
#include <map>
#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>
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
  : TrigMonitorToolBase(type, name, parent),
    m_log(msgSvc(), name)
{ 
  declareProperty("Histograms", m_histograms, "Definitions of histograms");
  declareInterface<IMonitorToolBase>(this);
}

TrigGenericMonitoringTool::~TrigGenericMonitoringTool() {
}

StatusCode TrigGenericMonitoringTool::initialize() {
  if ( TrigMonitorToolBase::initialize().isFailure() )
    return StatusCode::FAILURE;

  m_log.setLevel(outputLevel());
  return StatusCode::SUCCESS;
}

StatusCode TrigGenericMonitoringTool::bookHists() {

  if (outputLevel()<=MSG::DEBUG) {
    m_log << MSG::DEBUG << "bookHists" << endreq;
  }
  
  // Clear private members (bookHists can be called multiple times)
  m_fillers.clear();


  if (!service("THistSvc", m_rootHistSvc).isSuccess()) {
    m_log << MSG::ERROR << "Unable to locate THistSvc" << endreq;
    return StatusCode::FAILURE;
  }
  
  m_algo = dynamic_cast<const IMonitoredAlgo*>(parent());
  if ( !m_algo ) {
    m_log << MSG::WARNING << "Attached to algorithm which is not of type IMonitoredAlgo" << endreq;
    return StatusCode::FAILURE;
  }
  const INamedInterface* parentAlg = dynamic_cast<const INamedInterface*>(parent());
  m_parentName =  parentAlg->name();

  
  histogramCategory["EXPERT"]   = new TrigMonGroup ( this, m_parentName, expert );
  histogramCategory["SHIFT"]    = new TrigMonGroup ( this, m_parentName, shift );
  histogramCategory["DEBUG"]    = new TrigMonGroup ( this, m_parentName, debug );
  histogramCategory["RUNSTAT"]  = new TrigMonGroup ( this, m_parentName, runstat );
  histogramCategory["EXPRESS"]  = new TrigMonGroup ( this, m_parentName, express );

  for (vector<string>::const_iterator citem = m_histograms.begin(); citem != m_histograms.end(); ++citem ) {
    HistogramDef def = parseJobOptHistogram(*citem);
    if ( ! def.ok || createFiller(def).isFailure() ) {
      m_log << MSG::DEBUG << "Demand to monitor variable(s): " << def.name  << " can't be satisfied"  << endreq;       
    }
  }

  if ( m_fillers.empty() ) {
    m_log << MSG::ERROR << "No variables to be monitored, detach this tool, it will save time" << endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

void TrigGenericMonitoringTool::setOpts(TH1* histo, const std::string& opt) {
  // try to apply an option
  if ( opt.find("kCanRebin") != std::string::npos ) {
    histo->SetBit(TH1::kCanRebin);
  } else {
    histo->ResetBit(TH1::kCanRebin);
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
      if (outputLevel()<=MSG::DEBUG) {
        m_log << MSG::DEBUG << "setting label X" <<  labels[i] << " for bin " << bin << endreq;
      }
    }

    for ( int i = (int)histo->GetNbinsX(); i < std::min( (int)labels.size(), (int)histo->GetNbinsX()+(int)histo->GetNbinsY() ); ++i ) {
      int bin = i+1-(int)histo->GetNbinsX();
      histo->GetYaxis()->SetBinLabel(bin, labels[i].c_str());
      if (outputLevel()<=MSG::DEBUG) {
        m_log << MSG::DEBUG << "setting label Y" <<  labels[i] << " for bin " << bin << endreq;
      }
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

StatusCode TrigGenericMonitoringTool::createFiller(const HistogramDef& def) {
  TH1* histo(0);
  TH1* histo1D(0);
  TProfile* histoProfile(0);
  TH2* histo2D(0);
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
    histo = create1D<TProfile>(histo1D, histoLBN, def );
    histoProfile = dynamic_cast<TProfile*>(histo1D);
  }
  

  //create 2D
  else if (def.type == "TH2F")
    histo = create2D<TH2F>(histo2D, histoLBN, def );

  else if (def.type == "TH2D")
    histo = create2D<TH2D>(histo2D, histoLBN, def );

  else if (def.type == "TH2I") 
    histo = create2D<TH2I>(histo2D, histoLBN, def );
  
  
  if (histo == 0 ) {
    m_log << MSG::WARNING << "Can not create yet histogram of type: " << def.type << endreq;
    m_log << MSG::WARNING << "Try one of: TH1F,TH1D,TH1I,TH2F,TH2D,TH2I, TProfile" << endreq;
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
        if (outputLevel()<=MSG::DEBUG) m_log << MSG::DEBUG << "Variable: " << def.name << " from parent algorithm: "
                                             << m_parentName << " will be histogrammed in Cummulative histogram" << endreq;
	HistogramFiller* f = new CumulativeHistogramFiller1D(histo1D, varIt->second);
	m_fillers.push_back(f);
      } else if (def.opt.find("kVecUO") != std::string::npos) {
	if (outputLevel()<=MSG::DEBUG) m_log << MSG::DEBUG << "Variable: " << def.name << " from parent algorithm: "
                                             << m_parentName << " will be added to histogram" << endreq;
	if (unsigned(histo1D->GetNbinsX()+2) != varIt->second->size()) 
	  m_log << MSG::WARNING << "Variable: " << def.name << " from parent algorithm: " << m_parentName 
		 << " has different dimension: " << varIt->second->size() 
		 << " than hisogram: " << histo1D->GetNbinsX()
		 << " booked for it and kVecUO options is requested (variable has to accomodate Under/Overflows too)" <<  endreq;

      	HistogramFiller* f = new VecHistogramFiller1DWithOverflows(histo1D, varIt->second);
	m_fillers.push_back(f);

      } else if (def.opt.find("kVec") != std::string::npos) {
	if (outputLevel()<=MSG::DEBUG) m_log << MSG::DEBUG << "Variable: " << def.name << " from parent algorithm: "
                                             << m_parentName << " will be added to histogram" << endreq;
	if (unsigned(histo1D->GetNbinsX()) != varIt->second->size()) 
	  m_log << MSG::WARNING << "Variable: " << def.name << " from parent algorithm: " << m_parentName 
		 << " has different dimension: " << varIt->second->size() 
		 << " than hisogram: " << histo1D->GetNbinsX()
		 << " booked for it and kVec options is requested" <<  endreq;
       
	HistogramFiller* f = new VecHistogramFiller1D(histo1D, varIt->second);
	m_fillers.push_back(f);
 
      } else {
	if (outputLevel()<=MSG::DEBUG) m_log << MSG::DEBUG << "Variable: " << def.name << " from parent algorithm: " << m_parentName
                                             <<  " will be histogrammed" << endreq;
	HistogramFiller* f = new HistogramFiller1D(histo1D, varIt->second);
	m_fillers.push_back(f);
      }
    } else {
      m_log << MSG::WARNING << "Variable: " << def.name << " not exported by parent algorithm: " << m_parentName  << " this are available:" << endreq;
      for ( varIt = m_algo->variables().begin(); varIt != m_algo->variables().end(); ++varIt) {
	m_log << MSG::WARNING << varIt->first << " ";	
      }
      m_log << MSG::WARNING << endreq;
      
      return StatusCode::FAILURE;
    }
  } else if ( histo2D || histoProfile ){
    std::map<std::string, IMonitoredAlgo::IGetter*>::const_iterator var1It, var2It;
    var1It = m_algo->variables().find(def.name[0]);
    var2It = m_algo->variables().find(def.name[1]);
    
    if ( var1It != m_algo->variables().end() && var2It != m_algo->variables().end() ) {
      if (outputLevel()<=MSG::DEBUG) m_log << MSG::DEBUG << "Variables: " << def.name[0] << "," << def.name[1]
                                           << " from parent algorithm: " << m_parentName
                                           << " will be histogrammed in 2D histogram" << endreq;      
      if (histo2D)
	m_fillers.push_back(new HistogramFiller2D(histo2D, var1It->second, var2It->second));
      else {
	
	m_fillers.push_back(new HistogramFillerProfile(histoProfile, var1It->second, var2It->second));
      }
    } else {
      m_log << MSG::WARNING << "Variables: " << def.name[0] << "," << def.name[1] << " not exported by parent algorithm: " << m_parentName << endreq;
      for ( var1It = m_algo->variables().begin(); var1It != m_algo->variables().end(); ++var1It) {
	m_log << MSG::WARNING << var1It->first << " ";	
      }
      m_log << MSG::WARNING << endreq;
      return StatusCode::FAILURE;
    }
  }
  
  //  std::string path = generatePath(def.path, histo->GetName());
  //  msglog << MSG::DEBUG << "Variable: " << histo->GetName() << " will be histogrammed in histogram booked in: " << path << " with title: " << def.title << endreq;  


  if ( def.opt.find("kLBN") != std::string::npos ) {
    if ( histogramCategory[def.path]->regHist(histoLBN).isFailure() ) {
      m_log << MSG::WARNING << "LBN Histogram for variable: " << def.name << " for parent algorithm: " << m_parentName << " can't be booked" << endreq;   
    }
  } else {
    if ( histogramCategory[def.path]->regHist(histo).isFailure() ) {
      m_log << MSG::WARNING << "Histogram for variable: " << def.name << " for parent algorithm: " << m_parentName << " can't be booked" << endreq;   
    }
  }
  
  return StatusCode::SUCCESS;
  }

StatusCode TrigGenericMonitoringTool::fillHists() {
  //  m_log << MSG::DEBUG << "fillHists" << endreq;
  unsigned fills(0);
  std::vector<HistogramFiller*>::iterator i;
  for( i = m_fillers.begin(); i != m_fillers.end(); ++i ) {
    fills += (*i)->fill();
  }

  // Can be useful for debugging, in case one suspects a monitored container with ever increasing size
  if (outputLevel()<=MSG::DEBUG) {
    m_log << MSG::DEBUG << fills << " histogram fills done" << endreq;
  }
  
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


//wm: new parser
const TrigGenericMonitoringTool::HistogramDef TrigGenericMonitoringTool::parseJobOptHistogram(const std::string& histDef) {

  if (outputLevel()<=MSG::DEBUG) {
    m_log << MSG::DEBUG << "parseJobOptHistogram(\"" << histDef << "\")" << endreq;
  }
  
  // convert histogram definition string to an array of strings
  vector<string> histProperty;
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
  vector<string>::iterator itr = histProperty.begin();
  
  // stream
  string word = (*itr);
  boost::to_upper(word);
  //if (word.find("EXPERT") != string::npos || word.find("SHIFT") != string::npos) {
  if (word == "EXPERT" || word == "SHIFT" || word == "DEBUG" || word == "RUNSTAT" ) {
    histPar.path = word;
    histProperty.erase(itr);
  }
  else
    histPar.path = "EXPERT";
  
  const char* warning = " NOT booked: ";
  histPar.ok   = false;
  histPar.ycut = false;
  
  if (histProperty.size() < 5) return histPar;   
  
  histPar.type = (*itr);
  histProperty.erase(itr);

    

  histPar.name.push_back(*itr);
  histProperty.erase(itr);
  if (histPar.type.find("TH2") == 0 || histPar.type == "TProfile") {
    histPar.name.push_back(*itr);
    histProperty.erase(itr);
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
  }

  histPar.title = (*itr);
  histProperty.erase(itr);
  
  if (histProperty.size() < 2) {
    m_log << MSG::WARNING << histPar.alias << warning << "NOT enough parameters for defining 1-D histogram" << endreq;
    return histPar;   
  }
  
  try {
    histPar.xbins = boost::lexical_cast<int>(*itr);
    histProperty.erase(itr);
  }
  catch (boost::bad_lexical_cast&) {
    m_log << MSG::WARNING << histPar.alias << warning << "int expected for xbins while got"  << histProperty << endreq;
    return histPar;
  }
  
  if (histProperty.size() < 2) {
    m_log << MSG::WARNING << histPar.name[0] << warning << "xmin and xmax expected" << endreq;
    return histPar;
  }
  
  try {
    histPar.xmin = boost::lexical_cast<double>(*itr);
    histProperty.erase(itr);
  }
  catch (boost::bad_lexical_cast&) {
    m_log << MSG::WARNING << histPar.name[0] << warning << "double expected for xmin" << endreq;
    return histPar;
  }
  
  try {
    histPar.xmax = boost::lexical_cast<double>(*itr);
    histProperty.erase(itr);
  }
  catch (boost::bad_lexical_cast&) {
    m_log << MSG::WARNING << histPar.name[0] << warning << "double expected for xmax" << endreq;
    return histPar;
  }
  
  
  
  if (histPar.type.find("TH2") == 0) {
    if (histProperty.size() < 2) {
      m_log << MSG::WARNING << histPar.name[0] << warning << "y-axis definition expected for TH2" << endreq;
      return histPar;
    }
    
    try {
      histPar.ybins = boost::lexical_cast<int>(*itr);
      histProperty.erase(itr);
    }
    catch (boost::bad_lexical_cast&) {
      m_log << MSG::WARNING << histPar.name[0] << warning << "int expected for ybins" << endreq;
      return histPar;
    }
    
    if (histProperty.size() < 2) {
      m_log << MSG::WARNING << histPar.name[0] << warning << "ymin and ymax expected" << endreq;
      return histPar;
    }
    
    try {
      histPar.ymin = boost::lexical_cast<double>(*itr);
      histProperty.erase(itr);
    }
    catch (boost::bad_lexical_cast&) {
      m_log << MSG::WARNING << histPar.name[0] << warning << "double expected for ymin" << endreq;
      return histPar;
    }
    
    try {
      histPar.ymax = boost::lexical_cast<double>(*itr);
      histProperty.erase(itr);
    }
    catch (boost::bad_lexical_cast&) {
      m_log << MSG::WARNING << histPar.name[0] << warning << "double expected for ymax" << endreq;
      return histPar;
    }
  } //-end of TH2
  else if (histPar.type == "TProfile" && histProperty.size() >= 2) {
    // limited y-range
    try {
      histPar.ymin = boost::lexical_cast<double>(*itr);
      histProperty.erase(itr);
    }
    catch (boost::bad_lexical_cast&) {
      m_log  <<  MSG::WARNING  <<  histPar.name[0] << warning << "double expected for ymin of TProfile" << endreq;
      return histPar;
    }
    
    try {
      histPar.ymax = boost::lexical_cast<double>(*itr);
      histProperty.erase(itr);
    }
    catch (boost::bad_lexical_cast&) {
      m_log << MSG::WARNING << histPar.name[0] << warning << "double expected for ymax of TProfile" << endreq;
      return histPar;
    }
    histPar.ycut = true;
  }

  if (itr->find(":") != std::string::npos ) { // it means that last paramater has format str1:str2:str3:str4 which means this are bins labels
    // breate it 
    boost::char_separator<char> colon(":");
    tokenizer_t labels(*itr, colon);
    for ( tokenizer_t::iterator l = labels.begin(); l != labels.end(); ++l ) {
      histPar.labels.push_back(*l);
    }
    histProperty.erase(itr); 
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

