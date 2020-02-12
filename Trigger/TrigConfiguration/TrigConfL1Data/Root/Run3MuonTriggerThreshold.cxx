/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/Run3MuonTriggerThreshold.h"
#include <sstream>
using namespace std;

void
TrigConf::Run3MuonTriggerThreshold::Region::setEtamin(int etamin) {
   if( spec()!=UNSPEC && spec()!=ETAPHI ) {
      cout << "ERROR: already a region specification of a different type is given, will ignore etamin=\"" << etamin << "\"" << endl;
   } else {
      m_spec = ETAPHI;
      m_etamin = etamin;
   }
}

void
TrigConf::Run3MuonTriggerThreshold::Region::setEtamax(int etamax) {
   if( spec()!=UNSPEC && spec()!=ETAPHI ) {
      cout << "ERROR: already a region specification of a different type is given, will ignore etamax=\"" << etamax << "\"" << endl;
   } else {
      m_spec = ETAPHI;
      m_etamax = etamax;
   }
}

void
TrigConf::Run3MuonTriggerThreshold::Region::setPhimin(int phimin) {
   if( spec()!=UNSPEC && spec()!=ETAPHI ) {
      cout << "ERROR: already a region specification of a different type is given, will ignore phimin=\"" << phimin << "\"" << endl;
   } else {
      m_spec = ETAPHI;
      m_phimin = phimin;
   }
}

void
TrigConf::Run3MuonTriggerThreshold::Region::setPhimax(int phimax) {
   if( spec()!=UNSPEC && spec()!=ETAPHI ) {
      cout << "ERROR: already a region specification of a different type is given, will ignore phimax=\"" << phimax << "\"" << endl;
   } else {
      m_spec = ETAPHI;
      m_phimax = phimax;
   }
}

namespace {
   
   void tokenize(std::string const &str, const char delim,
                 std::vector<std::string> &out) {
      size_t start;
      size_t end = 0;

      while ((start = str.find_first_not_of(delim, end)) != std::string::npos) {
         end = str.find(delim, start);
         out.push_back(str.substr(start, end - start));
      }
   }
}

void
TrigConf::Run3MuonTriggerThreshold::Region::setDetector(const std::string & detector) {
   if( spec()==ETAPHI ) {
      cout << "ERROR: already a region specification through an eta/phi range is given, will ignore detector=\"" << detector << "\"" << endl;
   } else {
      if(detector=="all") {
         m_spec = FULL;
         m_detector = "all";
         m_region = 7;
      } else {
         m_spec = REGION;
         std::vector<std::string> v;
         tokenize(detector, ',',v);
         for(std::string t : v) {
            if(t == "ba") {
               m_region += 1;
            } else if(t == "ec") {
               m_region += 2;
            } else if(t == "fw") {
               m_region += 4;
            } else {
               m_region += 8;
               cout << "ERROR " << t << " does not specify a detector region, must be either ba, ec, or fw" << endl;
            }
         }
         switch(m_region) {
         case 0: m_detector = "error"; break;
         case 1: m_detector = "ba"; break;
         case 2: m_detector = "ec"; break;
         case 3: m_detector = "ba,ec"; break;
         case 4: m_detector = "fw"; break;
         case 5: m_detector = "ba,fw"; break;
         case 6: m_detector = "ec,fw"; break;
         case 7: m_detector = "ba,ec,fw"; break;
         default: m_detector = "error";
         }
      }
   }
}

std::string
TrigConf::Run3MuonTriggerThreshold::Cable::asString() const {
   stringstream s;
   s << name() << " (type " << type() << ") into " << input() << " as " << connector() << " [" << rangeBegin() << ".." << rangeEnd() << "]";
   return s.str();
}


TrigConf::Run3MuonTriggerThreshold::Run3MuonTriggerThreshold() :
   L1DataBaseclass()
{}

