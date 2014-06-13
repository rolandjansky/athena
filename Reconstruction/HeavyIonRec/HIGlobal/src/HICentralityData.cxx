/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HICentralityData.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "HIGlobal/HICentralityData.h"

#include <iostream>
using std::cout;using std::endl;
using std::string;

HICentralityData::HICentralityData(char *name)
{
  // initialize standard schema (10x10%)
  if( !m_CentralityPercSchema.size() ) 
    for(unsigned short i=0; i<10; i++) m_CentralityPercSchema.push_back(10);
  m_name = name;
}

HICentralityData::HICentralityData(std::vector<unsigned short> centralityPercSchema)
{
  // initialize given schema
  m_CentralityPercSchema = centralityPercSchema;
  m_name = 0;
}

unsigned short HICentralityData::GetBinBySchema(const char *type) const
{
  // Get centrality bin calculated based on distribution of "type" quantity
  // Recalculated to the percentage schema defined in this object

  std::map<string,unsigned short>::const_iterator it;
  it = m_CentralityBin.find(type);
  if(it == m_CentralityBin.end()) return 9999;
  const unsigned short nsch = m_CentralityPercSchema.size();

  if(!nsch) return 9999;

  const unsigned short bin = it->second; 

  unsigned short i=0;
  unsigned short percent = 0;
  std::vector<unsigned short>::const_iterator iter;   
  for( iter = m_CentralityPercSchema.begin(); 
       iter != m_CentralityPercSchema.end(); iter++ ) {
    percent += *iter;
    if( bin < percent ) return i;
    i++;
  }
  return 9999;
}

unsigned short HICentralityData::GetImpactParameterBinBySchema() const
{
  // Get centrality bin calculated based on distribution of 
  // impact parameter truth
  // Recalculated to the percentage schema defined in this object
  return GetBinBySchema("b"); 
}

unsigned short HICentralityData::GetNwoundedBinBySchema() const
{
  // Get centrality bin calculated based on distribution of 
  // number wounded nucleons truth
  // Recalculated to the percentage schema defined in this object
  return GetBinBySchema("nw"); 
}

unsigned short HICentralityData::GetNcollBinBySchema() const
{
  // Get centrality bin calculated based on distribution of 
  // number n-n collsions truth
  // Recalculated to the percentage schema defined in this object
  return GetBinBySchema("ncoll"); 
}

unsigned short HICentralityData::GetCaloCellEnergyBinBySchema() const
{
  // Get centrality bin calculated based on distribution of
  // total energy from calo cells
  // Recalculated to the percentage schema defined in this object
  return GetBinBySchema(" calocell_energy"); 
}

unsigned short HICentralityData::GetNumberOfSiClustersBinBySchema() const
{
  // Get centrality bin calculated based on distribution of 
  // the number of silicon clusters
  // Recalculated to the percentage schema defined in this object
  return GetBinBySchema("number_of_siclusters"); 
}

void HICentralityData::Print() const
{
  cout << "HICentralityData:" << m_name << endl;
  cout << "================" << endl;
  cout << "Centrality Schema:" << endl;
  std::vector<unsigned short>::const_iterator iter1;   
  for( iter1 = m_CentralityPercSchema.begin(); 
       iter1 != m_CentralityPercSchema.end(); iter1++ ) {
    cout << *iter1 << ", " << endl;
  }

  cout << "Centrality Bins" << endl;
  cout << "(in 100 bin schema):" << endl;
  std::map<std::string,unsigned short>::const_iterator iter2;   
  std::map<std::string,float>::const_iterator iter2v;
  for( iter2 = m_CentralityBin.begin(),iter2v = m_CentralityVar.begin(); 
       iter2 != m_CentralityBin.end(); iter2++,iter2v++ ) {
    cout << iter2->first << " value: " << iter2v->second 
	 << " bin: " << iter2->second << endl;
  }

  cout << "Centrality Bins in current schema" << endl;
  for( iter2 = m_CentralityBin.begin(),iter2v = m_CentralityVar.begin(); 
       iter2 != m_CentralityBin.end(); iter2++,iter2v++ ) {
    cout << iter2->first << " value: " << iter2v->second 
	 << " bin: " << GetBinBySchema(iter2->first.c_str()) << endl;
  }
  cout << "---------------------end-----------------------" << endl;
}

