/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MonteCarloReactUtils/EffInfo.h"
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>


using namespace MonteCarloReact;
using namespace std;


TrigVersion::TrigVersion(const std::string& version) {
  
  istringstream st ;
  int pos = int(string::npos) ;
  if ((pos=version.find("v")) != int(string::npos))
    st.str(version.substr(pos+1)) ;
  else st.str(version) ;
  st >> m_min ;
  if (version.find(".") != string::npos)
    m_max = m_min ;
  else m_max = m_min + 0.99 ;
}

TrigVersion::TrigVersion(const std::string& version_min, 
			 const std::string& version_max) {
  typedef string::size_type size_t;
  size_t pos = string::npos ;
  
  istringstream st ;
  if (version_min.size() == 0 || 
      version_min.find_first_of("0123456789") == string::npos ) {
    m_min = 0 ;
  } else {
    if ((pos=version_min.find("v")) != string::npos)
      st.str(version_min.substr(pos+1)) ;
    else st.str(version_min) ;
    st >> m_min ;
  }

  st.clear() ;
  if (version_max.size() == 0 || 
      version_max.find_first_of("0123456789") == string::npos ) {
    m_max = -1 ;
  } else {
    if ((pos=version_max.find("v")) != string::npos)
      st.str(version_max.substr(pos+1)) ;
    else st.str(version_max) ;
    st >> m_max ;
  }
  if (m_min == m_max && version_max.find(".") == string::npos)
    m_max += 0.99 ;
}

/******************************************************************/
EffInfo::EffInfo(std::string datatype, std::string efftype, std::string effname,
                 std::vector<std::string> effvarnames, int effversion, int objectversion,
                 std::string objecttype, std::string objectquality, std::string objectalgorithm) {
  clear();
  EffInfo::setDataType(datatype);
  EffInfo::setEffType(efftype);
  EffInfo::setEffName(effname);
  EffInfo::setEffVarNames(effvarnames);
  EffInfo::setEffVersion(effversion);
  EffInfo::setObjVersion(objectversion);
  EffInfo::setObjType(objecttype);
  EffInfo::setObjQuality(objectquality);
  EffInfo::setObjAlgorithm(objectalgorithm);

}




EffInfo::~EffInfo() {;}
  
void EffInfo::setDataType( const std::string & type) { setEntry("DataType", type);}
  
std::string EffInfo::getDataType() const {
  std::string type="UNDEFINED";
  getEntry("DataType",type);
  return type;
}


void EffInfo::setEffType( const std::string & type) { setEntry("EffType", type);}
  
std::string EffInfo::getEffType() const {
  std::string type="UNDEFINED";
  getEntry("EffType",type);
  return type;
}
  
void EffInfo::setEffName( const std::string & name) { setEntry("EffName",name);}

std::string EffInfo::getEffName() const {
  std::string name = "UNDEFINED";
  getEntry("EffName",name);
  return name;
}
  
void EffInfo::setEffVarNames(const std::vector<std::string>& names ) { setEntry("EffVarNames",names); }
  
std::vector<std::string> EffInfo::getEffVarNames() const {
  std::vector< std::string > names;
  getEntry("EffVarNames",names);
  return names;
}
  
int EffInfo::getEffNVars() const { 
  int n = 0;
  bool retval = getNParameters("EffVarNames", n);
  if( retval )
    return n;
  else
    return -1;
}

void EffInfo::setEffVersion( int version) {
  setEntry("EffVersion",version);
}
  
int EffInfo::getEffVersion() const {
  int retval = -1;
  getEntry("EffVersion",retval);
  return retval;
}
  
void EffInfo::setEffMethod( const std::string & input) { setEntry("EffMethod",input);}

std::string EffInfo::getEffMethod() const {
  std::string output="UNDEFINED";
  getEntry("EffMethod",output);
  return output;
}

void EffInfo::setEffTagQuality( const std::string & input) { setEntry("EffTagQuality",input);}

std::string EffInfo::getEffTagQuality() const {
  std::string output="UNDEFINED";
  getEntry("EffTagQuality",output);
  return output;
}

//-------- Epoch defenition -------------------------------

void EffInfo::setRunRangeLow( long lo) {
  setEntry("RunRangeLow",lo);
} 

long EffInfo::getRunRangeLow() const {
  long retval = -1;
  getEntry("RunRangeLow",retval);
  return retval;
}
  
void EffInfo::setRunRangeHigh( long hi) {
  setEntry("RunRangeHigh",hi);
}

long EffInfo::getRunRangeHigh() const {
  long retval = -1;
  getEntry("RunRangeHigh", retval);
  return retval;
}
  
void EffInfo::setDQDefinition( const std::string & input) { setEntry("DQDefinition",input);}

std::string EffInfo::getDQDefinition() const {
  std::string output="UNDEFINED";
  getEntry("DQDefinition",output);
  return output;
}

void EffInfo::setDataSetName( const std::string & input) { setEntry("DataSetName",input);}

std::string EffInfo::getDataSetName() const {
  std::string output="UNDEFINED";
  getEntry("DataSetName",output);
  return output;
}

