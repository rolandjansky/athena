/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfMuctpi/MioctTopoCellGeometry.h"
#include "TrigConfMuctpi/MioctROIGeometry.h"
#include "TrigConfMuctpi/MioctSectorGeometry.h"
#include "TrigConfMuctpi/MioctGeometry.h"
#include <iostream>

using namespace std;

MioctGeometry::MioctGeometry() :
  m_mioctid(0),
  m_slot(0),
  m_Sectors()
{}
  
MioctGeometry::~MioctGeometry() {}
  
void
MioctGeometry::print(const std::string& indent, unsigned int /*detail*/) const {
   cout << indent << "MioctGeometry: " << endl; 
   cout << indent << "\tid: " << m_mioctid    << endl;
   cout << indent << "\tslot:  " << m_slot     << endl;
   for(std::vector<MioctSectorGeometry>::const_iterator isec = m_Sectors.begin(); isec < m_Sectors.end(); ++isec)
     isec->print(indent+indent);
   for(std::vector<MioctSectorGeometry>::const_iterator isec = m_Sectors.begin(); isec < m_Sectors.end(); ++isec)
     isec->print(indent+indent);

}  

void
MioctGeometry::writeXML(std::ostream & xmlfile, int indentLevel, int indentWidth) const {
  mioctIndent(xmlfile, indentLevel, indentWidth) << "<MIOCT "
                                             << "id=\"" << m_mioctid     
						 << "\" slot=\"" <<  m_slot
						 << "\">" << endl;
  std::string xtmp =  "<!-- contains sectors ";
  for(std::vector<MioctSectorGeometry>::const_iterator isec = m_Sectors.begin(); isec < m_Sectors.end(); ++isec)
    {
      xtmp.append(isec->name()); 
      if (isec+1 != m_Sectors.end()) 
	xtmp.append(", ");
    }

  mioctIndent(xmlfile, indentLevel+1, indentWidth) <<xtmp << " -->"<<endl;;
  for(std::vector<MioctSectorGeometry>::const_iterator isec = m_Sectors.begin(); isec < m_Sectors.end(); ++isec)
    isec->writeXML(xmlfile, indentLevel+1, indentWidth);

  mioctIndent(xmlfile, indentLevel+1, indentWidth) << "<Decode>\n";
  for(std::vector<MioctTopoCellGeometry>::const_iterator itopo = m_TopoCells.begin(); itopo < m_TopoCells.end(); ++itopo)
    itopo->writeXML(xmlfile, indentLevel+2, indentWidth);
  mioctIndent(xmlfile, indentLevel+1, indentWidth) << "</Decode>\n";

  mioctIndent(xmlfile, indentLevel, indentWidth) << "</MIOCT>" << endl;

}
