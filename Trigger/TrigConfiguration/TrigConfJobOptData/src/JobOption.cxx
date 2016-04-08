/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     JobOption.cpp 
//AUTHOR:   J.Haller (CERN)	Johannes.Haller@cern.ch 
//CREATED:  06. Feb. 2006   
//                                                     
//PURPOSE:
//
//
//////////////////////////////////////////////////////////////////////

#include "TrigConfJobOptData/JobOption.h"
#include <cstdlib>

#include <iostream>

using namespace std;

TrigConf::JobOption::JobOption(const string& cp_alias, 
                               const string& name, 
                               const string& type, 
                               const string& op, 
                               const string& value)
   : m_Component_Alias (cp_alias),
     m_Name(name),
     m_Type(type),
     m_Op(op),
     m_Value(value)
{}

TrigConf::JobOption::~JobOption() {}

void
TrigConf::JobOption::print(const std::string& indent, unsigned int /*detail*/) const {
   cout << indent << "JobOption: " << m_Component_Alias << "." << m_Name << " = " << m_Value 
        << " type: " << m_Type << "\t op: " << m_Op << endl;
}  

std::ostream& operator<<(ostream &o, const TrigConf::JobOption& jo) {
   o << "JobOption: " << jo.component_alias() << "." << jo.name() << " = " << jo.value() 
     << " type: " << jo.type() << "\t op: " << jo.op();
   return o;
}  


