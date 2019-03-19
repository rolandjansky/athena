/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/TrigConfData.h"

#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;
using namespace TrigConf;

TrigConfData::TrigConfData(const string& name) : 
   m_smk(0),
   m_id(0),
   m_name(name),
   m_version(0),
   m_comment("")
{}

std::ostream&
TrigConfData::indent(std::ostream& o, int lvl, int size) const {
   int width = lvl*size;
   if(width==0) return o;
   o << setw(lvl*size) << " ";
   return o;
}

void
TrigConfData::printNameIdV(const string& indent) const {
   cout << indent << name();
   if(id()>0 || version()>0)
      cout << " (id=" << id() << "/v=" << version() << ")";
   cout  << endl;
   if(comment()!="")
      cout << indent << "Comment: " << comment() << endl;
}


std::ostream &
TrigConf::operator<<(std::ostream & o, const TrigConf::TrigConfData & data) {
   o <<  data.name();
   if(data.id()>0 || data.version()>0)
      o << " (id=" << data.id() << "/v=" << data.version() << ")";
   return o;
}

string
TrigConfData::__str__() const {
   stringstream s;
   s << *this;
   return s.str();
}

