/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfMuctpi/MioctROIGeometry.h"
#include <iostream>
#include <iomanip>
using namespace std;


std::ostream&
mioctIndent(std::ostream& o, int lvl, int size)
{
   int width = lvl*size;
   if(width==0) return o;
   o << std::setw(lvl*size) << " ";
   return o;
}

MioctROIGeometry::MioctROIGeometry() :
   m_eta(0),
   m_phi(0),
   m_etacode(0),
   m_phicode(0),
   m_etamin(0),
   m_etamax(0),
   m_phimin(0),
   m_phimax(0),
   m_roiid(0)
{}
  
MioctROIGeometry::~MioctROIGeometry() {}
  
void
MioctROIGeometry::print(const std::string& indent, unsigned int /*detail*/) const {
   cout << indent << "MioctROIGeometry: " << endl; 
   //cout << indent << "\tlvl1 master table id:                " << lvl1MasterTableId() << endl; 
   //printNameIdV(indent);
   cout << indent << "\teta     " << m_eta     ;
   cout << indent << "\tphi     " << m_phi     ;
   cout << indent << "\tetacode " << m_etacode ;
   cout << indent << "\tphicode " << m_phicode ;
   cout << indent << "\tetamin  " << m_etamin  ;
   cout << indent << "\tetamax  " << m_etamax  ;
   cout << indent << "\tphimin  " << m_phimin  ;
   cout << indent << "\tphimax  " << m_phimax  ;
   cout << indent << "\troiid   " << m_roiid   << endl;
}  

void
MioctROIGeometry::writeXML(std::ostream & xmlfile, int indentLevel, int indentWidth) const {
  mioctIndent(xmlfile, indentLevel, indentWidth) << "<ROI "
						 << fixed << setprecision(6)
						 << "eta=\"" << m_eta     
						 << "\" phi=\"" << m_phi     
						 << "\" etacode=\"0x" << std::hex << m_etacode
						 << "\" phicode=\"0x" << std::hex << m_phicode 
						 << std::dec << fixed << setprecision(6)
						 << "\" etamin=\"" << std::dec << m_etamin  
						 << "\" etamax=\"" << m_etamax  
						 << "\" phimin=\"" << m_phimin  
						 << "\" phimax=\"" << m_phimax  
						 << "\" roiid=\"" << m_roiid   
						 << "\"/>" << endl;
}
