/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "AthenaMonitoring/HistogramDef.h"

using namespace std;
using namespace Monitored;

const HistogramDef HistogramDef::parse(const std::string& jobOpts) {
  /* Parse histogram defintion
     Example:
     1D: "EXPERT, TH1I, Name;Alias, Title, nBins, xmin, xmax, BinLabel1:BinLabel2:BinLabel3, kCumulative"
  */

  // convert histogram definition string to an array of strings
  list<string> histProperty;
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer_t;
  boost::char_separator<char> sep(","); 
  tokenizer_t tokens(jobOpts, sep);
  
  for (tokenizer_t::iterator itr = tokens.begin(); itr != tokens.end(); ++itr) {
    string word = *itr;
    boost::trim(word);
    histProperty.push_back(word); 
  } 
  // return value
  HistogramDef histPar;
  list<string>::iterator itr = histProperty.begin();
  
  // stream
  const string word = (*itr);
  histPar.path = word;
  itr = histProperty.erase(itr);
  
  const char* warning = " NOT booked: ";
  histPar.ok   = false;
  histPar.ycut = false;
  histPar.zcut = false;
  
  if (histProperty.size() < 5) return histPar;   
  // extract type
  histPar.type = (*itr);
  itr = histProperty.erase(itr);

  // extract name
  histPar.name.push_back(*itr);
  itr = histProperty.erase(itr);


  if (histPar.type.find("TH2") == 0 || histPar.type == "TProfile" || histPar.type == "TEfficiency") {
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
  // consume title
  histPar.title = (*itr);
  itr = histProperty.erase(itr);
  
  if (histProperty.size() < 2) {
    throw HistogramDefParseException(histPar.alias + warning + "NOT enough parameters for defining 1-D histogram");
  }
  // consume N x bins
  try {
    histPar.xbins = boost::lexical_cast<int>(*itr);
    itr = histProperty.erase(itr);
  }
  catch (boost::bad_lexical_cast&) {
    stringstream ss;
    copy(histProperty.begin(), histProperty.end(), ostream_iterator<string>(ss, ","));
    throw HistogramDefParseException(histPar.alias + warning + "int expected for xbins while got" + ss.str());
  }
  
  if (histProperty.size() < 2) {
    throw HistogramDefParseException(histPar.name[0] + warning + "xmin and xmax expected");
  }
  
  try {
    histPar.xmin = boost::lexical_cast<double>(*itr);
    itr = histProperty.erase(itr);
  }
  catch (boost::bad_lexical_cast&) {
    throw HistogramDefParseException(histPar.name[0] + warning + "double expected for xmin");
  }
  
  try {
    histPar.xmax = boost::lexical_cast<double>(*itr);
    itr = histProperty.erase(itr);
  }
  catch (boost::bad_lexical_cast&) {
    throw HistogramDefParseException(histPar.name[0] + warning + "double expected for xmax");
  }

  if (histPar.type.find("TH2") == 0) {
    if (histProperty.size() < 2) {
      throw HistogramDefParseException(histPar.name[0] + warning + "y-axis definition expected for TH2");
    }
    
    try {
      histPar.ybins = boost::lexical_cast<int>(*itr);
      itr = histProperty.erase(itr);
    }
    catch (boost::bad_lexical_cast&) {
      throw HistogramDefParseException(histPar.name[0] + warning + "int expected for ybins");
    }
    
    if (histProperty.size() < 2) {
      throw HistogramDefParseException(histPar.name[0] + warning + "ymin and ymax expected");
    }
    
    try {
      histPar.ymin = boost::lexical_cast<double>(*itr);
      itr = histProperty.erase(itr);
    }
    catch (boost::bad_lexical_cast&) {
      throw HistogramDefParseException(histPar.name[0] + warning + "double expected for ymin");
    }
    
    try {
      histPar.ymax = boost::lexical_cast<double>(*itr);
      itr = histProperty.erase(itr);
    }
    catch (boost::bad_lexical_cast&) {
      throw HistogramDefParseException(histPar.name[0] + warning + "double expected for ymax");
    }
  } //-end of TH2

  //TProfile2D
  else if (histPar.type == "TProfile2D"){

    if (histProperty.size() < 2) {
      throw HistogramDefParseException(histPar.name[0] + warning + "y-axis definition expected for TProfile2D");
    }
    
    try {
      histPar.ybins = boost::lexical_cast<int>(*itr);
      itr = histProperty.erase(itr);
    }
    catch (boost::bad_lexical_cast&) {
      throw HistogramDefParseException(histPar.name[0] + warning + "int expected for ybins");
    }
    
    if (histProperty.size() < 2) {
      throw HistogramDefParseException(histPar.name[0] + warning + "ymin and ymax expected");
    }
    
    try {
      histPar.ymin = boost::lexical_cast<double>(*itr);
      itr = histProperty.erase(itr);
    }
    catch (boost::bad_lexical_cast&) {
      throw HistogramDefParseException(histPar.name[0] + warning + "double expected for ymin");
    }
    
    try {
      histPar.ymax = boost::lexical_cast<double>(*itr);
      itr = histProperty.erase(itr);
    }
    catch (boost::bad_lexical_cast&) {
      throw HistogramDefParseException(histPar.name[0] + warning + "double expected for ymax");
    }
    //For limited z range
    if(histProperty.size() >= 2){
      try {
        histPar.zmin = boost::lexical_cast<double>(*itr);
        itr = histProperty.erase(itr);
      }
      catch (boost::bad_lexical_cast&) {
        throw HistogramDefParseException(histPar.name[0] + warning + "double expected for zmin of TProfile2D");
      }
    
      try {
        histPar.zmax = boost::lexical_cast<double>(*itr);
        itr = histProperty.erase(itr);
      }
      catch (boost::bad_lexical_cast&) {
        throw HistogramDefParseException(histPar.name[0] + warning + "double expected for zmax of TProfile2D");
      }

      histPar.zcut = true;
    }
  }

  // check if there are bin labels (e.g. str1:str2:str3:str4)
  if (itr != histProperty.end() && itr->find(":") != std::string::npos ) {
    // split it 
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
