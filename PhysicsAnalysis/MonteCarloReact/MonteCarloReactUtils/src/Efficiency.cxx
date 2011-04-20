/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************
Created by: Marc Hohfeld on January 10 2008
Modified  :
Purpose   : Apply efficiency corrections to MC
****************************************************/
#include "MonteCarloReactUtils/Efficiency.h"
#include "MonteCarloReactUtils/EffExceptions.h"

#include <vector>
#include <algorithm>

using namespace std;
using namespace MonteCarloReact;

Efficiency::Efficiency() { m_isValid = false; }

Efficiency::Efficiency(istream &fstr, 
                       const EffInfo* request){ m_makeEfficiency( fstr, request);}


Efficiency::~Efficiency(){;}


bool Efficiency::isRequestInFile(istream& fp, const EffInfo* request) {
  
  bool endOfObject = false ; // true after filling one EffInfo object
  while(fp && fp.good()) {
    vector<string> tokens;
    std::string str;
    std::string key;
    getline(fp,str);
    // Tokenize is used to break the input line into a series of "words", the first of which is the "keyword". The fourth parameter to this call
    // is the set of delimeters for defining word boundaries
    bool goodTokens = m_tokenize(str, key, tokens, "\t\r\n: ");
    if(!goodTokens)  continue ;
    if( m_parseInputLine( key, tokens)) {
      endOfObject = true ;
      continue ;
    } 

    if (endOfObject) {
      if(*request == m_spec) return true ;
      clear() ;
      endOfObject = false ;
    }

    m_spec.parseInputLine( key, tokens);
  }
  if(*request == m_spec) return true  ;
  return false ;
}


void Efficiency::setInfo(EffInfo & spec ) { 
  m_spec.setDataType(spec.getDataType());
  m_spec.setEffName(spec.getEffName());
  m_spec.setEffType(spec.getEffType());
  m_spec.setEffVersion(spec.getEffVersion());
  m_spec.setEffVarNames(spec.getEffVarNames());
  m_spec.setObjType(spec.getObjType());
  m_spec.setObjQuality(spec.getObjQuality());
  m_spec.setObjVersion(spec.getObjVersion());
  m_spec.setObjAlgorithm(spec.getObjAlgorithm());
}

void Efficiency::m_doStream( ostream & os) const {
  // first dump efficiency info
  os << m_spec;
  // now dump the efficiency values themselves
  m_stream( os );
}

bool Efficiency::m_makeEfficiency(istream& fstr, const EffInfo* request){
  if(!m_effParse(fstr, request)) return false;  
  else return true;
}

bool Efficiency::m_effParse(istream& fp, const EffInfo* request) {
  // read in one Efficiency object from the input stream
  bool endOfObject = false ; // true after filling one EffInfo object
  while(fp && fp.good()) { 
    vector<string> tokens;
    std::string str;
    std::string key;
    std::streampos savePoint = fp.tellg();
    getline(fp,str);
    // Tokenize is used to break the input line into a series of "words", the first of which is the "keyword". The fourth parameter to this call
    // is the set of delimeters for defining word boundaries
    bool goodTokens = m_tokenize(str, key, tokens, "\t\r\n: ");
    if(!goodTokens)  continue ;
    // ParseInputLine is implemented by the derived class. 
    // It returns true if the derived class found a key which it recongnised
    // thus endOfObject is set to true the first time a key is found belonging 
    // to the derived class.
    if( m_parseInputLine( key, tokens)) { 
      endOfObject = true ;
      continue ;
    } 
    // this section can only be reached if the key was not recongnised by the derived class
    if (endOfObject) {
      // so we can only get into here when we find the first line with a key not 
      // recognised by the derived class after a block of lines (1 or more) which *were* recognised by the 
      // derived class.
      if(request && request->contains(m_spec)) { // does the specification of the efficiency match what we requested?
	m_isValid = m_doValidateInput(); // is it valid?
	if( m_isValid ) {
	  // now we need to put the current line back into the buffer in case the controller of the stream (most likely EffTool) wants to make 
	  // a new Efficiency object to continue parsing the input stream to look for further instances of matching efficiencies!
	  fp.seekg( savePoint );
	} 
	return m_isValid ; //
      }
      // if it matched the request but was invalid then we return false... if it doesn't match then we keep processing here.
      // by wiping the contents and continuing with the current line
      clear() ;
      endOfObject = false ;
    }
    // current line was not processed by the derived class so its most likely metadata rather than the efficiency data itself and thus belongs in the spec.
    m_spec.parseInputLine( key, tokens);
  }
  // if we got to the end of the stream without finding a file. throw an exception here.
  if(request && !request->contains(m_spec)) 
    throw EffRequestNotFoundInFile() ;
  m_isValid = m_doValidateInput();
  return m_isValid;
}



bool Efficiency::m_tokenize(const string& str, std::string & key, 
                            vector<string>& tokens, const string& delimiters) {
  int nfound = 0;
  if(str.empty()) return false;
  // Skip delimiters at beginning
  string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  // Find first non-delimiter
  string::size_type pos = str.find_first_of(delimiters, lastPos);
  bool first = true;
  while (string::npos != pos || string::npos != lastPos) {
    ++nfound;
    std::string tmp = str.substr(lastPos, pos - lastPos);
    if( first ) {
      key = tmp;
      first = false;
    } else {
      // Found a token, add it to the vector.
      tokens.push_back(tmp);
    }
    // Skip delimiters.
    lastPos = str.find_first_not_of(delimiters, pos);
    // Find next "non-delimiter"
    pos = str.find_first_of(delimiters, lastPos);
  }
  if(nfound == 0) return false; else return true;
}



bool Efficiency::m_doValidateInput() const {
  bool valid = true;
  // first make sure all the things needed to define an arbitrary efficiency have been defined
  
    // now allow derived class to check for what it requires
  if( valid ) valid = valid && validateInput();
  return valid;
}

