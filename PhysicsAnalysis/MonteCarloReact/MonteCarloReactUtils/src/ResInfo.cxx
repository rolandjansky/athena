/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MonteCarloReactUtils/ResInfo.h"
#include "MonteCarloReactUtils/EffInfo.h"
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>

using namespace MonteCarloReact;
using namespace std;

ResInfo::ResInfo(std::string datatype, std::string restype, std::string resname,
                 std::vector<std::string> resvarnames, std::vector<std::string> shiftparameters, 
                 int resversion, int objectversion,
                 std::string objecttype, std::string objectquality, std::string objectalgorithm) {
  ResInfo::setDataType(datatype);
  ResInfo::setResType(restype);
  ResInfo::setResName(resname);
  ResInfo::setResVarNames(resvarnames);
  ResInfo::setShiftParameters(shiftparameters);
  ResInfo::setResVersion(resversion);
  ResInfo::setObjVersion(objectversion);
  ResInfo::setObjType(objecttype);
  ResInfo::setObjQuality(objectquality);
  ResInfo::setObjAlgorithm(objectalgorithm);
  
}

ResInfo::~ResInfo() {;}

// ------------ Resolution meta data ---------------------------------
  
void ResInfo::setResType( const std::string & type) { setEntry("ResType", type);}
  
std::string ResInfo::getResType() const {
  std::string type="UNDEFINED";
  getEntry("ResType",type);
  return type;
}
  
void ResInfo::setResName( const std::string & name) { setEntry("ResName",name);}

std::string ResInfo::getResName() const {
  std::string name = "UNDEFINED";
  getEntry("ResName",name);
  return name;
}
  
void ResInfo::setResVarNames(const std::vector <std::string> & names ) { setEntry("ResVarNames",names); }
  
std::vector < std::string> ResInfo::getResVarNames() const {
  std::vector <std::string> names;
  getEntry("ResVarNames",names);
  return names;
}

void ResInfo::setShiftParameters(const std::vector <std::string> & names ) { setEntry("ShiftParameters",names); }
  
std::vector < std::string> ResInfo::getShiftParameters() const {
  std::vector <std::string> names;
  getEntry("ShiftParameters",names);
  return names;
}


int ResInfo::getResNVars() const { 
  int n = 0;
  bool retval = getNParameters("ResVarNames", n);
  if( retval )
    return n;
  else
    return -1;
}

int ResInfo::getResNParas() const { 
  int n = 0;
  bool retval = getNParameters("ShiftParameters", n);
  if( retval )
    return n;
  else
    return -1;
}
  
void ResInfo::setResVersion( int version) {
  setEntry("ResVersion",version);
}
  
int ResInfo::getResVersion() const {
  int retval = -1;
  getEntry("ResVersion",retval);
  return retval;
}
  
void ResInfo::setResMethod( const std::string & input) { setEntry("ResMethod",input);}

std::string ResInfo::getResMethod() const {
  std::string output="UNDEFINED";
  getEntry("ResMethod",output);
  return output;
}
  
void ResInfo::setResTagQuality( const std::string & input) { setEntry("ResTagQuality",input);}

std::string ResInfo::getResTagQuality() const {
  std::string output="UNDEFINED";
  getEntry("ResTagQuality",output);
  return output;
}

//-------- Epoch defenition -------------------------------

void ResInfo::setRunRangeLow( long lo) {
  setEntry("RunRangeLow",lo);
} 

long ResInfo::getRunRangeLow() const {
  long retval = -1;
  getEntry("RunRangeLow",retval);
  return retval;
}
  
void ResInfo::setRunRangeHigh( long hi) {
  setEntry("RunRangeHigh",hi);
}

long ResInfo::getRunRangeHigh() const {
  long retval = -1;
  getEntry("RunRangeHigh", retval);
  return retval;
}
  
void ResInfo::setDQDefinition( const std::string & input) { setEntry("DQDefinition",input);}

std::string ResInfo::getDQDefinition() const {
  std::string output="UNDEFINED";
  getEntry("DQDefinition",output);
  return output;
}

void ResInfo::setDataSetName( const std::string & input) { setEntry("DataSetName",input);}

std::string ResInfo::getDataSetName() const {
  std::string output="UNDEFINED";
  getEntry("DataSetName",output);
  return output;
}

