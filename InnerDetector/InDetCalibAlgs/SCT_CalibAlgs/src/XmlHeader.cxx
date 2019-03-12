/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file XmlHeader.cxx
 * implementation file for the XmlHeader class
 *
 * @author Shaun Roe shaun.roe@cern.ch
 **/

#include "XmlHeader.h"
#include <ostream>

namespace SCT_CalibAlgs {
  XmlHeader::XmlHeader(std::ostream& output): m_os(output) {
    m_os << "<?xml version=\"1.0\" encoding=\"UTF-8\"?> \n";
  }

  void
  XmlHeader::addStylesheet(const std::string& stylesheetPath) {
    m_os << std::string{"<?xml-stylesheet type=\"text/xsl\" href=\""} + stylesheetPath + std::string{"\"?>\n"};
  }

}
