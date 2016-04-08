/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfJobOptData/JobOptionTable.h"
#include <fstream>
#include <algorithm>

using namespace std;
using namespace TrigConf;

TrigConf::JobOptionTable::JobOptionTable() :
   m_SuperMasterTableId(0),
   m_HltMasterTableId(0),
   m_TriggerLevel(0)
{}
  
TrigConf::JobOptionTable::~JobOptionTable() {}

void 
TrigConf::JobOptionTable::addJobOption(const JobOption & jo) {
   m_JobOptionVector.push_back(jo);
}

void
TrigConf::JobOptionTable::print(const std::string& indent, unsigned int detail) const {
   if(detail>=1) {
      cout << indent << "=============================================" << endl;
      cout << indent << "JobOptionTable" << endl;
      cout << indent << "   Super MasterTable Id = " << m_SuperMasterTableId << endl;
      cout << indent << "   HLT   MasterTable Id = " << m_HltMasterTableId << endl;
      cout << indent << "   Number of parameters = " << m_JobOptionVector.size() << endl;
      cout << indent << "=============================================" << endl;
      if(detail>=3)
         for(unsigned int i=0; i < m_JobOptionVector.size();i++)
            m_JobOptionVector[i].print();
   }
} 

void
TrigConf::JobOptionTable::writeToFile(const std::string& filename) const {
   ofstream of;
   of.open (filename.c_str());
   of << "=============================================" << endl;
   of << "JobOptionTable" << endl;
   of << "   Super MasterTable Id = " << m_SuperMasterTableId << endl;
   of << "   HLT   MasterTable Id = " << m_HltMasterTableId << endl;
   of << "   Number of parameters = " << m_JobOptionVector.size() << endl;
   of << "=============================================" << endl;
   for(unsigned int i=0; i < m_JobOptionVector.size();i++) {
      of << m_JobOptionVector[i] << endl;
   }
   of.close();

}

namespace {
   struct JOLessThan {
      bool operator()(const TrigConf::JobOption& jo1, const TrigConf::JobOption& jo2) const {
         int cmp = jo1.component_alias().compare( jo2.component_alias() );
         if(cmp>0) return false;
         if(cmp<0) return true;
         return jo1.name().compare(jo2.name()) < 0;
      }
   };
}

void
TrigConf::JobOptionTable::sort() {
   std::sort(m_JobOptionVector.begin(), m_JobOptionVector.end(), JOLessThan());
}