// trigger version can be formatted as "v14" or "14" or "v14.99" or "14.99"
// only one version allowed. If you need to specify minimal and maximal versions
// use TriggerVersionLow and TriggerVersionHigh
void ResInfo::setTriggerVersion( const std::string & input) { 
  if (getTriggerVersionHigh() != "UNDEFINED" || 
      getTriggerVersionLow() != "UNDEFINED")
    throw std::runtime_error("TriggerVersion keyword can not be used if TriggerVersionHigh or TriggerVersionLow is used before");
  setEntry("TriggerVersion",input);
}

std::string ResInfo::getTriggerVersion() const {
  std::string output="UNDEFINED";
  getEntry("TriggerVersion",output);
  return output;
}

// trigger version can be formatted as "v14" or "14" or "v14.99" or "14.99"
void ResInfo::setTriggerVersionHigh( const std::string & input) { 
  if (getTriggerVersion() != "UNDEFINED")
    throw std::runtime_error("TriggerVersionHigh keyword can not be used if TriggerVersion is used before");
  setEntry("TriggerVersionHigh",input);
}

std::string ResInfo::getTriggerVersionHigh() const {
  std::string output="UNDEFINED";
  getEntry("TriggerVersionHigh",output);
  return output;
}


// trigger version can be formatted as "v14" or "14" or "v14.99" or "14.99"
void ResInfo::setTriggerVersionLow( const std::string & input) { 
  if (getTriggerVersion() != "UNDEFINED")
    throw std::runtime_error("TriggerVersionLow keyword can not be used if TriggerVersion is used before");
  setEntry("TriggerVersionLow",input);
}

std::string ResInfo::getTriggerVersionLow() const {
  std::string output="UNDEFINED";
  getEntry("TriggerVersionLow",output);
  return output;
}



// ------------ Object meta data ------------------------------------

void ResInfo::setDataType( const std::string & type) { setEntry("DataType",type);}
  
std::string ResInfo::getDataType() const {
  std::string type="UNDEFINED";
  getEntry("DataType",type);
  return type;
}

void ResInfo::setObjType( const std::string & type) { setEntry("ObjType",type);}
  
std::string ResInfo::getObjType() const {
  std::string type="UNDEFINED";
  getEntry("ObjType",type);
  return type;
}
  
void ResInfo::setObjQuality( const std::string & quality) { setEntry("ObjQuality",quality);}

std::string ResInfo::getObjQuality() const {
  std::string quality = "UNDEFINED";
  getEntry("ObjQuality",quality);
  return quality;
}
  
  
void ResInfo::setObjVersion( int version) {
  setEntry("ObjVersion",version);
}
  
int ResInfo::getObjVersion() const {
  int retval = -1;
  getEntry("ObjVersion",retval);
  return retval;
}
  
void ResInfo::setObjRelativeTo( const std::string & input) { setEntry("ObjRelativeTo",input);}

std::string ResInfo::getObjRelativeTo() const {
  std::string output="UNDEFINED";
  getEntry("ObjRelativeTo",output);
  return output;
}


void ResInfo::setObjAlgorithm( const std::string & algo ) {
  setEntry("ObjAlgorithm",algo);
}
  
std::string ResInfo::getObjAlgorithm() const {
  std::string algo = "";
  getEntry("ObjAlgorithm",algo);
  return algo;
}

// ------------ Additional meta data ---------------------------------

std::string ResInfo::getATLASNoteID() const {
  std::string retval = "";
  getEntry("ATLASNoteID",retval);
  return retval;
}
    
void ResInfo::setATLASNoteID( const std::string & note )  {
  setEntry("ATLASNoteID",note);
}
  
/// several comments line could be added
void ResInfo::setComments( const std::string & input) { setEntry("Comments",input);}

std::vector <std::string>  ResInfo::getComments() const {
  std::vector <std::string> output;
  getEntry("Comments",output);
  return output;
}

// --------------- META DATA FOR FUNCTIONS ------------

void ResInfo::setResFuncNames(const std::vector <std::string> & names ) { setEntry("ResFuncNames",names); }
  
std::vector < std::string> ResInfo::getResFuncNames() const {
  std::vector <std::string> names;
  getEntry("ResFuncNames",names);
  return names;
}

void ResInfo::setResFuncBegin(const std::vector <double> & begin ) { setEntry("ResFuncBegin",begin); }
  
std::vector <double> ResInfo::getResFuncBegin() const {
  std::vector <double> begin;
  getEntry("ResFuncBegin",begin);
  return begin;
}

void ResInfo::setResFuncEnd(const std::vector <double> & end ) { setEntry("ResFuncEnd",end); }
  
std::vector <double> ResInfo::getResFuncEnd() const {
  std::vector <double> end;
  getEntry("ResFuncEnd",end);
  return end;
}
  
