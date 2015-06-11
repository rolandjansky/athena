/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfMuctpi/MioctROIGeometry.h"  // defines mioctIndent
#include "TrigConfMuctpi/MioctTopoCellGeometry.h"
#include <iostream>
#include <iomanip>
using namespace std;

MioctTopoCellGeometry::MioctTopoCellGeometry() :
   m_eta(0),
  m_phi(0),
  m_etacode(0),
  m_phicode(0),
  m_etamin(0),
  m_etamax(0),
  m_phimin(0),
  m_phimax(0)
{}
  
MioctTopoCellGeometry::~MioctTopoCellGeometry() {}
  
void
MioctTopoCellGeometry::print(const std::string& indent, unsigned int /*detail*/) const {
   cout << indent << "MioctTopoCellGeometry: " << endl; 
   //cout << indent << "\tlvl1 master table id:                " << lvl1MasterTableId() << endl; 
   //printNameIdV(indent);
   cout << indent << "\tetacode " << m_etacode << endl;
   cout << indent << "\tphicode " << m_phicode << endl;
   cout << indent << "\teta     " << m_eta     << endl;
   cout << indent << "\tphi     " << m_phi     << endl;
   cout << indent << "\tetamin  " << m_etamin  << endl;
   cout << indent << "\tetamax  " << m_etamax  << endl;
   cout << indent << "\tphimin  " << m_phimin  << endl;
   cout << indent << "\tphimax  " << m_phimax  << endl;
}  

void
MioctTopoCellGeometry::writeXML(std::ostream & xmlfile, int indentLevel, int indentWidth) const {
  mioctIndent(xmlfile, indentLevel, indentWidth) << "<TopoCell "
						 << "etacode=\"0x" << std::hex << m_etacode
						 << "\" phicode=\"0x" << m_phicode <<std::dec
						 << fixed << setprecision(6)
						 << "\" eta=\"" << m_eta     
						 << "\" phi=\"" << m_phi     
						 << "\" etamin=\"" << m_etamin  
						 << "\" etamax=\"" << m_etamax  
						 << "\" phimin=\"" << m_phimin  
						 << "\" phimax=\"" << m_phimax  
						 << "\"/>" << endl;
}