// trigger version can be formatted as "v14" or "14" or "v14.99" or "14.99"
// only one version allowed. If you need to specify minimal and maximal versions
// use TriggerVersionLow and TriggerVersionHigh
void EffInfo::setTriggerVersion( const std::string & input) { 
  if (getTriggerVersionHigh() != "UNDEFINED" || 
      getTriggerVersionLow() != "UNDEFINED")
    throw std::runtime_error("TriggerVersion keyword can not be used if TriggerVersionHigh or TriggerVersionLow is used before");
  setEntry("TriggerVersion",input);
}

std::string EffInfo::getTriggerVersion() const {
  std::string output="UNDEFINED";
  getEntry("TriggerVersion",output);
  return output;
}

// trigger version can be formatted as "v14" or "14" or "v14.99" or "14.99"
void EffInfo::setTriggerVersionHigh( const std::string & input) { 
  if (getTriggerVersion() != "UNDEFINED")
    throw std::runtime_error("TriggerVersionHigh keyword can not be used if TriggerVersion is used before");
  setEntry("TriggerVersionHigh",input);
}

std::string EffInfo::getTriggerVersionHigh() const {
  std::string output="UNDEFINED";
  getEntry("TriggerVersionHigh",output);
  return output;
}


// trigger version can be formatted as "v14" or "14" or "v14.99" or "14.99"
void EffInfo::setTriggerVersionLow( const std::string & input) { 
  if (getTriggerVersion() != "UNDEFINED")
    throw std::runtime_error("TriggerVersionLow keyword can not be used if TriggerVersion is used before");
  setEntry("TriggerVersionLow",input);
}

std::string EffInfo::getTriggerVersionLow() const {
  std::string output="UNDEFINED";
  getEntry("TriggerVersionLow",output);
  return output;
}

// ------------ Object meta data ------------------------------------

void EffInfo::setObjType( const std::string & type) { setEntry("ObjType",type);}
  
std::string EffInfo::getObjType() const {
  std::string type="UNDEFINED";
  getEntry("ObjType",type);
  return type;
}
  
void EffInfo::setObjQuality( const std::string & quality) { setEntry("ObjQuality",quality);}

std::string EffInfo::getObjQuality() const {
  std::string quality = "UNDEFINED";
  getEntry("ObjQuality",quality);
  return quality;
}
  
  
void EffInfo::setObjVersion( int version) {
  setEntry("ObjVersion",version);
}
  
int EffInfo::getObjVersion() const {
  int retval = -1;
  getEntry("ObjVersion",retval);
  return retval;
}
  
void EffInfo::setObjRelativeTo( const std::string & input) { setEntry("ObjRelativeTo",input);}

std::string EffInfo::getObjRelativeTo() const {
  std::string output="UNDEFINED";
  getEntry("ObjRelativeTo",output);
  return output;
}

void EffInfo::setObjAlgorithm( const std::string & algo ) {
  setEntry("ObjAlgorithm",algo);
}
  
std::string EffInfo::getObjAlgorithm() const {
  std::string algo = "";
  getEntry("ObjAlgorithm",algo);
  return algo;
}

// ------------ Additional meta data ---------------------------------

std::string EffInfo::getATLASNoteID() const {
  std::string retval = "";
  getEntry("ATLASNoteID",retval);
  return retval;
}
    
void EffInfo::setATLASNoteID(const std::string & note )  {
  setEntry("ATLASNoteID",note);
}
  
/// several comments line could be added
void EffInfo::setComments( const std::string & input) { setEntry("Comments",input);}

std::vector <std::string>  EffInfo::getComments() const {
  std::vector <std::string> output;
  getEntry("Comments",output);
  return output;
}

bool EffInfo::getNParameters( const std::string & id, int & i) const {
  std::map< std::string, std::vector<std::string> >::const_iterator mapIter = m_map.find(id);
  if( mapIter == m_map.end() )
    return false;
  else {
    const std::vector< std::string > & vec = mapIter->second;
    i = vec.size();
    return true;
  }
}

std::string EffInfo::makeFileName() const {
  std::string _filename;
  std::vector< std::string > axes = getEffVarNames();
  int dim = axes.size();
  _filename = getDataType() + "_" + getEffName() + "_" + getObjType() ;
  if (getObjQuality()!="UNDEFINED") _filename += "_" + getObjQuality() ;
  _filename += "_" + convertToString(dim) + "D";

  for( int i = 0; i < dim; ++i) _filename += "_" + axes[i];
  
  _filename += "_" + getEffType() + ".mcr" ;
  if(!_filename.empty())
    return _filename;
  else {
    _filename = "NOFILENAME";
    return _filename;  
  }
}


  


template<class T> void EffInfo::setEntry( const std::string & id, const T & value) {
  std::string entry = convertToString( value );
  if (entry.empty()) return ;
  std::vector< std::string> vec;
  vec.push_back( entry );
  m_map[id] = vec;
}
  
void EffInfo::setEntry( const std::string & id, const std::vector< std::string > & values) {    
  m_map[id] = values;
}
  
