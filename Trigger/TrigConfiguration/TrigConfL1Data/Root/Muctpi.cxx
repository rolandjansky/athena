/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/Muctpi.h"
#include "TrigConfL1Data/Run3MuonTriggerThreshold.h"
#include <iostream>
#include <sstream>

using namespace std;

void
TrigConf::Muctpi::createRoiExclusionList(const std::string & name, Detector det) {
   if(det == RPC) {
      m_rpcExcl[name] = std::vector<std::pair<std::string, std::vector<unsigned int>>>();
   } else {
      m_tgcExcl[name] = std::vector<std::pair<std::string, std::vector<unsigned int>>>();
   }
}

namespace {
   
   std::vector<unsigned int>
   intVectorFromString(const std::string & str) { // "1,2,3,4,5,6,14"
      std::vector<unsigned int> v;
      std::stringstream ss(str);
      unsigned int i;
      while (ss >> i) {
         v.push_back(i);
         if (ss.peek() == ',')
            ss.ignore();
      }
      return v;
   }
}

void
TrigConf::Muctpi::addSectorRoiList(const std::string & listname, Detector det, const std::string & sectorname, const std::string & roiids) {
   auto & list = (det == RPC ? m_rpcExcl.at(listname) : m_tgcExcl.at(listname));

   std::vector<unsigned int> rois = intVectorFromString(roiids);

   list.push_back( {sectorname, rois} );
}

const std::vector<std::pair<std::string, std::vector<unsigned int>>> &
TrigConf::Muctpi::roiExclusionList(const std::string & name, Detector det) const {
   return (det == RPC ? m_rpcExcl.at(name) : m_tgcExcl.at(name));
}

TrigConf::Muctpi::Muctpi() :
   L1DataBaseclass(), 
   m_LowptThreshold(0),
   m_HighptThreshold(0),
   m_MaxCand(0)
{}
  
TrigConf::Muctpi::~Muctpi() {}
  
void
TrigConf::Muctpi::print(const std::string& indent, unsigned int detail) const {
   if(detail>=1) {
      cout << indent << "Muctpi with " << m_thresholds.size() << " new muon thresholds" << endl;
   }
   if(detail>=2) {
      cout << indent << " lvl1 master table id     : " << lvl1MasterTableId() << endl; 
      cout << indent << " low pt threshold         : " << m_LowptThreshold << endl;
      cout << indent << " high pt threshold        : " << m_HighptThreshold << endl;
      cout << indent << " maximal candidates       : " << m_MaxCand << endl;
      cout << indent << " run 3 trigger thresholds : " << m_thresholds.size() << endl;
      for ( auto thr: m_thresholds ) {
         thr->print(indent+indent,detail);
      }
      cout << indent << "RPC exclusion lists:" << endl;
      for( auto list : m_rpcExcl ) {
         cout << indent << "  " << list.first << ": ";
         bool s1(false);
         for( auto sector : list.second ) {
            cout << (s1?", ":"") << sector.first << "["; s1 = true;
            bool s2(false);
            for (unsigned int roi : sector.second) {
               cout << (s2?",":"") << roi; s2 = true;
            }
            cout << "]";
         }
         cout << endl;
      }
      cout << indent << "TGC exclusion lists:" << endl;
      for( auto list : m_tgcExcl ) {
         cout << indent << "  " << list.first << ": ";
         bool s1(false);
         for( auto sector : list.second ) {
            cout << (s1?", ":"") << sector.first << "["; s1 = true;
            bool s2(false);
            for (unsigned int roi : sector.second) {
               cout << (s2?",":"") << roi; s2 = true;
            }
            cout << "]";
         }
         cout << endl;
      }
   }
}  

void
TrigConf::Muctpi::writeXML(std::ostream & xmlfile, int indentLevel, int indentWidth) const {
   indent(xmlfile, indentLevel, indentWidth) << "<MuctpiInfo name=\"" << name() 
                                             << "\" low_pt=\"" << m_LowptThreshold 
                                             << "\" high_pt=\"" << m_HighptThreshold
                                             << "\" max_cand=\"" << m_MaxCand << "\">" << endl;
   for( auto thr : m_thresholds ) {
      thr->writeXML(xmlfile, indentLevel+1, indentWidth);
   }
   indent(xmlfile, indentLevel+1, indentWidth) 
      << "<ExclusionLists>" << endl;

   for( auto list : m_rpcExcl ) {
      if(list.second.size()==0) {
         indent(xmlfile, indentLevel+2, indentWidth) 
            << "<RpcExclROIList name=\"" << list.first << "\"/>" << endl;
      } else {
         indent(xmlfile, indentLevel+2, indentWidth) 
            << "<RpcExclROIList name=\"" << list.first << "\">" << endl;
         
         for( auto sector : list.second ) {
            indent(xmlfile, indentLevel+3, indentWidth) 
               << "<RoiIDlist sectorname=\"" << sector.first
               << "\" roiids=\"";
            bool comma(false);
            for (unsigned int roi : sector.second) {
               xmlfile << (comma?",":"") << roi; comma = true;
            }
            xmlfile << "\"/>" << endl;
         }

         indent(xmlfile, indentLevel+2, indentWidth) 
            << "</RpcExclROIList>" << endl;
      }
   }

   indent(xmlfile, indentLevel+1, indentWidth) 
      << "</ExclusionLists>" << endl;
   indent(xmlfile, indentLevel, indentWidth) 
      << "</MuctpiInfo>" << endl;
}
