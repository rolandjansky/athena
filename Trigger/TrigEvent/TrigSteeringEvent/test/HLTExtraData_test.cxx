/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Test of HLTExtraData class
 *
 * @author Frank Winklmeier
 */
#ifdef XAOD_STANDALONE
int main(){return 0;}
#endif


#ifndef XAOD_STANDALONE


#include "TrigSteeringEvent/HLTExtraData.h"
#include <vector>
#include <iostream>

using namespace std;
using namespace HLT;


ostream& operator<< (ostream& os, const HLTExtraData& x)
{
  os << ">";
  for (size_t i=0; i < x.appName.size(); i++) {
    if (isprint (x.appName[i]))
      os << x.appName[i];
    else
      os << "\\0x" << std::hex << (int)x.appName[i] << std::dec;
  }
  os << "< " << "[" << x.statusCode << "]";
  for (size_t i=0; i<x.anonymous.size(); ++i) os << " " << x.anonymous[i];
  return os;
}

bool operator==(const HLTExtraData& x1, const HLTExtraData& x2)
{
  return (x1.appName==x2.appName &&
          x1.statusCode==x2.statusCode &&
          x1.anonymous==x2.anonymous);
}

bool operator!=(const HLTExtraData& x1, const HLTExtraData& x2)
{
  return !(x1==x2);
}

int main()
{
  HLTExtraData x1, x2;
  vector<uint32_t> v;

  // Test (de)serialization
  x1.appName = "My application.name ";
  x1.statusCode = 2435;
  x1.anonymous.push_back(77);
  x1.anonymous.push_back(33);
  x1.serialize(v);
  x2.deserialize(v);

  cout << "Storage size: " << v.size() << endl;
  cout << "Before: " << x1 << endl;
  cout << "After:  " << x2 << endl;

  int rc(0);
  if (x1!=x2) {
    cout << "ERROR in deserialization" << endl;
    rc = 1;
  }

  // Test deserialization into same object
  x2.deserialize(v);
  if (x1!=x2) {
    cout << "ERROR in second deserialization" << endl;
    rc = 1;
  }
  
  // Test constructor
  HLTExtraData x3(v);
  if (x1!=x3) {
    cout << "ERROR in deserialization when using constructor" << endl;
    rc = 1;
  }

  // Test default constructucted object
  HLTExtraData x4;
  v.clear();
  x4.serialize(v);
  if (!v.empty()) {
    cout << "ERROR Default constructed object should result in empty payload" << endl;
    rc = 1;
  }

  // Test deserialization from empty payload
  x4.deserialize(v);
  if (x4 != HLTExtraData()) {
    cout << "ERROR Serialization from empty payload should result in default object" << endl;
    rc = 1;
  }
  
  // Test with garbage input (at least should not crash)
  HLTExtraData x5;
  vector<uint32_t> garbage;
  for (int i=1; i<20; ++i) garbage.push_back(i);
  x5.deserialize(garbage);
  cout << "From garbage: " << x5 << endl;
  
  return rc;
}
#endif
