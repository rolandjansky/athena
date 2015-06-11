/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfMuctpi/MioctROIGeometry.h"
#include "TrigConfMuctpi/MioctSectorGeometry.h"
#include <iostream>

using namespace std;

MioctSectorGeometry::MioctSectorGeometry() :
  m_connector(0),
  m_name(""),
  m_ROIs()
{}
  
MioctSectorGeometry::~MioctSectorGeometry() {}
  
void
MioctSectorGeometry::print(const std::string& indent, unsigned int /*detail*/) const {
   cout << indent << "MioctSectorGeometry: " << endl; 
   cout << indent << "\tconnector: " << m_connector << endl;
   cout << indent << "\tname:  " << m_name     << endl;
   for(std::vector<MioctROIGeometry>::const_iterator iROI = m_ROIs.begin(); iROI < m_ROIs.end(); ++iROI)
     iROI->print(indent+indent);

}  

void
MioctSectorGeometry::writeXML(std::ostream & xmlfile, int indentLevel, int indentWidth) const {
  mioctIndent(xmlfile, indentLevel, indentWidth) << "<Sector "
						 << "connector=\"" << m_connector     
						 << "\" name=\"" << m_name     << "\">" << endl;
  mioctIndent(xmlfile, indentLevel+1, indentWidth) << "<!-- contains "<< m_ROIs.size() << " ROIs -->\n";
  mioctIndent(xmlfile, indentLevel+1, indentWidth) << "<!-- mapping from ROI to coding scheme -->\n";
  for(std::vector<MioctROIGeometry>::const_iterator iROI = m_ROIs.begin(); iROI < m_ROIs.end(); ++iROI)
    iROI->writeXML(xmlfile, indentLevel+1, indentWidth);
  mioctIndent(xmlfile, indentLevel, indentWidth) << "</Sector>" << endl;

}
