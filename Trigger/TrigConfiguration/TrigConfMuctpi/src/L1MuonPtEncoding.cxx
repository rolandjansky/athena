/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfMuctpi/MioctROIGeometry.h"
#include "TrigConfMuctpi/L1MuonPtEncoding.h"
#include <iostream>

using namespace std;

L1MuonPtEncoding::L1MuonPtEncoding() {
   for(auto & x : m_encoding) {
      x.ptCode = 0;
      x.thresholdValue = 0;
   }
}

void
L1MuonPtEncoding::setCodingInfo(unsigned int idx, unsigned int ptCode, unsigned int thresholdValue ) {
   auto & x = m_encoding[idx];
   x.ptCode = ptCode;
   x.thresholdValue = thresholdValue;
}

void
L1MuonPtEncoding::getCodingInfo(unsigned int idx, unsigned int & ptCode, unsigned int & thresholdValue ) const {
   auto & x = m_encoding[idx];
   ptCode = x.ptCode;
   thresholdValue = x.thresholdValue;
}

void
L1MuonPtEncoding::print(const std::string& indent, unsigned int /*detail*/) const {
   uint idx = 1;
   for(auto & x : m_encoding) {
      cout << indent << "pt " << idx++
           << ", thr " << x.thresholdValue
           << " -> code = " << x.ptCode << endl;
   }
}
   
void
L1MuonPtEncoding::writeXML(std::ostream & xmlfile, int indentLevel, int indentWidth) const {
   uint idx = 1;
   mioctIndent(xmlfile, indentLevel, indentWidth) << "<PtEncoding>" << endl;
   for(auto & x : m_encoding) {
      mioctIndent(xmlfile, indentLevel+1, indentWidth) 
         << "<PtCodeElement pt=\"" << idx++
         << "\" code=\"" << x.ptCode
         << "\" value=\"" << x.thresholdValue
         << "\"/>\n";
   }
   mioctIndent(xmlfile, indentLevel, indentWidth) << "</PtEncoding>" << endl;
}
