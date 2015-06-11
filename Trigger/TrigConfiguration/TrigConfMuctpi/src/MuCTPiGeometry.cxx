/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfMuctpi/MioctTopoCellGeometry.h"
#include "TrigConfMuctpi/MioctROIGeometry.h"
#include "TrigConfMuctpi/MioctSectorGeometry.h"
#include "TrigConfMuctpi/MioctGeometry.h"
#include "TrigConfMuctpi/MuCTPiGeometry.h"
#include <iostream>

using namespace std;

MuCTPiGeometry::MuCTPiGeometry() :
  m_Octants()
{}
  
MuCTPiGeometry::~MuCTPiGeometry() {}
  
void
MuCTPiGeometry::print(const std::string& indent, unsigned int /*detail*/) const {
   cout << indent << "MuCTPiGeometry: " << endl; 
   for(std::vector<MioctGeometry>::const_iterator isec = m_Octants.begin(); isec < m_Octants.end(); ++isec)
     isec->print(indent+indent);

}  

void
MuCTPiGeometry::writeXML(std::ostream & xmlfile, int indentLevel, int indentWidth) const {
   mioctIndent(xmlfile, indentLevel, indentWidth) << "<MuCTPiGeometry>" << endl;
   for(std::vector<MioctGeometry>::const_iterator isec = m_Octants.begin(); isec < m_Octants.end(); ++isec)
      isec->writeXML(xmlfile, indentLevel+1, indentWidth);
   m_ptEncoding.writeXML(xmlfile, indentLevel+1, indentWidth);
   mioctIndent(xmlfile, indentLevel, indentWidth) << "</MuCTPiGeometry>" << endl;
}
