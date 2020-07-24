/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <sstream>
#include <map>
#include <mutex>
#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include "TrigMonitorBase/TrigLBNHist.h"

#include <boost/tokenizer.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "GaudiKernel/INamedInterface.h"
#include "TrigInterfaces/Algo.h"
#include "TrigInterfaces/IMonitoredAlgo.h"
#include "TrigGenericMonitoringTool.h"

using namespace std;


template <class M, class P>
TrigGenericMonitoringTool<M,P>::TrigGenericMonitoringTool(const std::string & type, 
                                                          const std::string & name,
                                                          const IInterface* parent)
  : TrigMonitorToolBase(type, name, parent)
{ 
  declareProperty("Histograms", m_histograms, "Definitions of histograms");
  declareInterface<IMonitorToolBase>(this);
}

template <class M, class P>
TrigGenericMonitoringTool<M,P>::~TrigGenericMonitoringTool() {
}

template <class M, class P>
StatusCode TrigGenericMonitoringTool<M,P>::bookHists() {

  ATH_MSG_DEBUG("bookHists");
  
  // Clear private members (bookHists can be called multiple times)
  m_fillers.clear();

  ATH_CHECK(service("THistSvc", m_rootHistSvc));
  
  m_algo = dynamic_cast<const IMonitoredAlgo*>(parent());  
  if ( !m_algo ) {
    ATH_MSG_DEBUG("Attached to algorithm which is not of type IMonitoredAlgo");
    //return StatusCode::FAILURE;
  }
  const INamedInterface* parentAlg = dynamic_cast<const INamedInterface*>(parent());
  if (parentAlg==0) {
    ATH_MSG_ERROR("Cannot retrieve INamedInterface of parent algorithm");
    return StatusCode::FAILURE;
  }
  
  m_parentName =  parentAlg->name();

  
  m_histogramCategory["EXPERT"]   = new TrigMonGroup ( this, m_parentName, expert );
  m_histogramCategory["SHIFT"]    = new TrigMonGroup ( this, m_parentName, shift );
  m_histogramCategory["DEBUG"]    = new TrigMonGroup ( this, m_parentName, debug );
  m_histogramCategory["RUNSTAT"]  = new TrigMonGroup ( this, m_parentName, runstat );
  m_histogramCategory["EXPRESS"]  = new TrigMonGroup ( this, m_parentName, express );

  for (const string& item : m_histograms) {
    HistogramDef def = parseJobOptHistogram(item);
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

template <class M, class P>
void TrigGenericMonitoringTool<M,P>::setProxy(const std::string& /*name*/, 
                                                IMonitoredAlgo::IGetter* g)
{
  for (auto f : m_fillers) {
    ATH_MSG_DEBUG("Setting new proxy for " << (g ? g->name() : "UNKNOWN"));
    f->updateGetter(g);
  }
}

template <class M, class P>
void TrigGenericMonitoringTool<M,P>::setOpts(TH1* histo, const std::string& opt) {
  // try to apply an option
  if ( opt.find("kCanRebin") != std::string::npos ) {
     histo->SetCanExtend(TH1::kAllAxes);
  }else {
     histo->SetCanExtend(TH1::kNoAxis);  
  }
  // try to apply option to make Sumw2 in histogram
  if ( opt.find("Sumw2") != std::string::npos ) {
    histo->Sumw2();
  }
}

template <class M, class P>
void TrigGenericMonitoringTool<M,P>::setLabels(TH1* histo, const std::vector<std::string>& labels) {
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

/**
 * Create and register histogram
 * 
 * If histogram already exists under that name, re-use it
 * H      : actual type of histogram to be created
 * HBASE  : histogram base class (TH[1,2,3])
 * Args   : remaining arguments to TH constructor (except name, title)
 */
template<class M, class P> template<class H, class HBASE, typename... Types> 
HBASE* TrigGenericMonitoringTool<M,P>::create( const HistogramDef& def, Types&&... hargs )
{
  // Check if histogram exists already
  HBASE* histo = m_histogramCategory[def.path]->template getHist<HBASE>(def.alias);
  if (histo) {
    ATH_MSG_DEBUG("Histogram " << def.alias << " already exists. Re-using it.");
    return histo;
  }

  // Create the histogram and register it
  H* h = new H(def.alias.c_str(), def.title.c_str(), std::forward<Types>(hargs)...);

  StatusCode sc;
  // Convert to LBN histogram if needed
  if ( def.opt.find("kLBN") != std::string::npos ) {
    auto lbh = new TrigLBNHist<H>(*h);
    delete h; 
    h = lbh;
    sc = m_histogramCategory[def.path]->regHist(static_cast<ITrigLBNHist*>(lbh));
  }
  else
    sc = m_histogramCategory[def.path]->regHist(static_cast<TH1*>(h));

  if (sc.isFailure()) {
    delete h;
    h = 0;
  }
  
  return h;
}


template<class M, class P> template<class H>
TH1* TrigGenericMonitoringTool<M,P>::create1D( TH1*& histo, const HistogramDef& def ) 
{
  histo = create<H,TH1>(def, def.xbins, def.xmin, def.xmax);
  return histo;
}

template<class M, class P> template<class H>
TH1* TrigGenericMonitoringTool<M,P>::createProfile( TProfile*& histo, const HistogramDef& def ) {
  TH1* h = create<H,TH1>( def, def.xbins, def.xmin, def.xmax, def.ymin, def.ymax );
  histo = dynamic_cast<TProfile*>(h);
  return histo;
}

template<class M, class P> template<class H>
TH1* TrigGenericMonitoringTool<M,P>::create2D( TH2*& histo, const HistogramDef& def ) {
  histo = create<H,TH2>( def, def.xbins, def.xmin, def.xmax, def.ybins, def.ymin, def.ymax  );
  return histo;
}

template<class M, class P> template<class H>
TH1* TrigGenericMonitoringTool<M,P>::create2DProfile( TProfile2D*& histo, const HistogramDef& def )
{
  TH1* h = create<H,TH2>(def, def.xbins, def.xmin, def.xmax,
                         def.ybins, def.ymin, def.ymax, def.zmin, def.zmax);
  histo = dynamic_cast<TProfile2D*>(h);
  return histo;
}

template <class M, class P>
StatusCode TrigGenericMonitoringTool<M,P>::createFiller(const HistogramDef& def) {
  TH1* histo(0);
  TH1* histo1D(0);
  TProfile* histoProfile(0);
  TH2* histo2D(0);
  TProfile2D* histo2DProfile(0);

  // create 1D
  if (def.type == "TH1F") 
    histo = create1D<TH1F>(histo1D, def);

  else if (def.type == "TH1D") 
    histo = create1D<TH1D>(histo1D, def);
  
  else if (def.type == "TH1I") 
    histo = create1D<TH1I>(histo1D, def);

  else if (def.type == "TProfile") {
    histo = createProfile<TProfile>(histoProfile, def);
  }

  //create 2D
  else if (def.type == "TH2F")
    histo = create2D<TH2F>(histo2D, def );

  else if (def.type == "TH2D")
    histo = create2D<TH2D>(histo2D, def );

  else if (def.type == "TH2I") 
    histo = create2D<TH2I>(histo2D, def);
  
  else if (def.type == "TProfile2D"){
    histo = create2DProfile<TProfile2D>(histo2DProfile, def);
  }
  
  if (histo == 0 ) {
    ATH_MSG_WARNING("Can not create yet histogram of type: " << def.type);
    ATH_MSG_WARNING("Try one of: TH1F,TH1D,TH1I,TH2F,TH2D,TH2I, TProfile, TProfile2D");
    return StatusCode::FAILURE;
  }

  setLabels(histo, def.labels);
  setOpts(histo, def.opt);

  // create filler for 1D
  if ( histo1D && !histoProfile){

    std::map<std::string, IMonitoredAlgo::IGetter*>::const_iterator varIt;
    if (m_algo) varIt = m_algo->variables().find(def.name[0]);
    
    if ( (m_algo && varIt != m_algo->variables().end()) || m_algo==nullptr ) {
    
      IMonitoredAlgo::IGetter* g = m_algo ? varIt->second : new IMonitoredAlgo::IGetter(def.name[0]);  
      if ( def.opt.find("kCumulative") != std::string::npos ) {
        ATH_MSG_DEBUG("Variable: " << def.name[0] << " from parent algorithm: "
                      << m_parentName << " will be histogrammed in Cummulative histogram");
        HistogramFiller* f = new CumulativeHistogramFiller1D(histo1D, g);
        m_fillers.push_back(f);
      } 
      else if (def.opt.find("kVecUO") != std::string::npos) {
        ATH_MSG_DEBUG("Variable: " << def.name << " from parent algorithm: "
                      << m_parentName << " will be added to histogram");
        if (g && (unsigned(histo1D->GetNbinsX()+2) != g->size()))
          ATH_MSG_WARNING("Variable: " << def.name << " from parent algorithm: " << m_parentName 
                          << " has different dimension: " << g->size() 
                          << " than histogram: " << histo1D->GetNbinsX()
                          << " booked for it and kVecUO options is requested (variable has to accomodate Under/Overflows too)");

      	HistogramFiller* f = new VecHistogramFiller1DWithOverflows(histo1D, g);
        m_fillers.push_back(f);
        
      } 
      else if (def.opt.find("kVec") != std::string::npos) {
        ATH_MSG_DEBUG("Variable: " << def.name << " from parent algorithm: "
                      << m_parentName << " will be added to histogram");
        if (g && (unsigned(histo1D->GetNbinsX()) != varIt->second->size()))
          ATH_MSG_WARNING("Variable: " << def.name << " from parent algorithm: " << m_parentName 
                          << " has different dimension: " << g->size() 
                          << " than hisogram: " << histo1D->GetNbinsX()
                          << " booked for it and kVec options is requested");
       
        HistogramFiller* f = new VecHistogramFiller1D(histo1D, g);
        m_fillers.push_back(f);
 
      } 
      else {
        ATH_MSG_DEBUG("Variable: " << def.name << " from parent algorithm: " << m_parentName
                      <<  " will be histogrammed");
        HistogramFiller* f = new HistogramFiller1D(histo1D, g);
        m_fillers.push_back(f);
      }
    } else {
      ATH_MSG_WARNING("Variable: " << def.name << " not exported by parent algorithm: " << m_parentName  << " this are available:");
      for ( varIt = m_algo->variables().begin(); varIt != m_algo->variables().end(); ++varIt) {
        msg() << MSG::WARNING << varIt->first << " ";	
      }
      msg() << MSG::WARNING << endmsg;
      
      return StatusCode::FAILURE;
    }

  } else if ( histo2DProfile ){


    std::map<std::string, IMonitoredAlgo::IGetter*>::const_iterator var1It, var2It, var3It;
    if (m_algo) {
      var1It = m_algo->variables().find(def.name[0]);
      var2It = m_algo->variables().find(def.name[1]);
      var3It = m_algo->variables().find(def.name[2]);
    }

    IMonitoredAlgo::IGetter* g1 = m_algo ? var1It->second : new IMonitoredAlgo::IGetter(def.name[0]);
    IMonitoredAlgo::IGetter* g2 = m_algo ? var2It->second : new IMonitoredAlgo::IGetter(def.name[1]);
    IMonitoredAlgo::IGetter* g3 = m_algo ? var3It->second : new IMonitoredAlgo::IGetter(def.name[2]);

    if ( (m_algo && var1It != m_algo->variables().end() && 
          var2It != m_algo->variables().end() && var3It != m_algo->variables().end()) || m_algo==nullptr ) {
      ATH_MSG_DEBUG("Variables: " << def.name[0] << "," << def.name[1] << "," << def.name[2]
                    << " from parent algorithm: " << m_parentName
                    << " will be histogrammed in 2D Profile histogram");
      m_fillers.push_back(new HistogramFiller2DProfile(histo2DProfile, g1, g2, g3));
    }
    else{
      ATH_MSG_WARNING("Variables: " << def.name[0] << "," << def.name[1] << "," << def.name[2] 
                      << " not exported by parent algorithm: " << m_parentName);
      for ( var1It = m_algo->variables().begin(); var1It != m_algo->variables().end(); ++var1It) {
        msg() << MSG::WARNING << var1It->first << " ";	
      }
      msg() << MSG::WARNING << endmsg;
      return StatusCode::FAILURE;
    }

  } else if ( histo2D || histoProfile ){
    std::map<std::string, IMonitoredAlgo::IGetter*>::const_iterator var1It, var2It;
    if (m_algo) {
      var1It = m_algo->variables().find(def.name[0]);
      var2It = m_algo->variables().find(def.name[1]);
    }
    
    IMonitoredAlgo::IGetter* g1 = m_algo ? var1It->second : new IMonitoredAlgo::IGetter(def.name[0]);
    IMonitoredAlgo::IGetter* g2 = m_algo ? var2It->second : new IMonitoredAlgo::IGetter(def.name[1]);

    if ( (m_algo && var1It != m_algo->variables().end() && 
          var2It != m_algo->variables().end()) || m_algo==nullptr ) {
      ATH_MSG_DEBUG("Variables: " << def.name[0] << "," << def.name[1]
                    << " from parent algorithm: " << m_parentName
                    << " will be histogrammed in 2D histogram");
      if (histo2D)
        m_fillers.push_back(new HistogramFiller2D(histo2D, g1, g2));
      else {       
        m_fillers.push_back(new HistogramFillerProfile(histoProfile, g1, g2));
      }
    }
    else {
      ATH_MSG_WARNING("Variables: " << def.name[0] << "," << def.name[1] 
                      << " not exported by parent algorithm: " << m_parentName);
      for ( var1It = m_algo->variables().begin(); var1It != m_algo->variables().end(); ++var1It) {
        msg() << MSG::WARNING << var1It->first << " ";	
      }
      msg() << MSG::WARNING << endmsg;
      return StatusCode::FAILURE;
    }
  }
  
  return StatusCode::SUCCESS;
}

template <class M, class P>
StatusCode TrigGenericMonitoringTool<M,P>::fillHists() {
  unsigned fills(0);
  for( HistogramFiller* hf : m_fillers ) {
    fills += hf->fill();
  }

  // Can be useful for debugging, in case one suspects a monitored container with ever increasing size
  ATH_MSG_DEBUG(fills << " histogram fills done");
  
  return StatusCode::SUCCESS;
}


template <class M, class P>
StatusCode TrigGenericMonitoringTool<M,P>::finalHists() {

  for ( const auto& i : m_histogramCategory ) {
    delete i.second;
  }
  m_histogramCategory.clear();
  
  for ( HistogramFiller* hf : m_fillers ) {
    delete hf;
  }
  m_fillers.clear();

  return StatusCode::SUCCESS;
}


template <class M, class P>
const typename TrigGenericMonitoringTool<M,P>::HistogramDef TrigGenericMonitoringTool<M,P>::parseJobOptHistogram(const std::string& histDef) {
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

template <class M, class P>
unsigned TrigGenericMonitoringTool<M,P>::HistogramFiller1D::fill() {
  unsigned i(0);
  std::lock_guard<M> lock(this->m_mutex);
  for (; i < this->m_variable->size() ; ++i )
    m_histogram->Fill(this->m_variable->get(i));
  return i;
} 

template <class M, class P>
unsigned TrigGenericMonitoringTool<M,P>::CumulativeHistogramFiller1D::fill() {
  unsigned i(0);
  std::lock_guard<M> lock(this->m_mutex);
  for (; i < this->m_variable->size() ; ++i ) {
    unsigned bin = this->m_histogram->FindBin( this->m_variable->get(i) );
    for ( unsigned j = bin; j > 0; --j) 
      this->m_histogram->AddBinContent(j);
  }
  return i;  
}

template <class M, class P>
unsigned TrigGenericMonitoringTool<M,P>::VecHistogramFiller1D::fill() {
  unsigned i(0);  
  std::lock_guard<M> lock(this->m_mutex);
  for (; i < this->m_variable->size() ; ++i ) {
    double v = this->m_variable->get(i);
    this->m_histogram->AddBinContent(i+1, v);
    this->m_histogram->SetEntries(this->m_histogram->GetEntries() + v );
  }
  return i;
}


template <class M, class P>
unsigned TrigGenericMonitoringTool<M,P>::VecHistogramFiller1DWithOverflows::fill() {
  unsigned i(0);
  std::lock_guard<M> lock(this->m_mutex);
  for (; i < this->m_variable->size() ; ++i ) {
    double v = this->m_variable->get(i);
    this->m_histogram->AddBinContent(i, v); 
    this->m_histogram->SetEntries(this->m_histogram->GetEntries() + v );
  }
  return i;
}

template <class M, class P>
TrigGenericMonitoringTool<M,P>::HistogramFillerProfile::HistogramFillerProfile(TProfile* hist, 
                                                                               IMonitoredAlgo::IGetter* var1, 
                                                                               IMonitoredAlgo::IGetter* var2) 
  : m_histogram(hist), m_variable1(var1), m_variable2(var2) {
}  

template <class M, class P>
unsigned TrigGenericMonitoringTool<M,P>::HistogramFillerProfile::fill() {
  if ( !m_histogram )
    return 0;

  unsigned i(0);  
  std::lock_guard<M> lock(this->m_mutex);
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

template <class M, class P>
TrigGenericMonitoringTool<M,P>::HistogramFiller2DProfile::HistogramFiller2DProfile(TProfile2D* hist, 
                                                                                     IMonitoredAlgo::IGetter* var1, 
                                                                                     IMonitoredAlgo::IGetter* var2,
                                                                                     IMonitoredAlgo::IGetter* var3) 
  : m_histogram(hist), m_variable1(var1), m_variable2(var2), m_variable3(var3) {
}  

template <class M, class P>
unsigned TrigGenericMonitoringTool<M,P>::HistogramFiller2DProfile::fill() {
  if ( !m_histogram )
    return 0;

  unsigned i(0);
  /*HERE NEED TO INCLUDE CASE IN WHICH SOME VARIABLES ARE SCALAR AND SOME VARIABLES ARE VECTORS
  unsigned i(0);
  if (m_variable1->size() != m_variable2->size() || m_variable1->size() != m_variable3->size() || m_variable2->size() != m_variable3->size() ) {

  }*/

  //For now lets just consider the case in which all variables are of the same length
  std::lock_guard<M> lock(this->m_mutex);
  for ( i = 0; i < m_variable1->size() ; ++i )
    m_histogram->Fill(m_variable1->get(i), m_variable2->get(i), m_variable3->get(i));
  
  return i;
}



template <class M, class P>
TrigGenericMonitoringTool<M,P>::HistogramFiller2D::HistogramFiller2D(TH2* hist, IMonitoredAlgo::IGetter* var1, 
								IMonitoredAlgo::IGetter* var2) 
  : m_histogram(hist), m_variable1(var1), m_variable2(var2) {
}  

template <class M, class P>
unsigned TrigGenericMonitoringTool<M,P>::HistogramFiller2D::fill() {
  if ( !m_histogram )
    return 0;

  unsigned i(0);
  std::lock_guard<M> lock(this->m_mutex);
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

// Explicitly instantiate the possible templates and define aliases
template class TrigGenericMonitoringTool<NoMutex, IMonitoredAlgo::IGetter*>;
template class TrigGenericMonitoringTool<std::mutex, ContextGetter<IMonitoredAlgo::IGetter>>;