int ResInfo::getResNFunc() const { 
  int n1 = 0;
  int n2 = 0;
  int n3 = 0;
  bool retval1 = getNParameters("ResFuncNames", n1);
  bool retval2 = getNParameters("ResFuncBegin", n2);
  bool retval3 = getNParameters("ResFuncEnd", n3);
  if( retval1 && retval2 && retval3) {
    if(n1 == n2 && n2 == n3) 
      return n1;
    else
      return -1;
  }
  else
    return -1;
}

// --------------- END of the META DATA  --------------

std::string ResInfo::makeFileName() const {
  std::string _filename;
  std::vector< std::string > axes = getResVarNames();
  int dim = axes.size();
  
  _filename = getDataType() + "_" + getResName() + "_" + getObjType() ;
  if (getObjQuality()!="UNDEFINED") _filename += "_" + getObjQuality() ;
  _filename += "_" + convertToString(dim) + "D";
  for( int i = 0; i < dim; ++i) _filename += "_" + axes[i];
  _filename += "_" + getResType() + ".mcr" ;
  if(!_filename.empty())
    return _filename;
  else {
    _filename = "NOFILENAME";
    return _filename;  
  }
}


template<class T> void ResInfo::setEntry( const std::string & id, const T & value) {
  std::string entry = convertToString( value );
  if (entry.empty()) return ;
  std::vector< std::string> vec;
  vec.push_back( entry );
  m_map[id] = vec;
}

template<class T> void ResInfo::setEntry( const std::string & id,const std::vector< T > & value) {
  if(value.size() == 0) return;
  std::vector< std::string> vec;
  for(int i = 0; i < int(value.size()); i++) {
    std::string entry = convertToString( value[i] );
    if (entry.empty()) return ;
    //std::vector< std::string> vec;
    vec.push_back( entry );
  }
  m_map[id] = vec;
}


  
template< class T> bool ResInfo::addToEntry( const std::string & id, const T & value) {
  std::map< std::string, std::vector< std::string> >::iterator mapIter = m_map.find(id);
  if( mapIter == m_map.end() ) 
    return false;
  std::vector< std::string > & vec = mapIter->second;
  std::string val = convertToString( value );
  vec.push_back( val );
  return true;
}
  
template<class T> bool ResInfo::getEntry( const std::string & id, T & value, int i) const  {
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


template<class T> bool ResInfo::getEntry( const std::string & id,std::vector< T > & value) const  {
  T tmp;
  std::map<std::string, std::vector<std::string> >::const_iterator mapIter = m_map.find(id);
  if( mapIter == m_map.end() ) 
    return false;
    
  const  std::vector< std::string > & vec = mapIter->second;
  if((int) vec.size() == 0 )
    return false;

  for(int i = 0; i < (int) vec.size(); i++) {
    const std::string & svalue = vec.at(i);
    std::istringstream ist( svalue );
    if( ist >> tmp ) {
      value.push_back(tmp);
    } else 
      return false;
  }
  return true;
}
  

bool ResInfo::getEntry( const std::string & id, std::vector< std::string> & vec) const  {
  // now lets get the whole vector!
  std::map<std::string, std::vector<std::string> >::const_iterator mapIter = m_map.find(id);
  if( mapIter == m_map.end() ) 
    return false;
    
  vec = mapIter->second;
  return true;
}


  
bool ResInfo::getNParameters( const std::string & id, int & i) const {
  std::map< std::string, std::vector<std::string> >::const_iterator mapIter = m_map.find(id);
  if( mapIter == m_map.end() )
    return false;
  else {
    const std::vector< std::string > & vec = mapIter->second;
    i = vec.size();
    return true;
  }
}
  
template<class type> std::string ResInfo::convertToString(const type & val)  const {
  std::ostringstream streamOut;
  streamOut << val;
  return streamOut.str();
}
  


bool ResInfo::parseInputLine( const std::string & key, const std::vector< std::string> & line )
{

  
  int n = line.size();
  
  //bool retval = ParseInputLine( key, line );
  bool retval = true;
  // look for our tokens first
  if( key == "DataType" && n == 1) 
    setDataType( line.at(0));
  else if( key == "ResType" && n == 1) 
    setResType( line.at(0));
  else if( key == "ResName" && n == 1) 
    setResName( line.at(0) );
  else if( key == "ResVersion" && n==1 ){
    int ver = atoi( line.at(0).c_str());
    setResVersion( ver );
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


bool ResInfo::contains (const ResInfo& info) const {
  
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