TrigConf::Run3MuonTriggerThreshold::Run3MuonTriggerThreshold(const Run3MuonTriggerThreshold& thr) :
   L1DataBaseclass(thr)
{}
  
TrigConf::Run3MuonTriggerThreshold::~Run3MuonTriggerThreshold()
{}


void
TrigConf::Run3MuonTriggerThreshold::print(const std::string& indent, unsigned int detail) const {
   if(detail>=2) {
      cout << indent << "muon threshold " << name() << ":" << endl;
   }
   if(detail>2) {
      cout << indent << " id " << id() << endl;
      cout << indent << " input " << m_input << endl;
      cout << indent << " bitnum " << m_bitnum << endl;
      cout << indent << " mapping " << m_mapping << endl;
      cout << indent << " is multiplicity threshold -> " << (m_isMultiplicityThr ? "yes" : "no") << endl;
      cout << indent << " definition RPC thr " << rpcThr() << ", TGC thr " << tgcThr() << ", TGC quality flags " << tgcFlags() << endl;
      cout << indent << " region " << m_region.detector() << endl;
      if(rpcExcl()=="" && tgcExcl()=="") {
         cout << indent << "   no exluded ROIs" << endl;
      } else {
         cout << indent << "   exluded ROIs: RPC '" << rpcExcl() << "', TGC '" << tgcExcl() << "'" << endl;         
      }
      cout << indent << " cable " << cable().asString() << endl;
   }
}


void
TrigConf::Run3MuonTriggerThreshold::writeXML(std::ostream & xmlfile, int indentLevel, int indentWidth) const {
   indent(xmlfile, indentLevel, indentWidth) 
      << "<TriggerThreshold name=\"" << name()
      << "\" bitnum=\"" << bitnum()
      << "\" id=\"" << id()
      << "\" mapping=\"" << mapping()
      << "\" isMultiplicityThr=\"" << (isMultiplicityThr() ? "1" : "0")
      << "\" type=\"" << type()
      << "\" input=\"" << input()
      << "\">" << endl;
   if(region().spec() == Region::FULL) {
      indent(xmlfile, indentLevel+1, indentWidth) 
         << "<Region detector=\"all\"/>" << endl;
   } else if( region().spec() == Region::ETAPHI ) {
      indent(xmlfile, indentLevel+1, indentWidth)
         << "<Region etamin=\"" << region().etamin() 
         << "\" etamax=\"" << region().etamax() 
         << "\" phimin=\"" << region().phimin() 
         << "\" phimax=\"" << region().phimax() 
         << "\"/>" << endl;
   } else if( region().spec()==Region::REGION ) {
      indent(xmlfile, indentLevel+1, indentWidth) 
         << "<Region detector=\"" << region().detector() << "\"/>" << endl;
   }
   indent(xmlfile, indentLevel+1, indentWidth) 
      << "<RPC rpcThr=\"" << rpcThr() 
      << "\" rpcExclROIList=\"" << rpcExcl()
      << "\"/>" << endl;
   indent(xmlfile, indentLevel+1, indentWidth) 
      << "<TGC tgcThr=\"" << tgcThr() 
      << "\" tgcFlags=\"" << tgcFlags()
      << "\"/>" << endl;
   indent(xmlfile, indentLevel+1, indentWidth) 
      << "<Cable connector=\"" << cable().connector() 
      << "\" input=\"" << cable().input()
      << "\" type=\"" << cable().type()
      << "\" name=\"" << cable().name()
      << "\">" << endl;
   indent(xmlfile, indentLevel+2, indentWidth) 
      << "<Signal range_begin=\"" << cable().rangeBegin() 
      << "\" range_end=\"" << cable().rangeEnd()
      << "\"/>" << endl;
   indent(xmlfile, indentLevel+1, indentWidth) 
      << "</Cable>" << endl;
   indent(xmlfile, indentLevel, indentWidth) 
      << "</TriggerThreshold>" << endl;
}