template< class T> bool EffInfo::addToEntry( const std::string & id, const T & value) {
  std::map< std::string, std::vector< std::string> >::iterator mapIter = m_map.find(id);
  if( mapIter == m_map.end() ) 
    return false;
  std::vector< std::string > & vec = mapIter->second;
  std::string val = convertToString( value );
  vec.push_back( val );
  return true;
}
  
template<class T> bool EffInfo::getEntry( const std::string & id, T & value, int i) const  {
  T tmp;
  std::map<std::string, std::vector<std::string> >::const_iterator mapIter = m_map.find(id);
  if( mapIter == m_map.end() ) 
    return false;
    
  const  std::vector< std::string > & vec = mapIter->second;
  if( i > (int) vec.size() )
    return false;
    
  const std::string & svalue = vec.at(i);
  std::istringstream ist( svalue );
  if( ist >> tmp ) {
    value = tmp;
    return true;
  } else 
    return false;
}

bool EffInfo::getEntry( const std::string & id, std::vector< std::string> & vec) const  {
  // now lets get the whole vector!
  std::map<std::string, std::vector<std::string> >::const_iterator mapIter = m_map.find(id);
  if( mapIter == m_map.end() ) 
    return false;
    
  vec = mapIter->second;
  return true;
}

template<class type> std::string EffInfo::convertToString(const type & val)  const {
  std::ostringstream streamOut;
  streamOut << val;
  return streamOut.str();
}


bool EffInfo::parseInputLine( const std::string & key, const std::vector< std::string> & line )
{
  
  int n = line.size();
  
  //bool retval = ParseInputLine( key, line );
  bool retval = true;
  // look for our tokens first
  if( key == "DataType" && n == 1) 
    setDataType( line.at(0));
  else if( key == "EffType" && n == 1) 
    setEffType( line.at(0));
  else if( key == "EffName" && n == 1) 
    setEffName( line.at(0) );
  else if( key == "EffVersion" && n==1 ){
    int ver = atoi( line.at(0).c_str());
    setEffVersion( ver );
  }
  else if( key == "ObjType" && n == 1) 
    setObjType( line.at(0) );
  else if( key == "ObjQuality" && n==1) 
    setObjQuality( line.at(0) ); 
  else if( key == "ObjAlgorithm" && n==1) 
    setObjAlgorithm( line.at(0) ); 
  else if( key == "ATLASNoteID" && n == 1) {
    setATLASNoteID( line.at(0) );
  }
  else if( key == "RunRange" && n == 3) {
    long low = atol( line.at(1).c_str());
    long hi = atol( line.at(2).c_str());
    if( low <= hi && low && hi) { // FIXME - double check this line
      setRunRangeLow( low );
      setRunRangeHigh( hi );
    }
  } else {
    retval = false;
  }
  // if none of these match then just put the whole thing into the map generally
  if( !retval) setEntry( key, line );
  return retval;
}

bool EffInfo::contains (const EffInfo& info) const {
  bool trigger_processed = false ;

  for ( map<string,vector<string> >::const_iterator it = m_map.begin() ;
	it != m_map.end(); it++) {
    if (it->first == "Comments" || it->first == "ATLASNoteID") continue ;
    map<string,vector<string> >::const_iterator jt ;

    // special treatment for the trigger versions
    // trigger version from this object must be equal or more specific than
    // trigger version in the info object
    if (it->first.find("TriggerVersion") != string::npos) {
      if(trigger_processed) continue ;
      trigger_processed = true ;
      TrigVersion ver ;
      
      if (it->first == "TriggerVersion") {
	ver = TrigVersion(it->second.front()) ;
      } else {
	string low = "", high = "" ;
	if (it->first == "TriggerVersionLow") {
	  low = it->second.front() ;
	  if ((jt = m_map.find("TriggerVersionHigh")) != m_map.end()) 
	    high = jt->second.front() ;
	} else {
	  high = it->second.front() ;
	  if ((jt=m_map.find("TriggerVersionLow")) != m_map.end()) 
	    low = jt->second.front() ;
	}
	ver = TrigVersion(low,high) ;
      }

      if ((jt = info.m_map.find("TriggerVersion")) != info.m_map.end()) {
	if (ver.isContainedBy(TrigVersion(jt->second.front()))) continue ;
	return false ;
      }

      string low = "", high = "" ;
      jt = info.m_map.find("TriggerVersionLow") ;
      map<string,vector<string> >::const_iterator jt1 = 
        info.m_map.find("TriggerVersionHigh") ;

      if (jt == info.m_map.end() && jt1 == info.m_map.end()) 
	return false ;
	
      if (jt != info.m_map.end()) low = jt->second.front() ;
      if (jt1 != info.m_map.end()) high = jt1->second.front() ;

      if (ver.isContainedBy(TrigVersion(low, high))) continue ;
      return false ;      

    } // end of special treatment for trigger versions

    if ((jt = info.m_map.find(it->first)) == info.m_map.end()) return false ; 
    if (jt->second.size() < it->second.size()) return false ;
    for (vector<string>::const_iterator vt = it->second.begin();
	 vt != it->second.end(); vt++) 
      if (find (jt->second.begin(), jt->second.end(), *vt) == jt->second.end()) return false ;    
  }
  return true ;
}


