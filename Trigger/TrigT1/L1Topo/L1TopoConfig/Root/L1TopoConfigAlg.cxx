/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoConfig/L1TopoConfigAlg.h"
#include "L1TopoCommon/Exception.h"

#include "boost/foreach.hpp"

#include <iostream>
#include <iomanip>

using namespace std;
using namespace TXC;

ostream&
operator<<(ostream& o, const TXC::InputElement& ie) {
   o << "name: " << ie.name << ", value: " << ie.value << ", pos: " << ie.position;
   return o;
}

ostream& operator<<(ostream& o, const TXC::OutputElement& oe) {
   o << "name: " << oe.name << ", value: " << oe.value << ", nbits: " << oe.nbits << ", outname: " << oe.outname << ", position: " << oe.position;
   return o;
}

ostream&
operator<<(ostream& o, const TXC::FixedParameter& ge) {
   o << "name: " << ge.name << ", value: " << ge.value;
   return o;
}

ostream&
operator<<(ostream& o, const TXC::RegisterParameter& pe) {
   o << "name: " << pe.name << ", value: " << pe.value << ", pos: " << pe.position << ", sel: " << pe.selection;
   return o;
}




/**
 * class L1TopoConfigAlg 
 **/

TXC::L1TopoConfigAlg::L1TopoConfigAlg(const string & name, const string & type) :
  m_name(name),
  m_type(type)
{}


TXC::L1TopoConfigAlg::~L1TopoConfigAlg() {}

string
TXC::L1TopoConfigAlg::fullname() const {
   return type() + "/" + name();
}


vector<string>
L1TopoConfigAlg::getInputNames() const {
   vector<string> names;
   for(auto & x : m_inputElements)
      names.push_back(x.value);
   return names;
}

vector<string>
L1TopoConfigAlg::getOutputNames() const {
   vector<string> names;
   for(auto & x : m_outputElements)
      names.push_back( isSortAlg() ? x.value : x.outname );
   return names;
}



void
TXC::L1TopoConfigAlg::addInput(const string &name, const string &value, unsigned int position) {
   m_inputElements.push_back(InputElement( name, value, position));
}


void TXC::L1TopoConfigAlg::addOutput(const string &name, const string &value, unsigned int bits, const string & outname, unsigned int position) {
  m_outputElements.push_back(OutputElement( name, value, bits, outname, position));
}

void TXC::L1TopoConfigAlg::addFixedParameter(const std::string &name, const std::string &value) {
  m_fixedParameters.push_back(FixedParameter( name, value));
}

void TXC::L1TopoConfigAlg::addParameter(const std::string &name, const std::string &value, unsigned int position, unsigned int selection) {
  m_variableParameters.push_back( RegisterParameter( name, value, position, selection));
}



std::ostream & operator<<(std::ostream &o, const TXC::L1TopoConfigAlg &alg) {

  if(alg.isSortAlg()) {
     o << "Sorting algorithm " << alg.type() << "/" << alg.name() << endl;
     o << "  Input  : " << alg.m_inputElements[0].value << endl;
     o << "  Output : " << alg.m_outputElements[0].value << endl;
  } else {
     o << "Decision algorithm " << alg.type() << "/" << alg.name() << endl;
     for(TXC::InputElement ie: alg.m_inputElements)
        o << "  Input " << ie.position << " : " << ie.value << endl;
     for(TXC::OutputElement oe: alg.m_outputElements)
        o << "  Output " << oe.position << " : " << oe.outname << endl;
  }
  o << "  Fixed paramters: " << alg.getFixedParameters().size() << endl;
  for(TXC::FixedParameter ge: alg.m_fixedParameters)
     o << "     " << setw(15) << left << ge.name << " : " << ge.value << endl;
  o << "  Parameters: " << alg.getParameters().size() << endl;
  for(TXC::RegisterParameter pe: alg.m_variableParameters)
     o << "     " << setw(15) << left << pe.name << " : " << pe.value << endl;
  return o;
}
