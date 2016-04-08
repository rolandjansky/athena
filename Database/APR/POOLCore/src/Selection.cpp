/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "POOLCore/Selection.h"
#include "POOLCore/Exception.h"
#include <sstream>

std::vector<std::string>& pool::Selection::selectionTypes(){
  static std::vector<std::string> types;
  types.push_back("=");
  types.push_back("!=");
  types.push_back(">");
  types.push_back(">=");
  types.push_back("<");
  types.push_back("<=");
  return types;
}

std::string  pool::Selection::variableNameFromUniqueString(const std::string& uniqueString)
{
  size_t ind = uniqueString.rfind("_");
  return uniqueString.substr(0,ind);
}

std::string pool::Selection::indexVariable(){
  static std::string s_var("pool::ContainerIndex");
  return s_var;
}

pool::Selection::Selection():m_items(),m_data(){
}

pool::Selection::~Selection(){
}

std::string
pool::Selection::uniqueVariableName(const std::string& varName) const {
  std::stringstream uniqueVarName;
  unsigned int i = 0;
  bool notUnique = true;
  while(notUnique){
    bool found = false;
    uniqueVarName.str("");
    uniqueVarName << varName;
    uniqueVarName << "_" << i;
    for(coral::AttributeList::const_iterator iAttr = m_data.begin();
        iAttr!=m_data.end() && !found; ++iAttr){
      if( iAttr->specification().name() == uniqueVarName.str() ) found = true;
    }
    notUnique = found;
    i++;
  }
  return uniqueVarName.str();
}

void pool::Selection::addIndexItem( int startIndex, int endIndex ){
  if(endIndex<startIndex && endIndex>=0) {
    throw pool::Exception("Cannot select with endIndex<startIndex.",std::string(""), std::string(""));
  } else if( startIndex==endIndex && endIndex>=0){
    std::string varName = uniqueVariableName( indexVariable() );
    SelectionItemType selType = pool::EQ;
    m_items.push_back(std::make_pair(varName,selectionTypes()[selType]));
    m_data.extend<int>(varName);
    m_data[varName].data<int>()=startIndex;    
  } else {
    if(startIndex>0){
      std::string varName0 = uniqueVariableName( indexVariable() );
      SelectionItemType firstType = pool::GE;
      m_items.push_back(std::make_pair(varName0,selectionTypes()[firstType]));
      m_data.extend<int>(varName0);
      m_data[varName0].data<int>()=startIndex;
    }
    if(endIndex>0){
      std::string varName1 = uniqueVariableName( indexVariable() );
      SelectionItemType secondType = pool::LE;
      m_items.push_back(std::make_pair(varName1,selectionTypes()[secondType]));
      m_data.extend<int>(varName1);
      m_data[varName1].data<int>()=endIndex;
    }
  }
}

bool
pool::Selection::isEmpty() const {
  return m_items.empty();
}

const std::vector<std::pair<std::string,std::string> >&
pool::Selection::items() const {
  return m_items;
}

const coral::AttributeList&
pool::Selection::data() const {
  return m_data;
}
