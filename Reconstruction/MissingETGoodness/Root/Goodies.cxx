/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MissingETGoodness/Goodies.h"
#include "TBuffer.h"
#include <iostream>

ClassImp(MET::Goodies)

// Constructor
MET::Goodies::Goodies(Long64_t entry) :
  m_entry(entry),
  m_runnr(-1)
{
  m_gInt.clear();
  m_gDouble.clear();
  m_gVectorInt.clear();
  m_gVectorDouble.clear();
}


// Destructor
MET::Goodies::~Goodies() 
{
  clear();
}


MET::Goodies&
MET::Goodies::instance() {
  // created on first call, deleted after main()
  static MET::Goodies instance;
  return instance;
}


MET::Goodies& 
MET::Goodies::operator=(const MET::Goodies& other)
{
  if (&other==this) {
    return *this ;
  } 
  m_entry     = other.m_entry ;
  m_runnr     = other.m_runnr ;
  m_gDouble   = other.m_gDouble ;
  m_gInt      = other.m_gInt ;
  m_gVectorInt   = other.m_gVectorInt ;
  m_gVectorDouble   = other.m_gVectorDouble ;

  return *this ;
}


void
MET::Goodies::clear()
{
  m_entry = -1;
  m_runnr = -1;
  m_gInt.clear();
  m_gDouble.clear();

  std::map<TString,std::vector<int> >::iterator gVectorIItr = m_gVectorInt.begin();
  for (; gVectorIItr != m_gVectorInt.end(); ++gVectorIItr) gVectorIItr->second.clear();
  std::map<TString,std::vector<double> >::iterator gVectorDItr = m_gVectorDouble.begin();
  for (; gVectorDItr != m_gVectorDouble.end(); ++gVectorDItr) gVectorDItr->second.clear();

  m_gVectorInt.clear();
  m_gVectorDouble.clear();
}


int MET::Goodies::get(const TString& varKey, const int& defaultValue) const
{
  int varValue(defaultValue);

  bool found(false);
  if (!found) {
    std::map<TString,int>::const_iterator gIntItr = m_gInt.find(varKey);
    if (gIntItr != m_gInt.end()) {
      varValue = (*gIntItr).second;
      found = true;
    }
  }
  if (!found) {
    std::map<TString,double>::const_iterator gDoubleItr = m_gDouble.find(varKey);
    if (gDoubleItr != m_gDouble.end()) {
      varValue = static_cast<int>((*gDoubleItr).second);
      found = true;
    }
  }
  return varValue;
}


double MET::Goodies::get(const TString& varKey, const double& defaultValue) const
{
  double varValue(defaultValue);

  bool found(false);
  if (!found) {
    std::map<TString,double>::const_iterator gDoubleItr = m_gDouble.find(varKey);
    if (gDoubleItr != m_gDouble.end()) {
      varValue = (*gDoubleItr).second;
      found = true;
    }
  }
  if (!found) {
    std::map<TString,int>::const_iterator gIntItr = m_gInt.find(varKey);
    if (gIntItr != m_gInt.end()) {
      varValue = static_cast<double>((*gIntItr).second);
      found = true;
    }
  }

  return varValue;
}

std::vector<double> MET::Goodies::get(const TString& varKey, const std::vector<double>& defaultValue) const
{
  std::vector<double> varValue(defaultValue);

  bool found(false);
  if (!found) {
    std::map<TString,std::vector<double> >::const_iterator gVectorItr = m_gVectorDouble.find(varKey);
    if (gVectorItr != m_gVectorDouble.end()) {
      varValue = (*gVectorItr).second;
      found = true;
    }
  }

  return varValue;
}

std::vector<int> MET::Goodies::get(const TString& varKey, const std::vector<int>& defaultValue) const
{
  std::vector<int> varValue(defaultValue);

  bool found(false);
  if (!found) {
    std::map<TString,std::vector<int> >::const_iterator gVectorItr = m_gVectorInt.find(varKey);
    if (gVectorItr != m_gVectorInt.end()) {
      varValue = (*gVectorItr).second;
      found = true;
    }
  }

  return varValue;
}


bool 
MET::Goodies::contains(const TString& varKey, bool suprWarn) const
{
  bool found(false);

  if (!found) {
    std::map<TString,double>::const_iterator gDoubleItr = m_gDouble.find(varKey);
    if (gDoubleItr != m_gDouble.end()) { found = true; }
  }
  if (!found) {
    std::map<TString,int>::const_iterator gIntItr = m_gInt.find(varKey);
    if (gIntItr != m_gInt.end()) { found = true; }
  }
  if (!found) {
    std::map<TString, std::vector<double> >::const_iterator gVecDoubleItr = m_gVectorDouble.find(varKey);
    if (gVecDoubleItr != m_gVectorDouble.end()) { found = true; }
  }
  if (!found) {
    std::map<TString, std::vector<int> >::const_iterator gVecIntItr = m_gVectorInt.find(varKey);
    if (gVecIntItr != m_gVectorInt.end()) { found = true; }
  }

  // variable still not found ...
  if (!found)
    if (!suprWarn) std::cout << "MET::Goodies::contains() : Variable name "
                             << varKey << " not found in event."
                             << std::endl;

  return found;
}


