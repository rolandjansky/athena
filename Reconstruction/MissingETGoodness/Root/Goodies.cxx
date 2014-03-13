/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MissingETGoodness/Goodies.h"
#include <iostream>

ClassImp(MET::Goodies)

// Constructor
MET::Goodies::Goodies(Long64_t entry) :
  _entry(entry),
  _runnr(-1)
{
  _gInt.clear();
  _gDouble.clear();
  _gVectorInt.clear();
  _gVectorDouble.clear();
}


// Destructor
MET::Goodies::~Goodies() 
{
  clear();
}


MET::Goodies&
MET::Goodies::instance() {
  // created on first call, deleted after main()
  static MET::Goodies _instance;
  return _instance;
}


MET::Goodies& 
MET::Goodies::operator=(const MET::Goodies& other)
{
  if (&other==this) {
    return *this ;
  } 
  _entry     = other._entry ;
  _runnr     = other._runnr ;
  _gDouble   = other._gDouble ;
  _gInt      = other._gInt ;
  _gVectorInt   = other._gVectorInt ;
  _gVectorDouble   = other._gVectorDouble ;

  return *this ;
}


void
MET::Goodies::clear()
{
  _entry = -1;
  _runnr = -1;
  _gInt.clear();
  _gDouble.clear();

  std::map<TString,std::vector<int> >::iterator gVectorIItr = _gVectorInt.begin();
  for (; gVectorIItr != _gVectorInt.end(); ++gVectorIItr) gVectorIItr->second.clear();
  std::map<TString,std::vector<double> >::iterator gVectorDItr = _gVectorDouble.begin();
  for (; gVectorDItr != _gVectorDouble.end(); ++gVectorDItr) gVectorDItr->second.clear();

  _gVectorInt.clear();
  _gVectorDouble.clear();
}


int MET::Goodies::get(const TString& varKey, const int& defaultValue) const
{
  int varValue(defaultValue);

  bool found(false);
  if (!found) {
    std::map<TString,int>::const_iterator gIntItr = _gInt.find(varKey);
    if (gIntItr != _gInt.end()) {
      varValue = (*gIntItr).second;
      found = true;
    }
  }
  if (!found) {
    std::map<TString,double>::const_iterator gDoubleItr = _gDouble.find(varKey);
    if (gDoubleItr != _gDouble.end()) {
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
    std::map<TString,double>::const_iterator gDoubleItr = _gDouble.find(varKey);
    if (gDoubleItr != _gDouble.end()) {
      varValue = (*gDoubleItr).second;
      found = true;
    }
  }
  if (!found) {
    std::map<TString,int>::const_iterator gIntItr = _gInt.find(varKey);
    if (gIntItr != _gInt.end()) {
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
    std::map<TString,std::vector<double> >::const_iterator gVectorItr = _gVectorDouble.find(varKey);
    if (gVectorItr != _gVectorDouble.end()) {
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
    std::map<TString,std::vector<int> >::const_iterator gVectorItr = _gVectorInt.find(varKey);
    if (gVectorItr != _gVectorInt.end()) {
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
    std::map<TString,double>::const_iterator gDoubleItr = _gDouble.find(varKey);
    if (gDoubleItr != _gDouble.end()) { found = true; }
  }
  if (!found) {
    std::map<TString,int>::const_iterator gIntItr = _gInt.find(varKey);
    if (gIntItr != _gInt.end()) { found = true; }
  }
  if (!found) {
    std::map<TString, std::vector<double> >::const_iterator gVecDoubleItr = _gVectorDouble.find(varKey);
    if (gVecDoubleItr != _gVectorDouble.end()) { found = true; }
  }
  if (!found) {
    std::map<TString, std::vector<int> >::const_iterator gVecIntItr = _gVectorInt.find(varKey);
    if (gVecIntItr != _gVectorInt.end()) { found = true; }
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
  
  std::cout << "MET::Goodies::print() : Properties of event <" << _entry << ">, run <" << _runnr << "> : " << std::endl;
    
  std::map<TString,double>::const_iterator gDoubleItr = _gDouble.begin();
  for (; gDoubleItr != _gDouble.end(); ++gDoubleItr) {
    varName  = (*gDoubleItr).first;
    double varValue = (*gDoubleItr).second;
    std::cout << ">> " << varName << " (d) : " << varValue << std::endl;
  }
  std::map<TString,int>::const_iterator gIntItr = _gInt.begin();
  for (; gIntItr != _gInt.end(); ++gIntItr) {
    varName  = (*gIntItr).first;
    int varValue = (*gIntItr).second;
    std::cout << ">> " << varName << " (i) : " << varValue << std::endl;
  }
  std::map<TString,std::vector<int> >::const_iterator gVecIntItr = _gVectorInt.begin();
  for (; gVecIntItr != _gVectorInt.end(); ++gVecIntItr) {
    varName  = (*gVecIntItr).first;
    std::vector<int> varValue = (*gVecIntItr).second;
    std::vector<int>::const_iterator iter=varValue.begin();
    for (; iter != varValue.end(); ++iter) {
      std::cout << ">> " << varName << " (v) : " << *iter << std::endl;
    }
  }
  std::map<TString,std::vector<double> >::const_iterator gVecDoubleItr = _gVectorDouble.begin();
  for (; gVecDoubleItr != _gVectorDouble.end(); ++gVecDoubleItr) {
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
      R__b >> _entry;
      R__b >> _runnr;

      // gInt,gDouble
      _gDouble.clear();
      R__b >> mapSize;
      if (mapSize) {
	for (int i=0; i<mapSize; ++i) {
	  varName.Streamer(R__b);
	  R__b >> varValueD;
	  _gDouble[varName.Data()] = varValueD;
	}
      }
      _gInt.clear();
      R__b >> mapSize;
      if (mapSize) {
	for (int i=0; i<mapSize; ++i) {
	  varName.Streamer(R__b);
	  R__b >> varValueI;
	  _gInt[varName.Data()] = varValueI;
	}
      }    

      R__b.CheckByteCount(R__s, R__c, MET::Goodies::IsA());
   } else {
      R__c = R__b.WriteVersion(MET::Goodies::IsA(), kTRUE);
      TObject::Streamer(R__b);

      // event info
      R__b << _entry;
      R__b << _runnr;

      // gInt,gDouble
      mapSize = int(_gDouble.size());
      R__b << mapSize;
      if (mapSize) {
	std::map<TString,double>::const_iterator _gDoubleItr = _gDouble.begin();
	for (; _gDoubleItr != _gDouble.end(); ++_gDoubleItr) {
	  varName  = (*_gDoubleItr).first;
	  varValueD = (*_gDoubleItr).second;
	  varName.Streamer(R__b);
	  R__b << varValueD;
	}
      }
      mapSize = int(_gInt.size());
      R__b << mapSize;
      if (mapSize) {
	std::map<TString,int>::const_iterator _gIntItr = _gInt.begin();
	for (; _gIntItr != _gInt.end(); ++_gIntItr) {
	  varName  = (*_gIntItr).first;
	  varValueI = (*_gIntItr).second;
	  varName.Streamer(R__b);
	  R__b << varValueI;
	}
      }     

      R__b.SetByteCount(R__c, kTRUE);
   }
}

