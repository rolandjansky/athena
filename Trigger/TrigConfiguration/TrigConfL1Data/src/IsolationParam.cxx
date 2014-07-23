/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/IsolationParam.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>


using namespace std;
using namespace TrigConf;


IsolationParam::IsolationParam( const std::string& thrtype,
                                const int isobit, 
                                int offset,
                                int slope, int mincut,
                                int upperlimit, 
                                int etamin, int etamax, 
                                int priority ) :
   m_thrtype(thrtype),
   m_isDefined(true),
   m_isobit(isobit),
   m_offset(offset),          
   m_slope(slope),
   m_mincut(mincut),
   m_upperlimit(upperlimit),
   m_etamin(etamin),
   m_etamax(etamax),
   m_priority(priority)
{}



void
IsolationParam::print(const std::string& indent, unsigned int /*detail*/) const 
{
   cout << indent << "  IsolationParam:" << endl
        << indent << "    thrtype         : " << m_thrtype     << endl
        << indent << "    isobit          : " << m_isobit      << endl
        << indent << "    offset          : " << m_offset      << endl
        << indent << "    slope           : " << m_slope       << endl
        << indent << "    mincut          : " << m_mincut      << endl
        << indent << "    upperlimit      : " << m_upperlimit  << endl
        << indent << "    etamin          : " << m_etamin      << endl
        << indent << "    etamax          : " << m_etamax      << endl
        << indent << "    priority        : " << m_priority    << endl;
}

void
IsolationParam::writeXML(std::ostream & xmlfile, int indentLevel, int indentWidth) const
{
   if(isDefined())
      indent(xmlfile, indentLevel, indentWidth)
         << "<Parametrization"
         << " isobit=\""     << m_isobit     << "\""
         << " offset=\""     << m_offset     << "\""
         << " slope=\""      << m_slope      << "\""
         << " mincut=\""     << m_mincut     << "\""
         << " upperlimit=\"" << m_upperlimit << "\""
         << " etamin=\""     << m_etamin     << "\""
         << " etamax=\""     << m_etamax     << "\""
         << " priority=\""   << m_priority   << "\""
         << "/>" << endl;
}
