/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __IdDictParser_h__  
#define __IdDictParser_h__  
  
#include "XMLCoreParser/XMLCoreParser.h"  
#include "IdDict/IdDictDefs.h"  
  
class IdDictParser : public XMLCoreParser  
{ 
public:  
  IdDictParser ();  
  IdDictMgr& parse (const std::string& file_name, std::string tag = ""); 
 
  IdDictMgr             m_idd; 
  IdDictDictionary*     m_dictionary; 
  IdDictField*          m_field; 
  IdDictRegion*         m_region; 
  IdDictAltRegions*	m_altregions;
  IdDictSubRegion*      m_subregion; 
  IdDictRegionEntry*    m_regionentry; 
}; 
  
#endif  