void
MET::Goodies::print()
{ 
  TString varName;
  
  std::cout << "MET::Goodies::print() : Properties of event <" << m_entry << ">, run <" << m_runnr << "> : " << std::endl;
    
  std::map<TString,double>::const_iterator gDoubleItr = m_gDouble.begin();
  for (; gDoubleItr != m_gDouble.end(); ++gDoubleItr) {
    varName  = (*gDoubleItr).first;
    double varValue = (*gDoubleItr).second;
    std::cout << ">> " << varName << " (d) : " << varValue << std::endl;
  }
  std::map<TString,int>::const_iterator gIntItr = m_gInt.begin();
  for (; gIntItr != m_gInt.end(); ++gIntItr) {
    varName  = (*gIntItr).first;
    int varValue = (*gIntItr).second;
    std::cout << ">> " << varName << " (i) : " << varValue << std::endl;
  }
  std::map<TString,std::vector<int> >::const_iterator gVecIntItr = m_gVectorInt.begin();
  for (; gVecIntItr != m_gVectorInt.end(); ++gVecIntItr) {
    varName  = (*gVecIntItr).first;
    std::vector<int> varValue = (*gVecIntItr).second;
    std::vector<int>::const_iterator iter=varValue.begin();
    for (; iter != varValue.end(); ++iter) {
      std::cout << ">> " << varName << " (v) : " << *iter << std::endl;
    }
  }
  std::map<TString,std::vector<double> >::const_iterator gVecDoubleItr = m_gVectorDouble.begin();
  for (; gVecDoubleItr != m_gVectorDouble.end(); ++gVecDoubleItr) {
    varName  = (*gVecDoubleItr).first;
    std::vector<double> varValue = (*gVecDoubleItr).second;
    std::vector<double>::const_iterator iter=varValue.begin();
    for (; iter != varValue.end(); ++iter) {
      std::cout << ">> " << varName << " (v) : " << *iter << std::endl;
    }
  }
}


void
MET::Goodies::Streamer(TBuffer &R__b) 
{
   // Stream an object of class RooRealVar.

   UInt_t R__s, R__c;
   TString varName;
   double varValueD;
   int varValueI;
   TString varValueS;
   int mapSize;

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); 
      if (R__v) { }
      TObject::Streamer(R__b);

      // event info
      R__b >> m_entry;
      R__b >> m_runnr;

      // gInt,gDouble
      m_gDouble.clear();
      R__b >> mapSize;
      if (mapSize) {
	for (int i=0; i<mapSize; ++i) {
	  varName.Streamer(R__b);
	  R__b >> varValueD;
	  m_gDouble[varName.Data()] = varValueD;
	}
      }
      m_gInt.clear();
      R__b >> mapSize;
      if (mapSize) {
	for (int i=0; i<mapSize; ++i) {
	  varName.Streamer(R__b);
	  R__b >> varValueI;
	  m_gInt[varName.Data()] = varValueI;
	}
      }    

      R__b.CheckByteCount(R__s, R__c, MET::Goodies::IsA());
   } else {
      R__c = R__b.WriteVersion(MET::Goodies::IsA(), kTRUE);
      TObject::Streamer(R__b);

      // event info
      R__b << m_entry;
      R__b << m_runnr;

      // gInt,gDouble
      mapSize = int(m_gDouble.size());
      R__b << mapSize;
      if (mapSize) {
	std::map<TString,double>::const_iterator gDoubleItr = m_gDouble.begin();
	for (; gDoubleItr != m_gDouble.end(); ++gDoubleItr) {
	  varName  = (*gDoubleItr).first;
	  varValueD = (*gDoubleItr).second;
	  varName.Streamer(R__b);
	  R__b << varValueD;
	}
      }
      mapSize = int(m_gInt.size());
      R__b << mapSize;
      if (mapSize) {
	std::map<TString,int>::const_iterator gIntItr = m_gInt.begin();
	for (; gIntItr != m_gInt.end(); ++gIntItr) {
	  varName  = (*gIntItr).first;
	  varValueI = (*gIntItr).second;
	  varName.Streamer(R__b);
	  R__b << varValueI;
	}
      }     

      R__b.SetByteCount(R__c, kTRUE);
   }
}

