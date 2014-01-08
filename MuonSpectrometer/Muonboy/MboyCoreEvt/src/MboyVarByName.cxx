/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MboyVarByName.h"

MboyVarByName::MboyVarByName(){}

MboyVarByName::~MboyVarByName(){}

void MboyVarByName::Reset()
{ 

 m_Name1Set.clear();
 m_Name2Set.clear();
 m_ValueSet.clear();
 m_CommentSet.clear();

}

void MboyVarByName::Define(std::string Name1,std::string Name2,std::string Comment){ 

 double Value = -999999. ;
 Define(Name1,Name2,Comment,Value) ;

}

void MboyVarByName::Define(std::string Name1,std::string Name2,std::string Comment, double Value){ 

 m_Name1Set.push_back(Name1);
 m_Name2Set.push_back(Name2);
 m_CommentSet.push_back(Comment);
 m_ValueSet.push_back(Value);

}

int MboyVarByName::Size() const {return m_Name1Set.size();}

std::string MboyVarByName::GetName1(int Index)   const {return m_Name1Set[Index];}
std::string MboyVarByName::GetName2(int Index)   const {return m_Name2Set[Index];}
std::string MboyVarByName::GetComment(int Index) const {return m_CommentSet[Index];}
double      MboyVarByName::GetValue(int Index)   const {return m_ValueSet[Index];}

void MboyVarByName::SetValue(int Index, double Value){m_ValueSet[Index] = Value;}

