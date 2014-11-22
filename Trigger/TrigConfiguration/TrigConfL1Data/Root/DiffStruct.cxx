/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/DiffStruct.h"

#include "boost/lexical_cast.hpp"

#include <iostream>
#include <fstream>

using namespace std;

TrigConf::DiffStruct::DiffStruct(const std::string& tagname, const std::string& name) :
   tagname(tagname),
   name(name)
{}

TrigConf::DiffStruct::~DiffStruct() {
   for(DiffStruct* d : subs)
      delete d;
}


void
TrigConf::DiffStruct::check(const std::string& attname, const std::string& lval, const std::string& rval) {
   if(lval!=rval) attdiffs.push_back(AttDiff(attname, lval, rval));
}

void
TrigConf::DiffStruct::check(const std::string& attname, int lval, int rval) {
   if(lval!=rval)
      attdiffs.push_back(AttDiff(attname, boost::lexical_cast<string,int>(lval), boost::lexical_cast<string,int>(rval) ));
}

void
TrigConf::DiffStruct::check(const std::string& attname, unsigned int lval, unsigned int rval) {
   if(lval!=rval)
      attdiffs.push_back(AttDiff(attname, boost::lexical_cast<string,unsigned int>(lval), boost::lexical_cast<string,unsigned int>(rval) ));
}

void
TrigConf::DiffStruct::check(const std::string& attname, float lval, float rval) {
   if(lval!=rval)
      attdiffs.push_back(AttDiff(attname, boost::lexical_cast<string,float>(lval), boost::lexical_cast<string,float>(rval) ));
}

void
TrigConf::DiffStruct::check(const std::string& attname, bool lval, bool rval) {
   if(lval!=rval)
      attdiffs.push_back(AttDiff(attname, boost::lexical_cast<string,bool>(lval), boost::lexical_cast<string,bool>(rval) ));
}

void
TrigConf::DiffStruct::addSub(DiffStruct* sub) {
   if(sub) subs.push_back(sub);
}

void
TrigConf::DiffStruct::addRightOnlySub(const string& tagname, const string& name) {
   rightonly.insert( std::pair<string,string>(tagname, name) );
}

void
TrigConf::DiffStruct::addLeftOnlySub(const string& tagname, const string& name) {
   leftonly.insert( std::pair<string,string>(tagname, name) );
}

bool
TrigConf::DiffStruct::empty() const {
   return attdiffs.size()==0 && subs.size()==0;
}


void
TrigConf::DiffStruct::writeXML(ofstream & xmlfile, const std::string& prefix) const {
   if(empty()) return;
   xmlfile << prefix << "<" << tagname;
   if(name!="")
      xmlfile << " name=\"" << name << "\"";
   for(AttDiff d : attdiffs)
      xmlfile << " " << d.attname << "_l=\"" << d.lval << "\" " << d.attname << "_r=\"" << d.rval << "\"";
   if(subs.size()==0 && rightonly.size()==0 && leftonly.size()==0) {
      xmlfile << "/>" << endl;
   } else {
      xmlfile << ">" << endl;
      for(DiffStruct *d : subs)
         d->writeXML(xmlfile, prefix+"    ");
      if(leftonly.size()>0) {
         xmlfile << prefix << "    <LEFT_ONLY>" << endl;
         for(OnlyMap_t::value_type tag_name : leftonly)
            xmlfile << prefix << "        <" << tag_name.first << " name=\"" << tag_name.second << "\"/>" << endl;
         xmlfile << prefix << "    </LEFT_ONLY>" << endl;
      }
      if(rightonly.size()>0) {
         xmlfile << prefix << "    <RIGHT_ONLY>" << endl;
         for(OnlyMap_t::value_type tag_name : rightonly)
            xmlfile << prefix << "        <" << tag_name.first << " name=\"" << tag_name.second << "\"/>" << endl;
         xmlfile << prefix << "    </RIGHT_ONLY>" << endl;
      }
      xmlfile << prefix << "</" << tagname << ">" << endl;
   }
}
