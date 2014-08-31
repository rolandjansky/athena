/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Test for StringSerializer
 *
 * See the requirements file on how to compile/run this.
 */

#include "TrigSerializeResult/StringSerializer.h"

#include <string>
#include <vector>

using namespace std;

int main() {
  vector<string> v;
  
  v.push_back("initialRoI");
  v.push_back("initialRoI");
  v.push_back("initialRoI");
  v.push_back("initialRoI");
  v.push_back("TrigT2CaloEgamma");

  StringSerializer ss; 

  // serialize strings into uv
  vector<uint32_t> uv;
  ss.serialize(v, uv);

  // deserialize from uv
  vector<string> vout;
  ss.deserialize(uv, vout);
  cout << dec << "# strings = " << vout.size() << endl;
  for ( unsigned i = 0; i <  vout.size(); i++ ) 
    cout << ">" << vout[i] << "<" << endl;

  if ( v != vout ) {
    cout << "ERROR in serialize/deserialize sequence" << endl;
  }

  // Do the same again
  uv.clear();
  vout.clear();
  ss.serialize(v, uv);
  ss.deserialize(uv, vout);
  for ( unsigned i = 0; i <  vout.size(); i++ ) 
    cout << ">" << vout[i] << "<" << endl;

  if ( v != vout ) {
    cout << "ERROR in serialize/deserialize sequence, run for the 2nd time" << endl;
  }

  // Short string (zero padding needed)
  vector<string> v1;
  v1.push_back("123");
  uv.clear();
  vout.clear();
  ss.serialize(v1, uv);
  ss.deserialize(uv, vout);
  for ( unsigned i = 0; i <  vout.size(); i++ ) 
    cout << ">" << vout[i] << "<" << endl;

  if ( v1 != vout ) {
    cout << "ERROR in serialize/deserialize on small string in small vector" << endl;
  }

  // Individual string
  string s("My individual string");
  string sout;
  uv.clear();
  ss.serialize(s, uv);
  ss.deserialize(uv, sout);
  cout << ">" << sout << "<" << endl;  
  if ( s != sout ) {
    cout << "ERROR in serialize/deserialize on individual string" << endl;
  }

  // Empty string
  s = "";
  uv.clear();
  sout = "";
  ss.serialize(s, uv);
  ss.deserialize(uv, sout);
  cout << ">" << sout << "<" << endl;  
  if ( s != sout ) {
    cout << "ERROR in serialize/deserialize of empty string" << endl;
  }

  // Test return value of deserialize
  s = "123456789";
  uv.clear();
  sout = "";
  ss.serialize(s, uv);
  // Add extra payload at end. String should not be affected
  uv.push_back(11);
  uv.push_back(22);
  size_t nWords = ss.deserialize(uv, sout);
  cout << ">" << sout << "<" << endl;  
  cout << "appended payload = " << uv[nWords] << " " << uv[nWords+1] << endl;
  if ( s != sout ) {
    cout << "ERROR in serialize/deserialize when adding extra payload" << endl;
  }

  // Deserialize
  ss.deserialize(uv.begin(), uv.end()-4, sout);
  cout << ">" << sout << "<" << endl;
  if ( "34567" != sout ) {
    cout << "ERROR in partial serialize/deserialize" << endl;
  }  
}


